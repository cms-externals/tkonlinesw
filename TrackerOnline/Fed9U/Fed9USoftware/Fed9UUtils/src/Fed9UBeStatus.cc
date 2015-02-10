#include "Fed9UBeStatus.hh"

namespace Fed9U {

  using std::ostream;
  
  ostream& operator<<(ostream& os, const Fed9UBeStatus& sd) {
    os << sd.getStatusString() << std::endl;
    return os;
  }
  
}
