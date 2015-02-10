#include "Fed9UTtcrxDescription.hh"
#include "ICAssert.hh"

#include <iostream>

namespace Fed9U {


  Fed9UTtcrxDescription::Fed9UTtcrxDescription(Fed9UTtcrxCounterReset counterReset,
					       u16 l1AcceptCoarseDelay,
					       u16 brcstStrTwoCoarseDelay,
					       u16 clockDesOneFineDelay,
					       u16 clockDesTwoFineDelay,
					       u16 dllPumpCurrent,
					       u16 pllPumpCurrent,
					       u16 iacId,
					       u16 i2cId,
					       u16 counterOperation,
					       bool disableHammingChecking,
					       bool selectClockTwo,
					       bool disableClockTwo,
					       bool disableClockL1Accept,
					       bool disableParrallelBus,
					       bool disableSerialB,
					       bool disableNonDeskewedClock) :
    _counterReset(counterReset),
    _l1AcceptCoarseDelay(l1AcceptCoarseDelay), _brcstStrTwoCoarseDelay(brcstStrTwoCoarseDelay),
    _clockDesOneFineDelay(clockDesOneFineDelay), _clockDesTwoFineDelay(clockDesTwoFineDelay),
    _dllPumpCurrent(dllPumpCurrent), _pllPumpCurrent(pllPumpCurrent),
    _iacId(iacId), _i2cId(i2cId),
    _counterOperation(counterOperation),
    _disableHammingChecking(disableHammingChecking), _selectClockTwo(selectClockTwo),
    _disableClockTwo(disableClockTwo), _disableClockL1Accept(disableClockL1Accept),
    _disableParrallelBus(disableParrallelBus), _disableSerialB(disableSerialB),
    _disableNonDeskewedClock(disableNonDeskewedClock)
  {
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    //_controlRegister.resize(8);
    // </NAC>
    _controlRegister[0] = counterOperation & FED9U_TTCRX_ENABLE_BUNCH_CTR;
    _controlRegister[1] = counterOperation & FED9U_TTCRX_ENABLE_EVENT_CTR;
    _controlRegister[2] = selectClockTwo;
    _controlRegister[3] = disableClockTwo;
    _controlRegister[4] = disableClockL1Accept;
    _controlRegister[5] = disableParrallelBus;
    _controlRegister[6] = disableSerialB;
    _controlRegister[7] = disableNonDeskewedClock;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setCounterReset(const Fed9UTtcrxCounterReset& counterReset) {
    _counterReset = counterReset;
    return *this;
  }


  Fed9UTtcrxCounterReset Fed9UTtcrxDescription::getCounterReset() const {
    return _counterReset;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setL1AcceptDelay(u16 coarseDelay, u16 fineDelay) {
    setL1AcceptCoarseDelay(coarseDelay);
    setClockDesOneFineDelay(fineDelay);
    return *this;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay) {
    setBrcstStrTwoCoarseDelay(coarseDelay);
    //The broadcast strobe two signal and the user broadcast command can either be synchronised to the
    //40MHz deskewed clock one or two depending on the setting in control register element two.
    //If true then it is synchronised to clock two and if false to clock one.
    if ( getDeskewedClock2Selected() && !getDeskewedClock2Disable() ) {
      setClockDesTwoFineDelay(fineDelay);
    } else {
      setClockDesOneFineDelay(fineDelay);
    }
    return *this;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setL1AcceptCoarseDelay(u16 coarseDelay) {
    ICUTILS_VERIFY(coarseDelay < 16)(coarseDelay).error().msg("The L1Accept, broadcast strobe 1 signal, bunch and event counter reset signals and the system broadcast command coarse delay value is out of range.");
    _l1AcceptCoarseDelay = coarseDelay;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getL1AcceptCoarseDelay() const {
    return _l1AcceptCoarseDelay;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setBrcstStrTwoCoarseDelay(u16 coarseDelay) {
    ICUTILS_VERIFY(coarseDelay < 16)(coarseDelay).error().msg("The broadcast strobe two and user broadcast command coarse delay value is out of range.");
    _brcstStrTwoCoarseDelay = coarseDelay;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getBrcstStrTwoCoarseDelay() const {
    return _brcstStrTwoCoarseDelay;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setClockDesOneFineDelay(u16 fineDelay) {
    ICUTILS_VERIFY(fineDelay < 240)(fineDelay).error().msg("The deskewed clock one fine delay value is out of range.");
    _clockDesOneFineDelay = fineDelay;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getClockDesOneFineDelay() const {
    return _clockDesOneFineDelay;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setClockDesTwoFineDelay(u16 fineDelay) {
    ICUTILS_VERIFY(fineDelay < 240)(fineDelay).error().msg("The deskewed clock two fine delay value is out of range.");
    _clockDesTwoFineDelay = fineDelay;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getClockDesTwoFineDelay() const {
    return _clockDesTwoFineDelay;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setDllPumpCurrent(u16 pumpCurrent) {
    ICUTILS_VERIFY(pumpCurrent < 8 && pumpCurrent > 0)(pumpCurrent).error().msg("The delay-lock loop pump current is out of range.");
    _dllPumpCurrent = pumpCurrent;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getDllPumpCurrent() const {
    return _dllPumpCurrent;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setPllPumpCurrent(u16 pumpCurrent) {
    ICUTILS_VERIFY(pumpCurrent < 8 && pumpCurrent > 0)(pumpCurrent).error().msg("The phase-lock loop pump current is out of range.");
    _pllPumpCurrent = pumpCurrent;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getPllPumpCurrent() const {
    return _pllPumpCurrent;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setIacId(u16 id) {
    ICUTILS_VERIFY(id < 16384)(id).error().msg("The IAC ID is out of range.");
    _iacId = id;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getIacId() const {
    return _iacId;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setI2cId(u16 id) {
    ICUTILS_VERIFY(id < 64)(id).error().msg("The IAC ID is out of range.");
    _i2cId = id;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getI2cId() const {
    return _i2cId;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setCounterOperation(u16 counterOperation) {
    ICUTILS_VERIFY(counterOperation < 4)(counterOperation).error().msg("The counterOperation argument is out of range.");
    _counterOperation = counterOperation;
    return *this;
  }


  u16 Fed9UTtcrxDescription::getCounterOperation() const {
    return _counterOperation;
  }



  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setHammingCheckingDisable(bool disableHammingChecking) {
    _disableHammingChecking = disableHammingChecking;
    return *this;
  }


  bool Fed9UTtcrxDescription::getHammingCheckingDisable() const {
    return _disableHammingChecking;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setDeskewedClock2Selected(bool selectClockTwo) {
    _selectClockTwo = selectClockTwo;
    return *this;
  }


  bool Fed9UTtcrxDescription::getDeskewedClock2Selected() const {
    return _selectClockTwo;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setDeskewedClock2Disable(bool disableClockTwo) {
    _disableClockTwo = disableClockTwo;
    return *this;
  }


  bool Fed9UTtcrxDescription::getDeskewedClock2Disable() const {
    return _disableClockTwo;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setClockL1AcceptDisable(bool l1AcceptDisable) {
    _disableClockL1Accept = l1AcceptDisable;
    return *this;
  }


  bool Fed9UTtcrxDescription::getClockL1AcceptDisable() const {
    return _disableClockL1Accept;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setNonDeskewedClockDisable(bool disable) {
    _disableNonDeskewedClock = disable;
    return *this;
  }


  bool Fed9UTtcrxDescription::getNonDeskewedClockDisable() const {
    return _disableNonDeskewedClock;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setParrallelOutputDisable(bool disable) {
    _disableParrallelBus = disable;
    return *this;
  }


  bool Fed9UTtcrxDescription::getParrallelOutputDisable() const {
    return _disableParrallelBus;
  }


  Fed9UTtcrxDescription& Fed9UTtcrxDescription::setSerialBDisable(bool disable) {
    _disableSerialB = disable;
    return *this;
  }


  bool Fed9UTtcrxDescription::getSerialBDisable() const {
    return _disableSerialB;
  }


  std::ostream& operator<<(std::ostream& os, const Fed9UTtcrxDescription& data) {
    return os << data.getCounterReset()._errorCounters
	      << '\t'
	      << data.getCounterReset()._bunchCount
	      << '\t'
	      << data.getCounterReset()._eventCount
	      << '\t'
	      << data.getCounterReset()._status
	      << '\n'
	      << data.getL1AcceptCoarseDelay()
              << '\t'
	      << data.getBrcstStrTwoCoarseDelay()
              << '\t'
	      << data.getClockDesOneFineDelay()
              << '\t'
	      << data.getClockDesTwoFineDelay()
              << '\n'
	      << data.getDllPumpCurrent()
              << '\t'
	      << data.getPllPumpCurrent()
              << '\t'
	      << data.getIacId()
              << '\t'
	      << data.getI2cId()
              << '\t'
	      << data.getHammingCheckingDisable()
              << '\n'
	      << (data.getCounterOperation() & FED9U_TTCRX_ENABLE_BUNCH_CTR)
              << '\t'
	      << (data.getCounterOperation() & FED9U_TTCRX_ENABLE_EVENT_CTR)
              << '\t'
	      << data.getDeskewedClock2Selected()
              << '\t'
	      << data.getDeskewedClock2Disable()
              << '\t'
	      << data.getClockL1AcceptDisable()
              << '\t'
	      << data.getParrallelOutputDisable()
              << '\t'
	      << data.getSerialBDisable()
              << '\t'
	      << data.getNonDeskewedClockDisable()
              << '\n';
  }


  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescription& data) {
    Fed9UTtcrxCounterReset tempReset;
    u16 tempU16;
    bool tempBool;
    std::vector<bool> tempVecBool(8);

    is >> tempReset._errorCounters;
    is >> tempReset._bunchCount;
    is >> tempReset._eventCount;
    is >> tempReset._status;
    data.setCounterReset(tempReset);

    is >> tempU16;
    data.setL1AcceptCoarseDelay(tempU16);
    is >> tempU16;
    data.setBrcstStrTwoCoarseDelay(tempU16);
    is >> tempU16;
    data.setClockDesOneFineDelay(tempU16);
    is >> tempU16;
    data.setClockDesTwoFineDelay(tempU16);
    is >> tempU16;
    data.setDllPumpCurrent(tempU16);
    is >> tempU16;
    data.setPllPumpCurrent(tempU16);
    is >> tempU16;
    data.setIacId(tempU16);
    is >> tempU16;
    data.setI2cId(tempU16);

    is >> tempBool;
    data.setHammingCheckingDisable(tempBool);

    u16 anotherTempU16;
    is >> tempU16 >> anotherTempU16;
    data.setCounterOperation(tempU16 | anotherTempU16);
    is >> tempBool;
    data.setDeskewedClock2Selected(tempBool);
    is >> tempBool;
    data.setDeskewedClock2Disable(tempBool);
    is >> tempBool;
    data.setClockL1AcceptDisable(tempBool);
    is >> tempBool;
    data.setParrallelOutputDisable(tempBool);
    is >> tempBool;
    data.setSerialBDisable(tempBool);
    is >> tempBool;
    data.setNonDeskewedClockDisable(tempBool);

    return is;
  }
  
  // <NAC date="24/04/2007"> operator to compare Ttcrx descriptions
  bool operator == (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r)
  {
    if (l._counterReset != r._counterReset) return false;
    if (l._l1AcceptCoarseDelay != r._l1AcceptCoarseDelay) return false;
    if (l._brcstStrTwoCoarseDelay != r._brcstStrTwoCoarseDelay) return false;
    if (l._clockDesOneFineDelay != r._clockDesOneFineDelay) return false;
    if (l._clockDesTwoFineDelay != r._clockDesTwoFineDelay) return false;
    if (l._dllPumpCurrent != r._dllPumpCurrent) return false;
    if (l._counterOperation != r._counterOperation) return false;
    if (l._disableHammingChecking != r._disableHammingChecking) return false;
    if (l._selectClockTwo != r._selectClockTwo) return false;
    if (l._disableClockTwo != r._disableClockTwo) return false;
    if (l._disableClockL1Accept != r._disableClockL1Accept) return false;
    if (l._disableParrallelBus != r._disableParrallelBus) return false;
    if (l._disableSerialB != r._disableSerialB) return false;
    if (l._disableNonDeskewedClock != r._disableNonDeskewedClock) return false;
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    for (size_t i = 0; i<Fed9UTtcrxDescription::CONTROL_REGISTER_SIZE; i++) {
      if (l._controlRegister[i] != r._controlRegister[i]) return false;
    }
    // </NAC>
    return true;
  }
  bool operator == (const Fed9UTtcrxCounterReset& l, const Fed9UTtcrxCounterReset& r)
  {
    if (l._errorCounters != r._errorCounters) return false;
    if (l._bunchCount != r._bunchCount) return false;
    if (l._eventCount != r._eventCount) return false;
    if (l._status != r._status) return false;
    return true;
  }
  // </NAC>

}   //End of Fed9U namespace.
