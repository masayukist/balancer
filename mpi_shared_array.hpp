#ifndef __BALANCER__MPI_SHARED_ARRAY_HPP__
#define __BALANCER__MPI_SHARED_ARRAY_HPP__

#include "mpi.h"
#include <cassert>
#include <vector>
#include <iostream>

#include "define.hpp"

template <typename TYPE>
class MPISharedArray
{
protected:
  const int myrank;
  const int _size;
  TYPE *map;
  MPI_Status stat;

  const int noderank;
  const int nodesize;
  const MPI_Comm nodecomm;

  int localsize;

  MPI_Aint winsize;
  int windisp;
  int *winptr;
  MPI_Win wintable;

  void allocate_shared_memory();
  
public:
  MPISharedArray( int _myrank, int __size, int _noderank, int _nodesize, MPI_Comm _nodecomm, TYPE initial );

  MPISharedArray( int _myrank, int __size, int _noderank, int _nodesize, MPI_Comm _nodecomm, TYPE initial[] );

  virtual ~MPISharedArray(){
    MPI_Win_fence(0, wintable);
    MPI_Win_free(&wintable); 
    map = nullptr;
  }

  TYPE& operator[] ( int i ) 
  { 
      //after calling this function, mpi_bcast_from must be called to apply the change to the all process 
      return map[i]; 
  };

  int size() const { return _size; };

  TYPE* data() const { return map; };
  
  void mpi_bcast_from( int root_rank );
  void mpi_send_to_recv_from( int dest, int source );

  bool is_equiv_map( std::vector<TYPE>&& cmap );
  void copy_map( std::vector<TYPE>& cmap );

};

// implementations (not specialized) of menber functions
#include "mpi_shared_array_impl.hpp"

#endif
