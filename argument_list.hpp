#ifndef __BALANCER_ARGUMENT_LIST_HPP__
#define __BALANCER_ARGUMENT_LIST_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class ArgumentsList
{
  std::vector<std::string> arguments;
public:
  ArgumentsList(std::string filename) {
    readfile(filename);
  }
  ArgumentsList(char* filename[], int nfile){
    for( int i = 0; i < nfile ; i++ )
      readfile(filename[i]);
  }
  void readfile(std::string filename) {
    std::ifstream ifs(filename);
    std::string args;
    if( ifs.fail() )
      std::cerr << "File " << filename << " does not exist. Please check!" << std::endl;
    while( getline(ifs, args) ) {
      if( args[0] != '#' )
      arguments.push_back(args);
    }
  }
  int size() { return arguments.size(); }
  std::string& operator[](int i) { return arguments[i]; }
};

#endif
