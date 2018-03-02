#ifndef __BALANCER__WORKER_THREAD_MANAGER_HPP__
#define __BALANCER__WORKER_THREAD_MANAGER_HPP__

#include <cassert>

#include "worker_thread.hpp"
#include "command.hpp"
#include "argument_list.hpp"

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

#endif
