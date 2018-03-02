#ifndef __BALANCER__MPI_SHARED_MAP_HPP__
#define __BALANCER__MPI_SHARED_MAP_HPP__

#include "mpi.h"
#include <cassert>

#include "define.hpp"

class MPISharedMap
{
protected:
  int map[MAX_ARGS_SIZE];
  int myrank;
  int size;
  MPI_Status stat;
  
public:
  MPISharedMap( int _myrank, int _size, bool initial );
  int& operator[]( int i );
  
  void mpi_bcast_from( int root_rank );
  void mpi_send_to_recv_from( int dest, int source );

  bool is_equiv_map( int* cmap );
  void copy_map( int *cmap );
};

#endif
