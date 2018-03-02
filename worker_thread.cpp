#include "worker_thread.hpp"

void worker_func( std::string cmd, std::string args, bool* completed )
{
  std::stringstream command;
  std::stringstream balancer_cmd;
  command << cmd << " " << args;
  //logger << stamp << "executing -> " << command.str() << endl;
  balancer_cmd << "export BALANCER=1; " << command.str() << std::endl;
  system(balancer_cmd.str().c_str());
  *completed = true;
}
