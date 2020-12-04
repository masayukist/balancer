#ifndef __BALANCER_ARGUMENT_LIST_HPP__
#define __BALANCER_ARGUMENT_LIST_HPP__

#include <mpi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>

#include "mpi_shared_array.hpp"

class ArgumentsList
{
  template<typename T>
  using sharedArrayPtr = std::shared_ptr<MPISharedArray<T>>;

  const int myrank;
  const int noderank;
  const int nodesize;
  const MPI_Comm nodecomm;
  std::vector<sharedArrayPtr<char>> arguments;
public:
  ArgumentsList(std::string filename, int _myrank, int _noderank, int _nodesize, MPI_Comm _nodecomm)
  : myrank(_myrank), noderank(_noderank), nodesize(_nodesize), nodecomm(_nodecomm)
  {
    readfile(filename);
  }
  ArgumentsList(char* filename[], int nfile, int _myrank, int _noderank, int _nodesize, MPI_Comm _nodecomm)
  : myrank(_myrank), noderank(_noderank), nodesize(_nodesize), nodecomm(_nodecomm)
  {
    for( int i = 0; i < nfile ; i++ )
      readfile(filename[i]);
  }
  void readfile(std::string filename) {
    std::ifstream ifs(filename);
    std::string args;
    if( ifs.fail() )
      std::cerr << "File " << filename << " does not exist. Please check!" << std::endl;
    while( getline(ifs, args) ) {
      if( args[0] != '#' ){
          auto c_str = const_cast<char*>(args.c_str());
          auto ptr = std::make_shared<MPISharedArray<char>>
            (myrank, args.size(), noderank, nodesize, nodecomm, c_str);
          arguments.push_back(ptr);
      }
    }
  }
  std::size_t size() { return arguments.size(); }
  const std::string operator[](int i) const{
      auto str = std::string{arguments[i]->data()};
      return str; 
  }
};

#endif
