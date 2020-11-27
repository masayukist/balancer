#include "define.hpp"
#include "job_status_map.hpp"
#include "utilities.hpp"
#include <time.h>

#include <iomanip>
#include <ios>

using namespace std;

JobStatusMap::JobStatusMap( int _myrank, int _size )
  : wait_jobs( _myrank, _size, TRUE ),
    exec_jobs( _myrank, _size, FALSE ),
    exit_jobs( _myrank, _size, FALSE ),
    start_time( _myrank, _size, 0 ),
    end_time( _myrank, _size, 0 ),
    size(_size)
{}


void JobStatusMap::output_map(Command* cmd, ArgumentsList* arglist)
{
  ofstream o(string(LOG_DIR)+string("/mpi.job_map.log"));

  o << "wait exec exit job | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ ) {
    long long int t;
    if (exec_jobs[i] == TRUE)
      t = time(NULL) - start_time[i];
    else if (exit_jobs[i] == TRUE)
      t = end_time[i] - start_time[i];
    else
      t = -1;
    o << setw(4) << left << (wait_jobs[i] == TRUE ? "yes" : "no") << " "
      << setw(4) << left << (exec_jobs[i] == TRUE ? "yes" : "no") << " "
      << setw(4) << left <<(exit_jobs[i] == TRUE ? "yes" : "no") << " "
      << setw(40) << left << cmd->get_str() + " " + (*arglist)[i] << " "
      << setw(20) << right << t << " sec." << endl;
  }
  o.close();
}

void
JobStatusMap::setExecuted( int i )
{
  assert (wait_jobs[i] == TRUE);
  wait_jobs[i] = FALSE;
  exec_jobs[i] = TRUE;
  time_t t = time(NULL);
  start_time[i] = t;
  end_time[i] = t;
}

void
JobStatusMap::setExit( int i )
{
  assert (wait_jobs[i] == FALSE);
  assert (exec_jobs[i] == TRUE);
  exec_jobs[i] = FALSE;
  exit_jobs[i] = TRUE;
  time_t t = time(NULL);
  end_time[i] = t;
}
