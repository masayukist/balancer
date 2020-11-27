#ifndef __BALANCER__WORKER_THREAD_HPP__
#define __BALANCER__WORKER_THREAD_HPP__

#include <thread>
#include <string>
#include <sstream>

void worker_func( std::string cmd, std::string args, bool* completed );

class WorkerThread
{
  bool completed;
  std::thread* th;
public:
  WorkerThread()
  :completed(false),th(nullptr) {}
  void execute(std::string cmd, std::string args) {
    completed = false;
    th = new std::thread(worker_func, cmd, args, &completed);
  }
  void join() { 
      if( th ){
          th->join();
          delete th;
          th=nullptr;
      }
  }
  bool is_completed() { return completed; }
};

#endif
