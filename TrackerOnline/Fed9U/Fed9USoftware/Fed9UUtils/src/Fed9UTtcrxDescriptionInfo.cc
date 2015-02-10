#include <stdint.h>
#include <inttypes.h>

#include "Fed9UTtcrxDescriptionInfo.hh"
#include "ICAssert.hh"

#include <algorithm>

namespace Fed9U {


  Fed9UTtcrxDescriptionInfo::Fed9UTtcrxDescriptionInfo(Fed9UTtcrxCounterReset counterReset,
						       u16 clockDesOneCoarseDelay,
						       u16 clockDesOneFineDelay,
						       u16 clockDesTwoCoarseDelay,
						       u16 clockDesTwoFineDelay,
						       u16 dllPumpCurrent,
						       u16 pllPumpCurrent,
						       u16 iacId,
						       u16 i2cId,
						       u16 counterOperation,
						       bool disableHammingChecking,
						       bool selectClockTwo,
						       bool disableClockTwo,
						       bool disbaleClockL1Accept,
						       bool disableParrallelBus,
						       bool disableSerialB,
						       bool disableNonDeskewedClock,
						       u16 bunchCount,
						       u32 eventCount,
						       u16 singleBitErrorCount,
						       u16 doubleBitErrorCount,
						       u16 seuErrorCount,
						       u16 masterModeA,
						       u16 masterModeB,
						       u16 testOutputs ,
						       u16 numbClockCycles,
						       u16 configSettings,
						       u16 statusRegister) :
    Fed9UTtcrxDescription(counterReset, clockDesOneCoarseDelay, clockDesOneFineDelay, clockDesTwoCoarseDelay, clockDesTwoFineDelay,
			  dllPumpCurrent, pllPumpCurrent, iacId, i2cId, counterOperation,
			  disableHammingChecking, selectClockTwo, disableClockTwo, disbaleClockL1Accept,
			  disableParrallelBus, disableSerialB, disableNonDeskewedClock),
    _bunchCount(bunchCount), _eventCount(eventCount),
    _singleBitErrorCount(singleBitErrorCount), _doubleBitErrorCount(doubleBitErrorCount), _seuErrorCount(seuErrorCount),
    _masterModeA(masterModeA), _masterModeB(masterModeB), _testOutputs(testOutputs), _numbClockCycles(numbClockCycles),
    _configSettings(configSettings), _statusRegister(statusRegister)
  {
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setBunchCounter(u16 bunchCount) {
    ICUTILS_VERIFY(bunchCount < 4096)(bunchCount).error().msg("The bunch count is invalid.");
    _bunchCount = bunchCount;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getBunchCounter() const {
    return _bunchCount;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setEventCounter(u32 eventCount) {
    ICUTILS_VERIFY(eventCount < 16777216)(eventCount).error().msg("The event count is invalid.");
    _eventCount = eventCount;
    return *this;
  }


  u32 Fed9UTtcrxDescriptionInfo::getEventCounter() const {
    return _eventCount;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setSingleBitErrorCount(u16 singleBitErrorCount) {
    //No need to VERIfY this is the correct value as it is a 16 bit variable and the its maximum value is 16 bits.
    _singleBitErrorCount = singleBitErrorCount;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getSingleBitErrorCount() const {
    return _singleBitErrorCount;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setDoubleBitErrorCount(u16 doubleBitErrorCount) {
    ICUTILS_VERIFY(doubleBitErrorCount < 256)(doubleBitErrorCount).error().msg("The double bit error count is out of range.");
    _doubleBitErrorCount = doubleBitErrorCount;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getDoubleBitErrorCount() const {
    return _doubleBitErrorCount;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setSeuErrorCount(u16 seuErrorCount) {
    ICUTILS_VERIFY(seuErrorCount < 256)(seuErrorCount).error().msg("The single event upset (SEU) error count is out of range.");
    _seuErrorCount = seuErrorCount;
    return *this;
  }



  u16 Fed9UTtcrxDescriptionInfo::getSeuErrorCount() const {
    return _seuErrorCount;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setMasterModeA(u16 masterMode) {
    ICUTILS_VERIFY(masterMode < 4)(masterMode).error().msg("The master mode value is invalid.");
    _masterModeA = masterMode;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getMasterModeA() const {
    return _masterModeA;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setMasterModeB(u16 masterMode) {
    ICUTILS_VERIFY(masterMode < 4)(masterMode).error().msg("The master mode value is invalid.");
    _masterModeB = masterMode;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getMasterModeB() const {
    return _masterModeB;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setTestOutputs(u16 testOutputs) {
    ICUTILS_VERIFY(testOutputs < 8)(testOutputs).error().msg("The test output setting is invalid.");
    testOutputs = _testOutputs;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getTestOutputs() const {
    return _testOutputs;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescriptionInfo::setFreqDetectCount(u16 numbClockCycles) {
    ICUTILS_VERIFY(numbClockCycles < 8)(numbClockCycles).error().msg("The numbClockCycles value is invalid.");
    numbClockCycles = _numbClockCycles;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getFreqDetectCount() const {
    return _numbClockCycles;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescriptionInfo::setConfigSettings(u16 configSettings) {
    ICUTILS_VERIFY(configSettings < 256)(configSettings).error().msg("The configSettings are invalid.");
    configSettings = _configSettings;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getConfigSettings() const {
    return _configSettings;
  }


  Fed9UTtcrxDescriptionInfo& Fed9UTtcrxDescriptionInfo::setStatusRegister(u16 statusRegister) {
    ICUTILS_VERIFY(statusRegister < 16)(statusRegister).error().msg("The status register is an invalid value.");
    _statusRegister = statusRegister;
    return *this;
  }


  u16 Fed9UTtcrxDescriptionInfo::getStatusRegister() const {
    return _statusRegister;
  }


  std::ostream& operator<<(std::ostream & os, const Fed9UTtcrxDescriptionInfo& data) {
    return os << reinterpret_cast<const Fed9UTtcrxDescription&>(data)
	      << data.getBunchCounter()
              << '\t'
	      << data.getEventCounter()
              << '\t'
	      << data.getSingleBitErrorCount()
              << '\t'
	      << data.getDoubleBitErrorCount()
              << '\t'
	      << data.getSeuErrorCount()
              << '\n'
	      << data.getMasterModeA()
	      << '\t'
	      << data.getMasterModeB()
	      << '\n'
	      << data.getTestOutputs()
              << '\t'
	      << data.getFreqDetectCount()
              << '\t'
	      <<   (data.getConfigSettings() & FED9U_TTCRX_CONFIG_SEL_TEST_PD)
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_SEL_INPUT_A) >> 1 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_PLL_RESET)    >> 2 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_DLL_RESET)   >> 3 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR) >> 4 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG)  >> 5 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION) >> 6 )
              << '\t'
	      << ( (data.getConfigSettings() & FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS) >> 7 )
              << '\n'
	      <<   (data.getStatusRegister() & FED9U_TTCRX_STATUS_AUTO_RESET)
              << '\t'
	      << ( (data.getStatusRegister() & FED9U_TTCRX_STATUS_FRAME_SYNCH) >> 1 )
              << '\t'
	      << ( (data.getStatusRegister() & FED9U_TTCRX_STATUS_DLL_READY)   >> 2 )
              << '\t'
	      << ( (data.getStatusRegister() & FED9U_TTCRX_STATUS_PLL_READY)   >> 3 )
	      << '\n';
  }


  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescriptionInfo& data) {

    is >> reinterpret_cast<Fed9UTtcrxDescription&>(data);

    u16 tempU16;
    is >> tempU16;
    data.setBunchCounter(tempU16);
    u32 tempU32;
    is >> tempU32;
    data.setEventCounter(tempU32);
    is >> tempU16;
    data.setSingleBitErrorCount(tempU16);
    is >> tempU16;
    data.setDoubleBitErrorCount(tempU16);
    is >> tempU16;
    data.setSeuErrorCount(tempU16);
    is >> tempU16;
    data.setMasterModeA(tempU16);
    is >> tempU16;
    data.setMasterModeB(tempU16);
    is >> tempU16;
    data.setTestOutputs(tempU16);
    is >> tempU16;
    data.setFreqDetectCount(tempU16);

    u16 configSettings = 0;
    is >> tempU16;
    configSettings |= tempU16;
    is >> tempU16;
    configSettings |= tempU16 << 1;
    is >> tempU16;
    configSettings |= tempU16 << 2;
    is >> tempU16;
    configSettings |= tempU16 << 3;
    is >> tempU16;
    configSettings |= tempU16 << 4;
    is >> tempU16;
    configSettings |= tempU16 << 5;
    is >> tempU16;
    configSettings |= tempU16 << 6;
    is >> tempU16;
    configSettings |= tempU16 << 7;
    data.setConfigSettings(configSettings);

    u16 statusRegister = 0;
    is >> tempU16;
    statusRegister |= tempU16;
    is >> tempU16;
    statusRegister |= tempU16 << 1;
    is >> tempU16;
    statusRegister |= tempU16 << 2;
    is >> tempU16;
    statusRegister |= tempU16 << 3;
    data.setStatusRegister(statusRegister);

    return is;
  }

} //End of Fed9U namespace.
