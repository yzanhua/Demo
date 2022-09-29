# HDF5 Tests
This folder contains three HDF5 test programs testing the behavior of openning the same HDF5 file multiple times and operating on them simultaneously. There's no `H5Fclose` call until after all `H5Dwrite` calls are made, meaning a file is modified while another instance(s) of the file is(are) still opened.

Using MPI-IO, the programs either give unexpected results or terminate with errors. Note that the test programs are run with 1 mpi process.

Using without MPI-IO, all three test programs finish without problem, and produce "expected" results.

## How to run:
Each test folder contains a makefile and a test program. The procedures to run these programs are the same. Here I use [test1](./test1/) as an example:
```shell
% cd test1
% vi makefile # set correct value for HDF5_DIR

# using MPI-IO
% make clean; make;  # compile
% make run  # run
% make show # call h5dump over the output file

# using withou MPI-IO
% make clean; make nompi;  # compile
% make run  # run
% make show # call h5dump over the output file
```

## Details on Each Test Case
1. [test1](./test1/): mimic the behavior of Log Vol if a same file is opened twice and written simultaneously. The program does the following:

    1. creatre a file with name "test.h5"
    1. create a group with name "_LOG"
    1. close the group and the file.
    1. open test.h5; returned file id is fid1.
    1. open test.h5 again; returned file id is fid2.
    1. open group "_LOG"; return group id is gid1;
    1. open group "_LOG" again; return group id is gid2;
    1. create and write dataset "_LOG/_ld_0" using gid1.
    1. create and write dataset "_LOG/_ld_1" using gid2.
    1. create and write dataset "_LOG/_md_0" using gid1.
    1. create and write dataset "_LOG/_md_1" using gid2.

    Using MPI-IO with 1 mpi process, the datasets "_LOG/_md_0" and  "_LOG/_ld_0" are missing. Using without MPI-IO, all 4 datasets appears without problem.

1. [test2](./test2/): almost the same as [test1](./test1/) but added some `H5Flush` between `H5Dwrite` calls. Using MPI-IO with 1 mpi process, the program exits with segmentation fault. Using without MPI-IO, the program runs without problem and produces expected output file.

1. [test3](./test3/): almost the same as [test2](./test2/) but added more `H5Flush`. Using MPI-IO with 1 mpi process, the program exits with error: `Unable to close file HDF5: infinite loop closing library ...`. Using without MPI-IO, the program runs without problem and produces expected output file.

## Detailed Results
1. <details> <summary>Test1: (Click me)</summary>


    ```shell
    # using MPI-IO
    % make clean; make
    % make run
    % make show

    HDF5 "test.h5" {
    GROUP "/" {
    GROUP "_LOG" {
        DATASET "_ld_1" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
        DATASET "_md_1" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
    }
    }
    }

    # using without MPI-IO
    % make clean; make nompi
    % make run
    % make show
    HDF5 "test.h5" {
    GROUP "/" {
    GROUP "_LOG" {
        DATASET "_ld_0" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
        DATASET "_ld_1" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
        DATASET "_md_0" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
        DATASET "_md_1" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 1, 10 ) / ( 1, 10 ) }
            DATA {
            (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
            }
        }
    }
    }
    }
    ```
    </details>
1. <details> <summary>Test2: (Click me)</summary>

    ```shell
    # using MPI-IO
    % make clean; make
    % make run
    test: H5Oint.c:3026: H5O__free: Assertion `0 == oh->rc' failed.

    ===================================================================================
    =   BAD TERMINATION OF ONE OF YOUR APPLICATION PROCESSES
    =   PID 3789478 RUNNING AT ecp.ece.northwestern.edu
    =   EXIT CODE: 134
    =   CLEANING UP REMAINING PROCESSES
    =   YOU CAN IGNORE THE BELOW CLEANUP MESSAGES
    ===================================================================================
    YOUR APPLICATION TERMINATED WITH THE EXIT STRING: Aborted (signal 6)
    This typically refers to a problem with your application.
    Please see the FAQ page for debugging suggestions
    make: *** [makefile:19: run] Error 134

    # using without MPI-IO does not give any error. Output same as test1-no-mpi-io
    ```
    </details>
1. <details> <summary>Test3: (Click me)</summary>

    ```shell
    # using MPI-IO
    % make clean; make
    % make run
    HDF5-DIAG: Error detected in HDF5 (1.13.2) MPI-process 0:
    #000: H5D.c line 186 in H5Dcreate2(): unable to synchronously create dataset
        major: Dataset
        minor: Unable to create file
    #001: H5D.c line 134 in H5D__create_api_common(): unable to create dataset
        major: Dataset
        minor: Unable to create file
    #002: H5VLcallback.c line 1874 in H5VL_dataset_create(): dataset create failed
        major: Virtual Object Layer
        minor: Unable to create file
    #003: H5VLcallback.c line 1839 in H5VL__dataset_create(): dataset create failed
        major: Virtual Object Layer
        minor: Unable to create file
    #004: H5VLnative_dataset.c line 202 in H5VL__native_dataset_create(): unable to create dataset
        major: Dataset
        minor: Unable to initialize object
    #005: H5Dint.c line 350 in H5D__create_named(): unable to create and link to dataset
        major: Dataset
        minor: Unable to initialize object
    #006: H5Lint.c line 517 in H5L_link_object(): unable to create new link to object
        major: Links
        minor: Unable to initialize object
    #007: H5Lint.c line 760 in H5L__create_real(): can't insert link
        major: Links
        minor: Unable to insert object
    #008: H5Gtraverse.c line 837 in H5G_traverse(): internal path traversal failed
        major: Symbol table
        minor: Object not found
    #009: H5Gtraverse.c line 614 in H5G__traverse_real(): traversal operator failed
        major: Symbol table
        minor: Callback failed
    #010: H5Lint.c line 606 in H5L__link_cb(): unable to create new link for object
        major: Links
        minor: Unable to initialize object
    #011: H5Gobj.c line 565 in H5G_obj_insert(): unable to insert entry into symbol table
        major: Symbol table
        minor: Unable to insert object
    #012: H5Gstab.c line 316 in H5G__stab_insert(): unable to insert the name
        major: Datatype
        minor: Unable to initialize object
    #013: H5Gstab.c line 273 in H5G__stab_insert_real(): unable to insert entry
        major: Symbol table
        minor: Unable to insert object
    #014: H5B.c line 582 in H5B_insert(): unable to insert key
        major: B-Tree node
        minor: Unable to initialize object
    #015: H5B.c line 940 in H5B__insert_helper(): can't insert maximum leaf node
        major: B-Tree node
        minor: Unable to insert object
    #016: H5Gnode.c line 598 in H5G__node_insert(): unable to protect symbol table node
        major: Symbol table
        minor: Unable to load metadata into cache
    #017: H5AC.c line 1396 in H5AC_protect(): H5C_protect() failed
        major: Object cache
        minor: Unable to protect metadata
    #018: H5C.c line 2336 in H5C_protect(): can't load entry
        major: Object cache
        minor: Unable to load metadata into cache
    #019: H5C.c line 7300 in H5C__load_entry(): Can't deserialize image
        major: Object cache
    Error at line 127: code -1
        minor: Unable to load metadata into cache
    #020: H5Gcache.c line 180 in H5G__cache_node_deserialize(): bad symbol table node signature
        major: Symbol table
        minor: Bad value
    Error at line 127: code -1
    HDF5-DIAG: Error detected in HDF5 (1.13.2) MPI-process 0:
    #000: H5D.c line 186 in H5Dcreate2(): unable to synchronously create dataset
        major: Dataset
        minor: Unable to create file
    #001: H5D.c line 134 in H5D__create_api_common(): unable to create dataset
        major: Dataset
        minor: Unable to create file
    #002: H5VLcallback.c line 1874 in H5VL_dataset_create(): dataset create failed
        major: Virtual Object Layer
        minor: Unable to create file
    #003: H5VLcallback.c line 1839 in H5VL__dataset_create(): dataset create failed
        major: Virtual Object Layer
        minor: Unable to create file
    #004: H5VLnative_dataset.c line 202 in H5VL__native_dataset_create(): unable to create dataset
        major: Dataset
        minor: Unable to initialize object
    #005: H5Dint.c line 350 in H5D__create_named(): unable to create and link to dataset
        major: Dataset
        minor: Unable to initialize object
    #006: H5Lint.c line 517 in H5L_link_object(): unable to create new link to object
        major: Links
        minor: Unable to initialize object
    #007: H5Lint.c line 760 in H5L__create_real(): can't insert link
        major: Links
        minor: Unable to insert object
    #008: H5Gtraverse.c line 837 in H5G_traverse(): internal path traversal failed
        major: Symbol table
        minor: Object not found
    #009: H5Gtraverse.c line 614 in H5G__traverse_real(): traversal operator failed
        major: Symbol table
        minor: Callback failed
    #010: H5Lint.c line 606 in H5L__link_cb(): unable to create new link for object
        major: Links
        minor: Unable to initialize object
    #011: H5Gobj.c line 565 in H5G_obj_insert(): unable to insert entry into symbol table
        major: Symbol table
        minor: Unable to insert object
    #012: H5Gstab.c line 316 in H5G__stab_insert(): unable to insert the name
        major: Datatype
        minor: Unable to initialize object
    #013: H5Gstab.c line 273 in H5G__stab_insert_real(): unable to insert entry
        major: Symbol table
        minor: Unable to insert object
    #014: H5B.c line 582 in H5B_insert(): unable to insert key
        major: B-Tree node
        minor: Unable to initialize object
    #015: H5B.c line 940 in H5B__insert_helper(): can't insert maximum leaf node
        major: B-Tree node
        minor: Unable to insert object
    #016: H5Gnode.c line 598 in H5G__node_insert(): unable to protect symbol table node
        major: Symbol table
        minor: Unable to load metadata into cache
    #017: H5AC.c line 1396 in H5AC_protect(): H5C_protect() failed
        major: Object cache
        minor: Unable to protect metadata
    #018: H5C.c line 2244 in H5C_protect(): incorrect cache entry type
        major: Object cache
        minor: Inappropriate type
    HDF5-DIAG: Error detected in HDF5 (1.13.2) MPI-process 0:
    #000: H5F.c line 1061 in H5Fclose(): decrementing file ID failed
        major: File accessibility
        minor: Unable to close file
    #001: H5Iint.c line 1156 in H5I_dec_app_ref(): can't decrement ID ref count
        major: Object ID
        minor: Unable to decrement reference count
    #002: H5Iint.c line 1108 in H5I__dec_app_ref(): can't decrement ID ref count
        major: Object ID
        minor: Unable to decrement reference count
    #003: H5Fint.c line 217 in H5F__close_cb(): unable to close file
        major: File accessibility
        minor: Unable to close file
    #004: H5VLcallback.c line 4164 in H5VL_file_close(): file close failed
        major: Virtual Object Layer
        minor: Unable to close file
    #005: H5VLcallback.c line 4133 in H5VL__file_close(): file close failed
        major: Virtual Object Layer
        minor: Unable to close file
    #006: H5VLnative_file.c line 778 in H5VL__native_file_close(): can't close file
        major: File accessibility
        minor: Unable to decrement reference count
    #007: H5Fint.c line 2315 in H5F__close(): can't close file, there are objects still open
        major: File accessibility
        minor: Unable to close file
    HDF5: infinite loop closing library
        L,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top,T_top

    ===================================================================================
    =   BAD TERMINATION OF ONE OF YOUR APPLICATION PROCESSES
    =   PID 3789753 RUNNING AT ecp.ece.northwestern.edu
    =   EXIT CODE: 134
    =   CLEANING UP REMAINING PROCESSES
    =   YOU CAN IGNORE THE BELOW CLEANUP MESSAGES
    ===================================================================================
    YOUR APPLICATION TERMINATED WITH THE EXIT STRING: Aborted (signal 6)
    This typically refers to a problem with your application.
    Please see the FAQ page for debugging suggestions
    make: *** [makefile:19: run] Error 134

    # using without MPI-IO does not give any error. Output same as test1-no-mpi-io
    ```
    </details>

## Library Version:
1. HDF5 1.13.2
2. MPICH 3.4.2