#include <inttypes.h>
#include <stdint.h>
#include "Fed9UVmeDevice.hh"
#include "Fed9UVoltageMonitor.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UVoltageControl.hh"
#include "Fed9UVoltageControlInfo.hh"
#include "Fed9ULogTemplate.hh"
#include "Fed9UWait.hh"
#include "Fed9UCounters.hh"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace Fed9U {

  using std::fstream;
  using std::ifstream;
  using std::ofstream;
  using std::ostringstream;
  using std::stringstream;

  /************************************************************************************************
   *                              These are the VME FPGA commands                                 *
   ************************************************************************************************/

  /**
   * Returns the firmware ID on the VME FPGA.
   */
  u32 Fed9UVmeDevice::getVmeFirmwareId() const  throw (Fed9UVmeDeviceException) {
    try {
      u32 vmeFirmwareId;
      theFed->vmeCommandFirmwareID(vmeFirmwareId);
      return vmeFirmwareId;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getVmeFirmwareId."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  Fed9UVmeDevice& Fed9UVmeDevice::setClock(Fed9UClockSource clockSelect) throw (Fed9UVmeDeviceException) {
    setClock(clockSelect, true) ;
    return * this;
  }
  
  /**
   * Sets the clock source.
   *
   * If the clock is successfully changed then a FED reset is sent.
   * If the clock is not successful changed then it will return to the
   * previous clock setting. If the clock is lost then it will default
   * to the on board clock. A FED reset is performed in each case.
   * This is not affect by a FED reset.
   * The clocks avaliable to be selected are the on board, TTC and back plane clock.
   * The Fed9UClockSource enum is defined in TypeDefs.hh.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setClock(Fed9UClockSource clockSelect, bool doWait) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(clockSelect==FED9U_CLOCK_INTERNAL || clockSelect==FED9U_CLOCK_TTC || clockSelect==FED9U_CLOCK_BACKPLANE)(clockSelect).error().msg("This is an invalid clock setting");

      theFed->vmeCommandSelectClock(WRITE, clockSelect, dummyWord, doWait);
      // If the clock is changed then a FED reset is performed automatically within the FED.
      // Note that if we are not doing the wait after clock and reset for speed purposes, we must wait 
      // at least the minimum time required for the VME to communicate after clock reset
    //<JRF date="23/05/2011" increased the wait from 500 to 600 in an attempt to try to remove the error messages seen when setting the clock. perhapse 500 is not quite long enough.
      if (!doWait)
	fed9Uwait(600); // we don't set 900 us here cause quite some time is taken up in the software and VME transfer before we even get here.  This value seems enough.


      //<JRF> 22/11/2006 I have taken this out due to the information sent by Saeed today:
      // As things are  in the FED, when you change clock or setClock as you call
      //  it, then the DCM in the vme fpga is reset and it in turn resets
      //  everything in the VME, BE, FE and DLY FPGAs.
      //  I would suggest that you issue the SetClock command,
      //  Then take out SendFedReset and Wait for DCM to lock commands, and send
      //  the next command.      
      //  The only problem is that the DCMs in the FED are connected in a chain
      //  and each DCM that locks, releases the reset line to the next stage.
      //  Therefore VME dcm has to lock first to release the reset to BE FPGA.
      //  Then the BE FPGA has to lock to release the reset to FE FPGA.
      //  Lastly, the FE FPGA DCM locks and releases the reset to DLY FPGA.      
      //  Each DCM can take up to 900us (1ms) to lock. We have about 9 in a chain.
      
      //sendFedReset(doWait);

      //</JRF>
      
      
      //ClockCheck is used to read back the new clock to ensure that the
      //user defined value has been set.
      u32 clockCheck;
      theFed->vmeCommandSelectClock(READ, dummyWord, clockCheck);
      ICUTILS_VERIFY(static_cast<u32>(clockSelect) == clockCheck)(clockSelect)(clockCheck).error().msg("The clock has not been changed, selected clock does not exist. The clock will have been reset to its hardware default");

      theLocalFedSettings.setClock(clockSelect);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setClock."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the location of the clock.
   *
   * The clocks avaliable to be selected are the on board, TTC and back plane clock.
   * The Fed9UClockSource enum is defined in TypeDefs.hh.
   */
  Fed9UClockSource Fed9UVmeDevice::getClock() const throw (Fed9UVmeDeviceException) {
    try {
      u32 encodedClockSelect;
      theFed->vmeCommandSelectClock(READ, dummyWord, encodedClockSelect);
      return static_cast<Fed9UClockSource>(encodedClockSelect);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getClock."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method will set the amount of read only memory on the EPROM.
   *
   * The read only level can be set to
   * FED9U_EPROM_NONE   - none of the memory is protected.
   * FED9U_EPROM_QUATER - the upper quater (1536 - 2046) is protected.
   * FED9U_EPROM_HALF   - the upper half (1024 - 2046) is protected.
   * FED9U_EPROM_ALL    - all the memory is write protected.
   * These values are from the Fed9UEpromReadOnlyLevel enum found in the TypeDefs.
   * At present it is not possible to read back this setting.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setEpromReadOnlyLevel(Fed9UEpromReadOnlyLevel readOnlyLevel) throw (Fed9UVmeDeviceException) {
    try {
      //A vector is required to pass the data (first element) and the register address (second element) to be written to theFed.
      vector<u32> epromDataAndOffset(2,0);
      //This holds the address of the register that is being written to. It is initialised to the write protection register.
      epromDataAndOffset[1] = 0x7ff;
      //The EPROM has a register which is used to control the levels of write protects.
      //The EPROM is initialised in such a state that only this register can be written to.
      //The first step in enabling writes to the EPROM is to write a 2 to the write protection register.
      //First a 2 must be written to the register.
      epromDataAndOffset[0] = 2;
      epromWrite(epromDataAndOffset);
      //This has enabled the other bits in the write protection register to be written to.
      //Next a 6 must be written to allow further changes to be made.
      epromDataAndOffset[0] = 6;
      epromWrite(epromDataAndOffset);
      //Now the write protection level can be set.
      switch (readOnlyLevel) {

      case FED9U_EPROM_NONE :
	//No write protection of any area of the memory.
	epromDataAndOffset[0] = 0x02;
	break;
      case FED9U_EPROM_QUATER :
	//Write protection on the upper quater of memory and no hardware protection.
	epromDataAndOffset[0] = 0x0a;
	break;
      case FED9U_EPROM_HALF :
	//Write protect the upper half but no hardware protection.
	epromDataAndOffset[0] = 0x12;
	break;
      case FED9U_EPROM_ALL :
	//Write protect the whole memory without any hardware protection.
	epromDataAndOffset[0] = 0x1a;
	break;
      default:
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Invalid read only level set."));
	break;

      }// End of switch.
      epromWrite(epromDataAndOffset);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setEpromReadOnlyLevel."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method is used to write to the EPROM memory.
   *
   * It takes an EPROM description, which should contain the new settings to be updated. The offset is the starting address
   * into the EPROM memory that needs to be written to. It is defaulted to 0 and has a maximum value of 2046.
   * There is also the option to specify the number of byte that should be written to. It has a maximum value of 2047
   * and must always be less than the size of the EPROM minus the offset. It is defaulted to 2047. The default
   * value will update the entire EPROM.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setEprom(const Fed9UEpromDescription& epromDescription, u32 offset, u32 numberOfBytes) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(offset < 2047)(offset).error().msg("The initial offset into the memory is too large.");
      u32 epromSize = epromDescription.getEpromSize();
      ICUTILS_VERIFY(numberOfBytes < epromSize - offset)(numberOfBytes)(epromSize - offset).error().msg("The number of bytes that has been requested to read is greater than the number of bytes left in the EPROM.");
      //Holds the data and the location in memory that it should be written on.
      vector<u32> epromDataAndOffset(2,0);
      //This writes to all the required data to the EPROM.
      //The last register on the EPROM contains the write protection settings and should not be written to.
      for (u32 byteNumber = 0; byteNumber < numberOfBytes; ++byteNumber) {
	epromDataAndOffset[0] = epromDescription[byteNumber + offset];
	epromDataAndOffset[1] = offset + byteNumber;
	epromWrite(epromDataAndOffset);
	theLocalFedSettings.setEprom(epromDescription);
      }
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setEprom."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method will return the contents of a section of the EPROM memory.
   *
   * The EPROM descrtiption passed as an argument will be updated with the data from the EPROM starting
   * at the initial offset until it has retrevied the specified number of bytes. The initialOffset is defaulted
   * to 0 and has a maximum value of 2046. The number of bytes has a maximum value of 2047 and is defaulted to 2047.
   * The updated EPROM is returned by reference.
   */
  Fed9UEpromDescription Fed9UVmeDevice::getEprom(u32 offset, u32 numberOfBytes) const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(offset < 2047)(offset).error().msg("The initial offset into the memory is too large.");

      //Creates a Fed9UEpromDescrition to hold the information read back from the EPROM.
      Fed9UEpromDescription epromDescription( theLocalFedSettings.getEprom() );
      u32 epromSize = epromDescription.getEpromSize();
      ICUTILS_VERIFY(numberOfBytes < epromSize - offset)(numberOfBytes)(epromSize - offset).error().msg("The number of bytes that has been requested to read is greater than the number of bytes left in the EPROM.");

      //The last register on the EPROM contains the write protection settings and cannot be read from at present.
      for (u32 byteNumber = 0; byteNumber < numberOfBytes; ++byteNumber) {
	epromDescription[offset + byteNumber] = epromRead(offset + byteNumber);
      }

      return epromDescription;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getEprom."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method will set the FED hardware id which is also called fed serial number.
   * this method maps to that in the Fed9UABC and the Fed9UDescription
   * NOTE: This method will be become depreciated during 2005.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFedHardwareId(u32 fedHardwareId) throw (Fed9UVmeDeviceException) {
    try {
      setFedSerialNumber (fedHardwareId);
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
   * This method will set the FED serial number.
   * NOTE: This method will be become depreciated during 2005.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFedSerialNumber(u32 serialNumber) throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> epromDataAndOffset(2,0);
      //The FED serial number is spread over two bytes in the EPROM memory.
      //This sets the 8 most significant bits of the offset.
      //the location of these bytes depends on the version of the eprom memory
      // we can get the version we are using from the description
      u32 offset;
      if (theLocalFedSettings.getEpromVersion() < FED9U_EPROM_VERSION_2)
	offset = FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET;
      else
	offset = FED9U_EPROM_INDUSTRY_RESULTS_OFFSET;
      
      if (theLocalFedSettings.getEpromVersion() < FED9U_EPROM_VERSION_2)
	epromDataAndOffset[0] = (serialNumber >> 8) & 0xFF;
      else 
	epromDataAndOffset[0] = (serialNumber / 10) & 0xFF;
      
      epromDataAndOffset[1] = offset;
      epromWrite(epromDataAndOffset);
      //This sets the 8 least significant bits.
      if (theLocalFedSettings.getEpromVersion() < FED9U_EPROM_VERSION_2)
	epromDataAndOffset[0] = ( serialNumber & 0xFF );
      else 
	epromDataAndOffset[0] = ( (serialNumber % 10) * 10) +  (epromRead(offset+1) % 10); // remember to add on the second digit of the decimal number, since that is not part of the serial number
      
      epromDataAndOffset[1] ++;
      epromWrite(epromDataAndOffset);
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
   * This method will return the FED serial number.
   * NOTE: This method will be become depreciated during 2005. Use the getFedSerialNumberProtected method instead.
   */
  u32 Fed9UVmeDevice::getFedHardwareId() const throw (Fed9UVmeDeviceException) {
    try {
      return getFedSerialNumber();
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This method will return the FED serial number.
   * NOTE: This method will be become depreciated during 2005. Use the getFedSerialNumberProtected method instead.
   */
  u32 Fed9UVmeDevice::getFedSerialNumber() const throw (Fed9UVmeDeviceException) {
    try {
      //The FED serial number occupies two bytes in the EPROM memory.
      //the location of these bytes depends on the version of the eprom memory
      // we can get the version we are using from the description
      u32 offset;
      if (theLocalFedSettings.getEpromVersion() < FED9U_EPROM_VERSION_2)
	offset = FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET;
      else
	offset = FED9U_EPROM_INDUSTRY_RESULTS_OFFSET;

      //This reads back the first byte, which is the 8 most signficant bits.
      u16 serialNumByteOne = epromRead(offset);
      //Now read back byte two, which is the least significant two bits.
      offset ++;
      u16 serialNumByteTwo = epromRead(offset);
      std::ostringstream msg;
      msg << "Here are the two bytes... first then second: " <<hex << serialNumByteOne << " " << hex << serialNumByteTwo << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");
      //Now combine the two bytes into a single FED serial number.
      
        if (theLocalFedSettings.getEpromVersion() < FED9U_EPROM_VERSION_2)
	  return ( ( (serialNumByteOne & 0XFF) << 8 ) | ( serialNumByteTwo & 0xFF  ) );
	else
	  return ( ( (serialNumByteOne & 0XFF) * 10 ) + ( ( serialNumByteTwo & 0xFF ) / 10 ) );
     
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This method will read the FED hardware ID (or serial number) from the write protected EPROM memory.
   * It uses a enum defined in TypeDefs.hh which sets the byte at which the write protected area of the
   * EPROM begins.
   */
  u32 Fed9UVmeDevice::getFedSerialNumberProtected() const throw (Fed9UVmeDeviceException) {
    try {
      //Read first byte (8 MSB of FED serial number)
      u16 serialNumByteOne = epromRead(EPROM_WRITE_PROTECTED);
      //Read 2nd byte (8 LSB of FED serial number)
      u16 serialNumByteTwo = epromRead(EPROM_WRITE_PROTECTED+1);
      //Return result
      return ( ( (serialNumByteOne & 0XFF) * 10 ) + ( ( serialNumByteTwo & 0xFF ) / 10 ) );
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
  /**
   * This can be used to place the voltage controller in a stand by mode which does not perform
   * any measurements or conversion and can be used to save power when the voltage controller is
   * not needed.
   *
   * A true will shut the controller down and a false enable it. This will not reset the chip.
   * All register values will remain unaffect by a shut down.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setVoltageMonitorStandBy(bool standBy) throw (Fed9UVmeDeviceException) {
    try {
      //A Fed9UVoltageMonitor require a Fed9UVoltageControl, however as we will only be calling the stand by command it can just be default constrcuted.
      Fed9UVoltageMonitor voltageMonitor(*theFed, Fed9UVoltageControl() );
      voltageMonitor.setStandBy(standBy);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setStandBy."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This provides the ability to reset the status registers to their hardware defaults.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::resetVoltageMonitorStatusRegister() throw (Fed9UVmeDeviceException) {
    try {
      //A Fed9UVoltageMonitor require a Fed9UVoltageControl, however as we will only be calling the stand by command it can just be default constrcuted.
      Fed9UVoltageMonitor voltageMonitor(*theFed, Fed9UVoltageControl() );
      voltageMonitor.resetStatusRegister();
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::resetStatusRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This is used to configure the voltage monitor.
   *
   * It must be given a Fed9UVoltageController which should contain all the 
   * appropiate settings.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setVoltageMonitor(const Fed9UVoltageControl& voltageController) throw (Fed9UVmeDeviceException) {
    try {
      //Create voltage monitor device so that the voltage monitor can be communitcated to.
      Fed9UVoltageMonitor voltageMonitor(*theFed, voltageController);

      //Need to shut down the voltage monitor before writing to the voltage monitor to prevent spurious out of limit conditions.
      voltageMonitor.setStandBy(true);
      //Set up the voltage monitor.
      voltageMonitor.set2Point5VoltMax( voltageController.get2Point5VoltMax() );
      voltageMonitor.set2Point5VoltMin( voltageController.get2Point5VoltMin() );
      voltageMonitor.set3Point3VoltMax( voltageController.get3Point3VoltMax() );
      voltageMonitor.set3Point3VoltMin( voltageController.get3Point3VoltMin() );
      voltageMonitor.set5VoltMax( voltageController.get5VoltMax() );
      voltageMonitor.set5VoltMin( voltageController.get5VoltMin() );
      voltageMonitor.set12VoltMax( voltageController.get12VoltMax() );
      voltageMonitor.set12VoltMin( voltageController.get12VoltMin() );
      voltageMonitor.setCoreVoltageMax( voltageController.getCoreVoltageMax() );
      voltageMonitor.setCoreVoltageMin( voltageController.getCoreVoltageMin() );
      voltageMonitor.setSupplyVoltageMax( voltageController.getSupplyVoltageMax() );
      voltageMonitor.setSupplyVoltageMin( voltageController.getSupplyVoltageMin() );
      voltageMonitor.setExternalTempMax( voltageController.getExternalTempMax() );
      voltageMonitor.setExternalTempMin( voltageController.getExternalTempMin() );
      voltageMonitor.setInternalTempMax( voltageController.getInternalTempMax() );
      voltageMonitor.setInternalTempMin( voltageController.getInternalTempMin() );
      voltageMonitor.setTempOffset( voltageController.getTempOffset(), voltageController.getOffsetTempSelect() );
      voltageMonitor.setStandBy( voltageController.getStandBy() );

      theLocalFedSettings.setVoltageMonitor(voltageController);

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setVoltageMonitor."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This return a Fed9UVoltageMonitor which contains all the current read and writable
   * settings on the voltage monitor.
   */
  Fed9UVoltageControl Fed9UVmeDevice::getVoltageMonitor() const throw (Fed9UVmeDeviceException) {
    try {
      //Create voltage monitor device.
      Fed9UVoltageMonitor voltageMonitor(*theFed, theLocalFedSettings.getVoltageMonitor());

      //The voltage monitor must first be enabled before measurements can be taken.
      //The stand by value is read back and saved.
      const bool standByEnabled = voltageMonitor.getStandBy();
      //If not already enabled, enable the voltage monitor to allow the settings to be read back.
      if (standByEnabled)
	voltageMonitor.setStandBy(false);

      //This will hold the settings that are read back.
      Fed9UVoltageControl voltageController(voltageMonitor.getStandBy(),
					    false,//This means don't reset the status register.
					    voltageMonitor.get2Point5VoltMax(),
					    voltageMonitor.get2Point5VoltMin(),
					    voltageMonitor.get3Point3VoltMax(),
					    voltageMonitor.get3Point3VoltMin(),
					    voltageMonitor.get5VoltMax(),
					    voltageMonitor.get5VoltMin(),
					    voltageMonitor.get12VoltMax(),
					    voltageMonitor.get12VoltMin(),
					    voltageMonitor.getCoreVoltageMax(),
					    voltageMonitor.getCoreVoltageMin(),
					    voltageMonitor.getSupplyVoltageMax(),
					    voltageMonitor.getSupplyVoltageMin(),
					    voltageMonitor.getExternalTempMax(),
					    voltageMonitor.getExternalTempMin(),
					    voltageMonitor.getInternalTempMax(),
					    voltageMonitor.getInternalTempMin(),
					    voltageMonitor.getTempOffset(),
					    voltageMonitor.getOffsetTempSelect());
      //If the stand by was initially enabled, then reenable it.
      if (standByEnabled)
	voltageMonitor.setStandBy(true);
	
      return voltageController;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getVoltageMonitor."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns a Fed9UVoltageMonitorInfo which contains all the current settings
   * on the voltage monitor. Both the read writable settings and the read only ones.
   */
  Fed9UVoltageControlInfo Fed9UVmeDevice::getVoltageMonitorInfo() const throw (Fed9UVmeDeviceException) {
    try {
      //Create voltage monitor device.
      Fed9UVoltageMonitor voltageMonitor(*theFed, theLocalFedSettings.getVoltageMonitor());

      //The voltage monitor must first be enabled before measurements can be taken.
      //The stand by value is read back and saved.
      const bool standByEnabled = voltageMonitor.getStandBy();
      //If not already enabled, enable the voltage monitor to allow the settings to be read back.
      if (standByEnabled)
	voltageMonitor.setStandBy(false);

      //This will hold the settings that are read back.
      Fed9UVoltageControlInfo voltageControllerInfo(voltageMonitor.getStandBy(),
						    false,//This means don't reset the status register.
						    voltageMonitor.get2Point5VoltMax(),
						    voltageMonitor.get2Point5VoltMin(),
						    voltageMonitor.get3Point3VoltMax(),
						    voltageMonitor.get3Point3VoltMin(),
						    voltageMonitor.get5VoltMax(),
						    voltageMonitor.get5VoltMin(),
						    voltageMonitor.get12VoltMax(),
						    voltageMonitor.get12VoltMin(),
						    voltageMonitor.getCoreVoltageMax(),
						    voltageMonitor.getCoreVoltageMin(),
						    voltageMonitor.getSupplyVoltageMax(),
						    voltageMonitor.getSupplyVoltageMin(),
						    voltageMonitor.getExternalTempMax(),
						    voltageMonitor.getExternalTempMin(),
						    voltageMonitor.getInternalTempMax(),
						    voltageMonitor.getInternalTempMin(),
						    voltageMonitor.getTempOffset(),
						    voltageMonitor.getOffsetTempSelect(),
						    voltageMonitor.getActual2Point5Volt(),
						    voltageMonitor.getActual3Point3Volt(),
						    voltageMonitor.getActual5Volt(),
						    voltageMonitor.getActual12Volt(),
						    voltageMonitor.getActualCoreVoltage(),
						    voltageMonitor.getActualSupplyVoltage(),
						    voltageMonitor.getActualExternalTemp(),
						    voltageMonitor.getActualInternalTemp(),
						    voltageMonitor.getManufacturesId(),
						    voltageMonitor.getSteppingId(),
						    voltageMonitor.getStatusRegister());
      //If the stand by was initially enabled, then reenable it.
      if (standByEnabled)
	voltageMonitor.setStandBy(true);

      return voltageControllerInfo;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getVoltageMonitorInfo."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This will reset all the configuration and status registers on the System ACE controller. It will
   * also perform a soft reset of the Compact Flash (CF) device.
   *
   * The firmware will be reloaded from the current configuration address.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::resetSystemAce() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //First get the existing configuration settings.
      //This is the word address. The byte address is 0x18.
      u32 regAddr = 0x0c;
      u32 data    = 0;
      //Start configuration once reset bit is cleared.
      data |= 0x0010;
      //Set the reset bit high.
      data |= 0x0080;
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      //The reset bit must be unset once the System ACE has been reset.
      fed9Uwait(1000000);
      //Clear the register control bits, which will clear the reset bit.
      data = 0x0000;
      theFed->vmeCommandSysAceControlWrite(data, regAddr);

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::resetSystemAce."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::reloadFpgaFirmwareFromCompactFlash(u32 revision) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(revision < 8)(revision).error().msg("The config address is invalid.");
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 1: " << theLocalFedSettings.getFedHardwareId() << std::endl;
      setSysAceWordMode();
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 2: " << theLocalFedSettings.getFedHardwareId()  << std::endl;
      //First read from the register and save the details.
      u32 regAddr( 0x0c );
      u32 data( theFed->vmeCommandSysAceControlRead(regAddr) );
      //Start configuration once the reset bit is cleared.
      data &= ~0x0010;
      //Force override of configuration pins and so it uses the reset bit.
      data |= 0x000c;
      //Set the reset bit high, will reset when cleared.
      data |= 0x0080;
      //Set the config address to reload from.
      //First clear any address that has been read back.
      data &= ~0xe000;
      //Put the revision number in its place.
      data |= (revision & 0x7) << 13;
      //Now write the command.
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 3: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
      //Read it back to see what we wrote and to update the correct bits.
      data = theFed->vmeCommandSysAceControlRead(regAddr);
      //Now clear the reset bit.
      data &= ~0x0080;
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 4: " << theLocalFedSettings.getFedHardwareId()  << std::endl;
      data = theFed->vmeCommandSysAceControlRead(regAddr);
      //Now tell it to restart.
      data |= 0x0020;
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 5: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
      data = theFed->vmeCommandSysAceControlRead(regAddr);
      //Clear the start config bit.
      data &= ~0x0020;
      data = theFed->vmeCommandSysAceControlRead(regAddr);
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 6: " << theLocalFedSettings.getFedHardwareId()  << std::endl;
      //Poll to check the reload completed, throw an exception if necessary.
      const bool reloadGood( pollCfForFirmwareReload() );
      ICUTILS_VERIFY(reloadGood)(reloadGood)(revision).error().msg("Compact flash card falied to reload firmware from \'revision\'.");
      //<JRF> 26/10/2006 we must now send a full reset to the fed to reset the DCMS to get everything working again.
      fed9Uwait(1000000);
      resetSystemAce();
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 7: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
      fed9Uwait(2000000);
      sendFedCompleteLogicReset();
      std::cout << "Reloading firmware!!!!!!!!!!!!!!!! 8: " << theLocalFedSettings.getFedHardwareId()  << std::endl;
      fed9Uwait(8000000);
      return *this;

    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::reloadFpgaFirmwareFromCompactFlash."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This is used to set the System ACE data words to 16 bit.
   *
   * When in word mode each System ACE register address refers to a 16 bit
   * register. It is possible to set the System ACE into byte mode where each
   * register will be 8 bits large. This class assumes that the System ACE is
   * in word mode. Each method that uses the System ACE sets itself into word
   * mode and so there should be no need to call this method.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setSysAceWordMode() throw (Fed9UVmeDeviceException) {
    try {
      //The address of the register that selects whether byte or word mode are used is at register
      //address 0x00, so it does not matter what the current mode is.
      u32 regAddr = 0x00;
      //First read the register so that we don't lose the existing settings.
      u32 wordMode = theFed->vmeCommandSysAceControlRead(regAddr);
      //Check to see if already in word mode.
      if ( 0x1 != (wordMode & 0x1) ) {
	//Update the relavant bit to put the System ACE into word mode.
	wordMode |= 0x1;
	//This sets the System ACE into word mode. If Byte mode is used then all the register addresses used in the
	//other commands must be changed.
	theFed->vmeCommandSysAceControlWrite(wordMode, regAddr);
	fed9Uwait(1000000);
	//Only interested in the first bit.
	u32 checkWordMode = theFed->vmeCommandSysAceControlRead(regAddr) & 0x1;
	ICUTILS_VERIFY(checkWordMode == 0x1)(checkWordMode).error().msg("The System ACE controller is not in word mode.");
      }

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setSysAceWordMode."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Checks the size of the data used by the Compact Flash (CF).
   *
   * The Compact Flash can either write 8 bit data words or 16 bit data words. This returns the mdoe
   * that it is currently using. Returns a true if the CF is in 16 bit data word mode. False if it is
   * not. All System ACE methods rely on it begin in word mode. Each method always sets itself into
   * word mode before writting to the System ACE.
   */
  bool Fed9UVmeDevice::getSysAceWordMode() const throw (Fed9UVmeDeviceException) {
    try {
      u32 regAddr = 0;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      return (data&0x1) ? true:false;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getSysAceWordMode."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  //\todo - these are test functions and need to be removed once everything is working.
  Fed9UVmeDevice& Fed9UVmeDevice::setSystemAceRegister(u32 data, u32 regAddr) throw (Fed9UVmeDeviceException) {
    try {
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setSystemAceRegister."));
    } catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  u32 Fed9UVmeDevice::getSystemAceRegister(u32 regAddr) const throw (Fed9UVmeDeviceException) {
    try {
      return theFed->vmeCommandSysAceControlRead(regAddr);
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getSystemAceRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  string Fed9UVmeDevice::getCfFileHeader(const string& cfFileName)  throw (Fed9UVmeDeviceException) {
    try {
      //Creates the output file to write to in the event of a test.
      ifstream cfInputImage(cfFileName.c_str(), std::ios::binary);
      //If we need to test the file then check it opened correctly.
      ICUTILS_VERIFY( cfInputImage.good() )(cfFileName)(cfInputImage.rdstate()).error().msg("The test image file did not open correctly.");

      //Copy the contents of the file until we reach the end of the file header into a string stream.
      ostringstream fileHeader;
      //Store the line that is read from the file.
      string imageLine;
      const string endOfFirmwareInfo("END OF FILE HEADER");
      u32 nLoops = 0;
      const u32 maxLoops = 200;
      bool readFile = true;
      do {
	getline(cfInputImage, imageLine);
	//Check to see if this is the end of the firmware information.
	if (endOfFirmwareInfo == imageLine) {
	  readFile = false;
	} else {
	  //getline discards the delimiter so it must be added.
	  fileHeader << imageLine << '\n';
	}
	//In the event it doesn't see the end of the firmware.
	ICUTILS_VERIFY(nLoops++ < maxLoops)(imageLine).error().msg("Failed to find the end of the firmware information in the Compact Flash image file. The current line of the image file is given by \"imageLine\".");
      } while(readFile);

      //Close the file.
      cfInputImage.close();

      //Convert string stream into a string and return it.
      return fileHeader.str();

      return string("Reimplementing this method, will work later.");

    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCfFileHeader."));
    } catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  string Fed9UVmeDevice::setCfImageFile(const string& cfInputImageFile, bool testCf, bool setGlobalFirmware) throw (Fed9UVmeDeviceException) {
    //This code reads the CF image file and removes the firmware information before writting to the CF.
    //The format of the firmware information must be identical that read from the CF.

    //Can hold a pointer to the test file if one is needed. Needs to be out of try scope so the catches can clean up the memory if necessary.
    ofstream* cfTestImage(NULL);
    try {
      gFed9UOut << std::dec;
      std::ostringstream msg;
      timer1.start();
      timer2.start();

      //Want to handle a control C in this method.
      handleControlC = true;

      if (testCf) {
	//Ensure each thread opens its own file. If there same threads are writting to the same slot and crate number
	//we are in bigger trouble than worrying about writting to the same test file...
	ostringstream testImageName;
	testImageName << "cf_image_test" << theLocalFedSettings.getCrateNumber() << "_" << theLocalFedSettings.getSlotNumber() << ".ace";
	cfTestImage = new ofstream( testImageName.str().c_str(), std::ios::binary );
	//If we need to test the file then check it opened correctly.
	ICUTILS_VERIFY( (*cfTestImage).good() )((*cfTestImage).rdstate())("cf_image_test.cf").error().msg("The test image file did not open correctly.");
      }

      //Process the given file.
      //First require the file lock, as only one thread should try and open a file at one time.
//       pthread_mutex_lock(&fileMutex);
      //Now we have the lock we can open the file that is to be read from and check it opened successfully.
      ifstream cfInputImage( cfInputImageFile.c_str(), std::ios::binary);
      ICUTILS_VERIFY( cfInputImage.good() )(cfInputImageFile)(cfInputImage.rdstate()).error().msg("The Compact Flash image cannot be opened.");
      //Variables to hold the beginning and end data markers.
      std::ios::pos_type startMarker(-1);
      std::ios::pos_type endMarker(-1);
      //A string to hold the firmware information.
      string firmwareInfo;
      //Check to see if anyone has opened the file and already parsed it for us IF we are using the global file only.
      if (setGlobalFirmware && cfFileParsed) {
	//Someone has already parsed the global file, so just get the static members that hold the start and end markers.
	startMarker = startCfDataMarker;
	endMarker   = endCfDataMarker;
      } else {
	//No one has processed the file for us, so we must be the first and best do it.
	removeCfFileHeaderAndMarkers(cfInputImage, endMarker, firmwareInfo/*, startMarker*/);
	timer2.stop();
	msg << "Time to strip header: " << timer2.read() << "us" << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	msg.str("");
	//We have processed the file so tell everyone else and update the file markers.
	startMarker       = cfInputImage.tellg();
	startCfDataMarker = startMarker;
	endCfDataMarker   = endMarker;
	cfFileParsed      = true;
      }
      //Just check we have sensible values for our file markers.
      ICUTILS_VERIFY((-1 != startMarker) && (-1 != endMarker))(startMarker)(startCfDataMarker)(endMarker)(endCfDataMarker)(cfFileParsed).error().msg("The startand end markers of the compact flash image file have not been set. Are the global markers (not -1) set and have we been told they are?");

      //Set our file to the start of the header.
      cfInputImage.seekg(startMarker);
      //Now we have the start and end information we can release the lock and start our writting.
//       pthread_mutex_unlock(&fileMutex);
      timer2.start();
      //Firmware versions have been read from the image file now we write the contents to the CF card.
      //First ensure that it is in word mode.
      if (!testCf)
	setSysAceWordMode();
      u32 lba = 0;
      const u32 sectorCount = 256;
      //A cluster is defined as 256 sectors (512bytes). 1 cluster is 128KB.
      //There are 8 clusters in 1 MB, so 512 clusters on a 64MB card.
      //Only one cluster at a time can be ready before the process of setting up the card is repeated.
      //Write the total number of clusters always checking for the end of file. If that is reached break.
      const u32 totalClusters = 488;
      bool eofReached(false);
      for(u32 clusterNumber = 0; clusterNumber < totalClusters; ++clusterNumber) {
	msg << "Cluster number: " << clusterNumber << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	msg.str("");
	if (!testCf) {
	  //The first step in writing to the CF is to get a the MPU to lock on to the CF.
	  getCompactFlashLock();
	  //Next the CF must be checked that it is ready for commands.
	  checkCfReadyForCommands();
	  //Set the region on the CF that we wish to write to.
	  setDataRegionToReadOrWrite(lba, sectorCount, WRITE);
	  //Prevent CF card from reloading when done.
	  u32 regAddr(0x0c);
	  u32 data( theFed->vmeCommandSysAceControlRead(regAddr) );
	  //Set reset bit.
	  data   |=  0x0080;
	  //Set CFGSTART bit low to ensure we don't restart after reset.
	  data   &= ~0x0010;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	}
	//Calculate the total number of buffers that are to be read.
	//Each buffer is 32 bytes big and each sector is 512 bytes big.
	u32 totalBuffers = sectorCount * 512 / 32;
	//Writes to each each buffer.
	for (u32 bufferNumber = 0; bufferNumber < totalBuffers; ++bufferNumber) {
	  //Check the data buffer is ready to be written to.
	  if (!testCf)
	    checkDataBufferReady();

	  //Each write to the System ACE register is a 16 bit word. Therefore 16 writes must be performed
	  //to set all the data in the buffer.
	  u32 totalWords(16);
	  for (u32 wordNumber = 0; wordNumber < totalWords; ++wordNumber) {
	    //Read two bytes from binary file and combine into a word.
	    char bytes[2] = {0,0};
	    //Read the raw bytes and test for eof (or errors) at same time.

	    if (!cfInputImage.read(bytes,2) || (endMarker <= cfInputImage.tellg()) || controlCCaught ) {
	      //Explain what is happening, but only the first time.
	      if (!eofReached && controlCCaught) {
		msg << "Caught control C signal, finishing cluster write then exiting." << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
		msg.str("");
	      }
	      //Set our eof marker if this is the case.
	      eofReached = true;
	    }
	    if (!eofReached) {
	      u16 cfWord(0);
	      cfWord |= (bytes[0] & 0xff);
	      cfWord |= (bytes[1] & 0xff) << 8;
	      //The memory buffer is a FIFO so the same location can be written each time
	      //and the FIFO automatically puts the next piece of data there.
	      //Write the data from the beginning of the CF image.
	      if (testCf) {
		char test[2] = { (char)(cfWord & 0xff), (char)((cfWord >> 8) & 0xff) };
		(*cfTestImage).write(test, 2);
	      } else {
            int jcount=0;
            do {
		      try {
                theFed->vmeCommandSysAceDataWrite(cfWord);
                jcount=0;
               } catch (exception & exc) {
                    std::cout << "Failed to Verify the Data written to System Ace File!!! Try again. Triles = "<< jcount << "error message: " << exc.what() << std::endl;
                    if (jcount > 10) {
                           std::cout << "Timed out trying to upload block!!! NOTE the Flash file for this fed is not correct!!! FEDHARDID = " << (int)theLocalFedSettings.getFedHardwareId() << std::endl;                 
                           jcount=0;
                    }
                    jcount++;
               }
             } while (jcount > 0);   
	      }
	    } else {//if (!eofReached)
	      //If we have no more file data to write we still have to give the system ACE enough to fill up the sector.
	      if (!testCf)
		theFed->vmeCommandSysAceDataWrite(0);
	    }//else from if (!eofReached)

	  }//End of word for

	  //Can't break out of this if end of file because we need to write the rest of the sector data!

	}//End of buffer for
	if (testCf) {
	  //Do nothing as the CF is not being written to.
	} else {
	  //Clean up, so ready to read next cluster.
	  //Need to reset the card and release the lock granted in getCompactFlashLock().
	  u32 regAddr(0x0c);
	  u32 data( theFed->vmeCommandSysAceControlRead(regAddr) );
	  //Ensure we won't reload after reset.
	  data &= ~0x0010;
	  //Reset
	  data |=  0x0080;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Both lock request and force lock request bits are cleared.
	  data &= ~0x0003;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Check that the lock is released
	  u32 maxLoops = 100;
	  u32 loops    = 0;
	  while (theFed->vmeCommandSysAceControlRead(0x02) & 0x0002) {
	    //The MPU lock has not been realeased. Try again.
	    data &= ~0x0003;
	    theFed->vmeCommandSysAceControlWrite(data, regAddr);
	    ICUTILS_VERIFY(loops++ < maxLoops)(theFed->vmeCommandSysAceControlRead(0x02) & 0x0002).error().msg("Could not release the MPU lock on the CF");
	  }
	}//End testCf else
	//Increment the LBA so that it points to the next cluster to be written.
	lba += 256;

	if (eofReached) {
	  if (controlCCaught) {
	    exit(0);
	  }
	  break;
	}
      }//End of cluster for

      timer1.stop();
      timer2.stop();
      msg << "Time to upload    : " << timer2.read()/1000000.0 << "s" << std::endl;
      msg << "Time to run method: " << timer1.read()/1000000.0 << "s" << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str();
      //Remove the test file if necessary.
      if (cfTestImage != NULL) {
	delete cfTestImage;
	cfTestImage = NULL;
      }

      //Can stop handling control C
      handleControlC = false;

      return firmwareInfo;
    } catch (const ICUtils::ICException& e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) {
	exit(0);
      }
      handleControlC = false;
      //Remove the test file if necessary.
      if (cfTestImage != NULL) {
	delete cfTestImage;
	cfTestImage = NULL;
      }
      //pthread_mutex_unlock(&fileMutex);
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setCfImageFile."));
    }
    catch (const std::exception &e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      handleControlC = false;
      //Remove the test file if necessary.
      if (cfTestImage != NULL) {
	delete cfTestImage;
	cfTestImage = NULL;
      }
      //pthread_mutex_unlock(&fileMutex);
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      handleControlC = false;
      //Remove the test file if necessary.
      if (cfTestImage != NULL) {
	delete cfTestImage;
	cfTestImage = NULL;
      }
      //pthread_mutex_unlock(&fileMutex);

      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setEraseCfImage() throw (Fed9UVmeDeviceException) {
    //This code formats the compact flash in the FED!!!!! be careful!
    try {
      std::ostringstream msg;

      timer1.start();
      timer2.start();

      //We want to handle control C in this method.
      handleControlC = true;
      //Create a string to hold the default location of the format .ace file.
      const string formatFilePath( getenv("ENV_CMS_TK_FED9U_ROOT")+string("/Fed9UFirmwareFiles/cf_formatted.ace") );

      //Require the member function gets a mutex before it can open the file.
//       pthread_mutex_lock(&fileMutex);
      //Open the file containing a blank file allocation table.
      ifstream formatFile( formatFilePath.c_str() );
      //If we need to test the file then check it opened correctly.
      ICUTILS_VERIFY( formatFile.good() )(formatFilePath)(formatFile.rdstate()).error().msg("The file with formatted data on did not open.");

      //Process the file to retrieve pure data...
      //...marks the end of the data in the file.
      std::ios::pos_type endDataMarker(0);
      //...we can't get any firmware info in this case as it is a blank card!!!
      string pointlessString;
      removeCfFileHeaderAndMarkers(formatFile, endDataMarker, pointlessString);
      //Release the mutex.
//       pthread_mutex_unlock(&fileMutex);
      timer2.stop();
      msg << "Time to strip header: " << timer2.read() << "us" << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");
      timer2.start();
      //Put in word mode.
      setSysAceWordMode();
      
      u32 lba = 0;
      u32 sectorCount = 256;
      //A cluster is defined as 256 sectors (512bytes). 1 cluster is 128KB.
      //There are 8 clusters in 1 MB, so 488 clusters on a 64*10^6 Byte card.
      //The formatted file is 1MB big so should need to write only 8 clusters. Mark all to be read and just read to the end of file instead.
      const u32 totalClusters = 488;
      bool eofReached(false);
      for(u32 clusterNumber = 0; clusterNumber < totalClusters; ++clusterNumber) {
	msg << "Cluster number: " << clusterNumber << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	msg.str("");
	//The first step in writing to the CF is to get a the MPU to lock on to the CF.
	getCompactFlashLock();
	//Next the CF must be checked that it is ready for commands.
	checkCfReadyForCommands();
	//Set the region on the CF that we wish to write to.
	setDataRegionToReadOrWrite(lba, sectorCount, WRITE);
	//Prevent CF card from reloading when done.



	u32 regAddr(0x0c);
	u32 data(0);
	/* <JRF date="21/05/2008"> commented out this section cause JC said in his code that it wasn't necessary.

	data = theFed->vmeCommandSysAceControlRead(regAddr) ;
	//Set reset bit.
	data   |=  0x0080;
	//Set CFGSTART bit low to ensure we don't restart after reset.
	data   &= ~0x0010;
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	*/

	//Calculate the total number of buffers that are to be read.
	//Each buffer is 32 bytes big and each sector is 512 bytes big.
	u32 totalBuffers = sectorCount * 512 / 32;
	//Writes to each each buffer.
	for (u32 bufferNumber = 0; bufferNumber < totalBuffers; ++bufferNumber) {
	  //Check the data buffer is ready to be written to.
	  checkDataBufferReady();

	  //Each write to the System ACE register is a 16 bit word. Therefore 16 writes must be performed
	  //to set all the data in the buffer.
	  u32 totalWords = 16;
	  for (u32 wordNumber = 0; wordNumber < totalWords; ++wordNumber) {
	    //Read two bytes from binary file and combine into a word.
	    char bytes[2] = {0,0};
	    //Read the raw bytes and test for eof (or errors) at same time.
	    if (!formatFile.read(bytes,2) || (endDataMarker <= formatFile.tellg()) || controlCCaught) {
	      //Explain what is happening, but only if it is the first loop.
	      if (!eofReached && controlCCaught) {
		msg << "Caught control C signal, finishing cluster write then exiting." << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
		msg.str("");
	      }
	      //Set our eof marker if this is the case.
	      eofReached = true;

	    }
	    if (!eofReached) {
	      u16 cfWord(0);
	      cfWord |= (bytes[0] & 0xff);
	      cfWord |= (bytes[1] & 0xff) << 8;
	      //The memory buffer is a FIFO so the same location can be written each time
	      //and the FIFO automatically puts the next piece of data there.
	      //Write the data from the beginning of the CF image.
	      theFed->vmeCommandSysAceDataWrite(cfWord);
	    } else {//if (!eofReached)
	      //If we have no more file data to write we still have to give the system ACE enough to fill up the sector.
	      theFed->vmeCommandSysAceDataWrite(0);
	    }//else from if (!eofReached)

	  }//End of word for
	}//End of buffer for

	//Clean up, so ready to read next cluster.
	//Need to reset the card and release the lock granted in getCompactFlashLock().
	regAddr = 0x0c;
	data = theFed->vmeCommandSysAceControlRead(regAddr);
	//Ensure we won't reload after reset.
	data &= ~0x0010;
	//Reset
	data |=  0x0080;
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	//Both lock request and force lock request bits are cleared.
	fed9Uwait(10000);
	data &= ~0x0003;
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	//Check that the lock is released
	u32 maxLoops = 100;
	u32 loops    = 0;
	while (theFed->vmeCommandSysAceControlRead(0x02) & 0x0002) {
	  //The MPU lock has not been realeased. Try again.
	  data &= ~0x0003;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  ICUTILS_VERIFY(loops++ < maxLoops)(theFed->vmeCommandSysAceControlRead(0x02) & 0x0002).error().msg("Could not release the MPU lock on the CF");
	}

	//Increment the LBA so that it points to the next cluster to be written.
	lba += 256;

	if (eofReached) {
	  if (controlCCaught) {
	    msg << "Exiting..." << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	    msg.str("");
	    exit(0);
	  }
	  break;
	}

      }//End of cluster for

      timer1.stop();
      timer2.stop();
      msg << "Time to Format    : " << timer2.read()/1000000.0 << "s" << std::endl;
      msg << "Time to run method: " << timer1.read()/1000000.0 << "s" << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");


      //Stop handling control C.
      handleControlC = false;

      return *this;

    } catch (const ICUtils::ICException& e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      //pthread_mutex_unlock(&fileMutex);
      handleControlC = false;
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setEraseCfImage."));
    }
    catch (const std::exception &e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      //pthread_mutex_unlock(&fileMutex);
      handleControlC = false;
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      //pthread_mutex_unlock(&fileMutex);
      handleControlC = false;
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::getCfImageFile(u32 mbToReadFromCf, string cfOutputFileName, u32 numRevToCheck) throw (Fed9UVmeDeviceException) {
    try {
      
      std::ostringstream msg;

      timer1.start();
      //This method writes both an image of the CF and the current firmware information to the image file.

      //Check we have a valid amount of mega bytes.
      ICUTILS_VERIFY(mbToReadFromCf <= 60)(mbToReadFromCf).error().msg("Requested number of mega bytes to read it greater than the size of the CF card.");

      //Need to handle control C in this method.
      handleControlC = true;
      bool controlCMarker(false);

      timer2.start();
      //Write the FE, BE and VME FPGAs firmware versions, for each of the valid revisions.
      vector<string> firmwareInfo( setCfFileHeader(numRevToCheck) );
      ICUTILS_VERIFY(firmwareInfo.size() == 2)(firmwareInfo.size()).error().msg("FirmwareInfo wrong size! Should be two.");
      timer2.stop();
      msg << "Time to find header info: " << (timer2.read()/1000000.0) << "s" << std::endl;
      std::cout << "Time to find header info: " << (timer2.read()/1000000.0) << "s" << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");
      timer2.start();

      //Create a file stream to store the image.
      string filename;
      if (string("UseBinary") == cfOutputFileName) {
	//Open a file with the binary encoded file name.
	filename = firmwareInfo[0];
	msg << "Using binary name: " << filename << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	msg.str("");
      } else {
	//Use the User given file name.
	filename = cfOutputFileName;
	msg << "Using User name: " << filename << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	msg.str("");
      }
      ofstream cfOutputImage(filename.c_str(), std::ios::binary);
      //Check the file opened correctly
      ICUTILS_VERIFY(cfOutputImage.good())(filename)(cfOutputImage.rdstate()).error().msg("Failed to open output file");

      //Write the header to the file.
      cfOutputImage << firmwareInfo[1];

      //Firmware versions are written to the image file now we write the contents of the CF card.
      //First ensure that it is in word mode.
      setSysAceWordMode();

      u32 lba = 0;
      const u32 sectorCount = 256;
      //A cluster is defined as 256 sectors (512bytes). 1 cluster is 128*2^10bytes
      //There are 8 clusters in 1*2^20bytes, so 488 clusters on a 64*10^6byte card.
      //Figure out how many clusters to read.
      const u32 totalClusters(8*mbToReadFromCf);//Set this to 488 when debugging complete.
      for(u32 clusterNumber = 0; clusterNumber < totalClusters; ++clusterNumber) {
	msg << "Reading cluster " << clusterNumber << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	msg.str("");
	//The first step in reading from the CF is to get a the MPU to lock on to the CF.
	getCompactFlashLock();
	//Next the CF must be checked that it is ready for commands.
	checkCfReadyForCommands();
	//Set the region on the CF that we wish to read from.
	setDataRegionToReadOrWrite(lba, sectorCount, READ);

	//These are required to selected the appropiate buffer and store the data read back.
	u32 regAddr;
	u32 data;

    	regAddr =  0x0c;
    	data    =  theFed->vmeCommandSysAceControlRead(regAddr);
	//Set reset bit.
    	data   |=  0x0080;
	//Set CFGSTART bit low to ensure we don't restart after reset.
	data   &= ~0x0010;
  	theFed->vmeCommandSysAceControlWrite(data, regAddr);

	//Calculate the total number of buffers that are to be read.
	//Each buffer is 32 bytes big and each sector is 512 bytes big.
	const u32 totalBuffers = sectorCount * 512 / 32;
	//Read each buffer.
	for (u32 bufferNumber = 0; bufferNumber < totalBuffers; ++bufferNumber) {
	  //Check the data buffer is ready to be read from.
	  checkDataBufferReady();

	  //Each read from the System ACE registers returns a 16 bit word. Therefore 16 reads must be performed
	  //to retrieve all the data in the buffer.
	  const u32 totalWords = 16;
	  for (u32 wordNumber = 0; wordNumber < totalWords; ++wordNumber) {
	    //The memory buffer is a FIFO so the same location can be read each time and the FIFO automatically puts the next piece of data there.
	    //The offset is defaulted to 0 so it always reads from the same place.
	    //This will not work in byte mode. The control register would have to be read at regAddr 0x40.
	    data = theFed->vmeCommandSysAceDataRead(0);

	    //As we are writing in binary mode we have to write it a byte at a time.
	    char byte[2];
	    byte[0] = data & 0xff;
	    byte[1] = (data>>8) & 0xff;
	    cfOutputImage.write(byte, 2);

	    if (controlCCaught) {
	      if (!controlCMarker) {
		msg << "Control C detected... finishing cluster write to CF card, will then exit." << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
		msg.str("");
	      }
	      controlCMarker = true;
	    }

	  }//End of word for
	}//End of buffer for

	//Clean up, so ready to read next cluster.
	//Need to reset the card and release the lock granted in getCompactFlashLock().
	regAddr = 0x0c;
	data = theFed->vmeCommandSysAceControlRead(regAddr);
	//Ensure we won't reload after reset.
	data &= ~0x0010;
	//Reset
	data |=  0x0080;
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	//Both lock request and force lock request bits are cleared.
	data &= ~0x0003;
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	//Check that the lock is released
	u32 maxLoops = 100;
	u32 loops    = 0;
	while (theFed->vmeCommandSysAceControlRead(0x02) & 0x0002) {
	  //The MPU lock has not been realeased. Try again.
	  data &= ~0x0003;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  ICUTILS_VERIFY(loops++ < maxLoops)(theFed->vmeCommandSysAceControlRead(0x02) & 0x0002).error().msg("Could not release the MPU lock on the CF");
	}

	if (controlCMarker) {
	  msg << "Exiting" << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");
	  exit(0);
	}
	//Increment the LBA so that it points to the next cluster to be read.
	lba += 256;
      }//End of cluster for

      cfOutputImage << "\nEND OF FILE" << std::endl;
      cfOutputImage.close();

      timer1.stop();
      timer2.stop();
      msg << "Time to read image: " << timer2.read()/1000000.0 << "s." << std::endl;
      msg << "Time to run method: " << timer1.read()/1000000.0 << "s." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");

      //Can stop handling a control C now.
      handleControlC = false;

      return *this;
    } catch (const ICUtils::ICException& e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      handleControlC = false;
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCfImageFile."));
    }
    catch (const std::exception &e) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      handleControlC = false;
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      //Make sure we exit if some one pressed control C!
      if (controlCCaught) 
	exit(0);
      handleControlC = false;
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::cfDataBufferWriteDump() throw (Fed9UVmeDeviceException) {
    try {
      //Calculate the total number of buffers that are to be written.
      //Each buffer is 32 bytes big and each sector is 512 bytes big.
      //There should only ever be <1 sectors worth of data in the buffer.
      const u32 totalBuffers = 512 / 32;
      //Writes to each each buffer.
      for (u32 bufferNumber = 0; bufferNumber < totalBuffers; ++bufferNumber) {
	//No requesting, or checking if the buffer it ready. It wants us to write data so write zeros!!!
	//It won't hurt to write too much as the system ace will just ignore us.

	//Each write to the System ACE register is a 16 bit word. Therefore 16 writes must be performed
	//to set all the data in the buffer.
	const u32 totalWords = 16;
	for (u32 wordNumber = 0; wordNumber < totalWords; ++wordNumber) {
	  theFed->vmeCommandSysAceDataWrite(0);
	}//End of word for
      }//End of buffer for

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::cfDataBufferWriteDump."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }


  Fed9UVmeDevice& Fed9UVmeDevice::cfDataBufferReadDump() throw (Fed9UVmeDeviceException) {
    try {
      //Calculate the total number of buffers that are to be read.
      //Each buffer is 32 bytes big and each sector is 512 bytes big.
      //There should only ever be <1 sectors worth of data in the buffer.
      const u32 totalBuffers = 512 / 32;
      //Read to each each buffer.
      for (u32 bufferNumber = 0; bufferNumber < totalBuffers; ++bufferNumber) {
	//No requesting, or checking if the buffer it ready. It wants to give us some data!!!
	//It won't hurt to read too much as the system ace will just ignore us.

	//Each write to the System ACE register is a 16 bit word. Therefore 16 reads must be performed
	//to set all the data in the buffer.
	const u32 totalWords = 16;
	for (u32 wordNumber = 0; wordNumber < totalWords; ++wordNumber) {
	  theFed->vmeCommandSysAceDataRead();
	}//End of word for
      }//End of buffer for

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::cfDataBufferReadDump."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }

  /**
   * Returns the version number.
   *
   * The returned string contains the major version, minor version and revision number each separated
   * by an underscore.
   */
  string Fed9UVmeDevice::getSystemAceVersionNumber() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //In word mode the version number is contained in register 0x0b. In byte mode 0x16 and 0x17.
      u32 regAddr = 0x0b;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      //If stored directly as a string it would interpret the numbers as ascii values and convert them to characters.
      //By storing them as an ostringstream and converting it to a string, using its memeber function, the number will be correctly interpreted.
      ostringstream versionNumber;
      versionNumber << ( (data & 0xf000) >> 12) << '_' << ( (data & 0x0f00) >> 8) << '_' << (data & 0x00ff);
      return versionNumber.str();
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getSystemAceVersionNumber."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns the address that the current firmware is loaded from.
   *
   * The Compact Flash (CF) card can contain multiple firmware versions. This provides the address off the firmware
   * used on the CF card. It should be used if the current firmware version is to be reloaded. It is a 3 bit number.
   */
  u16 Fed9UVmeDevice::getCfFirmwareAddress() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //The address is contain in bit [13:15] of register 0x02 in word mode. These are bits [5:7] in register 0x05 in byte mode.
      u32 regAddr = 0x02;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      return (data & 0xe0) >> 13;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCfFirmwareAddress."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns an 17 element vector which contains all the setting in the status register. A description of
   * all the elements is given below. For a more detailed description the XILINX System ACE Compact Flash (CF)
   * Solution documentation should be consulted.
   *
   * 0 Controller Lock Status - true if the controller has successfully locked to the CF.
   * 1 MPU Lock Status - true if the MPU has successfully locked to the CF.
   * 2 Configuration Controller Error Status - true if a configuration controller error has occured. Consult the
   *                                           error register for more information.
   * 3 CF Controller Error Status - true if a CF error has occured. Consult the error register for more information.
   * 4 CF Detect - true if there is a CF device connected to the ACE controller.
   * 5 Data Buffer Ready - true when the data buffer is ready to be read from or written to.
   * 6 Data Buffer Mode - true when the data buffer is in a write only state and false when in a read only state.
   * 7 Configuration Done - true when teh ACE controller configuration process is complete.
   * 8 Ready For CF Controller Command - true means ready for a command.
   * 9 Configuration Mode - true means that the configuration process will automatically start after the ACE
   *                        controller reset.
   * 10 CF Busy - if true the CF is not ready to accept commands. If false then elements 1-6 are not valid.
   * 11 CF Ready - if true the CF is ready for operation. It reflects the state of the RDY bit in the CF status register.
   * 12 CF Data Write Fault - true if there is a fault in the write process.
   * 13 CF Ready - if true the CF is ready for operation. It reflects the state of the DSC bit in the CF status register.
   * 14 CF Data Request Bit - if true the data is to be transferred to/from the CF.
   * 15 CF Correctable Error - true if a correctable error has occurred. See the error register for more details.
   * 16 CF Error - true if an error has occurred. See the error register for more details.
   */
  vector<bool> Fed9UVmeDevice::getSystemAceStatusRegister() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //The status register is spread over two 16 bit registers.
      //This is the register address in word mode. The register addresses are 0x04 and 0x05 in byte mode.
      u32 regAddr = 0x02;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      vector<bool> statusRegister(17,false);
      statusRegister[0] = (data & 0x1);
      statusRegister[1] = (data & 0x2) >> 1;
      statusRegister[2] = (data & 0x4) >> 2;
      statusRegister[3] = (data & 0x8) >> 3;
      statusRegister[4] = (data & 0x10) >> 4;
      statusRegister[5] = (data & 0x20) >> 5;
      statusRegister[6] = (data & 0x40) >> 6;
      statusRegister[7] = (data & 0x80) >> 7;
      statusRegister[8] = (data & 0x100) >> 8;
      statusRegister[9] = (data & 0x200) >> 9;
      //This now reads from the second register. 0x06 and 0x07 in byte mode.
      regAddr = 0x03;
      data = theFed->vmeCommandSysAceControlRead(regAddr);
      statusRegister[10] = (data & 0x2) >> 1;
      statusRegister[11] = (data & 0x4) >> 2;
      statusRegister[12] = (data & 0x8) >> 3;
      statusRegister[13] = (data & 0x10) >> 4;
      statusRegister[14] = (data & 0x20) >> 5;
      statusRegister[15] = (data & 0x40) >> 6;
      statusRegister[16] = (data & 0x80) >> 7;

      return statusRegister;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getSystemAceStatusRegister."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns a five element vector that contains all the error bits for the configuration
   * register.
   *
   * This register is used to configure the Compact Flash (CF) for reads and writes. Each element
   * is described below.
   * 0 Address Error - true if the configuration address used to specify the location of the firmware on
   *                   the CF is invalid.
   * 1 Configuration Failure - true if there is a failure during the configuration of the FPGAs.
   * 2 Read Error - true if an error occured during a read of the CF.
   * 3 Instruction Error - true if an invalid instruction was encountered during configuration.
   * 4 Monitor Error - true if the configuration did not start within 500ms of being told to start.
   */
  vector<bool> Fed9UVmeDevice::getSysAceConfigErrorStatus() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //The error register is spread over a 16 bit register, 0x04 in word mode. In byte mode it is two registers 0x08 and 0x09.
      u32 regAddr = 0x04;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      vector<bool> configErrorStatus(5,false);

      configErrorStatus[0] = (data & 0x20) >> 5;
      configErrorStatus[1] = (data & 0x40) >> 6;
      configErrorStatus[2] = (data & 0x80) >> 7;
      configErrorStatus[3] = (data & 0x100) >> 8;
      configErrorStatus[4] = (data & 0x200) >> 9;

      return configErrorStatus;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getsysAceConfigErrorStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns a 10 element vector that contains all the Compact Flash error bits.
   *
   * The means of each element is described below.
   * 0 Reset Error - true if the CF failed to reset properly.
   * 1 CF Ready Error - true if the CF failed to become ready.
   * 2 Read Error - true if the CF read command failed.
   * 3 Write Error - true if a CF write comand failed.
   * 4 Sector Ready Error - true if a sector failed to become ready during a CF read or write.
   * 5 Bad Block Error - true if a bad block has been detected on the CF.
   * 6 Uncorrectable Error - true if an uncorrectable error has been encountered.
   * 7 ID Not Found - true if the requested sector ID is in error or cannot be found.
   * 8 Command Abort - true if a CF command was aborted.
   * 9 General Error - true if a general error has occured.
   */
  vector<bool> Fed9UVmeDevice::getCfErrorStatus() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //The error register is a 16 bit register, 0x04 in word mode. In byte mode it is two registers 0x08 and 0x09.
      u32 regAddr = 0x04;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      vector<bool> cfErrorStatus(10,false);

      cfErrorStatus[0] = data & 0x1;
      cfErrorStatus[1] = (data & 0x2) >> 1;
      cfErrorStatus[2] = (data & 0x4) >> 2;
      cfErrorStatus[3] = (data & 0x8) >> 3;
      cfErrorStatus[4] = (data & 0x10) >> 4;
      cfErrorStatus[5] = (data & 0x800) >> 11;
      cfErrorStatus[6] = (data & 0x1000) >> 12;
      cfErrorStatus[7] = (data & 0x2000) >> 13;
      cfErrorStatus[8] = (data & 0x4000) >> 14;
      cfErrorStatus[9] = (data & 0x8000) >> 15;

      return cfErrorStatus;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCfErrorStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This returns an 8 element vector which contains the status of the Compact Flash (CF)
   * partition. The CF flash can be formatted as either a FAT12 or FAT16 partition.
   * This returns the status of this partition and which type it is formatted as.
   *
   * The contents of the vector elements is as follows.
   * 0 Master Boot Record (MBR) - true if there is a valid boot record.
   * 1 Partition Boot Record (PBR) - true if there is a vaild partition boot record.
   * 2 MBR FAT12 Flag - true if the FAT12 flag is set in the MBR.
   * 3 PBR FAT12 Flag - true if the FAT12 flag is set in the PBR.
   * 4 MBR FAT16 Flag - true if the FAT16 flag is set in the MBR.
   * 5 PBR FAT16 Flag - true if the FAT16 flag is set in the PBR.
   * 6 Calculate if FAT12 or not based on the cluster count - true if FAT12 format.
   * 7 Calculate if FAT16 or not based on the cluster count - true if FAT16 format.
   */
  vector<bool> Fed9UVmeDevice::getCfFatStatus() throw (Fed9UVmeDeviceException) {
    try {
      setSysAceWordMode();
      //The FAT status register is a 16 bit register, 0x0e in word mode. In byte mode it is register 0x1c.
      u32 regAddr = 0x0e;
      u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
      vector<bool> fatStatus(8,false);

      fatStatus[0] = data & 0x1;
      fatStatus[1] = (data & 0x2) >> 1;
      fatStatus[2] = (data & 0x4) >> 2;
      fatStatus[3] = (data & 0x8) >> 3;
      fatStatus[4] = (data & 0x10) >> 4;
      fatStatus[5] = (data & 0x20) >> 5;
      fatStatus[6] = (data & 0x40) >> 6;
      fatStatus[7] = (data & 0x80) >> 7;

      return fatStatus;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCfFatStatus."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Returns the number of events that have been readout over the VME link.
   */
  u32 Fed9UVmeDevice::vmeEventCounter() const throw (Fed9UVmeDeviceException) {
    try {
      u32 numberOfEvents;
      theFed->vmeCommandReadEventCounter(numberOfEvents);
      return numberOfEvents;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::vmeEventCounter."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u32 Fed9UVmeDevice::hasEvent() const throw (Fed9UVmeDeviceException) {
    try {
      u32 hasEvent;
      theFed->vmeCommandReadoutBufferStatus(hasEvent);
      //Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << std::string("checking has event!!!!!!!!! in FED\n");
      return hasEvent;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::hasEvent."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * The event in the buffer is read out to the location of memory specified by the
   * destBuffer pointer.
   *
   * The argument numU32sAddedToBuffer will return the number of 32 bits words that
   * were added to the destBuffer. The freeBufferSpace is the size of the destBuffer
   * in 32 bits words and the event counter will return the number of readouts that were
   * performed over the VME link (will be redudent when using S-Link). The software is
   * capable of performing a block transfer, which is faster than reading out a single
   * 32 bit word at a time. By setting this value to false a block transfer will be performed,
   * true and single reads will be used instead.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::getCompleteEvent(u32* destBuffer, u32 freeBufSpace, u32&  numU32sAddedToBuffer, u32& eventCounter, bool blockDisable) throw (Fed9UVmeDeviceException) {
    try {
      //If we are reading in block mode... just call turbo style!
      if (!blockDisable) {
        return getCompleteEventTurboStyle(destBuffer, freeBufSpace, numU32sAddedToBuffer, eventCounter); 
      } else {
	//Use single reads.      
	u32 bufferLength;
	//Declare some local readout management variables.
	bool readout         = true;
	u32  i               = 0;
	u32  offset          = 0;
	u32  nloops          = 0;
	//Ensure that we start counting at 0.
	numU32sAddedToBuffer = 0;
	//Checks that there is an event present to start the read out process.
	if (hasEvent() == 0) {
	  THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "There is no event in the buffer to read."));
	}
	do {
	  //Keeps track of how much data has been written so far and
	  //ensures that it does not go over the size of the buffer.
	  theFed->vmeCommandReadBufferLength(bufferLength);
	  //	  if (bufferLength < 10000) exit(0);
	  numU32sAddedToBuffer += bufferLength;
	  ICUTILS_VERIFY(numU32sAddedToBuffer <= freeBufSpace)(numU32sAddedToBuffer)(freeBufSpace).error().msg("The destination buffer length is smaller than the buffer that is being read");
	  //Exits the loop when the last fragment bit is set.
	  if(hasEvent() & 0x2) {
	    readout = false;
	    theFed->vmeCommandReadEventCounter(eventCounter);
	  }

	  offset = 0;
	  for(i = 0; i < bufferLength; ++i) {
	    theFed->vmeCommandReadBufferBlock(destBuffer, offset);
	    //Increases the destination buffer by 4 bytes (a u32) each time).
	    ++destBuffer;
	    //Increase the offset by 4 bytes at a time.
	    offset += 0x04;
	  }

	  theFed->vmeCommandResetControlStatus();

	  // now we must wait for the next event fragment to be ready
	  u32 j=0;
	  while( readout & ( ( hasEvent() & 0x1 ) != 1 )){
	    ICUTILS_VERIFY(j++ < 1000)(j)(1000).error().msg("The event readout is trapped in an infinite loop.");
	  }

	  //In case FED fails to flag last event fragment, we must avoid infinite loop.
	  const u32 maxLoops = 100;
	  ICUTILS_VERIFY(nloops++ < maxLoops)(nloops)(maxLoops).error().msg("The event readout is trapped in an infinite loop.");

	} while(readout);
	return *this;
      }
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getCompleteEvents."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * The event in the buffer is read out to the location of memory specified by the
   * destBuffer pointer.
   *
   * The argument numU32sAddedToBuffer will return the number of 32 bits words that
   * were added to the destBuffer. The freeBufferSpace is the size of the destBuffer
   * in 32 bits words and the event counter will return the number of readouts that were
   * performed over the VME link (will be redundant when using S-Link).
   * this method actually uses the full capability of the SBS block Transfer
   */
  Fed9UVmeDevice& Fed9UVmeDevice::getCompleteEventTurboStyle(u32* destBuffer, u32 freeBufSpace, u32&  numU32sAddedToBuffer, u32& eventCounter) throw (Fed9UVmeDeviceException) {
    try {
      u32 bufferLength;
      //Declare some local readout management variables.
      bool readout         = true;
      u32  nloops          = 0;
      const u32 maxLoops = 100;
      //Ensure that we start counting at 0.
      numU32sAddedToBuffer = 0;
      //If we are in block mode we will need the dest buffer in bytes.
      char* destBlockBuffer = reinterpret_cast<char*>(destBuffer);
      //Checks that there is an event present to start the read out process.
      if (hasEvent() == 0) {
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "There is no event in the buffer to read."));
      }
      do {
	//Keeps track of how much data has been written so far and
	//ensures that it does not go over the size of the buffer.
        theFed->vmeCommandReadBufferLength(bufferLength);
	numU32sAddedToBuffer += bufferLength;
	ICUTILS_VERIFY(numU32sAddedToBuffer <= freeBufSpace)(numU32sAddedToBuffer)(freeBufSpace).error().msg("The destination buffer length is smaller than the buffer that is being read");
	//Exits the loop when the last fragment bit is set.
        if(hasEvent() & 0x2) {
	  readout = false;
	  theFed->vmeCommandReadEventCounter(eventCounter);
	}
        theFed->vmeCommandBlockReadBufferBlock(bufferLength << 2, destBlockBuffer, 0);
        destBlockBuffer += bufferLength << 2;
        theFed->vmeCommandResetControlStatus();
        // now we must wait for the next event fragment to be ready
        u32 j=0;
        while( readout & ( ( hasEvent() & 0x1 ) != 1 )){
          ICUTILS_VERIFY(j++ < 1000)(j)(1000).error().msg("The event readout is trapped in an infinite loop.");
        }

	std::ostringstream msg;
	msg << "polled "<< j << " times" << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	msg.str("");
	//In case FED fails to flag last event fragment, we must avoid infinite loop
	ICUTILS_VERIFY(nloops++ < maxLoops)(nloops)(maxLoops).error().msg("The event readout is trapped in an infinite loop.");

      } while(readout);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getCompleteEvents."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  
  /**
   * Purge the event buffer.
   *
   * Removes any existing events from the buffer. It returns the number of events that
   * were purged and the number of events that have been readout over the VME link.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::purgeEvents(u32& numberOfPurgedEvents, u32& eventCounter) throw (Fed9UVmeDeviceException) {
    try {
      u32 checkEvent       = 0;
      numberOfPurgedEvents = 0;
      u32 count = 0,count2 = 0;
      do {


	// now wait till the checkEvent has been set if qdr frame count is still higher than 0
	theFed->vmeCommandReadoutBufferStatus(checkEvent);
	count = 0;
	while (	Fed9UCounters(getBeEventCounterStatus()).qdrFrameCount > 0 && !( checkEvent & 0x01 ) && count <  100000) {
	  theFed->vmeCommandReadoutBufferStatus(checkEvent);
	  count++;
	} 
	ICUTILS_VERIFY(count < 100000)(count).error().msg("Unable to purge fed buffers most likely because it is OOS and events are not pushed.");
	  
	if (checkEvent & 0x01 == 1) {
	  if ( (checkEvent&0x02) == 0x02 ) numberOfPurgedEvents++;
	  theFed->vmeCommandResetControlStatus();
	  fed9Uwait(100);
	}
	count2++;
      } while (checkEvent != 0 && count2<1000);
        ICUTILS_VERIFY(count2 < 1000)(count2).error().msg("Unable to purge fed buffers most likely because it is OOS and events are not pushed.");


      theFed->vmeCommandReadEventCounter(eventCounter);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::purgeEvents."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This is used to write to an byte of memory on the EPROM. It takes a two element
   * vector that contains the byte to be written and the location that byte should be
   * written to.
   *
   * The byte to be written to is given in the first element of the vector.
   * The memory address of the byte to be written is given in the second element of
   * the vector.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::epromWrite(const vector<u32>& epromDataAndOffset) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(epromDataAndOffset.size() == 2)(epromDataAndOffset.size()).error().msg("The argument vector is of an invalid size.");
      ICUTILS_VERIFY(epromDataAndOffset[1] < 0x800)(epromDataAndOffset[1]).error().msg("The memory location you wish to write to is invalid.");
      //If the write fails then either a busy or an error signel can be returned by the EPROM.
      bool busy  = false;
      bool error = false;
      //Used to limit the number of times that it attempts to write if a busy or error signal is present.
      u32 loopCount      = 0;
      const u32 maxLoops = 100;
      do {
	//Write to the EPROM.
	theFed->vmeCommandEpromControl(WRITE, epromDataAndOffset, dummyWord, busy, error);
	//If the write still fails after a set number of attempts then
	//the software gives up and throws an error.
	ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then serial EPROM chip is busy. If error = 1 then the write failed");
	//If the write fails there is a pause before it tries again.
	if (busy || error) {
	  fed9Uwait(100000);
	}
	++loopCount;
	//Will try to write again if a busy or error signal is present.
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
   * This is used to read a byte of memory on the EPROM. It takes only one arguement
   * the address of the byte of memory to be read.
   */
  u32 Fed9UVmeDevice::epromRead(u32 offset) const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(offset < 0x800)(offset).error().msg("The memory location you wish to read from is invalid.");
      //If the write fails then either a busy or an error signle can be returned by the EPROM.
      bool busy  = false;
      bool error = false;
      //Used to limit the number of times that it attempts to write if a busy or error signal is present.
      u32 loopCount      = 0;
      const u32 maxLoops = 100;
      //Used to hold the value that is read back.
      u32 epromData;
      //theFed requires a vector with the second element for the memory address that is to be written to.
      vector<u32> memoryAddr(2,0);
      memoryAddr[1] = offset;
      do {
	//Read from the EPROM.
	theFed->vmeCommandEpromControl(READ, memoryAddr, epromData, busy, error);
	//If the write still fails after a set number of attempts then
	//the software gives up and throws an error.
	ICUTILS_VERIFY(loopCount < maxLoops)(loopCount)(maxLoops)(busy)(error).error().msg("If busy = 1, then serial EPROM chip is busy. If error = 1 then the write failed");
	//If the write fails there is a pause before it tries again.
	if (busy || error) {
	  fed9Uwait(1000000);
      cout << "Fed9UVmeDevice::epromRead(u32 offset), busy || error are true" << endl; 
	}
	++loopCount;
      } while (busy || error);
      return epromData;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Before the Compact Flash (CF) card can be read from the System ACE controller must get a lock.
   * If the method fails to get a lock then a Fed9UVmeDeviceException error will be thrown.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::getCompactFlashLock() throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;
      //This loops until either a lock is achieved or it times out.
      bool cfLocked = false;
      u32 maxLoops  = 100;
      u32 loopCount = 0;
      u32 regAddr   = 0x00;
      u32 data      = 0;
      do {
	//A CF lock must be requested.
	//This is done by setting bit 1 of register 0x0c high.
	//The existing settings are first read back and then bit 1 updated.
	regAddr = 0x0c;
	data    = theFed->vmeCommandSysAceControlRead(regAddr);
	data    |= 0x0002;
	msg << "Requested lock. Data (bit 1 = 1) : 0x" << hex << data << dec << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	msg.str("");
	theFed->vmeCommandSysAceControlWrite(data, regAddr);
	//A lock is achieved when bit 1 of register 0x02 is set high. If in byte mode use reg 0x04.
	regAddr = 0x02;
	data = theFed->vmeCommandSysAceControlRead(regAddr);
	data = theFed->vmeCommandSysAceControlRead(regAddr);// did this twice cause John Coughland did it twice in his code!!!

	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << "CF status reg, want 0x2: " << (data) << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << "CF status reg & 0x2, want 0x2: " << (data & 0x0002) << std::endl;
	if (0x2 == (data & 0x2) ) {
	  msg << "Lock granted." << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	  msg.str("");
	  cfLocked = true;
	} else if ( !(data & 0x0003) && (loopCount > maxLoops - 5) ) {
	  //No one else has a lock, but we can't get one and the CF has had time to finish what it might be doing.
	  msg << "Lock not granted and no one else has a lock, forcing lock." << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");
	  //Force a lock by first resetting...
	  regAddr = 0x0c;
	  data    = 0x0080;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Allow some time to reset.
	  fed9Uwait(1000);
	  //...and the set the force lock and request bits.
	  data    = 0x0003;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Give it time to give us a lock.
	  fed9Uwait(500000);
	} else if ( (data & 0x0001) && (loopCount > (maxLoops-2)) ) {
	  //A lock has been granted to the configuration controller. It has had enough time to finish so force a lock.
	  msg << "Forcing lock away from configuration controller." << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");
	  //Force a lock by first resetting...
	  regAddr = 0x0c;
	  data    = 0x0080;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Allow some time to reset.
	  fed9Uwait(1000);
	  //...and the set the force lock and request bits.
	  data    = 0x0003;
	  theFed->vmeCommandSysAceControlWrite(data, regAddr);
	  //Give it time to give us a lock.
	  fed9Uwait(500000);
	} else {
	  //Wait a bit before requesting lock again.
	  msg << "Lock not granted, waiting for a bit." << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");
	  fed9Uwait(10000);
	}
	//If this is checked on the final loop then it can cause the expection to be thrown even if locked!
	if (!cfLocked) ICUTILS_VERIFY(loopCount++ < maxLoops).error().msg("The CF card has failed to lock.");

	//Now loop back round and check the lock again.
      } while(!cfLocked);

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::getCompactFlashLock."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * Once the System ACE has a lock with the Compact Flash (CF) card it must then check to see if the
   * CF device is ready to receive commands.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::checkCfReadyForCommands() throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;
      //Initialise the counter variables.
      bool cfReady = false;
      u32 maxLoops = 200;
      u32 loopCount = 0;
      //The CF is ready for a command when bit 8 of register 0x02 is set high. If in byte mode use reg 0x05.
      u32 regAddr = 0x02;
      do {
	u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
	data = theFed->vmeCommandSysAceControlRead(regAddr);// do this twice cause JC does it twice!!!
	if (data & 0x0100) {
	  msg << "Ready for commands :-D" << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	  msg.str("");
	  cfReady = true;
	} else {
	  msg << "Not ready for commands. Data : " << hex << data << dec << "Waiting..." << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	  msg.str("");
	  fed9Uwait(10000);
	}
	ICUTILS_VERIFY(loopCount++ < maxLoops).error().msg("The CF is not ready to receive commands.");
      } while(!cfReady);
      
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::checkCfReadyForCommands."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This sets the starting Logical Block Address (LBA), the number of sectors that are to be written
   * to and whether the Compact Flash (CF) will be written to or read from.
   *
   * A true will enable the CF to be read from and a false to be written to. The sector count has
   * a maximum value of 256 and a minimum of 1. The LBA is a pointer to the first 0.5KB block of
   * memory on the CF card that the read/write should start from. It has a maximum value of 128,000
   * on a 64MB CF card.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setDataRegionToReadOrWrite(u32 lba, u32 sectorCount, bool read) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY( (sectorCount >= 1) && (sectorCount <= 256) && (lba <= 128000) )(sectorCount)(lba).error().msg("Either the sector count or the logical block address (lba) is invalid.");

      std::ostringstream msg;

      //The logical block address is spread over two 16 bit registers (4 in byte mode).
      u32 lbaLower = lba & 0xffff;
      //In the upper register only the first 12 bits are relevant.
      u32 lbaUpper = (lba >> 16) & 0xfff;
      msg << "LBA : " << lba << " lbaLower : " << lbaLower << " lbaUpper : " << lbaUpper << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");
      //The lba should be written to registers 0x08 and 0x09 (0x10 - 0x13 in byte mode).
      u32 regAddr = 0x08;
      theFed->vmeCommandSysAceControlWrite(lbaLower, regAddr);
      regAddr = 0x09;
      theFed->vmeCommandSysAceControlWrite(lbaUpper, regAddr);

      //The sector count is an 8 bit number, where 0 corresponds to 256 on the CF.
      if (256 == sectorCount) {
	sectorCount = 0;
      }
      //This will decide whether the CF should read from or written to.
      u32 sectorCommand;
      if (read) {
	sectorCommand = 3;
      } else {
	sectorCommand = 4;
      }
      //In register 0x0a the sector count takes up the first 8 bits and the sector command the last 8 bits.
      //In byte mode the sector count is in reg 0x14 and the command is reg 0x15.
      regAddr  = 0x0a;
      u32 data = ((sectorCommand & 0x7) << 8) | (sectorCount & 0xff);
      msg << "sectorCommand : " << hex << sectorCommand << " sectorCount : " << sectorCount << " data : " << data << dec << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");
      theFed->vmeCommandSysAceControlWrite(data, regAddr);
      msg << "Wait to allow data buffer to get ready..." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");
      //Wait to allow the data buffer to get ready.
      fed9Uwait(1000000);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setDataRegionToReadOrWrite."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  /**
   * This polls the data buffer to ensure it is ready to be read.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::checkDataBufferReady() throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;
      //Initialise the counter variables.
      bool dataBufReady = false;
      u32 maxLoops = 200;
      u32 loopCount = 0;
      //The data buffer is ready to transfer data when bit 5 of register 0x02 is set high. If in byte mode use reg 0x04.
      u32 regAddr = 0x02;
      do {
	u32 data = theFed->vmeCommandSysAceControlRead(regAddr);
	data = theFed->vmeCommandSysAceControlRead(regAddr); // did it twice cause JC does it twice in his software!!!
	if (data & 0x0020) {
	  dataBufReady = true;
	} else {
	  msg << "Buffer not ready, status: 0x" << hex << data << dec << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << std::endl;
	  msg.str("");
	  fed9Uwait(10000);
	}
	ICUTILS_VERIFY(loopCount++ < maxLoops).error().msg("The data buffer is not ready.");
      } while(!dataBufReady);
      
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::checkDataBufferReady."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  vector<string> Fed9UVmeDevice::setCfFileHeader(u32 maxRev)  throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;
      //A stream to hold the file name.
      ostringstream fileName;
      //A stream to hold the file header.
      ostringstream fileHeader;
      //Maximum number of firmware IDs that need to be read.
      const u32 maxFirmId(4);
      //Just incase we can't reload from a revision on CF.
      vector<bool> validRev(maxRev, true);
      //Saves the position of none zero firmware values.
      vector<bool> revNoneZero(maxRev, true);
      //Firmware versions loaded from revision 0 for comparrison.
      vector<u32> revZeroVersions(maxFirmId,0);
      for (u32 rev(0); rev < maxRev; ++rev) {
	msg << "Checking revision " << dec << rev << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << std::endl;
	msg.str("");
	//Incase there is a failure to reload all the firmwares correctly then we try the reload again.
	bool reload(false);
 	u32 reloadAttempts(0);
 	const u32 maxReloadAttempts(1);
 	do {
	  try {
	    reloadFpgaFirmwareFromCompactFlash(rev);
	  } catch (const ICUtils::ICException& e) {
	    //If the revision fails to reload it will thrown this type of exception.
	    //We need to catch it and make a note of it.
	    validRev[rev] = false;
	  }
	  //fed9Uwait(8000000);
	  if (validRev[rev]) {
	    if (0 == rev) {
	      //Get the firmware versions for later comparison.
	      //Both getFe and getDelayFirmwareVersion check that all the FE and delay firmware version are the same.
	      //They throw error if they are not and thus we need to be prepared for that.
	      try {
		//Has to be none zero to be here
		fed9Uwait(8000000);

		revNoneZero[rev] = true;

		revZeroVersions[0] = getFeFirmwareVersion();
		revZeroVersions[1] = getDelayFirmwareVersion();
		revZeroVersions[2] = getBeFirmwareVersion();
		revZeroVersions[3] = getVmeFirmwareVersion();

		//Create the binary encoded file name.
		u32 feDelayBits(0);
		u32 beVmeBits(0);

		feDelayBits = (revZeroVersions[0]&0xfff)<<12 | (revZeroVersions[1]&0xfff);
		beVmeBits   = (revZeroVersions[2]&0xfff)<<12 | (revZeroVersions[3]&0xfff);
		fileName << hex << setw(6) << setfill('0') << feDelayBits;
		fileName << hex << setw(6) << setfill('0') << beVmeBits;

		//Save versions to file header stream.
		//Write revision number to file header stream.
		fileHeader << "Revision: " << rev << '\n';
		fileHeader << hex;
		fileHeader << "\tFE FPGA    : vers = $ " << setw(8) << setfill('0') << revZeroVersions[0] << '\n';
		fileHeader << "\tDELAY FPGA : vers = $ " << setw(8) << setfill('0') << revZeroVersions[2] << '\n';
		fileHeader << "\tBE FPGA    : vers = $ " << setw(8) << setfill('0') << revZeroVersions[1] << '\n';
		fileHeader << "\tVME FPGA   : vers = $ " << setw(8) << setfill('0') << revZeroVersions[3] << '\n';
		fileHeader << dec;
	      } catch (Fed9UVmeDeviceException& e) {
		//One of the versions must have been different on the FE or delay chips. Try reloading again.
		reload = true;
	      }
	    } else {//if (0==rev)
	      try {
		//Get the firmware versions
		vector<u32> currentFirmwareRevs(maxFirmId,0);
		currentFirmwareRevs[0] = getFeFirmwareVersion();
		currentFirmwareRevs[1] = getDelayFirmwareVersion();
		currentFirmwareRevs[2] = getBeFirmwareVersion();
		currentFirmwareRevs[3] = getVmeFirmwareVersion();

 		msg << "Revision: 0x" << rev << hex << '\n'
		    << "Current version\tRevision 0\n"
		    << currentFirmwareRevs[0] << "\t" << revZeroVersions[0] << "\n"
		    << currentFirmwareRevs[1] << "\t" << revZeroVersions[1] << "\n"
		    << currentFirmwareRevs[2] << "\t" << revZeroVersions[2] << "\n"
		    << currentFirmwareRevs[3] << "\t" << revZeroVersions[3] << "\n"
		    << dec << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
		msg.str("");
		//Check they are not equal to the revision 0 firmwares.
		u32 firmNotEqual(0);
		//Loops over the number of firmware versions there are.
		for (u32 currentFirm(0); currentFirm < maxFirmId; ++currentFirm) {
		  //Only set if the firmwares are different.
		  if ( currentFirmwareRevs[currentFirm] != revZeroVersions[currentFirm] ) {
		    firmNotEqual |= (1<<currentFirm);
		  }
		}//for currentFirm

		const u32 allEqual(0);
		if (firmNotEqual != allEqual) {
		  msg << "actually writing another revision to file." << std::endl;
		  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
		  msg.str("");
		  //The firmware IDs are different on this revision so save the data.

		  //Has to be none zero to be here!
		  revNoneZero[rev] = true;

		  //Create the binary encoded file name.
		  u32 feDelayBits(0);
		  u32 beVmeBits(0);

		  feDelayBits = (currentFirmwareRevs[0]&0xfff)<<12 | (currentFirmwareRevs[1]&0xfff);
		  beVmeBits   = (currentFirmwareRevs[2]&0xfff)<<12 | (currentFirmwareRevs[3]&0xfff);
		  fileName << setw(6) << setfill('0') << feDelayBits;
		  fileName << setw(6) << setfill('0') << beVmeBits;

		  //Write revision number to file header stream.
		  fileHeader << "Revision: " << rev << '\n';
		  fileHeader << hex;
		  fileHeader << "\tFE FPGA\t: vers = $ " << setw(8) << setfill('0') << currentFirmwareRevs[0] << '\n';
		  fileHeader << "\tBE FPGA\t: vers = $ " << setw(8) << setfill('0') << currentFirmwareRevs[1]  << '\n';
		  fileHeader << "\tDELAY FPGA\t: vers = $ " << setw(8) << setfill('0') << currentFirmwareRevs[2] << '\n';
		  fileHeader << "\tVME FPGA\t: vers = $ " << setw(8) << setfill('0') << currentFirmwareRevs[3] << '\n';
		  fileHeader << dec;
		} else {//if (firmNotEqual == checkNotEqual)
		  //Not a valid revision here
		  revNoneZero[rev] = false;

		  //Still need an entry in the file name, but it is zero.
		  fileName << setw(6) << setfill('0') << 0;
		  fileName << setw(6) << setfill('0') << 0;
		}//else if (firmNotEqual == checkNotEqual)
	      } catch (Fed9UVmeDeviceException& e) {
		//One of the versions must have been different on the FE or delay chips. Try reloading again.
		reload = true;
	      }
	    }//else from if (0==rev)
	  } else {//if (validRev[rev])
	    //invalid revision, write zeros to the file name.
	    //Create the binary encoded file name, in this case it is just zeros.
	    fileName << hex << setw(6) << setfill('0') << 0;
	    fileName << hex << setw(6) << setfill('0') << 0;
	    revNoneZero[rev] = false;
	    msg << "Revision " << rev << " was invalid."  << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	    msg.str("");
	  }//else from if (validRev[rev])

 	  ICUTILS_VERIFY(maxReloadAttempts > reloadAttempts)(reloadAttempts)(maxReloadAttempts).error().msg("Failed to read consistant FE/delay FPGA firmware version. Not recording revision data.");
 	  ++reloadAttempts;
 	} while (reload);
      }
       
      //Written all the revisions so add the file header.
      fileHeader << "END OF FILE HEADER" << std::endl;
      //Reload revision zero as there should always be something valid there.
      reloadFpgaFirmwareFromCompactFlash(0);

      //Strip any zero values for the revisions from the end of the file name.
      u16 lastNoneZeroRev(7);
      msg << "Is it a none zero rev?\n";
      for (vector<bool>::const_iterator it(revNoneZero.begin()); it != revNoneZero.end(); ++it) {
	msg << (*it ? "true":"false") << "\t";
      }
      msg << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << msg.str();
      msg.str("");
      for (vector<bool>::const_iterator it(revNoneZero.end()-1); it != revNoneZero.begin(); --it) {
	//We we are current at a false and the next one is true, then there must be none zero revisions before this point.
	if ( ( !(*it) && (*(it-1)) ) || lastNoneZeroRev == 0)
	  break;

	--lastNoneZeroRev;
      }
      msg << "lastNoneZeroRev: " << lastNoneZeroRev << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");
      string stringFileName( fileName.str() );
      string minimalName( stringFileName.begin(), (string::iterator( stringFileName.begin()+(lastNoneZeroRev*12) )) );
      minimalName += ".ace";
      vector<string> returnStrings;
      returnStrings.push_back( minimalName );
      returnStrings.push_back( fileHeader.str() );
      return returnStrings;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::setCfFileHeader."));
    } catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::removeCfFileHeaderAndMarkers(ifstream& cfInputFile, std::ios::pos_type& endDataMarker, string& firmwareInfo/*, std::ios::pos_type& startDataMarker*/) throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;
      msg << "Removing CF file header and markers." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
      msg.str("");

      //Check the file is in a good state.
      ICUTILS_VERIFY( cfInputFile.good() )(cfInputFile.rdstate()).error().msg("The file with formatted data on did not open.");

      //Check the full file is there.
      //Marks end of file.
      const string endMarker("END OF FILE");
      //Holds last line in the buffer temporarily.
      char* buf = new char[endMarker.size()];
      //Move to beginning of marker...
      //std::cout << "First TellG() " <<  cfInputFile.tellg() << std::endl;
      //    cfInputFile.seekg(-12,std::ios::end);
      //std::cout << "EOF TellG() " <<  cfInputFile.tellg() << std::endl;
      //cfInputFile.read(buf, endMarker.size());     
      //std::cout << "read = " << buf << std::endl;

      
      //      cfInputFile.clear();
      i16 tempSize = -(endMarker.size()+1);
      cfInputFile.seekg( tempSize, std::ios::end);
      //std::cout << "endMarker.size()+1 = " << (endMarker.size()+1) << std::endl;
      //	std::cout << "First TellG() " <<  cfInputFile.tellg() << std::endl;
      //...save marker point for end of file data checking...
      endDataMarker = cfInputFile.tellg();
      //std::cout << "First TellG() " <<  cfInputFile.tellg() << std::endl;

      //...readout the last line...
      cfInputFile.read(buf, endMarker.size());

      //std::cout << "read = " << buf << std::endl;

      //...turn it into a string for easy comparison...
      const string lastLine(buf, endMarker.size());//Give the size!

      //...delete the buf...
      if (!buf)
	delete buf;
      //...and throw an error if necessary.
      ICUTILS_VERIFY(endMarker == lastLine)(endMarker)(lastLine).error().msg("The compact flash file image is incomplete. Upload of new firmware cancelled.");

      //Go back to the beginning of the file for header extraction.
      cfInputFile.seekg(0,std::ios::beg);
      //Holds the firmware information from the file header.
      ostringstream firmwareInfoStream;
      //Stores each read line.
      string imageLine;
      //Marker for the end of the header section.
      const string endHeaderMarker("END OF FILE HEADER");
      u32 headLoops(0);
      const u32 maxHeadLoops(80);
      bool endOfHeader(false);
      do {
	getline(cfInputFile, imageLine);
	if (endHeaderMarker == imageLine) endOfHeader = true;
	if (!endOfHeader) firmwareInfoStream << imageLine << '\n';
	ICUTILS_VERIFY(maxHeadLoops > headLoops++)(headLoops)(maxHeadLoops).error().msg("Failed to find the end of the file header.");
      } while ( !endOfHeader );
      //Save the start position of the start of the data, leave the file pointing at it anyway.
      //startDataMarker = cfInputFile.tellg();
      //Write firmware info to the string.
      firmwareInfo = firmwareInfoStream.str();
      //Return.
      return *this;

    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::removeCfFileHeaderAndMarkers."));
    } catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  bool Fed9UVmeDevice::pollCfForFirmwareReload() const throw (Fed9UVmeDeviceException) {
    try {
      std::ostringstream msg;

      //Poll until new firmware is loaded, check status bit 7 to find out.
      u32 regAddr(0x02);
      u32 data(0);
      u32 polls(0);
      u32 maxPolls(20);
      bool reloaded(true);
      do {
	//Poll until the system ace tells us it has reloaded the card.
	//In the event of an empty revision it will not load any new firmware, and the loop will time out setting the revision invalid.
	data = theFed->vmeCommandSysAceControlRead(regAddr);
	if (maxPolls < polls) {
	  msg << "Compact flash card failed to reload, there is no valid revision at your chosen location."
	      << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");
	  reloaded = false;
	  break;
	}
	++polls;
	fed9Uwait(1000000);
      } while (!((data & 0x0080) && ((data & 0x0001) != 0x0001))  );//Loop until the lock is release (bit 0=0) and configuration is done (bit 7=1).

      return reloaded;

    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9USYSTEMACE, "error in Fed9UVmeDevice::removeCfFileHeaderAndMarkers."));
    } catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  Fed9UVmeDevice& Fed9UVmeDevice::sendVmeBusReset() throw (Fed9UVmeDeviceException) {
    theFed->vmeCommandResetBus();
  }

   /**
     * This will reset VME Bus using the Bus controller resetBus Method.
     *
     */
  Fed9UVmeDevice& Fed9UVmeDevice::sendVmeBusResetInhibit(bool value) throw (Fed9UVmeDeviceException) { 
    theFed->vmeCommandResetBusInhibit(value);
  }
}//end of Fed9U namespace
