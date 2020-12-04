#include "define.hpp"
#include "job_status_map.hpp"
#include "utilities.hpp"

#include <iomanip>
#include <ios>
#include <chrono>

using namespace std;
using namespace std::chrono;

JobStatusMap::JobStatusMap( int _myrank, int _size, int _noderank, int _nodesize, MPI_Comm _nodecomm )
  : myrank(_myrank),
    size(_size),
    noderank(_noderank),
    nodesize(_nodesize),
    nodecomm(_nodecomm),
    wait_jobs( myrank, size, noderank, nodesize, nodecomm, TRUE ),
    exec_jobs( myrank, size, noderank, nodesize, nodecomm, FALSE ),
    exit_jobs( myrank, size, noderank, nodesize, nodecomm, FALSE ),
    start_time( myrank, size, noderank, nodesize, nodecomm, 0 ),
    end_time( myrank, size, noderank, nodesize, nodecomm, 0 )
{}


void JobStatusMap::output_map(Command* cmd, ArgumentsList* arglist)
{
  std::ofstream o(string(LOG_DIR)+string("/mpi.job_map.log"));
  o << "wait exec exit job | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ ) {
    auto now = static_cast<TIME_T>(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
    TIME_T t;
    if (exec_jobs[i] == TRUE)
      t = now - start_time[i];
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
  assert (wait_jobs[i]);
  wait_jobs[i] = FALSE;
  exec_jobs[i] = TRUE;
  exit_jobs[i] = FALSE;
  auto t = static_cast<TIME_T>(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
  start_time[i] = t;
  end_time[i] = t;
}

void
JobStatusMap::setExit( int i )
{
  assert (!wait_jobs[i]);
  assert (exec_jobs[i]);
  wait_jobs[i] = FALSE;
  exec_jobs[i] = FALSE;
  exit_jobs[i] = TRUE;
  auto t = static_cast<TIME_T>(duration_cast<seconds>(system_clock::now().time_since_epoch()).count());
  end_time[i] = t;
}
