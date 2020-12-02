#ifndef __BALANCER__MPI_SHARED_MAP_HPP__
#define __BALANCER__MPI_SHARED_MAP_HPP__

#include "mpi.h"
#include <cassert>

#include "define.hpp"

template <typename TYPE>
class MPISharedMap
{
protected:
  TYPE *map;
  int myrank;
  int size;
  MPI_Status stat;
  
public:
  MPISharedMap( int _myrank, int _size, TYPE initial );
  TYPE& operator[]( int i );
  
  void mpi_bcast_from( int root_rank );
  void mpi_send_to_recv_from( int dest, int source );

  bool is_equiv_map( TYPE* cmap );
  void copy_map( TYPE *cmap );
};


template<typename TYPE>
MPISharedMap<TYPE>::MPISharedMap( int _myrank, int _size, TYPE initial )
  : myrank(_myrank), size(_size)
{
  map = new TYPE[size];
  for( int i = 0; i < size; i++ )
    map[i] = (initial == true ? TRUE : FALSE);
}

template<typename TYPE> TYPE&
MPISharedMap<TYPE>::operator[]( int i ) { return map[i]; } 

// template<typename TYPE>
// void
// MPISharedMap<TYPE>::mpi_bcast_from( int root_rank )
// {
//   MPI_Bcast( map, size, TYPE, root_rank, MPI_COMM_WORLD );
// }

// template<typename TYPE>
// void
// MPISharedMap<TYPE>::mpi_send_to_recv_from( int dest, int source )
// {
//   assert(dest != source);
//   if ( myrank == source )
//     MPI_Send( map, size, TYPE, dest, 0, MPI_COMM_WORLD );
//   else if ( myrank == dest )
//     MPI_Recv( map, size, TYPE, source, 0, MPI_COMM_WORLD, &stat );
// }

template<typename TYPE>
bool
MPISharedMap<TYPE>::is_equiv_map( TYPE* cmap )
{
  for( int i = 0; i < size; i++ ) if( cmap[i] != map[i] ) return false;
  return true;
}

template<typename TYPE>
void
MPISharedMap<TYPE>::copy_map( TYPE* cmap )
{
  for( int i = 0; i < size; i++ ) cmap[i] = map[i];
}


#endif
