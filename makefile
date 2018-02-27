# in LX
CXX=mpiicpc
# in grid
#CXX=/usr/lib64/openmpi/bin/mpic++

LDFLAGS=-pthread
CXXFLAGS=-std=c++0x -Wall -O3

balancer:

clean:
	rm -f balancer *~
