#include "mpi.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <string>
#include <mutex>
#include <vector>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <chrono>

#include "job_status_map.hpp"
#include "rank_status_map.hpp"
#include "worker_thread_manager.hpp"
#include "define.hpp"

using namespace std;

ofstream logger;

void usage(int argc, char* argv[])
{
  cerr << "Usage:" << endl;
  cerr << argv[0] 
       << " FEC_line_file BEC_list_file (BEC_list_files2 BEC_list_file3 ...)" << endl;
}

int main(int argc, char* argv[])
{
  int numprocs;
  int myrank;

  MPI_Init(&argc,&argv);
  {
      MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
      MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

      int nodesize;
      int noderank;
      MPI_Comm nodecomm;

      MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, myrank,
              MPI_INFO_NULL, &nodecomm);

      MPI_Comm_size(nodecomm, &nodesize);
      MPI_Comm_rank(nodecomm, &noderank);


      string logfile
          = string(LOG_DIR)+string("/mpi.proc.")
          + to_string((long long)myrank)
          + string(".log");

      logger.open(logfile);

      if( argc < 3 ) {
          if(myrank == 0) usage(argc, argv);
          logger << stamp << "error: please check command line arguments" << endl; 
          return 1;
      }

      Command              cmd( argv[1] );
      ArgumentsList        argslist( &argv[2], argc-2 );
      JobStatusMap         job_status_map( myrank, argslist.size(), noderank, nodesize, nodecomm);
      RankStatusMap        rank_status_map( myrank, numprocs, noderank, nodesize, nodecomm);
      WorkerThreadManager  workman(&cmd, &argslist);

      if(myrank == 0) {
          cout << "*** balancer: command-level MPI parallelization ***" << endl;
          cout << "front-end: [ " << string(cmd) << " ]" << endl;
          for( int i = 2; i < argc ; i++ )
              cout << "list of back-ends: " << argv[i] << endl;
      }

      MPI_Barrier(MPI_COMM_WORLD);
      assert(sizeof(long long int) == sizeof(time_t));

      while(true) {
          // schedule jobs to processes by ascending order of rank
          for( int i = 0; i < numprocs; i++ ) {
              if ( i == myrank ) { // if rank id indicates this process

                  if ( workman.isDeallocatable() ){ // if the thread completes the task
                      auto job_id = workman.getCurrentJobId();
                      job_status_map.setExit(job_id);
                      workman.deallocate();
                  }

                  if ( job_status_map.existWaitJobs() )
                  {
                      if ( !workman.isAllocated() ) {
                          auto job_id = job_status_map.getNextWaitJob();
                          job_status_map.setExecuted(job_id);
                          workman.allocate(job_id); 
                      }
                  }
                  else// no job and not allocation will cause termination
                  {
                      if ( !workman.isAllocated() ) {
                          rank_status_map.setRankExit( myrank );
                      }
                  }
              }

              if(myrank == 0) {
                  logger << "begin bcast:" << i << endl;
              }

              // share the status of jobs with the next process,
              // the last process broadcast the status to the all processes
              job_status_map.mpi_bcast_from( i );
              rank_status_map.mpi_bcast_from( i );

              if(myrank == 0) {
                  logger << "done bcast:" << i << endl;
              }
          }

          if (myrank == 0) {
              job_status_map.output_map(&cmd, &argslist);
              rank_status_map.output_map();
          }

          // if no more job, breaking loop
          if ( !job_status_map.existWaitJobs() ) {
              logger << stamp << "the worker thread waits for exit" << endl;
              if ( rank_status_map.isAllExit() )
                  break;
          }

          // wait until the next scheduling interval
          std::this_thread::sleep_for(std::chrono::seconds(SCHEDULE_INTERVAL));
      }

      MPI_Barrier(MPI_COMM_WORLD);
  } // mpi init scope
  MPI_Finalize();  
}
