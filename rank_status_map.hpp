#include "mpi_shared_map.hpp"
#include "define.hpp"
#include "utilities.hpp"

#include <fstream>
#include <string>

class RankStatusMap
{
  MPISharedMap<int> ranks_active;
  MPISharedMap<TIME_T> ranks_duration;

  int size;
  
public:
  RankStatusMap( int _myrank, int _size, int _noderank, int _nodesize, MPI_Comm _nodecomm);
  virtual ~RankStatusMap()
  {
  }

  void mpi_bcast_from( int root_rank ) {
    ranks_active.mpi_bcast_from( root_rank );
    ranks_duration.mpi_bcast_from( root_rank );
  }

  void mpi_send_to_recv_from( int dest, int source ) {
    ranks_active.mpi_send_to_recv_from( dest, source );
    ranks_duration.mpi_send_to_recv_from( dest, source );
  }

  void setRankExit(int i);
  bool isRankExit(int i) { return ranks_active[i] == TRUE ? false : true; };
  
  bool isAllExit() {
    for ( int i = 0; i < size; i++ ) {
      if ( !isRankExit(i) )
        return false;
    }
    return true;
  }

  void output_map();
};
