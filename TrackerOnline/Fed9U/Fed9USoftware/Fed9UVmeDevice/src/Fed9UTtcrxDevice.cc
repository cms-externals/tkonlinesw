#include <inttypes.h>
#include <stdint.h>
#include "Fed9UTtcrxDevice.hh"
#include "Fed9UVmeDeviceException.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UWait.hh"
namespace Fed9U {

  /**
   * Class for TTCrx configuration.
   * This class contains methods that can be used to configure
   * and read the status of the TTCrx chip.
   *
   * Every effort has been made to make this class fully documented. However for those
   * methods that are unclear or if not enough detail has been provided more information
   * can be found in the TTCrx Reference Manual - A Timing, Trigger and Control Reference
   * ASIC for LHC Detectors. The types of exceptions that can be expected to be thrown are
   * Fed9UVmeDeviceExceptions. All exceptions that are thrown from included classes are
   * hopefully caught and rethrown as a Fed9UVmeDeviceException. It is hoped that no other
   * exceptions can be thrown, although it cannot be guarantied.
   */

  /**
   * The Fed9UTtcrxDevice constructor.
   *
   * The Fed9UTtcrxDevice object passed as an arguement is used internally to retrieve
   * settings when writing to the TTCrx. The Fed9UVmeBase is required to give the TTCrx device
   * access to the TTCrx on the FED.
   */
  Fed9UTtcrxDevice::Fed9UTtcrxDevice(Fed9UVmeBase& fed, const Fed9UTtcrxDescription& ttcrx) :
    theFed(fed), theTtcrxDescription( *new Fed9UTtcrxDescription(ttcrx) ) {
  }

  /**
   * The Fed9UTtcrxDevice destructor.
   */
  Fed9UTtcrxDevice::~Fed9UTtcrxDevice() {
    delete &theTtcrxDescription;
  }

  /**
   * Will reset the TTCrx chip.
   *
   * An internal reset uses the I2C interface to send the reset, an external reset
   * does not. Both perform the same level of reset. You cannot reset the TTCrx while
   * the TTC clock is selected. If internal is true then an internal reset will be
   * sent. If it is false then an external reset will be sent.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::sendTtcrxReset(bool internal) {
    try {
      //The clock cannot be set to the TTC clock when the TTC is reset, this read the clock value from the
      //FED as this class has no access to Fed9UVmeDevice member functions i.e. the getClock() method.
      const bool read = true;
      u32 dummyWord = 0, selectedClock;
      theFed.vmeCommandSelectClock(read, dummyWord, selectedClock);
      ICUTILS_VERIFY(selectedClock != FED9U_CLOCK_TTC).error().msg("The TTCrx cannot be reset while the TTC clock has been selected");
      //Selects whether to perform an internal or external reset based on the boolean argument internal.
      internal ? theFed.beCommandTtcrxInternalReset() : theFed.beCommandTtcrxExternalReset();
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::sendTtcrxReset."));
    }
  }

  /**
   * This method will perform a reset on the error counters, bunch counter, event counter and the status
   * register. The register to be reset will be determined by the Fed9UTtcrxCounterReset struct.
   *
   * If the struct contains a true for a specific reset then that reset will be performed. A false and
   * it will not be performed.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setCounterReset(const Fed9UTtcrxCounterReset& counterReset) {
    try {
      //This selects which register is to be reset.
      //Some register need to write to more than one register address on the TTCrx.
      //This is reflected by multiple writes to the TTCrx with different register addresses.
      u32 regAddr;
      //The data that is written can be anything for the counter registers but must be 5
      //for the status register, hence 5 is written to the counters as well.
      const u32 writeData = 5;

      //Resets the error counters.
      if (true == counterReset._errorCounters) {
	//In order to reset all the data associated with these registers multiple registers
	//on the TTCrx must be reset.
	regAddr = 8;
	setTtcrxControls(regAddr, writeData);
	regAddr = 9;
	setTtcrxControls(regAddr, writeData);
	regAddr = 10;
	setTtcrxControls(regAddr, writeData);
	regAddr = 11;
	setTtcrxControls(regAddr, writeData);
      }
      //Resets the bunch count.
      if (true == counterReset._bunchCount) {
	regAddr = 24;
	setTtcrxControls(regAddr, writeData);
	regAddr = 25;
	setTtcrxControls(regAddr, writeData);
      }
      //Resets the event counter.
      if (true == counterReset._eventCount) {
	regAddr = 26;
	setTtcrxControls(regAddr, writeData);
	regAddr = 27;
	setTtcrxControls(regAddr, writeData);
	regAddr = 28;
	setTtcrxControls(regAddr, writeData);
      }
      //Resets the status resgister.
      if (true == counterReset._status) {
	regAddr = 22;
	setTtcrxControls(regAddr, writeData);
      }

      theTtcrxDescription.setCounterReset(counterReset);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setCounterReset."));
    }
  }

  /**
   * This set the total amount of delay skew for the L1Accept, broadcast strobe 1
   * signal, the bunch and event counter reset signals and the system broadcast
   * command.
   *
   * The coarse delay can be set from 0 to 15. Each step represents 25ns. The fine
   * delay can be set from 0 to 239, with each step representing approximately 104ps.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setL1AcceptDelay(u16 coarseDelay, u16 fineDelay) {
    try {
      ICUTILS_VERIFY(coarseDelay < 16 && fineDelay < 240)(coarseDelay)(fineDelay).error().msg("The L1Accept, broadcast strobe 1 signal, the bunch and event counter reset signals and the system broadcast command coarse delay is out of range.");
      setL1AcceptCoarseDelay(coarseDelay);
      setClockDesOneFineDelay(fineDelay);
      //There is no need to update the TTCrx description object as that will be done by the set coarse and fine delay methods.
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setL1AcceptDelay."));
    }
  }

  /**
   * This will set the total amount of delay skew for the broadcast strobe 2 signal
   * and the user broadcast command.
   *
   * This sets the coarse delay for those signals, where the coarse delay can be
   * set from 0 to 15. Each step represents 25ns. It sets the fine delay for either
   * the 40MHz deskewed clock one or two depending on which the signals are
   * synchronised to. This can be set in element two fo the control register vector.
   * The fine delay can be set from 0 to 239, with each step representing
   * approximately 104ps.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay) {
    try {
      ICUTILS_VERIFY(coarseDelay < 16 && fineDelay < 240)(coarseDelay)(fineDelay).error().msg("The broadcast strobe 2 signal and the user broadcast command delay is out of range.");
      setBrcstStrTwoCoarseDelay(coarseDelay);
      //The broadcast strobe two signal and the user broadcast command can either be synchronised to the
      //40MHz deskewed clock one or two depending on the setting in control register element two.
      //If true then it is synchronised to clock two and if false to clock one.
      if ( getDeskewedClock2Selected() ) {
	setClockDesTwoFineDelay(fineDelay);
      } else {
	setClockDesOneFineDelay(fineDelay);
      }
      //There is no need to update the TTCrx description object as that will be done by the set coarse and fine delay methods.
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setBrcstStrTwoDelay."));
    }
  }

  /**
   * This will set the coarse delay skew for the L1Accept, broadcast strobe 1 signal,
   * bunch and event counter reset signals and the system broadcast command.
   *
   * The coarse delay can be set from 0 to 15. Each step represents 25ns.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setL1AcceptCoarseDelay(u16 coarseDelay) {
    try {
      ICUTILS_VERIFY(coarseDelay < 16)(coarseDelay).error().msg("The L1Accept, broadcast strobe 1 signal, bunch and event counter reset signals and the system broadcast command coarse delay is out of range.");
      //The L1 Accept et al. coarse delay setting is in the first four bits of the register
      //and it needs to be combined with the broadcast strobe 2 et al. coarse delay, which
      //occupies the last four bits.
      u32 brcstStrTwoCoarseDelay = theTtcrxDescription.getBrcstStrTwoCoarseDelay();
      u32 timingData = (brcstStrTwoCoarseDelay << 4) | (coarseDelay & 0x0f);
      const u32 regAddr = 2;
      setTtcrxControls(regAddr, timingData);
      theTtcrxDescription.setL1AcceptCoarseDelay(coarseDelay);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setL1AcceptCoarseDelay."));
    }
  }

  /**
   * This will return the coarse delay skew for the deskewed 40MHz clock one.
   * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
   * event counter reset signals and the system broadcast command.
   *
   * The coarse delay can be 0 to 15. Each step represents 25ns.
   */
  u16 Fed9UTtcrxDevice::getL1AcceptCoarseDelay() const {
    try {
      const u32 regAddr = 2;
      u32 timingData    = getTtcrxControls(regAddr);
      //The deskewed clock one coarse delay is stored in the first four bits of this register.
      u16 coarseDelay  = timingData & 0x0f;
      return coarseDelay;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getL1AcceptCoarseDelay."));
    }
  }

  /**
   * This will set the coarse delay skew for the broadcast strobe 2 signal
   * and the user broadcast command.
   *
   * The coarse delay can be set from 0 to 15. Each step represents 25ns.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setBrcstStrTwoCoarseDelay(u16 coarseDelay) {
    try {
      ICUTILS_VERIFY(coarseDelay < 16)(coarseDelay).error().msg("The coarse delay is out of range.");
      //The braodcast strobe 2 and user broadcast data coarse delay setting is in the last four bits of the register
      //and it needs to be combined with the L1 accept et al. coarse delay, which occupies the last four bits.
      u32 l1AcceptCoarseDelay = theTtcrxDescription.getL1AcceptCoarseDelay();
      u32 timingData = (coarseDelay << 4) | (l1AcceptCoarseDelay & 0x0f);
      const u32 regAddr = 2;
      setTtcrxControls(regAddr, timingData);
      theTtcrxDescription.setBrcstStrTwoCoarseDelay(coarseDelay);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setBrcstStrTwoCoarseDelay."));
    }
  }

  /**
   * This will set the coarse delay skew for the broadcast strobe 2 signal
   * and the user broadcast command.
   *
   * The coarse delay can be 0 to 15. Each step represents 25ns.
   */
  u16 Fed9UTtcrxDevice::getBrcstStrTwoCoarseDelay() const {
    try {
      const u32 regAddr = 2;
      u32 timingData    = getTtcrxControls(regAddr);
      //The broadcast strobe 2 and user broad cast command coarse delay is stored in the last four bits of this register.
      u16 coarseDelay   = (timingData >> 4) & 0x0f;
      return coarseDelay;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getBrcstStrTwoCoarseDelay."));
    }
  }

  /**
   * This will set the fine delay skew for the deskewed 40MHz clock one.
   * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
   * event counter reset signals and the system broadcast command. It will
   * also effect the broadcast strobe two and the user broadcast command
   * if element 2 in the control register is disabled.
   *
   * The fine delay can be set from 0 to 239, with each step representing
   * approximately 104ps.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setClockDesOneFineDelay(u16 fineDelay) {
    try {
      ICUTILS_VERIFY(fineDelay < 240)(fineDelay).error().msg("The deskewed clock one fine delay is out of range.");
      //n and m are values used by the TTCrx to calculate the skew.
      u32 n = fineDelay%15;
      u32 m = (fineDelay/15 - n + 14)%16;
      u32 timingData = (n<<4) + m;
      //Sets the register address that needs to be written to on the TTCrx chip.
      const u32 regAddr = 0;
      //Begins the writing process.
      setTtcrxControls(regAddr, timingData);
      theTtcrxDescription.setClockDesOneFineDelay(fineDelay);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setClockDesOneFineDelay."));
    }
  }

  /**
   * This will return the fine delay skew for the deskewed 40MHz clock one.
   * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
   * event counter reset signals and the system broadcast command. It will
   * also effect the broadcast strobe two and the user broadcast command
   * if element 2 in the control register is disabled.
   *
   * The fine delay can be 0 to 239, with each step representing
   * approximately 104ps.
   */
  u16 Fed9UTtcrxDevice::getClockDesOneFineDelay() const {
    try {
      //Sets the register address that needs to be written to on the TTCrx chip.
      const u32 regAddr = 0;
      u32 timingData = getTtcrxControls(regAddr);
      //Convert the data from the TTCrx to the fine delay.
      u32 m =  timingData     & 0xf;
      u32 n = (timingData>>4) & 0xf;
      //This is the number of 104ps steps that the clock is skewed by.
      u16 fineDelay = (15*m + 16*n + 30)%240;
      return fineDelay;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getClockDesOneFineDelay."));
    }
  }

  /**
   * This will set the fine delay skew for the deskewed 40MHz clock two.
   * This will affect the broadcast strobe 2 signal and the user broadcast
   * command only if it is enabled by setting element 2 in the control register vector.
   *
   * The fine delay can be set from 0 to 239, with each step representing
   * approximately 104ps.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setClockDesTwoFineDelay(u16 fineDelay) {
    try {
      ICUTILS_VERIFY(fineDelay < 240)(fineDelay).error().msg("The deskewed clock two fine delay is out of range.");
      //n and m are values used by the TTCrx to calculate the skew.
      u32 n = fineDelay%15;
      u32 m = (fineDelay/15 - n + 14)%16;
      u32 timingData = (m<<4) | n;
      //Sets the register address that needs to be written to on the TTCrx chip.
      const u32 regAddr = 1;
      //Begins the writing process.
      setTtcrxControls(regAddr, timingData);
      theTtcrxDescription.setClockDesTwoFineDelay(fineDelay);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setClockDesTwoCoarseDelay."));
    }
  }

  /**
   * This will return the fine delay skew for the deskewed 40MHz clock two.
   * This will affect the broadcast strobe 2 signal and the user broadcast
   * command only if it is enabled by setting element 2 in the control register vector.
   *
   * The fine delay can be 0 to 239, with each step representing
   * approximately 104ps.
   */
  u16 Fed9UTtcrxDevice::getClockDesTwoFineDelay() const {
    try {
      //Sets the register address that needs to be written to on the TTCrx chip.
      const u32 regAddr = 1;
      u32 timingData = getTtcrxControls(regAddr);
      //Convert the data from the TTCrx to the fine delay.
      u32 n =  timingData     & 0xf;
      u32 m = (timingData>>4) & 0xf;
      //This is the number of 104ps steps that the clock is skewed by.
      u16 fineDelay = (15*m + 16*n + 30)%240;
      return fineDelay;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setClockDesTwoCoarseDelay."));
    }
  }

  /**
   * Sets the charge pump current used in the delay-lock loops of the high
   * resolution phase shifters.
   *
   * It has a maximum value of 7 and a minimum value of 1.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setDllPumpCurrent(u16 pumpCurrent) {
    try {
      ICUTILS_VERIFY(pumpCurrent < 8 && pumpCurrent > 0)(pumpCurrent).error().msg("The DLL pump current is out of range");
      u16 pllPumpCurrent = theTtcrxDescription.getPllPumpCurrent();
      //The configuration register also contains the PLL pump current setting and two read only bits
      //that are zero and so don't need to be included in the command.
      u32 configRegBits = 0;
      configRegBits |=  pumpCurrent & 0x07;
      configRegBits |= (pllPumpCurrent << 3) & 0x38;
      const u32 regAddr = 19;
      setTtcrxControls(regAddr, configRegBits);
      theTtcrxDescription.setDllPumpCurrent(pumpCurrent);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setDllPumpCurrent."));
    }
  }

  /**
   * Returns the charge pump current used in the delay-lock loops of the high
   * resolution phase shifters.
   */
  u16 Fed9UTtcrxDevice::getDllPumpCurrent() const {
    try {
      const u32 regAddr = 19;
      u32 configRegBits = getTtcrxControls(regAddr);
      //Removes the bit from the configuration register that specify the DLL pump current.
      u16 pumpCurrent = configRegBits & 0x7;
      return pumpCurrent;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getDllPumpCurrent."));
    }
  }

  /**
   * Sets the charge pump current used in the phase-lock loops which provide
   * the clock recovery.
   *
   * It has a maximum value of 7 and a minimum value of 1.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setPllPumpCurrent(u16 pumpCurrent) {
    try {
      ICUTILS_VERIFY(pumpCurrent < 8 && pumpCurrent > 0)(pumpCurrent).error().msg("The PLL pump current is out of range");
      u16 dllPumpCurrent = theTtcrxDescription.getDllPumpCurrent();
      //The configuration register also contains the DLL pump current setting and two read only bits
      //that are zero and so don't need to be included in the command.
      u32 configRegBits = 0;
      configRegBits |=  dllPumpCurrent    & 0x07;
      configRegBits |= (pumpCurrent << 3) & 0x38;
      const u32 regAddr = 19;
      setTtcrxControls(regAddr, configRegBits);
      theTtcrxDescription.setPllPumpCurrent(pumpCurrent);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setPllPumpCurrent."));
    }
  }

  /**
   * Returns the charge pump current used in the phase-lock loops which provide
   * the clock recovery.
   */
  u16 Fed9UTtcrxDevice::getPllPumpCurrent() const {
    try {
      const u32 regAddr = 19;
      u32 configRegBits = getTtcrxControls(regAddr);
      //Removes the bit from the configuration register that specify the PLL pump current.
      u16 pumpCurrent = (configRegBits >> 3) & 0x7;
      return pumpCurrent;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getPllPumpCurrent."));
    }
  }

  /**
   * This sets the 14 bit individually addressed command (IAC) ID.
   *
   * It is used to identifying the individually addressed commands (IAC) that are sent
   * over the optical link. It has a maximum value of 16383.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setIacId(u16 id) {
    try {
      ICUTILS_VERIFY(id < 16384)(id).error().msg("The individually addressed command (IAC) ID is out of range.");
      //The IAC ID is spread over two registers.
      //The first register contains the first 8 bits of the ID.
      u32 newId = id & 0xff;
      //This has its own register.
      u32 regAddr = 16;
      setTtcrxControls(regAddr, newId);

      //The last 6 bits of the IAC ID are contained in another register.
      //The last 2 bits are taken up by the master mode A settings.
      //This is hard wired to zero on the TTCrx.
      //Reset the newId to be used in the next write.
      newId = 0;
      newId = (id >> 8) & 0x3f;
      //This part of the ID has its own register.
      regAddr = 17;
      setTtcrxControls(regAddr, newId);

      theTtcrxDescription.setIacId(id);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setIacId."));
    }
  }

  /**
   * This returns the 14 bit individually addressed command (IAC) ID.
   *
   * It is used to identifying the individually addressed commands (IAC) that are sent
   * over the optical link. It has a maximum value of 16383.
   */
  u16 Fed9UTtcrxDevice::getIacId() const {
    try {
      //The chip id is spread over two registers so both
      //must be read and then combined to give the ID and mode.
      //This register only contains the first part of the id number.
      u32 regAddr = 16;
      u32 idLower = getTtcrxControls(regAddr) & 0xff;
      //This register contains both the msater mode A and the last
      //6 bits of the ID number.
      regAddr = 17;
      //This masks the returned ID to remove the mast mode A bits.
      u32 idUpper = getTtcrxControls(regAddr) & 0x3f;

      u16 id = (idUpper << 8) | idLower;
      return id;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getIacId."));
    }
  }

  /**
   * This sets the 6 bit I2C ID.
   *
   * It is used as the base address for accessing the TTCrx using the I2C interface.
   * It has a maximum value of 63.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setI2cId(u16 id) {
    try {
      ICUTILS_VERIFY(id < 64)(id).error().msg("The I2C ID is out of range.");
      //The I2C ID occupies only the first 6 bits of the register.
      //The last 2 bits are taken up by the master mode A settings.
      //This is hard wired to zero on the TTCrx.
      u32 newI2cId = id & 0x3f;

      u32 regAddr  = 18;
      setTtcrxControls(regAddr, newI2cId);
      theTtcrxDescription.setI2cId(id);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setI2cId."));
    }
  }

  /**
   * This returns the 6 bit I2C ID.
   *
   * It is used as the base address for accessing the TTCrx using the I2C interface.
   */
  u16 Fed9UTtcrxDevice::getI2cId() const {
    try {
      u32 regAddr = 18;
      //The I2C ID occupies only the first 6 bits of this register. Hence the returned
      //value is masked to remove only these bits.
      u16 id = getTtcrxControls(regAddr) & 0x3f;
      return id;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getI2cId."));
    }
  }

  /**
   * This is used to control enable or disable the bunch and event counter operations.
   * The individual settings can be enabled or disabled using the enum
   * Fed9UTtcrxCtrOperation found in TypeDefs.hh. Both flags can be set using a bitwise
   * OR and any flag not set will be disabled.
   *
   * The bunch and event counter settings are used in conjuction to specify the mode
   * of the level 1 trigger sequency. For more information see the TTCrx Reference
   * Manual chapter 6.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setCounterOperation(u16 counterOperation) {
    try {
      ICUTILS_VERIFY(counterOperation < 4)(counterOperation).error().msg("The counterOperation argument is out of range.");
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= counterOperation;
      controlRegBits |= static_cast<u32>(theTtcrxDescription.getDeskewedClock2Selected())   << 2;
      controlRegBits |= static_cast<u32>(!theTtcrxDescription.getDeskewedClock2Disable())   << 3;
      controlRegBits |= static_cast<u32>(!theTtcrxDescription.getClockL1AcceptDisable())    << 4;
      controlRegBits |= static_cast<u32>(!theTtcrxDescription.getParrallelOutputDisable())  << 5;
      controlRegBits |= static_cast<u32>(!theTtcrxDescription.getSerialBDisable())          << 6;
      controlRegBits |= static_cast<u32>(!theTtcrxDescription.getNonDeskewedClockDisable()) << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setCounterOperation(counterOperation);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setCounterOperation."));
    }
  }

  /**
   * This is used to the bunch and event counter operations enable or disable flags.
   * The individual settings can be checked by using the masks provided in
   * Fed9UTtcrxCtrOperation found in TypeDefs.hh.
   *
   * The bunch and event counter settings are used in conjuction to specify the mode
   * of the level 1 trigger sequency. For more information see the TTCrx Reference
   * Manual chapter 6. Disabling these can be used to save power.
   */
  u16 Fed9UTtcrxDevice::getCounterOperation() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      u16 counterOperation = controlRegBits & 0x3;
      return counterOperation;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getCounterOperation."));
    }
  }

  /**
   * This can be used to enable or disable the hamming checking.
   *
   * A true will disable the hamming checking and a false will enable hamming checking.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setHammingCheckingDisable(bool disableHammingChecking) {
    try {
      //There are two 8 bit registers that need to be written to for the hamming
      //checking to be enabled or disabled. It is the 8th bit that needs to be
      //written to in each case. The second register is there only for redundancy,
      //however both bits need to be zero to disable the checking.

      //For the 1st register the default values of the 1st 7 bits are equivalent to a hex 15 and are read only.
      u32 configRegBits = 0x15;
      configRegBits |= (disableHammingChecking ? 0:1) << 7;
      u32 regAddr = 20;
      setTtcrxControls(regAddr, configRegBits);

      //For the 2nd register the 1st 7 bits are equivalent to a hex 27 and are read only.
      configRegBits = 0x27;
      configRegBits |= (disableHammingChecking ? 0:1) << 7;
      regAddr = 21;
      setTtcrxControls(regAddr, configRegBits);
      theTtcrxDescription.setHammingCheckingDisable(disableHammingChecking);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setHammingCheckingDisabled."));
    }
  }

  /**
   * This can be used to read the status of the hamming checking.
   *
   * A true means that hamming checking is disabled and a false enabled.
   */
  bool Fed9UTtcrxDevice::getHammingCheckingDisable() const {
    try {
      //There are two different registers that control the state of the hammer checking.
      //This will read the state from the first register.
      u32 regAddr = 20;
      u32 configRegBits = getTtcrxControls(regAddr);
      //This uses the opposite logic system to the FED.
      bool hammingCheckingOne = !(configRegBits>>7 & 0x1);

      //This will read the state from the second register.
      regAddr = 21;
      configRegBits = getTtcrxControls(regAddr);
      bool hammingCheckingTwo = !(configRegBits>>7 & 0x1);

      //Both registers must be in the same state for the hamming checking to be properly enabled or disabled.
      ICUTILS_VERIFY(hammingCheckingOne == hammingCheckingTwo)(hammingCheckingOne)(hammingCheckingTwo).error().msg("The state of the hamming checking must be the same from both registers for it to be proeprly enabled or disabled.");
      //Both hamming checking variables will be in the same state if the above ICUTILS_VERIFY has succeded so only
      //one of them needs be used for the return value.
      return hammingCheckingOne;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getHammingCheckingDisabled."));
    }
  }

  /**
   * This selectes the 40MHz deskewed clock two. If selected it is used to sychronise
   * the broadcast strobe 2 signal and the user broadcast command. A true will select
   * this clock.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setDeskewedClock2Selected(bool selectClockTwo) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= selectClockTwo << 2;
      controlRegBits |= !theTtcrxDescription.getDeskewedClock2Disable()   << 3;
      controlRegBits |= !theTtcrxDescription.getClockL1AcceptDisable()    << 4;
      controlRegBits |= !theTtcrxDescription.getParrallelOutputDisable()  << 5;
      controlRegBits |= !theTtcrxDescription.getSerialBDisable()          << 6;
      controlRegBits |= !theTtcrxDescription.getNonDeskewedClockDisable() << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setDeskewedClock2Selected(selectClockTwo);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setDeskewedClock2Selected."));
    }
  }

  /**
   * If a true is returned then the broadcast strobe 2 signal and the user broadcast
   * command are synchronised to the 40MHz deskewed clock two.
   */
  bool Fed9UTtcrxDevice::getDeskewedClock2Selected() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool selectClockTwo = controlRegBits & 0x4;
      return selectClockTwo;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getDeskewedClock2Selected."));
    }
  }

  /**
   * If a true is passed this will disable the 40MHz deskewed clock two.
   * If false the clock will be disabled.
   *
   * This can be disabled to save power.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setDeskewedClock2Disable(bool disableClockTwo) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= theTtcrxDescription.getDeskewedClock2Selected()  << 2;
      controlRegBits |= !disableClockTwo << 3;
      controlRegBits |= !theTtcrxDescription.getClockL1AcceptDisable()    << 4;
      controlRegBits |= !theTtcrxDescription.getParrallelOutputDisable()  << 5;
      controlRegBits |= !theTtcrxDescription.getSerialBDisable()          << 6;
      controlRegBits |= !theTtcrxDescription.getNonDeskewedClockDisable() << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setDeskewedClock2Disable(disableClockTwo);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setDeskewedClock2Disable."));
    }
  }

  /**
   * If a true is returned then the deskewed clock two is disabled. If
   * false it is enabled.
   */
  bool Fed9UTtcrxDevice::getDeskewedClock2Disable() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool disable = !(controlRegBits & 0x8);
      return disable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getDeskewedClock2Disable."));
    }
  }

  /**
   * Disable the L1Accept clock output.
   *
   * A true will disable the output and a false enable it.
   * This can be disabled to save power.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setClockL1AcceptDisable(bool l1AcceptDisable) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= theTtcrxDescription.getDeskewedClock2Selected()   << 2;
      controlRegBits |= !theTtcrxDescription.getDeskewedClock2Disable()   << 3;
      controlRegBits |= !l1AcceptDisable << 4;
      controlRegBits |= !theTtcrxDescription.getParrallelOutputDisable()  << 5;
      controlRegBits |= !theTtcrxDescription.getSerialBDisable()          << 6;
      controlRegBits |= !theTtcrxDescription.getNonDeskewedClockDisable() << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setClockL1AcceptDisable(l1AcceptDisable);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setClockL1AcceptDisable."));
    }
  }

  /**
   * Returns the setting on the L1Accept clock disable flag.
   *
   * A true means it is disabled and a false enabled.
   */
  bool Fed9UTtcrxDevice::getClockL1AcceptDisable() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool disable = !(controlRegBits & 0x10);
      return disable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getClockL1AcceptDisable."));
    }
  }

  /**
   * This will disable the non-deskewed 40MHz clock output if a
   * true is passed to the function. A false will enable it.
   *
   * This can be disabled to save power.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setNonDeskewedClockDisable(bool disable) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= theTtcrxDescription.getDeskewedClock2Selected()   << 2;
      controlRegBits |= !theTtcrxDescription.getDeskewedClock2Disable()   << 3;
      controlRegBits |= !theTtcrxDescription.getClockL1AcceptDisable()    << 4;
      controlRegBits |= !theTtcrxDescription.getParrallelOutputDisable()  << 5;
      controlRegBits |= !theTtcrxDescription.getSerialBDisable()          << 6;
      controlRegBits |= !disable << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setNonDeskewedClockDisable(disable);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setNonDeskewedClockDisable."));
    }
  }

  /**
   * If true then the non-deskewed 40MHz clock is disabled.
   * False and it is enabled.
   */
  bool Fed9UTtcrxDevice::getNonDeskewedClockDisable() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool disable = !(controlRegBits & 0x80);
      return disable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getNonDeskewedClockDisable."));
    }
  }

  /**
   * Disable the parrallel bus output if true.
   *
   * This can be disabled to save power.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setParrallelOutputDisable(bool disable) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= theTtcrxDescription.getDeskewedClock2Selected()   << 2;
      controlRegBits |= !theTtcrxDescription.getDeskewedClock2Disable()   << 3;
      controlRegBits |= !theTtcrxDescription.getClockL1AcceptDisable()    << 4;
      controlRegBits |= !disable << 5;
      controlRegBits |= !theTtcrxDescription.getSerialBDisable()          << 6;
      controlRegBits |= !theTtcrxDescription.getNonDeskewedClockDisable() << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setParrallelOutputDisable(disable);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setParrallelOutputDisable."));
    }
  }

  /**
   * Return true if the parrallel bus output is disabled.
   */
  bool Fed9UTtcrxDevice::getParrallelOutputDisable() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool disable = !(controlRegBits & 0x20);
      return disable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getParrallelOutputDisable."));
    }
  }

  /**
   * Disable the serial B output if true.
   *
   * This can be disabled to save power.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setSerialBDisable(bool disable) {
    try {
      u32 controlRegBits = 0;
      //The control register contains 8 bits. This will combine all the relevant settings from the description
      //into a single word.
      controlRegBits |= theTtcrxDescription.getCounterOperation();
      controlRegBits |= theTtcrxDescription.getDeskewedClock2Selected()   << 2;
      controlRegBits |= !theTtcrxDescription.getDeskewedClock2Disable()   << 3;
      controlRegBits |= !theTtcrxDescription.getClockL1AcceptDisable()    << 4;
      controlRegBits |= !theTtcrxDescription.getParrallelOutputDisable()  << 5;
      controlRegBits |= !disable << 6;
      controlRegBits |= !theTtcrxDescription.getNonDeskewedClockDisable() << 7;
      const u32 regAddr = 3;
      setTtcrxControls(regAddr, controlRegBits);
      theTtcrxDescription.setSerialBDisable(disable);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setSerialBDisable."));
    }
  }

  /**
   * Return true if the serial B output is disabled.
   */
  bool Fed9UTtcrxDevice::getSerialBDisable() const {
    try {
      const u32 regAddr  = 3;
      u32 controlRegBits = getTtcrxControls(regAddr);
      //Mask out the two relevant bits.
      bool disable = !(controlRegBits & 0x40);
      return disable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getSerialBDisable."));
    }
  }

//   //Depreciated. \todo - to be removed in later versions.
//   /**
//    * This is used to control the operational mode of the TTCrx chip. It can also be
//    * used to reduce power consumption by disabling some of the chips features.
//    *
//    * The control register is set using an eight element vector. A description of the elements
//    * is given below.
//    * 0 Enable bunch counter operation - default true.
//    * 1 Enable event counter operation - default true.
//    * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//    * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//    * explanation.
//    * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//    *                        synchronise the Broadcast Strobe 2 - default false.
//    * 3 Enable Deskewed clock 2 output   - default false.
//    * 4 Enable L1Accept clock output - default true.
//    * 5 Enable Parallel output bus  - default false.
//    * 6 Enable Serial B output       - default false.
//    * 7 Enable (non-deskewed) 40MHz clock output - default true.
//    * In all cases a true is used to enable the setting and false to disable.
//    */
//   Fed9UTtcrxDevice& Fed9UTtcrxDevice::setControlRegister(vector<bool> controlRegister) {
//     try {
//       ICUTILS_VERIFY(controlRegister.size() == 8)(controlRegister.size()).error().msg("The control register settings vector is of an invalid size.");
//       u32 controlRegBits = 0;
//       //The control register contains 8 bits. This will loop over all the settings
//       //in the input vector elements and create an 8 bit number which is written to
//       //the TTCrx.
//       for (u32 i = 0; i < 8; i++) {
// 	controlRegBits |= controlRegister[i] << i;
//       }
//       const u32 regAddr = 3;
//       setTtcrxControls(regAddr, controlRegBits);
//       theTtcrxDescription.setControlRegister(controlRegister);

//       return *this;
//     }
//     catch (const Fed9UException& e) {
//       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setControlRegister."));
//     }
//   }

//   //Depreciated. \todo - to be removed in later versions.
//   /**
//    * This is used to read back the operational mode of the TTCrx chip.
//    *
//    * The settings are returned in an eight element vector. A description of the elements
//    * is given below.
//    * 0 Enable bunch counter operation - default true.
//    * 1 Enable event counter operation - default true.
//    * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//    * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//    * explanation.
//    * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//    *                        synchronise the Broadcast Strobe 2 - default false.
//    * 3 Enable Deskewed clock 2 output   - default false.
//    * 4 Enable L1Accept clock output - default true.
//    * 5 Enable Parallel output bus  - default false.
//    * 6 Enable Serial B output       - default false.
//    * 7 Enable (non-deskewed) 40MHz clock output - default true.
//    * In all cases a true is used to enable the setting and false to disable.
//    */
//   vector<bool> Fed9UTtcrxDevice::getControlRegister() const {
//     try {
//       const u32 regAddr  = 3;
//       u32 controlRegBits = getTtcrxControls(regAddr);
//       //Loops over the 8 bits read from the TTCrx and inserts each one into an element in
//       //the control register settings vector.
//       vector<bool> controlRegSettings(8,0);
//       for (u32 i = 0; i < 8; i++) {
// 	controlRegSettings[i] = (controlRegBits >> i) & 0x1;
//       }
//       return controlRegSettings;
//     }
//     catch (const Fed9UException& e) {
//       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getControlRegister."));
//     }
//   }

  /**
   * This returns the number of bunch crossing that have occured since the last reset.
   *
   * It counts the number of clock signals that are received by the TTCrx. It is a 12
   * bit register and so has a maximum value of 4095.
   */
  u16 Fed9UTtcrxDevice::getBunchCounter() const {
    try {
      //This retreives the first 8 bits of the bunch crossing counter.
      u32 regAddr = 24;
      u16 lowerBunchCount = getTtcrxControls(regAddr);
      //This retreives the last 4 bits of the bunch crossing counter.
      regAddr = 25;
      u16 upperBunchCount = getTtcrxControls(regAddr);
      //Now they are combined into a 12 bit number, which is the return value.
      u16 bunchCount = (upperBunchCount << 8) | (lowerBunchCount & 0xff);
      return bunchCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getBunchCounter."));
    }
  }

  /**
   * This returns the number of events that have been received since the last reset.
   *
   * It counts the number of trigger signals that are received by the TTCrx. It is a 24
   * bit register and so has a maximum value of 16777215.
   */
  u32 Fed9UTtcrxDevice::getEventCounter() const {
    try {
      //This retreives the first 8 bits of the eventcounter.
      u32 regAddr = 26;
      u32 lowerEventCount = getTtcrxControls(regAddr);
      //This retreives the middle 8 bits of the event counter.
      regAddr = 27;
      u32 middleEventCount = getTtcrxControls(regAddr);
      //This retreives the last 8 bits of the event counter.
      regAddr = 28;
      u32 upperEventCount = getTtcrxControls(regAddr);
      //Now they are combined into a 24 bit number, which is the return value.
      u32 eventCount = upperEventCount<<16 | middleEventCount<<8 | lowerEventCount&0xff;
      return eventCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getEventCounter."));
    }
  }

  /**
   * This return the number of single bit errors that are recignised and corrected by the
   * receiver's Hamming decoder.
   *
   * Once this reaches its maximum value of 65535 the register will need to be reset.
   */
  u16 Fed9UTtcrxDevice::getSingleBitErrorCount() const {
    try {
      //The single error count is split over two registers on the TTCrx.
      u32 regAddr = 8;
      u32 singleErrorCountOne = getTtcrxControls(regAddr);
      regAddr = 9;
      u32 singleErrorCountTwo = getTtcrxControls(regAddr);

      u32 singleErrorCount = (singleErrorCountTwo<<8) | singleErrorCountOne;
      return singleErrorCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getSingleBitErrorCount."));
    }
  }

  /**
   * Returns the number of double bit Hamming and frame errors.
   *
   * Once it reaches its maximum value of 255 the register will need to be reset.
   */
  u16 Fed9UTtcrxDevice::getDoubleBitErrorCount() const {
    try {
      //This just requires a read to the appropiate register to get the counter value.
      u32 regAddr = 10;
      u16 errorCount = getTtcrxControls(regAddr);
      return errorCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getDoubleBitErrorCount."));
    }
  }

  /**
   * Returns the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
   * It has a maximum value of 255.
   */
  u16 Fed9UTtcrxDevice::getSeuErrorCount() const {
    try {
      //This just requires a read to the appropiate register to get the counter value.
      u32 regAddr = 11;
      u16 errorCount = getTtcrxControls(regAddr);
      return errorCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getSeuErrorCount."));
    }
  }

  /**
   * This is a read only value that determines the over all operating mode of the circuit.
   *
   * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
   * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
   * when running a standard setup.
   */
  u16 Fed9UTtcrxDevice::getMasterModeA() const {
    try {
      u32 regAddr = 17;
      //The master mode B occupies bits 6 and 7 of this register. Hence the returned
      //value is masked to remove only these bits.
      u16 masterMode = (getTtcrxControls(regAddr) >> 6) & 0x3;
      return masterMode;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getMasterModeA."));
    }
  }

  /**
   * This is a read only value that determines the over all operating mode of the circuit.
   *
   * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
   * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
   * when running a standard setup.
   */
  u16 Fed9UTtcrxDevice::getMasterModeB() const {
    try {
      u32 regAddr = 18;
      //The master mode B occupies bits 6 and 7 of this register. Hence the returned
      //value is masked to remove only these bits.
      u16 masterMode = (getTtcrxControls(regAddr) >> 6) & 0x3;
      return masterMode;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getMasterModeB."));
    }
  }

  /**
   * This is a read only register, which selects the test outputs.
   *
   * This register is used for testing and debugging the TTCrx. It
   * has a maximum value of 7.
   */
  u16 Fed9UTtcrxDevice::getTestOutputs() const {
    try {
      u32 regAddr = 20;
      //The test output settings occupy the first 3 bits of this register.
      return getTtcrxControls(regAddr) & 0x7;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getTestOutputs."));
    }
  }

  /**
   * This is a read only register, which selects the number of clock
   * cycles after which the frequency phase detection will stop.
   *
   * It is a testing and debugging register. It has a maximum value
   * of 7 and a hardware default value of 7.
   */
  u16 Fed9UTtcrxDevice::getFreqDetectCount() const {
    try {
      u32 regAddr = 21;
      //The frequency detection count occupies the first 3 bits of this register.
      return getTtcrxControls(regAddr) & 0x7;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getFreqDetectCount."));
    }
  }

  /**
   * This is a read only configuration register. It contains some testing
   * and debugging settings. The enum Fed9UTtcrxConfig can be used to set
   * these bits. Any setting not set will be cleared.
   *
   * A description of the bits is given below. As these are read only on the
   * TTCrx, what is read back should correspond to the defaults given below.
   * These defaults are also the settings used by the TTCrx.
   * FED9U_TTCRX_CONFIG_SEL_TEST_PD - Selects the external test signal for enabling
   *                                  the PPL phase detector. Default is 0
   * FED9U_TTCRX_CONFIG_SEL_INPUT_A - When 1 selects the inputs from option link,
   *                                  otherwise test_in. Default is 1.
   * FED9U_TTCRX_CONFIG_PLL_RESET   - Assert PLL test reset line. Default 0.
   * FED9U_TTCRX_CONFIG_DLL_RESET   - Assert DLL test reset line. Default 0.
   * FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR - If 1 disables the automatic frequency increase
   *                                        after a PLL reset. Default 0.
   * FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG  - If 1 disbale watchdog circuit. Default 0.
   * FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION - Enable hamming error correction on
   *                                                incoming data stream. Default 1.
   * FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS - Enables test input/outputs. Default 0.
   */
  u16 Fed9UTtcrxDevice::getConfigSettings() const {
    try {
      u16 configSettings = 0;
      //The configuration settings are stored in two registers.
      u32 regAddr = 20;
      configSettings |= (getTtcrxControls(regAddr) & 0x78) >> 3;
      regAddr = 21;
      configSettings |= (getTtcrxControls(regAddr) & 0x78) << 1;
      return configSettings;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getConfigSettings."));
    }
  }

//   //\todo - depreciated, delete.
//   /**
//    * This is a read only configuration register. The settings here control the
//    * behaviour of the TTCrx.
//    *
//    * The returned vector has 10 elements. The contents of the elements is:-
//    * 0 Shows which test outputs have been selected.
//    * 1 Whether an external test signal is enabled for the phase-lock loop phase detector.
//    * 2 When 1 selects input from the optical link.
//    * 3 Assert phase-lock loop test reset line.
//    * 4 Assert delay-lock loop test reset line.
//    * 5 Frequency check setting.
//    * 6 If 1 then the automatic frequency increase after a phase-lock loop reset will be disabled.
//    * 7 If 1 then the watchdog circuit is disabled.
//    * 8 Whether or not the Hamming error detection/correction on incoming data stream is enabled.
//    * 9 Whether the test input/outputs are enabled or disabled.
//    */
//   vector<u16> Fed9UTtcrxDevice::getConfigurationSettings() const {
//     try {
//       //The configuration settings are spread out over two registers.
//       vector<u16> configurationSettings(10,0);
//       //This finds the settings from the first register.
//       u32 regAddr = 20;
//       const u32 configSettingsOne = getTtcrxControls(regAddr);
//       configurationSettings[0] =  configSettingsOne     & 0x7;
//       configurationSettings[1] = (configSettingsOne>>3) & 0x1;
//       configurationSettings[2] = (configSettingsOne>>4) & 0x1;
//       configurationSettings[3] = (configSettingsOne>>5) & 0x1;
//       configurationSettings[4] = (configSettingsOne>>6) & 0x1;
//       //From the second register.
//       regAddr = 21;
//       const u32 configSettingsTwo = getTtcrxControls(regAddr);
//       configurationSettings[5] =  configSettingsTwo     & 0x7;
//       configurationSettings[6] = (configSettingsTwo>>3) & 0x1;
//       configurationSettings[7] = (configSettingsTwo>>4) & 0x1;
//       configurationSettings[8] = (configSettingsTwo>>5) & 0x1;
//       configurationSettings[9] = (configSettingsTwo>>6) & 0x1;

//       return configurationSettings;
//     }
//     catch (const Fed9UException& e) {
//       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getConfigurationSettings."));
//     }
//   }

  /**
   * Returns the status of of internal monitoring signals. The returned u16 can be masked
   * with the Fed9UTtcrxStatus enum defined in TypeDefs.hh to check for set flags.
   *
   * The flags that can be set are as follows :-
   * Auto Reset Flag - a 1 indicates an automatic reset has occurred due to a
   *                   timeout condition in the watchdog circuit.
   * Frame Synch     - a 1 indicates that channel B is synchronized to the
   *                   data stream.
   * DLL Ready       - a 1 indicates the High-Resolution phase shiftes are
   *                   working properly.
   * PLL ready       - a 1 indicates that the clock and data recovery circuit
   *                   is locked on the incoming data stream.
   * Elements 1 to 3 should read a 1 under normal operating conditions.
   */
  u16 Fed9UTtcrxDevice::getStatusRegister() const {
    try {
      const u32 regAddr = 22;
      u32 statusRegisterBits = getTtcrxControls(regAddr);
      u16 statusRegister = 0;
      statusRegister |= statusRegisterBits>>4 & FED9U_TTCRX_STATUS_AUTO_RESET;
      statusRegister |= statusRegisterBits>>4 & FED9U_TTCRX_STATUS_FRAME_SYNCH;
      statusRegister |= statusRegisterBits>>4 & FED9U_TTCRX_STATUS_DLL_READY;
      statusRegister |= statusRegisterBits>>4 & FED9U_TTCRX_STATUS_PLL_READY;

      return statusRegister;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getStatusRegisters."));
    }
  }

  /**
   * Performs the writes to the registers on the TTCrx chip.
   */
  Fed9UTtcrxDevice& Fed9UTtcrxDevice::setTtcrxControls(const u32 regAddr, const u32 timingData) {
    try {
      ICUTILS_VERIFY(regAddr<32 && timingData<256)(regAddr)(timingData).error().msg("An invalid argument has been passed");
      //This is used because theFed requires read arguments when read and when writing. The dummy word is passed
      //in the place of the read arguments, but nothing is done with it.
      u32 dummyWord = 0;
      //These two bools are returned as true from the FED if there is an error in the write process.
      bool busy  = false;
      bool error = false;
      std::vector<u32> timeDataAndRegAddr(2,0);

      timeDataAndRegAddr[0] = timingData;
      timeDataAndRegAddr[1] = regAddr;
      //Passing a false to theFed will cause it to write.
      const bool write   = false;
      //Controls the number of time that the software wil try to write to the TTC if a busy or error signal is returned.
      const u32 maxLoops = 100;
      u32 loopCount      = 0;
      do {
	theFed.beCommandTtcrxControl(write, timeDataAndRegAddr, dummyWord, busy, error);
	//If the write still fails after a set number of attempts then the software gives up and throws an error.
	ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1 then the TTCrx device is busy. If error = 1 then the write failed");
	loopCount++;
	//If the write fails there is a pause before it tries again.
	if (busy || error) {
	  fed9Uwait(500000);
	}
      } while (busy || error);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::setTtcrxControls."));
    }
  }

  /**
   * Performs the reads to the registers on the TTCrx chip.
   */
  u32 Fed9UTtcrxDevice::getTtcrxControls(const u32 regAddr) const {
    try {
      ICUTILS_VERIFY(regAddr < 32)(regAddr).error().msg("The register address is invalid.");
      //These two bools are returned as true from the FED if there is an error in the read process.
      bool busy  = false;
      bool error = false;
      u32 timingData;
      //As the register address is in the 2nd element there must be a 1st element
      //where some dummy data is passed.
      std::vector<u32> regAddress(2,0);

      regAddress[1] = regAddr;
      //Passing a true to theFed will cause it to read.
      const bool read    = true;
      //Controls the number of time that the software wil try to write to the TTC if a busy or error signal is returned.
      const u32 maxLoops = 100;
      u32 loopCount      = 0;
      do {
	theFed.beCommandTtcrxControl(read, regAddress, timingData, busy, error);
	//If the write still fails after a set number of attempts then
	//the software gives up and throws an error.
	ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error)(loopCount)(maxLoops).error().msg("If busy = 1 then the TTCrx device is busy. If error = 1 then the write failed");
	//If the write fails there is a pause before it tries again.
	if (busy || error) {
	  fed9Uwait(500000);
	}
	loopCount++;
      } while (busy || error);
      return timingData;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UTTCRXDEVICE, "error in Fed9UTtcrxDevice::getTtcrxControls."));
    }
  }

}//End of Fed9U namespace
