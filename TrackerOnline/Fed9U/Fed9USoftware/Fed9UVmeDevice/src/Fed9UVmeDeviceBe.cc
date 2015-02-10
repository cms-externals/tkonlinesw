#include <inttypes.h>
#include <stdint.h>
#include "Fed9UVmeDevice.hh"
#include "Fed9UTtcrxDevice.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UAddress.hh"
#include "Fed9UTtcrxDescription.hh"
#include "Fed9UTtcrxDescriptionInfo.hh"
#include "Fed9UWait.hh"



namespace Fed9U {
  using std::ostringstream;

  /*********************************************************************************************
   *                 These are the commands for the Back End (BE) FPGA                            *
   ************************************************************************************************/

  /**
   * This allows you to set the trigger source for the BE FPGA.
   *
   * The values of the possible settings are defined in the Fed9UTrigSource enum
   * that can be found in TypeDefs.hh. The possible settings are TTC, backplane
   * or software triggers.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setTriggerSource(Fed9UTrigSource triggerSource) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(triggerSource==FED9U_TRIG_TTC || triggerSource==FED9U_TRIG_BACKPLANE || triggerSource==FED9U_TRIG_SOFTWARE)(triggerSource).error().msg("An invalid value for the trigger source has been passed");

      theFed->beCommandTriggerSelect(WRITE, triggerSource, dummyWord);

      theLocalFedSettings.setTriggerSource(triggerSource);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTriggerSource."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns the trigger source for the BE FPGA.
   *
   * The values of the possible settings are defined in the Fed9UTrigSource enum
   * that can be found in TypeDefs.hh. The possible settings are TTC, backplane
   * or software triggers.
   */
  Fed9UTrigSource Fed9UVmeDevice::getTriggerSource() const throw (Fed9UVmeDeviceException) {
    try {
      u32 fedTriggerSource;
      theFed->beCommandTriggerSelect(READ, dummyWord, fedTriggerSource);
      //Here the trigger source is cast to the Fed9UTrigSource enum before being returned.
      return static_cast<Fed9UTrigSource>(fedTriggerSource);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTriggerSource."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This will load all the read and write setting from the Fed9UTtcrxDescription
   * object to the TTCrx.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setTtcrx(const Fed9UTtcrxDescription& ttcrxDescription) throw (Fed9UVmeDeviceException) {
    try {
      //Creates the TTCrx device to be initialised.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, ttcrxDescription);


      //Calls each read/write command loading the values passed from ttcrxDescription initialised during the constructor.
      ttcrxDevice.setCounterReset( ttcrxDescription.getCounterReset() );
      ttcrxDevice.setL1AcceptCoarseDelay( ttcrxDescription.getL1AcceptCoarseDelay() );
      ttcrxDevice.setBrcstStrTwoCoarseDelay( ttcrxDescription.getBrcstStrTwoCoarseDelay() );
      ttcrxDevice.setClockDesOneFineDelay( ttcrxDescription.getClockDesOneFineDelay() );
      ttcrxDevice.setClockDesTwoFineDelay( ttcrxDescription.getClockDesTwoFineDelay() );
      ttcrxDevice.setDllPumpCurrent( ttcrxDescription.getDllPumpCurrent() );
      ttcrxDevice.setPllPumpCurrent( ttcrxDescription.getPllPumpCurrent() );
      ttcrxDevice.setIacId( ttcrxDescription.getIacId() );
      ttcrxDevice.setI2cId( ttcrxDescription.getI2cId() );
      ttcrxDevice.setHammingCheckingDisable( ttcrxDescription.getHammingCheckingDisable() );
      // Note that the following set of commands all share one byte of data which is written into one register, therefore we only need to perform a write on one of these registers
      // in order to write to all of them, therfore I have commented out all but one, also note that I have set the value specifically in the serialBDisable reg to disable serial B
      // this is important since serialB is enabled explicitly when the fed is started!!!
      //ttcrxDevice.setCounterOperation( ttcrxDescription.getCounterOperation() );
      //ttcrxDevice.setDeskewedClock2Selected( ttcrxDescription.getDeskewedClock2Selected() );
      //ttcrxDevice.setDeskewedClock2Disable( ttcrxDescription.getDeskewedClock2Disable() );
      //ttcrxDevice.setClockL1AcceptDisable( ttcrxDescription.getClockL1AcceptDisable() );
      //ttcrxDevice.setNonDeskewedClockDisable( ttcrxDescription.getNonDeskewedClockDisable() );
      //ttcrxDevice.setParrallelOutputDisable( ttcrxDescription.getParrallelOutputDisable() );
      ttcrxDevice.setSerialBDisable( true ); // explicitly disable serial B data during initialization, note that this is important and must only be set when the fed is ready for the run

      //TODO, the above methods should all be put into one init method which we can call on the TTCrx Device.
      theLocalFedSettings.setTtcrx( ttcrxDevice.getTtcrxDescription() );
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTtcrx."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  Fed9UVmeDevice& Fed9UVmeDevice::setTtcrxSerialBDataEnable(bool value) throw (Fed9UVmeDeviceException) {
    try {
      //Creates the TTCrx device to be initialised.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, theLocalFedSettings.getTtcrx());
      ttcrxDevice.setSerialBDisable( !value ); // explicitly disable serial B data during initialization, note that this is important and must only be set when the fed is ready for the run
      return * this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTtcrxSerialBDataEnable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
    /**
   * Returns all the current read and writable settings on the TTCrx device.
   */
  Fed9UTtcrxDescription Fed9UVmeDevice::getTtcrx() const throw (Fed9UVmeDeviceException) {
    try {
      //Create a Fed9UTtcrxDevice to read the TTCrx settings back from the FED with.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, theLocalFedSettings.getTtcrx());

      //Create a Fed9UTtcrxDescriptionInfo and initialise with the current FED settings.
      Fed9UTtcrxDescriptionInfo ttcrxDescription(Fed9UTtcrxCounterReset(false,false,false,false),
						 ttcrxDevice.getL1AcceptCoarseDelay(),
						 ttcrxDevice.getBrcstStrTwoCoarseDelay(),
						 ttcrxDevice.getClockDesOneFineDelay(),
						 ttcrxDevice.getClockDesTwoFineDelay(),
						 ttcrxDevice.getDllPumpCurrent(),
						 ttcrxDevice.getPllPumpCurrent(),
						 ttcrxDevice.getIacId(),
						 ttcrxDevice.getI2cId(),
						 ttcrxDevice.getCounterOperation(),
						 ttcrxDevice.getHammingCheckingDisable(),
						 ttcrxDevice.getDeskewedClock2Selected(),
						 ttcrxDevice.getDeskewedClock2Disable(),
						 ttcrxDevice.getClockL1AcceptDisable(),
						 ttcrxDevice.getParrallelOutputDisable(),
						 ttcrxDevice.getSerialBDisable(),
						 ttcrxDevice.getNonDeskewedClockDisable());

      return ttcrxDescription;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcrx."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns all the current settings on the TTCrx device. Both the read only and read
   * and writable settings.
   */
  Fed9UTtcrxDescriptionInfo Fed9UVmeDevice::getTtcrxInfo() const throw (Fed9UVmeDeviceException) {
    try {
      //Create a Fed9UTtcrxDevice to read the TTCrx settings back from the FED with.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, theLocalFedSettings.getTtcrx());

      //Create a Fed9UTtcrxDescriptionInfo and initialise with the current FED settings.
      Fed9UTtcrxDescriptionInfo ttcrxDescriptionInfo(Fed9UTtcrxCounterReset(false,false,false,false),
						     ttcrxDevice.getL1AcceptCoarseDelay(),
						     ttcrxDevice.getBrcstStrTwoCoarseDelay(),
						     ttcrxDevice.getClockDesOneFineDelay(),
						     ttcrxDevice.getClockDesTwoFineDelay(),
						     ttcrxDevice.getDllPumpCurrent(),
						     ttcrxDevice.getPllPumpCurrent(),
						     ttcrxDevice.getIacId(),
						     ttcrxDevice.getI2cId(),
						     ttcrxDevice.getCounterOperation(),
						     ttcrxDevice.getHammingCheckingDisable(),
						     ttcrxDevice.getDeskewedClock2Selected(),
						     ttcrxDevice.getDeskewedClock2Disable(),
						     ttcrxDevice.getClockL1AcceptDisable(),
						     ttcrxDevice.getParrallelOutputDisable(),
						     ttcrxDevice.getSerialBDisable(),
						     ttcrxDevice.getNonDeskewedClockDisable(),
						     ttcrxDevice.getBunchCounter(),
						     ttcrxDevice.getEventCounter(),
						     ttcrxDevice.getSingleBitErrorCount(),
						     ttcrxDevice.getDoubleBitErrorCount(),
						     ttcrxDevice.getSeuErrorCount(),
						     ttcrxDevice.getMasterModeA(),
						     ttcrxDevice.getMasterModeB(),
						     ttcrxDevice.getTestOutputs(),
						     ttcrxDevice.getFreqDetectCount(),
						     ttcrxDevice.getConfigSettings(),
						     ttcrxDevice.getStatusRegister());

      return ttcrxDescriptionInfo;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcrxInfo."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Will reset the TTCrx chip.
   *
   * An internal reset uses the I2C interface to send the reset, an external reset
   * does not. Both perform the same level of reset. You cannot reset the TTCrx while
   * the TTC clock is selected. If internal is true then an internal reset will be
   * sent. If it is false then an external reset will be sent.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendTtcrxReset(bool internal) throw (Fed9UVmeDeviceException) {
    try {
      //Create a TTCrx device and then call the reset.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, Fed9UTtcrxDescription() );
      ttcrxDevice.sendTtcrxReset(internal);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendTtcrxReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This command can be used to alter the settings in the test register.
   *
   * The argument passed is just the decimal representation of the bits in the register.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setTestRegister(u32 testSettings) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandTestReg(WRITE, testSettings, dummyWord);
      theLocalFedSettings.setTestRegister(testSettings);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTestRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This command will return the decimal equivalent of the number represented by the bits in this register.
   */
  u32 Fed9UVmeDevice::getTestRegister() const throw (Fed9UVmeDeviceException) {
    try {
      u32 apvFrameEnable;
      theFed->beCommandTestReg(READ, dummyWord, apvFrameEnable);
      return apvFrameEnable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTestRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Disables a FE FPGA.
   *
   * A true will enable a FE FPGA and false will disable it. If a FE FPGA is disabled
   * then a soft reset of the BE FPGA is performed. See BE FPGA soft reset documentation
   * for details of its effect.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFedFeUnitDisable(const Fed9UAddress& feFpga, bool feFpgaDisable) throw (Fed9UVmeDeviceException) {
    try {
      if (feFpga.getFedFeUnit() == Fed9UAddress::FEBROADCAST) {
	//This method is a write to the BE FPGA so in order to mick the effects of setting
	//all the FE FPGAs this option has to be used.
	theFed->beCommandFeEnableReg(WRITE, (feFpgaDisable ? 0x00:0xff), dummyWord);

	Fed9UAddress addr;
	addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
	theLocalFedSettings.setFedFeUnitDisable(addr, feFpgaDisable);
      } else {
	u32 feFpgaEnable   = 0;
	u32 feFpgaEnable_i = 0;
	u16 feFpga_ = feFpga.getFedFeUnit();

	for (u32 i = 0; i < FEUNITS_PER_FED; i++) {
	  Fed9UAddress addr;
	  addr.setFedFeUnit(i);
	  if (feFpga_ == i) {
	    feFpgaEnable_i = static_cast<u32>(!feFpgaDisable);
	  } else {
	    feFpgaEnable_i = static_cast<u32>( !(theLocalFedSettings.getFedFeUnitDisable(addr)) );
	  }
	  feFpgaEnable_i <<= i;
	  feFpgaEnable    |= feFpgaEnable_i;
	}
	theFed->beCommandFeEnableReg(WRITE, feFpgaEnable, dummyWord);
	theLocalFedSettings.setFedFeUnitDisable(feFpga, feFpgaDisable);
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFedFeUnitDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns a true if a FE FPGA is disabled and a false if the FE FPGA is enabled.
   */
  bool Fed9UVmeDevice::getFedFeUnitDisable(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      bool feFpgaDisable = 0;
      u32  feFpgaEnable  = 0;
      u16  feFpga_ = feFpga.getFedFeUnit();

      theFed->beCommandFeEnableReg(READ, dummyWord, feFpgaEnable);

      feFpgaEnable >>= feFpga_;
      feFpgaEnable  = feFpgaEnable & 0x01;
      feFpgaDisable = !feFpgaEnable;

      return feFpgaDisable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFedFeUnitDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This allows the user to select the output data route.
   *
   * The values of the Fed9UReadRoute enum can be found in TypeDefs.hh.
   * The settings for the read route is either the S-LINK or the VME-LINK.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setBeFpgaReadRoute(Fed9UReadRoute readRoute) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(readRoute==FED9U_ROUTE_SLINK64 || readRoute==FED9U_ROUTE_VME || readRoute == FED9U_ROUTE_NONE)(readRoute).error().msg("The readout route is invalid.");

      theFed->beCommandBeReadoutControl(WRITE, readRoute, dummyWord);

      theLocalFedSettings.setBeFpgaReadRoute(readRoute);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setBeFpgaReadRoute."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns the output data route.
   *
   * The values of the Fed9UReadRoute enum can be found in TypeDefs.hh.
   * The settings for the read route is either the S-LINK or the VME-LINK.
   */
  Fed9UReadRoute Fed9UVmeDevice::getBeFpgaReadRoute() const throw (Fed9UVmeDeviceException) {
    try {
      u32 fedReadRoute;
      theFed->beCommandBeReadoutControl(READ, dummyWord, fedReadRoute);
      return static_cast<Fed9UReadRoute>(fedReadRoute);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBeFpgaReadRoute."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This will disable the BE FPGA preventing it from seeing both triggers and frames.
   *
   * A true will disable the BE FPGA and a false will enable the BE FPGA.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFedBeFpgaDisable(bool fedDisable) throw (Fed9UVmeDeviceException) {
    try {
      u32 fedEnable = (fedDisable ? 0:3);
      theFed->beCommandBeRunControl(WRITE, fedEnable, dummyWord);
      theLocalFedSettings.setFedBeFpgaDisable(fedDisable);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFedBeFpgaDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * If a true is returned then the BE FPGA is disabled. If a false is returned then
   * the BE FPGA is enabled.
   *
   * If the BE FPGA is disabled then it will ignore both triggers and frames.
   */
  bool Fed9UVmeDevice::getFedBeFpgaDisable() const throw (Fed9UVmeDeviceException) {
    try {
      bool fedDisable;
      u32 fedEnable;
      theFed->beCommandBeRunControl(READ, dummyWord, fedEnable);

      ICUTILS_VERIFY(fedEnable==0 || fedEnable==3)(fedEnable).error().msg("The FED has return an invalid state.");

      //the verify above will ensure that only valid values reach here. The state should either be 0 or 3 (11 in binary).
      fedDisable = ( (fedEnable == 0) ? true : false );

      return fedDisable;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFedBeFpgaDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This allows the user to set the FED ID in the DAQ header.
   *
   * The FED ID is a number unique to a FED and allows the DAQ to identify
   * which FED data came from. It has a maximum value of 4095.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFedId(u16 fedId) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(fedId < 4096)(fedId).error().msg("An invalid value for the FED Id has been passed.");

      theFed->beCommandFedIdReg(WRITE, fedId, dummyWord);
      theLocalFedSettings.setFedId(fedId);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFedId."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns the FED ID in the DAQ header.
   *
   * The FED ID is a number unique to a FED and allows the DAQ to identify
   * which FED data came from.
   */
  u16 Fed9UVmeDevice::getFedId() const throw (Fed9UVmeDeviceException) {
    try {
      u32 fedId;
      theFed->beCommandFedIdReg(READ, dummyWord, fedId);
      return fedId;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFedId."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Generates a software trigger.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendSoftwareTrigger() throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandSoftwareTrigger();
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendSoftwareTrigger."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This performs a soft reset on the BE FPGA.
   *
   * It does not affect the DCMs and control registers. It does reset the event counters.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::beFpgaSoftReset() throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandSoftReset();
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::beFpgaSoftReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * The return value holds data from various sections.      
   */
  u32 Fed9UVmeDevice::getBeDebugRegister() const throw (Fed9UVmeDeviceException) {
    try {
      u32 debugReg(0);
      //note that this register is only available after BE firmware 0x220003CD
      if (theLocalFedSettings.getBeFirmwareVersion() >= 0x220003CD) 
	theFed->beCommandDebugRegister(debugReg);
      
      return debugReg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDebugRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
  * The return value holds data from various sections.      
  */
  u32 Fed9UVmeDevice::getBeExtraEventRegister() const throw (Fed9UVmeDeviceException) {
    try {
      u32 extraEvent(0);
      //note that this register is only available after BE firmware 0x220003CC
      if (theLocalFedSettings.getBeFirmwareVersion() >= 0x220003CC)
	theFed->beCommandExtraEventRegister(extraEvent);
      return extraEvent;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getExtraEventRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * The return value holds data from various sections.
   */
  u32 Fed9UVmeDevice::getBeStatusRegister() const throw (Fed9UVmeDeviceException) {
    try {
      u32 beStatus;
      theFed->beCommandBeStatusRegister(beStatus);
      return beStatus;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBeStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the number of triggers that the FED has received since last reset (BE FPGA or FED reset).
   */
  u32 Fed9UVmeDevice::getTriggerNumber() const throw (Fed9UVmeDeviceException) {
    try {
      u32 triggerNumber;
      theFed->beCommandReadL1aCount(triggerNumber);
      return triggerNumber;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTriggerNumber."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns how many bunch crossings occurred within the FED.
   */
  u32 Fed9UVmeDevice::getBunchCount() const throw (Fed9UVmeDeviceException) {
    try {
      u32 bunchCount;
      theFed->beCommandReadBxCount(bunchCount);
      return bunchCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBunchCount."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the number of frames that are held in the QDR memory at that instant in time.
   */
  u32 Fed9UVmeDevice::getQdrFrameCount() const throw (Fed9UVmeDeviceException) {
    try {
      u32 frameCount;
      theFed->beCommandQdrFrameCounter(frameCount);
      return frameCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getQdrFrameCount."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the status of the TTC chip.
   *
   * 1 ready
   * 0 not ready
   */
  bool Fed9UVmeDevice::getTtcrxStatus() const throw (Fed9UVmeDeviceException) {
    try {
      u32 ttcrxStatus;
      //The TTC ready bit is in the BE status register.
      theFed->beCommandBeStatusRegister(ttcrxStatus);
      //Now get the bit 6 from this number.
      return static_cast<bool>((ttcrxStatus & 64) >> 6);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the current version of the firmware that is being used by the BE FPGA.
   */
  u32 Fed9UVmeDevice::getBeFirmwareId() const throw (Fed9UVmeDeviceException) {
    try {
      u32 firmwareVersion;
      theFed->beCommandFirmwareIdReg(firmwareVersion);
      return firmwareVersion;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFirmwareVersion."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the current number of 64 bit word there are in the QDR buffer.
   */
  u32 Fed9UVmeDevice::getQdrBufferLevel() const throw (Fed9UVmeDeviceException) {
    try {
      u32 bufferLevel;
      theFed->beCommandQdrDataCounter(bufferLevel);
      return bufferLevel;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getQdrBufferLevel."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the total number of frames that have been held in the QDR memory since last reset (BE FPGA or
   * FED reset).
   */
  u32 Fed9UVmeDevice::getQdrTotalFrameCount() const throw (Fed9UVmeDeviceException) {
    try {
      u32 totalFrameCount;
      theFed->beCommandTotalFrameCount(totalFrameCount);
      return totalFrameCount;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getQdrTotalFrameCount."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  //Matthew Pearson April 2005 - new BE commands for FEDv2.                                                                                              
  /**Returns the DAQ register 32 bit number.                                                                                   
     This number is written to the FED data buffer header when in full debug mode.*/
  u32 Fed9UVmeDevice::getDaqRegister(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 daqReg = 0;
      theFed->beCommandDaqRegister(READ, dummyWord, daqReg);
      return daqReg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Sets the DAQ register 32 bit number.                                                                                           
     This number is written to the FED data buffer header when in full debug mode.*/
  Fed9UVmeDevice& Fed9UVmeDevice::setDaqRegister(u32 daqReg) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandDaqRegister(WRITE, daqReg, dummyWord);
      //theLocalFedSettings.setDaqRegister(daqReg);//To be commented out when implemented in Fed9UDescription.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the DAQ register Two 32 bit number.
     This number is written to the FED data buffer header when in full debug mode.*/
  u32 Fed9UVmeDevice::getDaqRegisterTwo(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 daqReg = 0;
      theFed->beCommandDaqRegisterTwo(READ, dummyWord, daqReg);
      return daqReg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
  /**Sets the DAQ register Two 32 bit number.
     This number is written to the FED data buffer header when in full debug mode.*/
  Fed9UVmeDevice& Fed9UVmeDevice::setDaqRegisterTwo(u32 daqReg) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandDaqRegisterTwo(WRITE, daqReg, dummyWord);
      //theLocalFedSettings.setDaqRegister(daqReg);//To be commented out when implemented in Fed9UDescription.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
  /**Returns the status of the requested buffer from the BE status register.
     Uses the enum Fed9UFifoStatus to return a value according to whether the buffer is empty, partially full or full.
     Currently the default is to set it to empty.
     It also uses the enum Fed9UFifoType to identify which buffer status is being requested.*/
  // JEC 23-02-06
  // JEC 04-04-06: added NOT_EMPTY state to cover normal running conditions for some FIFOs
  // JEC 20-11-06: added PARTIAL_EMPTY and HALF_FULL states from second BE status register.
  //               Left the NOT_EMPTY state there for the moment - can this happen now ?
  Fed9UFifoStatus Fed9UVmeDevice::getFifoStatus(Fed9UFifoType fifoId) const throw (Fed9UVmeDeviceException) {
    try {
      u32 beStatus = 0;
      theFed->beCommandBeStatusRegister(beStatus);
      vector<u32> beStatusReg2;
      if (getBeFirmwareId() >= 0x2200037F) {
	beStatusReg2.resize(1,0);
	theFed->beCommandLongReadRegister(beStatusReg2[0]);
      } else {
	theFed->beCommandLongReadRegister(beStatusReg2);
      }
      u32 beStatus2 = 0;
      if (beStatusReg2.size() == 1) {beStatus2 = beStatusReg2[0];}
      else {beStatus2 = beStatusReg2[1];}
      
      // Identify which bit in beStatus corresponds to the full flag for the requested FIFO
      u8 fullbit = (static_cast<u8>(fifoId)) + 8;
      // Identify which bit in beStatus2 corresponds to the partial empty flag for the requested FIFO
      u8 pemptybit = (static_cast<u8>(fifoId)) + 16;
      
      // assume it is empty as default
      Fed9UFifoStatus result = FED9U_FIFO_EMPTY;
      // check empty flag
      if ((beStatus >> (fullbit+16)) & 0x1) {result = FED9U_FIFO_EMPTY;}
      // check partial flag
      if ((beStatus >> (fullbit+8)) & 0x1)  {result = FED9U_FIFO_PARTIAL;}
      // check full flag
      if ((beStatus >> fullbit) & 0x1) {result = FED9U_FIFO_FULL;}
      // check partial empty flag from second status register
      if ((beStatus2 >> pemptybit) & 0x1) {result = FED9U_FIFO_PARTIAL_EMPTY;}
      // check half full empty flag from second status register
      if ((beStatus2 >> (pemptybit+8)) & 0x1) {result = FED9U_FIFO_HALF_FULL;}
      
      // If result is still empty, but empty flag is zero then none of the bits were set
      // assume this to be the normal conditions for data-taking
      if ((result ==  FED9U_FIFO_EMPTY) && (!((beStatus >> (fullbit+16)) & 0x1))) {result = FED9U_FIFO_NOT_EMPTY;}
      
      return result;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFifoStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the status of the Tracker Header Monitor data from the BE status register.*/
  // JEC 24-02-06
  bool Fed9UVmeDevice::getTrackerHeaderMonitorStatus(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg = 0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg >> 7) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTrackerHeaderMonitorStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns an error flag if the internal bunch crossing counter and the value from the TTC system do not match
     at orbit reset.*/
  // JEC 27-02-06
  bool Fed9UVmeDevice::getBunchCrossingAtOrbitReset(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg = 0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg >> 5) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBunchCrossingAtOrbitReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  } 
  
  /**Returns the S-LINK back pressure flag from the BE Status register.*/
  bool Fed9UVmeDevice::getSlinkBackPressure(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg & 0x10) >> 4);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSlinkBackPressure."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the S-LINK link full flag from the BE Status register.*/
  bool Fed9UVmeDevice::getSlinkFull(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg & 0x8) >> 3);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSlinkFull."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the S-LINK link down flag from the BE Status register.*/
  bool Fed9UVmeDevice::getSlinkDown(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg & 0x4) >> 2);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSlinkDown."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the internal freeze flag. This flag indicated if the FED trigger FIFOs have                                            
     overflowed. The FED will be frozen for debugging if positive.*/
  bool Fed9UVmeDevice::getTriggerFifoOverflow(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>((reg & 0x2) >> 1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTriggerFifoOverflow."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the over temperature interrupt signal flag.*/
  bool Fed9UVmeDevice::getTempInterrupt(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBeStatusRegister(reg);
      return static_cast<bool>(reg & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTempInterrupt."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  //<JEC date=09/01/2006>
  //Added tracker header type.
  Fed9UHeaderFormat Fed9UVmeDevice::getHeaderFormatType(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandHeaderFormatType(READ, dummyWord, reg);
      return static_cast<Fed9UHeaderFormat>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getHeaderFormatType."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setHeaderFormatType(Fed9UHeaderFormat headerType) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(headerType==FED9U_HEADER_FULLDEBUG || headerType==FED9U_HEADER_APVERROR)(headerType).error().msg("The header format is invalid.");
      theFed->beCommandHeaderFormatType(WRITE, headerType, dummyWord);
      theLocalFedSettings.setHeaderFormatType(headerType);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setHeaderFormatType."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  //<JEC date=23/02/06>
  //Added BunchCrossingOffset
  Fed9UVmeDevice& Fed9UVmeDevice::setBunchCrossingOffset(u16 bxOffset) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandBunchCrossingOffset(WRITE, static_cast<u32>(bxOffset), dummyWord);
      theLocalFedSettings.setBunchCrossingOffset(bxOffset);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setBunchCrossingOffset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u16 Fed9UVmeDevice::getBunchCrossingOffset(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBunchCrossingOffset(READ, dummyWord, reg);
      return static_cast<u16>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBunchCrossingOffset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  //<JEC date=08/12/2005>
  //Added eventType
  Fed9UVmeDevice& Fed9UVmeDevice::setDaqEventType(u16 eventType) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandDaqEventType(WRITE, static_cast<u32>(eventType), dummyWord);
      theLocalFedSettings.setDaqEventType(eventType);                            
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqEventType."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u16 Fed9UVmeDevice::getDaqEventType(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandDaqEventType(READ, dummyWord, reg);
      return static_cast<u16>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqEventType."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>


  //<JEC date=08/12/2005>
  //Added DaqFov to description
  Fed9UVmeDevice& Fed9UVmeDevice::setDaqFov(u16 fov) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandDaqFov(WRITE, static_cast<u32>(fov), dummyWord);
      theLocalFedSettings.setDaqFov(fov);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqFov."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u16 Fed9UVmeDevice::getDaqFov(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandDaqFov(READ, dummyWord, reg);
      return static_cast<u16>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqFov."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>


  // <JEC date="1/12/2006"> set for bunch crossing orbit total added
  Fed9UVmeDevice& Fed9UVmeDevice::setBunchCrossingOrbitTotal(u16 orbitTotal) throw (Fed9UVmeDeviceException) {
    try {
      theFed->beCommandBunchCrossingOrbitTotal(WRITE, static_cast<u32>(orbitTotal), dummyWord);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setBunchCrossingOrbitTotal."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  /**Returns the bunch crossing number total for each LHC orbit. This is a read                                                            
     only register. It should return the same value each orbit. It is a 12-bit number.*/
  u16 Fed9UVmeDevice::getBunchCrossingOrbitTotal(void) throw (Fed9UVmeDeviceException) {
    try {
      u32 reg=0;
      theFed->beCommandBunchCrossingOrbitTotal(READ, dummyWord, reg);
      return static_cast<u16>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBunchCrossingOrbitTotal."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**This command can be used to alter the settings in the FMM test register.
     The argument passed is just the decimal representation of the bits in the register.*/
  // JEC 8/3/06
  Fed9UVmeDevice& Fed9UVmeDevice::setFmmTestRegister(u16 testSettings) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getBeFirmwareId()>=0x2200037B)(getBeFirmwareId()).error().msg("FMM test register does not exist in this firmware version.");
      // First check to see if the FMM test register is enabled (check bit 5)
      bool testRegEnabled = false;
      while (!testRegEnabled) {
	u32 testReg = theLocalFedSettings.getTestRegister();
	testRegEnabled = static_cast<bool>((testReg >> 4) & 0x1);
	if (!testRegEnabled) {
	  // Set bit 5 equal to 1 to enable the FMM test register
	  u32 testSettings = 16;
	  theLocalFedSettings.setTestRegister(testSettings);
	}
      }
      ICUTILS_VERIFY(testRegEnabled)(testRegEnabled).error().msg("The FMM test register has not been enabled.");
      
      // write to the register
      theFed->beCommandFmmTestReg(WRITE, testSettings, dummyWord);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFmmTestRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the contents of the FMM test register.  It is a 4-bit number which is used to test the signals to the FMM
     manually.  This register is enabled via bit 5 of the main test register, but should not be enabled during normal
     operation.*/
  // JEC 8/3/06
  u16 Fed9UVmeDevice::getFmmTestRegister(void) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getBeFirmwareId()>=0x2200037B)(getBeFirmwareId()).error().msg("FMM test register does not exist in this firmware version.");
      // First check to see if the FMM test register is enabled (check bit 5)
      bool testRegEnabled = false;
      while (!testRegEnabled) {
	u32 testReg = theLocalFedSettings.getTestRegister();
	testRegEnabled = static_cast<bool>((testReg >> 4) & 0x1);
	if (!testRegEnabled) {
	  // Set bit 5 equal to 1 to enable the FMM test register
	  u32 testSettings = 16;
	  theLocalFedSettings.setTestRegister(testSettings);
	}
      }
      ICUTILS_VERIFY(testRegEnabled)(testRegEnabled).error().msg("The FMM test register has not been enabled.");
      
      u32 reg=0;
      theFed->beCommandFmmTestReg(READ, dummyWord, reg);
      return static_cast<u16>(reg);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFmmTestRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  //<JEC date=16/03/07>
  Fed9UVmeDevice& Fed9UVmeDevice::setBEFrameCapturePrescale(u16 prescale) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getBeFirmwareId()>=0x22000383)(getBeFirmwareId()).error().msg("BE trigger scale down register does not exist in this firmware version.");
      // write to the register
      // whether prescale is the actual prescale factor or the number of events that should be skipped is taken
      // care of in the higher-level method
      theFed->beCommandFrameCapturePrescale(WRITE, prescale, dummyWord);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setBEFrameCapturePrescale."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  //<JEC date=16/03/07>
  u16 Fed9UVmeDevice::getBEFrameCapturePrescale(void) const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getBeFirmwareId()>=0x22000383)(getBeFirmwareId()).error().msg("BE trigger scale down register does not exist in this firmware version.");
      u32 reg;
      theFed->beCommandFrameCapturePrescale(READ, dummyWord, reg);
      // return the prescale factor, not the number of events that should be skipped
      return reg+1;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBEFrameCapturePrescale."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  
  /**Returns all the tracker header monitor information. The tracker header monitor for                                         
     each FE contains the status words for each fibre in that FE and the majority APV pipeline address.                    
     The method returns a vector which has 13 elements. Elements 1-12 contain the fibre status words                          
     for fibres 1-12 respectively. Element 13 contains the majority APV pipeline address for that FE.                       
     The method expects a u8 input from 1-8 to select the front end unit.                                                    
     NOTE: this method internally checks the tracker header monitor status flag which is                                    
     in the DAQ register before reading the tracker header monitor. An exception is thrown                              
     if the data is not ready to be read.*/
   vector<u8> Fed9UVmeDevice::getTrackerHeaderMonitor(u16 feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      cout << "feFpga = "<< feFpga << endl;
      //First make sure the register is ready for reading, by checking the tracker
      //header monitor data ready bit in the BE status register.
      bool readyFlag = this->getTrackerHeaderMonitorStatus();
      ICUTILS_VERIFY(readyFlag)(readyFlag).error().msg("The tracker header monitor data is not ready to be read.");

      //Now read the tracker header monitor
      vector<u32> reg;
      theFed->beCommandTrackerHeaderMonitor(reg);
      //Now extract the appropriate bits according to the fpga number input. 
      //We have to convert the fpga number to firmware format here.
      u8 firmwareFeFpga = feFpga;
      //Get the correct tracker header monitor for that FE FPGA.
      //      u32 bitPosition = 80 * (8-firmwareFeFpga);
      //      u32 regElement = static_cast<u32>(static_cast<float>(bitPosition) / 32.0); //The first vector element to read from.
      //      u32 startPosInRegElement = bitPosition % 32; //The bit within the first vector element.
      //      u32 regElementInc = 0;
      vector<u8> packet;
      //      u16 inc = startPosInRegElement; //Set the increment variable equal to the intial start bit position.
      //Hopefully this works! 
      //      for (int i=0; i<=11; ++i) {
      //	if ((inc+6)>31) {
      //	  packet.push_back(((reg[regElement+regElementInc] & ((0xffffffff >> (32-inc)) << startPosInRegElement+inc)) >> startPosInRegElement+inc) | ((reg[regElement+regElementInc+1] & (0xffffffff >> ((inc+6)-32))) << (32-inc)));
      //	  inc = (inc+6)-32;
      //	  ++regElementInc;
      //	  startPosInRegElement = 0;
      //	}
      //	else {
      //	  packet.push_back((reg[regElement+regElementInc] & (0x3f << startPosInRegElement+inc)) >> startPosInRegElement+inc);
      //	  inc+=6;
      //	}
      //      }
      //Now get the majority APV pipeline address from the subsequent 8 bits.
      //      packet.push_back((reg[regElement+regElementInc] & (0xff << inc)) >> inc);

      // JEC 6/3/06:  Alternative method of extracting the status words
      // Get the bit associated with the start of status word 1
      u32 bitPosition = 80 * (8 - firmwareFeFpga);

      // Loop over the 12 status words associated to the FE-FPGA
      for (int statuswd = 0; statuswd < 12; statuswd++) {
      // Find the first vector element you want to read from
      	u32 regElement = static_cast<u32>(static_cast<float>(bitPosition)/32.0);
      // Find the starting position within that vector element
      	u32 startPosition = bitPosition % 32;
      	if ((startPosition+6) > 31) {
	  packet.push_back(((reg[regElement] >> startPosition) & (0xFFFFFFFF >> startPosition)) || ((reg[regElement+1] & (0xFFFFFFFF >> (64 - (startPosition + 6)))) << (32 - startPosition)));
      	} else {
      	  packet.push_back((reg[regElement] >> startPosition) & 0x3F);
      	}
      	bitPosition+=6;
      }
      // Now get the majority APV pipeline address from the subsequent 8 bits
      u32 regElement = static_cast<u32>(static_cast<float>(bitPosition)/32.0);
      u32 startPosition = bitPosition % 32;
      packet.push_back((reg[regElement] >> startPosition) & 0xFF);
      // JEC end of alternative method
      
      //Return the vector
      return packet;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTrackerHeaderMonitor."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u32 Fed9UVmeDevice::getL1AcceptID(void) const throw (Fed9UVmeDeviceException) {
    try {
      //<GR date=18/08/2006>
      //Changed this to check from the BE status register instead if DAQ register as in commend.
      //Also masking bit 6 and then converting bit 7 to a bool!
      //First make sure the register is ready for reading, by checking the tracker                                        
      //header monitor data ready bit in the BE status register.                                      
//       u32 daqReg = 0;
//       theFed->beCommandDaqRegister(READ, dummyWord, daqReg);
//       bool readyFlag = static_cast<bool>((daqReg & 0x40) >> 7);
//       ICUTILS_VERIFY(readyFlag)(readyFlag).error().msg("The tracker header monitor data is not ready to be read.");
      
      //Get the tracker header monitor status, as we will not read a valid number uness the tracker header is constructed.
      bool readyFlag = this->getTrackerHeaderMonitorStatus();
      ICUTILS_VERIFY(readyFlag)(readyFlag).error().msg("The tracker header monitor data is not ready to be read.");
      //</GR>

      //Now read the tracker header monitor                                                                            
      vector<u32> reg;
      theFed->beCommandTrackerHeaderMonitor(reg);
      //Extract the L1A ID number from bits 640-663 (24 bits)
      u32 vectorElement = static_cast<u32>(640.0 / 32.0);
      //<GR date=18/08/2006>
      //Change the start bit to be 8th rather than 0th bit and just removed it any calculation is no transparent!
      //Reduced number of operations by moving bits and then masking, rather than moving mask
      //u32 startBit = 640 % 32;
      //u32 id = (reg[vectorElement] & (0xffffff << startBit)) >> startBit;
      u32 id( (reg[vectorElement]>>8)&0xffffff );
      //</GR>
      return id;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getL1AcceptID."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the TTS Read bit.*/
  bool Fed9UVmeDevice::getTTSRead(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (getBeFirmwareId() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      //Get first data from first element of vector.
      //      return static_cast<bool>((reg[0] & 0x8000) >> 15);
      u32 tmpWd = 0;
      if (reg.size() == 1) {tmpWd = reg[0];}
      else {tmpWd = reg[1];}
      return static_cast<bool>((tmpWd >> 15) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTTSRead."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the TTS Warn Overflow bit.*/
  bool Fed9UVmeDevice::getTTSWarnOverflow(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (getBeFirmwareId() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      //Get first data from first element of vector. 
      //      return static_cast<bool>((reg[0] & 0x4000) >> 14);
      u32 tmpWd = 0;
      if (reg.size() == 1) {tmpWd = reg[0];}
      else {tmpWd = reg[1];}
      return static_cast<bool>((tmpWd >> 12) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTTSWarnOverflow."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the TTS Busy bit.*/
  bool Fed9UVmeDevice::getTTSBusy(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (getBeFirmwareId() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      //Get first data from first element of vector.                                                                                  
      //      return static_cast<bool>((reg[0] & 0x2000) >> 13);
      u32 tmpWd = 0;
      if (reg.size() == 1) {tmpWd = reg[0];}
      else {tmpWd = reg[1];}
      return static_cast<bool>((tmpWd >> 14) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTTSBusy."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Returns the TTS Out of Sync bit.*/
  bool Fed9UVmeDevice::getTTSOutOfSync(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (getBeFirmwareId() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      //Get first data from first element of vector.        
      //      return static_cast<bool>((reg[0] & 0x1000) >> 12);
      u32 tmpWd = 0;
      if (reg.size() == 1) {tmpWd = reg[0];}
      else {tmpWd = reg[1];}
      return static_cast<bool>((tmpWd >> 13) & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTTSOutOfSync."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Reads the Bx counter value at LHC orbit from the long_read_1 register.                                                  
     This should be identical to the value read using the getBunchCrossingOrbitTotal() method.*/
  u16 Fed9UVmeDevice::getBunchCrossingOrbitLongRead1(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (getBeFirmwareId() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      //Get first data from first element of vector.                                                                    
      u32 tmpWd = 0;
      if (reg.size() == 1) {tmpWd = reg[0];}
      else {tmpWd = reg[1];}
      return static_cast<u16>(tmpWd & 0xFFF);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBunchCrossingOrbitLongRead1."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setSpyArmCommand(u8 spyArmCommand_)  throw (Fed9UVmeDeviceException) {
    try {
       u32 dummyWord = 0;
       // <NAC date="17/02/2010"> changed && to |
       theFed->beCommandSpyArmTtcBData(WRITE, (0xF100 | (spyArmCommand_>>2)),dummyWord );
       // </NAC>
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setSpyArmCommand()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }


  // <NAC date="25/06/2009"> Added second spy L1A/TotalEvent register commands
  // <JRF date="12/06/2008"> get spy L1a Count
  u32 Fed9UVmeDevice::getSpyTotalEventCount() throw (Fed9UVmeDeviceException) {
    try {
      u32 length = 24;
      if (theLocalFedSettings.getBeFirmwareVersion() >= 0x220003AF) 
        length = 32;
      u32 reg;
      theFed->beCommandSpyTotalEventCount( reg, length );
      return reg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSpyTotalEventCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  // </JRF>
  u32 Fed9UVmeDevice::getSpyL1ACount() throw (Fed9UVmeDeviceException) {
    try {
      u32 length = 24;
      if (theLocalFedSettings.getBeFirmwareVersion() >= 0x220003AF)
        length = 32;

      u32 reg;
      theFed->beCommandSpyL1ACount( reg, length );
      return reg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSpyL1ACount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  // </NAC>
  u32 Fed9UVmeDevice::getTotalEventCount32() throw (Fed9UVmeDeviceException) {
    try {
      u32 reg;
      theFed->beCommandTotalEventCount32( reg );
      return reg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventCount32T()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  // </JRF>                                                                                                                                                                                                                         
  u32 Fed9UVmeDevice::getTotalL1ACount32() throw (Fed9UVmeDeviceException) {
    try {
      u32 reg;
      theFed->beCommandTotalL1ACount32( reg );
      return reg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalL1ACount32()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u32 Fed9UVmeDevice::getTotalEventOutCount() throw (Fed9UVmeDeviceException) {
    try {
      if (getBeFirmwareVersion() >= 0x220003F3) {

         u32 reg;
         theFed->beCommandTotalEventOutCount( reg );
         return reg;
      } else {
        return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventOutCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u32 Fed9UVmeDevice::getFeFrameLengthCount() throw (Fed9UVmeDeviceException) {
    try {
      if (getBeFirmwareVersion() >= 0x220003F3) {

         u32 reg;
         theFed->beCommandFeFrameLengthCount( reg );
         return reg;
      } else {
        return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventOutCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  

  u32 Fed9UVmeDevice::getDummyEventCount() throw (Fed9UVmeDeviceException) {
    try {
      if (getBeFirmwareVersion() >= 0x220003F3) {
         u32 reg;
         theFed->beCommandDummyEventCount( reg );
         return reg;
      } else {
        return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventOutCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u32 Fed9UVmeDevice::getPayloadDownCount() throw (Fed9UVmeDeviceException) {
    try {
      if (getBeFirmwareVersion() >= 0x220003F9) {

	u32 reg;
	theFed->beCommandPayloadDownCount( reg );
	return reg;
      } else {
        return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventOutCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u32 Fed9UVmeDevice::getPayloadLength() throw (Fed9UVmeDeviceException) {
    try {
      if (getBeFirmwareVersion() >= 0x220003F9) {
	u32 reg;
	theFed->beCommandPayloadLength( reg );
	return reg;
      } else {
        return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTotalEventOutCount()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  
  u64 Fed9UVmeDevice::getDebugRegTwo()  throw (Fed9UVmeDeviceException){
    try {
      if (getBeFirmwareVersion() >= 0x220003F6) {
        //cout << "Reading debugRegTwo... " << endl;
         vector<u32> reg; 
         theFed->beCommandDebugRegTwo(reg);
         u64 reg64 = 0;
         //cout << " length of reg = " << reg.size() << " reg[0] = " << reg[0] << " reg[1] = " << reg[1] << endl;
         reg64 = static_cast<u64>(reg[0]);
         reg64 = (reg64 << 20) | reg[1];
         return reg64;
      } else {
         return 0;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcIndividuallyAddressedCommand."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  // JEC 20-11-06
  vector<u32> Fed9UVmeDevice::getSecondBeStatusRegister(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      if (theLocalFedSettings.getBeFirmwareVersion() >= 0x2200037F) {
	reg.resize(1,0);
	theFed->beCommandLongReadRegister(reg[0]);
      } else {
	theFed->beCommandLongReadRegister(reg);
      }
      return reg;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSecondBeStatusRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**Reads the last TTC broadcast frame (short format) decoded from channel B of the TTCrx.
     Broadcast commands are sent to all TTCrx chips in the system.
     It is a 13-bit register which contains an 8-bit data byte and 5-bits for error checking.
     The data byte contains system commands (resync, reset, test_enable and apv_calibrate) as well as
     dedicated bunch counter and event counter reset bits. */
  // JEC method added 16/02/06
  vector<u16> Fed9UVmeDevice::getTtcBroadcastCommand(void) const throw (Fed9UVmeDeviceException) {
    try {
      u32 broadcastCommand;
      theFed->beCommandTtcBroadcastCommand(broadcastCommand);
      vector<u16> broadcast;

      // Extract the error checking from bits 4-0
      broadcast.push_back(broadcastCommand & 0x1F);
      // Extract the command word from bits 12-5
      broadcast.push_back((broadcastCommand & 0x1FE0) >> 5);
      //      u16 commandWd = (broadcastCommand>>5);

      return broadcast;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcBroadcastCommand."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**Reads the last TTC individually addressed command (long format) decoded from channel B of the TTCrx.
     IACs are sent to specific TTCrx chips, specified by the address given as part of the command.
     Can be sent to all TTCrx chips in the system by specifying a special address.
     It is a 39-bit register:
     6-0 = check word (as for TTC Broadcast command)
     14-7 = data/command word (as for TTC Broadcast command)
     22-15 = Sub address word
     23 = 1 always
     24 = 0 external, = 1 internal
     38-25 = TTCrx address word
     Data word has the same content as in the TTC Broadcast command.
     The contents of the register are output in a u16 vector in the same order.*/
  // JEC method added 20/02/06
  vector<u16> Fed9UVmeDevice::getTtcIndividuallyAddressedCommand(void) const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> reg;
      theFed->beCommandTtcIndividuallyAddressedCommand(reg);
      
      vector<u16> IacCmd;
      // Extract the error checking from bits 6-0 of reg[0]
      IacCmd.push_back(reg[0] & 0x7F);
      // Extract the command word from bits 14-7 of reg[0]
      IacCmd.push_back((reg[0] >> 7) & 0xFF);


      // Extract the subaddress word from bits 22-15 of reg[0]
      IacCmd.push_back((reg[0] >> 15) & 0xFF);
      
      // Extract the internal/external flag bit 24 of reg[0]
      IacCmd.push_back((reg[0] >> 24) & 0x1);

      // Finally extract the TTCrx address word - split over reg[0] and reg[1]
      IacCmd.push_back(((reg[0] >> 25) & 0x7F)  | ((reg[1] << 7) & 0x7F));
      
      return IacCmd;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTtcIndividuallyAddressedCommand."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  // JEC 10-04-06
  // JEC 06-10-06:  Modified to include the read-only registers.
  /**Reads the contents of all read/write and read-only BE registers.*/
  void Fed9UVmeDevice::dumpBeRegisters(ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {
      u32 dummyWord = 0;

      vector<string> registerName;
      vector< vector<u32> > outputArgs;
      vector<u32> opArg;
      vector<string> niceOutput;
      vector<bool> showDeci;

      string sop;

      // trigger source
      opArg.resize(1,0);
      theFed->beCommandTriggerSelect(true,dummyWord,opArg[0]);
      if (opArg[0] == 1) {sop = "TTC trigger";}
      else if (opArg[0] == 2) {sop = "BACKPLANE trigger";}
      else if (opArg[0] == 4) {sop = "SOFTWARE trigger";}
      else {sop = "NOT RECOGNISED";}

      registerName.push_back("Trigger select register (designator = 1):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // mode register
      opArg.clear();
      opArg.resize(1,0);
      sop.clear();
      theFed->beCommandModeRegister(true,dummyWord,opArg[0]);
      if (opArg[0] == 1) {sop = "SCOPE mode";}
      else if (opArg[0] == 2 || opArg[0] == 3) {sop = "VIRGIN RAW mode";}
      else if (opArg[0] == 6 || opArg[0] == 7) {sop = "PROCESSED RAW mode";}
      else if (opArg[0] == 10 || opArg[0] == 11) {sop = "ZERO SUPPRESSED mode";}
      else if (opArg[0] == 12 || opArg[0] == 13) {sop = "ZERO SUPPRESSED LITE mode";}
      else {sop = "NOT RECOGNISED";}

      registerName.push_back("Mode register (designator = 2):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // Test register
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandTestReg(true,dummyWord,opArg[0]);
      ostringstream op4;
      ConvertToBinary(op4,opArg,32);
      op4 << std::endl;
      
      registerName.push_back("Test register (designator = 4):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(op4.str());
      showDeci.push_back(false);

      // while we're here, see if the FMM test register is in use
      vector<u32> extraArguments(1,0);
      if (static_cast<bool>((opArg[0] >> 4) & 0x1)) {
      	theFed->beCommandFmmTestReg(true,dummyWord,extraArguments[0]);
	ostringstream op33;
	ConvertToBinary(op33,extraArguments,4);
	op33 << std::endl;
	
	registerName.push_back("FMM test register enabled (designator = 33)");
	outputArgs.push_back(extraArguments);
	niceOutput.push_back(op33.str());
	showDeci.push_back(false);
      } else {
	registerName.push_back("");
	outputArgs.push_back(extraArguments);
	niceOutput.push_back("");
	showDeci.push_back(false);
      }

      // FE enable
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandFeEnableReg(true,dummyWord,opArg[0]); 
      ostringstream op5;
      for (u32 loop = 0; loop < 8; ++loop) {
	if (!static_cast<bool>((opArg[0] >> loop) & 0x1)) {op5 << loop+1 << " ";}
      }
      if (op5.str().empty()) {op5 << "All FE-FPGAs enabled";}
      else {op5 << "DISABLED";}
      
      registerName.push_back("FE Enable register (designator = 5):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(op5.str());
      showDeci.push_back(false);
      
      // BE readout control
      opArg.clear();
      opArg.resize(1,0);
      sop.clear();
      theFed->beCommandBeReadoutControl(true,dummyWord,opArg[0]);
      if (opArg[0] == 0) {sop = "Output DISABLED";}
      else if (opArg[0] == 1) {sop = "Output route:  SLINK";}
      else if (opArg[0] == 2) {sop = "Output route:  VME";}
      else {sop = "Output route:  NOT RECOGNISED";}

      registerName.push_back("BE readout control register (designator = 6):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // BE run control
      opArg.clear();
      opArg.resize(1,0);
      sop.clear();
      theFed->beCommandBeRunControl(true,dummyWord,opArg[0]);
      if (opArg[0] == 0) {sop = "Ignoring trigger and FE frames & headers";}
      else if (opArg[0] == 1) {sop = "Accept trigger; ignore FE frames & headers";}
      else if (opArg[0] == 2) {sop = "Ignore trigger; Accept FE frames & headers";}
      else if (opArg[0] == 3) {sop = "Accept trigger and FE frames & headers";}
      else {sop = "NOT RECOGNISED";}

      registerName.push_back("BE Run Control register (designator = 7):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // FED ID register
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandFedIdReg(true,dummyWord,opArg[0]);

      registerName.push_back("FED ID register (designator = 10):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // DAQ register
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandDaqRegister(true,dummyWord,opArg[0]);
      ostringstream op13;
      ConvertToBinary(op13,opArg,32);
      op13 << std::endl;

      registerName.push_back("DAQ register (designator = 13):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(op13.str());
      showDeci.push_back(false);

      // Tracker header format type
      opArg.clear();
      opArg.resize(1,0);
      sop.clear();
      theFed->beCommandHeaderFormatType(true,dummyWord,opArg[0]);
      if (opArg[0] == 1) {sop = "FULL DEBUG mode";}
      else if (opArg[0] == 2) {sop = "APV ERROR mode";}
      else {sop = "NOT RECOGNISED";}

      registerName.push_back("Tracker header format type (designator = 14):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // Bx_offset
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandBunchCrossingOffset(true,dummyWord,opArg[0]);
      
      registerName.push_back("Bx_offset register (designator = 15):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // BE Status Register
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandBeStatusRegister(opArg[0]);
      ostringstream op16;
      ConvertToBinary(op16,opArg,32);
      op16 << std::endl;
      
      registerName.push_back("BE Status register (designator = 16):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(op16.str());
      showDeci.push_back(false);

      // Total number of valid L1 accepts
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandReadL1aCount(opArg[0]);
      registerName.push_back("Read L1A Count (designator = 17):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // Bunch crossing counter
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandReadBxCount(opArg[0]);
      registerName.push_back("Read BX count (designator = 18):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // QDR frame counter
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandQdrFrameCounter(opArg[0]);
      registerName.push_back("QDR Frame Counter (designator = 19):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // Firmware version ID
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandFirmwareIdReg(opArg[0]);
      registerName.push_back("Firmware version ID (designator = 21):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(false);

      // QDR data counter
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandQdrDataCounter(opArg[0]);
      registerName.push_back("QDR Data Counter (designator = 22):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);
      
      // Total Frame count
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandTotalFrameCount(opArg[0]);
      registerName.push_back("Total Frame Count (designator = 23):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      opArg.clear();
      vector<u16> ttcOutput = getTtcIndividuallyAddressedCommand();
      for (u32 i = 0; i < ttcOutput.size(); i++) {opArg.push_back(static_cast<u32>(ttcOutput[i]));}
      if (!opArg.empty()) {
	registerName.push_back("TTC Individually Addressed Command (designator = 26):");
	outputArgs.push_back(opArg);
	//	sop = "error checking none command subaddress internal flag TTCrx address";
	niceOutput.push_back("");
	showDeci.push_back(false);
      }

      // TTC Broadcast command
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandTtcBroadcastCommand(opArg[0]);
      registerName.push_back("TTC Broadcast command (designator = 27):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(false);

      // Tracker Header Monitor
      bool readyFlag = getTrackerHeaderMonitorStatus();
      if (readyFlag) {
	opArg.clear();
	theFed->beCommandTrackerHeaderMonitor(opArg);
	if (!opArg.empty()) {
	  registerName.push_back("Tracker Header Monitor (designator = 28):");
	  outputArgs.push_back(opArg);
	  niceOutput.push_back("");
	  showDeci.push_back(false);
	}
      } 

      // Second Status register
      opArg.clear();
      if (getBeFirmwareId() >= 0x2200037F) {
	opArg.resize(1,0);
	theFed->beCommandLongReadRegister(opArg[0]);
      } else {
	theFed->beCommandLongReadRegister(opArg);
      }
      ostringstream op29;
      ConvertToBinary(op29,opArg,64);
      op29 << std::endl;

      if (!opArg.empty()) {
	registerName.push_back("Second Status Register (designator = 29):");
	outputArgs.push_back(opArg);
	niceOutput.push_back(op29.str());
	showDeci.push_back(false);
      }

      // Event Type Register
      opArg.clear();
      opArg.resize(1,0);
      sop.clear();
      theFed->beCommandDaqEventType(true,dummyWord,opArg[0]);
      if (opArg[0] == 1) {sop = "PHYSICS trigger";}
      else if (opArg[0] == 2) {sop = "CALIBRATION trigger";}
      else if (opArg[0] == 3) {sop = "TEST trigger";}
      else if (opArg[0] == 4) {sop = "TECHNICAL/EXTERNAL trigger";}
      else if (opArg[0] == 5) {sop = "SIMULATED EVENTS";}
      else if (opArg[0] == 6) {sop = "TRACED EVENTS";}
      else if (opArg[0] == 0xF) {sop = "ERROR !!";}
      else {sop = "NOT RECOGNISED";}

      registerName.push_back("Event Type register (designator = 30):");
      outputArgs.push_back(opArg);
      niceOutput.push_back(sop);
      showDeci.push_back(false);

      // FOV register
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandDaqFov(true,dummyWord,opArg[0]);

      registerName.push_back("FOV register (designator = 31):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      // Total bunch crossing
      opArg.clear();
      opArg.resize(1,0);
      theFed->beCommandBunchCrossingOrbitTotal(opArg[0]);
      registerName.push_back("Total bunch crossing register (designator = 32):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(true);

      //  DAQ Register 2
      opArg.clear();
      opArg.resize(1,0);
      dummyWord = 0;
      theFed->beCommandDaqRegisterTwo(true,dummyWord,opArg[0]);
      registerName.push_back("DAQ register 2 (designator = 34):");
      outputArgs.push_back(opArg);
      niceOutput.push_back("");
      showDeci.push_back(false);

      // print out nicely according to whether this is for the web or not
      os << "Dumping all readable BE registers ...";
      if (addHtml) {os << "<br>";}
      os << std::endl;
      if (addHtml) {
	os << "<br><br>" << std::endl;
	os << "<TABLE>" << std::endl;
      }
      os << std::endl;

      for (unsigned int loop = 0; loop < registerName.size(); ++loop) {
	if (!registerName[loop].empty()) {
	  // register name
	  if (addHtml) {
	    os << "<TR><TD>" << registerName[loop] << "</TD> <TD>";
	  } else {
	    string info = registerName[loop];
	    info.append(55-info.size(),' ');
	    os << info;
	  }

	  // register contents
	  ostringstream opVals;
	  for (u32 i = 0; i < outputArgs[loop].size(); ++i) {
	    if (i != 0 && !(i % 6) && addHtml) {opVals << "<br>";}
	    opVals << "0x" << std::hex << outputArgs[loop][i] << std::dec << " ";
	  }
	  os << opVals.str();
	  if (!niceOutput[loop].empty() || showDeci[loop]) {
	    string addsp;
	    addsp.append(15-opVals.str().size(),' ');
	    os << addsp;
	  }
	  if (addHtml) {os << "</TD>";}

	  // interpretation
	  if (!niceOutput[loop].empty()) {
	    if (addHtml) {os << "<TD>";}
	    os  << niceOutput[loop];
	    if (addHtml) {os << "</TD>";}
	  }
	  
	  if ((showDeci[loop]) && (outputArgs[loop].size() == 1)) {
	    if (addHtml) {os << "<TD>";}
	    os << outputArgs[loop][0];
	    if (addHtml) {os << "</TD>";}
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}
      }
      

      
      Fed9UTtcrxDescription ttcrxdesc = getTtcrx();
      
      bool serialb = ttcrxdesc.getSerialBDisable();
      if (addHtml) os << "<TR><TD>TTCRX Serial B Data Disable? </TD><TD>" << (serialb?"true":"false") << "</TD>";

      if (addHtml) {
	os << "</TABLE>" << std::endl;
	os << "<br>";
      }

      os << std::endl;
      if (addHtml) {os << "<p>" << std::endl;}
      os << "Breakdown of TTC Individually addressed command:  First value = error checking bits, second value = command word, third value = subaddress word, fourth value = internal flag and fifth value = TTCrx address." << std::endl;
      if (addHtml) {os << "</p>" << std::endl;}
      os << std::endl;

      os << "Dump of BE registers complete !";
      if (addHtml) {os << "<br>";}
      os << std::endl;

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpBeRegisters."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpBeRegisters."));
    }
  }

  // JEC 15/11/06
  // More detailed breakdown of BE status registers one and two
  void Fed9UVmeDevice::dumpBeStatusRegisters(ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {
      u32 statusReg1;
      vector<u32> statusReg2;
      theFed->beCommandBeStatusRegister(statusReg1);
      if (getBeFirmwareId() >= 0x2200037F) {
	statusReg2.resize(1,0);
	theFed->beCommandLongReadRegister(statusReg2[0]);
      } else {
	theFed->beCommandLongReadRegister(statusReg2);
      }

      if (addHtml) {os << "<br>";}
      os << std::endl;
      os << "Detailed breakdown of BE Status Register information";
      if (addHtml) {os << "<br>";}
      os << std::endl;
      if (addHtml) {os << "<br>";}
      os << std::endl;

      // Other information from status register 1
      vector<string> opMessage(8,"");
      opMessage[0] = "Bit 0 - temp_int_b:";
      opMessage[1] = "Bit 1 - Internal freeze:";
      opMessage[2] = "Bit 2 - SLINK DOWN flag:";
      opMessage[3] = "Bit 3 - SLINK FULL flag:";
      opMessage[4] = "Bit 4 - Backpressure flag:";
      opMessage[5] = "Bit 5 - Bunch crossing error at orbit reset:";
      opMessage[6] = "Bit 6 - TTC READY flag:";
      opMessage[7] = "Bit 7 - Tracker Header Monitor data READY flag:";

      if (addHtml) {os << "<TABLE>" << std::endl;}

      for (u32 loop = 0; loop < opMessage.size(); ++loop) {
	if (!addHtml) {opMessage[loop].append(60-opMessage[loop].size(),' ');}
	else {os << "<TR><TD>";}
	os << opMessage[loop];
	if (addHtml) {os << "</TD><TD>";}
	if (static_cast<bool>((statusReg1 >> loop) & 0x1)) {
	  os << "true";
	} else {
	  os << "false";
	}
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;
      }
      os << std::endl;

      vector<string> opMessage2(5,"");
      opMessage2[0] = "Bits 0 - 11:  Bunch crossing counter value at orbit reset:";
      opMessage2[1] = "Bit 12:  TTS Warn Overflow flag:";
      opMessage2[2] = "Bit 13:  TTS Out of Synch flag:";
      opMessage2[3] = "Bit 14:  TTS Busy flag:";
      opMessage2[4] = "Bit 15:  TTS Ready flag:";

      for (u32 loop = 0; loop < opMessage2.size(); ++loop) {
	if (!addHtml) {opMessage2[loop].append(60-opMessage2[loop].size(),' ');}
	else {os << "<TR><TD>";}
	os << opMessage2[loop];
	if (addHtml) {os << "</TD><TD>";}
	u32 statusReg = 0;
	if (statusReg2.size() == 1) {statusReg = statusReg2[0];}
	else {statusReg = statusReg2[1];}

	if (loop == 0) {
	  os << "0x" << std::hex << static_cast<u32>(statusReg & 0xFFF) << std::dec << "  " << static_cast<u32>(statusReg & 0xFFF);
	} else {
	  if (static_cast<bool>((statusReg >> (loop + 11)) & 0x1)) {
	    os << "true";
	  } else {
	    os << "false";
	  }
	}
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;
      }

      if (addHtml) {
	os << "</TABLE>" << std::endl;
	os << "<br>" << std::endl;
      }
      os << std::endl;

      //  FIFO status flags
      vector<string> fifoCond1(5,""), fifoCond2(5,""), fifoName(7,"");
      fifoCond1[0] = "Empty";
      fifoCond1[1] = "Partial";
      fifoCond1[2] = "Half";
      fifoCond1[3] = "Partial";
      fifoCond1[4] = "Full";

      fifoCond2[0] = "";
      fifoCond2[1] = "Empty";
      fifoCond2[2] = "Full";
      fifoCond2[3] = "Full";
      fifoCond2[4] = "";

      fifoName[0] = "FE FPGA";
      fifoName[1] = "FE Event Length";
      fifoName[2] = "L1A & Bx";
      fifoName[3] = "Tracker Header";
      fifoName[4] = "Total Length";
      fifoName[5] = "Frame Address";
      fifoName[6] = "QDR";

      if (addHtml) {os << "<TABLE>" << std::endl;}

      if (addHtml) {os << "<TR><TD>";}
      string nameA = "FIFO";
      if (!addHtml) {nameA.append(20-nameA.size(),' ');}
      os << nameA;
      if (addHtml) {os << "</TD>";}

      for (u32 loop = 0; loop < fifoCond1.size(); ++loop) {
	string name1 = fifoCond1[loop];
	if (addHtml) {os << "<TD>";}
	else {name1.append(10-fifoCond1[loop].size(),' ');}
	os << name1;
	if (addHtml) {os << "</TD>";}
      }
      if (addHtml) {os << "</TR>";}
      os << std::endl;

      if (addHtml) {os << "<TR><TD>";}
      string nameB = "STATUS";
      if (!addHtml) {nameB.append(20-nameB.size(),' ');}
      os << nameB;
      if (addHtml) {os << "</TD>";}

      for (u32 loop = 0; loop < fifoCond2.size(); ++loop) {
	string name1 = fifoCond2[loop];
	if (addHtml) {os << "<TD>";}
	else {name1.append(10-fifoCond2[loop].size(),' ');}
	os << name1;
	if (addHtml) {os << "</TD>";}
      }
      if (addHtml) {os << "</TR>";}
      os << std::endl;

      u32 fifoFullID = 30;
      for (u32 loop = 0; loop < fifoName.size(); ++loop) {
	if (addHtml) {os << "<TR>";}
	string name2 = fifoName[loop];
	if (addHtml) {os << "<TD>";}
	else {name2.append(20-fifoName[loop].size(),' ');}
	os << name2;
	if (addHtml) {os << "</TD>";}

	u32 fifoID = fifoFullID;
	string opVal;

	// Empty
	opVal.clear();
	if (static_cast<bool>((statusReg1 >> fifoID) & 0x1)) {opVal = "  1";}
	else {opVal = "  0";}
	opVal.append(10-opVal.size(),' ');
	if (addHtml) {os << "<TD>";}
	if (addHtml) {os << "<center> " << opVal << " </center>";}
	else {os << opVal;}
	if (addHtml) {os << "</TD>";}
	fifoID -= 9;

	// Partial Empty
	opVal.clear();
	if (static_cast<bool>((statusReg2[0] >> fifoID) & 0x1)) {opVal = "  1";}
	else {opVal = "  0";}
	opVal.append(10-opVal.size(),' ');
	if (addHtml) {os << "<TD>";}
	if (addHtml) {os << "<center> " << opVal << " </center>";}
	else {os << opVal;}
	if (addHtml) {os << "</TD>";}
	fifoID += 9;

	// Half full
	opVal.clear();
	if (static_cast<bool>((statusReg2[0] >> fifoID) & 0x1)) {opVal = "  1";}
	else {opVal = "  0";}
	opVal.append(10-opVal.size(),' ');
	if (addHtml) {os << "<TD>";}
	if (addHtml) {os << "<center> " << opVal << " </center>";}
	else {os << opVal;}
	if (addHtml) {os << "</TD>";}
	fifoID -= 8;

	// Partial full
	opVal.clear();
	if (static_cast<bool>((statusReg1 >> fifoID) & 0x1)) {opVal = "  1";}
	else {opVal = "  0";}
	opVal.append(10-opVal.size(),' ');
	if (addHtml) {os << "<TD>";}
	if (addHtml) {os << "<center> " << opVal << " </center>";}
	else {os << opVal;}
	if (addHtml) {os << "</TD>";}
	fifoID -= 8;

	// Full
	opVal.clear();
	if (static_cast<bool>((statusReg1 >> fifoID) & 0x1)) {opVal = "  1";}
	else {opVal = "  0";}
	opVal.append(10-opVal.size(),' ');
	if (addHtml) {os << "<TD>";}
	if (addHtml) {os << "<center> " << opVal << " </center>";}
	else {os << opVal;}
	if (addHtml) {os << "</TD>";}

	if (addHtml) {os << "</TR>";}
	os << std::endl;
	fifoFullID--;
      }

      if (addHtml) {
	os << "</TABLE>" << std::endl;
	os << "<br>";
      }
      os << std::endl;


    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpBeStatusRegisters."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpBeStatusRegisters."));
    }
  }

  void Fed9UVmeDevice::dumpTrackerHeaderMonitor(ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {
      bool readyFlag = getTrackerHeaderMonitorStatus();
      if (readyFlag) {
	for (u16 fpga = 1; fpga < 9; fpga++) {
	  ostringstream fpgaNo;
	  fpgaNo << "FPGA " << fpga;
	  string fpgaNumber = fpgaNo.str();
	  fpgaNumber.append(10-fpgaNumber.size(),' ');
	  os << fpgaNumber;
	  
	  
	  vector<u8> opArg = getTrackerHeaderMonitor(fpga);
	  
	  os << "Good Fibre Status:  ";
	  for (u32 loop = 0; loop < opArg.size()-1; loop++) {
	    if (opArg[loop] == 0x3F) {
	      os << loop+1 << "  ";
	    }
	  }
	  os << std::endl;
	  if (addHtml) {os << "<br>";}
	  
	  os << "          ";
	  os << "Bad Fibre Status:   ";
	  for (u32 loop = 0; loop < opArg.size()-1; loop++) {
	    if (opArg[loop] != 0x3F) {
	      os << loop+1 << " (bits ";
	      for (u32 i = 0; i < 6; i++) {
		if (!((opArg[loop] >> i) & 0x1)) {os << i << " ";}
	      }
	      os << ")";
	    }
	  }
	  os << std::endl;
	  if (addHtml) {os << "<br>";}
	}
      } else {
	os << "Tracker header monitor not ready to be read!<br>" << std::endl;
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpTrackerHeaderMonitor."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpTrackerHeaderMonitor."));
    }
  }
  
  
  // under development
  void Fed9UVmeDevice::ConvertToBinary(ostream &op, vector<u32> numberToConvert, u32 length) {
    ostringstream element1;
    for (u32 eloop = 0; eloop < numberToConvert.size(); eloop++) {
      u32 element = numberToConvert.size() - eloop - 1;
      for (u32 loop = 0; loop < 32; loop++) {
	if ((loop + 32*eloop) < length) {
	  if (eloop == 0) {
	    if (loop != 0 && !(loop % 4)) {element1 << " ";}
	  } else {
	    if (!(loop % 4)) {element1 << " ";}
	  }
	  if ((numberToConvert[element] >> loop) & 0x1) {element1 << "1";}
	  else {element1 << "0";}
	}
      }
    }
    op << element1.str();
  }




  
}//end of Fed9U namespace
