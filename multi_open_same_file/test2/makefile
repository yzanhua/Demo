# HDF5_DIR points to HDF5 installation dir
# change this
HDF5_DIR=${LOCAL_HOME}/HDF5/1.13.2

TARGET=test

all:
	mpicc ${TARGET}.c -g -o ${TARGET} \
		-I${HDF5_DIR}/include \
		-L${HDF5_DIR}/lib -lhdf5 \
		-DUSEMPI

nompi:
	mpicc ${TARGET}.c -g -o ${TARGET} \
		-I${HDF5_DIR}/include \
		-L${HDF5_DIR}/lib -lhdf5

run:
	LD_LIBRARY_PATH=${HDF5_DIR}/lib:${LD_LIBRARY_PATH} \
	mpirun -n 1 ./${TARGET}

show:
	${HDF5_DIR}/bin/h5dump ${TARGET}.h5

clean:
	rm -rf core.* ${TARGET}.h5 ${TARGET}