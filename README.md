1. [logvol_passthru](./logvol_passthru) contains a demo showing on to stack HDF5 Log Vol on top of other HDF5 Vol, in particular
the [Passthru Vol](https://github.com/HDFGroup/hdf5/blob/develop/src/H5VLpassthru.c).

2. [multi_open_same_file](./multi_open_same_file/) contains several HDF5 test programs testing the behavior of openning the same HDF5 file multiple times and operating on them simultaneously. There's no `H5Fclose` call until after all `H5Dwrite` calls are made, meaning a file is modified while another instance(s) of the file is(are) still opened.