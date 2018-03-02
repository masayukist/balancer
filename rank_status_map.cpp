#include "rank_status_map.hpp"

using namespace std;

void RankStatusMap::output_map()
{
  ofstream o(string(LOG_DIR)+string("/mpi.rank_map.log"));
  
  o << "active rank | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ )
    o << (ranks_active[i] == TRUE ? "yes    " : "no     ")
      << i << endl;
  o.close();
}
