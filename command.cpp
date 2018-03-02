#include <iostream>
#include "command.hpp"

using namespace std;

Command::Command(std::string filename)
{
  std::ifstream ifs(filename);
  if( ifs.fail() )
    cerr << "File " << filename << " does not exist. Please check!" << endl;
  while( getline(ifs, cmd) ) {
    if( cmd[0] == '#' ) continue;
    else                break;
  }
}
