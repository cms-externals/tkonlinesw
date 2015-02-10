#include "Fed9UCreateDescription.hh"
#include "Fed9UCreateDescriptionException.hh"
#include "Fed9UAddress.hh"
#include "ICAssert.hh"

#include <string>

namespace Fed9U {

  using namespace std;


  Fed9UCreateDescription::Fed9UCreateDescription(u32 fed9UDescriptionNumber) : fed9UDescriptionNumber_(fed9UDescriptionNumber) {
  }


  Fed9UDescription Fed9UCreateDescription::makeFed9UDescription(u16 vmeControllerDaisyChainId, u32 baseAddress, std::string halAddressTable) {
    try {
      fedDescription.setVmeControllerDaisyChainId(vmeControllerDaisyChainId);
      fedDescription.setBaseAddress(baseAddress);
      fedDescription.setHalAddressTable(halAddressTable);
      fedDescription.setEpromVersion(2);
      loadFeFpgaSettings().loadBeFpgaSettings().loadVmeFpgaSettings().loadHigherLevelSettings();

      ++fed9UDescriptionNumber_;

      return fedDescription;
    } catch(ICUtils::ICException& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::makeFed9UDescription."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadFeFpgaSettings() {
    try {
      //Most of the FE FPGAs methods require a Fed9UAddress object.
      Fed9UAddress addr;

      //Setup all the FE FPGA settings.
      fedDescription.setScopeLength( fed9UDescriptionNumber_%1022 );

      for(u16 feFpga = 0; feFpga < FEUNITS_PER_FED; ++feFpga) {
	addr.setFedFeUnit(feFpga);

	fedDescription.setOptoRxInputOffset(addr, (feFpga%16 + fed9UDescriptionNumber_%16)%16 );
	fedDescription.setOptoRxOutputOffset(addr, (feFpga%4 + fed9UDescriptionNumber_%4)%4 );
	fedDescription.setOptoRxCapacitor(addr, (feFpga%4 + fed9UDescriptionNumber_%4)%4 );

	vector<u16> cmMedianValues(24,0);
	for(u16 fpgaChannel = 0; fpgaChannel < CHANNELS_PER_FEUNIT; ++fpgaChannel) {
	  addr.setFeUnitChannel(fpgaChannel);

	  fedDescription.setFineDelay(addr, (fpgaChannel%32 + fed9UDescriptionNumber_%32)%32 );
	  fedDescription.setCoarseDelay(addr, (fpgaChannel%16 + fed9UDescriptionNumber_%16)%16 );
	  fedDescription.setTrimDacOffset(addr, (fpgaChannel%256 + fed9UDescriptionNumber_%256)%256 );
	  fedDescription.setFrameThreshold(addr, ((fpgaChannel%32 + fed9UDescriptionNumber_%32)%32) * 32 );
	  fedDescription.setComplement(addr, (fpgaChannel%2 + fed9UDescriptionNumber_%2)%2 );
	  //The ADCs have two valid operating modes.
	  Fed9UAdcControls adcSettings;
//  	  if ( (fpgaChannel%2 + fed9UDescriptionNumber_%2)%2 ) {
	  adcSettings._dfsen  = fed9UDescriptionNumber_%2;
	  adcSettings._dfsval = fed9UDescriptionNumber_%2;
	  adcSettings._s1     = fed9UDescriptionNumber_%2;
	  adcSettings._s2     = fed9UDescriptionNumber_%2;
//  	  } else {
//  	    adcSettings._dfsen  = false;
//  	    adcSettings._dfsval = false;
//  	    adcSettings._s1     = true;
//  	    adcSettings._s2     = false;
//  	  }
	  fedDescription.setAdcControls(addr, adcSettings);

	  for (u16 channelApv = 0; channelApv < APVS_PER_CHANNEL; ++channelApv) {
	    addr.setChannelApv(channelApv);

	    fedDescription.setApvDisable(addr, fed9UDescriptionNumber_%2 );
	    cmMedianValues[ addr.getFeUnitApv() ] = (addr.getFeUnitApv()%1024 + fed9UDescriptionNumber_%1024)%1024;

	    vector<u32> pedestals(128,0);
	    vector<bool> disableStrips(128,0);
	    vector<u32> highThresholds(128,0);
	    vector<u32> lowThresholds(128,0);
	    vector<Fed9UStripDescription> apvStripDescription(128);
	    for (u16 apvStrip = 0; apvStrip < STRIPS_PER_APV; ++apvStrip) {
	      addr.setApvStrip(apvStrip);

	      pedestals[apvStrip]      = (apvStrip%1024 + fed9UDescriptionNumber_%1024)%1024;
	      disableStrips[apvStrip]  = (apvStrip%2 + fed9UDescriptionNumber_%2)%2;
	      highThresholds[apvStrip] = (apvStrip%256 + fed9UDescriptionNumber_%256)%256;
	      lowThresholds[apvStrip]  = (apvStrip%256 + fed9UDescriptionNumber_%256)%256;

	      apvStripDescription[apvStrip].setPedestal( pedestals[apvStrip] );
	      apvStripDescription[apvStrip].setDisable( disableStrips[apvStrip] );
	      apvStripDescription[apvStrip].setLowThreshold( lowThresholds[apvStrip] );
	      apvStripDescription[apvStrip].setHighThreshold( highThresholds[apvStrip] );
	    }//End of strip APV loop.
	    fedDescription.getFedStrips().setApvStrips(addr, apvStripDescription);

	  }//End of channel APV loop.
	}//End of FPGA channel loop.
	//The description median values will only be set when the median override is enabled.
	if ( (feFpga%2 + fed9UDescriptionNumber_%2)%2 ) {
	  //If the median override is disabled then the median values will not be set. To prevent an
	  //error in a check they are set to zero which is what they will be read back as on the FED.
	  vector<u16> dummyVector(24,0);
	  fedDescription.setCmMedianOverride(addr, (feFpga%2 + fed9UDescriptionNumber_%2)%2, dummyVector );
	} else {
	  fedDescription.setCmMedianOverride(addr, (feFpga%2 + fed9UDescriptionNumber_%2)%2, cmMedianValues);
	}
      }//End of FE FPGA loop.

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadFeFpgaSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadBeFpgaSettings() {
    try {
      loadTtcrxDeviceSettings();
      fedDescription.setFedBeFpgaDisable( fed9UDescriptionNumber_%2 );
      fedDescription.setFedId( fed9UDescriptionNumber_%4096 );
      fedDescription.setTestRegister( fed9UDescriptionNumber_%1024 );

      Fed9UTrigSource trigSource;
      Fed9UReadRoute readRoute;
      if (0 == fed9UDescriptionNumber_%3) {
	trigSource = FED9U_TRIG_TTC;
	readRoute  = FED9U_ROUTE_SLINK64;
      } else if (1 == fed9UDescriptionNumber_%3) {
	trigSource = FED9U_TRIG_BACKPLANE;
	readRoute  = FED9U_ROUTE_VME;
      } else {
	trigSource = FED9U_TRIG_SOFTWARE;
	readRoute  = FED9U_ROUTE_SLINK64;
      }
      fedDescription.setTriggerSource(trigSource);
      fedDescription.setBeFpgaReadRoute(readRoute);

      Fed9UAddress addr;
      for (u16 feFpga = 0; feFpga < FEUNITS_PER_FED; ++feFpga) {
	addr.setFedFeUnit(feFpga);
	fedDescription.setFedFeUnitDisable(addr, (feFpga%2 + fed9UDescriptionNumber_%2)%2 );
      }//End of FE FPGA loop.

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadBeFpgaSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadVmeFpgaSettings() {
    try {
      loadVoltageMonitorSettings();
      loadEpromSettings();

      Fed9UClockSource clockSource;
//        if (0 == fed9UDescriptionNumber_%3) {
//  	clockSource = FED9U_CLOCK_BACKPLANE;
//        } else if (1 == fed9UDescriptionNumber_%3) {
//  	clockSource = FED9U_CLOCK_INTERNAL;
//        } else {
	clockSource = FED9U_CLOCK_TTC;
//        }
      fedDescription.setClock(clockSource);

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadVmeFpgaSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadHigherLevelSettings() {
    try {
      Fed9UDaqMode daqMode;
      if (0 == fed9UDescriptionNumber_%4) {
	daqMode = FED9U_MODE_PROCESSED_RAW;
      } else if (1 == fed9UDescriptionNumber_%3) {
	daqMode = FED9U_MODE_VIRGIN_RAW;
      } else if (1 == fed9UDescriptionNumber_%3) {
	daqMode = FED9U_MODE_ZERO_SUPPRESSED;
      } else {
	daqMode = FED9U_MODE_SCOPE;
      }
      fedDescription.setDaqMode(daqMode);

      Fed9UAddress addr;
      const u16 FPGAS_PER_FED = 10;
      Fed9UTempControl tempControl;
      for (u16 fedFpga = 0; fedFpga <= FPGAS_PER_FED; ++fedFpga) {
	if (8 == fedFpga) {
	  //do nothing as there is no FED FPGA at address 8.
	} else {
	  addr.setFedFpga(fedFpga);
	  //Write to the temperature monitor.
	  tempControl.setLm82High( (fedFpga%128 + fed9UDescriptionNumber_%128)%128 );
	  tempControl.setFpgaHigh( (fedFpga%128 + fed9UDescriptionNumber_%128)%128 );
	  //If the critical temperature is set to low then the FED will shut down.
	  //This would stop the soak test. Therefore the critical temperature is limit to the range 120-127.
	  tempControl.setCritical(120 + fed9UDescriptionNumber_%8);
	  fedDescription.setTempControl(addr, tempControl);
	}//End of if-else 8 == fedFpga
      }//End of for fedFpga

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadHigherLevelSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadTtcrxDeviceSettings() {
    try {
      Fed9UTtcrxDescription ttcrxDescription;

      ttcrxDescription.setL1AcceptCoarseDelay( fed9UDescriptionNumber_%16 );
      ttcrxDescription.setBrcstStrTwoCoarseDelay( fed9UDescriptionNumber_%16 );
      ttcrxDescription.setClockDesOneFineDelay( fed9UDescriptionNumber_%240 );
      ttcrxDescription.setClockDesTwoFineDelay( fed9UDescriptionNumber_%240 );
      ttcrxDescription.setDllPumpCurrent( 1 + fed9UDescriptionNumber_%7 );
      ttcrxDescription.setPllPumpCurrent( 1 + fed9UDescriptionNumber_%7 );
      ttcrxDescription.setIacId( fed9UDescriptionNumber_%16384 );
      //Setting this to anything but zero will cause the I2C interface to stop working.
      ttcrxDescription.setI2cId(0);
      ttcrxDescription.setHammingCheckingDisable( fed9UDescriptionNumber_%2 );
      ttcrxDescription.setCounterOperation( fed9UDescriptionNumber_%4 );
      ttcrxDescription.setDeskewedClock2Selected( (2 + fed9UDescriptionNumber_%2)%2 );
      ttcrxDescription.setDeskewedClock2Disable( (1 + fed9UDescriptionNumber_%2)%2 );
      ttcrxDescription.setClockL1AcceptDisable( (2 + fed9UDescriptionNumber_%2)%2 );
      ttcrxDescription.setNonDeskewedClockDisable( (1 + fed9UDescriptionNumber_%2)%2 );
      ttcrxDescription.setParrallelOutputDisable( (2 + fed9UDescriptionNumber_%2)%2 );
      ttcrxDescription.setSerialBDisable( (1 + fed9UDescriptionNumber_%2)%2 );

      fedDescription.setTtcrx(ttcrxDescription);

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadTtcrxDeviceSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadVoltageMonitorSettings() {
    try {
      Fed9UVoltageControl voltageController;

      voltageController.set2Point5VoltMax( (fed9UDescriptionNumber_%256 / 255) * 3.320 );
      voltageController.set2Point5VoltMin( (fed9UDescriptionNumber_%256 / 255) * 3.320 );
      voltageController.set3Point3VoltMax( (fed9UDescriptionNumber_%256 / 255) * 4.383 );
      voltageController.set3Point3VoltMin( (fed9UDescriptionNumber_%256 / 255) * 4.383 );
      voltageController.set5VoltMax( (fed9UDescriptionNumber_%256 / 255) * 6.640 );
      voltageController.set5VoltMin( (fed9UDescriptionNumber_%256 / 255) * 6.640 );
      voltageController.set12VoltMax( (fed9UDescriptionNumber_%256 / 255) * 15.938 );
      voltageController.set12VoltMin( (fed9UDescriptionNumber_%256 / 255) * 15.938 );
      voltageController.setCoreVoltageMax( (fed9UDescriptionNumber_%256 / 255) * 2.987 );
      voltageController.setCoreVoltageMin( (fed9UDescriptionNumber_%256 / 255) * 2.987 );
      voltageController.setSupplyVoltageMax( (fed9UDescriptionNumber_%256 / 255) * 4.383 );
      voltageController.setSupplyVoltageMin( (fed9UDescriptionNumber_%256 / 255) * 4.383 );
      i8 tempLimit = fed9UDescriptionNumber_%256;
      voltageController.setExternalTempMax( tempLimit );
      voltageController.setExternalTempMin( tempLimit );
      voltageController.setInternalTempMax( tempLimit );
      voltageController.setInternalTempMin( tempLimit );
      voltageController.setTempOffset( tempLimit, fed9UDescriptionNumber_%2 );

      fedDescription.setVoltageMonitor(voltageController);

      return *this;
    }	catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadVoltageMonitorSettings."));
    }
  }


  Fed9UCreateDescription& Fed9UCreateDescription::loadEpromSettings() {
    try {
      Fed9UEpromDescription epromDescription(2);
      u16 epromSize = epromDescription.getEpromSize() - 1;

      for (u16 offset = 0; offset < epromSize; ++offset) {
	epromDescription[offset] = (offset%256 + fed9UDescriptionNumber_%256)%256;
      }//End of for EPROM offset

      return *this;
    } catch(exception& e) {
      RETHROW(e, Fed9UCreateDescriptionException(Fed9UCreateDescriptionException::ERROR_CREATE_DESCRIPTION, "Error in Fed9UCreateDescription::loadEpromSettings."));
    }
  }

}// End of Fed9U namespace
