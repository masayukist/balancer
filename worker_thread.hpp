#ifndef __BALANCER__WORKER_THREAD_HPP__
#define __BALANCER__WORKER_THREAD_HPP__

#include <thread>
#include <string>
#include <sstream>
#include <mutex>
#include <cstdlib>
#include <cassert>

//void worker_func( std::string cmd, std::string args, bool* completed );

class WorkerThread
{
  bool completed;
  std::thread* th;
  std::mutex mtx;
public:
  WorkerThread() 
  :completed(false),th(nullptr) 
  {}

  void execute(std::string cmd, std::string args) {
    completed = false;
    auto worker_func = [this]( std::string cmd, std::string args){
        std::stringstream command;
        std::stringstream balancer_cmd;
        command << cmd << " " << args;
        //logger << stamp << "executing -> " << command.str() << endl;
        balancer_cmd << "export BALANCER=1; " << command.str() << std::endl;
        std::system(balancer_cmd.str().c_str());
        set_completed();
    };
    th = new std::thread(worker_func, cmd, args);
  }

  void join() { 
      assert(completed);
      if( th ){
          th->join();
          delete th;
          th=nullptr;
      }
  }

  void set_completed() {
      std::lock_guard<std::mutex> lock(mtx);
      completed=true;
  }

  bool is_completed() { 
      std::lock_guard<std::mutex> lock(mtx);
      return completed; 
  }
};

#endif
