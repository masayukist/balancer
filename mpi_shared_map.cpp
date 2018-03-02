#include "mpi_shared_map.hpp"

MPISharedMap::MPISharedMap( int _myrank, int _size, bool initial )
  : myrank(_myrank), size(_size)
{
  for( int i = 0; i < size; i++ )
    map[i] = (initial == true ? TRUE : FALSE);
}

int& MPISharedMap::operator[]( int i ) { return map[i]; } 

void MPISharedMap::mpi_bcast_from( int root_rank )
{
  MPI_Bcast( map, size, MPI_INT, root_rank, MPI_COMM_WORLD );
}

void
MPISharedMap::mpi_send_to_recv_from( int dest, int source )
{
  assert(dest != source);
  if ( myrank == source )
    MPI_Send( map, size, MPI_INT, dest, 0, MPI_COMM_WORLD );
  else if ( myrank == dest )
    MPI_Recv( map, size, MPI_INT, source, 0, MPI_COMM_WORLD, &stat );
}

bool
MPISharedMap::is_equiv_map( int* cmap )
{
  for( int i = 0; i < size; i++ ) if( cmap[i] != map[i] ) return false;
  return true;
}

void
MPISharedMap::copy_map( int *cmap )
{
  for( int i = 0; i < size; i++ ) cmap[i] = map[i];
}
