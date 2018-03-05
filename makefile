# in LX
CC=mpiicc
CXX=mpiicpc
# in grid
#CXX=/usr/lib64/openmpi/bin/mpic++

LDFLAGS=-pthread
CXXFLAGS=-std=c++0x -Wall

balancer: mpi_shared_map.o command.o argument_list.o job_status_map.o rank_status_map.o utilities.o balancer.o worker_thread.o worker_thread_manager.o

clean:
	rm -f balancer *.o *~
