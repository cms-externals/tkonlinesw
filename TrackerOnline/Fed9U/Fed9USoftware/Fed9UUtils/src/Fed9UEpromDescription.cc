#include "Fed9UEpromDescription.hh"
#include "ICAssert.hh"

#include <iostream>
#include <iomanip>

namespace Fed9U {

  using std::istream;
  using std::ostream;
  using std::hex;
  using std::setfill;
  using std::setw;


  Fed9UEpromDescription::Fed9UEpromDescription(u32 version, const u8* const data):epromVersion_(version){
    //Check for a null pointer.
    if (0 != data) {
      //If the pointer is not null then the data must be copied to the locally held epromData_.
      for (u32 offset = 0; offset < epromSize_; ++offset) {
	epromData_[offset] = data[offset];
      }
    } else {
      //In the case of a null pointer the EPROM data can be left default constructed and the memory can be filled in later.
      for (u32 offset = 0; offset < epromSize_; ++offset) {
	epromData_[offset] = 0;
      }
    }
  }


  Fed9UEpromDescription& Fed9UEpromDescription::setFedSerialNumber(u32 serialNumber) {
    //The FED serial number is spread over two bytes in the EPROM memory.
    //This sets the 8 most significant bits of the offset.
    //the location of these bytes depends on the version of the eprom memory
    // we can get the version we are using from the description
    u32 offset;
    if (epromVersion_ < FED9U_EPROM_VERSION_2) {
      offset = FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET;
      epromData_[offset] = (serialNumber >> 8) & 0xFF;
      epromData_[offset+1] = ( serialNumber & 0xFF );
    }else{
      offset = FED9U_EPROM_INDUSTRY_RESULTS_OFFSET;
      epromData_[offset] = (serialNumber / 10) & 0xFF;
      epromData_[offset+1] = ( (serialNumber % 10) * 10) +  (epromData_[offset+1] % 10);
    }
    return *this;
  }


  u32 Fed9UEpromDescription::getFedSerialNumber() const {
    //The FED serial number occupies two bytes in the EPROM memory.
    //the location of these bytes depends on the version of the eprom memory
    // we can get the version we are using from the description
    u32 offset;
    if (epromVersion_ < FED9U_EPROM_VERSION_2) {
      offset = FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET;
      return ( ( (epromData_[offset] & 0XFF) << 8 ) | ( epromData_[offset+1] & 0xFF  ) );
    } else {
      offset = FED9U_EPROM_INDUSTRY_RESULTS_OFFSET;
      return ( ( (epromData_[offset] & 0XFF) * 10 ) + ( (epromData_[offset+1] & 0xFF ) / 10 ) );      
    }
  }
    

  Fed9UEpromDescription& Fed9UEpromDescription::setSingleByte(u32 offset, u8 data) {
    epromData_[offset] = data;
    return *this;
  }


  u8 Fed9UEpromDescription::getSingleByte(u32 offset) const {
    return epromData_[offset];
  }


  u8& Fed9UEpromDescription::operator[](u32 offset) {
    return epromData_[offset];
  }


  u8 Fed9UEpromDescription::operator[](u32 offset) const {
    return epromData_[offset];
  }


  u32 Fed9UEpromDescription::getEpromSize() const {
    return epromSize_;
  }
  

  u32 Fed9UEpromDescription::getEpromVersion() const {
    return epromVersion_;
  }
  

  Fed9UEpromDescription& Fed9UEpromDescription::setEpromVersion(const u32 version) {
    epromVersion_ = version;
    return *this;
  }


  ostream& operator<<(ostream& os, const Fed9UEpromDescription& data) {

    for (u32 offset = 0; offset < data.getEpromSize(); offset+=4) {
      os << hex
	 << setfill('0') << setw(2) << static_cast<u16>(data[offset])
	 << setfill('0') << setw(2) << static_cast<u16>(data[offset+1])
	 << setfill('0') << setw(2) << static_cast<u16>(data[offset+2])
	 << setfill('0') << setw(2) << static_cast<u16>(data[offset+3])
	 << '\n';
    }
    os << data.getEpromVersion();
    return os;
  }


  istream& operator>>(istream& is, Fed9UEpromDescription& data) {
    u32 tempU32;
    for (u32 offset = 0; offset < data.getEpromSize(); offset+=4) {
      is >> hex >> tempU32;
      data[offset]   = (tempU32 >> 24) & 0xff;
      data[offset+1] = (tempU32 >> 16) & 0xff;
      data[offset+2] = (tempU32 >> 8) & 0xff;
      data[offset+3] = (tempU32 >> 0) & 0xff;
    }
    is >> tempU32;
    data.setEpromVersion(tempU32);
    return is;
  }
  
  // <NAC date="24/04/2007"> operator to compare EpromDescriptions
  bool operator == (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r)
  {
    if (l.epromVersion_ != r.epromVersion_) return false; //required?
    for (int i=0; i<Fed9UEpromDescription::epromSize_; i++) {
      if (l.epromData_[i] != r.epromData_[i]) return false;
    }
    return true;
  }
  // </NAC>
  
}// End of Fed9U namespace.
