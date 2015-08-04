CXX=mpiicpc
LDFLAGS=-pthread
CXXFLAGS=-std=c++0x -Wall -O3

DATE=$(shell date +%Y%m%d%H%M%S )

balancer:

clean:
	rm -f balancer *~
