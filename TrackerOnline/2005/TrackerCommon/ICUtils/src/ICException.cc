#include "ICException.hh"
#include "ICAssert.hh"

#include <sstream>
#include <iomanip>

namespace ICUtils {

static const volatile char id[] = "@(#) $Id: ICException.cc,v 1.1 2005/06/14 10:28:40 mirabitl Exp $";
inline void suppress_unused_var_warning() { id; }

const char ICException::_default_text[] = "Internal Error: exception thrown while constructing what() string";

ICException::ICException(const ICAssert & ass) : 
  _file(ass.getFile()), _line(ass.getLine()), _func(ass.getFunc()), _time(getTime()),
  _more(ass.getDesc()), _name("ICException"), _error("Assertion Failed")
{ }

ICException::ICException(const string & name, const ICAssert & ass) : 
  _file(ass.getFile()), _line(ass.getLine()), _func(ass.getFunc()), _time(getTime()),
  _more(ass.getDesc()), _name(name), _error("Assertion Failed")
{ }

const char* ICException::what() const throw() {
  try {
    if (_text.empty()) {
      std::ostringstream str;
      str << _name << ": " << _error;
      if (!_desc.empty()) { str << "\n  Message: " << _desc;                    }
      if (!_file.empty()) { str << "\n  File: " << _file << " Line: " << _line; }
      if (!_func.empty()) { str << "\n  Function: " << _func;                   }
      if (!_time.empty()) { str << "\n  Timestamp: " << _time;                  }
      if (!_more.empty()) { str << "\n" << _more;                               }
      if (!_prev.empty()) { str << "\nPrevious: " << _prev;                     }
      str << std::endl;
      _text = str.str();
    }
    return _text.c_str();
  } catch (...) {
    // an error occurred, so fall through and return default text
  }
  return _default_text;
}

#include <sys/time.h>

ICException::string ICException::getTime() {
  std::stringstream s;
  char buf[32];
  timeval tv;
  gettimeofday(&tv, 0);
  //strftime(buf, sizeof(buf), "%a %d/%m/%Y %H:%M:%S\n", localtime(&t));
  strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&tv.tv_sec));
  s << buf << '.' << std::setw(3) << std::setfill('0') << (tv.tv_usec / 1000);
  return s.str();
}

}
