#include "mpi.h"
#include "mpi_shared_map.hpp"
#include "command.hpp"
#include "argument_list.hpp"

#include <time.h>
#include <fstream>
#include <cstdlib>

class JobStatusMap
{
  MPISharedMap<int> wait_jobs;
  MPISharedMap<int> exec_jobs;
  MPISharedMap<int> exit_jobs;
  MPISharedMap<long long int> start_time;
  MPISharedMap<long long int> end_time;
  int size;
  
public:
  JobStatusMap( int _myrank, int _size );
  
  void mpi_bcast_from( int root_rank ) {
    wait_jobs.mpi_bcast_from(root_rank);
    exec_jobs.mpi_bcast_from(root_rank);
    exit_jobs.mpi_bcast_from(root_rank);
    start_time.mpi_bcast_from(root_rank);
    end_time.mpi_bcast_from(root_rank);
  }

  void mpi_send_to_recv_from( int dest, int source ) {
    wait_jobs.mpi_send_to_recv_from( dest, source );
    exec_jobs.mpi_send_to_recv_from( dest, source );
    exit_jobs.mpi_send_to_recv_from( dest, source );
    start_time.mpi_send_to_recv_from( dest, source );
    end_time.mpi_send_to_recv_from( dest, source );
  }
  
  bool existWaitJobs() {
    for( int i = 0; i < size; i++ )
      if( wait_jobs[i] == TRUE )  
        return true;
    return false;
  }

  int getNextWaitJob() {
    assert(existWaitJobs());
    for( int i = 0; i < size; i++ )
      if( wait_jobs[i] == TRUE ) {
        return i;
      }
    std::exit(1);
  }

  void setExecuted( int i );
  void setExit( int i );

  void output_map(Command* cmd, ArgumentsList* arglist);
};
