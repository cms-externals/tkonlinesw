/***************************************************************************************************
 * Notes for coders:
 * The FED uses a positive logic system. This means that a boolean true is used to ENABLE a setting.
 * The local FED settings which is an object of the Fed9UDescription class use a negative logic
 * system. This means that they use a boolean true to DISABLE a setting. Hence in the following
 * methods booleans returned from the description are inverted before being passed to the FED and
 * vice versa.
 *
 * Most of the methods require a Fed9UAddress to specify what area of the FED you wish to address.
 * theFed, however requires an integer. As the address object often refers to the same location
 * as that integer and a style has been adopted that allows the integer and object to be easily
 * distinguished. Integers passed to theFed in the same methods that have Fed9UAddress objects
 * have the form 'addr_', whereas the objects have the form 'addr'.
 *
 * This classes uses the 'internal' number scheme defined in Fed9UAddress:
 *  FPGA         0-7 (front end), 9 (back end), 10 VME, 14 (broadcast to all front-end)
 *  FED channel  0-95
 *  FPGA channel 0-11
 *  FED APV      0-191
 *  FPGA APV     0-23
 *  Channel APV  0-1
 * FPGA number scheme is converted to an initial FE FPGA of 1 increase each FE and broadcast FPGA
 * number by 1 to be sent to theFed. BE and VME are unchanged.
 ***************************************************************************************************/
#include <inttypes.h>
#include <stdint.h>

#include "Fed9UVmeDevice.hh"
#include "Fed9UAddress.hh"
#include "Fed9UVmeBase.hh"
//#include "Fed9UWait.hh"
#include "sstream"

#ifdef EVENT_STREAMLINE
#include "Fed9UEventStreamLine.hh"
#include "Fed9UEventUnitStreamLine.hh"
#else
#include "Fed9UEvent.hh"
#endif


namespace Fed9U {
  using std::ostringstream;

  /************************************************************************************************
   *                These are the commands for the Front End (FE) FPGAs                           *
   ************************************************************************************************/


    // <JRF date=16/02/2010>
    /**
     * \brief  sets the periodic counter for spy arming every N events.
     * \param  spyArmPeriod to set the value of N to arm on every Nth trigger
     */
  Fed9UVmeDevice& Fed9UVmeDevice::setSpyArmPeriod(const u32  spyArmPeriod) throw (Fed9UVmeDeviceException) {
    try {
      Fed9UAddress feFpga;
      feFpga.setFedFeUnit(Fed9UAddress::FEBROADCAST);
      vector<Fed9U::u32> arguments(1,0),dummy;
      arguments[0] = spyArmPeriod;
      theFed->feCommandSpyPeriod(feFpga.getFirmwareFedFeUnit(),
                     WRITE,
                     arguments,
                     dummy);
      return *this;  
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setSpyArmPeriod."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }

  u32 Fed9UVmeDevice::getSpyArmPeriod() throw (Fed9UVmeDeviceException) {

    try {
      Fed9UAddress feFpga;
      feFpga.setFedFeUnit(1);
      vector<Fed9U::u32> readArgs,dummy;
      theFed->feCommandSpyPeriod(feFpga.getFirmwareFedFeUnit(),
                     READ,
                     dummy,
                     readArgs);
      return readArgs[0]; 

    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getSpyArmPeriod."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }


  // <NAC date=28/03/2007>
  void Fed9UVmeDevice::armSpy(const Fed9UAddress& selectedDelayChip) throw (Fed9UVmeDeviceException) {
    try {
      theFed->feCommandArmSpyChannel(selectedDelayChip.getFirmwareFedFeUnit(),selectedDelayChip.getFeUnitDelayChip());
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::armSpy."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  // </NAC>
  // <NAC date=23/03/2007>
  void Fed9UVmeDevice::fireSpy(const Fed9UAddress& selectedDelayChip, void* destBuffer) throw (Fed9UVmeDeviceException)
  {
    try {
      //get the spy channel data
      std::vector<Fed9U::u32> spyData( theFed->feCommandFireSpyChannel(selectedDelayChip.getFirmwareFedFeUnit(),selectedDelayChip.getFeUnitDelayChip()) );
      //copy data into buffer
      memcpy(destBuffer,&spyData.front(),spyData.size()*sizeof(Fed9U::u32));
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::fireSpy."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  // </NAC>
  Fed9UVmeDevice& Fed9UVmeDevice::setOptoRxInputOffset(const Fed9UAddress& feFpga, u16 inputOffset) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(inputOffset < 16)(inputOffset).error().msg("An invalid argument has been passed.");
      //Retrieve the other OptoRx settings from the description.
      u16 outputOffset = theLocalFedSettings.getOptoRxOutputOffset(feFpga);
      u16 capacitor    = theLocalFedSettings.getOptoRxCapacitor(feFpga);
      //Create the OptoRx command to be written to the FED.
      vector<u32> optoRxSettings(1,0);
      optoRxSettings[0] |= (inputOffset  & 0xf);
      optoRxSettings[0] |= (outputOffset & 0x3) << 4;
      optoRxSettings[0] |= (capacitor    & 0x3) << 6;
      //Write that command to the description.
      theFed->feCommandOptoControl(feFpga.getFirmwareFedFeUnit(), WRITE, optoRxSettings, dummyVector);
      //Update theLocalFedSettings.
      theLocalFedSettings.setOptoRxInputOffset(feFpga, inputOffset);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setOptoRxInputOffsets."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u16 Fed9UVmeDevice::getOptoRxInputOffset(const Fed9UAddress& feFpga) const  throw (Fed9UVmeDeviceException) {
    try {
      //Retrieves the OptoRx settings from the FED.
      vector<u32> optoRxSettings;
      theFed->feCommandOptoControl(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, optoRxSettings);
      //Selects the inputOffsets setting from the OptoRxSettings and returns it.
      return (optoRxSettings[0] & 0xf);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getOptoRxInputOffsets."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setOptoRxOutputOffset(const Fed9UAddress& feFpga, u16 outputOffset) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(outputOffset < 4)(outputOffset).error().msg("An invalid argument has been passed.");
      //Retrieve the other OptoRx settings from the description.
      u16 inputOffset = theLocalFedSettings.getOptoRxInputOffset(feFpga);
      u16 capacitor   = theLocalFedSettings.getOptoRxCapacitor(feFpga);
      //Create the OptoRx command to be written to the FED.
      vector<u32> optoRxSettings(1,0);
      optoRxSettings[0] |= (inputOffset  & 0xf);
      optoRxSettings[0] |= (outputOffset & 0x3) << 4;
      optoRxSettings[0] |= (capacitor    & 0x3) << 6;
      //Write that command to the description.
      theFed->feCommandOptoControl( feFpga.getFirmwareFedFeUnit(), WRITE, optoRxSettings, dummyVector);
      //Update theLocalFedSettings.
      theLocalFedSettings.setOptoRxOutputOffset(feFpga, outputOffset);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setOptoRxOutputOffsets."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u16 Fed9UVmeDevice::getOptoRxOutputOffset(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      //Retrieves the OptoRx settings from the FED.
      vector<u32> optoRxSettings;
      theFed->feCommandOptoControl(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, optoRxSettings);
      //Selects the outputOffsets setting from the OptoRxSettings and returns it.
      return (optoRxSettings[0] >> 4) & 0x3;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getOptoRxOutputOffsets."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setOptoRxCapacitor(const Fed9UAddress& feFpga, u16 capacitor) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(capacitor < 4)(capacitor).error().msg("An invalid argument has been passed.");
      //Retrieve the other OptoRx settings from the description.
      u16 inputOffset  = theLocalFedSettings.getOptoRxInputOffset(feFpga);
      u16 outputOffset = theLocalFedSettings.getOptoRxOutputOffset(feFpga);
      //Create the OptoRx command to be written to the FED.
      vector<u32> optoRxSettings(1,0);
      optoRxSettings[0] |= (inputOffset  & 0xf);
      optoRxSettings[0] |= (outputOffset & 0x3) << 4;
      optoRxSettings[0] |= (capacitor    & 0x3) << 6;
      //Write that command to the description.
      theFed->feCommandOptoControl(feFpga.getFirmwareFedFeUnit(), WRITE, optoRxSettings, dummyVector);
      //Update theLocalFedSettings.
      theLocalFedSettings.setOptoRxCapacitor(feFpga, capacitor);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setOptoRxCapcitor."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  u16 Fed9UVmeDevice::getOptoRxCapacitor(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      //Retrieves the OptoRx settings from the FED.
      vector<u32> optoRxSettings;
      theFed->feCommandOptoControl(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, optoRxSettings);
      //Selects the capacitor setting from the OptoRxSettings and returns it.
      return (optoRxSettings[0] >> 6) & 0x3;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getOptoRxCapacitor."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setPedestals(const Fed9UAddress& fedApv, const vector<u16>& pedestals) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == pedestals.size())(pedestals.size()).error().msg("The pedestal vector is of an invalid size.");
      for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	ICUTILS_VERIFY(pedestals[apvStrip] < 1024)(apvStrip)(pedestals[apvStrip]).error().msg("An invalid argument has been passed");
      }
      //Retrieves the description of each strip on the user chosen FED APV.
      vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );
      //A vector to hold the disable strip values before they are written to the FED.
      vector<bool> disabledStrips(STRIPS_PER_APV,0);
      for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	disabledStrips[apvStrip_] = apvStripsDescription[apvStrip_].getDisable();
      }
      //Starts the next stage in the writing to FED process.
      setPedsAndDisabledStrips(fedApv, pedestals, disabledStrips);
      //There is no need to write these updated values to the description as that will be done by the setPedsAndDisabledStrips method.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setPedestals."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  vector<u16> Fed9UVmeDevice::getPedestals(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //Holds the pedestal values that are returned by reference.
      vector<u16> pedestals;
      //The disabled strips and the number of valid strips must also be read back as well but they are not needed.
      vector<bool> disabledStrips;
      u16 numberOfValidStrips;
      getPedsAndDisabledStrips(fedApv, pedestals, disabledStrips, numberOfValidStrips);

      return pedestals;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getPedestals."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setDisabledStrips(const Fed9UAddress& fedApv, const vector<bool>& disabledStrips) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == disabledStrips.size())(disabledStrips.size()).error().msg("The disabled strip vector is of an invalid size.");
      //Retrieves the description of each strip on the user chosen FED APV.
      vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );
      //A vector to hold the pedestals values before they are written to the FED.
      vector<u16> pedestals(STRIPS_PER_APV,0);
      for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	pedestals[apvStrip_] = apvStripsDescription[apvStrip_].getPedestal();
      }
      //Starts the next stage in the writing to FED process.
      setPedsAndDisabledStrips(fedApv, pedestals, disabledStrips);
      //There is no need to write these updated values to the description as that will be done by the setPedsAndDisabledStrips method called above.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDisabledStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  vector<bool> Fed9UVmeDevice::getDisabledStrips(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //Holds the disabled strip settings that are returned by reference.
      vector<bool> disabledStrips;
      //The pedestals and the number of valid strips must also be read back as well but they are not needed.
      vector<u16> pedestals;
      u16 numberOfValidStrips;
      getPedsAndDisabledStrips(fedApv, pedestals, disabledStrips, numberOfValidStrips);

      return disabledStrips;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDisabledStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Returns the number of valid strips used in the Common Mode Median calculation.
   *
   * This number is determined by the number of valid strips and cannot be set on its own.
   */
  u16  Fed9UVmeDevice::getNumberOfValidStrips(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //Holds the number of valid strips that are returned by reference.
      u16 numberOfValidStrips;
      //The pedestals and disabled strip settings must also be read back as well but they are not needed.
      vector<u16> pedestals;
      vector<bool> disabledStrips;
      getPedsAndDisabledStrips(fedApv, pedestals, disabledStrips, numberOfValidStrips);

      return numberOfValidStrips;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getDisabledStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setPedsAndDisabledStrips(const Fed9UAddress& fedApv, const vector<u16>& pedestals, const vector<bool>& disabledStrips) throw (Fed9UVmeDeviceException) {
    //The validStrips vector is not a vector of booleans because it needs to be passed to
    //the setStripData(). This requires a vector of u16s in order to be compatible with the
    //setClusterThresholds() which cannot pass a boolean vector.
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == pedestals.size() && STRIPS_PER_APV == disabledStrips.size())(pedestals.size())(disabledStrips.size()).error().msg("Either the pedestal vector or the disabled strip vector is of an invalid size.");
      for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	ICUTILS_VERIFY(pedestals[apvStrip] < 1024)(apvStrip)(pedestals[apvStrip]).error().msg("An invalid argument has been passed");
      }
      //createStripData uses the FED logic system so the disableStrip vector values must be inverted.
      vector<u16> validStrips(STRIPS_PER_APV,0);
      for (u32 i = 0; i < STRIPS_PER_APV; ++i) {
	validStrips[i] = !disabledStrips[i];
      }
      //This writes to the FED.
      //A false is sent to signify that the pedestals and disbaled strip are being set.
      const bool setPeds = false;
      createStripData(fedApv, setPeds, pedestals, validStrips);
      //This sets the number of valid strips that are used in the median calculation.
      //It must be equal to the number of valid channels in the validStrips vector.
      setNumberValidStrips(fedApv, disabledStrips);
      //Writes the user arguments to the description file.
      vector<Fed9UStripDescription> stripDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );

      for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	stripDescription[apvStrip_].setPedestal(pedestals[apvStrip_]);
	stripDescription[apvStrip_].setDisable(disabledStrips[apvStrip_]);
      }

      theLocalFedSettings.getFedStrips().setApvStrips(fedApv, stripDescription);
      //\todo - Fed9UDescription API does not match Fed9UVmeDevice API
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setPedsAndDisabledStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  const Fed9UVmeDevice& Fed9UVmeDevice::getPedsAndDisabledStrips(const Fed9UAddress& fedApv, vector<u16>& pedestals, vector<bool>& disabledStrips, u16& numberValidStrips) const throw (Fed9UVmeDeviceException) {
    //The validStrips vector is not a vector of booleans because it needs to be passed to
    //the setStripData(). This requires a u16 vector in order to be compatible with the
    //setClusterThresholds() which cannot pass a boolean vector.
    try {
      vector<u16> disOrderedPeds(STRIPS_PER_APV,0);
      vector<u16> disOrderedValStrips(STRIPS_PER_APV,0);
      //This will return a vector which contains a one if the strip is valid and zero if not.
      getPedsAndDisabledStripData(fedApv, disOrderedPeds, disOrderedValStrips);
      numberValidStrips = getNumberValidStrips(fedApv);

      //This does not need to be initialised as the re-ordering method will do that.
      vector<u16> reOrderedValStrips;

      //The order of the elements that are returned from the FED is not in the strip order.
      //This method is used to re-order the returned vector so that the elements are in strip order.
      reOrderingAlgorithm(disOrderedPeds, pedestals);
      reOrderingAlgorithm(disOrderedValStrips, reOrderedValStrips);
      //The validStrip vector must be inverted so that a one means disable and a zero enabled
      //so that it matches the Fed9UDescription logic system.
      // <NAC date="24/04/2007"> set size of disabledStrips in case this is called with disabledStrips.size() < STRIPS_PER_APV (eg by getPedestals())
      disabledStrips.clear();
      disabledStrips.resize(STRIPS_PER_APV);
      // </NAC>
      for (u32 i = 0; i < STRIPS_PER_APV; ++i) {
	disabledStrips[i] = !reOrderedValStrips[i];
      }
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getPedsAndDisabledStrips."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Sets the 128 high cluster thresholds for an APV.
   *
   * Two types of cluster threshold can be set. The high thresholds are used to identify single strip clusters
   * in the cluster finding algorithm. This has a maximum value of 255.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setHighClusterThresholds(const Fed9UAddress& fedApv, const vector<u16>& highThresholds) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == highThresholds.size())(highThresholds.size()).error().msg("The high thresholds vector is of an invalid size.");
      for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	ICUTILS_VERIFY(highThresholds[apvStrip] < 256)(apvStrip)(highThresholds[apvStrip]).error().msg("An invalid argument has been passed");
      }
      //Check to see if the APV is disabled, because if it is we write high and low cluster thresholds to be max.
      if ( theLocalFedSettings.getApvDisable(fedApv) ) {
	//APV is disabled so write maximum values for the high and low thresholds.
	vector<u16> lowThresholdsMax(STRIPS_PER_APV, 255);
	vector<u16> highThresholdsMax(STRIPS_PER_APV, 255);
	//Starts the next stage in the writing to FED process.
	setClusterThresholds(fedApv, highThresholdsMax, lowThresholdsMax);
      } else {
	//Leave the high thresholds unmodified and get the low thresholds from the description.
	//A vector to hold the low threshold values before they are written to the FED.
	vector<u16> lowThresholds(STRIPS_PER_APV,0);
	vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );
	for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	  lowThresholds[apvStrip] = apvStripsDescription[apvStrip].getLowThreshold();
	}
	//Starts the next stage in the writing to FED process.
	setClusterThresholds(fedApv, highThresholds, lowThresholds);
      }
      //There is no need to write these updated values to the description as that will be done by the setClusterThresholds method called above.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setHighClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  vector<u16> Fed9UVmeDevice::getHighClusterThresholds(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //Holds the high cluster thresholds that are return by reference.
      vector<u16> highClusters;
      //The low clusters are also read bakc but they are not needed.
      vector<u16> lowClusters;
      getClusterThresholds(fedApv, highClusters, lowClusters);

      return highClusters;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getHighClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setLowClusterThresholds(const Fed9UAddress& fedApv, const vector<u16>& lowThresholds) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == lowThresholds.size())(lowThresholds.size()).error().msg("The low thresholds vector is of an invalid size.");
      for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	ICUTILS_VERIFY(lowThresholds[apvStrip] < 256)(apvStrip)(lowThresholds[apvStrip]).error().msg("An invalid argument has been passed");
      }
      //Check to see if the APV is disabled, because if it is we write high and low cluster thresholds to be max.
      if ( theLocalFedSettings.getApvDisable(fedApv) ) {
	//APV is disabled so write maximum values for the high and low thresholds.
	vector<u16> lowThresholdsMax(STRIPS_PER_APV, 255);
	vector<u16> highThresholdsMax(STRIPS_PER_APV, 255);
	//Starts the next stage in the writing to FED process.
	setClusterThresholds(fedApv, highThresholdsMax, lowThresholdsMax);
      } else {
	//APV is enabled so write the given high thresholds and get the low from the description to be written.
	//Retrieves the description of each strip on the user chosen FED APV.
	vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );
	//A vector to hold the high threshold values before they are written to the FED.
	vector<u16> highThresholds(STRIPS_PER_APV,0);
	for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	  highThresholds[apvStrip] = apvStripsDescription[apvStrip].getHighThreshold();
	}
	//Starts the next stage in the writing to FED process.
	setClusterThresholds(fedApv, highThresholds, lowThresholds);
      }
      //There is no need to write these updated values to the description as that will be done by the setClusterThresholds method called above.
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setLowClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  vector<u16> Fed9UVmeDevice::getLowClusterThresholds(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //Holds the low cluster thresholds that are return by reference.
      vector<u16> lowClusters;
      //The high clusters are also read bakc but they are not needed.
      vector<u16> highClusters;
      getClusterThresholds(fedApv, highClusters, lowClusters);

      return lowClusters;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getLowClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setClusterThresholds(const Fed9UAddress& fedApv, const vector<u16>& highThresholds, const vector<u16>& lowThresholds) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(STRIPS_PER_APV == highThresholds.size() && STRIPS_PER_APV == lowThresholds.size())(highThresholds.size())(lowThresholds.size()).error().msg("Either the high thresolds or the low thresholds vector is of an invalid size.");
      for (u32 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	ICUTILS_VERIFY( (highThresholds[apvStrip] < 256) && (lowThresholds[apvStrip] < 256) )(apvStrip)(highThresholds[apvStrip])(lowThresholds[apvStrip]).error().msg("An invalid argument has been passed");
	ICUTILS_VERIFY(highThresholds[apvStrip] >= lowThresholds[apvStrip])(apvStrip)(highThresholds[apvStrip])(lowThresholds[apvStrip]).error().msg("The high threshold must be greater than the low threshold.");
      }
      //A true is used to signify a that the cluster thresholds are being written to.
      const bool setCluster = true;
      createStripData(fedApv, setCluster, highThresholds, lowThresholds);
      //Writes the user arguments to the description file.
      vector<Fed9UStripDescription> apvStripsDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );

      for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	apvStripsDescription[apvStrip_].setLowThreshold( lowThresholds[apvStrip_] );
	apvStripsDescription[apvStrip_].setHighThreshold( highThresholds[apvStrip_] );
      }

      theLocalFedSettings.getFedStrips().setApvStrips(fedApv, apvStripsDescription);
      //\todo - Fed9UDescription API does not match Fed9UVmeDevice API
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Gets the 128 cluster thresholds for an APV.
   *
   * Two types of clusters can be returned. The high thresholds and the low thresholds. The high threshold
   * is to be used on single strip hits and low on multiple strip hits. The maximum threshold value is 255
   * for both high and low thresholds. Each vector should contain 128 values which corresponds to an APV
   * of strips. Each element in the vector corresponds to the value for that strip number.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::getClusterThresholds(const Fed9UAddress& fedApv, vector<u16>& highThresholds, vector<u16>& lowThresholds) const throw (Fed9UVmeDeviceException) {
    try {
      //This exists to provide a more user fristd::endly means of returning the cluster thresholds and as such need to do very little.
      getClusterData(fedApv, highThresholds, lowThresholds);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getClusterThresholds."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Performs a soft reset on the FPGA.
   *
   * Performs a one clock cycle reset on a specified FE FPGA.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::feFpgaSoftReset(const Fed9UAddress& feFpga) throw (Fed9UVmeDeviceException) {
    try {
      theFed->feCommandSoftReset(feFpga.getFirmwareFedFeUnit());
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::softReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * A method to set the ADC controls. It takes a Fed9UAdcControl structure. This contains
   * all the ADC settings.
   *
   * This relates the ADC letter to the channel number.
   * adc_a_top = opto ch C11 & C08
   * adc_a_bot =         C09 & C10
   * adc_b_top = opto ch C07 & C04
   * adc_b_bot =         C05 & C06
   * adc_c_top = opto ch C03 & C00
   * adc_c_bot =         C01 & C02
   * when the C number is the channel number.
   * The ADC bit shift array values are calculated from the FED FE FPGA
   * technical description which shows the return value format.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setAdcControls(const Fed9UAddress& fedChannel, const Fed9UAdcControls& adcControlSettings) throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      u16 fedChannel_  = fedChannel.getFedChannel();
      vector<u32> adcSettings(1,0);
      //This is used to select the first bit of the range of bits that represent
      //the ADC settings that are to be written to the FED.
      //const u16 bitShiftValue[12] = {4, 0, 0, 4, 12, 8, 8, 12, 20, 16, 16, 20};
      const u16 bitShiftValue[12] = {20, 16, 16, 20, 12, 8, 8, 12, 4, 0, 0, 4}; //bug fix <JRF> 20/10/2006 channel order was wrong so disabling ADC on channel pair 0 and 3 would disable channel pair 8 and 11 by mistake.
      //Only the even numbers need to be written as there are two channels per ADC.
      //If both channels are written then this can cause problem in the construction
      //of adcSettings due to the over writing of bits. This array will select the
      //even numbered channel on an ADC.
      const u16 fpgaChannelToBeWritten[12] = {0, 2, 2, 0, 4, 6, 6, 4, 8, 10, 10, 8};
      
      // the following map is used to map a channel number directly to it's paired channel number, i.e. channel 0 maps to 3 since 3 is stored at location 0 in the map.
      const u16 channelPairMap[12] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8};

      Fed9UAddress tempAddress(fedChannel);
      for (u32 i = 0; i < CHANNELS_PER_FEUNIT; i += 2) {
	//If all the APVs on an ADC channel pair are disabled then we can turn off an APV to save power.
	tempAddress.setFeUnitChannel(i); //note you have to set the actual channel to ensure you get the write set of 4 apvs below <JRF> BUG FIX 18/10/2006
	Fed9UAddress apv0(tempAddress.getFedChannel(), 0);
	Fed9UAddress apv1(tempAddress.getFedChannel(), 1);
	tempAddress.setFeUnitChannel(channelPairMap[i]);
	Fed9UAddress apv2( tempAddress.getFedChannel(), 0 );
	Fed9UAddress apv3( tempAddress.getFedChannel(), 1 );
	const bool channelPairDisabled=( getApvDisable(apv0) && getApvDisable(apv1) && getApvDisable(apv2) && getApvDisable(apv3) );
	if ( channelPairDisabled ) {
	  adcSettings[0] |= 0 << (bitShiftValue[i] + 3);
	  adcSettings[0] |= 0 << (bitShiftValue[i] + 2);
	  adcSettings[0] |= 0 << (bitShiftValue[i] + 1);
	  adcSettings[0] |= 0 <<  bitShiftValue[i]     ;
	} else {
	  if (fpgaChannelToBeWritten[fpgaChannel_] != i) {
	    Fed9UAddress addr(fedChannel_ - fpgaChannel_ + i); //sets the FED channel
	    Fed9UAdcControls adcControlBits;
	    adcControlBits = theLocalFedSettings.getAdcControls(addr);

	    adcSettings[0] |= adcControlBits._dfsen  << (bitShiftValue[i] + 3);
	    adcSettings[0] |= adcControlBits._dfsval << (bitShiftValue[i] + 2);
	    adcSettings[0] |= adcControlBits._s1     << (bitShiftValue[i] + 1);
	    adcSettings[0] |= adcControlBits._s2     <<  bitShiftValue[i]     ;

	  } else {
	    adcSettings[0] |= adcControlSettings._dfsen  << (bitShiftValue[i] + 3);
	    adcSettings[0] |= adcControlSettings._dfsval << (bitShiftValue[i] + 2);
	    adcSettings[0] |= adcControlSettings._s1     << (bitShiftValue[i] + 1);
	    adcSettings[0] |= adcControlSettings._s2     <<  bitShiftValue[i]     ;
	  }
	}
      }
      theFed->feCommandAdcControl(fedChannel.getFirmwareFedFeUnit(), WRITE, adcSettings, dummyVector);
      //This writes the new ADC data to the two channels on that ADC. The array is used
      //to select the second channel on that ADC and the first comes from the Fed9UAddress
      //object passed as an argument to this method.
      const u16 secondAdcChannel[12] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8};
      Fed9UAddress addr(fedChannel_ - fpgaChannel_ + secondAdcChannel[fpgaChannel_]); //sets the FED channel
      theLocalFedSettings.setAdcControls(fedChannel, adcControlSettings);
      theLocalFedSettings.setAdcControls(addr, adcControlSettings);

      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setAdcControls."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * The function reads back a 24 bit number which is four bits per ADC and
   * then removes the four bit number specifiying the setting for the channel.
   *
   * This relates the ADC letter to the channel number.
   * adc_a_top = opto ch C11 & C08
   * adc_a_bot =         C09 & C10
   * adc_b_top = opto ch C07 & C04
   * adc_b_bot =         C05 & C06
   * adc_c_top = opto ch C03 & C00
   * adc_c_bot =         C01 & C02
   * when the C number is the channel number.
   * The ADC bit shift array values are calculated from the FED FE FPGA
   * technical description which shows the return value format.
   */
  Fed9UAdcControls Fed9UVmeDevice::getAdcControls(const Fed9UAddress& fedChannel) const throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      //The mask is used to ensure that only one bit of the command is selected.
      const u16 mask = 0x1;
      vector<u32> adcSettings;
      //This is used to select the first bit of the range of bits that are represent
      //the ADC settings that are returned from the FED.
      //const u16 bitShiftValue[12] = {4, 0, 0, 4, 12, 8, 8, 12, 20, 16, 16, 20};
      // bug fix <JRF> had to swap the order of channels in the shift array since they were backwards.
      const u16 bitShiftValue[12] = {20, 16, 16, 20, 12, 8, 8, 12, 4, 0, 0, 4};
      theFed->feCommandAdcControl(fedChannel.getFirmwareFedFeUnit(), READ, dummyVector, adcSettings);
      //The bitShiftValue selects a range of four bits, therefore an amount must be
      //added in order to select the specific bit and the mask ensures that it is
      //only a one bit number.
      Fed9UAdcControls adcControls;
      adcControls._dfsen  = ( adcSettings[0] >> (bitShiftValue[fpgaChannel_]+3) ) & mask;
      adcControls._dfsval = ( adcSettings[0] >> (bitShiftValue[fpgaChannel_]+2) ) & mask;
      adcControls._s1     = ( adcSettings[0] >> (bitShiftValue[fpgaChannel_]+1) ) & mask;
      adcControls._s2     = ( adcSettings[0] >>  bitShiftValue[fpgaChannel_]    ) & mask;
      return adcControls;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getAdcControls."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay) throw (Fed9UVmeDeviceException) {
    try {
      //We are using values for the mapping array here so it does not matter what the size of the fine delay is on the FED command.
      ICUTILS_VERIFY(fineDelay < 25)(fineDelay).error().msg("An invalid delay setting has been passed");

      u16 coarseDelay = theLocalFedSettings.getCoarseDelay(fedChannel);
      setDelay(fedChannel, coarseDelay, fineDelay);
      //The fine delay value will be updated in the description in the setDelay method.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay)"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(coarseDelay < 16)(coarseDelay).error().msg("An invalid delay setting has been passed");

      u16 fineDelay = theLocalFedSettings.getFineDelay(fedChannel);
      setDelay(fedChannel, coarseDelay, fineDelay);
      //The coarse delay value will updated in the description in the setDelay method.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay)"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setDelay(const Fed9UAddress& fedChannel, float nanoDelay) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(nanoDelay < 400 && nanoDelay >= 0)(nanoDelay).error().msg("An invalid delay value has been passed");

      //The coarse delay is a multiple of 25ns.
      const u32 coarseDelay = static_cast<u32>(nanoDelay/25.0);
      //Fine delay is the just the number of nano second steps between each coarse delay.
      const u32 fineDelay   = static_cast<u32>(nanoDelay)%25;

      //setDelay will now perform the fine skew dependent tasks, including the appropriate mapping.
      setDelay(fedChannel, coarseDelay, fineDelay);
      //The fine delay value will updated in the description in the setDelay method.
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setDelay(const Fed9UAddress& fedChannel, float nanoDelay)"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(fineDelay<25 && coarseDelay<16)(fineDelay)(coarseDelay).error().msg("An invalid delay setting has been passed");
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      u16 fedChannel_  = fedChannel.getFedChannel();
      u32 delayChipNumber  = static_cast<u32>( static_cast<float>(fpgaChannel_)/4.0 );
      u32 delayChipChannel = fedChannel_%4;
      const u32 delayChipChannelOrder[4] = {0, 3, 2, 1};
      vector<u32> delayCommandAll(5,0);

      //This will need to be added to the total delay setting. Note that this delay is in the 
      //positive sense like the coarse delay, unlike the fine delay on each channel
      u16 globalFineDelay = theLocalFedSettings.getGlobalFineSkew();

      // now we have to set the values into the local Fed Settings.
      // note that the value stored in the localFedSettings does not directly 
      // represent what is stored on the FED, since we have combined the individual channel delays and the global delay.
      theLocalFedSettings.setDelay(fedChannel, coarseDelay, fineDelay); 

      //Define the fine skew length specific variables. Initialise to 6 bit values.
      u32 coarseShift(6);      
      //Useful for later decisions.
      //switch on firmware to decide which version of the skew map to use
      bool use6BitSkew(true);
      if (theLocalFedSettings.getDelayFirmwareVersion() < FED9U_DELAY_FIRMWARE_VERSION_6_BIT) {
 	use6BitSkew = false;
 	coarseShift = 5;
      }

      // once the desired skews have been loaded all we now have to do is loop over all the channels in this delay chip
      // construct the delay values and then write them to the FED
      u16 fineDelayTemp, coarseDelayTemp;
      for (u32 i = 0; i < CHANNELS_PER_DELAY_CHIP; ++i) {
	//The delay chip channel numbers are not equivalent to the FED channel numbers and so need to be reordered.
	u32 reorderedDelayChannel = delayChipChannelOrder[i];
	Fed9UAddress selectChannel(fedChannel_ - delayChipChannel + i);//set FED channel
	//Retrieve the current fine and coarse delay setting from the description, combine this with the global fine skew and then map that to the 
	//actual setting, don't forget that global fine skew is positive and local fine delay is negative so the combination is non-trivial
	fineDelayTemp    = theLocalFedSettings.getFineDelay(selectChannel);

	coarseDelayTemp  = (theLocalFedSettings.getCoarseDelay(selectChannel)
				+ ( ( (globalFineDelay - fineDelayTemp) <= 0 ) ? 0 : (globalFineDelay - fineDelayTemp) / 25 + 1) );//
		
 	if (use6BitSkew) {
 	  delayCommandAll[reorderedDelayChannel] = (coarseDelayTemp<<coarseShift) | 
 	    _fineSkewMap6Bit[( ( (fineDelayTemp - globalFineDelay%25) >= 0 ) ? ( fineDelayTemp - globalFineDelay % 25 ): fineDelayTemp + ( 25 - globalFineDelay % 25) )];
 	} else {
	  delayCommandAll[reorderedDelayChannel] = (coarseDelayTemp<<coarseShift) | 
	    _fineSkewMap[( ( (fineDelayTemp - globalFineDelay%25) >= 0 ) ? ( fineDelayTemp - globalFineDelay % 25 ): fineDelayTemp + ( 25 - globalFineDelay % 25) )];
 	}

	u32 upperDelayValue(1024);
	if (!use6BitSkew) {
	  upperDelayValue = 512;
	}
	u32 totalDelay = delayCommandAll[reorderedDelayChannel];
	ICUTILS_VERIFY(totalDelay < upperDelayValue)(coarseDelayTemp)(fineDelayTemp)(globalFineDelay).error().msg("The delay skew setting is too large. This is likely due to a large globalFineDelay and large coarse delay.causing coarseDelay to be greater than 16");
      }
      delayCommandAll[4] = delayChipNumber;
      theFed->feCommandLoadSkew(fedChannel.getFirmwareFedFeUnit(), delayCommandAll, use6BitSkew);
      return *this;
    }
    catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "An error has occurred in Fed9UVmeDevice::setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay)."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setApvDisable(const Fed9UAddress& fedApv, bool apvDisable) throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaApv_ = fedApv.getFeUnitApv();
      vector<u32> apvAll(1,0);
      //This gets all the APV disable settings from the FED as a 24 bit number.
      //Each bit must first be inverted to match the FED logic system and it is masked to ensure only the relevant 32 bits have a suitable number.
      //Next the appropiate bit is updated with the User passed setting apvDisable, which also must first be inverted.
      u32 mask = 0x00FFFFFF & ~(0x1<<( (APVS_PER_FEUNIT - 1) - fpgaApv_) );
      apvAll[0] = ( ~theLocalFedSettings.getAllApvDisables(fedApv) & mask ) | ( (static_cast<u32>(!apvDisable) & 0x1) << ( (APVS_PER_FEUNIT - 1) - fpgaApv_) );
      //std::cout << "APV load tick data is : " << std::hex << apvAll[0] <<  std::endl;
      theFed->feCommandLoadTick(fedApv.getFirmwareFedFeUnit(), WRITE, apvAll, dummyVector);
      theLocalFedSettings.setApvDisable(fedApv, apvDisable);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setApvDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will return the state of the APV.
   *
   * A returned value of true signifies that the APV is disabled. A false signifies enabled.
   */
  bool Fed9UVmeDevice::getApvDisable(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaApv_ = fedApv.getFeUnitApv();
      vector<u32> apvAll;
      theFed->feCommandLoadTick(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, apvAll);
      //The bitshift amount is used to select the bit containing the state of the APV from
      //the 24 bit command the minus 1 comes from the fedApv_ having a maximum value of 23
      //and the APVS_PER_FEUNIT having a value of 24.
      u32 bitShiftAmount = (APVS_PER_FEUNIT-1) - fpgaApv_;
      apvAll[0] >>= bitShiftAmount;
      return !(apvAll[0] & 0x1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getApvDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * The thresholds for the frame finding can be set.
   *
   * The frame threshold register sets the value at which the FED recognises
   * an APV frame. This can be in the range 0 to 992, in steps of 32.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setFrameThreshold(const Fed9UAddress& fedChannel, u16 threshOnChannel) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(threshOnChannel <= 992)(threshOnChannel).error().msg("Invalid frame threshold value");
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      u16 fedChannel_  = fedChannel.getFedChannel();
      vector<u32> threshValues(12, 0);

      for (u32 i = 0; i < CHANNELS_PER_FEUNIT; ++i) {
	     Fed9UAddress selectChannel(fedChannel_ - fpgaChannel_ + i); //set the FED channel
	     //The threshold is sent to the FED as a 5 bit number and so must be converted from a range
	     //of 0 - 992 to 0 - 31.
	     if (theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(true)) && theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(false)) ) {
            threshValues[i] = 992;
            theLocalFedSettings.setFrameThreshold(selectChannel,992);
         }
         else { 
              threshValues[i] = theLocalFedSettings.getFrameThreshold(selectChannel);
	     }
         
         threshValues[i] = static_cast<u32>( static_cast<float>(threshValues[i])/32 + 0.5 );
      }
      Fed9UAddress selectChannel=fedChannel;
      threshValues[fpgaChannel_] =( theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(true)) && theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(false))) ?
                                  31 :  static_cast<u32>( static_cast<float>(threshOnChannel)/32.0 + 0.5 );
      theFed->feCommandLoadThresh(fedChannel.getFirmwareFedFeUnit(), WRITE, threshValues, dummyVector);
      theLocalFedSettings.setFrameThreshold(fedChannel, ( theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(true)) && theLocalFedSettings.getApvDisable(selectChannel.setChannelApv(false))) ? 992 :threshOnChannel);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFrameThreshold."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Returns the settings for the frame finding settings on a FED channel.
   *
   * The frame threshold register stores the value at which the FED recognises
   * an APV frame. This can be in the range 0 to 992, in steps of 32.
   */
  u16 Fed9UVmeDevice::getFrameThreshold(const Fed9UAddress& fedChannel) const throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      vector<u32> threshValue(12,0);

      theFed->feCommandLoadThresh(fedChannel.getFirmwareFedFeUnit(), READ, dummyVector, threshValue);
      //The range 0 to 992 but it stored as a 5 bit number (max 31) so this
      //converts it to the 0 to 992 range.
      u16 threshOnChannel = 32 * threshValue[fpgaChannel_];
      return threshOnChannel;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFrameThreshold."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }

  
  Fed9UStrips & Fed9UVmeDevice::getFedStrips()  throw (Fed9UVmeDeviceException) { 
    try {
      return theLocalFedSettings.getFedStrips(); 
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This is used to either enable the common mode (CM) median override values or to disable
   * them.
   *
   * The CM median over-ride has a maximum value of 1023 and a default value of zero. Enabling
   * the CM median over-ride with zero (the default value) will disable the CM median subtraction.
   * To enable the median over-ride register medianOverrrideDisable must be set to false.
   * To disable them it must be set to true. The vector of median values has 24 elements one for each APV
   * FE FPGA. These values will only be used if the median over-ride is enabled and they can only be set
   * if the median over-ride is enabled.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setCmMedianOverride(Fed9UAddress feFpga, bool medianOverrideDisable, vector<u16> medianValues) throw (Fed9UVmeDeviceException) {
    try {
      //Disabling the median override values enables the median calculation on the FED.
      //Therefore a false will be passed to the FED to enable the median override and
      //true to disable the override. It uses the same logic as medianOverrideDisable
      //and so inverting is not required.
      vector<u32> medianOverride(1);
      medianOverride[0]  = (medianOverrideDisable ? 1:0);
      theFed->feCommandMedianEnable(feFpga.getFirmwareFedFeUnit(), WRITE, medianOverride, dummyVector);
      //If the median override setting is enabled (medianOverrrideDisable == false)
      //then the CM median values need to be set.
      if (!medianOverrideDisable) {
	ICUTILS_VERIFY(medianValues.size() == 24)(medianValues.size()).error().msg("An invalid median over-ride vector has been passed. It should contain an element for a single APV.");
	for (u32 fpgaApv = 0; fpgaApv < APVS_PER_FEUNIT; ++fpgaApv) {
	  ICUTILS_VERIFY(medianValues[fpgaApv] < 1024)(medianValues[fpgaApv]).error().msg("An invalid median over-ride value has been passed");
	}
	//Have to copy the vector<u16> to a vector<u32> before it can be passed to the description.
	vector<u32> allMedianValues( medianValues.begin(), medianValues.end() );
	//Now the median common mode values can be written to the FED and theLocalFedSettings.
	theFed->feCommandLoadMedian( feFpga.getFirmwareFedFeUnit(), WRITE, allMedianValues, dummyVector);

      }	//else Do nothing the median override registers have been disabled and so no further action is required.
      theLocalFedSettings.setCmMedianOverride(feFpga, medianOverrideDisable, medianValues);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevce::setCmMedianOverride."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will read the common mode (CM) median override settings on a FE FPGA.
   *
   * The returned vector contains 24 elements, where each element contains the median override
   * value for a FE FPGA APV. If the medianOverrideDisable, which is returned by reference, is
   * true then the median over-ride values are not used and the CM median is calculated by the FED.
   * If it returns false then the median override is enabled and the median over-ride values will
   * be used. If the medianOverrideDisable returns true then the median over-ride values will be
   * zero. In this case the FED is calculating the CM median. If the medianOverrideDisable returns
   * false and the median over-ride values are zero then no median subtraction will take place.
   * The CM median subtraction is disabled.
   */
  vector<u16> Fed9UVmeDevice::getCmMedianOverride(Fed9UAddress feFpga, bool& medianOverrideDisable) const throw (Fed9UVmeDeviceException) {
    try {
      //Used to determine if the median overrides are enabled or disabled.
      vector<u32> medianOverride;
      theFed->feCommandMedianEnable(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, medianOverride);
      medianOverrideDisable = medianOverride[0];
      //An array is returned that contains all the median values for the APVs on a FE FPGA.
      //Each element contains a different APV.
      vector<u32> allMedianValues(24,0);
      if (!medianOverrideDisable) {
	allMedianValues.clear();
	theFed->feCommandLoadMedian(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, allMedianValues);
      } else {
	//Do nothing the allMedianValues vector has already been initialised to have 24 elements
	//all zero. The FED will always read zero for the median value when the over-ride is disabled.
      }
      //Need to convert the vector<u32> to a vector<u16>.
      vector<u16> medianValues( allMedianValues.begin(), allMedianValues.end() );
      return medianValues;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getCmMedianOverride."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This is used to set the common mode (CM) median over-ride value on an APV.
   * Setting the CM median value does not necessarily mean it is enabled. To
   * enable the CM median over-ride the method setCmMedianOverrideDisable must
   * be called with a false.
   *
   * In order to set the over-ride values correctly the CM median over-ride must
   * be enabled. This method will temporarily enable the CM median over-ride
   * if it is not already. It is recommended that this method is only called
   * when the CM median over-ride is enabled.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setCmMedianOverrideValue(const Fed9UAddress& fedApv, u16 medianValue) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(medianValue < 1024)(medianValue).error().msg("An invalid median over-ride value has been passed.");

      //In order to correctly set the CM median over-ride values the CM median
      //over-ride must be enabled. First we must check it is enabled.
      vector<u32> medianOverrideSetting;
      theFed->feCommandMedianEnable(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, medianOverrideSetting);
      //This is used to store the over-ride disable setting so that it can be returned to it orginal state
      //at the end of the method.
      const bool medianOverrideDisable = medianOverrideSetting[0];
      //If the median over-ride is disbaled, it must be enabled.
      if (medianOverrideDisable) {
	medianOverrideSetting[0] = false;
	theFed->feCommandMedianEnable(fedApv.getFirmwareFedFeUnit(), WRITE, medianOverrideSetting, dummyVector);
      }

      //Get the rest of the FE FPGA median values from the description.
      //This will actually store the median disable setting in the description, but I don't need it here.
      bool junkBool = true;
      //The description returns a vector of u16, which cannot be used to construct a vector of u32 so first
      //it must be stored in a vector of u16.
      vector<u16> tempVector( theLocalFedSettings.getCmMedianOverride(fedApv, junkBool) );
      vector<u32> allMedianValues( tempVector.begin(), tempVector.end() );
      allMedianValues[fedApv.getFeUnitApv()] = medianValue;
      //Now the median common mode values can be written to the FED and theLocalFedSettings.
      theFed->feCommandLoadMedian(fedApv.getFirmwareFedFeUnit(), WRITE, allMedianValues, dummyVector);

      //If the median over-ride was disabled at the start of the method it should be reset to disabled.
      if (medianOverrideDisable) {
	medianOverrideSetting[0] = medianOverrideDisable;
	theFed->feCommandMedianEnable(fedApv.getFirmwareFedFeUnit(), WRITE, medianOverrideSetting, dummyVector);
      }

      theLocalFedSettings.setMedianOverride(fedApv, medianValue);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setCmMedianOverrideValue."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This returns the common mode (CM) median over-ride value for an APV.
   *
   * If the CM median over-ride is disabled (which can be checked using the
   * method getCmMedianOverrideDisable) then zero will be returned. If the
   * median over-ride is enabled and zero is returned then the CM median
   * subtraction is effectivily disabled.
   */
  u16 Fed9UVmeDevice::getCmMedianOverrideValue(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
      //An array is returned that contains all the median values for the APVs on a FE FPGA.
      vector<u32> allMedianValues;
      theFed->feCommandLoadMedian(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, allMedianValues);
      //Return the relevant value.
      return allMedianValues[fedApv.getFeUnitApv()];
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getCmMedianOverrideValue."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will enable the common mode (CM) median over-ride for a FE FPGA.
   *
   * Once enabled the FED will not calculate the CM median values, but will
   * use the user defined values by the method setCmMedianOverrideValue.
   * If no values are set the CM median subtraction is disabled. To disable
   * the over-ride a true is passed, to enable it a false.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setCmMedianOverrideDisable(const Fed9UAddress& feFpga, bool disable) throw (Fed9UVmeDeviceException) {
    try {
      //Used to determine if the median overrides are enabled or disabled.
      vector<u32> medianOverrideDisable(1, disable);
      theFed->feCommandMedianEnable(feFpga.getFedFeUnit()+1, WRITE, medianOverrideDisable, dummyVector);
      theLocalFedSettings.setMedianOverrideDisable(feFpga, disable);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setCmMedianOverrideDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will return the common mode (CM) over-ride disable setting. If
   * enabled then the FED will not calcuate the CM median values and use
   * the values set by the method setCmMedianOverrideValue, which can be
   * read using getCmMedianOverrideValue.
   *
   * If the returned value is true then the CM median over-ride is disabled
   * if false then it is enabled.
   */
  bool Fed9UVmeDevice::getCmMedianOverrideDisable(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      //Used to determine if the median overrides are enabled or disabled.
      vector<u32> medianOverrideSetting;
      theFed->feCommandMedianEnable(feFpga.getFedFeUnit()+1, READ, dummyVector, medianOverrideSetting);
      return medianOverrideSetting[0];
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getCmMedianOverrideDisable."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Allows the user to set the number of words of Virgin Raw Data written
   * into the output FIFO after a scope mode event. Must be the same for
   * FE FPGAs.
   * 
   * The maximum number of words that can be written is 1020.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setScopeLength(u16 numberOfWords) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(numberOfWords <= 1020)(numberOfWords).error().msg("An invalid scope length has been passed");

      //As the scope length must be the same for all FE FPGAs the broadcast value is
      //set for the FPGA number.
      const u16 feFpga = 14;
      vector<u32> scopeLength(1, numberOfWords);

      theFed->feCommandLoadScopeLength(feFpga+1, WRITE, scopeLength, dummyVector);
      theLocalFedSettings.setScopeLength(numberOfWords);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setScopeLength."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * The returned value is the number of words of Virgin Raw Data to be
   * written to the output FIFO after a scope mode event.
   *
   * The maximum number of words that can be written is 1020.
   */
  u16 Fed9UVmeDevice::getScopeLength() const throw (Fed9UVmeDeviceException) {
    try {
      u16 numberOfWords;
      vector<u32> scopeLength;

      theFed->feCommandLoadScopeLength(1, READ, dummyVector, scopeLength);
      //Ensures that the scope length is the same for the FE FPGAs
      //starts at FPGA 1 as scopeLength is the value for FPGA 0.
      std::ostringstream strExternalZeroFeFpga;
      strExternalZeroFeFpga << Fed9UAddress().setFedFeUnit(0).getExternalFedFeUnit(); //Get external zero FPGA number for error message.
      for (u32 feFpga = 1; feFpga < FEUNITS_PER_FED; ++feFpga) {
	// if the feunit is disabled then we don't check the scope length for that channel.
	if ( theLocalFedSettings.getFedFeUnitDisable(Fed9UAddress(static_cast<u8>((feFpga - 1 )*12) ,static_cast<u8>(0))) ) continue;
	vector<u32> scopeLengthCheck(1,0);
	theFed->feCommandLoadScopeLength(feFpga+1, READ, dummyVector, scopeLengthCheck);
	std::ostringstream strExternalFeFpga;
	u32 externalFeFpga = Fed9UAddress().setFedFeUnit(feFpga).getExternalFedFeUnit();
	strExternalFeFpga << externalFeFpga;
	string errMesg = "The scope length on fpga " + strExternalFeFpga.str() + " is different from fpga " + strExternalZeroFeFpga.str();
	ICUTILS_VERIFY(scopeLengthCheck[0] == scopeLength[0])(externalFeFpga)(scopeLengthCheck[0])(scopeLength[0]).error().msg(errMesg.c_str());
      }
      numberOfWords = scopeLength[0];
      return numberOfWords;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getScopeLength."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setComplement(const Fed9UAddress& fedChannel, bool disableComplement) throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      //The load complement channel ordering does not match the software channel ordering. This array is used to reorder the channel numbers.
      const u32 fedChannelOrdering[12] = {10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1};
      //The first tweleve bits contains the settings for the 12 channel on a FE FPGA.
      u32 allComplements = 0;
      for (u32 i = 0; i < CHANNELS_PER_FEUNIT; ++i) {
	if (fpgaChannel_ != i) {
	  //An address object used to retrieve the complement settings for the other FE FPGA channel.
	  Fed9UAddress addr(i);
	  allComplements |= !theLocalFedSettings.getComplement(addr) << fedChannelOrdering[i];
	} else {
	  allComplements |= !disableComplement << fedChannelOrdering[fpgaChannel_];
	}
      }
      theFed->feCommandLoadComplement(fedChannel.getFirmwareFedFeUnit(), WRITE, allComplements, dummyWord);
      theLocalFedSettings.setComplement(fedChannel, disableComplement);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setComplement"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This allows the user to read back the complement setting.
   *
   * If the returned value is true then the complement is enabled
   * and all the ones are changed into zeros and the zeros into ones.
   * If false it is disabled. It should be used if the inverter stage of
   * the APVs is turned off.
   */
  bool Fed9UVmeDevice::getComplement(const Fed9UAddress& fedChannel) const throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      //This returned from the fed by reference and the first 12 bits will contain the complement settings for a FE FPGA.
      u32 allComplements;
      theFed->feCommandLoadComplement(fedChannel.getFirmwareFedFeUnit(), READ, dummyWord, allComplements);
      allComplements >>= fpgaChannel_;
      return !(allComplements & 1);
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getComplement"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  Fed9UVmeDevice& Fed9UVmeDevice::setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(offset < 256)(offset).error().msg("An invalid Trim DAC offset has been passed.");
      //std::cout << dec << "Setting trimDACs" << std::endl;
      //These two bools are used to shutdown the Trim DAC chip. A seperate method is provided to reset the DAC
      //and the ability to shut down the chip is not given as a user option.
      const bool reset    = false;
      const bool shutdown = false;
      vector<u32> regAddrAndOffset(2,0);

      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      //std::cout << "fpgaChannel_: " << fpgaChannel_ << std::endl;
      //The Trim DAC register number scheme is in reverse order to the FE FPGA channel numbering
      // and so needs to be reversed before being written to the FED.
      const u32 regAddr[12] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
      regAddrAndOffset[0] = offset;
      regAddrAndOffset[1] = regAddr[fpgaChannel_];

      //std::cout << "setting offset to : " << regAddrAndOffset[0] << " to channel: " << regAddrAndOffset[1] << std::endl;

      theFed->feCommandTrimDacControl(fedChannel.getFirmwareFedFeUnit(), regAddrAndOffset, reset, shutdown);

      //std::cout << "set values on FED. " << std::endl;

      theLocalFedSettings.setTrimDacOffset(fedChannel, offset);

      //std::cout << "in description now returning. " << std::endl;
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setTrimDacOffset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This command is used to reset the DAC.
   *
   * When called this method resets the trim DAC on a specific FE FPGA. One trim DAC
   * controls 12 channels and so is reset for a FPGA.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::sendTrimDacReset(const Fed9UAddress& feFpga) throw (Fed9UVmeDeviceException) {
    try {
      //Must be true in order to reset the DAC.
      const bool reset = true;
      //Not given as a user option.
      const bool shutdown = false;

      theFed->feCommandTrimDacControl(feFpga.getFirmwareFedFeUnit(), dummyVector, reset, shutdown);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::sendTrimDacReset."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This command will read the occupancy of the FPGA buffer on a channel.
   */
  u16 Fed9UVmeDevice::getChannelBufferOccupancy(const Fed9UAddress& fedChannel) const throw (Fed9UVmeDeviceException) {
    try {
      u16 fpgaChannel_ = fedChannel.getFeUnitChannel();
      vector<u32> fpgaOccupancy;

      theFed->feCommandMonitor(fedChannel.getFirmwareFedFeUnit(), fpgaOccupancy);
      u16 channelOccupancy = fpgaOccupancy[fpgaChannel_];
      return channelOccupancy;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFpgaBufferOccupancy"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will read the firmware ID on all the FE FPGAs.
   *
   * It ensures that they are the same before returning the firmware ID.
   */
  u32 Fed9UVmeDevice::getFeFirmwareId() const throw (Fed9UVmeDeviceException) {
    try {
      //Only read the first FE FPGA and ICUTILS_VERIFY that the other FE FPGAs have the same value.
      u32 feFirmwareVersion;
      theFed->feCommandFirmwareIdReg(1, feFirmwareVersion);
      //Ensures that the firmware id is the same for the FE FPGAs
      //starts at FPGA 1 as firmwareVersion is the value for FPGA 0.
      std::ostringstream strExternalZeroFeFpga;
      strExternalZeroFeFpga << static_cast<u16>(Fed9UAddress().setFedFeUnit(0).getExternalFedFeUnit()); //Get external zero FPGA number for error message.
      for (u32 feFpga = 1; feFpga < FEUNITS_PER_FED; feFpga++) {
	u32 feFirmwareVersionCheck;
	theFed->feCommandFirmwareIdReg(feFpga+1, feFirmwareVersionCheck);
	std::ostringstream strExternalFeFpga;
        u32 externalFeFpga = Fed9UAddress().setFedFeUnit(feFpga).getExternalFedFeUnit();
        strExternalFeFpga << externalFeFpga;
        string errMesg = "The FE Firmware ID on fpga " + strExternalFeFpga.str() + " is different from fpga " + strExternalZeroFeFpga.str();
	ICUTILS_VERIFY(feFirmwareVersionCheck == feFirmwareVersion)(externalFeFpga)(feFirmwareVersionCheck)(feFirmwareVersion).error().msg(errMesg.c_str());
      }

      return feFirmwareVersion;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFeFirmwareId"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * This will read the firmware ID on all the FE FPGA delay chips.
   *
   * It ensures that they are the same before returning the firmware ID.
   */
  u32 Fed9UVmeDevice::getDelayFirmwareId() const throw (Fed9UVmeDeviceException) {
    try {
      //Only read the first FE FPGA and ICUTILS_VERIFY that the other FE FPGAs have the same value.
      u32 delayChipFirmwareVersion( theFed->feCommandDelayChipFirmwareId(1,0) );
      //Ensures that the firmware id is the same for the delay chips on all the FE FPGAs.
      //Starts at delay chip 1, FPGA 0 as delayChipFirmwareVersion is the value for FPGA 0.
      std::ostringstream strExternalZeroFeFpga;
      strExternalZeroFeFpga << static_cast<u16>(Fed9UAddress().setFedFeUnit(0).getExternalFedFeUnit()); //Get external zero FPGA number for error message.
      for (u32 feFpga(0); feFpga < FEUNITS_PER_FED; ++feFpga) {
	for (u32 delayChip(1); delayChip < 3; ++delayChip) {
	  u32 delayChipFirmwareVersionCheck( theFed->feCommandDelayChipFirmwareId(feFpga+1, delayChip) );

	  std::ostringstream strExternalFeFpga;
	  u32 externalFeFpga = Fed9UAddress().setFedFeUnit(feFpga).getExternalFedFeUnit();
	  strExternalFeFpga << externalFeFpga;

	  std::ostringstream strDelayChip;
	  strDelayChip << delayChip;
	  
	  std::ostringstream strFedIdSlot;
	  strFedIdSlot << "FED HARDWARE ID = " << theLocalFedSettings.getFedHardwareId() << " Fed Slot = " << ((u16)theLocalFedSettings.getSlotNumber());
	  string errMesg = strFedIdSlot.str() + "The delay chip firmware id on delay chip number " + strDelayChip.str() + ", FE FPGA " + strExternalFeFpga.str() + " is different from delay chip 0, FE FPGA " + strExternalZeroFeFpga.str();
	  ICUTILS_VERIFY(delayChipFirmwareVersionCheck == delayChipFirmwareVersion)(delayChip)(externalFeFpga)(delayChipFirmwareVersionCheck)(delayChipFirmwareVersion).error().msg(errMesg.c_str());
	}
      }
      return delayChipFirmwareVersion;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFeFirmwareId"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  // JEC 30-05-06
  u32 Fed9UVmeDevice::getFeFakeEventTriggerFifoData(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      // This register is only available for certain Firmware versions
      u32 feFirmwareVersion;
      theFed->feCommandFirmwareIdReg(feFpga.getFirmwareFedFeUnit(), feFirmwareVersion);
      ICUTILS_VERIFY(feFirmwareVersion >= 23100338)(feFirmwareVersion).error().msg("This register is not available for the current FE Firmware version");

      u32 triggerFifo;
      theFed->feCommandFakeEventTriggerFifo(feFpga.getFirmwareFedFeUnit(), triggerFifo);
      return triggerFifo;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFeFakeEventTriggerFifoData"));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Used to create the strip data vectors that are required by the setStripData() method.
   *
   * This method will create vectors containing either the pedestals and valid strip settings
   * or the cluster finding threshold settings. If set threshold is true then the thresholds
   * are produced. If false the pedestals and valid strips are produced. The vectors contain
   * 512 strips of data, which corresponds to two channels of data. Each vector element
   * represents a strip. This method uses the FED logic system so a 1 will enable a strip
   * and a 0 will disable it.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::createStripData(const Fed9UAddress& fedApv, const bool setThreshold, const vector<u16>& pedsOrHighThresh, const vector<u16>& valStripOrLowThresh) throw (Fed9UVmeDeviceException) {
    try {
    //Ensures that the passed vector are of the correct size and that they contain valid values.
    ICUTILS_VERIFY(pedsOrHighThresh.size() == STRIPS_PER_APV && valStripOrLowThresh.size() == STRIPS_PER_APV)(pedsOrHighThresh.size())(valStripOrLowThresh.size()).error().msg("Invalid vector size");

    //When writing the pedestals four APVs worth of strip data must be written. As only one APV is passed into this method
    //the other APVs must be retrieved from theLocalFedSettings. The APVs are then combined into one vector.
    vector<u16> allPedsOrHighThresh(512,0);
    vector<u16> allValStripOrLowThresh(512,0);
    //The strip data is set per RAM on the FED. This represents four APVs of strip data.
    const u32 APVS_PER_RAM = 4;
    const u16 ramApv = fedApv.getFedApv()%4;
    for (u32 i = 0; i < APVS_PER_RAM; i++) {
      Fed9UAddress desAddr;
      desAddr.setFedApv(fedApv.getFedApv() - ramApv + i);
      vector<Fed9UStripDescription> stripDescription( theLocalFedSettings.getFedStrips().getApvStrips(desAddr) );

      /*if (theLocalFedSettings.getApvDisable(desAddr)){
	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	  //Selects the appropiate element number to write the strip data into.
	  u32 vectorElement = (i * STRIPS_PER_APV) + apvStrip_;
	  if (setThreshold) {
	    allPedsOrHighThresh[vectorElement]    = 255;
	    allValStripOrLowThresh[vectorElement] = 255;
	  } else {//end of setThreshold if
	    allPedsOrHighThresh[vectorElement]    =   1023;
	    allValStripOrLowThresh[vectorElement] = false; //note that false here means disable the strip
	  }//end of set threshold else
	}//end of STRIPS_PER_APV for
	
	} else {*/
	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	  //Selects the appropiate element number to write the strip data into.
	  u32 vectorElement = (i * STRIPS_PER_APV) + apvStrip_;
	  
	  if (ramApv != i) {
	    
	    if (setThreshold) {
	      allPedsOrHighThresh[vectorElement]    = stripDescription[apvStrip_].getHighThreshold();
	      allValStripOrLowThresh[vectorElement] = stripDescription[apvStrip_].getLowThreshold();
	    } else {//end of setThreshold if
	      allPedsOrHighThresh[vectorElement]    =   stripDescription[apvStrip_].getPedestal();
	      allValStripOrLowThresh[vectorElement] = !(stripDescription[apvStrip_].getDisable());
	    }//end of set threshold else
	    
	  } else {
	    
	  allPedsOrHighThresh[vectorElement]    = pedsOrHighThresh[apvStrip_];
	  allValStripOrLowThresh[vectorElement] = valStripOrLowThresh[apvStrip_];

	}//end of rampApv != i else
	
	if (fedApv.getFedApv() == 0 && setThreshold) {
	  //cout << "cluster value for strip " << apvStrip_ << " = " << stripDescription[apvStrip_].getLowThreshold() << ", VectorElement = " << vectorElement << endl;
	}
	
      }//end of STRIPS_PER_APV for
      //}
      //note that since we force strip data for disabled APVs in this method, we must also reflect the change in the localfedsettings
	//note again, I have removed this code cause it cannot work here. In stead the description we use to create the FED object has already been changed in the fed supervisor
	//this method of doing things is no longer necessary.
      /*if (theLocalFedSettings.getApvDisable(desAddr)){
	
	vector<Fed9UStripDescription> stripDescription( theLocalFedSettings.getFedStrips().getApvStrips(fedApv) );
	
	for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	  stripDescription[apvStrip_].setPedestal(1023);
	  stripDescription[apvStrip_].setDisable(true);
	  stripDescription[apvStrip_].setLowThreshold(255);
	  stripDescription[apvStrip_].setHighThreshold(255);
	}
	
	theLocalFedSettings.getFedStrips().setApvStrips(fedApv, stripDescription);
	
	}*/

    }//end of APVS_PER_RAM for

    if (setThreshold) {
      //The thresholds do not need to be reordered and so are written straight to the FED.
      setStripData(fedApv, setThreshold, allPedsOrHighThresh, allValStripOrLowThresh);
    } else {
      //This re-orders the elements of the vectors so that the strip data is in the same order as that
      //which is stored in the memory.
      vector<u16> allFedOrderedPeds;
      disOrderingAlgorithm(allPedsOrHighThresh, allFedOrderedPeds);
      vector<u16> allFedOrderedValStrip;
      disOrderingAlgorithm(allValStripOrLowThresh, allFedOrderedValStrip);
      //Writes to the FED.
      setStripData(fedApv, setThreshold, allFedOrderedPeds, allFedOrderedValStrip);
    }
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
   * Writes the strip data to the FED.
   *
   * This method will set either the pedestal and valid strip values or the threshold values
   * for four APVs. Each element of the vectors represents a different strip. The vectors can
   * contain two different sets of data depending on the value of setThreshold. If setThreshold
   * is true then the cluster threshold data is returned. False and the pedestal and valid
   * channel data is returned.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setStripData(const Fed9UAddress& fedApv, const bool setThreshold, const vector<u16>& allPedsOrHighThresh, const vector<u16>& allValStripOrLowThresh ) throw (Fed9UVmeDeviceException) {
    try {
    //Ensures that the passed vector are of the correct size and that they contain valid values.
    const u32 STRIPS_PER_RAM = 4 * STRIPS_PER_APV;
    ICUTILS_VERIFY( allPedsOrHighThresh.size() == STRIPS_PER_RAM && allValStripOrLowThresh.size() == STRIPS_PER_RAM )(allPedsOrHighThresh.size())(allValStripOrLowThresh.size()).error().msg("Invalid vector size");
    u16 fpgaApv_    = fedApv.getFeUnitApv();
    //This is the initial strip that the pedAddress is to start from. The address will start at this strip and then
    //automatically increment upwards each time the feCommandLoadPedData() is called. Only the first 9 bits are
    //affect by the auto increment.
    const u32 initialStrip = 0;
    u32 ramNumber = static_cast<u32>(fpgaApv_ / 4.0);
    vector<u32> addr(1,0);
    addr[0] = (ramNumber<<9) | (setThreshold<<8) | initialStrip;

    timer2.stop();
    timer2Value+=timer2.read();
    timer1.start();
    theFed->feCommandSetPedAddress(fedApv.getFirmwareFedFeUnit(), WRITE, addr, dummyVector);
    timer1.stop();
    timer1Value+=timer1.read();
    timer2.start();
    vector<u32> data(4,0);
    if (blockModeUpload_) data.resize(4*STRIPS_PER_CHANNEL);
    for (u32 apvStrip_ = 0; apvStrip_ < STRIPS_PER_CHANNEL; ++apvStrip_) {
      
      /*      if (setThreshold && fedApv.getFedApv() == 0) {
	cout << "high threshold or ped values: " <<  allPedsOrHighThresh[apvStrip_] 
	     << ", low threshold or val stirp values: " << allValStripOrLowThresh[apvStrip_]
	     << endl;
	     }*/
      if (blockModeUpload_) {
	//For APVs 0 and 1.
	data[apvStrip_*4+0] = allValStripOrLowThresh[apvStrip_];
	data[apvStrip_*4+1] = allPedsOrHighThresh   [apvStrip_];
	//For APVs 2 and 3.
	data[apvStrip_*4+2] = allValStripOrLowThresh[apvStrip_ + STRIPS_PER_CHANNEL];
	data[apvStrip_*4+3] = allPedsOrHighThresh   [apvStrip_ + STRIPS_PER_CHANNEL];
      } else {
	//For APVs 0 and 1.
	data[0] = allValStripOrLowThresh[apvStrip_];
	data[1] = allPedsOrHighThresh   [apvStrip_];
	//For APVs 2 and 3.
	data[2] = allValStripOrLowThresh[apvStrip_ + STRIPS_PER_CHANNEL];
	data[3] = allPedsOrHighThresh   [apvStrip_ + STRIPS_PER_CHANNEL];
	timer2.stop();
	timer2Value+=timer2.read();
	timer1.start();
	theFed->feCommandLoadPedData(fedApv.getFirmwareFedFeUnit(), WRITE, data, dummyVector, setThreshold);
	methodCallCounter+=1;
	timer1.stop();
	timer1Value+=timer1.read();
	timer2.start();
      }
    }
    if (blockModeUpload_) {
      timer2.stop();
      timer2Value+=timer2.read();
      timer1.start();
      theFed->feCommandBlockLoadPedData(fedApv.getFirmwareFedFeUnit(), WRITE, data, dummyVector, setThreshold);
      methodCallCounter+=1;
      timer1.stop();
      timer1Value+=timer1.read();
      timer2.start();
    }
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
   * This will return a 128 element vector that contains the pedestals and disable strip
   * settings on an APV.
   */
  // JEC 4/04/06:  This is not true ... the address is set in the method, same as in setStripData
  // This should only be called from getStripData as it relies on the pedestal address
  // being set before this method is called.

  const Fed9UVmeDevice& Fed9UVmeDevice::getPedsAndDisabledStripData(const Fed9UAddress& fedApv, vector<u16>& pedestals, vector<u16>& validStrips) const throw (Fed9UVmeDeviceException) {
    try {
    //To ensure that the passed vectors are of the correct size.
    ICUTILS_VERIFY(pedestals.size() == STRIPS_PER_APV && validStrips.size() == STRIPS_PER_APV)(pedestals.size())(validStrips.size()).error().msg("Invalid vector size");

    u16 fpgaChannel_ = fedApv.getFeUnitChannel();
    u16 fpgaApv_     = fedApv.getFeUnitApv();
    //Each RAM contains two channels.
    u16 ramChannel_  = fpgaChannel_%2;
    //This is the initial strip that the pedAddress is to start from. The address will start at this strip and then
    //automatically increment upwards. The first nine bits of the pedAddress with automatically be incremented upwards
    //each time the feCommandLoadPedData() is called.
    const u32 initialStrip = 0;
    const u32 ramNumber    = static_cast<u32>(fpgaChannel_/2.0);
    //We are dealing with the pedestals.
    const bool setThreshold = false;
    vector<u32> addr(1,0);
    addr[0] = (ramNumber<<9) | (setThreshold<<8) | initialStrip;
    theFed->feCommandSetPedAddress(fedApv.getFirmwareFedFeUnit(), WRITE, addr, dummyVector);
    //This reads the data back from the FED and organises it into 128 element vectors.
    //This will contain the data that is read back from the FED.
    vector<u32> data;
    //There will only be 128 strips extracted from the returned data. Therefore another loop counter
    //variable needs to be created to count the number of APV strips extracted and placed into the output vector.
    u16 apvStrip_ = 0;
    for (u16 channelStrip_ = 0; channelStrip_ < STRIPS_PER_CHANNEL; ++channelStrip_) {
      //This ensures that a null vector is passed to the FED. This is because Fed9UVmeBase
      //uses the push back method to insert elements. This would result in the new values
      //always being placed at the end of the vector unless it is cleared first.
      data.clear();
      theFed->feCommandLoadPedData(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, data, setThreshold);
      //When settings the Pedestals the data read back from the FED contains two interleaved APVS.
      //In order to read back the correct APV data then when the APV number is even then the even
      //strip numbers should be read back. When the APV number is odd then only the odd strip numbers
      //read back. This if statement selects those cases. The else does nothing.
      bool fpgaApvOdd      = fpgaApv_%2;
      bool channelStripOdd = channelStrip_%2;
      if ( (fpgaApvOdd && channelStripOdd) || (!fpgaApvOdd && !channelStripOdd) ) {
	//If both the APV No. and the strip No. are both even or both odd the select then read out the data.
	u32 vectorElement;
	if (ramChannel_ == 0) {
	  vectorElement = 0;
	} else {
	  vectorElement = 2;
	}
	validStrips[apvStrip_] = data[vectorElement];
	pedestals[apvStrip_]   = data[vectorElement + 1];
	++apvStrip_;
      } else {
	//APV No. is odd and APV strip is even or APV No. is even and APV strip is odd do nothing
      }
    }
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
   * This will return a 128 element vector that contains the thresholds on an APV.
   */
  // JEC 4/04/06:  This is not true ... address is set in the method, same as in setStripData
  // This should only be called from getStripData as it relies on the pedestal address
  // being set before this method is called.
  const Fed9UVmeDevice& Fed9UVmeDevice::getClusterData(const Fed9UAddress& fedApv, vector<u16>& highThresholds, vector<u16>& lowThresholds) const throw (Fed9UVmeDeviceException) {
    try {
    //To ensure that the passed vectors are of the correct size.
    ICUTILS_VERIFY(highThresholds.size() == STRIPS_PER_APV && lowThresholds.size() == STRIPS_PER_APV)(highThresholds.size())(lowThresholds.size()).error().msg("Invalid vector size");

    u16 fpgaChannel_ = fedApv.getFeUnitChannel();
    u16 channelApv_  = fedApv.getChannelApv();
    //Each RAM contains two channels.
    u16 ramChannel_  = fpgaChannel_%2;
    //This is the initial strip that the pedAddress is to start from. The address will start at this strip and then
    //automatically increment upwards. The first nine bits of the pedAddress with automatically be incremented upwards
    //each time the feCommandLoadPedData() is called.
    const u32 initialStrip = channelApv_ * STRIPS_PER_APV;
    const u32 ramNumber    = static_cast<u32>(fpgaChannel_/2.0);
    //We are dealing with the cluster thresholds.
    const bool setThreshold = true;
    vector<u32> addr(1,0);
    addr[0] = (ramNumber<<9) | (setThreshold<<8) | initialStrip;
    theFed->feCommandSetPedAddress(fedApv.getFirmwareFedFeUnit(), WRITE, addr, dummyVector);
    //This reads the data back from the FED and organises it into 128 element vectors.
    //This will contain the data that is read back from the FED.
    vector<u32> data;
    for (u16 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
      //This ensures that a null vector is passed to the FED. This is because Fed9UVmeBase
      //uses the push back method to insert elements. This would result in the new values
      //always being placed at the end of the vector unless it is cleared first.
      data.clear();
      theFed->feCommandLoadPedData(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, data, setThreshold);
      //Selects the appropiate element number to read the strip data from.
      u32 vectorElement;
      if (ramChannel_ == 0) {
	vectorElement = 0;
      } else {
	vectorElement = 2;
      }
      lowThresholds[apvStrip_]  = data[vectorElement];
      highThresholds[apvStrip_] = data[vectorElement + 1];
    }
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
   * This will take a vector that contains four APVs of strips (512 elements) with
   * each element corresponding to a strip. The vector is the reordered so that the
   * elements are ordered to match the ordering of the strips as they are sent
   * from the APV to the FED.
   *
   * The input ordering is the same as that which is kept by the description and the
   * output ordering is the same as that coming into the FED. This method was written by
   * Mattew Pearson, RAL, 12/2003 and modified by Gareth Rogers, RAL, 01/2004.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::disOrderingAlgorithm(const vector<u16>& stripOrderedInput, vector<u16>& apvOrderedOutput) throw (Fed9UVmeDeviceException) {
    try {
    const u32 STRIPS_PER_RAM = 4 * STRIPS_PER_APV;
    //Check input vector is the correct size. It should contain four APVs of strip data.
    ICUTILS_VERIFY( stripOrderedInput.size() == STRIPS_PER_RAM )(stripOrderedInput.size()).error().msg("The number of elements in stripOrderedInput is an invalid number.");
    //To ensure that an empty vector is passed to this command and then resize to hold new data.
    apvOrderedOutput.clear();
    apvOrderedOutput.resize(STRIPS_PER_RAM);
    //Create an iterator that is pointing to the beginning of the output vector.
    vector<u16>::iterator outputIterator = apvOrderedOutput.begin();
    //Calculate the channel mapping and make new vector with dis-ordered strips.
    //reorderedPlace contains the location of the strip in the FED ordering.
    //newPlace calculates it in a set of four APVS.
    u16 reorderedPlace = 0;
    u16 newPlace = 0;
    const u16 CHANNELS_PER_RAM = 2;
    for (u16 ramChannel = 0; ramChannel < CHANNELS_PER_RAM; ++ramChannel) {
      for (u16 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {

	reorderedPlace = (32 * (apvStrip%4)) +
	  (8 * static_cast<u16>(static_cast<float>(apvStrip)/4.0)) -
	  (31 * static_cast<u16>(static_cast<float>(apvStrip)/16.0));

	for (u16 channelApv = 0; channelApv < APVS_PER_CHANNEL; ++channelApv) {

	  //The data for the two APVs on a channel must also be interleaved.
	  newPlace = ( (STRIPS_PER_CHANNEL * ramChannel) + (STRIPS_PER_APV * channelApv) ) + reorderedPlace ;
	  //The output is a combination of four APVs of data (512 strips).
	  //Assign to the data at the end of the iterator and then increment the address.
	  *outputIterator = stripOrderedInput[newPlace];
	  ++outputIterator;
	}//end of channelApv for
      }//end of apvStrip for
    }//end of ramChannel for
    return *this;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }//end of member function


  /**
   * This will take a vector that contains one APV of strips (128 elements) where
   * the element ordering matches that sent into the FED from an APV. It will
   * reorder the vector so that it is returned with its elements in the order that
   * is output by the FED after it has performed re-ordering.
   *
   * This method was written by Matthew Pearson, RAL, 12/2003.
   */
  const Fed9UVmeDevice& Fed9UVmeDevice::reOrderingAlgorithm(const vector<u16>& apvOrderedInput, vector<u16>& stripOrderedOutput) const throw (Fed9UVmeDeviceException) {
    try {
    //Check input vector is the correct size.
    ICUTILS_VERIFY(apvOrderedInput.size() == STRIPS_PER_APV)(apvOrderedInput.size()).error().msg("The number of elements in apvOrderedInput is invalid.");
    //Clears the vector and then creates enough space to hold the output data.
    stripOrderedOutput.clear();
    stripOrderedOutput.resize(STRIPS_PER_APV);
    //Calculate the FED re-ordering and put into output vector.
    u16 newPlace = 0;
    for (u16 apvStrip = 0; apvStrip < STRIPS_PER_APV; apvStrip++) {
      newPlace = 4*((static_cast<u16>((static_cast<float>(apvStrip)/8.0)))%4) +
	static_cast<u16>(static_cast<float>(apvStrip)/32.0) +
	16*(apvStrip%8);
      stripOrderedOutput[apvStrip] = apvOrderedInput[newPlace];
    }
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
   * This sets the number of strips that should be used in the median calculation for an APV.
   *
   * The vector input contains 128 elements which represent the state of an APV's worth of strips. A
   * 1 represents disabled and a 0 enabled. Each element contains the state of a strip.
   * It must always be the same as the common-mode calculation values.
   */
  Fed9UVmeDevice& Fed9UVmeDevice::setNumberValidStrips(const Fed9UAddress& fedApv, const vector<bool>& disabledStrips) throw (Fed9UVmeDeviceException) {
    try {
    ICUTILS_VERIFY(disabledStrips.size() == STRIPS_PER_APV)(disabledStrips.size()).error().msg("Valid channel vector argument is of the wrong size.");
    u16 fedApv_  = fedApv.getFedApv();
    u16 fpgaApv_ = fedApv.getFeUnitApv();
    vector<u32> validFpga(CHANNELS_PER_FEUNIT,0);

    Fed9UAddress addr;
    for (u16 i = 0; i < APVS_PER_FEUNIT; ++i) {
      addr.setFedApv( (fedApv_ - fpgaApv_) + i );
      u16 totalCount = 0;
      for (u16 apvStrip_ = 0; apvStrip_ < STRIPS_PER_APV; ++apvStrip_) {
	addr.setApvStrip(apvStrip_);
	//When the FE FPGA APV number is equal to the loop number then the user passed argument is read
	//if this is a true then the counter is incremented.
	if (  (i == fpgaApv_) && !disabledStrips[apvStrip_] ) totalCount++;
	//For all other cases the description is read and if that is false the counter is incremented.
	if (   i != fpgaApv_  && !( theLocalFedSettings.getFedStrips().getStrip(addr).getDisable()) ) totalCount++;
      }
      //The number of valid strips is created using this command. The channel APV 0 is the most significant eight bits
      //and the channel APV 1 is the least significant eight bits.
      validFpga[addr.getFeUnitChannel()] |= totalCount << ( 8 * !(addr.getChannelApv()) );
    }
    theFed->feCommandSetNumberValid(fedApv.getFirmwareFedFeUnit(), WRITE, validFpga, dummyVector);
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
   * This gets the number of strips that should be used in the median calculation.
   *
   * The return value is the number strips that are enabled on an APV. It must always be the same as
   * the common-mode calculation values.
   */
  u16 Fed9UVmeDevice::getNumberValidStrips(const Fed9UAddress& fedApv) const throw (Fed9UVmeDeviceException) {
    try {
    u16 numberValidStrips;
    vector<u32> validFpga;
    u16 fpgaChannel_ = fedApv.getFeUnitChannel();
    u16 channelApv_  = fedApv.getChannelApv();

    theFed->feCommandSetNumberValid(fedApv.getFirmwareFedFeUnit(), READ, dummyVector, validFpga);
    //The number of valid strips for an APV is retrieved using this command. The channel APV 0 is the most
    //significant eight bits and the channel APV 1 is the least significant eight bits.
    numberValidStrips = (validFpga[fpgaChannel_] >> (8 * !channelApv_)) & 0xff;
    return numberValidStrips;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  /**
   * Loads one Fake Event into the specified APV RAM.
   *
   * 
   */
#ifdef EVENT_STREAMLINE
  Fed9UVmeDevice & Fed9UVmeDevice::setFakeEventFrame(Fed9UAddress & fedChannel, Fed9UEventStreamLine * fakeEvent, bool blockMode) throw (Fed9UVmeDeviceException)
#else
    Fed9UVmeDevice & Fed9UVmeDevice::setFakeEventFrame(Fed9UAddress & fedChannel, Fed9UEvent * fakeEvent, bool blockMode) throw (Fed9UVmeDeviceException)
#endif
  {
    try {
      // first we must set the address of the RAM to which we are loading the Fake Event
      vector<u32> arguments(1,0);
      vector<u32> tempRead(1,0);
      vector<u32> tempRead2(1,0);
      u32 blockindex = 0;
      // calculate the address code to write
      arguments[0] = 
	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
	| ( 0 & 0x1c00 ) // since these 3 bits are reserved and may be used later
	| ( ( ( (fedChannel.getFeUnitChannel() % 2) & 0x1 ) << 9) & 0x200)  // sets the lower or upper channel from the pair
	| ( 0 & 0x1FF ); // since we start from address 0 and use auto increment to load the data

      // if we are using real data then we write all FEs separately otherwise we write them in broadcast mode
      u16 feunit;
      if ( fakeEvent ) {
	//For the moment always write to all channels even if it is the same data so we can check the header status bits.
	//\TODO - Create a setting to do this.
	feunit = fedChannel.getFirmwareFedFeUnit();
// 	feunit = ( ( fakeEvent->getDataMode() == FED9U_EVENT_MODE_REAL ) ? fedChannel.getFirmwareFedFeUnit() : 15 );
      } 
      else {
        // <NAC date="25/09/2007"> when a NULL pointer for event is passed, encode channel number and ID in data so disable random noise
	//feunit = 15;
        theLocalFedSettings.setFakeEventRandomMask(fedChannel,0);
        setFakeEventSettings(fedChannel,NULL);
        feunit = fedChannel.getFirmwareFedFeUnit();
        // </NAC>
      }

      timer2.stop();
      timer2Value+=timer2.read();
      timer1.start();
      theFed->feCommandSetFakeEventAddress(feunit , WRITE, arguments, dummyVector );
      timer1.stop();
      timer1Value+=timer1.read();
      timer2.start();

      //start by setting the data values for the apv header
      // load 4 of these values
      //used to set tick hights.
      u16 height(0);
      //Used to access the FED APVs.
      //Initialise from FED channel and set the APV later.
      Fed9UAddress fedApv(fedChannel);
      for (u32 i = 0 ; i < 4 ; i++) {
	if (0 == i%2) {
	  //Set first channel APV.
	  fedApv.setChannelApv(0);
	  //Check settings and appropriate height.
	  if (theLocalFedSettings.getApvFakeEventDisable(fedApv) ) {
	    height = 0;
// 	    std::cout << "Disabled APV. Setting tick height to " << height 
// 		 << " for FED APV " << static_cast<u16>(fedApv.getFedApv() )
// 		 << " for FED channel " << static_cast<u16>(fedApv.getFedChannel() )
// 		 << std::endl;
	  } else {
	    //std::cout << "Enabled APV. Setting tick height to " << height << " for FED APV " << static_cast<u16>(fedApv.getFedApv() ) << std::endl;
	    height = 0x3aa;
	  }
	} else {
	  //Set second channel APV.
	  fedApv.setChannelApv(1);
	  //Check settings and appropriate height.
	  if (theLocalFedSettings.getApvFakeEventDisable(fedApv) ) {
	    height = 0;
// 	    std::cout << "Disabled APV. Setting tick height to " << height
// 		 << " for FED APV " << static_cast<u16>(fedApv.getFedApv() )
// 		 << " for FED channel " << static_cast<u16>(fedApv.getFedChannel() )
// 		 << std::endl;
	  } else {
	    height = 0x3aa;
	    //std::cout << "Enabled APV. Setting tick height to " << height << " for FED APV " << static_cast<u16>(fedApv.getFedApv() ) << std::endl;
	  }
	}

	if (blockMode) {
	  arguments.resize(278);
	  arguments[i]= height;//0x3aa;
	  blockindex++;
	  /*if ( i == 0 )
	    arguments[0] = 0x3aa;
	  else 
	    arguments.push_back( 0x3aa );
	  */
	}
	else {
	  arguments[0] = height;//0x3aa; // note you must set this for each one , since we left shift the vector in the load
	  timer2.stop();
	  timer2Value+=timer2.read();
	  timer1.start();
	  theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
	  methodCallCounter+=1;
	  timer1.stop();
	  timer1Value+=timer1.read();
	  timer2.start();
	}
      }

      // load the pileline address
      // if we are loading a default ramp, we have no Fed9UEvent we must invent an address
      u16 apvPipelineAddress, errorBit1, errorBit2;
      //std::cout << "C" << std::endl;
      //std::cout << "fakeEvent pointer = " << fakeEvent << std::endl;
      if ( fakeEvent && (fakeEvent->getDataMode() == FED9U_EVENT_MODE_REAL  ) ) {
	//	u32 tmpfeunit, tmpfeunitchannel;
#ifdef EVENT_STREAMLINE
	apvPipelineAddress = fakeEvent->getApvEmulatorAddress(); // assume the same address for each apv.
	u8 tmptestStatus = fakeEvent->getFeUnit(fedChannel.getExternalFedFeUnit()).getChannelStatus(fedChannel.getExternalFeUnitChannel());
	errorBit1 = ((( tmptestStatus & 0x4) == 0x4 ) ? 0x3aa : 0x0 ) ;
	errorBit2 = ((( tmptestStatus & 0x1) == 0x1 ) ? 0x3aa : 0x0 ) ; // APV2 error bit
#else
	apvPipelineAddress = fakeEvent->getSpecialApvEmulatorAddress(); // assume the same address for each apv.
	//	tmpfeunit = fedChannel.getFedFeUnit();
	//	tmpfeunitchannel = fedChannel.getFeUnitChannel();
	Fed9UEventChannel tmpechannel = fakeEvent->channel(fedChannel.getFedFeUnit(),fedChannel.getFeUnitChannel());
	u32 tmptestStatus = tmpechannel.getStatus();
	errorBit1 = ((( tmptestStatus & 0x4) == 0x4 ) ? 0x3aa : 0x0 ) ; // APV1 error bit
	errorBit2 = ((( fakeEvent->channel(fedChannel.getFedFeUnit(),fedChannel.getFeUnitChannel()).getStatus() & 0x1) == 0x1 ) ? 0x3aa : 0x0 ) ; // APV2 error bit
#endif
      }
      else {
	apvPipelineAddress = 0xed;
	errorBit1 = 0x3aa;
	errorBit2 = 0x3aa;
      }
      //std::cout << "Error bits 1 and 2 are: " << std::hex << errorBit1 << ", " << errorBit2 << std::dec <<std::endl;
      //std::cout << "D" << std::endl;
      u32 argVal;
      for (u32 i = 0 ; i < 16 ; i++) {
	argVal = ( ( ( apvPipelineAddress >> (7 - (i/2)) ) & 0x1 ) ? 0x3aa : 0x0 ); // i/2 is used so we write the same value twice, one for each APV in the channel

	if (blockMode) {
	  //arguments.push_back(argVal);
	  arguments[blockindex]=argVal;
	  blockindex++;
	}
	else {
	  // load twice for each address bit for both apvs.
	  arguments[0] = argVal;
	  timer2.stop();
	  timer2Value+=timer2.read();
	  timer1.start();
	  theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
	  methodCallCounter+=1;
	  timer1.stop();
	  timer1Value+=timer1.read();
	  timer2.start();
	}
      }
      //std::cout << "E" << std::endl;
      // now load the error bits
      if (blockMode) {
	//arguments.push_back(errorBit1);
	//arguments.push_back(errorBit2);
	arguments[blockindex]=errorBit1;
	blockindex++;
	arguments[blockindex]=errorBit2;
	blockindex++;
	//std::cout << "F" << std::endl;
      }
      else {
	arguments[0] = errorBit1;
	timer2.stop();
	timer2Value+=timer2.read();
	timer1.start();
	theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
	arguments[0] = errorBit2;
	theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
	methodCallCounter+=2;
	timer1.stop();
	timer1Value+=timer1.read();
	timer2.start();
      }
      
      //now load the data
      
      for (u32 i = 0 ; i < 2*STRIPS_PER_APV ; i++) {
	
	// now set up the data to load
	if ( fakeEvent ) { 
#ifdef EVENT_STREAMLINE
	  argVal = fakeEvent->getFeUnit(fedChannel.getExternalFedFeUnit()).getSample(static_cast<u32>(fedChannel.getExternalFeUnitChannel()),i) & 0x3ff;
#else
	  argVal = fakeEvent->channel(fedChannel.getFedFeUnit(),fedChannel.getFeUnitChannel()).sample(i) & 0x3FF;
#endif
	  // cout << "fake event data for strip " << i << " = " << argVal << endl;
	}
	else {
          // <NAC date="25/09/2007"> store ID and channel number in event file
          /*switch (i) {
            case 0:
              argVal = (theLocalFedSettings.getFedId() & 0x000F) | 0x80;
              break;
            case 1:
              argVal = ((theLocalFedSettings.getFedId() & 0x00F0) >> 4) | 0x80;
              break;
            case 2:
              argVal = ((theLocalFedSettings.getFedId() & 0x0F00) >> 8) | 0x80;
              break;
            case 3:
              argVal = ((theLocalFedSettings.getFedId() & 0xF000) >> 12) | 0x80;
              break;
            case 4:
            case 5:
              argVal = 0x80 | fedChannel.getFedChannel();
              break;
            default:
              argVal = 0x0;
              break;
	      }*/
	  argVal = ( (0x3ff / 256 ) *  i + 250 ) & 0x3ff;
          // </NAC>
	  // replace with this line if debugging pedestal subtraction ordering
	  
	  //thisargVal = theLocalFedSettings.getFedStrips().getApvStrips(fedChannel.setChannelApv(i/128))[i%128].getPedestal();
	}

	if (blockMode) {
	  arguments[blockindex]=argVal;
	  blockindex++;
	  //      std::cout << "G" << std::endl;
	}
	else {
	  arguments[0] = argVal;
	  // Now we can load the Frame data
	  timer2.stop();
	  timer2Value+=timer2.read();
	  timer1.start();
	  theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
	  methodCallCounter+=1;
	  timer1.stop();
	  timer1Value+=timer1.read();
	  timer2.start();
	}
      }
      if (blockMode) { 
	// dump the block
// 	if (0 == fedChannel.getFedChannel() ) {//do it for just one channel to reduce print out.
// 	for (int i=0 ; i < 278 ; i++) {
// 	  std::cout << " fake event value " << i << " = " << arguments[i] << std::endl; 
// 	  }
// 	}
	timer2.stop();
	timer2Value+=timer2.read();
	timer1.start();
	//std::cout << "G" << std::endl;
	theFed->feCommandBlockLoadFakeEventData( feunit, WRITE, arguments, dummyVector ); // load the data in block mode.. only once!
	//std::cout << "H" << std::endl;
	methodCallCounter+=1;
	timer1.stop();
	timer1Value+=timer1.read();
	timer2.start();
	//std::cout << "Loaded the fake event in block mode!!!" << std::endl;
      }


      //Just call getFakeEventFrame that has all the debug print out we need.
// 	if (0 == fedChannel.getFedChannel() ) {//do it for just one channel to reduce print out.
// 	  std::cout << "Reading event buffer:" << std::endl;
// 	  getFakeEventFrame(fedChannel);
// 	}
//       if (1 == fedChannel.getExternalFedChannel() ) {
// 	const vector<u32> fakeEventBuffer( getFakeEventFrame(fedChannel) );
// 	u32 nLoops(0);
// 	std::cout << "Loaded fake event frame. Event size: " << fakeEventBuffer.size() 
// 	     << "Read buffers to see what we have written:\n" << std::hex;
// 	for (vector<u32>::const_iterator it(fakeEventBuffer.begin() ); it != fakeEventBuffer.end(); ++it) {
// 	  std::cout << *it << (nLoops%16==0 ? "\n":"\t");
// 	  ++nLoops;
// 	}
//       std::cout << std::dec << std::endl;
//       }

    } catch ( std::exception & e ) {
       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch ( ... ) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
    return *this;
  }




  vector<u32> Fed9UVmeDevice::getFakeEventFrame(Fed9UAddress & fedChannel) throw (Fed9UVmeDeviceException) {
     try {
       vector<u32> frameData;
       
       // first we must set the address of the RAM from which we want to retrieve the fake event
       // first we must set the address of the RAM from which we are downloading the Fake Event

       vector<u32> arguments(1,0);
       // calculate the address code to write
       arguments[0] = 
	 ( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
	 | ( 0 & 0x1c00 ) // since these 3 bits are reserved and may be used later
	 | ( ( ( (fedChannel.getFeUnitChannel() % 2) & 0x1 ) << 9) & 0x200)  // sets the lower or upper channel from the pair
	 | ( 0 & 0x1FF ); // since we start from address 0 and use auto increment to load the data
       
       // set the feUnit
       u16 feunit  = fedChannel.getFirmwareFedFeUnit(); 
       
       theFed->feCommandSetFakeEventAddress(feunit , WRITE, arguments, dummyVector );
       
       // now loop over the whole apv frame 280 values and put into the vector
       for (u32 i = 0 ; i < ( ( (2 * STRIPS_PER_APV ) + 24 ) - 2 ) ; i++) { // we read out 278 samples, since the firmware treats the first pair of 1s as a tick
	 // Now we can read the Frame data
	 theFed->feCommandLoadFakeEventData( feunit, READ, dummyVector, frameData );
	 //	 std::cout << "FakeEvent vector size = " << frameData.size() << " FakeEvent Data = " << frameData[0] << std::endl;
       }
       return frameData; 
     } catch ( std::exception & e ) {
       RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
     } catch ( ... ) {
       THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
     }
     
  }

  u16 Fed9UVmeDevice::getFakeEventAddress(Fed9UAddress & fedChannel) throw (Fed9UVmeDeviceException) {
    try {
      vector<u32> dummyVector, fakeEventAddress;
      
      u8 feunit = fedChannel.getExternalFedFeUnit();
      theFed->feCommandSetFakeEventAddress( feunit, READ, dummyVector, fakeEventAddress );
      
      return (fakeEventAddress[0] & 0xFFFF);
      
    } catch ( std::exception & e ) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch ( ... ) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  

    /**
     * Set the settings for the Fake Events
     *
     * Settings include tick level, Frame offset DCM low phase, DCM HiPhase, 
     * ( Frame Interval and Frame Repeat not yet implemented )
     */
#ifdef EVENT_STREAMLINE
  Fed9UVmeDevice & Fed9UVmeDevice::setFakeEventSettings(Fed9UAddress & fedChannel, Fed9UEventStreamLine * fakeEvent) throw (Fed9UVmeDeviceException) 
#else
    Fed9UVmeDevice & Fed9UVmeDevice::setFakeEventSettings(Fed9UAddress & fedChannel, Fed9UEvent * fakeEvent) throw (Fed9UVmeDeviceException) 
#endif
    {
      // these settings are loaded on a per channel pair basis... so we need only upload for each pair of channels
      // TODO we must also add the frame offset and DCM phase shifts to the description and here... can do this later
      // first we must set the address of the register to which we are loading the settings
      vector<u32> arguments(1,0);
      vector<u32> readArgs(1,321);
 
      // if we are using real data then we write all fes separately otherwise we write them in broadcast mode
      u16 feunit;
      if ( fakeEvent ) {
       feunit = ( ( fakeEvent->getDataMode() == FED9U_EVENT_MODE_REAL ) ? fedChannel.getFirmwareFedFeUnit() : 15 ); 
      }
      else {
	feunit = 15;
      }
     
      // first set the tick level!
      arguments[0] = 
	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
	| 0x1ffb; // code for Tick Level
      
      theFed->feCommandSetFakeEventAddress( feunit, WRITE, arguments, dummyVector );
      arguments[0] = 0x3aa ; // set the tick level to the maximum for the hell of it.
      theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
     

      // debug readback of tick level
      //      arguments[0] = 
      //	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
      //	| 0x1ffb; // code for Tick Level
      
      //theFed->feCommandSetFakeEventAddress( 1, WRITE, arguments, dummyVector );
      //theFed->feCommandLoadFakeEventData( 1, READ, arguments, readArgs );
      
      //std::cout <<"read back args for tick level are " << std::hex << readArgs[0] <<std::dec << std::endl;
 
      
      // TO DO fix the delay in firmware cause at the moment it causes the first trigger to be missed!! OSMAN (7/12/05)
      // now set the trigger delay
      // arguments[0] = 
      //	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
      //	| 0x1ffa; // code for Delay trigger offset
      
      //theFed->feCommandSetFakeEventAddress( feunit, WRITE, arguments, dummyVector );
      //arguments[0] = 0x3aa & theLocalFedSettings.getFakeEventTriggerDelay() ; // set the tick level to the maximum for the hell of it.
      //theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );


      // Now we set the seed for the Pseudo Random Generator, if we set this to 0 then no values are added to the fake event
      arguments[0] = 
	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
	| 0x1ffe; // code for the pseudo random generator seed
      //std::cout << "arguemtns for generator seed are " << std::hex << arguments[0] <<std::dec<< std::endl;
      theFed->feCommandSetFakeEventAddress( feunit, WRITE, arguments, dummyVector );
      arguments[0] = theLocalFedSettings.getFakeEventRandomSeed(fedChannel);
      //std::cout << "random number seed for channel pair'" << fedChannel.getFedChannelPair() << " = "<< arguments[0] << std::endl;
      theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );
      
      //arguments[0] = 
      //	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
      //	| 0x1ffe; // code for the pseudo random generator seed
      
      
      //std::cout <<"read back args are " << std::hex << readArgs[0] << std::dec << std::endl;
      
      //theFed->feCommandSetFakeEventAddress( 1, WRITE, arguments, dummyVector );
      //theFed->feCommandLoadFakeEventData( 1, READ, arguments, readArgs );
      
      //std::cout <<"read back args are " << std::hex << readArgs[0] << " readArgs length = " <<std::dec << readArgs.size() << std::endl;
      
      
      // Now we set the seed mask for the Pseudo Random Generator, if we set this to 0 then no values are added to the fake event
      arguments[0] = 
	( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
	| 0x1fff; // code for the pseudo random generator seed
      //std::cout << "arguemtns for generator seed are " << std::hex << arguments[0] <<std::dec<< std::endl;
      theFed->feCommandSetFakeEventAddress( feunit, WRITE, arguments, dummyVector );
      arguments[0] =  theLocalFedSettings.getFakeEventRandomMask(fedChannel);
      theFed->feCommandLoadFakeEventData( feunit, WRITE, arguments, dummyVector );



      //       arguments[0] = 
      // ( ( ( fedChannel.getFeUnitChannel() / 2 ) & 0x07 ) << 13 ) // selects 1 of the 6 pairs of channels
      //	| 0x1fff; // code for the pseudo random generator seed
 
      //theFed->feCommandSetFakeEventAddress( 1, WRITE, arguments, dummyVector );
      //theFed->feCommandLoadFakeEventData( 1, READ, arguments, readArgs );
      //std::cout <<"read back args for random mask are " << std::hex << readArgs[0] << std::dec << std::endl;


      return *this;
    }

 
  /**
   * Loads all Fake Events into the FED for all Fake Event RAMs
   * for a specific Event passed in by reference
   * 
   */
#ifdef EVENT_STREAMLINE
  Fed9UVmeDevice & Fed9UVmeDevice::setAllFakeEvents(Fed9UEventStreamLine * fakeEvent, bool blockUpload) throw (Fed9UVmeDeviceException)
#else
    Fed9UVmeDevice & Fed9UVmeDevice::setAllFakeEvents(Fed9UEvent * fakeEvent, bool blockUpload) throw (Fed9UVmeDeviceException)
#endif
  {
    Fed9UAddress addr;
    //std::cout << "setting fake events and the fakeEvent pointer is " << fakeEvent <<  std::endl;
    StopWatch timer(0);
    timer.start();
    try {
      for (u32 i =0 ; i < CHANNELS_PER_FED ; i++){
	// If we are using the single apv frame file or loading a ramp with no file we must only do things 12 times and broadcast to all front ends when loading event data
	// however, we still load settings for all channels in order to collect the correct random number seeds and masks
	addr.setFedChannel(i);

	//TODO - Create a setting for this!
// 	if  ( ( !fakeEvent && i < CHANNELS_PER_FEUNIT ) || (fakeEvent && ( (fakeEvent->getDataMode() == FED9U_EVENT_MODE_REAL) || i < CHANNELS_PER_FEUNIT )) )
// 	  {
	    //std::cout << "setting fake event for channel " << i << std::endl;
	    setFakeEventFrame(addr,fakeEvent,blockUpload); // set the block mode upload in the last parameter
	    //std::cout << "set fake event for channel " << i << std::endl;

	    
	    if (i % 2) // only set this for each pair of channels
	      setFakeEventSettings(addr,fakeEvent); // TODO should probably only write this once for each pair of channels using broadcast to all FE FPGAs rather than doing each one separately!!!
// 	  }
// 	else 
// 	  break;
	
	//char temp;
	//std::cin >> temp;
      }
      timer.stop();
      //std::cout << "Time taken to upload fake Event = " << timer.read() << "us" << std::endl;
      // std::cout << "fakeEvent Pointer at end of setAllFakeEvents = " << std::hex << fakeEvent << std::dec << std::endl;
    } catch ( std::exception & e ) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch ( ... ) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
    return *this;
  }

#ifdef EVENT_STREAMLINE
  Fed9UEventStreamLine * Fed9UVmeDevice::getAllFakeEvents() throw (Fed9UVmeDeviceException)
#else    
    Fed9UEvent * Fed9UVmeDevice::getAllFakeEvents() throw (Fed9UVmeDeviceException)
#endif
  {
    Fed9UAddress addr;
    std::stringstream eventStream;
    try {
      vector<u32> eventVector;
      eventStream << 50 << '\n';
      for (u32 i =0 ; i < CHANNELS_PER_FED ; i++){
	addr.setFedChannel(i);
	eventVector = getFakeEventFrame(addr);      
	for ( u32 j=0; j <( ( (2 * STRIPS_PER_APV ) + 24 ) - 2 ) ; j++ ) {
	  eventStream << std::dec << eventVector[j] << '\n';
	}
      }
    } catch ( std::exception & e ) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    } catch ( ... ) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
#ifdef EVENT_STREAMLINE
    return new Fed9UEventStreamLine(eventStream);
#else
    return new Fed9UEvent(eventStream);
#endif
  }

  //<JEC date=16/03/07>
  Fed9UVmeDevice & Fed9UVmeDevice::setFEFrameCapturePrescale(u16 prescale) throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(theLocalFedSettings.getFeFirmwareVersion() >= 0x2310033F)(theLocalFedSettings.getFeFirmwareVersion()).error().msg("FE scale down trigger register not implemented in this firmware version !");
      // As the prescale must be set for all FE FPGAs the broadcast value is used for the fpga number
      const u16 feFpga = 14;
      vector<u32> prescaleVector(1,prescale);
      // whether the value is the actual prescale factor or the number of events to skip is handled in the higher-level method
      theFed->feCommandFrameCapturePrescale(feFpga+1, WRITE, prescaleVector, dummyVector);
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::setFEFrameCapturePrescale."));
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
  u16 Fed9UVmeDevice::getFEFrameCapturePrescale(const Fed9UAddress& feFpga) const throw (Fed9UVmeDeviceException) {
    try {
      ICUTILS_VERIFY(theLocalFedSettings.getFeFirmwareVersion() >= 0x2310033F)(theLocalFedSettings.getFeFirmwareVersion()).error().msg("FE scale down trigger register not implemented in this firmware version !");
      vector<u32> Prescale;
      theFed->feCommandFrameCapturePrescale(feFpga.getFirmwareFedFeUnit(), READ, dummyVector, Prescale);
      // we want to return the prescale factor, not the number of events that should be skipped
      u16 prescale = Prescale[0] + 1;
      return prescale;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "error in Fed9UVmeDevice::getFEFrameCapturePrescale."));
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }
  //</JEC>

  // <JEC date=03-05-06>
  void Fed9UVmeDevice::dumpFeRegisters(unsigned short selectedFpga, ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {
      vector<Fed9U::u32> dummyVector, argumentVector;

      // If looking at an individual FE-FPGA
      if (selectedFpga <= 8) {
	os << std::endl;
	os << "Dumping all read-write FE registers for FE-FPGA " << selectedFpga << " ..." << std::endl;
	os << std::endl;

	vector< vector<Fed9U::u32> > opArguments;
	vector<string> registerNames;
	vector <string> niceOutput;
	vector<bool> showDeci;

	string sop;

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	theFed->feCommandLoadTick(selectedFpga,true,dummyVector,argumentVector);
	ostringstream op2;
	if (argumentVector[0] == 0x0) {
	  op2 << "All APVs DISABLED,";
	} else {
	  for (u32 loop = 0; loop < 24; loop++) {
	    if (!static_cast<bool>((argumentVector[0] >> (23 - loop)) & 0x3)) {
	      u32 fibre = (loop/2)+1;
	      u32 apv = loop % 2;
	      op2 << "DISABLED APVs: " << "Fibre " << fibre << " APV " << apv << ",";
	    }
	  }
	}
	sop = op2.str();
	if (!sop.empty()) {
	  int lastChar = sop.size()-1;
	  sop.erase(lastChar,1);
	} else {
	  sop = "All APVs ENABLED";
	}
	opArguments.push_back(argumentVector);
	registerNames.push_back("Load tick register (designator = 2)");
	niceOutput.push_back(sop);
	showDeci.push_back(false);

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	argumentVector.assign(1,0);
	theFed->feCommandFirmwareIdReg(selectedFpga,argumentVector[0]);
	opArguments.push_back(argumentVector);
	registerNames.push_back("Firmware ID register (designator = 3)");
	niceOutput.push_back("");
	showDeci.push_back(false);

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	sop.clear();
	theFed->feCommandMedianEnable(selectedFpga,true,dummyVector,argumentVector);
	if (argumentVector[0] == 1) {sop = "Medians will be calculated";}
	else if (argumentVector[0] == 0) {sop = "Medians will not be calculated";}
	else {sop = "Value NOT RECOGNISED";}

	opArguments.push_back(argumentVector);
	registerNames.push_back("Common mode median enable bits (designator = 11)");
	niceOutput.push_back(sop);
	showDeci.push_back(false);

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	sop.clear();
	ostringstream op16;
	Fed9UDaqMode daqMode = getDaqMode();
	if (daqMode == FED9U_MODE_VIRGIN_RAW) {op16 << "VIRGIN RAW mode.";}
	else if (daqMode == FED9U_MODE_SCOPE) {op16 << "SCOPE mode.";}
	else if (daqMode == FED9U_MODE_PROCESSED_RAW) {op16 << "PROCESSED RAW mode.";}
	else if (daqMode == FED9U_MODE_ZERO_SUPPRESSED) {op16 << "ZERO SUPPRESSED mode";}
	else {op16 << "Mode not recognised !";}
	sop = op16.str();

	argumentVector.clear();
	argumentVector.push_back(static_cast<Fed9U::u32>(daqMode));
	opArguments.push_back(argumentVector);
	registerNames.push_back("Mode configuration register (designator = 16)");
	niceOutput.push_back(sop);
	showDeci.push_back(false);

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	theFed->feCommandLoadScopeLength(selectedFpga,true,dummyVector,argumentVector);
	opArguments.push_back(argumentVector);
	registerNames.push_back("Scope length register (designator = 17)");
	niceOutput.push_back("");
	showDeci.push_back(true);

	/*
	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	theFed->feCommandMonitor(selectedFpga,argumentVector);
	opArguments.push_back(argumentVector);
	registerNames.push_back("Command Monitor (designator = 20)");
	*/

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	theFed->feCommandOptoControl(selectedFpga,true,dummyVector,argumentVector);
	ostringstream op22;
	ConvertToBinary(op22,argumentVector,8);
	op22 << std::endl;
	opArguments.push_back(argumentVector);
	registerNames.push_back("OptoRx Control Bits (designator = 22)");
	niceOutput.push_back(op22.str());
	showDeci.push_back(false);

	if (!argumentVector.empty()) {argumentVector.clear();}
	if (!dummyVector.empty()) {dummyVector.clear();}
	theFed->feCommandAdcControl(selectedFpga,true,dummyVector,argumentVector);
	ostringstream op23;
	ConvertToBinary(op23,argumentVector,24);
	op23 << std::endl;
	opArguments.push_back(argumentVector);
	registerNames.push_back("ADC control bits (designator = 23)");
	niceOutput.push_back(op23.str());
	showDeci.push_back(false);
	
	Fed9U::u32 dummyWord = 0;
	if (!argumentVector.empty()) {argumentVector.clear();}
	argumentVector.assign(1,0);
	theFed->feCommandLoadComplement(selectedFpga,true,dummyWord,argumentVector[0]);
	ostringstream op26;
	if (argumentVector[0] == 0x0) {
	  op26 << "No APV data inverted.";
	} else if (argumentVector[0] == 0xFFF) {
	  op26 << "All APV data inverted";
	} else {
	  op26 << "Inverted Fibres: ";
	  for (u32 loop = 0; loop < 12; loop++) {
	    if (static_cast<bool>((argumentVector[0] >> (11 - loop)) & 0x1)) {
	      op26 << (loop/2)+1 << " ";
	    }
	  }
	}

	opArguments.push_back(argumentVector);
	registerNames.push_back("Complement flags (designator = 26)");
	niceOutput.push_back(op26.str());
	showDeci.push_back(false);
	
	ostringstream op30;
	Fed9UDaqSuperMode superDaqMode = getDaqSuperMode();
	if (superDaqMode == FED9U_SUPER_MODE_FAKE) {op30 << "FAKE events; not ZERO SUPPRESSED LITE mode";}
	else if (superDaqMode == FED9U_SUPER_MODE_ZERO_LITE) {op30 << "REAL events; ZERO SUPPRESSED LITE mode.";}
	else if (superDaqMode == FED9U_SUPER_MODE_FAKE_ZERO_LITE) {op30 << "FAKE events; ZERO SUPPRESSED LITE mode.";}
	else if (superDaqMode == FED9U_SUPER_MODE_NORMAL) {op30 << "REAL events; not ZERO SUPPRESSED LITE mode.";}
	else {op30 << " Super DAQ Mode not recognised.";}

	argumentVector.clear();
	argumentVector.push_back(static_cast<Fed9U::u32>(superDaqMode));
	opArguments.push_back(argumentVector);
	registerNames.push_back("Super mode configuration register (designator = 30)");
	niceOutput.push_back(op30.str());
	showDeci.push_back(false);
	
	if (addHtml) {
	  os << "<br><br>" << std::endl;
	  os << "<TABLE>" << std::endl;
	}

	for (unsigned int reg = 0; reg < opArguments.size(); reg++) {
	  if (addHtml) {
	    os << "<TR>";
	    os << "<TD> " << registerNames[reg] << " </TD>";
	    os << "<TD> ";
	  } else {
	    string name = registerNames[reg];
	    name.append(55-name.size(),' ');
	    os << name << "     ";
	  }
	  
	  for (unsigned int v = 0; v < opArguments[reg].size(); ++v) {
	    if (v != 0 && (v % 6) == 0 && addHtml) {os << "<br>";}
	    ostringstream opVals;
	    opVals << std::hex << opArguments[reg][v] << std::dec;
	    string addsp;
	    addsp.append(11-opVals.str().size(),' ');
	    os << "0x" << std::hex << opVals.str() << std::dec << " ";
	    if (opArguments[reg].size() == 1) {os << addsp;}
	  }
	  if (addHtml) {os << " </TD>";}
	  
	  if (!niceOutput[reg].empty()) {
	    if (addHtml) {
	      os << "<TD>";
	      os << niceOutput[reg];
	      os << "</TD>";
	    } else {
	      os << "  " << niceOutput[reg];
	    }
	  } else {
	    if ((showDeci[reg]) && (opArguments[reg].size() == 1)) {
	      if (addHtml) {os << "<TD>";}
	      os << "  " << opArguments[reg][0];
	      if (addHtml) {os << "</TD>";}
	    }
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}
	
	if (addHtml) {
	  os << "</TABLE>" << std::endl;
	  os << "<br>";
	}
      }

      if (selectedFpga == 20) {

	os << std::endl;
	os << "Dumping all read-write FE registers for all eight FE-FPGAs ..." << std::endl;
	if (addHtml) {os << "<br>";}
	os << std::endl;

	vector< vector<Fed9U::u32> > opArguments;
	vector<string> registerNames;
	vector<Fed9U::u32> opFpga;

	opFpga.clear();
	registerNames.push_back("Load Tick register (designator = 2)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  theFed->feCommandLoadTick(fpga,true,dummyVector,argumentVector);
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	opFpga.clear();
	registerNames.push_back("Firmware ID register (designator = 3)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  argumentVector.assign(1,0);
	  theFed->feCommandFirmwareIdReg(fpga,argumentVector[0]); 
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	opFpga.clear();
	registerNames.push_back("Common mode median enable bits (designator = 11)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  theFed->feCommandMedianEnable(fpga,true,dummyVector,argumentVector);
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	opFpga.clear();
	registerNames.push_back("Mode configuration register (designator = 16)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  argumentVector.push_back(static_cast<u32>(getDaqMode()));
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	opFpga.clear();
	registerNames.push_back("Scope length register (designator = 17)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  theFed->feCommandLoadScopeLength(fpga,true,dummyVector,argumentVector);
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);
	
	opFpga.clear();
	registerNames.push_back("OptoRx Control bits (designator = 22)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  theFed->feCommandOptoControl(fpga,true,dummyVector,argumentVector);
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	opFpga.clear();
	registerNames.push_back("ADC control bits (designator = 23)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  theFed->feCommandAdcControl(fpga,true,dummyVector,argumentVector);
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);
	
	Fed9U::u32 dummyWord = 0, argumentWord = 0;
	opFpga.clear();
	registerNames.push_back("Complement flags (designator = 26)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  theFed->feCommandLoadComplement(fpga,true,dummyWord,argumentWord);
	  opFpga.push_back(argumentWord);
	}
	opArguments.push_back(opFpga);
	
	opFpga.clear();
	registerNames.push_back("Super mode configuration register (designator = 30)");
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  argumentVector.push_back(static_cast<u32>(getDaqSuperMode()));
	  opFpga.push_back(argumentVector[0]);
	}
	opArguments.push_back(opFpga);

	string titleString = "FPGA  ";

	if (addHtml) {
	  os << "<TABLE>" << std::endl
	     << "<TR> ";
	} else {
	  titleString.append(55-titleString.size(),' ');
	  titleString.append(3,' ');
	}

	std::ostringstream output;
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (addHtml) {
	    output << "<TD> " << fpga << " </TD>";
	  } else {
	    output << " " << fpga << "          ";
	  }
	}
	if (!addHtml) {titleString.append(output.str());}
	
	if (addHtml) {
	  os << "<TD> " << titleString << " </TD>" << output.str() << " </TR>";
	} else {
	  os << titleString << std::endl;
	}
	os << std::endl;

	for (unsigned int reg = 0; reg < opArguments.size(); reg++) {
	  string name = registerNames[reg];
	  if (addHtml) {
	    os << "<TR>";
	    os << "<TD> " << name << " </TD>";
	  } else {
	    name.append(55-name.size(),' ');
	    os << name << "   ";
	  }
	  for (unsigned int fpga = 0; fpga < opArguments[reg].size(); fpga++) {
	    std::ostringstream output;
	    if (addHtml) {
	      output << "<TD> 0x" << std::hex << opArguments[reg][fpga] << std::dec << "</TD>";
	    } else {
	      output << "0x" << std::hex << opArguments[reg][fpga] << std::dec;
	    }
	    string opArg = output.str();
	    if (addHtml) {
	      if (fpga < opArguments[reg].size()-1) {os << opArg;}
	      else {os << opArg << " </TR>" << std::endl;}
	    } else {
	      opArg.append(12-opArg.size(),' ');
	      if (fpga < opArguments[reg].size()-1) {os << opArg;}
	      else {os << opArg << std::endl;}
	    }
	  }
	}

	if (addHtml) {os << "</TABLE><br>" << std::endl;}
	os << std::endl << std::endl;

	// designator 2
	string sop;
	if (opArguments[0][0] == 0x0) {sop = "All APVs DISABLED";}
	else if (opArguments[0][0] == 0xFFFFFF) {sop = "All APVs ENABLED";}
	else {
	  ostringstream op2;
	  for (u32 loop = 0; loop < 24; loop++) {
	    if (!static_cast<bool>((opArguments[0][0] >> (23 - loop)) & 0x3)) {
	      u32 fibre = (loop/2)+1;
	      u32 apv = loop % 2;
	      op2 << "DISABLED APVs: " << "Fibre " << fibre << " APV " << apv << ",";
	    }
	  }
	  sop = op2.str();
	  int lastChar = sop.size()-1;
	  sop.erase(lastChar,1);
	}
	os << "Designator 2:   " << sop;
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 11
	sop.clear();
	if (opArguments[2][0] == 1) {sop = "Medians will be calculated";}
	else if (opArguments[2][0] == 0) {sop = "Medians will not be calculated";}
	else {sop = "Value NOT RECOGNISED";}
	os << "Designator 11:  " << sop;
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 16
	ostringstream op16;
	Fed9UDaqMode daqMode = static_cast<Fed9UDaqMode>(opArguments[3][0]);
	if (daqMode == FED9U_MODE_VIRGIN_RAW) {op16 << "VIRGIN RAW mode.";}
	else if (daqMode == FED9U_MODE_SCOPE) {op16 << "SCOPE mode.";}
	else if (daqMode == FED9U_MODE_PROCESSED_RAW) {op16 << "PROCESSED RAW mode.";}
	else if (daqMode == FED9U_MODE_ZERO_SUPPRESSED) {op16 << "ZERO SUPPRESSED mode";}
	else {op16 << "Mode not recognised !";}
	os << "Designator 16:  " << op16.str();
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 17
	os << "Designator 17:  " << opArguments[4][0];
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 22
	ostringstream op22;
	ConvertToBinary(op22,opArguments[5],8);
	os << "Designator 22:  " << op22.str();
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 23
	ostringstream op23;
	ConvertToBinary(op23,opArguments[6],24);
	os << "Designator 23:  " << op23.str();
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 26
	ostringstream op26;
	if (opArguments[7][0] == 0x0) {
	  op26 << "No APV data inverted.";
	} else if (opArguments[7][0] == 0xFFF) {
	  op26 << "All APV data inverted";
	} else {
	  op26 << "Inverted Fibres: ";
	  for (u32 loop = 0; loop < 12; loop++) {
	    if (static_cast<bool>((opArguments[7][0] >> (11 - loop)) & 0x1)) {
	      op26 << (loop/2)+1 << " ";
	    }
	  }
	}
	os << "Designator 26:  " << op26.str();
	if (addHtml) {os << "<br>";}
	os << std::endl;

	// designator 30
	ostringstream op30;
	Fed9UDaqSuperMode superDaqMode = static_cast<Fed9UDaqSuperMode>(opArguments[8][0]);
	if (superDaqMode == FED9U_SUPER_MODE_FAKE) {op30 << "FAKE events; not ZERO SUPPRESSED LITE mode";}
	else if (superDaqMode == FED9U_SUPER_MODE_ZERO_LITE) {op30 << "REAL events; ZERO SUPPRESSED LITE mode.";}
	else if (superDaqMode == FED9U_SUPER_MODE_FAKE_ZERO_LITE) {op30 << "FAKE events; ZERO SUPPRESSED LITE mode.";}
	else if (superDaqMode == FED9U_SUPER_MODE_NORMAL) {op30 << "REAL events; not ZERO SUPPRESSED LITE mode.";}
	else {op30 << " Super DAQ Mode not recognised.";}
	os << "Designator 30:  " << op30.str();
	if (addHtml) {os << "<br>";}
	os << std::endl;
	
	/*
	os << "Command Monitor (designator = 20)" << std::endl;
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  theFed->feCommandMonitor(fpga,argumentVector);
	  os << "FE-FPGA " << fpga;
	  for (unsigned int loop = 0; loop < argumentVector.size(); loop++) {
	    os << " 0x" << argumentVector[loop];
	  }
	  os << std::endl;
	}	
	*/
	
      }
      
      os << std::endl;
      if (selectedFpga <= 8) {
	os << "Dump of control registers for FE-FPGA " << selectedFpga << " is complete !";
	if (addHtml) {os << "<br>";}
      } else {
	os << "Dump of control registers for all eight FE-FPGAs is complete !";
	if (addHtml) {os << "<br>";}
      }
      os << std::endl;

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpFeRegisters."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpFeRegisters."));
    }
  }
  //</JEC>


  // <JEC date=03-05-06>
  void Fed9UVmeDevice::dumpApvData(unsigned short selectedFpga, ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {
      vector<u32> dummyVector, argumentVector, argumentVector2, argumentVector3;
      Fed9UAddress addr;

      if (selectedFpga <= 8) {
	addr.setExternalFedFeUnit(selectedFpga);
	string titleString1, titleString2;
	string regName1, regName2;

	if (addHtml) {
	  titleString1 = "<TR><TD> Fibre </TD>";
	  titleString2 = "<TR><TD>       </TD>";
	} else {
	  titleString1 = "Fibre";
	  titleString1.append(10-titleString1.size(),' ');
	  titleString2.assign(10,' ');
	}

	os << std::endl;
	os << "Dumping APV/fibre data for FE-FPGA " << selectedFpga << " ...";
	if (addHtml) {os << "<br>";}
	os << std::endl;

	if (addHtml) {
	  regName1 = "<TD> Frame Thresholds </TD>";
	  regName2 = "<TD> (designator = 4) </TD>";
	} else {
	  regName1 = "Frame Thresholds";
	  regName2 = "(designator = 4)";
	  regName1.append(30-regName1.size(),' ');
	  regName2.append(30-regName2.size(),' ');
	}

	titleString1.append(regName1);
	titleString2.append(regName2);

	if (addHtml) {
	  regName1 = "<TD> Number of valid strips </TD>";
	  regName2 = "<TD> (designator = 15) </TD>";
	} else {
	  regName1 = "Number of valid strips";
	  regName1.append(30-regName1.size(),' ');
	  regName2 = "(designator = 15)";
	  regName2.append(30-regName2.size(),' ');
	}

	titleString1.append(regName1);
	titleString2.append(regName2);

	if (addHtml) {
	  regName1 = "<TD> Load median (per APV) </TD></TR>";
	  regName2 = "<TD> (designator = 10) </TD></TR>";
	} else {
	  regName1 = "Load median (per APV)";
	  regName1.append(30-regName1.size(),' ');
	  regName2 = "(designator = 10)";
	  regName2.append(30-regName2.size(),' ');
	}
	
	titleString1.append(regName1);
	titleString2.append(regName2);
	
	// Frame Threshold data
	for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	  addr.setFeUnitChannel(ch);
	  argumentVector.push_back(static_cast<u32>(getFrameThreshold(addr)));
	}
	
	// Number of valid strips for each APV
	for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	  addr.setFeUnitChannel(ch);
	  for (u8 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	    addr.setChannelApv(apv);
	    argumentVector2.push_back(static_cast<u32>(getNumberValidStrips(addr)));
	  }
	}
	
	// Load median
	bool disable = false;
	vector<u16> medianOverride = getCmMedianOverride(addr,disable);
	for (u32 loop = 0; loop < medianOverride.size(); loop++) {
	  argumentVector3.push_back(static_cast<u32>(medianOverride[loop]));
	}
	
	// Output of fibre-related data
	if (addHtml) {os << "<TABLE>" << std::endl;}
	os << titleString1 << std::endl;
	os << titleString2 << std::endl;

	unsigned int counter = 0;

	for (unsigned int fibre = 0; fibre < argumentVector.size(); fibre++) {
	  std::ostringstream output1, output2, output3, fibreNumber;
	  string opArg, opArg1, opArg2, fNumber;

	  output1 << "   0x" << std::hex << argumentVector[fibre] << std::dec;
	  opArg = output1.str();
	  if (!addHtml) {opArg.append(30-opArg.size(),' ');}

	  output2 << "   0x" << std::hex << argumentVector2[counter] << "   0x" << std::hex << argumentVector2[counter+1] << std::dec;
	  opArg1 = output2.str();
	  if (!addHtml) {opArg1.append(30-opArg1.size(),' ');}

	  output3 << "   0x" << std::hex << argumentVector3[counter] << "   0x" << std::hex << argumentVector3[counter+1] << std::dec;
	  counter += 2;
	  opArg2 = output3.str();
	  if (!addHtml) {opArg2.append(30-opArg2.size(),' ');}

	  fibreNumber << fibre+1;
	  fNumber = fibreNumber.str();
	  if (!addHtml) {fNumber.append(10-fNumber.size(),' ');}

	  if (addHtml) {
	    os << "<TR><TD> " << fNumber << " </TD><TD> " << opArg << " </TD><TD> " << opArg1 << " </TD><TD> " << opArg2 << " </TD></TR>" << std::endl;
	  } else {
	    os << fNumber << opArg << opArg1 << opArg2 << std::endl;
	  }
	}
	if (addHtml) {
	  os << "</TABLE>" << std::endl;
	  os << "<br>" << std::endl;
	  os << "<br>";
	}
	os << std::endl;
	os << "Threshold setting is (designator = 4):             " << argumentVector[0];
	if (addHtml) {os << "<br>";}
	os << std::endl;
	os << "Number of valid strips per APV (designator = 15):  ";
	os << "APV0 = " << argumentVector2[0];
	os << " APV1 = " << argumentVector2[1];
	if (addHtml) {os << "<br>";}
	os << std::endl;
	os << "Load Median register contents (designator = 10):   " << argumentVector3[0] << " " << argumentVector3[1];
	if (addHtml) {os << "<br>";}
	os << std::endl;
	if (disable) {os << "Contents of load Median register will be overwritten by next Median calculation.";}
	else {os << "Contents of load Median register will be used to set the Median values.";}
	if (addHtml) {os << "<br>";}
	os << std::endl;
	
      } // end of block for looking at an individual FE-FPGA

      if (selectedFpga == 20) {
	os << std::endl;
	os << "Dumping APV/fibre data for all eight FE-FPGAs ..." << std::endl;
	if (addHtml) {os << "<br>";}
	os << std::endl;

	string titleString1, titleString2;
	u32 threshold = 0;

	os << "Frame thresholds (designator = 4) ...";
	if (addHtml) {os << "<br>";}
	os << std::endl;

	if (addHtml) {
	  titleString1 = "<TR><TD> FPGA/Fibre </TD>";
	  titleString2 = titleString1;
	} else {
	  titleString1 = "FPGA/Fibre";
	  titleString1.append(15-titleString1.size(),' ');
	}

	for (unsigned int fibre = 1; fibre < 13; fibre++) {
	  std::ostringstream fpgaNumber;
	  if (addHtml) {
	    fpgaNumber << "<TD> " << fibre << " </TD>";
	  } else {
	    fpgaNumber << fibre;
	  }
	  string fNumber = fpgaNumber.str();
	  if (!addHtml) {fNumber.append(10-fNumber.size(),' ');}
	  titleString1.append(fNumber);
	  if (addHtml) {
	    titleString2.append("<TD> </TD>");
	    titleString2.append(fNumber);
	  }
	}

	if (addHtml) {os << "<TABLE>" << std::endl;}
	os << titleString1 << std::endl;

	// threshold settings
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  addr.setExternalFedFeUnit(static_cast<u8>(fpga));
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  
	  std::ostringstream fpgaNumber;
	  string output;
	  
	  fpgaNumber << fpga;
	  string fNumber = fpgaNumber.str();
	  if (!addHtml) {fNumber.append(15-fNumber.size(),' ');}

	  // Frame Threshold data
	  for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    addr.setFeUnitChannel(ch);
	    argumentVector.push_back(static_cast<u32>(getFrameThreshold(addr)));
	  }
	  if (fpga == 1) {threshold = argumentVector[0];}

	  for (unsigned int fibre = 0; fibre < argumentVector.size(); fibre++) {
	    std::ostringstream op;
	    if (addHtml) {
	      op << "<TD> 0x" << std::hex << argumentVector[fibre] << std::dec << "</TD>";
	    } else {
	      op << "0x" << std::hex << argumentVector[fibre] << std::dec;
	    }
	    string opArg = op.str();
	    if (!addHtml) {opArg.append(10-opArg.size(),' ');}
	    output.append(opArg);
	  }
	  if (addHtml) {
	    os << "<TR><TD> " << fNumber << " </TD>" << output << "</TR>" << std::endl;
	  } else {
	    os << fNumber << output << std::endl;
	  }
	}
	if (addHtml) {os << "</TABLE><br>" << std::endl;}

	os << std::endl;
	os << "Number of valid strips (designator = 15) ...";
	if (addHtml) {os << "<br>";}
	os << std::endl;

	if (addHtml) {
	  os << "<TABLE>" << std::endl;
	  os << titleString2 << std::endl;
	} else {
	  os << titleString1 << std::endl;
	}

	// number of valid strips
	u32 validStrips1 = 0, validStrips2 = 0;

	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  addr.setExternalFedFeUnit(static_cast<u8>(fpga));
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  
	  std::ostringstream fpgaNumber;
	  string output;
	  
	  fpgaNumber << fpga;
	  string fNumber = fpgaNumber.str();
	  if (!addHtml) {fNumber.append(15-fNumber.size(),' ');}
	  
	  // Number of valid strips for each APV
	  for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    addr.setFeUnitChannel(ch);
	    for (u8 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	      addr.setChannelApv(apv);
	      argumentVector.push_back(static_cast<u32>(getNumberValidStrips(addr)));
	    }
	  }
	  if (fpga == 1) {
	    validStrips1 = argumentVector[0];
	    validStrips2 = argumentVector[1];
	  }
	  
	  for (unsigned int fibre = 0; fibre < argumentVector.size(); fibre++) {
	    std::ostringstream op;
	    if (addHtml) {
	      op << "<TD> 0x" << std::hex << argumentVector[fibre] << std::dec << "</TD>";
	    } else {
	      op << "0x" << std::hex << argumentVector[fibre] << std::dec;
	    }
	    string opArg = op.str();
	    if (!addHtml) {opArg.append(5-opArg.size(),' ');}
	    output.append(opArg);
	  }
	  if (addHtml) {
	    os << "<TR><TD> " << fNumber << " </TD>" << output << "</TR>" << std::endl;
	  } else {
	    os << fNumber << output << std::endl;
	  }
	}
	if (addHtml) {os << "</TABLE><br>" << std::endl;}

	os << std::endl;
	os << "Load median register (designator = 10)...";
	if (addHtml) {os << "<br>";}
	os << std::endl;

	if (addHtml) {
	  os << "<TABLE>" << std::endl;
	  os << titleString2 << std::endl;
	} else {
	  os << titleString1 << std::endl;
	}

	// load median
	u32 lMedian1 = 0, lMedian2 = 0;
	bool Mdisable = false;
	for (unsigned int fpga = 1; fpga < 9; fpga++) {
	  addr.setExternalFedFeUnit(static_cast<u8>(fpga));
	  if (!dummyVector.empty()) {dummyVector.clear();}
	  if (!argumentVector.empty()) {argumentVector.clear();}
	  
	  std::ostringstream fpgaNumber;
	  string output;
	  
	  fpgaNumber << fpga;
	  string fNumber = fpgaNumber.str();
	  if (!addHtml) {fNumber.append(15-fNumber.size(),' ');}
	  
	  //	  theFed->feCommandLoadMedian(fpga,true,dummyVector,argumentVector);
	  bool disable = false;
	  vector<u16> medianOverride = getCmMedianOverride(addr,disable);
	  for (u32 loop = 0; loop < medianOverride.size(); loop++) {
	    argumentVector.push_back(static_cast<u32>(medianOverride[loop]));
	  }
	  
	  if (fpga == 1) {
	    lMedian1 = argumentVector[0];
	    lMedian2 = argumentVector[1];
	    Mdisable = disable;
	  }
	  
	  for (unsigned int apv = 0; apv < argumentVector.size(); apv++) {
	    std::ostringstream op;
	    if (addHtml) {
	      op << "<TD> 0x" << std::hex << argumentVector[apv] << std::dec << "</TD>";
	    } else {
	      op << "0x" << std::hex << argumentVector[apv] << std::dec;
	    }
	    string opArg = op.str();
	    if (!addHtml) {opArg.append(5-opArg.size(),' ');}
	    output.append(opArg);
	  }
	  if (addHtml) {
	    os << "<TR><TD> " << fNumber << " </TD>" << output << "</TR>" << std::endl;
	  } else {
	    os << fNumber << output << std::endl;
	  }
	}
	if (addHtml) {os << "</TABLE><br>" << std::endl;}

	os << std::endl;
	os << "Frame threshold is (designator = 4):             " << threshold;
	if (addHtml) {os << "<br>";}
	os << std::endl;
	os << "Number of valid strips per APV (designator = 15):  ";
	os << "APV0 = " << validStrips1;
	os << " APV1 = " << validStrips2;
	if (addHtml) {os << "<br>";}
	os << std::endl;
	os << "Load Median register contents (designator = 10):   " << lMedian1 << " " << lMedian2;
	if (addHtml) {os << "<br>";}
	os << std::endl;
	if (Mdisable) {os << "Contents of load Median register will be overwritten by next Median calculation.";}
	else {os << "Contents of load Median register will be used to set the Median values.";}
	if (addHtml) {os << "<br>";}
	os << std::endl;


      } // end of block for looking at all FE-FPGAs

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpApvData."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpApvData."));
    }
  }
  //</JEC>


  // <JEC date=10-05-06>
  // <JEC date=30-01-07> New table layout for pedestals, thresholds etc.  Remove simultaneous dump of all eight FE-FPGAs.
  void Fed9UVmeDevice::dumpPedAndThreshData(unsigned short selectedFpga, ostream &os, bool addHtml) throw (Fed9UVmeDeviceException) 
  {
    try {

      vector< vector<u16> > allPedestals, allLoThresh, allHiThresh;
      vector< vector<bool> > allDisabledStrips;

      vector<u32> dummyVector, argumentVector, argumentVector2;
      vector<u16> hiThresh, loThresh, pedest;
      vector<bool> disabledStrip;
      Fed9UAddress addr;
      string titleString1, regName;

      if (selectedFpga <= 8) {
	addr.setExternalFedFeUnit(selectedFpga);
	os << "Dumping Pedestals and thresholds for FE-FPGA " << selectedFpga << " using designator 13" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}
	os << "Setting validstrip low excludes the strip from Common-mode calculation and Cluster Finding." << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}
	os << "Setting the cluster threshold to 255 just excludes it from Cluster Finding." << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}	

	// extract the required information into vectors
	u16 numberValidStrips = 0;
	for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	  addr.setFeUnitChannel(ch);
	  for (u8 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	    addr.setChannelApv(apv);
	    disabledStrip.clear();
	    disabledStrip.resize(STRIPS_PER_APV);
	    pedest.clear();
	    getPedsAndDisabledStrips(addr,pedest,disabledStrip,numberValidStrips);
	    allPedestals.push_back(pedest);
	    allDisabledStrips.push_back(disabledStrip);

	    hiThresh.clear();
	    loThresh.clear();
	    hiThresh.resize(STRIPS_PER_APV);
	    loThresh.resize(STRIPS_PER_APV);
	    getClusterThresholds(addr,hiThresh,loThresh);
	    allLoThresh.push_back(loThresh);
	    allHiThresh.push_back(hiThresh);
	  }
	}

	// generic title strings
	string newTitle1, newTitle2;

	if (addHtml) {
	  newTitle1 = "<TR><TD>Channel</TD>";
	  newTitle2 = "<TR><TD>APV</TD>";
	} else {
	  newTitle1 = "Channel";
	  newTitle1.append(10-newTitle1.size(),' ');
	  newTitle2 = "APV";
	  newTitle2.append(10-newTitle2.size(),' ');
	}

	for (u8 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	  addr.setFeUnitChannel(ch);
	  std::ostringstream chNumber;
	  chNumber << static_cast<u32>(addr.getExternalFeUnitChannel());
	  if (addHtml) {newTitle1.append("<TD>");}
	  newTitle1.append(chNumber.str());
	  if (addHtml) {newTitle1.append("</TD><TD></TD>");}
	  if (!addHtml) {newTitle1.append(12-chNumber.str().size(),' ');}
	  
	  for (u8 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	    addr.setChannelApv(apv);
	    std::ostringstream aNumber;
	    aNumber << static_cast<u32>(addr.getExternalChannelApv());
	    if (addHtml) {newTitle2.append("</TD><TD>");}
	    newTitle2.append(aNumber.str());
	    if (!addHtml) {newTitle2.append(6-aNumber.str().size(),' ');}
	  }
	}


	// pedestal values
	if (addHtml) {os << "<br>" << std::endl;}
	os << "PEDESTAL VALUES:" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}

	if (addHtml) {os << "<TABLE>" << std::endl;}

	os << newTitle1; 
	if (addHtml) {os << "</TD></TR>";} 
	os << std::endl;
	os << newTitle2;
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;

	for (u32 strips = 0; strips < allPedestals[0].size(); strips++) {
	  u32 vec_count = 0;
	  if (addHtml) {
	    os << "<TR><TD> " << strips << " </TD>";
	  } else {
	    std::ostringstream aStripNumber;
	    aStripNumber << strips;
	    string stripNumber = aStripNumber.str();
	    stripNumber.append(10-aStripNumber.str().size(),' ');
	    os << stripNumber;
	  }
	  for (u32 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    for (u32 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	      if (addHtml) {
		os << "<TD> 0x" << std::hex << allPedestals[vec_count][strips] << std::dec << "</TD>";
	      } else {
		os << "0x" << std::hex << allPedestals[vec_count][strips] << std::dec << "  ";
	      }
	      vec_count++;
	    }
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}

	if (addHtml) {os << "</TABLE>" << std::endl;}

	// disabled strips
	if (addHtml) {os << "<br>" << std::endl;}
	os << "DISABLED STRIPS:" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}

	if (addHtml) {os << "<TABLE>" << std::endl;}
	
	os << newTitle1; 
	if (addHtml) {os << "</TD></TR>";} 
	os << std::endl;
	os << newTitle2;
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;
	
	for (u32 strips = 0; strips < allPedestals[0].size(); strips++) {
	  u32 vec_count = 0;
	  if (addHtml) {
	    os << "<TR><TD> " << strips << " </TD>";
	  } else {
	    std::ostringstream aStripNumber;
	    aStripNumber << strips;
	    string stripNumber = aStripNumber.str();
	    stripNumber.append(10-aStripNumber.str().size(),' ');
	    os << stripNumber;
	  }
	  for (u32 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    for (u32 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	      if (addHtml) {os << "<TD>";}
	      if (allDisabledStrips[vec_count][strips]) {os << "true  ";}
	      else {os << "false ";}
	      if (addHtml) {os << "</TD>";}
	      vec_count++;
	    }
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}

	if (addHtml) {os << "</TABLE>" << std::endl;}

	// low threshold values
	if (addHtml) {os << "<br>" << std::endl;}
	os << "LOW THRESHOLD VALUES:" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}

	if (addHtml) {os << "<TABLE>" << std::endl;}
	
	os << newTitle1; 
	if (addHtml) {os << "</TD></TR>";} 
	os << std::endl;
	os << newTitle2;
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;

	for (u32 strips = 0; strips < allLoThresh[0].size(); strips++) {
	  u32 vec_count = 0;
	  if (addHtml) {
	    os << "<TR><TD> " << strips << " </TD>";
	  } else {
	    std::ostringstream aStripNumber;
	    aStripNumber << strips;
	    string stripNumber = aStripNumber.str();
	    stripNumber.append(10-aStripNumber.str().size(),' ');
	    os << stripNumber;
	  }
	  for (u32 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    for (u32 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	      if (addHtml) {
		os << "<TD> 0x" << std::hex << allLoThresh[vec_count][strips] << std::dec << "</TD>";
	      } else {
		os << "0x" << std::hex << allLoThresh[vec_count][strips] << std::dec << "   ";
	      }
	      vec_count++;
	    }
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}

	if (addHtml) {os << "</TABLE>" << std::endl;}

	// high thresholds
	if (addHtml) {os << "<br>" << std::endl;}
	os << "HIGH THRESHOLD VALUES:" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}

	if (addHtml) {os << "<TABLE>" << std::endl;}

	os << newTitle1; 
	if (addHtml) {os << "</TD></TR>";} 
	os << std::endl;
	os << newTitle2;
	if (addHtml) {os << "</TD></TR>";}
	os << std::endl;
	
	for (u32 strips = 0; strips < allHiThresh[0].size(); strips++) {
	  u32 vec_count = 0;
	  if (addHtml) {
	    os << "<TR><TD> " << strips << " </TD>";
	  } else {
	    std::ostringstream aStripNumber;
	    aStripNumber << strips;
	    string stripNumber = aStripNumber.str();
	    stripNumber.append(10-aStripNumber.str().size(),' ');
	    os << stripNumber;
	  }
	  for (u32 ch = 0; ch < CHANNELS_PER_FEUNIT; ch++) {
	    for (u32 apv = 0; apv < APVS_PER_CHANNEL; apv++) {
	      if (addHtml) {
		os << "<TD> 0x" << std::hex << allHiThresh[vec_count][strips] << std::dec << "</TD>";
	      } else {
		os << "0x" << std::hex << allHiThresh[vec_count][strips] << std::dec << "  ";
	      }
	      vec_count++;
	    }
	  }
	  if (addHtml) {os << "</TR>";}
	  os << std::endl;
	}

	if (addHtml) {
	  os << "</TABLE>" << std::endl;
	  os << "<br>" << std::endl;
	}

      } else if (selectedFpga == 20) {
	os << "Pedestal values and thresholds not available for all eight FE-FPGAs simultaneously" << std::endl;
	if (addHtml) {os << "<br>" << std::endl;}
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "std::exception caught in Fed9UVmeDevice::dumpPedAndThreshData."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unknown exception caught in Fed9UVmeDevice::dumpPedAndThreshData."));
    }
  }
  //</JEC></JEC>

}//end of namespace
