#include <inttypes.h>
#include <stdint.h>
#include "Fed9UVersion.inc"

#include <sstream>
#include "Fed9UDevice.hh"

namespace Fed9U {

  std::string getFed9ULibVersion() {
    std::ostringstream s;
    s << getFed9ULibVersionMajor() << "." << getFed9ULibVersionMinor();
    return s.str();
  }

  unsigned getFed9ULibVersionMajor() {
    return Fed9UVersion[0];
  }

  unsigned getFed9ULibVersionMinor() {
    return Fed9UVersion[1];
  }

}


