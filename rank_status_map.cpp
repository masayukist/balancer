#include "rank_status_map.hpp"

#include <time.h>

#include <iomanip>
#include <ios>

using namespace std;

RankStatusMap::RankStatusMap( int _myrank, int _size )
  : ranks_active( _myrank, _size, TRUE ),
    ranks_duration( _myrank, _size, 0 ),
    size(_size)
{  
  for (int i = 0; i < size; i++) ranks_duration[i] = time(NULL);
}

void
RankStatusMap::setRankExit(int i)
{
  if ( isRankExit(i) )
    return;
  ranks_active[i] = FALSE;
  ranks_duration[i] = time(NULL) - ranks_duration[i];
}

void RankStatusMap::output_map()
{
  ofstream o(string(LOG_DIR)+string("/mpi.rank_map.log"));
  long long int duration;
  
  o << "active rank | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ ) {
    if ( isRankExit(i) ) { duration = ranks_duration[i]; }
    else                 { duration = time(NULL) - ranks_duration[i]; }
    o << setw(6) << (isRankExit(i) ? "no" : "yes")
      << setw(5) << i
      << setw(20) << right << duration << " sec." << endl;
  }
  o.close();
}
