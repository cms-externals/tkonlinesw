#include <inttypes.h>
#include <stdint.h>
#ifdef EVENT_STREAMLINE
#include "Fed9UEventStreamLine.hh"
#else
#include "Fed9UEvent.hh"
#endif
#include "Fed9UVmeDevice.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UTtcrxDevice.hh"
#include "Fed9UVoltageMonitor.hh"
#include "Fed9UWait.hh"
#include "Fed9ULogTemplate.hh"

#include <cmath>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <string>

namespace Fed9U {
  using std::ostringstream;
  /************************************************************************************************
   *                              These are the high level commands                               *
   ************************************************************************************************/

  u32 Fed9UVmeDevice::getFeFirmwareVersion() const {
    return getFeFirmwareId();
  }
  
  u32 Fed9UVmeDevice::getBeFirmwareVersion() const {
    return getBeFirmwareId();
  }

  u32 Fed9UVmeDevice::getVmeFirmwareVersion() const {
    return getVmeFirmwareId();
  }

  u32 Fed9UVmeDevice::getDelayFirmwareVersion() const {
    return getDelayFirmwareId();
  }

  u32 Fed9UVmeDevice::getFedVersion() const {
    return theLocalFedSettings.getFedVersion();
  }

  u32 Fed9UVmeDevice::getEpromVersion() const {
    return theLocalFedSettings.getEpromVersion();
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setFeFirmwareVersion(u32 version) {
    ICUTILS_VERIFYX( version == getFeFirmwareVersion(),Fed9UVmeDeviceException)(version)(getFeFirmwareVersion()).error().msg("you cannont set the firmware version to a value other than that which matches what is loaded in the FED");
    theLocalFedSettings.setFeFirmwareVersion(version);
    return *this;
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setBeFirmwareVersion(u32 version) {
    ICUTILS_VERIFYX( version == getBeFirmwareVersion(),Fed9UVmeDeviceException)(version)(getBeFirmwareVersion()).error().msg("you cannont set the firmware version to a value other than that which matches what is loaded in the FED");
    theLocalFedSettings.setBeFirmwareVersion(version);
    return *this;
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setVmeFirmwareVersion(u32 version){
    ICUTILS_VERIFYX( version == getVmeFirmwareVersion(),Fed9UVmeDeviceException)(version)(getVmeFirmwareVersion()).error().msg("you cannont set the firmware version to a value other than that which matches what is loaded in the FED");
    theLocalFedSettings.setVmeFirmwareVersion(version);
    return *this;
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setDelayFirmwareVersion(u32 version){
    ICUTILS_VERIFYX( version == getDelayFirmwareVersion(),Fed9UVmeDeviceException)(version)(getDelayFirmwareVersion()).error().msg("you cannont set the firmware version to a value other than that which matches what is loaded in the FED");
    theLocalFedSettings.setDelayFirmwareVersion(version);
    return *this;
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setEpromVersion(u32 version) {
    theLocalFedSettings.setEpromVersion(version);
    return *this;
  }
  
  Fed9UVmeDevice& Fed9UVmeDevice::setFedVersion(u32 version){
    theLocalFedSettings.setFedVersion(version);
    return *this;
  }

  /**
   * This method sends a complete logic reset to the FE and BE FPGAs.
   *
   * It will not reset the control registers (e.g. the scope length, FED DAQ mode, clock etc)
   * but it will reset all the counters and associated logic. Can be used to clear an event without
   * having to rerun the init() method. Warning this will reset the APV disable settings.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendFedCompleteLogicReset() throw (Fed9UVmeDeviceException) {
    try {
      Fed9UAddress addr;
      addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      feFpgaSoftReset(addr);
      beFpgaSoftReset();
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendFedCompleteLogivReset()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This will reset the whole FED.
   *
   * The TTCrx chip, the temperature monitors, pedestals, disabled strips, high and
   * low cluster thresholds and the clock source are unaffected by a FED Reset.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendFedReset(bool doWait) throw (Fed9UVmeDeviceException) {
    try {
      theFed->vmeCommandSoftwareReset(doWait);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendFedReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This will reset VME VLink and counters only.
   *
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendVLinkReset() throw (Fed9UVmeDeviceException) {
    try {
      theFed->vmeCommandVLinkReset();
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendFedReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Sets the DAQ mode for the FED.
   *
   * The possible DAQ modes are defined in the Fed9UDaqMode enum, which
   * is declared in TypeDefs.hh. There are four DAQ modes avaliable
   * process raw data mode, zero suppressed data mode, virgin raw dat
   * mode and scope mode.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setDaqMode(Fed9UDaqMode fedDaqMode) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY( fedDaqMode==FED9U_MODE_PROCESSED_RAW 
		      || fedDaqMode==FED9U_MODE_ZERO_SUPPRESSED 
		      || fedDaqMode==FED9U_MODE_VIRGIN_RAW 
		      || fedDaqMode==FED9U_MODE_SCOPE 
		      || fedDaqMode==FED9U_MODE_NONE)(fedDaqMode).error().msg("Invalid FED DAQ mode");

      //Broadcasts the DAQ mode to all FE FPGAs
      const u16 feFpga_ = 14;
      vector<u32> feDaqMode(1,0);
      u32 beDaqMode;

      // JF 24/1/2006 added the code to cope with the new super mode, this affects the setting of the back end mode register
      if (theLocalFedSettings.getFeFirmwareVersion() >= 0x23100320 )  {
	switch (fedDaqMode) {
	case FED9U_MODE_SCOPE:
	  feDaqMode[0] = 0x5;
	  beDaqMode    = 0x1;
	  break;
	case FED9U_MODE_VIRGIN_RAW:
	  switch (theLocalFedSettings.getDaqSuperMode()) {
	  case FED9U_SUPER_MODE_NORMAL:
	    feDaqMode[0] = 0x6;
	    beDaqMode    = 0x2;
	    break;
	  case FED9U_SUPER_MODE_FAKE:
	    feDaqMode[0] = 0x6;
	    beDaqMode    = 0x3;
	    break;
	  default:
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised SUPER DAQ mode. Are you sure the super mode is compatible with the daq mode"));
	    break;
	  }
	   break;
	case FED9U_MODE_PROCESSED_RAW:
	  switch (theLocalFedSettings.getDaqSuperMode()) {
	  case FED9U_SUPER_MODE_NORMAL:
	    feDaqMode[0] = 0x12;
	    beDaqMode    = 0x6;
	    break;
	  case FED9U_SUPER_MODE_FAKE:
	    feDaqMode[0] = 0x12;
	    beDaqMode    = 0x7;
	    break;
	  default:
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised SUPER DAQ mode. Are you sure the super mode is compatible with the daq mode"));
	    break;
	  }
	  break; 
	case FED9U_MODE_ZERO_SUPPRESSED:
	  switch (theLocalFedSettings.getDaqSuperMode()) {
	  case FED9U_SUPER_MODE_NORMAL:
	    feDaqMode[0] = 0xa;
	    beDaqMode    = 0xa;
	    break;
	  case FED9U_SUPER_MODE_FAKE:
	    feDaqMode[0] = 0xa;
	    beDaqMode    = 0xb;
	    break;
	  case FED9U_SUPER_MODE_ZERO_LITE:
	    feDaqMode[0] = 0xa;
	    beDaqMode    = 0xc;
	    break;
	  case FED9U_SUPER_MODE_FAKE_ZERO_LITE:
	    feDaqMode[0] = 0xa;
	    beDaqMode    = 0xd;
	    break;
	  default:
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised sUPER DAQ mode. Are you sure the super mode is compatible with the daq mode?"));
	    break;
	  }
	  break;
	case FED9U_MODE_NONE:
	  feDaqMode[0] = 0x0;
	  beDaqMode    = 0x1;
	  break;
	default:
	  THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised DAQ mode."));
	  break;
	}
      }
      else {
	cout << "you must have a higher firmware version for the front end. if you want to use Fake Events or Zero Lite data modes!" << endl;
	switch (fedDaqMode) {
	case FED9U_MODE_SCOPE:
	  feDaqMode[0] = 0x5;
	  beDaqMode    = 0x1;
	  break;
	case FED9U_MODE_VIRGIN_RAW:
	  feDaqMode[0] = 0x6;
	  beDaqMode    = 0x2;
	  break;
	case FED9U_MODE_PROCESSED_RAW:
	  feDaqMode[0] = 0x12;
	  beDaqMode    = 0x6;
	  break;
	case FED9U_MODE_ZERO_SUPPRESSED:
	  feDaqMode[0] = 0xa;
	  beDaqMode    = 0xa;
	  break;
	default:
	  THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised DAQ mode."));
	  break;
	}
      }
      
      theFed->feCommandLoadModeReg(Fed9UAddress().setFedFeUnit(feFpga_).getFirmwareFedFeUnit(), WRITE, feDaqMode, dummyVector);
      theFed->beCommandModeRegister(WRITE, beDaqMode, dummyWord);
      theLocalFedSettings.setDaqMode(fedDaqMode);
      return *this;
    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqMode()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  Fed9UVmeDevice& Fed9UVmeDevice::setDaqSuperMode(Fed9UDaqSuperMode fedDaqSuperMode) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(fedDaqSuperMode==FED9U_SUPER_MODE_FAKE || fedDaqSuperMode==FED9U_SUPER_MODE_NORMAL 
	     || fedDaqSuperMode==FED9U_SUPER_MODE_ZERO_LITE ||  fedDaqSuperMode==FED9U_SUPER_MODE_FAKE_ZERO_LITE)(fedDaqSuperMode).error().msg("Invalid FED DAQ mode");
      
      //Broadcasts the DAQ mode to all FE FPGAs
      const u16 feFpga_ = 14;
      vector<u32> feDaqSuperMode(1,0);
      
      switch (fedDaqSuperMode) {
      case FED9U_SUPER_MODE_FAKE:
	feDaqSuperMode[0] = 0x1;
	break;
      case FED9U_SUPER_MODE_ZERO_LITE:
	feDaqSuperMode[0] = 0x2;
	break;
      case FED9U_SUPER_MODE_FAKE_ZERO_LITE:
	feDaqSuperMode[0] = 0x3;
	break;
      case FED9U_SUPER_MODE_NORMAL:
	feDaqSuperMode[0] = 0x0;
	break;
      default:
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised DAQ super mode."));
	break;
      }
      cout << "fed fe unit for supermode = " << static_cast<u16>(Fed9UAddress().setFedFeUnit(feFpga_).getFirmwareFedFeUnit()) << endl;
      theFed->feCommandLoadSuperModeReg(Fed9UAddress().setFedFeUnit(feFpga_).getFirmwareFedFeUnit(), WRITE, feDaqSuperMode, dummyVector);
      theLocalFedSettings.setDaqSuperMode(fedDaqSuperMode);
      // IMPORTANT!!! Once we have set the daq supermode, we must reset the daq mode to make sure that it corresponds to the correct super mode
      setDaqMode(theLocalFedSettings.getDaqMode());
      return *this;
    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDaqSuperMode()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
    
  }

  /**
   * Returns bool noting that we either have a FED or not.
   *
   * In order to identify FEDs in the crate we read from this register
   * It should hold the value 0x00000FED, if it does we return true, in not false
   */
  bool Fed9UVmeDevice::getIsFed()  const throw (Fed9UVmeDeviceException) {
    try {
      u32 readArguments;
      theFed->vmeCommandReadFedBoardSpecifier(readArguments) ;
      if ( readArguments!=0x00000FED ) {
	try {// if we are an old version FED then we might have to try to reset the FED to find out if it is a FED.
	  getFedId();
	} catch (...) {
	  // failed to reset the FED for some reason, it is probably not a fed in the slot
	  return(false);
	}
	return (true);
      } else {
      // return true if we find a FED
	return (true);
      }

    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFedBoardSpecifier()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Gets the DAQ mode for the FED.
   *
   * The possible DAQ modes are defined in the Fed9UDaqMode enum, which
   * is declared in TypeDefs.hh. There are four DAQ modes avaliable
   * process raw data mode, zero suppressed data mode, virgin raw dat
   * mode and scope mode.
   */
  Fed9UDaqMode Fed9UVmeDevice::getDaqMode()  const throw (Fed9UVmeDeviceException) {
    try {
      //Only read back from the first FE FPGA the rest are ICUTILS_VERIFYed in consistancyCheck() to ensure
      //they have the same value.
      const u16 feFpga = 0;
      vector<u32> feDaqMode;
      u32 beDaqMode = 0;

      theFed->feCommandLoadModeReg(Fed9UAddress().setFedFeUnit(feFpga).getFirmwareFedFeUnit(), READ, dummyVector, feDaqMode);
      theFed->beCommandModeRegister(READ, dummyWord, beDaqMode);
      //This method will check the DAQ mode is the same on all the FE FPGAs and that the BE FPGA
      //is in the same mode as the FE FPGAs.
     return daqConsistancyCheck(feDaqMode, beDaqMode);
    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqMode()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }



  Fed9UDaqSuperMode Fed9UVmeDevice::getDaqSuperMode()  const throw (Fed9UVmeDeviceException) {
    try {
      //Only read back from the first FE FPGA the rest are ICUTILS_VERIFYed in consistancyCheck() to ensure
      //they have the same value.
      const u16 feFpga = 0;
      vector<u32> feDaqSuperMode(1,6);

      theFed->feCommandLoadSuperModeReg(Fed9UAddress().setFedFeUnit(feFpga).getFirmwareFedFeUnit(), READ, dummyVector, feDaqSuperMode);
      //cout << " daq mode read back as: " << feDaqSuperMode[0] << endl;
      //This method will check the DAQ mode is the same on all the FE FPGAs and that the BE FPGA
      //is in the same mode as the FE FPGAs.
      return daqSuperModeConsistancyCheck(feDaqSuperMode);
    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDaqSuperMode()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This allows the user to control the LM82 temperature monitor chip for the FPGA.
   * The maximum values for the temperature values are 127oC.
   *
   * This takes a Fed9UTempControl class. The class contains all the settings for the LM82 chip.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setTempControl(const Fed9UAddress& fedFpga, const Fed9UTempControl& temperatureSettings) throw (Fed9UVmeDeviceException) {
    try {
      //Hold the write register addresses.
      enum { FED9U_TEMP_CONFIG = 0x09, FED9U_TEMP_LM82_HIGH = 0x0b, FED9U_TEMP_FPGA_HIGH = 0x0d, FED9U_TEMP_CRITICAL = 0x5a };

      u16 fedFpga_ = fedFpga.getFedFpga();

      //The first element holds the temperature monitor data and the second holds the register address.
      vector<u32> tempDataAndRegAddr(2,0);

      //Sets the LM82 temperature high point.
      tempDataAndRegAddr[0] = temperatureSettings.getLm82High();
      tempDataAndRegAddr[1] = FED9U_TEMP_LM82_HIGH;
      tempControlWrite(fedFpga_, tempDataAndRegAddr);

      //Sets the FPGA temperature high point.
      tempDataAndRegAddr[0] = temperatureSettings.getFpgaHigh();
      tempDataAndRegAddr[1] = FED9U_TEMP_FPGA_HIGH;
      tempControlWrite(fedFpga_, tempDataAndRegAddr);

      //Sets the critical temperature. This requires the configuration register be written to first.
      //0xbc must first be written to disable the critical temperature shut down.
      tempDataAndRegAddr[0] = 0xbc;
      tempDataAndRegAddr[1] = FED9U_TEMP_CONFIG;
      tempControlWrite(fedFpga_, tempDataAndRegAddr);
      //Now the critical temperature can be set.
      tempDataAndRegAddr[0] = temperatureSettings.getCritical();
      tempDataAndRegAddr[1] = FED9U_TEMP_CRITICAL;
      tempControlWrite(fedFpga_, tempDataAndRegAddr);
      //Once writing to the critical temperature register has been completed the configuration register must be reset to 0xa8.
      tempDataAndRegAddr[0] = 0xa8;
      tempDataAndRegAddr[1] = FED9U_TEMP_CONFIG;
      tempControlWrite(fedFpga_, tempDataAndRegAddr);

      //Write the new data to theLocalFedSettings.
      theLocalFedSettings.setTempControl(fedFpga, temperatureSettings);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTempControl"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns a Fed9UTempControl object which contain all the read/write settings on the LM82
   * temperature monitor, but not the read only settings.
   */
  Fed9UTempControl Fed9UVmeDevice::getTempControl(const Fed9UAddress& fedFpga)  const throw (Fed9UVmeDeviceException) {
    try {
      //These are all the possible addresses that can be read from.
      enum { FED9U_TEMP_LM82_HIGH = 0x05, FED9U_TEMP_FPGA_HIGH = 0x07, FED9U_TEMP_CRITICAL  = 0x42};
      u16 fedFpga_ = fedFpga.getFedFpga();

      //As the register address is in the 2nd element there must be a 1st element
      //where some dummy data is passed.
      vector<u32> regAddress(2,0);

      //Read the temperature values from the FED.
      //Read the LM82 temperature high point.
      regAddress[1] = FED9U_TEMP_LM82_HIGH;
      u32 lm82High = tempControlRead(fedFpga_, regAddress);

      //Read the FPGA temperature high point.
      regAddress[1] = FED9U_TEMP_FPGA_HIGH;
      u32 fpgaHigh = tempControlRead(fedFpga_, regAddress);

      //Read the critical temperature.
      regAddress[1] = FED9U_TEMP_CRITICAL;
      u32 critical = tempControlRead(fedFpga_, regAddress);

      Fed9UTempControl temperature(lm82High, fpgaHigh, critical);

      return temperature;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTempControl"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This reads back all the settings on the LM82 temperature monitor. It returns a
   * Fed9UTempControlInfo object which holds the read only and read/write settings.
   *
   * Fed9UTemControlInfo contains the read and writable settings (inherited from Fed9UTempControl), but
   * it also contains the read only registers. The status register u32 can be masked with the enum
   * Fed9UTemperatureStatus, found in TypeDefs.hh, to check the individual status bits.
   */
  Fed9UTempControlInfo Fed9UVmeDevice::getTempControlInfo(const Fed9UAddress& fedFpga)  const throw (Fed9UVmeDeviceException) {
    try {
      //These are all the possible addresses that can be read from.
      enum { FED9U_TEMP_LM82_TEMP = 0x00, FED9U_TEMP_FPGA_TEMP = 0x01, FED9U_TEMP_STATUS  = 0x02, FED9U_TEMP_READ_ID = 0xfe};
      u16 fedFpga_ = fedFpga.getFedFpga();
      //As the register address is in the 2nd element there must be a 1st element
      //where some dummy data is passed.
      vector<u32> regAddress(2,0);
      Fed9UTempControl temp = getTempControl( fedFpga );
      
      //Read the current LM82 temperature.
      regAddress[1] = FED9U_TEMP_LM82_TEMP;
      u32 lm82CurrentTemp = tempControlRead(fedFpga_, regAddress);

      //Read the current FPGA temperature.
      regAddress[1] = FED9U_TEMP_FPGA_TEMP;
      u32 fpgaCurrentTemp = tempControlRead(fedFpga_, regAddress);

      //Read the manufacturers ID.
      regAddress[1] =  FED9U_TEMP_READ_ID;
      u32 id = tempControlRead(fedFpga_, regAddress);

      //Read the LM82 status register.
      regAddress[1] = FED9U_TEMP_STATUS;
      //The returned status bits are not in the order that the enum Fed9UTemperatureStatus
      //requires so they are reodered.
      u32 data = tempControlRead(fedFpga_, regAddress);
      u32 status = 0;
      status |= (data >> 6) & FED9U_TEMP_STATUS_LM82_HIGH;
      status |= (data >> 3) & FED9U_TEMP_STATUS_FPGA_HIGH;
      status |= (data << 2) & FED9U_TEMP_STATUS_LM82_CRIT;
      status |= (data << 2) & FED9U_TEMP_STATUS_FPGA_CRIT;
      status |= (data << 2) & FED9U_TEMP_STATUS_FPGA_DISCONNETED;

      //Holds all the temperature control information. This has no default constructor
      //so it must be constructed with the value read back from the FED.
      Fed9UTempControlInfo temperatureInfo(temp, lm82CurrentTemp, fpgaCurrentTemp, id, status);

      return temperatureInfo;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTempControlInfo"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

//   /**
//    * A method to read the status register of the FE FPGA LM82 temperature monitoring chip.
//    *
//    * Returns a five element vector. A return value of true signifies a warning
//    * that a limit has been exceeded or the FPGA is disconnected.
//    * In element -
//    * 0 LM82 chip high point temperature exceeded,
//    * 1 LM82 chip critical temperature exceed,
//    * 2 FPGA high point temperature exceeded,
//    * 3 FPGA critical temperature point exceeded,
//    * 4 FPGA not connected to LM82 temperature monitoring chip.
//    * It can take a few seconds for the LM82 chip to reset the high temperature
//    * bit to zero after either the LM82 or the FPGA have come down below there
//    * high point values.
//    */
//   vector<bool> Fed9UVmeDevice::getTempControlStatus(const Fed9UAddress& fedFpga) const {
//     //\todo - should be able to replace this once the description starts using Fed9UTempControlInfo.
//     try {
//       bool busy  = false;
//       bool error = false;
//       vector<bool> tempStatus(5,0);
//       //As the register address is in the 2nd element there must be a 1st element
//       //where some dummy data is passed.
//       vector<u32> regAddress(2,0);
//       regAddress[1] = 2;
//       u16 fedFpga_ = fedFpga.getFedFpga();

//       u32 lm82Status;
//       u32 loopCount      = 0;
//       const u32 maxLoops = 2;
//       do {
// 	if (fedFpga_ != 9) {
// 	  theFed->feCommandLm82Control(fedFpga_+1, READ, regAddress, lm82Status, busy, error);
// 	} else {
// 	  theFed->beCommandLm82Control(READ, regAddress, lm82Status, busy, error);
// 	}
// 	//If the write still fails after a set number of attempts then
// 	//the software gives up and throws an error.
// 	ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then temperature control chip is busy. If error = 1 then the write failed");
// 	//If the write fails there is a 500ms pause before it tries again.
// 	fed9Uwait(1000000);
// 	++loopCount;
//       } while (busy || error);

//       tempStatus[0] = (lm82Status >> 6) & 1;
//       tempStatus[1] =  lm82Status       & 1;
//       tempStatus[2] = (lm82Status >> 4) & 1;
//       tempStatus[3] = (lm82Status >> 1) & 1;
//       tempStatus[4] = (lm82Status >> 2) & 1;

//       return tempStatus;
//     }
//     catch (const ICUtils::ICException& e) {
//       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getTempControlStatus"));
//     }
//   }

  /**
   * The returned vector contains the status of the BE FPGA registers that contain
   * the event information.
   *
   * This returns an 4 element vector. The settings contained in the elements are as
   * follows:-
   * 0 Number of triggers received,
   * 1 Number of frames stored in QDR memory,
   * 2 Number of unread words in QDR,
   * 3 Total frame count written to QDR.
   */
  vector<u32> Fed9UVmeDevice::getBeEventCounterStatus()  const throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> beCounterStatus(5, 0);

      beCounterStatus[0]  = getTriggerNumber();
      beCounterStatus[1]  = getQdrFrameCount();
      beCounterStatus[2]  = getQdrBufferLevel();
      beCounterStatus[3]  = getQdrTotalFrameCount();
      beCounterStatus[4]  = getBunchCount();

      return beCounterStatus;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getBeEventCounterStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * In the event that the localFedSettings gets totally out of sync with the FED settings this
   * method can be called to retrieve all the current infromation stored on the FED and pass
   * it to the localFedSettings. The only information it does not get is what is stored in
   * the EPROM.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::updateLocalDescriptionFromFed() throw (Fed9UVmeDeviceException) {
    try {
      Fed9UDaqMode fedDaqMode;
      fedDaqMode = getDaqMode();
      theLocalFedSettings.setDaqMode(fedDaqMode);

      theLocalFedSettings.setClock( getClock() );

      //Update the BE FPGA registers.
      theLocalFedSettings.setTriggerSource( getTriggerSource() );
      theLocalFedSettings.setTestRegister( getTestRegister() );
      theLocalFedSettings.setBeFpgaReadRoute( getBeFpgaReadRoute() );
      theLocalFedSettings.setHeaderFormatType( getHeaderFormatType() );
      theLocalFedSettings.setFedBeFpgaDisable( getFedBeFpgaDisable() );
      theLocalFedSettings.setFedId( getFedId() );
      theLocalFedSettings.setScopeLength( getScopeLength() );

      //Update the TTCrx, serial EPROM and voltage monitor.
      theLocalFedSettings.setTtcrx( getTtcrx() );

      //The LM82 is unique in that it can be set for the eight FE FPGAs, the BE FPGA
      //and the VME FPGA. Therefore the following loop will cover all the possible
      //values that the Fed9UAddress can take for the LM82 command.
      const u32 ALL_FPGA_ADDRESSES = 11;
      for (u32 fedFpga_ = 0; fedFpga_ < ALL_FPGA_ADDRESSES; fedFpga_++) {
	if (fedFpga_ == 8) {
	  //There is no FPGA at address number 8.
	} else {
	  //This loops over all the valid addresses for the FPGAs.
	  //It covers the FE, BE and VME FPGAs.
	  Fed9UAddress addr;
	  addr.setFedFpga(fedFpga_);
	  theLocalFedSettings.setTempControl(addr, getTempControl(addr));
	}
      }

      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	Fed9UAddress addr;
	addr.setFedFeUnit(feFpga_);

	theLocalFedSettings.setFedFeUnitDisable(addr, getFedFeUnitDisable(addr));

	theLocalFedSettings.setOptoRxInputOffset( addr, getOptoRxInputOffset(addr) );
	theLocalFedSettings.setOptoRxOutputOffset( addr, getOptoRxOutputOffset(addr) );
	theLocalFedSettings.setOptoRxCapacitor( addr, getOptoRxCapacitor(addr) );

	bool medianOverrideDisable;
	vector<u16> medianValues = getCmMedianOverride(addr, medianOverrideDisable);
	theLocalFedSettings.setCmMedianOverride(addr, medianOverrideDisable, medianValues);
      }

      for (u32 fedChannel_ = 0; fedChannel_ < CHANNELS_PER_FED; fedChannel_++) {
	Fed9UAddress addr;
	addr.setFedChannel(fedChannel_);

 	theLocalFedSettings.setFrameThreshold(addr, getFrameThreshold(addr));

 	theLocalFedSettings.setAdcControls(addr, getAdcControls(addr));

      }

      for (u32 fedApv_ = 0; fedApv_ < APVS_PER_FED; fedApv_++) {
	Fed9UAddress addr;
	addr.setFedApv(fedApv_);

	theLocalFedSettings.setApvDisable(addr, getApvDisable(addr));

	vector<u16> pedestals  (128,0);
	vector<bool> disableStrips(128,0);
	u16 numberValidStrips;
	getPedsAndDisabledStrips(addr, pedestals, disableStrips, numberValidStrips);

	vector<u16> highThresholds(128,0);
	vector<u16> lowThresholds (128,0);
	getClusterThresholds(addr, highThresholds, lowThresholds);

	vector<Fed9UStripDescription> apvStripDescriptions(128);

	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; apvStrip_++) {
	  apvStripDescriptions[apvStrip_].setPedestal( pedestals[apvStrip_] );
	  apvStripDescriptions[apvStrip_].setDisable( disableStrips[apvStrip_] );
	  apvStripDescriptions[apvStrip_].setLowThreshold( lowThresholds[apvStrip_] );
	  apvStripDescriptions[apvStrip_].setHighThreshold( highThresholds[apvStrip_] );
	}
	theLocalFedSettings.getFedStrips().setApvStrips(addr, apvStripDescriptions);
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::updateLocalDescriptionFromFed()."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method will prepare the FED to run in a DAQ mode specified in theLocalFedSettings
   * and setup the FED so that it will be ready to receive data once the start() method
   * has been called.
   *
   * This calls the stop method, so once initialised the start method must be called before data
   * can be taken. The strip settings (pedestals, disabled strips and cluster thresholds) and
   * the APV disable flag settings and the BE FPGA disable flag are not setup.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::init(bool setClockAndReset) throw (Fed9UVmeDeviceException) {
    try {

      stop(); // just to be sure nothing happens during the clock setting!
      stringstream msg;
      bool clockSet=false;
      if (setClockAndReset && getClock() == theLocalFedSettings.getClock()) {
	clockSet = true;
	if (setClockAndReset) {
	  timer1.start();
	  sendFedReset();
	  timer1.stop();
	  msg << "sendFedReset() takes " << timer1.read() << "us" << endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	}
      }
      //Added stop here to prevent any strange effects from causing a frame-like readout from occuring during init.
      //timer1.start();
      stop();
      //timer1.stop();
      //msg.str("");
      //msg << "stop() takes " << timer1.read() << "us" << endl;
      //Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      timer1.start();
      msg.str("");
      msg << "started timer now it is at " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      setTtcrx( theLocalFedSettings.getTtcrx() );
      timer1.stop();
      msg.str("");
      msg << "setTTcrx() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      if (setClockAndReset && !clockSet){
	timer1.start();
	msg.str("");
	//cout << "started timer now it is at " << timer1.read() << "us" << endl;
	setClock(theLocalFedSettings.getClock());
	timer1.stop();
	msg.str("");
	msg << "setClock() takes " << timer1.read() << "us" << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      }

      stop();

      // <JRF date="06/06/2008"> set the Spy Arm TTC B channel command data value = (binary  1111 0001 0000 xxxx )
      //<TODO> this should be tidied up and put into the fed description!!!
      // <NAC date="31/07/2009"> changed to match 0x88 in Laurent's code
      theFed->beCommandSpyArmTtcBData(WRITE, 0xF10A,dummyWord );
      // </NAC>

      u32 readBackBData = 0;
      theFed->beCommandSpyArmTtcBData(READ, 0, readBackBData );
      msg.str("");
      msg << "read Back spy arm TTC B data as 0x" << hex << readBackBData << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();

      // </JRF>

      
      timer1.start();
      beFpgaSoftReset();
      timer1.stop();
      msg.str("");
      msg << "beFpgaSoftReset() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      Fed9UAddress addr;
      addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      feFpgaSoftReset(addr);
      timer1.stop();
      msg.str("");
      msg << "feFpgaSoftReset() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      //JF TO DO... is this necessary?
      //sendVLinkReset();


      timer1.start();
      setDaqMode(theLocalFedSettings.getDaqMode());
      //fed9Uwait(1000000); // this wait is required for some reason to ensure that the super mode is set correctly after the daq mode is set.
      timer1.stop();
      msg.str("");
      msg << "setDaqMode() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    

      timer1.start();
      if (theLocalFedSettings.getFeFirmwareVersion() >= 0x23100320 )  {
	setDaqSuperMode(theLocalFedSettings.getDaqSuperMode());
      }
      else {
	cout << "you must have a higher firmware version for the front end. if you want to use Fake Events or Zero Lite data modes!" << endl;
      }
      msg.str("");
      // << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!For laurent!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      timer1.stop();


      msg.str("");
      msg << "setDaqSuperMode() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setScopeLength(theLocalFedSettings.getScopeLength());
      timer1.stop();
      msg.str("");
      msg << "setScopeLength() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllFrameThresholds();
      timer1.stop();
      msg.str("");
      msg << "setAllFrameThresholds() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
            
      timer1.start();
      setTestRegister(theLocalFedSettings.getTestRegister());
      timer1.stop();
      msg.str("");
      msg << "setTestRegister() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setFedId(theLocalFedSettings.getFedId());
      timer1.stop();
      msg.str("");
      msg << "setFedId() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setBeFpgaReadRoute(theLocalFedSettings.getBeFpgaReadRoute());
      timer1.stop();
      msg.str("");
      msg << "setBeFpgaReadRoute() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setHeaderFormatType(theLocalFedSettings.getHeaderFormatType());
      timer1.stop();
      msg.str("");
      msg << "setHeaderFormatType() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllFedFeUnitDisable();
      timer1.stop();
      msg.str("");
      msg << "setAllFedFeUnitDisable() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      // moved to start and stop setAllOptoRxControls();
      timer1.stop();
      msg.str("");
      msg << "setAllOptoRxControls() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllCmMedianOverrideValues();
      timer1.stop();
      msg.str("");
      msg << "setAllCmMedianOverrideValues() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllDelays();
      timer1.stop();
      msg.str("");
      msg << "setAllDelays() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllComplements();
      timer1.stop();
      msg.str("");
      msg << "setAllComlements() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      sendTrimDacReset(Fed9UAddress::FEBROADCAST);
      timer1.stop();
      msg.str("");
      msg << "sendTrimDacReset() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setAllTrimDacOffsets();
      timer1.stop();
      msg.str("");
      msg << "setAlltrimDacOffsets() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      /* We no longer need to do this since it is now done in the start() method!!!!!!!!!!
	timer1.start();
	setAllAdcControls();
	timer1.stop();
	//cout << "setAllAdcControls() takes " << timer1.read() << "us" << endl;
      */
      //setDaqEventType(theLocalFedSettings.getDaqEventType());
      // Does this really need to be set here ??
      timer1.start();
      setDaqFov(theLocalFedSettings.getDaqFov());
      timer1.stop();
      msg.str("");
      msg << "setDaqFov() takes " << timer1.read() << "us" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      
      timer1.start();
      setTriggerSource(theLocalFedSettings.getTriggerSource());
      timer1.stop();
      msg.str("");
      //cout << "sendTriggerSource() takes " << timer1.read() << "us" << endl;


      fed9Uwait(10000); // this wait is required for some reason to ensure that the super mode is set correctly after the daq mode is set.

      beFpgaSoftReset();
      addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      feFpgaSoftReset(addr);

      sendVLinkReset();

      msg.str("");
      msg << hex << " value stored in description " << (( theLocalFedSettings.getDaqSuperMode() == FED9U_SUPER_MODE_FAKE ) ? "fake mode!" : "something else!!!" ) << endl;
      msg << hex << " value stored in FED " << static_cast<u16>(getDaqSuperMode()) << (( theLocalFedSettings.getDaqSuperMode() == FED9U_SUPER_MODE_FAKE ) ? "fake mode!" : "something else!!!" ) << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      // <JRF date="22/06/2009"> must set the bunch crossing offset value to be sure it's getting setup correctly
      setBunchCrossingOffset(BUNCH_CROSSING_OFFSET);
      // </JRF>
      // <JEC date="1/12/2006"> Set the bunch crossing orbit total
      setBunchCrossingOrbitTotal(BUNCH_CROSSING_ORBIT_TOTAL);
      std::cout << "Bx orbit total is set to: " << getBunchCrossingOrbitTotal() << std::endl;
      //</JEC>
      
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::init."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  Fed9UVmeDevice& Fed9UVmeDevice::fastInit() throw (Fed9UVmeDeviceException) {
    try {
      // might need this if counters are out of sync     sendFedReset();
      timer2.start();
      beFpgaSoftReset();
      Fed9UAddress addr;
      addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      feFpgaSoftReset(addr);
      stop();
      timer2.stop();
      cout << "Time for Fast Init = " << timer2.read() << "us" << endl;
      ICUTILS_VERIFY(theLocalFedSettings.getVmeFirmwareVersion() >= 0x2100032A)( theLocalFedSettings.getVmeFirmwareVersion() )(0x2100032A).msg("you must have a higher firmware version for the Vme in order for fast Init to operate completely. The fed will still operate correctly now, but you may find some counters are not reset!!!").error();
      sendVLinkReset();
      return *this;
    }  
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::init."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This method will initialise all the strips on a FED so that they are ready to take data.
   *
   * Initialises the pedestals, disabled strips, low and high cluster thresholds from theLocalFedSettings.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::initStrips() throw (Fed9UVmeDeviceException) {
    try {
      timer2.reset();
      timer1.reset();
      timer1Value=timer2Value=0;
      timer2.start();
      methodCallCounter = 0;
      setAllStripData();
      timer2.stop();
      timer2Value+=timer2.read();
      cout << "Time taken to upload strips in c++ = " << timer2Value << endl;
      cout << "Time taken to upload strips in CAEN upload  = " << timer1Value << endl;
	cout << "Number of calls to VmeBase commands = " << methodCallCounter << endl;
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
  /**
   * This method will initialise the Fake Event RAMs on a FED so that they are ready to output fake events.
   *
   * 
   */
  Fed9UVmeDevice& Fed9UVmeDevice::initFakeEvent(bool useEventFile, bool blockUpload) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(theLocalFedSettings.getFeFirmwareVersion() >= 0x23100320)(theLocalFedSettings.getFeFirmwareVersion())(0x23100320).msg("you must have a higher firmware version for the front end.").error();
      cout << "use event file = " << (useEventFile ? "true" : "false") << "filename = " << theLocalFedSettings.getFakeEventFile() << endl;
      ifstream fs(theLocalFedSettings.getFakeEventFile().c_str());	  

      if (useEventFile) {
	try {
   
	  ICUTILS_VERIFY(fs.is_open())(fs).error().msg("can't open file for reading");
	
	} catch ( const ICUtils::ICException& e ) {
	  cout << "Failed to load Fake Event Data File, switching to default Fake Event Ramp Data!!!!!" << endl;
	  useEventFile = false;
	} catch ( ... ) {
	  cout << "unknown exception... Failed to load Fake Event Data File, switching to default Fake Event Ramp Data!!!!!" << endl;
	  useEventFile = false;
	}
      }
      if (!useEventFile) {
	timer1Value=timer2Value=0;
	timer2.start();      
	cout << "About to load fake events!" << endl;
	setAllFakeEvents(NULL,blockUpload);
	cout << "Loaded fake events!" << endl;
	timer2.stop();
	timer2Value+=timer2.read();
	cout << "Time taken to upload FakeEvent in c++ = " << timer2Value << endl;
	cout << "Time taken to upload FakeEvent in CAEN upload  = " << timer1Value << endl;
      
      }
      else {
	cout << "just about to construct fake event" << endl;
#ifdef EVENT_STREAMLINE
	Fed9UEventStreamLine fakeEvent(fs);
#else
	Fed9UEvent fakeEvent(fs);
#endif
	cout << "constructed fake event" << endl;
	fs.close();
	cout << "closed file" << endl;
	timer1Value=timer2Value=0;
	timer2.start();
	methodCallCounter=0;
	setAllFakeEvents(&fakeEvent,blockUpload);
	timer2.stop();
	timer2Value+=timer2.read();
	cout << "Time taken to upload FakeEvent in c++ = " << timer2Value << endl;
	cout << "Time taken to upload FakeEvent in CAEN upload  = " << timer1Value << endl;
	cout << "Number of calls to VmeBase commands = " << methodCallCounter << endl;
 	cout << "set fake events completed and fakeEvent Buffer pointer = " << hex << &fakeEvent << dec << endl;
      }
      //getFakeEventFrame(Fed9UAddress().setFedChannel(1));
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initFakeEvent."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initFakeEvent Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initFakeEvent Caught unknown exception."));
    }
  }
  
  /**
   * This will initialise the temperature monitors, TTCrx device and the voltage monitor.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::initAuxiliaryChips() throw (Fed9UVmeDeviceException) {
    try {
      setAllTempControlData();
      //      setTtcrx( theLocalFedSettings.getTtcrx() ); // moved to the init() for the bt.
      setVoltageMonitor( theLocalFedSettings.getVoltageMonitor() );

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initAuxiliaryChips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initFakeEvent. Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::initFakeEvent. Caught unknown exception."));
    }
  }

  /**
   * This enables the FED so that it can start taking data.
   *
   * Sets the APV disable flag settings and the BE FPGA disable flag settings.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::start() throw (Fed9UVmeDeviceException) {
    try {
      cout << "Entering Fed9UVmeDevice::start() in FED ID : " << getFedId() << endl;
	//Ensures that there are no events left over from a previous run.
      u32 purgedEventsNo = 0;
      u32 eventCounter = 0;
      u32 i = 0;
      do {
	try {
	  cout << "In Fed9UVmeDevice::start() just before checkEvent() in FED ID : " << getFedId() << ", i=" << i << endl;
		checkEvent();
          cout << "In Fed9UVmeDevice::start() just after  checkEvent() in FED ID : " << getFedId()  << ", i=" << i<< endl;
	} catch (ICUtils::ICException & exc) {
	   cout << "In Fed9UVmeDevice::start() just after Catch and before purgeEvents in FED ID : " << getFedId()  << ", i=" << i << endl;
	  try {
	     purgeEvents(purgedEventsNo,eventCounter);
	  } catch (ICUtils::ICException & exc2) {
             cout << "In Fed9UVmeDevice::start()  FED ID : " << getFedId()  << ", i=" << i << " Resynch required!!! Unable to purge Events most likely because we are OOS and not pushing events" << exc2.what() << endl;
	  }
	  i+=1;
	  ICUTILS_VERIFY(i<1000000)(i)(purgedEventsNo).msg("could not purge all events in buffer... we purged i = 1000000 times!!!!!").error();
	  continue;
	}
	break;
      } while ( i < 1 ) ; //1000000); reduced for debugging
      cout << "In Fed9UVmeDevice::start() just after Loop in FED ID : " << getFedId() << endl;

      setFedBeFpgaDisable(false);
      setAllApvDisable();
      setAllAdcControls(); //reenable all the adcs with the configuration originally specified in the local Setings description
      setAllOptoRxControls(); //reenable all optoRX

      //cout << "all ADCs and APVs are enabled!!!" << endl;
      _isStarted = true;
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::start."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Stops the FED after it has performed a run.
   *
   * This is done by disabling all the APVs and the BE FPGA.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::stop() throw (Fed9UVmeDeviceException) {
    try {
      // disable all Apvs from finding frames in the front end
      disableAllApvs();
      
      // disable the be from accepting triggers and fe frames and headers
      setFedBeFpgaDisable(true);
      
      // disable the adcs to save power
      // we have to be a bit tricky here since we do not want to lose the settings for the adcs, 
      // we must store the configuration first before disabling them and then restor the configuration

      Fed9UDescription tempBackupDescription = theLocalFedSettings;
      theLocalFedSettings = Fed9UDescription(); // reset the local fed settings description to a default in which all adcs are disabled
      setAllAdcControls();
      setAllOptoRxControls();
      theLocalFedSettings = tempBackupDescription; // restore the original settings description.
      _isStarted=false;

      //Verifys that the number of triggers received is equal to the number of frames received.
      ICUTILS_VERIFYX(getTriggerNumber() == getQdrTotalFrameCount(), Fed9UVmeDeviceException)(getTriggerNumber())(getQdrTotalFrameCount()).warn().msg("The number of triggers received does not match the number of frames that were received");
      
      return *this;

    }
    catch (const ICUtils::ICException& e)
      {
	RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::stop."));
      }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Scans through all the FED delay settings.
   *
   * It can either scan through a nano second range or fine and coarse skew setting range
   * for a given channel. It waits for 500ms after each value.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::scanDelayValues(const Fed9UAddress& fedChannel, bool nanoDelay, bool scanCoarse) throw (Fed9UVmeDeviceException) {
    try {
      if (nanoDelay) {
	//Scan from 0 to 400ns.
	for (u32 nanoSecondDelay(0); nanoSecondDelay < 400; ++nanoSecondDelay) {
	  //Set the nano second delay value.
	  setDelay(fedChannel, nanoSecondDelay);
	  //Pause for a quick rest.
	  fed9Uwait(500000);
	}
      } else {
	u32 coarseDelayUpperLimit(1);
	if (scanCoarse) {
	  //Set the upper limit on the coarse delay range to maximum if we want to scan that range too.
	  coarseDelayUpperLimit = 16;
	}
	//Determine what firmware is being used to decide the fine delay range.
	u32 fineDelayUpperLimit(64);
	if (getDelayFirmwareVersion() < FED9U_DELAY_FIRMWARE_VERSION_6_BIT) {
	  fineDelayUpperLimit = 32;
	}
	//Create two loops to run over the coarse and fine delay values.
	for (u32 coarseDelay(0); coarseDelay < coarseDelayUpperLimit; ++coarseDelay) {
	  for (u32 fineDelay(0); fineDelay < fineDelayUpperLimit; ++fineDelay) {
	    //Now scan through the desired range over the desired channel.
	    setDelay(fedChannel, coarseDelay, fineDelay);
	    //Pause for a quick rest.
	    fed9Uwait(500000);
	  }
	}
      }
      return *this;
    }
    catch (const ICUtils::ICException& e)
      {
	RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::scanDelayValue."));
      }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  const Fed9UVmeDevice& Fed9UVmeDevice::checkEvent() const throw (Fed9UVmeDeviceException) {
    try {
      //Verifys that there is no event left in the VME buffer.
      fed9Uwait(1000);
      u32 checkEvent = hasEvent() & 0x01;
      ICUTILS_VERIFY(checkEvent != 1)(checkEvent).error().msg("There is still an event in the buffer");
      //Checks the BE FPGA event status registers to check that there is not an event present.
      vector<u32> beFpgaEventStatus(4,0);
      beFpgaEventStatus       = getBeEventCounterStatus();
      u32 currentUnreadWords  = beFpgaEventStatus[1];
      u32 currentUnreadFrames = beFpgaEventStatus[2];
      ICUTILS_VERIFY(currentUnreadWords == 0 && currentUnreadWords == currentUnreadFrames)(currentUnreadWords)(currentUnreadFrames).error().msg("The BE FPGA event status buffer still shows an event present");
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::checkEvent."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method can be used to compare the values of the Fed9UDescrition to those on the FED for the BE FPGA.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::backEndCheck() const throw (Fed9UVmeDeviceException) {
    try {

      ICUTILS_VERIFY(getTriggerSource() == theLocalFedSettings.getTriggerSource())(getTriggerSource())(theLocalFedSettings.getTriggerSource()).msg("The trigger source setting on the FED does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(getTestRegister() == theLocalFedSettings.getTestRegister())(getTestRegister())(theLocalFedSettings.getTestRegister()).msg("The getTestRegister() setting on the FED does not match that in the theLocalFedSettings");

      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	Fed9UAddress feFpga;
	feFpga.setFedFeUnit(feFpga_);

	ICUTILS_VERIFY(getFedFeUnitDisable(feFpga) == theLocalFedSettings.getFedFeUnitDisable(feFpga))(static_cast<int>(feFpga.getExternalFedFeUnit()))(getFedFeUnitDisable(feFpga))(theLocalFedSettings.getFedFeUnitDisable(feFpga)).msg("The getFedFeUnitDisable() setting on the FED does not match that in the theLocalFedSettings");
      }

      ICUTILS_VERIFY(getBeFpgaReadRoute() == theLocalFedSettings.getBeFpgaReadRoute())(getBeFpgaReadRoute())(theLocalFedSettings.getBeFpgaReadRoute()).msg("The getBeFpgaReadRoute() setting on the FED does not match that in the theLocalFedSettings");

       ICUTILS_VERIFY(getHeaderFormatType() == theLocalFedSettings.getHeaderFormatType())(getHeaderFormatType())(theLocalFedSettings.getHeaderFormatType()).msg("The getHeaderFormatType() setting on the FED does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(getFedBeFpgaDisable() == theLocalFedSettings.getFedBeFpgaDisable())(getFedBeFpgaDisable())(theLocalFedSettings.getFedBeFpgaDisable()).msg("The getFedBeFpgaDisableFlag() setting on the FED does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(getFedId() == theLocalFedSettings.getFedId())(getFedId())(theLocalFedSettings.getFedId()).msg("The getFedId() setting on the FED does not match that in the theLocalFedSettings");

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::backEndCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }







  /**
   * This method can be used to compare the values of the Fed9UDescrition to those on the FED for the FE FPGA.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::frontEndCheck(bool CMMedianOverridOnly) const throw (Fed9UVmeDeviceException) {
    try {
      stringstream errorMessage;
      try {
	ICUTILS_VERIFY(getScopeLength() == theLocalFedSettings.getScopeLength())(getScopeLength())(theLocalFedSettings.getScopeLength()).msg("The Scope Length setting on the FED does not match that in the theLocalFedSettings").error();
      }
      catch (ICUtils::ICException & e) {
	errorMessage << e.what() << endl;
      }
      
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; ++feFpga_) {
	Fed9UAddress addr;
	addr.setFedFeUnit(feFpga_);
	
	//u16 inputOffset  = getOptoRxInputOffset(addr);
	//u16 outputOffset = getOptoRxOutputOffset(addr);
	//u16 capacitor    = getOptoRxCapacitor(addr);
	
	try {
	  //ICUTILS_VERIFY(inputOffset == theLocalFedSettings.getOptoRxInputOffset(addr))(static_cast<int>(Fed9UAddress().setFedFeUnit(feFpga_).getExternalFedFeUnit()))(inputOffset)(theLocalFedSettings.getOptoRxInputOffset(addr)).msg("The setting OptoRxInputOffsets on the FED does not match that in the theLocalFedSettings").error();
	}
	catch (ICUtils::ICException & e) {
	  errorMessage << e.what() << endl;
	}
	
	try {
	  //ICUTILS_VERIFY(outputOffset == theLocalFedSettings.getOptoRxOutputOffset(addr))(static_cast<int>(Fed9UAddress().setFedFeUnit(feFpga_).getExternalFedFeUnit()))(outputOffset)(theLocalFedSettings.getOptoRxOutputOffset(addr)).msg("The setting OptoRxOutputOffsets on the FED does not match that in the theLocalFedSettings").error();
	}
	catch (ICUtils::ICException & e) {
	  errorMessage << e.what() << endl;
	}
	
	try {
	  //ICUTILS_VERIFY(capacitor == theLocalFedSettings.getOptoRxCapacitor(addr))(static_cast<int>(Fed9UAddress().setFedFeUnit(feFpga_).getExternalFedFeUnit()))(capacitor)(theLocalFedSettings.getOptoRxCapacitor(addr)).msg("The setting OptoRxCapacitor on the FED does not match that in the theLocalFedSettings").error();
	}
	catch (ICUtils::ICException & e) {
	  errorMessage << e.what() << endl;
	}
	
	bool medianOverrideDisable;
	vector<u16> medianValues = getCmMedianOverride(addr, medianOverrideDisable);
	bool descriptionMedianOverrideDisable;
	vector<u16> descriptionMedianValues = theLocalFedSettings.getCmMedianOverride(addr, descriptionMedianOverrideDisable);
	try {
	  ICUTILS_VERIFY(medianOverrideDisable == descriptionMedianOverrideDisable)(static_cast<int>(Fed9UAddress().setFedFeUnit(feFpga_).getExternalFedFeUnit()))(medianOverrideDisable)(descriptionMedianOverrideDisable).msg("The setting median over-ride disable setting on the FED does not match that in the theLocalFedSettings").error();
	}
	catch (ICUtils::ICException & e) {
	  errorMessage << e.what() << endl;
	}
	
	for (u32 fpgaApv_ = 0; fpgaApv_ < APVS_PER_FEUNIT; ++fpgaApv_) {
	  try {
	    ICUTILS_VERIFY(medianValues[fpgaApv_] == descriptionMedianValues[fpgaApv_])(static_cast<int>(Fed9UAddress().setFeUnitApv(fpgaApv_).getExternalFeUnitApv()))(medianValues[fpgaApv_])(descriptionMedianValues[fpgaApv_]).msg("The median over-ride value on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	}
	
      }


// if we only want to check Median Override values we verify here and return
      if(CMMedianOverridOnly) {
        ICUTILS_VERIFY(errorMessage.str().size() == 0).msg(errorMessage.str().c_str()).error();
        return *this;
      }
      // the following map is used to map a channel number directly to it's paired channel number, i.e. channel 0 maps to 3 since 3 is stored at location 0 in the map.
      const u16 channelPairMap[12] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8};

      Fed9UAddress tempAddress;

      for (u32 fedChannel_ = 0; fedChannel_ < CHANNELS_PER_FED; ++fedChannel_) {
	Fed9UAddress addr;
	
	addr.setFedChannel(fedChannel_);
	
	//If all the APVs on an ADC channel pair are disabled then the ADC is off so we will not match description, but it should be set to 0.
	tempAddress.setFedChannel(fedChannel_); //note you have to set the actual channel to ensure you get the write set of 4 apvs below <JRF> BUG FIX 18/10/2006
	Fed9UAddress apv0(tempAddress.getFedChannel(), 0);
	Fed9UAddress apv1(tempAddress.getFedChannel(), 1);
	tempAddress.setFedChannel(channelPairMap[fedChannel_%12]);
	Fed9UAddress apv2( tempAddress.getFedChannel(), 0 );
	Fed9UAddress apv3( tempAddress.getFedChannel(), 1 );
	const bool channelPairDisabled=( theLocalFedSettings.getApvDisable(apv0) && theLocalFedSettings.getApvDisable(apv1) && theLocalFedSettings.getApvDisable(apv2) && theLocalFedSettings.getApvDisable(apv3) );
	
	
	
	try {
	  ICUTILS_VERIFY(getFrameThreshold(addr) == theLocalFedSettings.getFrameThreshold(addr))(static_cast<int>(Fed9UAddress().setFedChannel(fedChannel_).getExternalFedChannel()))(getFrameThreshold(addr))(theLocalFedSettings.getFrameThreshold(addr)).msg("The setting frame thresholds on the FED does not match that in the theLocalFedSettings").error();
	}
	catch (ICUtils::ICException & e) {
	  errorMessage << e.what() << endl;
	}
	
	if (_isStarted && !channelPairDisabled) {// note that when the fed is stopped all adcs are off to conserve power, also if all apvs on a channel pair are disabled then the adc is disabled 

	  Fed9UAdcControls theFedAdcControl( getAdcControls(addr) );
	  Fed9UAdcControls theLocalFedAdcSettings( theLocalFedSettings.getAdcControls(addr) );
	  
	  try {
	    ICUTILS_VERIFY(theFedAdcControl._dfsen == theLocalFedAdcSettings._dfsen)(static_cast<int>(Fed9UAddress().setFedChannel(fedChannel_).getExternalFedChannel()))(theFedAdcControl._dfsen)(theLocalFedAdcSettings._dfsen).msg("The ADC control setting dfsen on the FED does not match that in the theLocalFedAdcSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }	  
	  try {
	    ICUTILS_VERIFY(theFedAdcControl._dfsval == theLocalFedAdcSettings._dfsval)(static_cast<int>(Fed9UAddress().setFedChannel(fedChannel_).getExternalFedChannel()))(theFedAdcControl._dfsval)(theLocalFedAdcSettings._dfsval).msg("The ADC control setting dfsval on the FED does not match that in the theLocalFedAdcSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	  try {
	    ICUTILS_VERIFY(theFedAdcControl._s1 == theLocalFedAdcSettings._s1)(static_cast<int>(Fed9UAddress().setFedChannel(fedChannel_).getExternalFedChannel()))(theFedAdcControl._s1)(theLocalFedAdcSettings._s1).msg("The ADC control setting s1 on the FED does not match that in the theLocalFedAdcSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	  
	  try {
	    ICUTILS_VERIFY(theFedAdcControl._s2 == theLocalFedAdcSettings._s2)(static_cast<int>(Fed9UAddress().setFedChannel(fedChannel_).getExternalFedChannel()))(theFedAdcControl._s2)(theLocalFedAdcSettings._s2).msg("The ADC control setting s2 on the FED does not match that in the theLocalFedAdcSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	}
      }
      
      for (u32 fedApv_ = 0; fedApv_ < APVS_PER_FED; fedApv_++) {
	Fed9UAddress addr;
	addr.setFedApv(fedApv_);
	
	vector<u16> pedestals(128,0);
	vector<bool> disableStrips(128,0);
	u16 numberValidStrips;
	getPedsAndDisabledStrips(addr, pedestals, disableStrips, numberValidStrips);
	
	vector<u16> highThresholds(128,0);
	vector<u16> lowThresholds(128,0);
	getClusterThresholds(addr, highThresholds, lowThresholds);
	
	vector<Fed9UStripDescription> apvStripDescription( theLocalFedSettings.getFedStrips().getApvStrips(addr) );
	
	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; apvStrip_++) {
	  addr.setApvStrip(apvStrip_);
	  
	  try {
	    ICUTILS_VERIFY(pedestals[apvStrip_] == apvStripDescription[apvStrip_].getPedestal())
	      (static_cast<int>(addr.getExternalFedChannel()))
	      (static_cast<int>(addr.getExternalChannelApv()))
	      (static_cast<int>(addr.getExternalApvStrip()))
	      (pedestals[apvStrip_])
	      (apvStripDescription[apvStrip_].getPedestal()).msg("The pedestal settings on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	  
	  try {
	    ICUTILS_VERIFY(disableStrips[apvStrip_] == apvStripDescription[apvStrip_].getDisable())
	      (static_cast<int>(addr.getExternalFedChannel()))
	      (static_cast<int>(addr.getExternalChannelApv()))
	      (static_cast<int>(addr.getExternalApvStrip()))
	      (static_cast<int>(disableStrips[apvStrip_]))
	      (apvStripDescription[apvStrip_].getDisable()).msg("The valid channel setting on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	  
	  try {
	    ICUTILS_VERIFY(highThresholds[apvStrip_] == apvStripDescription[apvStrip_].getHighThreshold())
	      (static_cast<int>(addr.getExternalFedChannel()))
	      (static_cast<int>(addr.getExternalChannelApv()))
	      (static_cast<int>(addr.getExternalApvStrip()))
	      (highThresholds[apvStrip_])
	      (apvStripDescription[apvStrip_].getHighThreshold()).msg("The high threshold settings on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	  
	  try {
	    ICUTILS_VERIFY(lowThresholds[apvStrip_] == apvStripDescription[apvStrip_].getLowThreshold())(static_cast<int>(Fed9UAddress().setApvStrip(apvStrip_).getExternalApvStrip()))(lowThresholds[apvStrip_])(apvStripDescription[apvStrip_].getLowThreshold()).msg("The low threhold settings on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	}

	if (_isStarted) {// note that when the fed is stopped all adcs are off to conserve power 	
	  try {
	    ICUTILS_VERIFY(getApvDisable(addr) == theLocalFedSettings.getApvDisable(addr))(static_cast<int>(Fed9UAddress().setFedApv(fedApv_).getExternalFedApv()))(getApvDisable(addr))(theLocalFedSettings.getApvDisable(addr)).msg("The setting APV disable flag on the FED does not match that in the theLocalFedSettings").error();
	  }
	  catch (ICUtils::ICException & e) {
	    errorMessage << e.what() << endl;
	  }
	}

      }
      ICUTILS_VERIFY(errorMessage.str().size() == 0).msg(errorMessage.str().c_str()).error();      
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::frontEndCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method can be used to compare the values of the Fed9UDescrition to
   * those on the FED for the VME FPGA.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::vmeCheck() const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getClock() == theLocalFedSettings.getClock())(getClock())(theLocalFedSettings.getClock()).msg("The setting on the FED does not match that in the theLocalFedSettings");

      //First get a copy of the settings on both the FED and in the description of the voltage monitor settings.
      Fed9UVoltageControl theLocalVoltageControlSettings( theLocalFedSettings.getVoltageMonitor() );
      //The Fed9UVoltageMonitor can be constructed with theLocalVoltageSettings (which it is being compared to) because the read methods do not use this class.
      //By making the object const it prevents write methods from being performed. Although these would only update a local copy of voltage controller anyway.
      const Fed9UVoltageMonitor theVoltageMonitor(*theFed, theLocalVoltageControlSettings);

      ICUTILS_VERIFY(theVoltageMonitor.getStandBy() == theLocalVoltageControlSettings.getStandBy())(theVoltageMonitor.getStandBy())(theLocalVoltageControlSettings.getStandBy()).msg("The StandBy setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get2Point5VoltMax() , theLocalVoltageControlSettings.get2Point5VoltMax()))(theVoltageMonitor.get2Point5VoltMax())(theLocalVoltageControlSettings.get2Point5VoltMax()).msg("The 2.5V max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get2Point5VoltMin() , theLocalVoltageControlSettings.get2Point5VoltMin()))(theVoltageMonitor.get2Point5VoltMin())(theLocalVoltageControlSettings.get2Point5VoltMin()).msg("The 2.5V min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get3Point3VoltMax() , theLocalVoltageControlSettings.get3Point3VoltMax()))(theVoltageMonitor.get3Point3VoltMax())(theLocalVoltageControlSettings.get3Point3VoltMax()).msg("The 3.3V max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get3Point3VoltMin() , theLocalVoltageControlSettings.get3Point3VoltMin()))(theVoltageMonitor.get3Point3VoltMin())(theLocalVoltageControlSettings.get3Point3VoltMin()).msg("The 3.3V min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get5VoltMax() , theLocalVoltageControlSettings.get5VoltMax()))(theVoltageMonitor.get5VoltMax())(theLocalVoltageControlSettings.get5VoltMax()).msg("The 5V max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get5VoltMin() , theLocalVoltageControlSettings.get5VoltMin()))(theVoltageMonitor.get5VoltMin())(theLocalVoltageControlSettings.get5VoltMin()).msg("The 5V min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get12VoltMax() , theLocalVoltageControlSettings.get12VoltMax()))(theVoltageMonitor.get12VoltMax())(theLocalVoltageControlSettings.get12VoltMax()).msg("The 12V max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.get12VoltMin() , theLocalVoltageControlSettings.get12VoltMin()))(theVoltageMonitor.get12VoltMin())(theLocalVoltageControlSettings.get12VoltMin()).msg("The 12V min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.getCoreVoltageMax() , theLocalVoltageControlSettings.getCoreVoltageMax()))(theVoltageMonitor.getCoreVoltageMax())(theLocalVoltageControlSettings.getCoreVoltageMax()).msg("The core voltage max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.getCoreVoltageMin() , theLocalVoltageControlSettings.getCoreVoltageMin()))(theVoltageMonitor.getCoreVoltageMin())(theLocalVoltageControlSettings.getCoreVoltageMin()).msg("The core voltage min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.getSupplyVoltageMax() , theLocalVoltageControlSettings.getSupplyVoltageMax()))(theVoltageMonitor.getSupplyVoltageMax())(theLocalVoltageControlSettings.getSupplyVoltageMax()).msg("The supply voltage max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(floatEqual(theVoltageMonitor.getSupplyVoltageMin() , theLocalVoltageControlSettings.getSupplyVoltageMin()))(theVoltageMonitor.getSupplyVoltageMin())(theLocalVoltageControlSettings.getSupplyVoltageMin()).msg("The supply voltage min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getExternalTempMax() == theLocalVoltageControlSettings.getExternalTempMax())(theVoltageMonitor.getExternalTempMax())(theLocalVoltageControlSettings.getExternalTempMax()).msg("The external temperature max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getExternalTempMin() == theLocalVoltageControlSettings.getExternalTempMin())(theVoltageMonitor.getExternalTempMin())(theLocalVoltageControlSettings.getExternalTempMin()).msg("The external temperature min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getInternalTempMax() == theLocalVoltageControlSettings.getInternalTempMax())(theVoltageMonitor.getInternalTempMax())(theLocalVoltageControlSettings.getInternalTempMax()).msg("The internal temperature max limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getInternalTempMin() == theLocalVoltageControlSettings.getInternalTempMin())(theVoltageMonitor.getInternalTempMin())(theLocalVoltageControlSettings.getInternalTempMin()).msg("The internal temperature min limit setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getTempOffset() == theLocalVoltageControlSettings.getTempOffset())(theVoltageMonitor.getTempOffset())(theLocalVoltageControlSettings.getTempOffset()).msg("The temperature offset setting on the voltage monitor does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY(theVoltageMonitor.getOffsetTempSelect() == theLocalVoltageControlSettings.getOffsetTempSelect())(theVoltageMonitor.getOffsetTempSelect())(theLocalVoltageControlSettings.getOffsetTempSelect()).msg("The temperature offfset selection setting on the voltage monitor does not match that in the theLocalFedSettings");

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::vmeCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method can be used to compare the values of the Fed9UDescrition to those on the Fed
   * for the higher level commands.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::higherLevelCheck() const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(getDaqMode() == theLocalFedSettings.getDaqMode())(getDaqMode())(theLocalFedSettings.getDaqMode()).msg("The setting FED DAQ mode on the FED does not match that in the theLocalFedSettings");

      //The LM82 is unique in that it can be set for the eight FE FPGAs and
      //the BE FPGA. Therefore the following loop will cover all the possible
      //values that the Fed9UAddress can take for the LM82 command.
      const u32 ALL_FPGA_ADDRESSES = 11;
      for (u32 fedFpga_ = 0; fedFpga_ < ALL_FPGA_ADDRESSES; fedFpga_++) {
	if (fedFpga_ == 8) {
	  //There is no FPGA at address number 8.
	} else {
	  //This loops over all the valid addresses for the FPGAs.
	  //It covers the FE and BE FPGAs.
	  Fed9UAddress addr;
	  addr.setFedFpga(fedFpga_);

	  ICUTILS_VERIFY(getTempControl(addr).getLm82High() == theLocalFedSettings.getTempControl(addr).getLm82High())(static_cast<int>(Fed9UAddress().setFedFeUnit(fedFpga_).getExternalFedFeUnit()))(getTempControl(addr).getLm82High())(theLocalFedSettings.getTempControl(addr).getLm82High()).msg("The LM82 temperature monitor high point setting on the FED does not match that in the theLocalFedSettings").warn();

	  ICUTILS_VERIFY(getTempControl(addr).getFpgaHigh() == theLocalFedSettings.getTempControl(addr).getFpgaHigh())(static_cast<int>(Fed9UAddress().setFedFeUnit(fedFpga_).getExternalFedFeUnit()))(getTempControl(addr).getFpgaHigh())(theLocalFedSettings.getTempControl(addr).getFpgaHigh()).msg("The BE FPGA temperature high point setting on the FED does not match that in the theLocalFedSettings").warn();

	  ICUTILS_VERIFY(getTempControl(addr).getCritical() == theLocalFedSettings.getTempControl(addr).getCritical())(static_cast<int>(Fed9UAddress().setFedFeUnit(fedFpga_).getExternalFedFeUnit()))(getTempControl(addr).getCritical())(theLocalFedSettings.getTempControl(addr).getCritical()).msg("The critical temperature setting on the FED does not match that in the theLocalFedSettings").warn();
	}
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::HigherLevelCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method can be used to compare the values of the Fed9UDescrition to those
   * on the FED for the TTCrx device.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::ttcrxCheck() const throw (Fed9UVmeDeviceException) {
    try {
      //Get the current TTCrx settings from the description.
      Fed9UTtcrxDescription ttcrxDescription(theLocalFedSettings.getTtcrx());
      //Creates a Fed9UTtcrxDevice to compare the settings in the TTCrx description with.
      Fed9UTtcrxDevice ttcrxDevice(*theFed, ttcrxDescription);

      //Reads back all the read/write settings from the TTCrx on the FED and ensures that they are the same as that in the description.
      ICUTILS_VERIFY( ttcrxDevice.getL1AcceptCoarseDelay() == ttcrxDescription.getL1AcceptCoarseDelay() )( ttcrxDevice.getL1AcceptCoarseDelay() )( ttcrxDescription.getL1AcceptCoarseDelay() ).msg("The L1AcceptCoarseDelay setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getBrcstStrTwoCoarseDelay() == ttcrxDescription.getBrcstStrTwoCoarseDelay() )( ttcrxDevice.getBrcstStrTwoCoarseDelay() )( ttcrxDescription.getBrcstStrTwoCoarseDelay() ).msg("The BrcstStrTwoCoarseDelay setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getClockDesOneFineDelay() == ttcrxDescription.getClockDesOneFineDelay() )( ttcrxDevice.getClockDesOneFineDelay() )( ttcrxDescription.getClockDesOneFineDelay() ).msg("The ClockDesOneFineDelay setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getClockDesTwoFineDelay() == ttcrxDescription.getClockDesTwoFineDelay() )( ttcrxDevice.getClockDesTwoFineDelay() )( ttcrxDescription.getClockDesTwoFineDelay() ).msg("The ClockDesTwoFineDelay setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getDllPumpCurrent() == ttcrxDescription.getDllPumpCurrent() )( ttcrxDevice.getDllPumpCurrent() )( ttcrxDescription.getDllPumpCurrent() ).msg("The DllPumpCurrent setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getPllPumpCurrent() == ttcrxDescription.getPllPumpCurrent() )( ttcrxDevice.getPllPumpCurrent() )( ttcrxDescription.getPllPumpCurrent() ).msg("The PllPumpCurrent setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getIacId() == ttcrxDescription.getIacId() )( ttcrxDevice.getIacId() )( ttcrxDescription.getIacId() ).msg("The IacId setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getI2cId() == ttcrxDescription.getI2cId() )( ttcrxDevice.getI2cId() )( ttcrxDescription.getI2cId() ).msg("The I2cId setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getHammingCheckingDisable() == ttcrxDescription.getHammingCheckingDisable() )( ttcrxDevice.getHammingCheckingDisable() )( ttcrxDescription.getHammingCheckingDisable() ).msg("The Hamming CheckingDisable setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getCounterOperation() == ttcrxDescription.getCounterOperation() )( ttcrxDevice.getCounterOperation() )( ttcrxDescription.getCounterOperation() ).msg("The CounterOperation setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getDeskewedClock2Selected() == ttcrxDescription.getDeskewedClock2Selected() )( ttcrxDevice.getDeskewedClock2Selected() )( ttcrxDescription.getDeskewedClock2Selected() ).msg("The DeskewedClock2Selected setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getDeskewedClock2Disable() == ttcrxDescription.getDeskewedClock2Disable() )( ttcrxDevice.getDeskewedClock2Disable() )( ttcrxDescription.getDeskewedClock2Disable() ).msg("The DeskewedClock2Disable setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getClockL1AcceptDisable() == ttcrxDescription.getClockL1AcceptDisable() )( ttcrxDevice.getClockL1AcceptDisable() )( ttcrxDescription.getClockL1AcceptDisable() ).msg("The ClockL1AcceptDisable setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getNonDeskewedClockDisable() == ttcrxDescription.getNonDeskewedClockDisable() )( ttcrxDevice.getNonDeskewedClockDisable() )( ttcrxDescription.getNonDeskewedClockDisable() ).msg("The NonDeskewedClockDisable setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getParrallelOutputDisable() == ttcrxDescription.getParrallelOutputDisable() )( ttcrxDevice.getParrallelOutputDisable() )( ttcrxDescription.getParrallelOutputDisable() ).msg("The setting on the TTCrx does not match that in the theLocalFedSettings");

      ICUTILS_VERIFY( ttcrxDevice.getSerialBDisable() == ttcrxDescription.getSerialBDisable() )( ttcrxDevice.getSerialBDisable() )( ttcrxDescription.getSerialBDisable() ).msg("The setting on the TTCrx does not match that in the theLocalFedSettings");

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::ttcrxCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method checks each byte of the EPROM memory to ensure that it is the same
   * as that stored in the Fed9UDescription.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::epromCheck() const throw (Fed9UVmeDeviceException) {
    try {
      //The last byte of the EPROM is the write protection register and it cannot be read.
      for(u32 offset = 0; offset < theLocalFedSettings.getEprom().getEpromSize()-1; ++offset) {
	ICUTILS_VERIFY(epromRead(offset) == theLocalFedSettings.getEprom()[offset])(offset)( epromRead(offset) )( static_cast<u32>(theLocalFedSettings.getEprom()[offset]) ).msg("The byte at memory address \"offset\" on the FED EPROM does not match that in theLocalFedSettings EPROM.");
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::epromCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method can be used to compare the values of the Fed9UDescrition to those on the FED for
   * the whole FED.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::fedCheck() const throw (Fed9UVmeDeviceException) {
    try {
      //cout << "Begin Front end check" << endl;
      frontEndCheck();
      //cout << "Begin back end check" << endl;
      backEndCheck();
      //cout << "Begin vme check" << endl;
      vmeCheck();
      //cout << "Begin high level check" << endl;
      higherLevelCheck();
      //cout << "Begin TTCrx check" << endl;
      ttcrxCheck();
      //cout << "Begin EPROM check" << endl;
      //epromCheck();
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::fedCheck."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  // JEC 10-04-06
  // dumpChoice = 1 BE registers only
  // dumpChoice = 2 FE registers only
  // dumpChoice = 3 BE & FE registers
  // dumpChoice = 4 APV/fibre data only
  // dumpChoice = 5 FE registers & APV/fibre data
  // dumpChoice = 6 BE, FE and APV/fibre data
  // dumpChoice = 7 pedestals and cluster thresholds only
  // dumpChoice = 8 APV/fibre data and peds &thresh data
  // dumpChoice = 9 APV/fibre data, peds & thresh data, FE registers
  // dumpChoice = 10 everything !
  void Fed9UVmeDevice::dumpFedInformation(int dumpChoice, unsigned short Fpga, ostream &os) throw (Fed9UVmeDeviceException)
  {
    try {
      if ((dumpChoice == 1) || (dumpChoice == 3) || (dumpChoice == 6) || (dumpChoice == 10)) {
	ostringstream osBe;
	dumpBeRegisters(osBe,false);
	os << osBe.str();

	ostringstream osDetail;
	dumpBeStatusRegisters(osDetail,false);
	os << osDetail.str();

	bool readyFlag = getTrackerHeaderMonitorStatus();
	if (readyFlag) {
	  ostringstream osTHM;
	  dumpTrackerHeaderMonitor(osTHM,false);
	  os << osTHM.str();
	}
      }
      if ((dumpChoice == 2) || (dumpChoice == 3) || (dumpChoice == 5) || (dumpChoice == 6) || (dumpChoice >= 9)) {
	ostringstream osFe;
	dumpFeRegisters(Fpga,osFe,false);
	os << osFe.str();
      }
      if ((dumpChoice == 4) || (dumpChoice == 5) || (dumpChoice == 6) || (dumpChoice >= 8)) {
	ostringstream osApv;
	dumpApvData(Fpga,osApv,false);
	os << osApv.str();
      }
      if (dumpChoice >= 7) {
	if (Fpga <= 8) {
	  ostringstream osPeds;
	  dumpPedAndThreshData(Fpga,osPeds,false);
	  os << osPeds.str();
	} else {
	  os << "Not possible to dump Pedestals and Thresholds for all eight FE-FPGAs simultaneously !" << std::endl;
	}
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::dumpFedInformation."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Loads a new Fed9UDecription object into the Fed9UVmeDevice object, allowing
   * new FED setup to be loaded.
   *
   * The method takes the new Fed9UDescription object to be loaded, of which a
   * local copy is created (as in the constructor). There is also the option to
   * load these new parameters into the FED. This calls init, initStrips,
   * initAuxiliaryChips and setEprom. It is not possible to change FEDs or crates.
   * The base address and crate number should remain the same.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFed9UVMEDeviceDescription(const Fed9UDescription& newFed9UDescription, bool initFed) throw (Fed9UVmeDeviceException) {
    try {
      ostringstream errorMessageBase;
      errorMessageBase << "The base address in the newFed9UDescription does not match that in the current Fed9UDescription.\n"
		       << "The current base address "
		       << theLocalFedSettings.getBaseAddress()
		       << " will be used." << endl;
      ICUTILS_VERIFY(newFed9UDescription.getBaseAddress() == theLocalFedSettings.getBaseAddress())(newFed9UDescription.getBaseAddress())(theLocalFedSettings.getBaseAddress()).warn().msg(errorMessageBase.str().c_str());

      ostringstream errorMessageCrate;
      errorMessageCrate << "The crate number in the newFed9UDescription does not match that in the current Fed9UDescription.\n"
		       << "The current crate number "
		       << theLocalFedSettings.getCrateNumber()
		       << " will be used." << endl;
      ICUTILS_VERIFY(newFed9UDescription.getCrateNumber() == theLocalFedSettings.getCrateNumber())(newFed9UDescription.getCrateNumber())(theLocalFedSettings.getCrateNumber()).warn().msg(errorMessageCrate.str().c_str());

      //delete &theLocalFedSettings;
      theLocalFedSettings = newFed9UDescription;//*new Fed9UDescription(newFed9UDescription);

      if (initFed) {
	init();
	initStrips();
	initAuxiliaryChips();
	setEprom( theLocalFedSettings.getEprom() );
      }

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFed9UVmeDeviceDescription."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  // <NAC date="23/04/2007"> Method to load a new description
  Fed9UVmeDevice& Fed9UVmeDevice::loadDescription(const Fed9UDescription& newDescription) throw (Fed9UVmeDeviceException) {
    try {
      ostringstream errorMessageBase;
      errorMessageBase << "The base address in the newFed9UDescription does not match that in the current Fed9UDescription.\n"
                       << "The current base address "
                       << theLocalFedSettings.getBaseAddress()
                       << " will be used." << endl;
      ICUTILS_VERIFY(newDescription.getBaseAddress() == theLocalFedSettings.getBaseAddress())(newDescription.getBaseAddress())(theLocalFedSettings.getBaseAddress()).warn().msg(errorMessageBase.str().c_str());
      
      ostringstream errorMessageCrate;
      errorMessageCrate << "The crate number in the newFed9UDescription does not match that in the current Fed9UDescription.\n"
                       << "The current crate number "
                       << theLocalFedSettings.getCrateNumber()
                       << " will be used." << endl;
      ICUTILS_VERIFY(newDescription.getCrateNumber() == theLocalFedSettings.getCrateNumber())(newDescription.getCrateNumber())(theLocalFedSettings.getCrateNumber()).warn().msg(errorMessageCrate.str().c_str());
      
      //save state
      bool wasStarted = _isStarted;
      bool resetDone = false;
      //update the description from the FED
      try {
        updateLocalDescriptionFromFed();
      } catch (const ICUtils::ICException& e) {
        ostringstream errorUpdateDescription;
        errorUpdateDescription << "Exception caught while updating local description from FED. " << std::endl;
        errorUpdateDescription << e.what() << std::endl;
        Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errorUpdateDescription.str();
        //todo: make this call init(), initAuxChips(), initStrips() and warn that that is happening. (also change message above to reflect this) 
      }
      //if description is unchanged then return now
      if (newDescription == theLocalFedSettings) return *this;
      //if not the change to the new description
      Fed9UDescription oldDescription = theLocalFedSettings;
      theLocalFedSettings = newDescription;
      Fed9UAddress addr;
      //update settings on Fed
      if (newDescription.getTtcrx() != oldDescription.getTtcrx()) setTtcrx(theLocalFedSettings.getTtcrx());
      if (newDescription.getClock() != oldDescription.getClock()) {
        try{
          setClock(theLocalFedSettings.getClock());
        } catch (const Fed9UVmeDeviceException& e) {
          ostringstream errorClock;
          ostringstream errorClockDetailed;
          errorClock << "Exception caught while setting the clock source." << std::endl;
          errorClock << e.what() << std::endl;
          Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errorClock.str();
          theLocalFedSettings.setClock(getClock());
        }
        //these resets are probably not needed
        beFpgaSoftReset();
        addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
        feFpgaSoftReset(addr);
        resetDone = true;
      }
      if ((newDescription.getDaqMode() != oldDescription.getDaqMode()) || (newDescription.getDaqSuperMode() != oldDescription.getDaqSuperMode())) {
        setDaqMode(theLocalFedSettings.getDaqMode());
        if (theLocalFedSettings.getFeFirmwareVersion() >= 0x23100320 )  {
          setDaqSuperMode(theLocalFedSettings.getDaqSuperMode());
        }
        else {
          ostringstream errorDaqSuperMode;
          errorDaqSuperMode << "you must have a higher firmware version for the front end. if you want to use Fake Events or Zero Lite data modes!" << std::endl;
          Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errorDaqSuperMode.str();
        }
      }
      if (newDescription.getScopeLength() != oldDescription.getScopeLength()) setScopeLength(theLocalFedSettings.getScopeLength());
      if (newDescription.getTestRegister() != oldDescription.getTestRegister()) setTestRegister(theLocalFedSettings.getTestRegister());
      if (newDescription.getFedId() != oldDescription.getFedId()) setFedId(theLocalFedSettings.getFedId());
      if (newDescription.getBeFpgaReadRoute() != oldDescription.getBeFpgaReadRoute()) setBeFpgaReadRoute(theLocalFedSettings.getBeFpgaReadRoute());
      if (newDescription.getHeaderFormatType() != oldDescription.getHeaderFormatType()) setHeaderFormatType(theLocalFedSettings.getHeaderFormatType());
      if (newDescription.getDaqFov() != oldDescription.getDaqFov()) setDaqFov(theLocalFedSettings.getDaqFov());
      if (newDescription.getTriggerSource() != oldDescription.getTriggerSource()) setTriggerSource(theLocalFedSettings.getTriggerSource());
      //FE
      bool updateFe = false;
      for (int f=0; f<FEUNITS_PER_FED; f++) { 
        addr.setFedFeUnit(f); 
        if (newDescription.getFrontEndDescription(addr) != oldDescription.getFrontEndDescription(addr)) updateFe = true;
      }
      if (updateFe) {
        cout << "Updating fe settings" << std::endl;
        setAllFrameThresholds();
        setAllFedFeUnitDisable();
        setAllOptoRxControls();
        setAllCmMedianOverrideValues();
        setAllDelays();
        setAllComplements();
      }
      //sendTrimDacReset(Fed9UAddress::FEBROADCAST);
      //setAllTrimDacOffsets();
      //fed9Uwait(10000);
      //beFpgaSoftReset();
      //addr.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      //feFpgaSoftReset(addr);
      //sendVLinkReset();
      //STRIPS
      if (newDescription.getFedStrips() != oldDescription.getFedStrips()) setAllStripData();
      //AUX CHIPS
      addr.setFedFpga(Fed9UAddress::BACKEND);
      bool updateTemp = (newDescription.getTempControl(addr) != oldDescription.getTempControl(addr));
      for (int f=0; f<FEUNITS_PER_FED; f++) { addr.setFedFeUnit(f); if (newDescription.getTempControl(addr) != oldDescription.getTempControl(addr)) updateTemp = true; }
      if (updateTemp) setAllTempControlData();
      if (newDescription.getVoltageMonitor() != oldDescription.getVoltageMonitor()) setVoltageMonitor( theLocalFedSettings.getVoltageMonitor() );
      //Restore state
      if (resetDone) setBunchCrossingOrbitTotal(BUNCH_CROSSING_ORBIT_TOTAL);
      if (wasStarted) start();
      return *this;
    } 
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::loadDescription."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  // </NAC>

  /**
   * This method is used to ensure that either the FE FPGA DAQ mode is the same on all the FE FPGAs and that
   * the BE FPGA is in the same mode as the FE FPGAs.
   *
   * A reference value for FE DAQ mode is passed and all other FE DAQ modes are compared to this value.
   * The BE FPGA is then compared to the FE FPGA mode to ensure they are consistant.
   */
  Fed9UDaqMode Fed9UVmeDevice::daqConsistancyCheck(const vector<u32>& feDaqMode, const u32 beDaqMode) const throw (Fed9UVmeDeviceException) {
    try {
    //Ensure the FE DAQ mode vector is of the correct size.
    ICUTILS_VERIFY(feDaqMode.size() == 1)(feDaqMode.size()).error().msg("The FE DAQ mode argument vector is of an invalid size.");
    //Ensures that the DAQ mode is the same for all the FE FPGAs
    //starts at FE FPGA 1 as FE FPGA 0 is the comparision value.
    ostringstream strZeroExternalFeFpga;
    strZeroExternalFeFpga << Fed9UAddress().setFedFeUnit(0).getExternalFedFeUnit(); //Get external zero FPGA number for error message.
    for (u32 feFpga = 1; feFpga < FEUNITS_PER_FED; feFpga++) {
      vector<u32> feDaqModeCheck(1,0);
      theFed->feCommandLoadModeReg(Fed9UAddress().setFedFeUnit(feFpga).getFirmwareFedFeUnit(), READ, dummyVector, feDaqModeCheck);
      u32 externalFeFpga = Fed9UAddress().setFedFeUnit(feFpga).getExternalFedFeUnit();
      ostringstream strExternalFeFpga;
      strExternalFeFpga << externalFeFpga;
      string errMesg = "The DAQ mode on FPGA " + strExternalFeFpga.str() + " is different to FPGA " + strZeroExternalFeFpga.str(); 
      ICUTILS_VERIFY(feDaqModeCheck[0] == feDaqMode[0])(static_cast<int>(externalFeFpga))(feDaqModeCheck[0])(feDaqMode[0]).error().msg(errMesg.c_str());
    }

    //This checks the BE and FE FPGA modes to ensure that they are consistant and set the return value to the appropiate mode.
    Fed9UDaqMode fedDaqMode;

    //cout << "\n\nBack End Daq mode is set to " <<beDaqMode << " , fedaqmode is set to " << feDaqMode[0] <<  endl;
    switch (beDaqMode) {
    case 0x1:
      ICUTILS_VERIFY(feDaqMode[0] == 0x5 || feDaqMode[0] == 0x0)(feDaqMode[0]).error().msg("The FE and BE FPGA DAQ modes are inconsistant.");
      if (feDaqMode[0] == 0x5)
	fedDaqMode = FED9U_MODE_SCOPE;
      else 
	fedDaqMode = FED9U_MODE_NONE;
      break;
    case 0x2:
    case 0x3:
      ICUTILS_VERIFY(feDaqMode[0] == 0x6)(feDaqMode[0]).error().msg("The FE and BE FPGA DAQ modes are inconsistant.");
      fedDaqMode = FED9U_MODE_VIRGIN_RAW;
      break;
    case 0x6:
    case 0x7:
      ICUTILS_VERIFY(feDaqMode[0] == 0x12)(feDaqMode[0]).error().msg("The FE and BE FPGA DAQ modes are inconsistant.");
      fedDaqMode = FED9U_MODE_PROCESSED_RAW;
      break;
    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
      ICUTILS_VERIFY(feDaqMode[0] == 0xa)(feDaqMode[0]).error().msg("The FE and BE FPGA DAQ modes are inconsistant.");
      fedDaqMode = FED9U_MODE_ZERO_SUPPRESSED;
      break;
    default:
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unrecognised DAQ mode."));
      break;
    }

    return fedDaqMode;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

 /**
   * This method is used to ensure that either the FE FPGA DAQ SUPER mode is the same on all the FE FPGAs
   *
   * A reference value for FE DAQ Super mode is passed and all other FE DAQ super modes are compared to this value.
   */
  Fed9UDaqSuperMode Fed9UVmeDevice::daqSuperModeConsistancyCheck(const vector<u32>& feDaqSuperMode) const throw (Fed9UVmeDeviceException) {
    try {
      //Ensure the FE DAQ mode vector is of the correct size.
      ICUTILS_VERIFY(feDaqSuperMode.size() == 1)(feDaqSuperMode.size()).error().msg("The FE DAQ super mode argument vector is of an invalid size.");
      //Ensures that the DAQ super mode is the same for all the FE FPGAs
      //starts at FE FPGA 1 as FE FPGA 0 is the comparision value.
      ostringstream strZeroExternalFeFpga;
      strZeroExternalFeFpga << Fed9UAddress().setFedFeUnit(0).getExternalFedFeUnit(); //Get external zero FPGA number for error message.
      for (u32 feFpga = 1; feFpga < FEUNITS_PER_FED; feFpga++) {
	vector<u32> feDaqSuperModeCheck(1,6);
	theFed->feCommandLoadSuperModeReg(Fed9UAddress().setFedFeUnit(feFpga).getFirmwareFedFeUnit(), READ, dummyVector, feDaqSuperModeCheck);
	u32 externalFeFpga = Fed9UAddress().setFedFeUnit(feFpga).getExternalFedFeUnit();
	ostringstream strExternalFeFpga;
	strExternalFeFpga << externalFeFpga;
	string errMesg = "The DAQ Super mode on FPGA " + strExternalFeFpga.str() + " is different to FPGA " + strZeroExternalFeFpga.str(); 
	ICUTILS_VERIFY(feDaqSuperModeCheck[0] == feDaqSuperMode[0])(static_cast<int>(externalFeFpga))(feDaqSuperModeCheck[0])(feDaqSuperMode[0]).error().msg(errMesg.c_str());
      }
      //cout << " daq mode read back as: " << feDaqSuperMode[0] << endl;
      switch (feDaqSuperMode[0]) {
      case 0x0:
	return FED9U_SUPER_MODE_NORMAL;
	break;
      case 0x1:
	return FED9U_SUPER_MODE_FAKE;
	break;
      case 0x2:
	return FED9U_SUPER_MODE_ZERO_LITE;
	break;
      case 0x3:
	return FED9U_SUPER_MODE_FAKE_ZERO_LITE;
	break;
      default:
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "incorrect super mode setting!"));
	break;
      }
   }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }




  /**
   * Writes the LM82 temperature monitoring chip setting for a specific register.
   *
   * The FPGA can be 0-7, 9 or 10. The vector contains two elements. The first element
   * contains the new temperature value and the second element the register address.
   * The possible registers are :-
   * 0x09 - Writes to the configuration register. Only required when setting the critical temperature.
   * 0x0b - Writes the LM82 temperature high point.
   * 0x0d - Writes the FPGA temperature high point.
   * 0x5a - Writes the critical temperature set point.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::tempControlWrite(u16 fpga_, const vector<u32>& tempDataAndRegAddr) throw (Fed9UVmeDeviceException) {
    try {
    //Ensures that the enum is set to a valid value (could not be if a static_cast is used).
    ICUTILS_VERIFY(tempDataAndRegAddr[1] == 0x09 || tempDataAndRegAddr[1] == 0x0b || tempDataAndRegAddr[1] == 0x0d || tempDataAndRegAddr[1] == 0x5a)(tempDataAndRegAddr[1]).error().msg("An invalid register address for the temperature monitor has been passed");

    bool busy  = false;
    bool error = false;
    //Used to limit the number of times that it attempts to write if a busy or error signal is present.
    u32 loopCount      = 0;
    const u32 maxLoops = 100;
    do {
      if (fpga_ == Fed9UAddress::BACKEND) {
	theFed->beCommandLm82Control(WRITE, tempDataAndRegAddr, dummyWord, busy, error);
      } else if (fpga_ == Fed9UAddress::VME) {
	theFed->vmeCommandLm82Control(WRITE, tempDataAndRegAddr, dummyWord, busy, error);
      } else {
	theFed->feCommandLm82Control(Fed9UAddress().setFedFeUnit(fpga_).getFirmwareFedFeUnit(), WRITE, tempDataAndRegAddr, dummyWord, busy, error);
      }
      //If the write still fails after a set number of attempts then
      //the software gives up and throws an error.
      ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then temperature control chip is busy. If error = 1 then the write failed");
      //If the write fails there is a pause before it tries again.
      if (busy || error) {
	fed9Uwait(100000);
      }
      ++loopCount;
    } while (busy || error);
    return *this;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Reads the LM82 temperature monitoring chip setting for a specific register.
   *
   * The FPGA can be 0-7, 9 or 10. The vector contains two elements. The first element
   * contains zero and is not used and the second element the register address.
   * The possible register address are :-
   * 0x00 - Reads the LM82 current temperature.
   * 0x01 - Reads the FPGA current temperature.
   * 0x05 - Reads the LM82 temperature high point.
   * 0x07 - Reads the FPGA temperature high point.
   * 0x42 - Reads the critical temperature set point.
   * 0xfe - Reads the LM82 manufacturers ID.
   */
  u32 Fed9UVmeDevice::tempControlRead(u16 fpga_, const vector<u32>& regAddress) const throw (Fed9UVmeDeviceException) {
    try {
    //Ensures that the enum is set to a valid value (could not be if a static_cast is used).
    ICUTILS_VERIFY(regAddress[1] == 0x00 || regAddress[1] == 0x01 ||  regAddress[1] == 0x02 || regAddress[1] == 0x05 || regAddress[1] == 0x07 || regAddress[1] == 0x42 || regAddress[1] == 0xfe)(regAddress[1]).error().msg("An invalid register address for the temperature monitor has been passed");

    bool busy  = false;
    bool error = false;
    //Used to limit the number of times that it attempts to write if a busy or error signal is present.
    u32 loopCount      = 0;
    const u32 maxLoops = 2;
    //Used to hold the value that is read back.
    u32 tempSetting;
    do {
      if (fpga_ == Fed9UAddress::BACKEND) {
	    theFed->beCommandLm82Control(READ, regAddress, tempSetting, busy, error);
      } else if (fpga_ == Fed9UAddress::VME) {
 	    theFed->vmeCommandLm82Control(READ, regAddress, tempSetting, busy, error);
      } else {
	    theFed->feCommandLm82Control(Fed9UAddress().setFedFeUnit(fpga_).getFirmwareFedFeUnit(), READ, regAddress, tempSetting, busy, error);
      }
      //If the write still fails after a set number of attempts then
      //the software gives up and throws an error.
      ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then temperature control chip is busy. If error = 1 then the write failed");
      //If the write fails there is a pause before it tries again.
      if (busy || error) {
        cout << "busy or error are true in tempControlRead() trying again, loopCount = " << loopCount << endl;
	    fed9Uwait(1);
      }
      ++loopCount;
    } while (busy || error);
    return tempSetting;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of FE FPGA disbale settings to the FED from the localFedSettings.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllFedFeUnitDisable() throw (Fed9UVmeDeviceException) {
    try {
      Fed9UAddress addr;
      u32 feFpgaEnable = 0;
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	addr.setFedFeUnit(feFpga_);
	//setFedFeUnitDisable(addr, theLocalFedSettings.getFedFeUnitDisable(addr));
	feFpgaEnable |= static_cast<u32>( !(theLocalFedSettings.getFedFeUnitDisable(addr)) ) << feFpga_;
	theFed->beCommandFeEnableReg(WRITE, feFpgaEnable, dummyWord);
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllFeFpgaDisableFlag()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads all the temperature monitor data.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllTempControlData() throw (Fed9UVmeDeviceException) {
    try {
      //The LM82 is unique in that it can be set for the eight FE FPGAs, BE FPGA
      //and the VME FPGA. Therefore the following loop will cover all the possible
      //values that the Fed9UAddress can take for the LM82 command.
      const u32 ALL_FPGA_ADDRESSES = 11;
      for (u32 fedFpga_ = 0; fedFpga_ < ALL_FPGA_ADDRESSES; fedFpga_++) {
	if (fedFpga_ == 8) {
	  //There is no FPGA at address number 8.
	} else {
	  //This loops over all the valid addresses for the FPGAs.
	  //It covers the FE, BE and VME FPGAs.
	  Fed9UAddress addr;
	  addr.setFedFpga(fedFpga_);
	  //std::cout << "setting temp control for fpga = " << fedFpga_<< std::endl;
	  setTempControl(addr, theLocalFedSettings.getTempControl(addr));
	}
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllTempControlData."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads all the frame finding thresholds.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllFrameThresholds() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	//When writing to the FED a FE FPGA must be written. The following functions
	//always write a FE FPGA set of data despite taking in only one channel therefore there
	//is only the need to write on one channel as the methods get the rest of the
	//channel data and write it to the FED also.
	u32 fedChannel_ = 12 * feFpga_;
	Fed9UAddress addr(fedChannel_);
 	setFrameThreshold(addr, theLocalFedSettings.getFrameThreshold(addr));
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllFrameThresholds()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads all the OptoRx controls
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllOptoRxControls() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	Fed9UAddress addr;
	addr.setFedFeUnit(feFpga_);
	setOptoRxInputOffset( addr, theLocalFedSettings.getOptoRxInputOffset(addr) );
	setOptoRxOutputOffset( addr, theLocalFedSettings.getOptoRxOutputOffset(addr) );
	setOptoRxCapacitor( addr, theLocalFedSettings.getOptoRxCapacitor(addr) );
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllOptoRxControls()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads all the median over-ride values from the localFedSettings to the FED.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllCmMedianOverrideValues() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	Fed9UAddress addr;
	addr.setFedFeUnit(feFpga_);
	//Used to ensure that the median override should be enabled for a FE FPGA.
	bool disableMedianOverride;
	vector<u16> medianValues = theLocalFedSettings.getCmMedianOverride(addr, disableMedianOverride);
	setCmMedianOverride(addr, disableMedianOverride, medianValues);
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllCmMedianOverrideValues."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of delay FPGA skews.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllDelays() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 fedChannel_ = 0; fedChannel_ < CHANNELS_PER_FED; fedChannel_+= 4) {
	//When writing to the FED four channels must be written at one time. The following
	//method always writes a four channel set of data despite taking in only one
	//channel therefore there is only the need to write on one channel as the methods get
	//the rest of the channel data and writes it to the FED also.
	Fed9UAddress addr(fedChannel_);
	setDelay(addr, theLocalFedSettings.getCoarseDelay(addr), 
		 theLocalFedSettings.getFineDelay(addr));
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllDelays()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  

  /**
   * Applies the global shift to all the channels in the FED.
   */
  Fed9UVmeDevice & Fed9UVmeDevice::setGlobalFineSkew(u16 globalSkew) throw (Fed9UVmeDeviceException)
  {
    try{
      ICUTILS_VERIFY(globalSkew < 25)(globalSkew).error().msg("The global skew setting is out of range.");
      theLocalFedSettings.setGlobalFineSkew(globalSkew);
      //This now sets all the delay values from theLocalFedSettings, which has just had
      //it global skew setting updated.
      setAllDelays();
      return *this;
    }catch(ICUtils::ICException & exc){
      RETHROW(exc, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Error in  Fed9UVmeDevice::setGlobalFineSkew."));      
    }    
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of APV complement flags.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllComplements() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 fedChannel_ = 0; fedChannel_ < CHANNELS_PER_FED; fedChannel_++) {
	Fed9UAddress addr;
	addr.setFedChannel(fedChannel_);
	setComplement(addr, theLocalFedSettings.getComplement(addr));
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllComplements"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of Trim DAC offsets.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllTrimDacOffsets() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 fedChannel_ = 0; fedChannel_ < CHANNELS_PER_FED; fedChannel_++) {
	//This must be run over all channels as the Trim DAC methods sets the value on a channel by
	//passing the FE FPGA channel number to the FED as a register address and no values are retrieved
	//from the description.
	Fed9UAddress addr(fedChannel_);
	setTrimDacOffset(addr, theLocalFedSettings.getTrimDacOffset(addr));
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllTrimDacOffsets()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the all the strip data for a FED.
   *
   * It loads the pedestal values, the cluster thresholds and the number of valid channels.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllStripData() throw (Fed9UVmeDeviceException) {
    try {
      //The set strips methods always write 4 APVs of data to the FED (it gets the rest from the description) so there is only
      //the need to set every fourth strip.
      for (u32 fedApv_ = 0; fedApv_ < APVS_PER_FED; fedApv_+=4) {
	vector<u16>  pedestals(128,0);
	vector<bool> disableStrips(128,0);
	vector<u16>  highThresholds(128,0);
	vector<u16>  lowThresholds(128,0);

	Fed9UAddress addr;
	addr.setFedApv(fedApv_);

	vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(addr) );

	// <JRF date="22/08/2008"> check to see if apv has been disabled, if so we should load values to prevent this apv from outputting data
	
	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	  pedestals[apvStrip_]      = apvStripsDescription[apvStrip_].getPedestal();
	  disableStrips[apvStrip_]  = apvStripsDescription[apvStrip_].getDisable ();
	  highThresholds[apvStrip_] = apvStripsDescription[apvStrip_].getHighThreshold();
	  lowThresholds[apvStrip_]  = apvStripsDescription[apvStrip_].getLowThreshold();
	}


	setPedsAndDisabledStrips(addr, pedestals, disableStrips);
	setClusterThresholds(addr, highThresholds, lowThresholds);

      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllStripData."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of ADC settings from the FED.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllAdcControls() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	//When writing to the FED a FE FPGA must be written to the FED the following
	//methods always write a FE FPGA set of data despite taking in only one channel
	//therefore there is only the need to write on one channel as the methods get
	//the rest of the channel data and writes it to the FED also.
	u32 fedChannel_ = 12 * feFpga_;
	Fed9UAddress addr(fedChannel_);
	Fed9UAdcControls adcSettings;
	adcSettings = theLocalFedSettings.getAdcControls(addr);
	setAdcControls(addr, adcSettings);
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_INIT_FAILED, "error in Fed9UVmeDevice::setAllAdcControls."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Loads the complete set of APV disable settings from theLocalFedSettings to the FED.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAllApvDisable() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	//When writing to the FED a FE FPGA must be written to the following methods
	//which always write a FE FPGAs set of data despite taking in only one APV therefore
	//there is only the need to write on one APV as the methods get the rest of
	//the APV data and write it to the FED also.
	Fed9UAddress addr;
	addr.setFedFeUnit(feFpga_).setFeUnitApv(0);
	setApvDisable(addr, theLocalFedSettings.getApvDisable(addr));
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_START_FAILED, "error in Fed9UVmeDevice::setAllApvDisable()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Stops the FE FPGAs receiving data from the APVs.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::disableAllApvs() throw (Fed9UVmeDeviceException) {
    try {
      for (u32 feFpga_ = 0; feFpga_ < FEUNITS_PER_FED; feFpga_++) {
	//Will disable all the APVs and not write to the localFedSettings. If writing to
	//theLocalFedSettings it will cause problems when try to set the APV disable flag
	//as they are now all set to disable.
	vector<u32> apvDisableAll(1,0);
	theFed->feCommandLoadTick(Fed9UAddress().setFedFeUnit(feFpga_).getFirmwareFedFeUnit(), WRITE, apvDisableAll, dummyVector);
      }
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_STOP_FAILED, "error in Fed9UVmeDevice::disableAllApvs()"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  //<JEC date=16/03/07>
  Fed9UVmeDevice& Fed9UVmeDevice::setFrameCapturePrescale(u32 prescale) throw (Fed9UVmeDeviceException) {
    try {
      // The FED accepts and reads out every Nth event after the first one
      // This method accepts a prescale factor, not N.  Therefore need to convert from prescale to N
      // prescale = N + 1
      u32 skipEvents = prescale - 1;

      // set the BE prescale value
      setBEFrameCapturePrescale(skipEvents);
      // set the FE prescale value
      setFEFrameCapturePrescale(skipEvents);
      return *this;
    }
    catch(const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_STOP_FAILED, "error in Fed9UVmeDevice::setFrameCapturePrescale"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    } 
  }
  //</JEC>


   /**
    * This is used to compare two floating point numbers to see if they are equivalent.
    *
    * It only checks their accuracy to 3 d.p. as this is all that is required for the
    * voltage monitor floating point values.
    */
  bool Fed9UVmeDevice::floatEqual(float floatOne, float floatTwo) const {
    return ( std::fabs(floatOne-floatTwo) <= 0.001 );
  }


  bool Fed9UVmeDevice::isStarted() {
    return _isStarted;
  }
  
}//end of Fed9U namespace
