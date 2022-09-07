#include <hdf5.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "H5VL_log.h"

#define N 10

#define CHECK_ERR(A)                                                           \
  {                                                                            \
    if (A < 0) {                                                               \
      printf("Error at line %d: code %d\n", __LINE__, A);                      \
      goto err_out;                                                            \
      nerrs++;                                                                 \
    }                                                                          \
  }

int main(int argc, char **argv) {
  herr_t err = 0;
  int nerrs = 0;
  htri_t ret;
  int i;
  int rank, np;
  const char *file_name = "test.h5";

  // VOL IDs
  hid_t log_vlid = -1, pass_through_vlid = -1, native_vlid = -1;

  // VOL info
  H5VL_pass_through_info_t passthru_info;
  H5VL_log_info_t logvol_info;

  hid_t fid = -1;          // File ID
  hid_t did = -1;          // Dataset ID
  hid_t filespace_id = -1; // File space ID
  hid_t memspace_id = -1;  // Memory space ID
  hid_t faplid = -1;       // File Access Property List
  hid_t dxplid = -1;       // Data transfer Property List

  int buf[N];
  hsize_t dims[2] = {0, N}; // dims[0] will be modified later
  hsize_t start[2], count[2];

  // init MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* Get native vol */
  ret = H5VLis_connector_registered_by_name("native");
  CHECK_ERR(ret);
  native_vlid = H5VLpeek_connector_id_by_name("native");
  CHECK_ERR(native_vlid);

  /* Register Passthrough Vol plugin */
  pass_through_vlid = H5VL_pass_through_register();
  CHECK_ERR(pass_through_vlid);
  // ask Passthru VOL to use Native VOL underneath
  passthru_info.under_vol_id = native_vlid;
  passthru_info.under_vol_info = NULL;

  /* Register LOG VOL plugin */
  log_vlid = H5VLregister_connector(&H5VL_log_g, H5P_DEFAULT);
  // ask LOG VOL to use Passthru VOL underneath
  logvol_info.uvlid = pass_through_vlid;
  logvol_info.under_vol_info = &passthru_info;

  /* Set faplid to use LOG VOL */
  faplid = H5Pcreate(H5P_FILE_ACCESS);
  CHECK_ERR(faplid);
  // MPI and collective metadata is required by LOG VOL
  err = H5Pset_fapl_mpio(faplid, MPI_COMM_WORLD, MPI_INFO_NULL);
  CHECK_ERR(err);
  err = H5Pset_all_coll_metadata_ops(faplid, 1);
  CHECK_ERR(err);
  err = H5Pset_vol(faplid, log_vlid, &logvol_info);
  CHECK_ERR(err);

  /* Create file using LOG VOL. */
  fid = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, faplid);
  CHECK_ERR(fid);

  /* Create 2D (num_process x N) datasets */
  dims[0] = np;
  filespace_id = H5Screate_simple(2, dims, dims);
  CHECK_ERR(filespace_id);
  did = H5Dcreate(fid, "D", H5T_STD_I32LE, filespace_id, H5P_DEFAULT,
                  H5P_DEFAULT, H5P_DEFAULT);
  CHECK_ERR(did);

  /* Prepare data */
  for (i = 0; i < N; i++) {
    buf[i] = rank * 100 + i;
  }
  start[0] = rank;
  start[1] = 0;
  count[0] = 1;
  count[1] = N;

  /* File space setting */
  err = H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, start, NULL, count,
                            NULL);
  CHECK_ERR(err);

  /* Mem space setting */
  memspace_id = H5Screate_simple(1, dims + 1, dims + 1);
  CHECK_ERR(memspace_id);

  /* Request to write data */
  err = H5Dwrite(did, H5T_NATIVE_INT, memspace_id, filespace_id, H5P_DEFAULT,
                 buf);
  CHECK_ERR(err);

  /* Close everything */
err_out:;
  if (memspace_id >= 0)
    H5Sclose(memspace_id);
  if (filespace_id >= 0)
    H5Sclose(filespace_id);
  if (did >= 0)
    H5Dclose(did);
  if (fid >= 0)
    H5Fclose(fid);
  if (faplid >= 0)
    H5Pclose(faplid);
  if (log_vlid >= 00)
    H5VLclose(log_vlid);
  if (pass_through_vlid >= 0)
    H5VLclose(pass_through_vlid);

  MPI_Finalize();

  return (nerrs > 0);
}
