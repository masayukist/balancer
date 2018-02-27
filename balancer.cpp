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
#include <unistd.h>
#include <cassert>

#include <time.h>

#define MAX_ARGS_SIZE   1024
#define LOG_DIR         "./results/"

#define TRUE  1
#define FALSE 0

using namespace std;

ofstream logger;


class Command
{
  string cmd;
public:
  Command(string filename)
  {
    ifstream ifs(filename);
    if( ifs.fail() )
      cerr << "File " << filename << " does not exist. Please check!" << endl;
    while( getline(ifs, cmd) ) {
      if( cmd[0] == '#' ) continue;
      else                break;
    }
  }
  operator string() { return cmd; }
  string get_str() { return cmd; }
};


class ArgumentsList
{
  vector<string> arguments;
public:
  ArgumentsList(string filename) {
    readfile(filename);
  }
  ArgumentsList(char* filename[], int nfile){
    for( int i = 0; i < nfile ; i++ )
      readfile(filename[i]);
  }
  void readfile(string filename) {
    ifstream ifs(filename);
    string args;
    if( ifs.fail() )
      cerr << "File " << filename << " does not exist. Please check!" << endl;
    while( getline(ifs, args) ) {
      if( args[0] != '#' )
      arguments.push_back(args);
    }
  }
  int size() { return arguments.size(); }
  string& operator[](int i) { return arguments[i]; }
};


string localtimestamp() {
  time_t timer;
  struct tm *ltime;
  timer = time(NULL);
  ltime = localtime(&timer);

  stringstream temp;
  temp << ltime->tm_year + 1900 << "/"
       << setw(2) << setfill('0') << ltime->tm_mon + 1 << "/"
       << setw(2) << setfill('0') << ltime->tm_mday << " "
       << setw(2) << setfill('0') << ltime->tm_hour << ":"
       << setw(2) << setfill('0') << ltime->tm_min << ":"
       << setw(2) << setfill('0') << ltime->tm_sec;
  return temp.str();
}

class MPISharedMap
{
protected:
  int map[MAX_ARGS_SIZE];
  int myrank;
  int size;
  MPI_Status stat;
  
public:
  MPISharedMap( int _myrank, int _size, bool initial )
    : myrank(_myrank), size(_size)
  { for( int i = 0; i < size; i++ ) map[i] = (initial == true ? TRUE : FALSE); }

  int& operator[]( int i ) { return map[i]; } 

  void mpi_bcast_from( int root_rank ) {
    MPI_Bcast( map, size, MPI_INT, root_rank, MPI_COMM_WORLD );
  }

  void mpi_send_to_recv_from( int dest, int source ) {
    assert(dest != source);
    if ( myrank == source )
      MPI_Send( map, size, MPI_INT, dest, 0, MPI_COMM_WORLD );
    else if ( myrank == dest )
      MPI_Recv( map, size, MPI_INT, source, 0, MPI_COMM_WORLD, &stat );
  }

  bool is_equiv_map( int* cmap ) {
    for( int i = 0; i < size; i++ ) if( cmap[i] != map[i] ) return false;
    return true;
  }

  void copy_map( int *cmap ) {
    for( int i = 0; i < size; i++ ) cmap[i] = map[i];
  }

};

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

  void output_map(Command* cmd, ArgumentsList* arglist) {
    ofstream o(string(LOG_DIR)+string("/mpi.job_map.log"));
    o << "wait exec exit job | " << localtimestamp() << endl;
    for ( int i = 0; i < size; i++ )
      o << (wait_jobs[i] == TRUE ? "yes  " : "no   ")
        << (exec_jobs[i] == TRUE ? "yes  " : "no   ")
        << (exit_jobs[i] == TRUE ? "yes  " : "no   ")
        << cmd->get_str() << " " << (*arglist)[i] << endl;
    o.close();
  }
};

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

  void output_map() {
    ofstream o(string(LOG_DIR)+string("/mpi.rank_map.log"));
    o << "active rank | " << localtimestamp() << endl;
    for ( int i = 0; i < size; i++ )
      o << (ranks_active[i] == TRUE ? "yes    " : "no     ")
        << i << endl;
    o.close();
  }
};

ostream& stamp(ostream& ros) {
  ros << localtimestamp() << "  ";
  return ros;
}


void worker_func( string cmd, string args, bool* completed )
{
  stringstream command;
  stringstream balancer_cmd;
  command << cmd << " " << args;
  logger << stamp << "executing -> " << command.str() << endl;
  balancer_cmd << "export BALANCER=1; " << command.str() << endl;
  system(balancer_cmd.str().c_str());
  *completed = true;
}

class WorkerThread
{
    bool completed;
    thread* th;
  public:
    WorkerThread() { completed = false; th = NULL; }
    void execute(string cmd, string args) {
        completed = false;
        th = new thread(worker_func, cmd, args, &completed);
    }
    void join() { if( th ){ th->join(); delete th; } }
    bool is_completed() { return completed; }
};

class WorkerThreadManager
{
  WorkerThread  thread;
  Command*      command;
  ArgumentsList* arglist;
  int current_jobid;
  bool allocated;
  
public:
  WorkerThreadManager(Command* cmd, ArgumentsList* alst){
    command = cmd;
    arglist = alst;
    allocated = false;
  }

  void allocate(int jobid) {
    thread.execute(*command, (*arglist)[jobid]);
    current_jobid = jobid;
    allocated = true;
  }
  bool isAllocated() { return allocated; }
  int getCurrentJobId() {
    assert(allocated);
    return current_jobid;
  }
  void deallocate() { thread.join(); allocated = false; }
  bool isDeallocatable() { return thread.is_completed() && allocated; }
};

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
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  string logfile
    = string(LOG_DIR)+string("/mpi.proc.")
    + to_string((long long)myrank)
    + string(".log");
  
  //logger.open(logfile);

  if( argc < 3 ) {
    if(myrank == 0) usage(argc, argv);
    logger << stamp << "error: please check command line arguments" << endl; 
    return 1;
  }

  Command              cmd( argv[1] );
  ArgumentsList        argslist( &argv[2], argc-2 );
  JobStatusMap         job_status_map( myrank, argslist.size() );
  RankStatusMap        rank_status_map( myrank, numprocs );
  WorkerThreadManager  workman(&cmd, &argslist);
  int                  job_id;

  if(myrank == 0) {
    cout << "*** balancer: command-level MPI parallelization ***" << endl;
    cout << "front-end: [ " << string(cmd) << " ]" << endl;
    for( int i = 2; i < argc ; i++ )
      cout << "list of back-ends: " << argv[i] << endl;
  }

  while(true) {
    // schedule jobs to processes by ascending order of rank
    for( int i = 0; i < numprocs; i++ ) {
      if ( i == myrank ) { // if rank id indicates this process

        if ( workman.isDeallocatable() ){ // if the thread completes the task
          job_id = workman.getCurrentJobId();
          job_status_map.setExit(job_id);
          workman.deallocate();
        }
        
        if ( job_status_map.existWaitJobs() )
          if ( !workman.isAllocated() ) {
            job_id = job_status_map.getNextWaitJob();
            job_status_map.setExecuted(job_id);
            workman.allocate(job_id); 
          }

        // no job and not allocation will cause termination
        if ( !job_status_map.existWaitJobs() )
          if ( !workman.isAllocated() )
            rank_status_map.setExit(myrank);
      }
      // share the status of jobs with the next process,
      // the last process broadcast the status to the all processes
      job_status_map.mpi_bcast_from( i );
      rank_status_map.mpi_bcast_from( i );
    }

    if (myrank == 0) {
      job_status_map.output_map(&cmd, &argslist);
      rank_status_map.output_map();
    }

    // if no more job, breaking loop
    if ( !job_status_map.existWaitJobs() ) {
      //logger << stamp << "the worker thread waits for exit" << endl;
      if ( rank_status_map.isAllExit() )
        break;
    }

    // wait until the next scheduling interval
    sleep(3);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();  
}
