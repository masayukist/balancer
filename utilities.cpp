#include <iomanip>
#include <sstream>
#include <time.h>

#include "utilities.hpp"

using namespace std;

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

ostream& stamp(ostream& ros) {
  ros << localtimestamp() << "  ";
  return ros;
}
