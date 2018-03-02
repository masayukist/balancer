#include "define.hpp"
#include "job_status_map.hpp"
#include "utilities.hpp"

using namespace std;

void JobStatusMap::output_map(Command* cmd, ArgumentsList* arglist)
{
  ofstream o(string(LOG_DIR)+string("/mpi.job_map.log"));

  o << "wait exec exit job | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ )
    o << (wait_jobs[i] == TRUE ? "yes  " : "no   ")
      << (exec_jobs[i] == TRUE ? "yes  " : "no   ")
      << (exit_jobs[i] == TRUE ? "yes  " : "no   ")
      << cmd->get_str() << " " << (*arglist)[i] << endl;
  o.close();
}
