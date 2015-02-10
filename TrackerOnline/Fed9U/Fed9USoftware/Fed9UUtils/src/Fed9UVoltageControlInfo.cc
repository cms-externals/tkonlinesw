#include <stdint.h>
#include <inttypes.h>
#include "Fed9UVoltageControlInfo.hh"
#include "ICAssert.hh"

#include <algorithm>
#include <iostream>

namespace Fed9U {

  using std::istream;
  using std::ostream;


  Fed9UVoltageControlInfo::Fed9UVoltageControlInfo(bool standBy,
						   bool resetStatusRegister,
						   float max2Point5Volt,
						   float min2Point5Volt,
						   float max3Point3Volt,
						   float min3Point3Volt,
						   float max5Volt,
						   float min5Volt,
						   float max12Volt,
						   float min12Volt,
						   float maxCoreVoltage,
						   float minCoreVoltage,
						   float maxSupplyVoltage,
						   float minSupplyVoltage,
						   i16 maxExternalTemp,
						   i16 minExternalTemp,
						   i16 maxInternalTemp,
						   i16 minInternalTemp,
						   i16 tempOffset,
						   bool offsetInternalTemp,
						   float actual2Point5Volt,
						   float actual3Point3Volt,
						   float actual5Volt,
						   float actual12Volt,
						   float actualCoreVoltage,
						   float actualSupplyVoltage,
						   i16 actualExternalTemp,
						   i16 actualInternalTemp,
						   u32 manufacturesId,
						   u32 steppingId,
						   u16 statusRegister) :
    Fed9UVoltageControl(standBy, resetStatusRegister, max2Point5Volt, min2Point5Volt, max3Point3Volt, min3Point3Volt,
			   max5Volt, min5Volt, max12Volt, min12Volt, maxCoreVoltage, minCoreVoltage, maxSupplyVoltage, minSupplyVoltage, maxExternalTemp, minExternalTemp,
			   maxInternalTemp, minInternalTemp, tempOffset, offsetInternalTemp),
    actual2Point5Volt_(actual2Point5Volt), actual3Point3Volt_(actual3Point3Volt), actual5Volt_(actual5Volt), actual12Volt_(actual12Volt),
    actualCoreVoltage_(actualCoreVoltage), actualSupplyVoltage_(actualSupplyVoltage),
    actualExternalTemp_(actualExternalTemp), actualInternalTemp_(actualInternalTemp),
    manufacturesId_(manufacturesId), steppingId_(steppingId), statusRegister_(statusRegister)
  {
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActual2Point5Volt(float voltage){
    actual2Point5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActual2Point5Volt() const {
    return actual2Point5Volt_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActual3Point3Volt(float voltage) {
    actual3Point3Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActual3Point3Volt() const {
    return actual3Point3Volt_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActual5Volt(float voltage) {
    actual5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActual5Volt() const {
    return actual5Volt_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActual12Volt(float voltage) {
    actual12Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActual12Volt() const {
    return actual12Volt_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActualCoreVoltage(float voltage) {
    actualCoreVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActualCoreVoltage() const {
    return actualCoreVoltage_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActualSupplyVoltage(float voltage) {
    actualSupplyVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControlInfo::getActualSupplyVoltage() const {
    return actualSupplyVoltage_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActualExternalTemp(i16 temp) {
    actualExternalTemp_ = temp;
    return *this;
  }


  i16 Fed9UVoltageControlInfo::getActualExternalTemp() const {
    return actualExternalTemp_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setActualInternalTemp(i16 temp) {
    actualInternalTemp_ = temp;
    return *this;
  }


  i16 Fed9UVoltageControlInfo::getActualInternalTemp() const {
    return actualInternalTemp_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setManufacturesId(u32 manufacturesId) {
    manufacturesId_ = manufacturesId;
    return *this;
  }


  u32 Fed9UVoltageControlInfo::getManufacturesId() const {
    return manufacturesId_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setSteppingId(u32 steppingId) {
    steppingId_ = steppingId;
    return *this;
  }


  u32 Fed9UVoltageControlInfo::getSteppingId() const {
    return steppingId_;
  }


  Fed9UVoltageControlInfo& Fed9UVoltageControlInfo::setStatusRegister(u16 statusRegister) {
    statusRegister_ = statusRegister;
    return *this;
  }

  u16 Fed9UVoltageControlInfo::getStatusRegister() const {
    return statusRegister_;
  }


  ostream& operator<<(ostream & os, const Fed9UVoltageControlInfo& data) {
    return os << reinterpret_cast<const Fed9UVoltageControl&>(data)
      //The Fed9UVoltageControl operator<< ends with a \n so there is no need to put on here.
	      << data.getActual2Point5Volt()
	      << '\t'
	      << data.getActual3Point3Volt()
	      << '\t'
	      << data.getActual5Volt()
	      << '\t'
	      << data.getActual12Volt()
	      << '\t'
	      << data.getActualCoreVoltage()
	      << '\t'
	      << data.getActualSupplyVoltage()
	      << '\n'
	      << data.getActualExternalTemp()
	      << '\t'
	      << data.getActualInternalTemp()
	      << '\n'
	      << data.getManufacturesId()
	      << '\t'
	      << data.getSteppingId()
	      << '\n'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_2POINT5) >> 0)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_CORE) >> 1)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_3POINT3) >> 2)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_5) >> 3)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_INTERNAL_TEMP) >> 4)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_EXTERNAL_TEMP) >> 5)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_12) >> 6)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_SUPPLY) >> 7)
	      << '\t'
	      << ( (data.getStatusRegister() & FED9U_VOLT_STATUS_NO_EXTERNAL_TEMP) >> 8)
	      << '\n';
  }


  istream& operator>>(istream & is, Fed9UVoltageControlInfo& data) {

    is >> reinterpret_cast<Fed9UVoltageControl&>(data);

    float tempFloat;
    is >> tempFloat;
    data.setActual2Point5Volt(tempFloat);
    is >> tempFloat;
    data.setActual3Point3Volt(tempFloat);
    is >> tempFloat;
    data.setActual5Volt(tempFloat);
    is >> tempFloat;
    data.setActual12Volt(tempFloat);
    is >> tempFloat;
    data.setActualCoreVoltage(tempFloat);
    is >> tempFloat;
    data.setActualSupplyVoltage(tempFloat);

    i16 tempI16;
    is >> tempI16;
    data.setActualExternalTemp(tempI16);
    is >> tempI16;
    data.setActualInternalTemp(tempI16);

    u32 tempU32;
    is >> tempU32;
    data.setManufacturesId(tempU32);
    is >> tempU32;
    data.setSteppingId(tempU32);

    u16 tempU16;
    u16 statusReg = 0;
    is >> tempU16;
    statusReg |= tempU16 << 0;
    is >> tempU16;
    statusReg |= tempU16 << 1;
    is >> tempU16;
    statusReg |= tempU16 << 2;
    is >> tempU16;
    statusReg |= tempU16 << 3;
    is >> tempU16;
    statusReg |= tempU16 << 4;
    is >> tempU16;
    statusReg |= tempU16 << 5;
    is >> tempU16;
    statusReg |= tempU16 << 6;
    is >> tempU16;
    statusReg |= tempU16 << 7;
    is >> tempU16;
    statusReg |= tempU16 << 8;
    data.setStatusRegister(statusReg);

    return is;
  }
  
}// End of Fed9U namespace.
