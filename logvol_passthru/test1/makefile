# change this
HDF5_DIR=${ZHOME}/HDF5/1.13.2
LOGVOL_DIR=${ZHOME}/Log-Kai/install

# do not modify below
TARGET=main

C_INCLUDES=-I${HDF5_DIR}/include -I${LOGVOL_DIR}/include
C_LOADS=-L${HDF5_DIR}/lib -lhdf5 -L${LOGVOL_DIR}/lib -lH5VL_log

all:
	mpicxx -o ${TARGET} ${TARGET}.cpp -g -O2 ${C_INCLUDES} ${C_LOADS}

mpi:
	mpicxx -o ${TARGET} ${TARGET}.cpp -g -O2 ${C_INCLUDES} ${C_LOADS}

run:
	LD_LIBRARY_PATH="${HDF5_DIR}/lib:${LOGVOL_DIR}/lib:${LD_LIBRARY_PATH}" \
	mpirun -np 4 ./${TARGET}

convert:
	time -o log1.txt mpirun -np 4 ${ZHOME}/Log-Kai/install/bin/h5lreplay -i test.h5 -o convert.h5 1>/dev/null 2>/dev/null

clean:
	rm -rf ${TARGET} test.h5 core.* *.h5.subfiles
