#ifndef __BALANCER__MPI_SHARED_MAP_HPP__
#define __BALANCER__MPI_SHARED_MAP_HPP__

#include "mpi.h"
#include <cassert>
#include <vector>
#include <iostream>

#include "define.hpp"

template <typename TYPE>
class MPISharedMap
{
protected:
  const int myrank;
  const int size;
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
  
public:
  MPISharedMap( int _myrank, int _size, int _noderank, int _nodesize, MPI_Comm _nodecomm, TYPE initial );
  virtual ~MPISharedMap(){
    MPI_Win_fence(0, wintable);
    MPI_Win_free(&wintable); 
    map = nullptr;
  }

  TYPE& operator[] ( int i );
  
  void mpi_bcast_from( int root_rank );
  void mpi_send_to_recv_from( int dest, int source );

  bool is_equiv_map( std::vector<TYPE>&& cmap );
  void copy_map( std::vector<TYPE>& cmap );

};


template<typename TYPE>
MPISharedMap<TYPE>::MPISharedMap( int _myrank, int _size, int _noderank, 
                                  int _nodesize, MPI_Comm _nodecomm, TYPE initial )
    : myrank(_myrank), size(_size), 
      noderank(_noderank), nodesize(_nodesize), nodecomm(_nodecomm)
{
    TYPE *localmap;

    // only rank 0 on a node actually allocates memory
    if(noderank != 0){
        localsize = 0;
    }else{
        localsize=size;
    }

    /*
    std::cout << "myank:" << myrank << ", noderank:" << noderank << " of " << nodesize
              << " size = " << size << std:: endl;
    */

    MPI_Win_allocate_shared(localsize*sizeof(TYPE), sizeof(TYPE),
                  MPI_INFO_NULL, nodecomm, &localmap, &wintable);

    int *model;
    int flag;
    MPI_Win_get_attr(wintable, MPI_WIN_MODEL, &model, &flag);

    if (1 != flag)
    {
        printf("Attribute MPI_WIN_MODEL not defined\n");
    }
    else
    {
        if (MPI_WIN_UNIFIED == *model)
        {
            if (myrank == 0) printf("Memory model is MPI_WIN_UNIFIED\n");
        }
        else
        {
            if (myrank == 0) printf("Memory model is *not* MPI_WIN_UNIFIED\n");
            std::exit(1);
        }
    }


    map = localmap;

    if (noderank !=0){
        MPI_Win_shared_query(wintable, 0, &winsize, &windisp, &map);
        assert(windisp == sizeof(TYPE));
    }

    // All table pointers should now point to copy on noderank 0

    MPI_Win_fence(0, wintable);

    // Initialize table on rank 0 with appropriate synchronization

    if (noderank == 0){
        for (auto i = TYPE{0}; i < size; ++i){
            map[i] = initial;
        }
    }
    MPI_Win_fence(0, wintable);

}

template<typename TYPE> 
TYPE&
MPISharedMap<TYPE>::operator[]( int i ) { 
    return map[i]; 
} 

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
MPISharedMap<TYPE>::is_equiv_map(std::vector<TYPE>&& cmap )
{
  for( int i = 0; i < size; i++ ) if( cmap[i] != map[i] ) return false;
  return true;
}

template<typename TYPE>
void
MPISharedMap<TYPE>::copy_map(std::vector<TYPE>& cmap )
{
  for( int i = 0; i < size; i++ ) cmap[i] = map[i];
}


#endif
