#include "Fed9UStrips.hh"

#include <iostream>

namespace Fed9U {

  using std::istream;
  using std::ostream;
  using std::endl;



  Fed9UStrips::Fed9UStrips() {
//      vector<Fed9UStripDescription> vec1(128,strip);
//      vector< vector<Fed9UStripDescription> > vec2(2,vec1);
//      vector< vector< vector<Fed9UStripDescription> > > vec3(12,vec2);
//      _strips.insert(_strips.begin(), 8, vec3);
  }


  Fed9UStrips::~Fed9UStrips() {
  }
  

  void Fed9UStrips::saveStrips(std::ostream& os) const {
    for(int i = 0; i < STRIPS_PER_FED; i++) {
      os << _strips[i] << endl;
    }
  }
  

  void Fed9UStrips::loadStrips(std::istream& is) {
    for(int i = 0; i < STRIPS_PER_FED; i++) {
      is >> _strips[i];
    }
  }


  void Fed9UStrips::loadDefaultStrips() {
    for (int i = 0; i < STRIPS_PER_FED; i++) {
      _strips[i] = Fed9UStripDescription();
    }
  }
  

  Fed9UStripDescription& Fed9UStrips::getStrip(Fed9UAddress fedStrip) {
    return _strips[fedStrip.getFedStrip()];
  }


  const Fed9UStripDescription& Fed9UStrips::getStrip(Fed9UAddress fedStrip) const {
    return _strips[fedStrip.getFedStrip()];
  }
  

  void Fed9UStrips::setStrip(Fed9UAddress fedStrip, const Fed9UStripDescription& value){
    _strips[fedStrip.getFedStrip()] = value;
  }
  
  
  std::vector<Fed9UStripDescription> Fed9UStrips::getApvStrips(Fed9UAddress fedApv) const {
    //const Fed9UStripDescription* ptr = _strips + (fedChannel.getFedApvNumber() * STRIPS_PER_APV);
    //return vector<Fed9UStripDescription>(ptr, ptr+STRIPS_PER_APV);
    //return _strips[fedChannel.getFEFeUnit()][fedChannel.getFEChannel()][APV];
    std::vector<Fed9UStripDescription> temp(STRIPS_PER_APV);
    for (int i = 0; i < STRIPS_PER_APV; i++) {
      fedApv.setApvStrip(i);
      temp[i] = _strips[fedApv.getFedStrip()];
    }
    return temp;
  }
  
  void Fed9UStrips::setApvStrips(Fed9UAddress fedApv, const std::vector<Fed9UStripDescription>& values) {
    for (int i = 0; i < STRIPS_PER_APV; i++) {
      fedApv.setApvStrip(i);
      _strips[fedApv.getFedStrip()] = values[i];
    }
  }
  
  std::ostream& operator<<(std::ostream& os, const Fed9UStrips& fs) {
    fs.saveStrips(os);
    return os;
  }
  
  std::istream& operator>>(std::istream& is, Fed9UStrips& fs) {
    fs.loadStrips(is);
    return is;
  }
  
  // <NAC date="24/04/2007"> operator to compare strips
  bool operator == (const Fed9UStrips& l, const Fed9UStrips& r)
  {
    Fed9UAddress addr;
    for (int s=0; s<STRIPS_PER_FED; s++) {
      addr.setFedStrip(s);
      if (l.getStrip(addr) != r.getStrip(addr)) return false;
    }
    return true;
  }
  // </NAC>
  
}
