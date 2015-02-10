#include <stdint.h>
#include <inttypes.h>
#include "Fed9ULog.hh"
#include "Fed9ULogTemplate.hh"

#include <sstream>
#include <iomanip>
#include <string>
//#include <ctime>
#include <sys/time.h>
#include <exception>
#include <iostream>
#include <cstdlib>

namespace Fed9U {
  
  using std::string;
  using std::ostringstream;
  using std::endl;
  using std::setw;
  using std::setfill;
  using std::cerr;

  void Fed9ULog::new_unexpected() {
    gFed9ULog.stamp() << "Program terminated due to unexpected exception " << getDateString() << endl;
    //cerr << "Program terminated due to unexpected exception." << endl;
    abort();
  }


  static double getTime() {    timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
  }

  static double time_zero = getTime();

  Fed9ULog::Fed9ULog(const char* filename) {
    std::ifstream temp(filename);
    if (temp.is_open()) {
     temp.close();
     string cmd = string("mv ") + filename + " " + filename + "~";
     system(cmd.c_str());
    }
    
    //TODO find out if there is a method to do this in gcc3.2
    //setbuf(0, 0); // don't buffer the output
    this->setf(std::ios::unitbuf);// does this do it?
    open(filename, std::ios::app);
    (*this) << "\n**************************************************"
       << "\nStart of run " << getDateString() << endl;
    //std::set_unexpected(new_unexpected);
  }

  Fed9ULog::~Fed9ULog() {
    (*this) << "End of run " << getDateString() << endl;
  }
  
  Fed9ULog& Fed9ULog::stamp() {
    (*this) << "Timestamp: " << getTimeString() << endl;
    return *this;
  }

  string Fed9ULog::getTimeString() {
    double t = getTime() - time_zero;
    long tmp = static_cast<long>(t);
    ostringstream s;
    s << tmp << ".";
    tmp = static_cast<long>((t - tmp) * 1000);
    s << setw(3) << setfill('0') << tmp;
    return s.str();
  }

  string Fed9ULog::getDateString() {
    char buf[256];
    time_t t = time(0);
    strftime(buf, sizeof(buf), "on %a %d/%m/%Y at %H:%M:%S\n", localtime(&t));
    return string(buf);
  }

}
