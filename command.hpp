#ifndef __BALANCER_COMMAND_HPP__
#define __BALANCER_COMMAND_HPP__

#include <string>
#include <fstream>

class Command
{
  std::string cmd;
public:
  Command(std::string filename);
  operator std::string() { return cmd; }
  std::string get_str() { return cmd; }
};

#endif
