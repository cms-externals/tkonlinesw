#include "Fed9UStripDescription.hh"

#include <iostream>

namespace Fed9U {

  std::ostream& operator<<(std::ostream& os, const Fed9UStripDescription& sd) {
    sd.save(os);
    return os;
  }

  std::istream& operator>>(std::istream& is, Fed9UStripDescription& sd) {
    sd.load(is);
    return is;
  }
  
  void Fed9UStripDescription::load(std::istream& is) {
    is >> _pedestal >> _highThresholdFactor >> _lowThresholdFactor >> _noise >> _disable;
  }

  void Fed9UStripDescription::save(std::ostream& os) const {
    os << _pedestal << '\t' << _highThresholdFactor << '\t' << _lowThresholdFactor << '\t' << _noise << '\t' << _disable;
  }
  
  // <NAC date="24/04/2007"> operator to compare stripDescriptions
  bool operator == (const Fed9UStripDescription& l, const Fed9UStripDescription& r)
  {
    if (l._pedestal != r._pedestal) return false;
    if (l._highThresholdFactor != r._highThresholdFactor) return false;
    if (l._lowThresholdFactor != r._lowThresholdFactor) return false;
    if (l._noise != r._noise) return false;
    if (l._disable != r._disable) return false;
    return true;
  }
  // </NAC>
  
}
