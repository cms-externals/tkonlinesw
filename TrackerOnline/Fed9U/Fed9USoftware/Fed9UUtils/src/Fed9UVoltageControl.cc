#include "Fed9UVoltageControl.hh"
#include "ICAssert.hh"

#include <iostream>
#include <iomanip>

namespace Fed9U {

  using std::istream;
  using std::ostream;
  using std::ios;


  Fed9UVoltageControl::Fed9UVoltageControl(bool standBy,
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
					   bool offsetTempSelect) :
    standBy_(standBy), resetStatusRegister_(resetStatusRegister),
    max2Point5Volt_(max2Point5Volt), min2Point5Volt_(min2Point5Volt), max3Point3Volt_(max3Point3Volt), min3Point3Volt_(min3Point3Volt),
    max5Volt_(max5Volt), min5Volt_(min5Volt), max12Volt_(max12Volt), min12Volt_(min12Volt),
    maxCoreVoltage_(maxCoreVoltage), minCoreVoltage_(minCoreVoltage), maxSupplyVoltage_(maxSupplyVoltage), minSupplyVoltage_(minSupplyVoltage),
    maxExternalTemp_(maxExternalTemp), minExternalTemp_(minExternalTemp), maxInternalTemp_(maxInternalTemp), minInternalTemp_(minInternalTemp),
    offset_(tempOffset), internal_(offsetTempSelect) {
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setStandBy(bool standBy) {
    standBy_ = standBy;
    return *this;
  }


  bool Fed9UVoltageControl::getStandBy() const {
    return standBy_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setResetStatusRegister(bool reset) {
    resetStatusRegister_ = reset;
    return *this;
  }


  bool Fed9UVoltageControl::getResetStatusRegister() const {
    return resetStatusRegister_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set2Point5VoltMax(float voltage) {
    float max = 3.320;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 2.5V is out of range.");
    max2Point5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get2Point5VoltMax() const {
    return max2Point5Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set2Point5VoltMin(float voltage) {
    float max = 3.320;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 2.5V is out of range.");
    min2Point5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get2Point5VoltMin() const {
    return min2Point5Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set3Point3VoltMax(float voltage) {
    float max = 4.383;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 3.3V is out of range.");
    max3Point3Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get3Point3VoltMax() const {
    return max3Point3Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set3Point3VoltMin(float voltage) {
    float max = 4.383;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 3.3V is out of range.");
    min3Point3Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get3Point3VoltMin() const {
    return min3Point3Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set5VoltMax(float voltage) {
    float max = 6.640;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 5V is out of range.");
    max5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get5VoltMax() const {
    return max5Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set5VoltMin(float voltage) {
    float max = 6.640;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 5V is out of range.");
    min5Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get5VoltMin() const {
    return min5Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set12VoltMax(float voltage) {
    float max = 15.938;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 12V is out of range.");
    max12Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get12VoltMax() const {
    return max12Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::set12VoltMin(float voltage) {
    float max = 15.938;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 12V is out of range.");
    min12Volt_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::get12VoltMin() const {
    return min12Volt_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setCoreVoltageMax(float voltage) {
    float max = 2.988;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the processor core voltage is out of range.");
    maxCoreVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::getCoreVoltageMax() const {
    return maxCoreVoltage_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setCoreVoltageMin(float voltage) {
    float max = 2.988;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the processor core voltage is out of range.");
    minCoreVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::getCoreVoltageMin() const {
    return minCoreVoltage_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setSupplyVoltageMax(float voltage) {
    float max = 4.383;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the supply 3.3V is out of range.");
    maxSupplyVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::getSupplyVoltageMax() const {
    return maxSupplyVoltage_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setSupplyVoltageMin(float voltage) {
    float max = 4.383;
    float min = 0;
    ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the supply 3.3V is out of range.");
    minSupplyVoltage_ = voltage;
    return *this;
  }


  float Fed9UVoltageControl::getSupplyVoltageMin() const {
    return minSupplyVoltage_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setExternalTempMax(i16 temperature) {
    i16 max = 127;
    i16 min = -128;
    ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The upper limit on the external temperature is out of range.");
    maxExternalTemp_ = temperature;
    return *this;
  }


  i16 Fed9UVoltageControl::getExternalTempMax() const {
    return maxExternalTemp_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setExternalTempMin(i16 temperature) {
    i16 max = 127;
    i16 min = -128;
    ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The lower limit on the external temperature is out of range.");
    minExternalTemp_ = temperature;
    return *this;
  }


  i16 Fed9UVoltageControl::getExternalTempMin() const {
    return minExternalTemp_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setInternalTempMax(i16 temperature) {
    i16 max = 127;
    i16 min = -128;
    ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The upper limit on the internal temperature is out of range.");
    maxInternalTemp_ = temperature;
    return *this;
  }


  i16 Fed9UVoltageControl::getInternalTempMax() const {
    return maxInternalTemp_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setInternalTempMin(i16 temperature) {
    i16 max = 127;
    i16 min = -128;
    ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The lower limit on the internal temperature is out of range.");
    minInternalTemp_ = temperature;
    return *this;
  }


  i16 Fed9UVoltageControl::getInternalTempMin() const {
    return minInternalTemp_;
  }


  Fed9UVoltageControl& Fed9UVoltageControl::setTempOffset(i16 offset, bool internal) {
    i16 max = 127;
    i16 min = -128;
    ICUTILS_VERIFY( (offset >= min) && (offset <= max) )(offset)(min)(max).error().msg("The temperature offset is out of bounds.");
    offset_   = offset;
    internal_ = internal;
    return *this;
  }


  i16 Fed9UVoltageControl::getTempOffset() const {
    return offset_;
  }


  bool Fed9UVoltageControl::getOffsetTempSelect() const {
    return internal_;
  }


  ostream& operator<<(ostream& os, const Fed9UVoltageControl& data) {
    os.setf(ios::fixed, ios::floatfield);
    os.precision(3);
    return os << data.getStandBy()
	      << '\t'
	      << data.getResetStatusRegister()
	      << '\n'
	      << data.get2Point5VoltMax()
	      << '\t'
	      << data.get2Point5VoltMin()
	      << '\n'
	      << data.get3Point3VoltMax()
	      << '\t'
	      << data.get3Point3VoltMin()
	      << '\n'
	      << data.get5VoltMax()
	      << '\t'
	      << data.get5VoltMin()
	      << '\n'
	      << data.get12VoltMax()
	      << '\t'
	      << data.get12VoltMin()
	      << '\n'
	      << data.getCoreVoltageMax()
	      << '\t'
	      << data.getCoreVoltageMin()
	      << '\n'
	      << data.getSupplyVoltageMax()
	      << '\t'
	      << data.getSupplyVoltageMin()
	      << '\n'
	      << data.getExternalTempMax()
	      << '\t'
	      << data.getExternalTempMin()
	      << '\n'
	      << data.getInternalTempMax()
	      << '\t'
	      << data.getInternalTempMin()
	      << '\n'
	      << data.getTempOffset()
	      << '\t'
	      << data.getOffsetTempSelect()
	      << '\n';
  }


  istream& operator>>(istream& is, Fed9UVoltageControl& data) {
    bool tempBool;
    is >> tempBool;
    data.setStandBy(tempBool);
    is >> tempBool;
    data.setResetStatusRegister(tempBool);

    float tempFloat;
    is >> tempFloat;
    data.set2Point5VoltMax(tempFloat);
    is >> tempFloat;
    data.set2Point5VoltMin(tempFloat);
    is >> tempFloat;
    data.set3Point3VoltMax(tempFloat);
    is >> tempFloat;
    data.set3Point3VoltMin(tempFloat);
    is >> tempFloat;
    data.set5VoltMax(tempFloat);
    is >> tempFloat;
    data.set5VoltMin(tempFloat);
    is >> tempFloat;
    data.set12VoltMax(tempFloat);
    is >> tempFloat;
    data.set12VoltMin(tempFloat);
    is >> tempFloat;
    data.setCoreVoltageMax(tempFloat);
    is >> tempFloat;
    data.setCoreVoltageMin(tempFloat);
    is >> tempFloat;
    data.setSupplyVoltageMax(tempFloat);
    is >> tempFloat;
    data.setSupplyVoltageMin(tempFloat);

    i16 tempI16;
    is >> tempI16;
    data.setExternalTempMax(tempI16);
    is >> tempI16;
    data.setExternalTempMin(tempI16);
    is >> tempI16;
    data.setInternalTempMax(tempI16);
    is >> tempI16;
    data.setInternalTempMin(tempI16);
    is >> tempI16;

    is >> tempBool;
    data.setTempOffset(tempI16, tempBool);

    return is;
  }
  
  // <NAC date="24/04/2007"> operator to compare Volatge Monitors
  bool operator == (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r)
  {
    if (l.standBy_ != r.standBy_) return false;
    if (l.resetStatusRegister_ != r.resetStatusRegister_) return false;
    if (l.max2Point5Volt_ != r.max2Point5Volt_) return false;
    if (l.min2Point5Volt_ != r.min2Point5Volt_) return false;
    if (l.max3Point3Volt_ != r.max3Point3Volt_) return false;
    if (l.min3Point3Volt_ != r.min3Point3Volt_) return false;
    if (l.max5Volt_ != r.max5Volt_) return false;
    if (l.min5Volt_ != r.min5Volt_) return false;
    if (l.max12Volt_ != r.max12Volt_) return false;
    if (l.min12Volt_ != r.min12Volt_) return false;
    if (l.maxCoreVoltage_ != r.maxCoreVoltage_) return false;
    if (l.minCoreVoltage_ != r.minCoreVoltage_) return false;
    if (l.maxSupplyVoltage_ != r.maxSupplyVoltage_) return false;
    if (l.minSupplyVoltage_ != r.minSupplyVoltage_) return false;
    if (l.maxExternalTemp_ != r.maxExternalTemp_) return false;
    if (l.minExternalTemp_ != r.minExternalTemp_) return false;
    if (l.maxInternalTemp_ != r.maxInternalTemp_) return false;
    if (l.minInternalTemp_ != r.minInternalTemp_) return false;
    if (l.offset_ != r.offset_) return false;
    if (l.internal_ != r.internal_) return false;
    return true;
  }
  // </NAC>

}// End of Fed9U namespace.
