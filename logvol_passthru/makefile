# change this
HDF5_DIR=${HOME}/HDF5/1.13.2
LOGVOL_DIR=${HOME}/LOG-VOL/install

# do not modify below
TARGET=main

C_INCLUDES=-I${HDF5_DIR}/include -I${LOGVOL_DIR}/include
C_LOADS=-L${HDF5_DIR}/lib -lhdf5 -L${LOGVOL_DIR}/lib -lH5VL_log

all:
	mpicxx -o ${TARGET} ${TARGET}.cpp -g -O2 ${C_INCLUDES} ${C_LOADS}

run:
	LD_LIBRARY_PATH="${HDF5_DIR}/lib:${LOGVOL_DIR}/lib:${LD_LIBRARY_PATH}" \
	mpirun -np 4 ./${TARGET}

clean:
	rm -f ${TARGET} test.h5 core.*
