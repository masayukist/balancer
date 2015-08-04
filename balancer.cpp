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
#include <string.h>
#include <stdlib.h>
#include <cassert>

#include <time.h>

#define MAX_ARGS_SIZE   1024
#define LOG_DIR         "./results/"

#define TRUE  1
#define FALSE 0

using namespace std;

ofstream logger;

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
  MPISharedMap( int _myrank, int _size, int initial )
    : myrank(_myrank), size(_size)
  { for( int i = 0; i < size; i++ ) map[i] = initial; }

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

class WaitJobMap : public MPISharedMap
{
  int prev_map[MAX_ARGS_SIZE];
public:
  WaitJobMap( int _myrank, int _size )
    : MPISharedMap( _myrank, _size, TRUE ) { }
  
  int exists() {
    for( int i = 0; i < size; i++ )
      if( map[i] == TRUE )
        return true;
    return false;
  }

  int next() {
    assert(exists());
    for( int i = 0; i < size; i++ )
      if( map[i] == TRUE ) {
        return i;
      }
    assert(false);
  }
  
  void set_executed( int i ) { map[i] = FALSE; }
  
  void output_map() {
    if( is_equiv_map(prev_map) ) return;
    ofstream o(string(LOG_DIR)+string("/mpi.wait_map.log"));
    o << "# job wait map " << localtimestamp() << endl;
    for ( int i = 0; i < size; i++ )
      o << i << "\t"
      << (map[i] == TRUE ? "waiting" : "executed") << endl;
    o.close();
    copy_map(prev_map);
  }
};

ostream& stamp(ostream& ros) {
  ros << localtimestamp() << "  ";
  return ros;
}

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
  thread* th = NULL;
public:
  WorkerThread() { completed = true; }
  void execute(string cmd, string args) {
    completed = false;
    th = new thread(worker_func, cmd, args, &completed);
  }
  void join() { if( th ){ th->join(); delete th; } }
  bool is_completed() { return completed; }
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
  
  logger.open(logfile);

  if( argc < 3 ) {
    if(myrank == 0) usage(argc, argv);
    logger << stamp << "error: please check command line arguments" << endl; 
    return 1;
  }

  Command              cmd( argv[1] );
  ArgumentsList        argslist( &argv[2], argc-2 );
  WaitJobMap           wait_map( myrank, argslist.size() );
  WorkerThread         worker;
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
        if ( wait_map.exists() ) {
          if ( worker.is_completed() ) {
            worker.join();
            job_id = wait_map.next();
            worker.execute(cmd, argslist[job_id]);
            wait_map.set_executed(job_id);
          }
        }
      }
      // share the status of jobs with the next process,
      // the last process broadcast the status to the all processes
      if      ( i < numprocs-1 ) wait_map.mpi_send_to_recv_from( i+1, i );
      else if ( i = numprocs-1 ) wait_map.mpi_bcast_from( i );
    }

    if (myrank == 0) {
      wait_map.output_map();
    }

    // if no more job, breaking loop
    if ( !wait_map.exists() ) {
      worker.join();
      logger << stamp << "the worker thread exits" << endl;
      MPI_Barrier(MPI_COMM_WORLD);
      break;
    }

    // wait until the next scheduling interval
    sleep(3);
  }
  MPI_Finalize();  
}
