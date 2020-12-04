#include "mpi_shared_array.hpp"

//specialized for long long int
template<>
void
MPISharedArray<long long int>::mpi_bcast_from( int root_rank )
{
    MPI_Win_fence(0,wintable);

    auto ret = MPI_Bcast( map, _size, MPI_LONG_LONG_INT, root_rank, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);

    MPI_Win_fence(0,wintable);
}


template<>
void
MPISharedArray<long long int>::mpi_send_to_recv_from( int dest, int source )
{
  if (dest == source) return;

  assert(dest != source);
  if ( myrank == source ){
    auto ret = MPI_Send( map, _size, MPI_LONG_LONG_INT, dest, 0, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);
  }else if ( myrank == dest ){
    auto ret = MPI_Recv( map, _size, MPI_LONG_LONG_INT, source, 0, MPI_COMM_WORLD, &stat );
    assert(ret == MPI_SUCCESS);
  }
}

//specialized for int
template<>
void
MPISharedArray<int>::mpi_bcast_from( int root_rank )
{
    MPI_Win_fence(0,wintable);

    auto ret = MPI_Bcast( map, _size, MPI_INT, root_rank, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);

    MPI_Win_fence(0,wintable);
}

template<>
void
MPISharedArray<int>::mpi_send_to_recv_from( int dest, int source )
{
  if (dest == source) return;

  assert(dest != source);
  if ( myrank == source ){
    auto ret = MPI_Send( map, _size, MPI_INT, dest, 0, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);
  }else if ( myrank == dest ){
    auto ret = MPI_Recv( map, _size, MPI_INT, source, 0, MPI_COMM_WORLD, &stat );
    assert(ret == MPI_SUCCESS);
  }
}

//specialized for char
template<>
void
MPISharedArray<char>::mpi_bcast_from( int root_rank )
{
    MPI_Win_fence(0,wintable);

    auto ret = MPI_Bcast( map, _size, MPI_CHAR, root_rank, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);

    MPI_Win_fence(0,wintable);
}

template<>
void
MPISharedArray<char>::mpi_send_to_recv_from( int dest, int source )
{
  if (dest == source) return;

  assert(dest != source);
  if ( myrank == source ){
    auto ret = MPI_Send( map, _size, MPI_CHAR, dest, 0, MPI_COMM_WORLD );
    assert(ret == MPI_SUCCESS);
  }else if ( myrank == dest ){
    auto ret = MPI_Recv( map, _size, MPI_CHAR, source, 0, MPI_COMM_WORLD, &stat );
    assert(ret == MPI_SUCCESS);
  }
}
