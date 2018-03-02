#include "mpi_shared_map.hpp"
#include "command.hpp"
#include "argument_list.hpp"

#include <fstream>

class JobStatusMap
{
  MPISharedMap wait_jobs;
  MPISharedMap exec_jobs;
  MPISharedMap exit_jobs;
  int size;
  
public:
  JobStatusMap( int _myrank, int _size )
    : wait_jobs( _myrank, _size, TRUE ),
      exec_jobs( _myrank, _size, FALSE ),
      exit_jobs( _myrank, _size, FALSE ),
      size(_size)
  {}

  void mpi_bcast_from( int root_rank ) {
    wait_jobs.mpi_bcast_from(root_rank);
    exec_jobs.mpi_bcast_from(root_rank);
    exit_jobs.mpi_bcast_from(root_rank);
  }

  void mpi_send_to_recv_from( int dest, int source ) {
    wait_jobs.mpi_send_to_recv_from( dest, source );
    exec_jobs.mpi_send_to_recv_from( dest, source );
    exit_jobs.mpi_send_to_recv_from( dest, source );
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
    assert(false);
  }

  void setExecuted( int i ) {
    if (wait_jobs[i] != TRUE) assert(false);
    wait_jobs[i] = FALSE;
    exec_jobs[i] = TRUE;
  }

  void setExit( int i ) {
    if (wait_jobs[i] != FALSE) assert(false);
    if (exec_jobs[i] != TRUE) assert(false);
    exec_jobs[i] = FALSE;
    exit_jobs[i] = TRUE;
  }

  void output_map(Command* cmd, ArgumentsList* arglist);
};
