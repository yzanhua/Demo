# Log Vol + Passthru Vol
This is a demo showing how to stack HDF5 Log Vol on top of other HDF5 Vol, in particular
the [Passthru Vol](https://github.com/HDFGroup/hdf5/blob/develop/src/H5VLpassthru.c). The demo
program follows the following pattern:
`user application` -> `Log Vol` -> `Passthru Vol` -> `Native Vol`.

# Installations
## HDF5
We require HDF5 version at least 1.13.0 and this demo is tested on version 1.13.2. Example
commands to install HDF5:
```shell
cat command.cfg
#!/bin/sh
VERSION=1.13.2
INSTALL_DIR=$HOME/HDF5/$VERSION

CPPFLAGS="-DENABLE_PASSTHRU_LOGGING $CPPFLAGS" \
../hdf5-$VERSION/configure \
          --prefix=$INSTALL_DIR \
          --silent \
          --enable-parallel \
          --enable-build-mode=debug \
          CC=mpicc
make -j 8 install
```
The Passthru VOL comes together with HDF5. There's no need to install the Passthru VOL seperately.
However, adding `-DENABLE_PASSTHRU_LOGGING` to `CPPFLAGS` makes the the Passthru VOL print
addtional messages to `stdout` every time it is invoked.
## Log Vol
This demo uses a Log Vol version from [this branch](https://github.com/yzanhua/vol-log-based/tree/pass_thru). Installation guide is available [here](https://github.com/yzanhua/vol-log-based/blob/pass_thru/doc/INSTALL.md). Here are the example commands for installtion:
```shell
INSTALL_DIR=${HOME}/LOG-VOL/install
HDF5_DIR=${HOME}/HDF5/1.13.2
./configure --prefix=${INSTALL_DIR} --with-hdf5=${HDF5_DIR} --enable-debug
make -j 8 install
```

# Experiments
## Test Program
In the demo program [main.cpp](./main.cpp), each mpi process with rank `i` makes
one `H5Dwrite` call to write numbers `100*i` to `100*i+9` to the dataset `/D`. 
An example output using 4 mpi processes, **without using any plugin VOL** should be is:
```shell
% h5dump base.h5
HDF5 "base.h5" {
GROUP "/" {
   DATASET "D" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 4, 10 ) / ( 4, 10 ) }
      DATA {
      (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      (1,0): 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
      (2,0): 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
      (3,0): 300, 301, 302, 303, 304, 305, 306, 307, 308, 309
      }
   }
}
}
```
When a `H5Dwrite` is made, it is expected to invoke Log VOL first, then Passthru
VOL and finally the native VOL.
## Results.
We provide a simple [makefile](./makefile) to run the project. 

```shell
# compile and run, output saved to out.txt
% make
% make run > out.txt

# check output file type is LOG VOL
% ${HOME}/LOG-VOL/install/bin/h5ldump -k test.h5
HDF5-LogVOL

# convert LOG file to regular HDF5
% ${HOME}/LOG-VOL/install/bin/h5lreplay -i test.h5 -o convert.h5
Skip log-based VOL data objects _int_att
Reconstructing user dataset D
Skip log-based VOL data objects _ID
Skip log-based VOL data objects _dims
Skip log-based VOL data objects _mdims
Skip log-based VOL data objects _LOG
Skip log-based VOL data objects _LOG/_ld_0
Skip log-based VOL data objects _LOG/_md_0

# content of convert.h5 is as expected
% h5dump convert.h5
HDF5 "convert.h5" {
GROUP "/" {
   DATASET "D" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 4, 10 ) / ( 4, 10 ) }
      DATA {
      (0,0): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      (1,0): 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
      (2,0): 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
      (3,0): 300, 301, 302, 303, 304, 305, 306, 307, 308, 309
      }
   }
}
}

# check for difference
% h5diff basic.h5 convert.h5

# check Passthru VOL is invoked.
% grep -r "PASS THROUGH VOL" out.txt | wc -l
229
```
