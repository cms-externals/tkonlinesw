#include <inttypes.h>
#include <stdint.h>
#include "Fed9UDevice.hh"
#include "Fed9UWait.hh"
#include "Fed9ULog.hh"
//#include "Fed9UEvent.hh"
#ifdef EVENT_STREAMLINE
#include "Fed9UEventStreamLine.hh"
#else
#include "Fed9UEvent.hh"
#endif
#include "Fed9UCreateDescription.hh"
#include "Fed9ULogTemplate.hh"
// <NAC date="04/05/2009"> for spy channel header packing
#include "Fed9UCrc.hh"
// </NAC>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <signal.h>

#include "Fed9UBufferedEvent.hh"
//#define EVENT_FILE "/home/xdaq/data/testnumber.dat"

namespace Fed9U {

  using namespace std;

  Fed9UDevice::Fed9UDevice(const Fed9UDescription& fed9UDescription,u32 adaptorSlot):
    Fed9UVmeDevice(fed9UDescription,adaptorSlot),
    _fed9UDescription(new Fed9UDescription(fed9UDescription)),
    _eventNumber(0), _blockModeReadout(true)
  {
    //<JRF date="30/11/2006"> added this to prevent block readout errors in all other controllers
    if (fed9UDescription.getBusAdaptorType() != FED9U_HAL_BUS_ADAPTOR_CAEN_PCI)
      _blockModeReadout=false;
    //</JRF>
    
    //Used to ensure that our writes are atomic, by sending everything to our thread safe stream in one operation.
    std::ostringstream mess;
    mess<<"Fed9UDevice::Fed9UDevice"<<std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED)<<mess.str();
    mess.str("");
    _oldHandler = signal(SIGINT, ControlCHandler);
    //ifstream ifile(EVENT_FILE);
    //if (ifile.is_open()) {
    //  ifile >> _eventNumber;
    //} else {
    //  mess << "Event number file (" << EVENT_FILE << ") cannot be opened, enter starting event number" << std::endl;
    //  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess;
    //  mess.str("");
    //  gFed9ULog.stamp() << "Event-number file doesn't exist";
    //  cin >> _eventNumber;
    //  gFed9ULog << ", starting from " << _eventNumber << endl;
    //}
  }

  Fed9UDevice::~Fed9UDevice() {
    signal(SIGINT, _oldHandler);
    //delete _fed;
    // <NAC date="23/05/2007"> changed to auto_ptr so unnecessary
    //delete _fed9UDescription;
    // </NAC>
    //ofstream ofile(EVENT_FILE);
    //if (ofile.is_open()) {
    //  ofile << _eventNumber;
    //} else {
    //  std::ostringstream mess;
    //  mess << "Unable to write to event number file (" << EVENT_FILE << ")" << std::endl
    //   << "Final event number = " << _eventNumber << std::endl;
    //  Fed9UMessage<Fed9UDebug>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
    //  mess.str("");
    //  gFed9ULog.stamp() << "Event-number file doesn't exist, final event number is "<<_eventNumber<<endl;
    //}
  }
  /*
  void Fed9UDevice::init() {
    try {
      init();
      //_fed->frontEndCheck();
      //_fed->backEndCheck();
    } catch(exception & e) {
      RETHROW(e, Fed9UDeviceException("Init failed"));
    }
  }
  
  void Fed9UDevice::start() {
    _fed->start();
  }
  
  void Fed9UDevice::stop() {
    _fed->stop();
  }
  
  void Fed9UDevice::sendSoftwareTrigger() {
    _fed->sendSoftwareTrigger();
  }
  
    u8 Fed9UDevice::hasEvent() {
    return _fed->hasEvent();
  }

  void Fed9UDevice::getCompleteEvent(u32* buffer, u32 bufferSize, u32& numU32sAddedToBuffer, u32& eventCount, bool blockDisable) {
    _fed->getCompleteEvent(buffer, bufferSize, numU32sAddedToBuffer, eventCount, blockDisable);
  }
  */
  void Fed9UDevice::softReadoutLoop() {
    
    //Used to make all writes to our thread safe gFed9UOut occur in one operation and hence be atomic.
    std::ostringstream mess;

    u32 buffersize = 20*1024;
    u32* buffer = new u32[buffersize];
    _isTakingData = true;
    i32 triggers = 0;
    u32 eventCounter = 0;
    start();
    
    while(_isTakingData)
      {
	//u32 frames;
	if (triggers > 0) {
	  mess<<"_fed->sendSoftwareTrigger() decrementing trigger counter..."<<endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
	  mess.str("");
	  triggers--;
	  
	} 
	if(triggers == 0) {
	  //A cin would cause trouble in multithreaded environment, will come back to this.
	  std::cout << "Number of triggers to send? (0 = exit)" << endl;
	  cin >> triggers;
	  if (triggers <= 0) { _isTakingData = false; break; }
	}
	
	
	
	sendSoftwareTrigger();
	
	u16 ev=0;
	while (!(ev=hasEvent()) && _isTakingData) {//((_fed->getQdrFrameCount(frames), frames <= 0) && _isTakingData) {
	  
	  fed9Uwait(10);
	}
	
	if (!_isTakingData) break;
	mess << "Found an event!" << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
	mess.str("");
	u32 size = 0;
	u32 eventCount=0;
	try 
	  {
	    getCompleteEvent(buffer, size, buffersize, eventCount, !_blockModeReadout );
#ifdef EVENT_STREAMLINE
            // <NAC date="23/05/2007"> added .get()
	    Fed9UEventStreamLine event(buffer,_fed9UDescription.get());
#else
	    Fed9UEvent event(buffer,_fed9UDescription.get(), size);
            // </NAC>
#endif
	    mess << event << endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
	    mess.str("");
	    if ( event.getEventNumber() != eventCounter || event.getEventNumber() != eventCount - 1 ) {
	      mess << "\n\n\n******************event number mismatch!!! event number = " << event.getEventNumber() << "\n\n\n***************************" << endl;
	      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
	      mess.str("");
	      break;
	    }
	    eventCounter++;
	    ostringstream fname;
	    fname << "/home/xdaq/data/testevent" << _eventNumber++;
	    ofstream ofile(fname.str().c_str());
	    ICUTILS_VERIFYX(ofile.is_open(), Fed9UDeviceException)(fname.str()).msg("Unable to open data file for writing events read out from FED");
	    for (u32 i = 0; i < size; i++) 
	      {
		ofile<<'\t'<< hex << setfill('0') << setw(8) << buffer[i] << '\n';
	      }
	    
	    ofile.close();
	  } catch (const Fed9UVmeDeviceException& e) {
	    //if (e.getErrorCode() == Fed9UVmeDeviceException::ERROR_BUFSMALL) {
	    //  delete[] buffer;
	    //  buffersize *= 2;
	    //  buffer = new u32[buffersize];
	    //} else {
	    stop();  
	    delete[] buffer;
	    RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_READOUT, "Fed9UDevice::softReadoutLoop() failed to read event"));
	    //}
	  }

      }
    stop();
    delete[] buffer;
  }
  void Fed9UDevice::getCompleteBufferedEvent( Fed9UBufferedEvent & bev ) {
    
    //
    // generate temporary buffer for data.
    //

    u32 bufLen=0; 

    bufLen = theLocalFedSettings.getRequiredBufferSize();
    std::ostringstream mess;
    mess<<"\n\nINFO:  Fed9UDevice::getCompleteBufferedEvent reserving "
        <<bufLen<<" u32s.\n\n"<<endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
    mess.str("");
    u32 * tempBuf=new u32 [bufLen];
    
    //
    // transfer data from fed to local buffer
    //

    u32 numU32sAddedToBuffer=0;
    u32 eventCount=0;
    try{
      if (_blockModeReadout) {
	getCompleteEventTurboStyle(tempBuf, bufLen, numU32sAddedToBuffer, eventCount);
      } else {
	getCompleteEvent(tempBuf, bufLen, numU32sAddedToBuffer, eventCount, 1);
      }
	bev.loadEvent(tempBuf, &theLocalFedSettings, numU32sAddedToBuffer);
    }catch(ICUtils::ICException & exc){
      delete [] tempBuf;
      throw exc;
    }
    
    //
    // clean up dynamic allocation
    //

    delete [] tempBuf;
  }

  void Fed9UDevice::ControlCHandler(int) {
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << "Control-C detected, stopping run" << endl;
    _isTakingData = false;
    fed9Uwait(1000);
    exit(0);
    //kill(0,SIGKILL);
  }
  //x;
  volatile bool Fed9UDevice::_isTakingData = false;


  /**
   * The soak test will write to all read/write registers on the FED and then check
   * that the write was successful.
   *
   * If any errors are found they will be written to the log file. No User action is
   * required. The method takes the number of different description files to be tested.
   * The description files are generated by the class Fed9UCreateDescription. Testing
   * the EPROM doubles the time taken to run the soak test and the User is given the option
   * not to test it. It is defaulted to true, which will test the EPROM. If it is false
   * the EPROM will not be tested.
   */
  Fed9UDevice& Fed9UDevice::soakTest(u32 numberOfTests, bool testEprom) {
    try {
      Fed9UCreateDescription createDescription(0);

      u16 fedSerialNumber = 0;
      if (testEprom) {
	//Saves the serial number as this will be lost during the test.
	//Also the readOnlyLevel is set to none to allow all the EPROM
	//to be tested.
	Fed9UEpromReadOnlyLevel readOnlyLevel( FED9U_EPROM_NONE );
	setEpromReadOnlyLevel(readOnlyLevel);
	fedSerialNumber = getFedSerialNumber();
      }

      for (u32 testNumber = 0; testNumber < numberOfTests; ++testNumber) {
	try {
	  gFed9ULog.stamp();
	  Fed9UMessage<Fed9ULogFile>(FED9U_LOG_LEVEL_DEBUG) << endl << "Start of Fed9UDevice::soakTest number " << testNumber << endl;
	  Fed9UDescription fedDescription( createDescription.makeFed9UDescription(getFed9UDeviceDescription().getCrateNumber(), getFed9UDeviceDescription().getBaseAddress(), getFed9UDeviceDescription().getHalAddressTable()) );

	  //Don't init FED.
	  setFed9UVMEDeviceDescription(fedDescription, false);

	  init().initStrips().initAuxiliaryChips();
	  if (testEprom) {
	    setEprom( fedDescription.getEprom() );
	  }
	  start();


	  frontEndCheck();
	  backEndCheck();
	  vmeCheck();
	  higherLevelCheck();
	  ttcrxCheck();
	  if (testEprom) {
	    epromCheck();
	  }

	  sendFedReset();

	  //Get the description that was used for this run and write it to a .fed file.
	  //Set the output file name.
	  ostringstream outputFedFile;
	  outputFedFile << "../Fed9UDescriptionFiles/soakTestDescriptions/soakTestRun"
			<< testNumber << ".fed";
	  //Create an ofstream which is required to save the file in a .fed format.
	  ofstream fileo( outputFedFile.str().c_str() );
	  //Get the copy of the Fed9UDescription used during the test.
	  getFed9UVMEDeviceDescription().saveDescription(fileo);
	  //Close the output file.
	  fileo.close();

	  gFed9ULog.stamp();
	  Fed9UMessage<Fed9ULogFile>(FED9U_LOG_LEVEL_DEBUG) << "End of Fed9UDevice::soakTest number " << testNumber << endl;
	}
	catch (const ICUtils::ICException& e) {
	  gFed9ULog.stamp();
	  Fed9UMessage<Fed9ULogFile>(FED9U_LOG_LEVEL_DEBUG) << "Fed9UDevice::soakTest number " << testNumber << " aborted due to ICUtils::ICException error:-\n" << e.what() << endl;
	}
	catch (const std::exception& e) {
	  gFed9ULog.stamp();
	  Fed9UMessage<Fed9ULogFile>(FED9U_LOG_LEVEL_DEBUG) << "Fed9UDevice::soakTest number " << testNumber << " aborted due to std::exception error:-\n" << e.what() << endl;
	}
      }

      if (testEprom) {
	setFedSerialNumber(fedSerialNumber);
      }
      std::ostringstream mess;
      mess << "End of test. Tested " << numberOfTests << " description files." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
      mess.str("");
      return *this;
    }
    catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_SOAK_TEST, ""));
    }
  }


  void Fed9UDevice::trimDACInternalCalibration(u16 mode, u16 UpperThresh, u16 LowerThresh)
  {
    //
    // variables that may become arguments.
    //

    //This is used to ensure that the whole message we want to write is sent to the gFed9UOut in one operation.
    //gFed9UOut provides atomic writes to its stream, however successive writes are not hence we need everything
    //written in one stream operation. Must make sure it is clear after each write to allow reuse.
    std::ostringstream message;
    message << std::dec;

    u16 Rload=theLocalFedSettings.getOptoRXResistor();
    message << "RLoad resistor value set to " << Rload << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << message.str();
    message.str("");
    u16 TempScopeLen=70;

    u16 uThresh=UpperThresh;
    u16 lThresh=LowerThresh;
    
    //
    // let's do this in parallel for some speed...      
    //

    //
    // need software triggers, scope mode capture, and FED enabled...
    //     -> back up original settings first...
    //

    Fed9UTrigSource FedTrig=theLocalFedSettings.getTriggerSource ();
    Fed9UDaqMode DaqMode=theLocalFedSettings.getDaqMode();
    Fed9UDaqSuperMode SuperDaqMode=theLocalFedSettings.getDaqSuperMode();
    Fed9UReadRoute ReadRoute=getBeFpgaReadRoute();
    u16 ScopeLen=theLocalFedSettings.getScopeLength();

    // 
    // set the required config.
    //

    setBeFpgaReadRoute(FED9U_ROUTE_VME);
    setTriggerSource(FED9U_TRIG_SOFTWARE);
    setDaqMode(FED9U_MODE_SCOPE);
    setDaqSuperMode(FED9U_SUPER_MODE_NORMAL);
    setScopeLength(TempScopeLen);
    
    start();

    
    //
    // storage for TrimDAC offsets + offset inititalisation
    //
    vector <u16> offsets(CHANNELS_PER_FED);
    vector <u16> oldOffsets(CHANNELS_PER_FED);
    Fed9UAddress addr;
    for(u16 feNo0=0; feNo0<FEUNITS_PER_FED; feNo0++){
      u16 Xpre=0;
      u16 Xpost=0;
      addr.setFedFeUnit(static_cast<u8>(feNo0));
      //      separateOptoRXOffsets(_fed9UDescription->getOptoRxOffset(addr), Xpre, Xpost);
      Xpre = _fed9UDescription->getOptoRxInputOffset(addr);
      Xpost = _fed9UDescription->getOptoRxOutputOffset(addr);
      u16 startOffset=estimateTrimDACStartPoint(Xpre, Xpost, Rload);
      message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"INFO: Initialisation : "<<static_cast<int>(Fed9UAddress().setFedFeUnit(feNo0).getExternalFedFeUnit())<<" = "<<startOffset<<endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
      message.str("");
      for(u16 chNo=feNo0*CHANNELS_PER_FEUNIT; chNo<(feNo0+1)*CHANNELS_PER_FEUNIT; chNo++){
        offsets[chNo]=startOffset;
        message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG:offsets[chNo]= "<<offsets[chNo]<<endl; 
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
	if(mode==1){
	  Fed9UAddress a;
	  a.setFedChannel(static_cast<u8>(chNo));
	  oldOffsets[chNo]=_fed9UDescription->getTrimDacOffset(a);
	}

      }
    }
 
    //    
    // Need a channel status container 
    // with information about which channels need to be 
    // calibrated 
    //            - bit field 0 holds 1 if channel has completed.
    //		  - bit field 1 holds 1 if channel is disabled            
    //            - bit field 2 holds 1 if an error occured on this channel
    //
    const u16 CHSTAT_DOME=0;
    const u16 CHSTAT_COMPLETED=1;
    const u16 CHSTAT_DISABLED=2;
    const u16 CHSTAT_ERROR=4;
    //? const u16 CHSTAT_IGNORE=8;? //need this one?

    
    vector<u16> channelStatus(CHANNELS_PER_FED); 
    message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: channelStatus.size() = "<<channelStatus.size()<<endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
    message.str("");
    for(u16 feNo1=0; feNo1<FEUNITS_PER_FED; feNo1++){
      addr.setFedFeUnit(static_cast<u8>(feNo1));
      bool isDisabled=_fed9UDescription->getFedFeUnitDisable(addr);
      if(!isDisabled) {
	message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"INFO: FE "<< static_cast<int>(Fed9UAddress().setFedFeUnit(feNo1).getExternalFedFeUnit())<<" is enabled."<<endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
      }
      for(u16 chNo=feNo1*CHANNELS_PER_FEUNIT; chNo<(feNo1+1)*CHANNELS_PER_FEUNIT; chNo++){
        if(isDisabled){
          channelStatus[chNo]=CHSTAT_DISABLED;
        }else{ 
          channelStatus[chNo]=CHSTAT_DOME;
        }
        message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: channelStatus = "<<channelStatus[chNo]<<endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
      }
    }
    message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"INFO: setup and init complete, beginning run."<<endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
    message.str("");

    //
    //////////////////////////////////////////////////////// 
    // setup has finished, begin calibration loop         //
    ////////////////////////////////////////////////////////
    //
    
    u16 loopCnt=0;
    while((trimDACCalibrationHasFinished(channelStatus)&0x1)){
      message<<endl<<loopCnt<<endl;
      message << "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<< "in while loop inside trim dac cal routine!!! loop count = " << loopCnt <<endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
      message.str("");
      loopCnt++;
      if(loopCnt>256){// have to place a limit on the convergence.

	// In the case where some channels do not converge, we flag these channels as in ERROR and then we allow the calibration to finish.
        message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"void Fed9UDevice::trimDACInternalCalibration() is unable to complete on some channels."
	       <<endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << message.str();
	message.str("");

	_errorStr << FED9U_FUNCTION<<" "<<__LINE__<< "void Fed9UDevice::trimDACInternalCalibration() has failed to converge on some channels, these channels have been disabled in the description. The following channels failed to Converge: " << endl;

	for(u16 i=0; i<CHANNELS_PER_FED; i++){
	  //
	  // if channel is enabled, and channel has not completed,
	  // set the error bit in the channelStatus container.
	  //
	  if(!(channelStatus[i]&CHSTAT_COMPLETED) && !(channelStatus[i]&CHSTAT_DISABLED)) {
	    channelStatus[i]|=CHSTAT_ERROR;// set the error bit
	    _errorStr<< FED9U_FUNCTION<<" "<<__LINE__ << "Setting Error bit for Fed Channel Number " << static_cast<u16>(Fed9UAddress().setFedChannel(i).getExternalFedChannel()) << endl; 
	  }
	}
	
	//<JRF date="14/03/2008">Following code no longer needed since we now count channels in error as completed, and allow the trimDAC calibration to complete properly.
	// we then disable any failed channels in the description for further use.
	/*    
	      stop();
	      
	      setTriggerSource(FedTrig);
	      setDaqMode(DaqMode);
	      setDaqSuperMode(SuperDaqMode);
	      setBeFpgaReadRoute(ReadRoute);
	      setScopeLength(ScopeLen);
	      
	      return; 
	*/
	//</JRF>
      }
     

      //
      // numbers for TrimDAC Offsets have been estimated, 
      //  -> apply them to the FED
      // 
      for(u16 chNo=0; chNo<CHANNELS_PER_FED; chNo++){
	// ignore disabled channels for more speed....
	message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: channelStatus "<<static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel())<<" = "<<channelStatus[chNo]<<endl;
	if(channelStatus[chNo]&CHSTAT_DISABLED){
	  message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: Setting TrimDAC: Ignoring channel "<<static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel())<<endl;
	  continue;
	}
	if(channelStatus[chNo]&CHSTAT_COMPLETED){
	  // ignore completed channels for more speed....
	  message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: Setting TrimDAC: channel has finished "<<static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel())<<endl;
	  continue;
	}
	if(channelStatus[chNo]&CHSTAT_ERROR){
	  // ignore channels with errors....
	  message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: Setting TrimDAC: channel has failed, channel set into error state and ignored! "<<static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel())<<endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<message.str();
	  continue;
	}
	message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"DBG: Setting TrimDAC: Applying to channel "<<static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel())<<endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
	addr.setFedChannel(static_cast<u8>(chNo));//bug fixed (11-chNo%12)+12*(chNo/12));
        try {
          setTrimDacOffset(addr, offsets[chNo]);
        } catch (ICUtils::ICException & exc) {
          _errorStr << FED9U_FUNCTION<<" "<<__LINE__<< " Failed while attempting to calibrate TrimDAC Setting for channel " << static_cast<int>(Fed9UAddress().setFedChannel(chNo).getExternalFedChannel()) 
		    << ". Disabling this channel in the description, please check the error and fix this channel before repeating the trim DAC calibration" << "\n" << exc.what() << "\n";
	  channelStatus[chNo] = CHSTAT_ERROR; // set this channel to error  to avoid waisting time trying to set it.
        }
      }
      //<JRF date="7/03/2008">
      //Added a wait here to allow ADC level to settle after an adjustment of trimDAC value ( this bug caused channel 4 to produce bad trimdac cal values from time to time, and 
      //also caused the threaded version of the trim dac calibration to produce untrustworthy results
      fed9Uwait(1000);
      //</JRF>

      //
      // collect and check the offset data 
      // 
      Fed9UBufferedEvent ev;
      getTrimDACCalibrationEvent(ev);

      //
      // process offset data
      // 

      // need this one because Fed9UEvent contains no info about disabled channels
      u16 eventCh=0;
      for(u16 ch=0; ch<CHANNELS_PER_FED; ch++){

	// if any bits set, look at what they are...        
        if(channelStatus[ch]){
          message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"INFO: channel is disabled or has finished: "<<ch<<endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	  message.str("");
          if(channelStatus[ch]&CHSTAT_COMPLETED || channelStatus[ch]&CHSTAT_ERROR)
            eventCh++;
          continue;
        }
        
        
        message << "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<< "INFO: channel will be processed: " << ch << endl;
        Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << message.str();
	message.str("");
        //
        // calculate the mean of the capture
        //
        vector<u16> data(TempScopeLen);
#ifdef EVENT_STREAMLINE
	u32 feUnitChannel = 0;
	//<JEC date=27/4/07> bug fix in input of eventCh into getFeUnit
	//<JEC date=1/6/07> tidied up conversion from internal to external numbering scheme
	u32 externalChannelNumber = static_cast<u32>(Fed9UAddress().setFedChannel(eventCh).getExternalFedChannel());
	//	Fed9UEventUnitStreamLine feUnit = ev.getFeUnit(static_cast<u32>(96-eventCh),feUnitChannel);
	Fed9UEventUnitStreamLine feUnit = ev.getFeUnit(externalChannelNumber,feUnitChannel);
	//</JEC>
	feUnit.getSample(feUnitChannel, &data[0] );
	//</JEC>
#else
        ev.channel(eventCh).getSamples( &data[0] );
#endif
        message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"Processing a sample set of "<<data.size()<<" for channel "<<eventCh<<endl;
        u16 u16Mean=getTrimDACCalibrationChannelMean(data);
        //if(eventCh>8)
	message<<"| "<<offsets[ch]<<", "<<u16Mean<<'\t';
        message<<u16Mean<<'\t';
        //if(ch==37)
	message<<"channel = "<<ch<<"   uMean = "<<u16Mean<<" eventCh = "<<eventCh<<endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
        //
        // calculated the mean, act upon it
        //
        if(u16Mean<uThresh){ // approach from the top
          
          if(u16Mean<lThresh){ // under-run? should never happen, but...
            message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<<"Calibation Under-run on channel "<<ch
		   <<".\nTry making the target window a little larger"<<endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	    message.str("");
            offsets[ch]--;
            eventCh++;
            continue;
            //channelStatus[eventCh]|=0x1;
          }
          message<< "Crate " << _fed9UDescription->getCrateNumber() << ", slot " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", FED ID "  << _fed9UDescription->getFedId()<<dec<< ", Ch "<< static_cast<int>(Fed9UAddress().setFedChannel(ch).getExternalFedChannel() )<<" finished. TrimDAC level "<<offsets[ch]
		 <<"  Mean signal level  = "<<u16Mean<<endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << message.str();
	  message.str("");
          channelStatus[ch]|=0x1;// set complete flag in status container
          eventCh++;
          continue;
        }else{
          // to start with, but a bit inefficient.
          //    -> how about a direct calculation or binary search?
          if((u16Mean-uThresh)>50){
            //if(ch==45)
	    message<< "Crate " << _fed9UDescription->getCrateNumber() << ", Slot " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", FED ID  "  << _fed9UDescription->getFedId()<<", INFO: loop"<<loopCnt<<" (u16Mean-uThresh)= "
		   <<u16Mean-uThresh<<" offsets[ch] "
		   <<offsets[ch]
		   <<" -> "<<((u16Mean-uThresh)>>3)<<endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	    message.str("");
            offsets[ch]+=((u16Mean-uThresh)>>3);
            
            u16 residue=(u16Mean-uThresh)&0x7;
            if(residue>4)
              offsets[ch]++;
            eventCh++;
            continue;            
          }
	  
          offsets[ch]++;
          eventCh++;
          continue;
        }
      }
      
    }// while(!(trimDACCalibrationHasFinished(channelStatus)&0x1))
    

    message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"\n"<< "finished while loop!!!" << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
    message.str("");
    stop();
    
    //
    // Need to put these data into the Fed9UDescription depending
    // on the value of mode passed to the function.
    //
    
    //    std::cout << "\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!! MODE = " << (u16)mode << "\n\n\n"  << std::endl;
    if(mode==0){

      


      for(u8 chan=0; chan<CHANNELS_PER_FED; chan++){
	Fed9UAddress a;
	a.setFedChannel(chan);
	//std::cout <<"!channelStatus[chan]&CHSTAT_DISABLED && channelStatus[chan]&CHSTAT_COMPLETED = " 
	//	  <<  ((!( (channelStatus[chan]&CHSTAT_DISABLED)>>1)  && (channelStatus[chan]&CHSTAT_COMPLETED))?"true":"false") 
	//	  << " channelStatus for channel id = " << (u16)chan << " is " << (u16)channelStatus[chan] << std::endl;
	if( !( (channelStatus[chan]&CHSTAT_DISABLED)>>1)  && (channelStatus[chan]&CHSTAT_COMPLETED)) {
	  try {
	    //std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Setting final value for channel " << (u16)chan << ", TriDAC = " << (u16)offsets[chan] << std::endl;
            setTrimDacOffset(a, offsets[chan]);
          } catch (ICUtils::ICException & exc) {
            _errorStr<< FED9U_FUNCTION<<" "<<__LINE__ << " Failed while setting final TrimDAC Setting for channel " <<  chan 
		      << ". " << "\n" << exc.what() << "\n";
          }
	}
	else if(!((channelStatus[chan]&CHSTAT_DISABLED)>>1) && ((channelStatus[chan]&CHSTAT_ERROR)>>2)) {
	  try {
	    //_errorStr<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Setting trim dac offset to 0 for failed channel!!!" << (u16)chan << std::endl;
            setTrimDacOffset(a, 0); //Set the trim dac offset to 0 for channels which failed!
	    setApvDisable(a.setChannelApv(0), true);
	    setApvDisable(a.setChannelApv(1), true);
	    //std::cout << "getApvDisable() for channel " << (u16)chan << " gives value " << (getApvDisable(a.setChannelApv(1))?"true":"false") <<  std::endl;
          } catch (ICUtils::ICException & exc) {
            _errorStr << FED9U_FUNCTION<<" "<<__LINE__<< " Failed while disabling channels which failed TrimDAC Setting for channel " <<  chan 
		      << ". " << "\n" << exc.what() << "\n";
	    setApvDisable(a.setChannelApv(0), true);
	    setApvDisable(a.setChannelApv(1), true);
          }
	}
	else
	  continue;
      }
    }
    if(mode==1){
      for(u8 chan=0; chan<CHANNELS_PER_FED; chan++){
	Fed9UAddress a;
	a.setFedChannel(chan);
	// restore the FED to the pre-calibration state.
	if( !( (channelStatus[chan]&CHSTAT_DISABLED)>>1)  && (channelStatus[chan]&CHSTAT_COMPLETED)) {
	  try {
	    //std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Setting final value for channel " << (u16)chan << ", TriDAC = " << (u16)offsets[chan] << std::endl;
            setTrimDacOffset(a, oldOffsets[chan]);
          } catch (ICUtils::ICException & exc) {
            _errorStr << FED9U_FUNCTION<<" "<<__LINE__<< "Un able to complete trim dac calibration, "
		      << "Failed while trying to return the TrimDAC settings to the original configuration."
		      << chan
		      << "\n" << exc.what() << "\n";
	    //            RETHROW(exc, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_TRIM_DAC_CAL, _errorStr.str().c_str() ) );
          }
          // store the results in the Fed9UDevice description.
	  _fed9UDescription->setTrimDacOffset(a, offsets[chan]);
	}else if(!((channelStatus[chan]&CHSTAT_DISABLED)>>1) && ((channelStatus[chan]&CHSTAT_ERROR)>>2)) {
	  try {
	    setTrimDacOffset(a, 0); //Set the trim dac offset to 0 for channels which failed!
	    setApvDisable(a.setChannelApv(0), true);
	    setApvDisable(a.setChannelApv(1), true);
	    _fed9UDescription->setTrimDacOffset(a, 0); //Set the trim dac offset to 0 for channels which failed!
	    _fed9UDescription->setApvDisable(a.setChannelApv(0), true);
	    _fed9UDescription->setApvDisable(a.setChannelApv(1), true);
          } catch (ICUtils::ICException & exc) {
            _errorStr<< FED9U_FUNCTION<<" "<<__LINE__<< " Failed while disabling channels which failed TrimDAC Setting for channel " <<  chan 
		      << ". " << "\n" << exc.what() << "\n";
	    setApvDisable(a.setChannelApv(0), true);
	    setApvDisable(a.setChannelApv(1), true);
	    _fed9UDescription->setApvDisable(a.setChannelApv(0), true);
	    _fed9UDescription->setApvDisable(a.setChannelApv(1), true);
          }
	}
	else{
	  continue;
	}
      }
    }

    //
    // i think the restore can go here:
    //  -> _fed9UDescription shouldn't have been changed. 
    //  -> theLocalFedSettings should be restored whatever the value of mode.
    //    

    setTriggerSource(FedTrig);
    setDaqMode(DaqMode);
    setDaqSuperMode(SuperDaqMode);
    setBeFpgaReadRoute(ReadRoute);
    setScopeLength(ScopeLen);

    if ( _errorStr.str().length() > 0) {
      _errorStr << FED9U_FUNCTION<<" "<<__LINE__<<  "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId() <<  "Un able to complete trim dac calibration, "
		<< "please check that all inputs to the FED are switched off "
		<< "(laser drivers are off), and that all faulty channels are "
		<< "disabled.\n";
      message<< "FED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId() <<"!!!!!TrimDAC Calibration Completed With Errors:"<<   endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<message.str();
      message.str("");
      
      THROW( Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_TRIM_DAC_CAL, _errorStr.str().c_str() ) );
    }
    message<< "\nFED in Crate " << _fed9UDescription->getCrateNumber() << ", In slot number " <<  static_cast<u16>(_fed9UDescription->getSlotNumber()) << ", with FED software ID  "  << _fed9UDescription->getFedId()<<"TrimDAC Calibration Completed."<< endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<message.str();
    message.str("");


  }// void Fed9UDevice::trimDACInternalCalibration()






  ////////////////////////////////////////////////////////////////////////
  u16 Fed9UDevice::estimateTrimDACStartPoint(u16 Xpre, u16 Xpost, u16 Rload)
  {
    // mask input values to prevent silly output
    Xpre&=0xf;
    Xpost&=0x3;
    std::ostringstream message;
    message<<"DBG: TrimDAC estimate : "<<Xpre<<" "<<Xpost<<" "<<Rload<<endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
    message.str("");

    // evaulate OptoRX offset output
    // multiply by load resistor -> voltage
    // divide by the no mV/DAC count -> TrimDAC equivilant count
    float temp=((8.5+2.5*static_cast<float>(Xpost)-static_cast<float>(Xpre))*Rload)/8;
    return static_cast<u16>(temp) & 0xff;
  }



  ////////////////////////////////////////////////////////////////////////
  void Fed9UDevice::separateOptoRXOffsets(u16 combined, u16 & Xpre, u16 & Xpost)
  {
    Xpre=combined&0xf;
    Xpost=(combined>>4)&0x3;
  }

  ////////////////////////////////////////////////////////////////////////
  u8 Fed9UDevice::trimDACCalibrationHasFinished(const vector<u16> & status)
  {
    u8 hasIt=0;
    for(vector<u16>::const_iterator i=status.begin(); i!=status.end(); i++){
      if((*i)&0x2)
        continue;
      
      hasIt|=( ((*i)&0x1) | ( ( (*i)&0x4)>>2 ) )?0:1;
      
      if ((*i)==0x4)
	std::cout << " *i = " << (*i) << "hasIt = " << (u16)hasIt << std::endl;
    }
    std::cout << " Final hasIt = " << (u16)hasIt << std::endl;
    return hasIt;
  }
  
  ////////////////////////////////////////////////////////////////////////
  void Fed9UDevice::getTrimDACCalibrationEvent(Fed9UBufferedEvent & ev)
  {
   
    try{
      std::ostringstream message; 
      sendSoftwareTrigger();
      u32 timeout = 0;
      while(!hasEvent() & timeout < 1000){ // this timeout was adjusted by hand to make sure it is a reasonable amount of time to wait for the fed to get the event.
	timeout+=1;
	//fed9Uwait(10);
	message << "stuck in has event while loop!" << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED)<<message.str();
	message.str("");
      }
      if (timeout >= 100000) {
	message << "about to chuck a wobbly!!!" << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<message.str();
	message.str("");
	THROW( Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_TRIM_DAC_CAL, "Unable to take events from FED!!! Check Firmware and Hardware of this FED!!!" ) );
      }
      getCompleteBufferedEvent(ev);
      ev.checkEvent();
    }catch(ICUtils::ICException & exc){
      throw exc;
    }
  }
  
  ////////////////////////////////////////////////////////////////////////
  u16 Fed9UDevice::getTrimDACCalibrationChannelMean(vector<u16> & data)
  {
    std::ostringstream message;
    double mean=0.0;
    for(vector <u16>::const_iterator i=data.begin(); i!=data.end(); i++)
      mean+=*i;
    mean/=data.size();
    return static_cast<u16>(mean);
  }
  
  u32 Fed9UDevice::purgeEvents() noexcept(false) {
     cout << "Entering Fed9UDevice::PurgeEvents() in FED ID : " << getFedId() << endl;
     try {
      std::ostringstream mess;
        u32 countTheRealCount(0);
      u32 firstEventCount = getCounters().qdrFrameCount;
      bool stopAtEnd(false);
      Fed9UCounters counters, oldCounters;
      if (!_isStarted) {
        try {
          start();
        } catch ( const ICUtils::ICException& e ) {
          // we ignore this cause it's probably just cause there are events in the buffer!!!
        }
          stopAtEnd = true;
      }
      // have to set the readroute to SLINK and then return it aftewards. 
      Fed9UReadRoute currentReadRoute = getBeFpgaReadRoute();
      setBeFpgaReadRoute(FED9U_ROUTE_SLINK64);
      setTestRegister(theLocalFedSettings.getTestRegister() | 0x02); // this forces the fed to ignore backpressure and therefore purge all events
      setBeFpgaReadRoute(currentReadRoute);
      oldCounters = getCounters();
      do {
        counters = getCounters();
        mess << "!!!!!!!!!Purging all events in the FED! QDR count = " << counters.qdrFrameCount  << std::endl;
        Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
        mess.str("");
        // this is a sort of time out, we check that if the frames are not being reduced then we stop the loop
        if (oldCounters.qdrFrameCount==counters.qdrFrameCount) 
          countTheRealCount ++;
        else
          countTheRealCount = 0;

        //double check that triggers are not still being sent
        if (oldCounters.qdrFrameCount < counters.qdrFrameCount)
          break;

        oldCounters = counters;
        if (countTheRealCount%1000 == 0)
           cout<< "countTheREalCount = " << countTheRealCount << endl;

      } while (counters.qdrFrameCount && countTheRealCount < 1000);

      if (oldCounters.qdrFrameCount < counters.qdrFrameCount) {	
	mess.str("");
	mess << "!!!!!!!!!triggers still being sent during purge!!!!!!! QDR count = " << counters.qdrFrameCount  << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
      }
      if (countTheRealCount >= 1000) {	
	mess.str("");
	mess << "!!!!!!!!!timed out on purge!!!!!! QDR count = " << counters.qdrFrameCount  << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << mess.str();
      }
      setTestRegister(theLocalFedSettings.getTestRegister() & 0xFD); // must return this to 0 so fed runs correctly afterwards

      if (stopAtEnd)
	stop();

      return firstEventCount;
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
  
  // <NAC date="30/04/2009">
  // <NAC date="10/02/2010"> added run number to spy data
  // <NAC date="15/02/2010"> added 1/N mode
  boost::shared_array<u8> Fed9UDevice::readCompleteSpyEvent(const std::vector<bool>& delayChipsEnabled, const u32 lvl1ID, const uint32_t runNumber, 
                                                            const bool allowMixedEvent,
                                                            u32* pEventCountBeforeRead, u32* pL1ACountBeforeRead,
                                                            const bool oneOverNMode) noexcept(false)
  // </NAC>
  // </NAC>
  {
      //
      // Check which delay chips are enabled, calculate buffer size and mask
      //
      u32 delayChipMask = 0x00000000;
      u8 nEnabledDelayChips = 0;
      ICUTILS_VERIFYX(delayChipsEnabled.size() == DELAY_CHIPS_PER_FED, Fed9UDeviceException).error();
      for (u8 delayChipIndex = 0; delayChipIndex < DELAY_CHIPS_PER_FED; delayChipIndex++) {
        if (delayChipsEnabled.at(delayChipIndex)) {
          ++nEnabledDelayChips;
          delayChipMask |= (0x1<<delayChipIndex);
        }
      }
      //delay chip buffers + 5 64 bit words (DAQ header + trailer, TkSpecial header, spy header, counters)
      u32 bufferSizeInBytes = SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES*nEnabledDelayChips + 5*8;
      
      //
      // Allocate memory
      //
      boost::shared_array<u8> buffer;
      try {
          buffer.reset(new u8[bufferSizeInBytes]);
      } catch (const std::exception& e) {
          RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_MEMORY, "Failed to allocate buffer for spy event."));
      }
      
      //
      // Set delay chip mask, version ID and reserved bits in header
      //
      buffer[16] = (delayChipMask & 0x000000FF);
      buffer[17] = ((delayChipMask & 0x0000FF00) >> 8);
      buffer[18] = ((delayChipMask & 0x00FF0000) >> 16);
      // <NAC date="10/02/2010"> added run number to spy data
      // <NAC date="17/11/2009"> Added version ID to determine where data starts (see comment below: Fixed bug where this was still set to 24)
      //version ID. Change if an incompatible change is made to the packing of the buffer
      //Version 2: added run number
      buffer[19] = 0x02;
      // </NAC>
      buffer[20] = ( (runNumber & 0x000000FF) );
      buffer[21] = ( (runNumber & 0x0000FF00) >> 8);
      buffer[22] = ( (runNumber & 0x00FF0000) >> 16);
      buffer[23] = ( (runNumber & 0xFF000000) >> 24);
      // </NAC>

      //
      // Check event and L1A count before read and add to buffer
      //
      u32 eventCountBeforeRead;
      try {
          if (oneOverNMode) eventCountBeforeRead = getQdrTotalFrameCount();
          else eventCountBeforeRead = getSpyTotalEventCount();
      } catch (const ICUtils::ICException& e) {
          RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy total event count register."));
      }
      u32 l1aCountBeforeRead;
      try {
          if (oneOverNMode) l1aCountBeforeRead = getL1AcceptID();
          else l1aCountBeforeRead = getSpyL1ACount();
      } catch (const ICUtils::ICException& e) {
          RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy L1A count register."));
      }
      buffer[24] = (eventCountBeforeRead & 0x000000FF);
      buffer[25] = ((eventCountBeforeRead & 0x0000FF00) >> 8);
      buffer[26] = ((eventCountBeforeRead & 0x00FF0000) >> 16);
      buffer[27] = ((eventCountBeforeRead & 0xFF000000) >> 24);
      buffer[28] = (l1aCountBeforeRead & 0x000000FF);
      buffer[29] = ((l1aCountBeforeRead & 0x0000FF00) >> 8);
      buffer[30] = ((l1aCountBeforeRead & 0x00FF0000) >> 16);
      buffer[31] = ((l1aCountBeforeRead & 0xFF000000) >> 24);

      //
      // Read the data
      //
      // <NAC date="17/11/2009"> Fixed bug where this was still set to 24
      u8* pointerToNextByte = buffer.get()+32; //leave space for DAQ header, TkSpecialHeader and the word containg the initial event count
      // </NAC>
      for (u8 delayChipIndex = 0; delayChipIndex < DELAY_CHIPS_PER_FED; delayChipIndex++) {
          //check if delay chip is supposed to be enabled
          if ( !(delayChipMask & (0x1<<delayChipIndex) ) ) continue;
          Fed9UAddress delayChipAddress;
          delayChipAddress.setFedDelayChip(delayChipIndex);
          //read data
          try {
              fireSpy(delayChipAddress,pointerToNextByte);
          } catch (const Fed9UVmeDeviceException& e) {
              RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_SPY_READOUT, "Error while reading spy channel data from FED."));
          }
          pointerToNextByte += SPY_DELAY_CHIP_PAYLOAD_SIZE_IN_BYTES;
          //add event and L1A count after read
          u32 eventCountAfterDelayChipRead;
          try {
              if (oneOverNMode) eventCountAfterDelayChipRead = getQdrTotalFrameCount();
              else eventCountAfterDelayChipRead = getSpyTotalEventCount();
          } catch (const ICUtils::ICException& e) {
              RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy total event count register."));
          }
          u32 l1aCountAfterDelayChipRead;
          try {
              if (oneOverNMode) l1aCountAfterDelayChipRead = getL1AcceptID();
              else l1aCountAfterDelayChipRead = getSpyL1ACount();
          } catch (const ICUtils::ICException& e) {
              RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy L1A count register."));
          }
          (*pointerToNextByte++) = (eventCountAfterDelayChipRead & 0x000000FF);
          (*pointerToNextByte++) = ((eventCountAfterDelayChipRead & 0x0000FF00) >> 8);
          (*pointerToNextByte++) = ((eventCountAfterDelayChipRead & 0x00FF0000) >> 16);
          (*pointerToNextByte++) = ((eventCountAfterDelayChipRead & 0xFF000000) >> 24);
          (*pointerToNextByte++) = (l1aCountAfterDelayChipRead & 0x000000FF);
          (*pointerToNextByte++) = ((l1aCountAfterDelayChipRead & 0x0000FF00) >> 8);
          (*pointerToNextByte++) = ((l1aCountAfterDelayChipRead & 0x00FF0000) >> 16);
          (*pointerToNextByte++) = ((l1aCountAfterDelayChipRead & 0xFF000000) >> 24);
      }

      //
      // Check total event count again to make sure the event wasn't overwritten
      //
      // <NAC date="15/02/2010"> added 1/N mode
      if (!allowMixedEvent && !oneOverNMode) {
          u32 eventCountAfterRead;
          try {
              eventCountAfterRead = getSpyTotalEventCount();
          } catch (const ICUtils::ICException& e) {
              RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy event count register."));
          }
          u32 l1aCountAfterRead;
          try {
              l1aCountAfterRead = getSpyL1ACount();
          } catch (const ICUtils::ICException& e) {
              RETHROW(e, Fed9UDeviceException(Fed9UDeviceException::ERROR_FED_REGISTER_READ, "Error reading spy L1A count register."));
          }
          ICUTILS_VERIFYX(eventCountBeforeRead == eventCountAfterRead, Fed9UDeviceException).error().msg("The spy channel was re-armed during readout.");
          ICUTILS_VERIFYX(l1aCountBeforeRead == l1aCountAfterRead, Fed9UDeviceException).error().msg("The spy channel was re-armed during readout.");
      }

      //
      // Add DAQ header
      //
      //set the BoE nibble to indicate this is the last fragment (5) and the event type nibble to physics trigger (1)
      buffer[7] = 0x51;
      //set the L1 ID to the supplied value
      buffer[6] = ((lvl1ID & 0x00FF0000) >> 16);
      buffer[5] = ((lvl1ID & 0x0000FF00) >> 8);
      buffer[4] = (lvl1ID & 0x000000FF);
      //set the BX ID to the same thing (can't know what the real one is and it needs to be consistent in all fragments)
      buffer[3] = ((lvl1ID & 0x00000FF0) >> 4);
      buffer[2] = ((lvl1ID & 0x0000000F) << 4);
      //set the source ID to the FED ID
      buffer[2] = ( (buffer[2] & 0xF0) | ((theLocalFedSettings.getFedId() & 0x0F00) >> 8) );
      buffer[1] = (theLocalFedSettings.getFedId() & 0x00FF);
      //set H bit to 0 to indicate this is the last header word, set format version to 0 and set reserved bits to 0
      buffer[0] = 0;

      //
      // Add TkSpecialHeader
      //
      //version identifier
      buffer[15] = 0xED;
      //set invalid header format nibble to invalid value (4), readout mode bits to unused value (111) and real/fake bit to real (0)
      buffer[14] = 0x4E;
      //set APVe Address, APV majority address error bits, FE overflow bits to 0 and FE enable bits to 1
      buffer[13] = 0;
      buffer[12] = 0;
      buffer[11] = 0xFF;
      buffer[10] = 0;
      //set all missing data bits and state flags to 0 in FEDStatus register
      buffer[9] = 0;
      buffer[8] = 0;

      //
      // Add DAQ trailer
      //
      //set end of event fragment nibble
      buffer[bufferSizeInBytes-1] = 0xA0;
      //event length
      buffer[bufferSizeInBytes-2] = (((bufferSizeInBytes/8) & 0x00FF0000) >> 16);
      buffer[bufferSizeInBytes-3] = (((bufferSizeInBytes/8) & 0x0000FF00) >> 8);
      buffer[bufferSizeInBytes-4] = ((bufferSizeInBytes/8) & 0x000000FF);
      //set CRC to 0 for now (needed to calculate CRC)
      buffer[bufferSizeInBytes-5] = 0;
      buffer[bufferSizeInBytes-6] = 0;
      //set S-Link transmission error bit and bad ID at FRL bits to 0, set 2 reserved bits to 0 and set event status nibble to zero
      buffer[bufferSizeInBytes-7] = 0;
      //set the TTS bits to DISCONNECTED1 (0000),
      //set the T bit to 0 to indicate this is the last trailer, set the bad CRC at FRL bit to 0 and set the reserved bits to 0
      buffer[bufferSizeInBytes-8] = 0;
      //calculate the CRC and set it
      const u16 crc = calculateFEDBufferCRC(buffer.get(),bufferSizeInBytes,false);
      buffer[bufferSizeInBytes-5] = ((crc & 0xFF00) >> 8);
      buffer[bufferSizeInBytes-6] = (crc & 0x00FF);

      //
      // Swap to VME order
      //
      u32* bufferU32 = reinterpret_cast<u32*>(buffer.get());
      for (uint32_t i = 0; i < bufferSizeInBytes/8; i++) {
        u32 tmp32 = bufferU32[i*2];
        bufferU32[i*2] = bufferU32[i*2+1];
        bufferU32[i*2+1] = tmp32;
      }

      if (pEventCountBeforeRead) *pEventCountBeforeRead = eventCountBeforeRead;
      if (pL1ACountBeforeRead) *pL1ACountBeforeRead = l1aCountBeforeRead;
      
      return buffer;
  }
  // </NAC>
}
