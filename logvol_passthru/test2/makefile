HDF5=/home/zanhua/Desktop/Lib/HDF5/1.13.2
LOGVOL=/home/zanhua/Desktop/Lib/Log-Vol/install

all:
	mpicc test.cpp -g -o test -DENABLE_PASSTHRU \
	-I${HDF5}/include -I${LOGVOL}/include \
	-L${HDF5}/lib -lhdf5 -lhdf5_hl \
	-L${LOGVOL}/lib -lH5VL_log

mpi:
	mpicc test.cpp -g -o test \
	-I${HDF5}/include -I${LOGVOL}/include \
	-L${HDF5}/lib -lhdf5 -lhdf5_hl \
	-L${LOGVOL}/lib -lH5VL_log

run:
	LD_LIBRARY_PATH=${LOGVOL}/lib:${HDF5}/lib:${LD_LIBRARY_PATH} \
	mpirun -n 1 ./test

kind:
	${LOGVOL}/bin/h5ldump -k test.h5

show:
	${HDF5}/bin/h5dump test.h5

convert:
	${LOGVOL}/bin/h5lreplay -i test.h5 -o convert.h5

clean:
	rm -rf *.h5 core.* test