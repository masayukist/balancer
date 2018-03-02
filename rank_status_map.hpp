#include "mpi_shared_map.hpp"
#include "define.hpp"
#include "utilities.hpp"

#include <fstream>
#include <string>

class RankStatusMap {
  MPISharedMap ranks_active;
  int size;
public:
  RankStatusMap( int _myrank, int _size )
    : ranks_active( _myrank, _size, TRUE ),
      size(_size)
  {}

  void mpi_bcast_from( int root_rank ) {
    ranks_active.mpi_bcast_from(root_rank);
  }

  void mpi_send_to_recv_from( int dest, int source ) {
    ranks_active.mpi_send_to_recv_from( dest, source );
  }

  void setExit(int i) { ranks_active[i] = FALSE; }

  bool isAllExit() {
    for ( int i = 0; i < size; i++ ) {
      if ( ranks_active[i] == TRUE )
        return false;
    }
    return true;
  }

  void output_map();
};
