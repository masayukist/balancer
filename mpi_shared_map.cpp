#include "mpi_shared_map.hpp"

//specialized for long long int

template<>
void
MPISharedMap<long long int>::mpi_bcast_from( int root_rank )
{
  MPI_Bcast( map.data(), size, MPI_LONG_LONG_INT, root_rank, MPI_COMM_WORLD );
}

template<>
void
MPISharedMap<long long int>::mpi_send_to_recv_from( int dest, int source )
{
  assert(dest != source);
  if ( myrank == source )
    MPI_Send( map.data(), size, MPI_LONG_LONG_INT, dest, 0, MPI_COMM_WORLD );
  else if ( myrank == dest )
    MPI_Recv( map.data(), size, MPI_LONG_LONG_INT, source, 0, MPI_COMM_WORLD, &stat );
}

//specialized for int

template<>
void
MPISharedMap<int>::mpi_bcast_from( int root_rank )
{
  MPI_Bcast( map.data(), size, MPI_INT, root_rank, MPI_COMM_WORLD );
}

template<>
void
MPISharedMap<int>::mpi_send_to_recv_from( int dest, int source )
{
  assert(dest != source);
  if ( myrank == source )
    MPI_Send( map.data(), size, MPI_INT, dest, 0, MPI_COMM_WORLD );
  else if ( myrank == dest )
    MPI_Recv( map.data(), size, MPI_INT, source, 0, MPI_COMM_WORLD, &stat );
}
