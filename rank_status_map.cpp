#include "rank_status_map.hpp"

#include <chrono>

#include <iomanip>
#include <ios>

using namespace std;
using namespace std::chrono;

RankStatusMap::RankStatusMap( int _myrank, int _size, int _noderank, int _nodesize, MPI_Comm _nodecomm)
  : ranks_active( _myrank, _size, _noderank, _nodesize, _nodecomm, TRUE ),
    ranks_duration( _myrank, _size, _noderank, _nodesize, _nodecomm, 0 ),
    size(_size)
{  
  for (int i = 0; i < size; i++){
      auto now = static_cast<TIME_T>(duration_cast<seconds>(chrono::system_clock::now().time_since_epoch()).count());
      ranks_duration[i] = now;
  }
}

void
RankStatusMap::setRankExit(int i)
{
  if ( isRankExit(i) )
    return;
  auto now = static_cast<TIME_T>(duration_cast<seconds>(chrono::system_clock::now().time_since_epoch()).count());
  ranks_active[i] = FALSE;
  ranks_duration[i] = now - ranks_duration[i];
}

void RankStatusMap::output_map()
{
  TIME_T duration;
  auto o = ofstream{string(LOG_DIR)+string("/mpi.rank_map.log")};
  
  o << "active rank | " << localtimestamp() << endl;
  for ( int i = 0; i < size; i++ ) {
    if ( isRankExit(i) ) { 
        duration = ranks_duration[i]; 
    }else{
        auto now = static_cast<TIME_T>(duration_cast<seconds>(chrono::system_clock::now().time_since_epoch()).count());
        duration = now - ranks_duration[i]; 
    }
    o << setw(6) << (isRankExit(i) ? "no" : "yes")
      << setw(5) << i
      << setw(20) << right << duration << " sec." << endl;
  }
  o.close();
}
