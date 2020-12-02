#include <iomanip>
#include <sstream>
//#include <ctime>
#include <chrono>

#include "utilities.hpp"

using namespace std;

string localtimestamp() {
  time_t timer = chrono::system_clock::to_time_t(chrono::system_clock::now());
  struct tm *ltime = localtime(&timer);

  stringstream temp;
  temp << ltime->tm_year + 1900 << "/"
       << setw(2) << setfill('0') << ltime->tm_mon + 1 << "/"
       << setw(2) << setfill('0') << ltime->tm_mday << " "
       << setw(2) << setfill('0') << ltime->tm_hour << ":"
       << setw(2) << setfill('0') << ltime->tm_min << ":"
       << setw(2) << setfill('0') << ltime->tm_sec;
  return temp.str();
}

ostream& stamp(ostream& ros) {
  ros << localtimestamp() << "  ";
  return ros;
}
