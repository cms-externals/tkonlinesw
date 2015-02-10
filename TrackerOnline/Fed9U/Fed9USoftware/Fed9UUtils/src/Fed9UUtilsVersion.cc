#include "Fed9UVersion.inc"

#include <sstream>
#include "TypeDefs.hh"

namespace Fed9U {

  std::string getFed9UUtilsVersion() {
    std::ostringstream s;
    s << getFed9UUtilsVersionMajor() << "." << getFed9UUtilsVersionMinor();
    return s.str();
  }

  unsigned getFed9UUtilsVersionMajor() {
    return Fed9UVersion[0];
  }

  unsigned getFed9UUtilsVersionMinor() {
    return Fed9UVersion[1];
  }

}


