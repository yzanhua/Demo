#include "hdf5.h"
#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define N 10
#define CHECK_ERR(A)                                                           \
    {                                                                            \
        if (A < 0) {                                                               \
        printf("Error at line %d: code %d\n", __LINE__, A);                      \
        goto err_out;                                                            \
        nerrs++;                                                                 \
        }                                                                          \
    }

// helper functions; closing HDF5 object
#define GCLOSE(A) { if (A >= 0) H5Gclose(A); A = -1;} // closing group
#define FCLOSE(A) { if (A >= 0) H5Fclose(A); A = -1;} // closing file
#define PCLOSE(A) { if (A >= 0) H5Pclose(A); A = -1;} // closing propertylist
#define SCLOSE(A) { if (A >= 0) H5Sclose(A); A = -1;} // closing space
#define DCLOSE(A) { if (A >= 0) H5Dclose(A); A = -1;} // closing dataset

// This function does the following in order:
// 1. create a dataset with dset_name under the location specified by pid.
// 2. post 1 H5Dwrite request
// 3. close the dataset.
herr_t create_and_write_dset(const char* dset_name, hid_t pid, int rank, int nproc);

int main(int argc, char **argv) {
    herr_t err = 0;
    htri_t ret;
    int rank, nproc, i, nerrs = 0;
    const char *file_name = "test.h5";
    hid_t faplid = -1;
    hid_t fid = -1, fid1 = -1, fid2 = -1;
    hid_t gid = -1, gid1 = -1, gid2 = -1;

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // set up faplid
    faplid = H5Pcreate(H5P_FILE_ACCESS);
    CHECK_ERR(faplid);
#ifdef USEMPI
    err = H5Pset_fapl_mpio(faplid, MPI_COMM_WORLD, MPI_INFO_NULL);
    CHECK_ERR(err);
    err = H5Pset_all_coll_metadata_ops(faplid, 1);
    CHECK_ERR(err);
#endif

    // create file and group; and then close
    fid = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, faplid);
    CHECK_ERR(fid);
    gid = H5Gcreate2(fid, "_LOG", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK_ERR(gid);
    GCLOSE(gid); FCLOSE(fid);  // close every thing

    // reopen the same file twice
    fid1 = H5Fopen(file_name, H5F_ACC_RDWR, faplid);
    CHECK_ERR(fid1);
    fid2 = H5Fopen(file_name, H5F_ACC_RDWR, faplid);
    CHECK_ERR(fid2);

    // reopen the same group twice
    gid1 = H5Gopen2(fid1, "_LOG", H5P_DEFAULT);
    CHECK_ERR(gid1);
    gid2 = H5Gopen2(fid2, "_LOG", H5P_DEFAULT);
    CHECK_ERR(gid2);

    // create and write to dataset "_ld_0" under group "_LOG" using gid1
    create_and_write_dset("_ld_0", gid1, rank, nproc);
    err = H5Fflush(fid1, H5F_SCOPE_GLOBAL);  // flush fid1
    CHECK_ERR(err);
    err = H5Fflush(fid2, H5F_SCOPE_GLOBAL);  // flush fid2
    CHECK_ERR(err);

    // create and write to dataset "_ld_1" under group "_LOG" using gid2
    create_and_write_dset("_ld_1", gid2, rank, nproc);
    err = H5Fflush(fid2, H5F_SCOPE_GLOBAL);  // flush fid2
    CHECK_ERR(err);
    err = H5Fflush(fid1, H5F_SCOPE_GLOBAL);  // flush fid1
    CHECK_ERR(err);

    // create and write to dataset "_md_0" under group "_LOG" using gid1
    create_and_write_dset("_md_0", gid1, rank, nproc);  
    err = H5Fflush(fid1, H5F_SCOPE_GLOBAL);  // flush fid1
    CHECK_ERR(err);
    err = H5Fflush(fid2, H5F_SCOPE_GLOBAL);  // flush fid2
    CHECK_ERR(err);

    // create and write to dataset "_md_1" under group "_LOG" using gid2
    create_and_write_dset("_md_1", gid2, rank, nproc);
    err = H5Fflush(fid2, H5F_SCOPE_GLOBAL);  // flush fid2
    CHECK_ERR(err);
    err = H5Fflush(fid1, H5F_SCOPE_GLOBAL);  // flush fid1
    CHECK_ERR(err);

err_out:;
    GCLOSE(gid); GCLOSE(gid1); GCLOSE(gid2);
    FCLOSE(fid); FCLOSE(fid1); FCLOSE(fid2);
    PCLOSE(faplid);
    return 0;
}

herr_t create_and_write_dset(const char* dset_name, hid_t pid, int rank, int nproc) {
    herr_t err = 0;
    int i, nerrs = 0;

    hid_t fsid = -1, msid = -1; // space id // file and mem
    hid_t did = -1;

    // prepare data
    int buf[N];
    hsize_t dims[2] = {nproc, N};
    hsize_t start[2] = {rank, 0};
    hsize_t count[2] = {1, N};
    for (i = 0; i < N; i++) {
        buf[i] = rank * 100 + i;
    }

    // create 2D (num_proc x N) dataets
    fsid = H5Screate_simple(2, dims, dims);
    CHECK_ERR(fsid);
    did = H5Dcreate2(pid, dset_name, H5T_STD_I32LE, fsid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK_ERR(did);
    
    // space settings
    err = H5Sselect_hyperslab(fsid, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK_ERR(err);
    msid = H5Screate_simple(1, dims+1, dims+1);
    CHECK_ERR(msid);

    // write to dataset
    err = H5Dwrite(did, H5T_NATIVE_INT, msid, fsid, H5P_DEFAULT, buf);
    CHECK_ERR(err);

err_out:;
    DCLOSE(did);
    SCLOSE(fsid); SCLOSE(msid);
}