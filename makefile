CXX=mpiicpc
CXX=mpic++
LDFLAGS=-pthread
CXXFLAGS=-std=c++0x -Wall -O3

balancer:

clean:
	rm -f balancer *~
