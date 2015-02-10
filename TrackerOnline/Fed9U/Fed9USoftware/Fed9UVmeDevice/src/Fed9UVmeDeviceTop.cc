#include <inttypes.h>
#include <stdint.h>
#include "Fed9UVmeDevice.hh"
//#include "Fed9UAddress.hh"
//#include "Fed9UEvent.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UWait.hh"
#include "Fed9ULogTemplate.hh"
#include <fstream>
#include <sstream>

/**********************************************************************************
 *                                                                                *
 * This file contains the constructor, destructor and static member functions and *
 * data members, as well as some member functions associated with construction    *
 *                                                                                *
 **********************************************************************************/

namespace Fed9U {

         u32  Fed9UVmeDevice::dummyWord  = 0;
  vector<u32> Fed9UVmeDevice::dummyVector;

  //
  //Mapping of integer nanosecond skews to delay chip settings.
  //
  const u16 Fed9UVmeDevice::_fineSkewMap[25]={ 0,  1,  2,  3,  4,
                                               6,  7,  10, 11, 12,
                                               13, 14, 15, 17, 18,
                                               19, 20, 21, 24, 25,
                                               26, 27, 29, 30, 31};
  //
  //Following based on John's measurements 27/09/2005 for the 6 fine skew settings.
  //
  const u16 Fed9UVmeDevice::_fineSkewMap6Bit[25] = {  0,  2,  4,  6,  8,
						      10, 17, 19, 21, 23,
						      25, 27, 29, 33, 35,
						      37, 39, 41, 43, 45,
						      47, 54, 56, 58, 60 };

  bool Fed9UVmeDevice::handleControlC(false);
  bool Fed9UVmeDevice::controlCCaught(false);

  pthread_mutex_t Fed9UVmeDevice::fileMutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t Fed9UVmeDevice::multiFedAccessMutex = PTHREAD_MUTEX_INITIALIZER;

  std::ios::pos_type Fed9UVmeDevice::startCfDataMarker(-1);
  std::ios::pos_type Fed9UVmeDevice::endCfDataMarker(-1);
  bool Fed9UVmeDevice::cfFileParsed(false);

  u16 Fed9UVmeDevice::firmwareUploadInProcessCount(0);



  Fed9UVmeDevice::Fed9UVmeDevice(const Fed9UDescription& description, u32 adaptorSlot) throw (Fed9UVmeDeviceException) try :
  // <NAC date="23/05/2007"> changed theLocalFedSettings from a reference to real object to prevent memory leak if this constructor throws
  //no extra copying as theLocalFedSettings was initialised from a new Fed9UDescription anyway
  //theLocalFedSettings( *new Fed9UDescription(description) ), _isStarted(false),
  theLocalFedSettings(description), _isStarted(false),
  // </NAC>
    theFed( new Fed9UVmeBase(description.getBaseAddress(), description.getHalAddressTable(), description.getBusAdaptorType(), adaptorSlot) ),
    //theFed( new Fed9UVmeBase(description.getBaseAddress(), description.getHalAddressTable(), description.getBusAdaptorType(), description.getVmeControllerDaisyChainId()) ), 
    timer1(0), timer2(0), timer1Value(0),timer2Value(0), methodCallCounter(0),blockModeUpload_(false),mHaveUploadedNewFirmware(false)
  {
         //dummyWord  = 0;
         //dummyVector = const vector<u32>(0,0);

    _oldHandler = signal(SIGINT, ControlCHandler);
    std::ostringstream msg;
    msg << "Crate Number is set to: " << description.getCrateNumber() << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
    msg.str("");
    // If we have a problem fed and we want to run with it then we set the hardware ID to 999 for emergency! 
    if (theLocalFedSettings.getFedHardwareId() != 999 ) {
      // If we are detecting feds in the crate then we do not know what their HardwareIDs are
      // In this case we must set the hardware Id in the Description rather than verifying it.
      if (theLocalFedSettings.getFedHardwareId() == 0 || theLocalFedSettings.getFedHardwareId() == 998 ) { // these are special ids reserved for crate scan configuration
	// If we arrive here then we are most likely trying to establish what sort of fed we have found
	// in this case we are not sure of the FED version or Firmware versions, or eprom version.
	// first we try to read from eprom version 1 and check to see that the hardwareid we find is between 1 and 17 inclusive.
	theLocalFedSettings.setEpromVersion(1);
	theLocalFedSettings.setFedVersion(1);
    u32 localHardwareId=0;
    localHardwareId = getFedHardwareId();

	if (! (localHardwareId <= 17 && localHardwareId > 0 ) ) {
	  theLocalFedSettings.setEpromVersion(2);
	  theLocalFedSettings.setFedVersion(2);
	  // Assume that we have a version 2 FED so we then read again as version 2 and make sure that it is V2

      localHardwareId = getFedHardwareId();
    
      ICUTILS_VERIFYX ( (localHardwareId > 17 && localHardwareId <= 600 ) ,Fed9UVmeDeviceException)
	    (localHardwareId)(localHardwareId).error().msg("Fed Hardware ID does not seem to be readable as either V1 or V2 FED!!!");
	
	  // we can tell the FED to do something about the firmware differences by uploading the appropriate .ace file by setting the correct hardware id.
	  if (theLocalFedSettings.getFedHardwareId() == 998) {
	    // call this twice because there's a 
	    // local static variable that tracks the 2nd 
	    // call and causes a exception to be raised if the 
	    // first call failed. 
	    verifySystemACEFirmwareVersions();
	    verifySystemACEFirmwareVersions();
	  }
	}
	
	// if we get to here then we have set the correct FED version and Eprom version!!! so now we can continue to update the localFedSettings with 
	// values from the real fed.
	theLocalFedSettings.setFedHardwareId(localHardwareId);
	theLocalFedSettings.setBeFirmwareVersion(getBeFirmwareVersion());
	theLocalFedSettings.setVmeFirmwareVersion(getVmeFirmwareVersion());
	//These can possibly throw an execption if the FE or delay FPGA firmware IDs are out of sync
	//we may be able to correct this, so catch it and give it a go.
	u32 count=10;
    do {
        try {
	        theLocalFedSettings.setFeFirmwareVersion(getFeFirmwareVersion());
	        theLocalFedSettings.setDelayFirmwareVersion(getDelayFirmwareVersion());
	        count=0;
        } catch (Fed9UVmeDeviceException& e) {
	        count-=1; // we increment the counter and try again.
            if (count >0 ) {
              continue;
            }
            msg << "Caught the following error.\n\n" << e.what() << std::endl;
	        msg << "FE or delay FPGA firmware versions must be incorrect. Reloading firmware from revision 0 on Compact Flash card." << std::endl;
	        Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_EXPECTED) << msg.str();
	        msg.str("");
	        std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << localHardwareId << std::endl;
	  	    // lock a mutex to make sure we don't reload firmware on more than one fed at a time!.
	        int errLock( pthread_mutex_lock(&multiFedAccessMutex) );
	        if (errLock)
	            THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!")));

	        reloadFpgaFirmwareFromCompactFlash(0);
	        fed9Uwait(50000000);	    
	        std::cout <<__PRETTY_FUNCTION__ << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << localHardwareId << std::endl;
	    // release the mutex to allow other feds to reload.
	        pthread_mutex_unlock(&multiFedAccessMutex);


	        theLocalFedSettings.setFeFirmwareVersion(getFeFirmwareVersion());
	        theLocalFedSettings.setDelayFirmwareVersion(getDelayFirmwareVersion());
	    }	  
    } while (count > 0);
      } else {
    u32 count = 10;
    do {
	try { 
      //std::cout << "about to Verify the HardwareID." << std::endl;
	  //verifyHardwareId();
      std::cout << "about to Verify th  FirmwareVersions." << std::endl;
	  //verifyVersions();
      count = 0 ;
	} catch (Fed9UVmeDeviceException& e) {
      count -=1;
      msg << "Caught the following error.\n\n" << e.what() << std::endl;
      msg << std::endl << std::endl<< "we will try again: trial number: " << (10-count) << std::endl << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_EXPECTED) << msg.str();
      msg.str("");
                        
      if (count >0 ) {
       continue;
      }
	  msg << "Caught the following error.\n\n" << e.what() << std::endl;
	  msg << "FE or delay FPGA firmware versions must be incorrect. Resetting System Ace..." << std::endl;
	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_EXPECTED) << msg.str();
	  msg.str("");
      std::cout << __PRETTY_FUNCTION__ << "Caught Exception when verifying the hardware id or the firmware versions and tried 10 times!!" << std::endl;
	  /*std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << getFedHardwareId() << std::endl;
	  int errLock ( pthread_mutex_lock(&multiFedAccessMutex)) ;
	  if (errLock)
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!")));
	  
	  //reloadFpgaFirmwareFromCompactFlash(0);
	  resetSystemAce();
	  fed9Uwait(1000000);
	  sendFedCompleteLogicReset();
	  verifyHardwareId();
	  verifyVersions();
	  std::cout <<__PRETTY_FUNCTION__ << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << getFedHardwareId() << std::endl;
	  // release the mutex to allow other feds to reload.
	  pthread_mutex_unlock(&multiFedAccessMutex);
	  */
	}	  
    } while (count > 0) ;
      }
    }
  }
  catch (ICUtils::ICException& exc) {
    std::ostringstream errMsg;
    errMsg << "Caught an ICUtils::ICException during construction Fed9UVmeDevice object." << std::endl;
    Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << errMsg.str();
    RETHROW(exc, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, errMsg.str() ) );
  }
  catch (std::exception& exc) {
    std::ostringstream errMsg;
    errMsg << "Caught an std::exception during construction Fed9UVmeDevice object." << std::endl;
    Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << errMsg.str();
    RETHROW(exc, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, errMsg.str() ) );
  }
  catch (...) {
    std::ostringstream errMsg;
    errMsg << "Caught an unknown exception during construction Fed9UVmeDevice object." << std::endl;
    Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << errMsg.str();
    THROW( Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, errMsg.str() ) );
  }


  Fed9UVmeDevice::~Fed9UVmeDevice() {
    //Because an auto_ptr is used there is no need to delete theFed.
    //theLocalFedSettings is a references to new'd memory do need to be deleted.
    signal(SIGINT, _oldHandler);
    // <NAC date="23/05/2007"> changed to be a stack object not a refernce to a heap object so no need to delete it
    //if (&theLocalFedSettings) {
    //  delete &theLocalFedSettings;
    //}
    // </NAC>
  }


  Fed9UVmeDevice& Fed9UVmeDevice::verifyHardwareId() throw (Fed9UVmeDeviceException) {
    // Now we get the hardware address of the FED and make sure that it corresponds to the description hardwareid
    try {
        u32 localHardwareId = getFedHardwareId();
      ICUTILS_VERIFYX(theLocalFedSettings.getFedHardwareId() == localHardwareId,Fed9UVmeDeviceException)
	(theLocalFedSettings.getFedHardwareId())(localHardwareId).error().msg("HardwareID of description does not match that of FED, Make sure you have your FED version and Eprom Version number set correctly. Most likely is that V1 FED would have V1 Eprom Version. Also check that the hardwareID matches that written on the front panel of the FED.");

      return *this;
    }
    catch (const std::exception &e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


  void Fed9UVmeDevice::verifySystemACEFirmwareVersions() throw (Fed9UVmeDeviceException) {

    try{

      std::ostringstream msg;

      std::string fVFileName;
      
      bool lMustReload=false;
      
      try {
	fVFileName = std::string(getenv ("ENV_CMS_TK_FED9U_ROOT"))+std::string("/Fed9UFirmwareFiles/.FirmwareVersions") ;
      } catch (...) {
	try {
		fVFileName = std::string(getenv ("ENV_TRACKER_DAQ"))+std::string("/Fed9UFirmwareFiles/.FirmwareVersions") ;
	} catch (...) {



	msg << FED9U_FUNCTION << " " << __LINE__ << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << msg.str();
	msg.str("");
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Unable to get Fed software directory from environmental variable!"));
	}
      }

      std::ifstream lInputStream(fVFileName.c_str());
      if(!lInputStream){
	msg << FED9U_FUNCTION <<" can't open  "<<fVFileName<<std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << msg.str();
	msg.str("");
	THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to open file: ")+fVFileName));
      }

      // the order of the firmware versions in the list is:
      // 
      // Revision
      // Front end 
      // Delay 
      // Backend 
      // VME

      //Add elements to these vectors to mark where the revisions become available.
      vector<u32> lStoredRevisions;
      vector<u32> lVersions;

      u16 lCount=0;
      do{
	std::string lBuf;
	std::getline(lInputStream, lBuf, '\n');
	
	if(lBuf.size()==0)
	  continue;
	
	u32 lStart=0; 
	u32 lEnd=lBuf.size()-1;
	
	while(lBuf[lStart]==' ' || lBuf[lStart]=='\t' || lBuf[lStart]=='\n') lStart++;
	
	while(lBuf[lEnd]==' ' || lBuf[lEnd]=='\t' || lBuf[lEnd]=='\n') lEnd--;

	
	lBuf=lBuf.substr(lStart, lEnd+1);

	msg<<"buf: "<<lBuf<<std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	msg.str("");

	if(lBuf[0]=='#')
	  continue;
	
	//Read in the revision numbers and firmware versions.
	std::istringstream lIStrStr(lBuf);
	if (0 == lCount%5) {
	  u32 tempRev(0);
	  lIStrStr>>std::hex>>tempRev;
	  lStoredRevisions.push_back(tempRev);
	} else {
	  u32 tempFirm(0);
	  lIStrStr>>std::hex>>tempFirm;
	  lVersions.push_back(tempFirm);
	}

	lCount++;

      }while( lCount<40 && !(lInputStream.eof()) );
      
      // we now have the official global firmware versions in the lVersions array,
      // have to compare them with both those read from the hardware
      // and those read from the description that has been passed.

      //Get all the firmware versions currently in use by the FED.
      u32 lHardwareVersions[4]={0};
      try{
	lHardwareVersions[0]=getFeFirmwareVersion();
	lHardwareVersions[1]=getDelayFirmwareVersion();
      }catch(...){
	lMustReload=true;
      }
      lHardwareVersions[2]=getBeFirmwareVersion();
      lHardwareVersions[3]=getVmeFirmwareVersion();
      
      //Used to count how many revisions there have been checked.
      u16 revCount(0);
      for (vector<u32>::const_iterator itRev(lStoredRevisions.begin()); itRev != lStoredRevisions.end(); ++itRev) {
	//Try to load the firmware in this revision.
	msg << "Revision being probed: " << *itRev << " at BA: 0x" << std::hex << theLocalFedSettings.getBaseAddress() << std::dec << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	msg.str("");

	try {
	  std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  int errLock ( pthread_mutex_lock(&multiFedAccessMutex) );
	  if (errLock)
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!")));
	  
	  reloadFpgaFirmwareFromCompactFlash(*itRev);
	  //fed9Uwait(5000000);
	  std::cout <<__PRETTY_FUNCTION__ << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  // release the mutex to allow other feds to reload.
	  pthread_mutex_unlock(&multiFedAccessMutex);
	  
	} catch (ICUtils::ICException& exc) {
	  //Will throw this type of exception if the CF can't reload. If it can't then reload the firmware.
	  // release the mutex to allow other feds to reload.
	  pthread_mutex_unlock(&multiFedAccessMutex);
	  lMustReload = true;
	}

	//If there is a valid firmware in that location on the CF then check it matches the global revision.	
	if (!lMustReload) {
	  for(u16 i=0; i<4; i++) {
	    //Check the appropriate revisions for matching firmware.
	    msg << "What we are comparing: " << std::hex << lHardwareVersions[i] << " " << lVersions[(4*revCount)+i] << std::dec << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    msg.str("");
	    if(lHardwareVersions[i]!=lVersions[(4*revCount)+i]){
	      if(i!=3){
		lMustReload=true;
	      }else{
		// this means that the VME firmware is wrong and we can't 
		// deal with this here;
		// throw
		msg << FED9U_FUNCTION <<" "<<std::hex<<lHardwareVersions[3]<<"   "<<lVersions[3]<<std::endl;
		Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << msg.str();
		msg.str("");
		THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("VME Firmware version mismatch: reload by hand, or return the FED.")));
	      }
	    }
	  }
	}
	if (lMustReload)
	  break;
	++revCount;
      }

      if(lMustReload){
	// reloading: 
	
	if(!mHaveUploadedNewFirmware){
	  msg << "Firmware difference detected: we're reloading the firmware now. Also formatting the hard drive...\n"
	      <<"only joking about the hard drive...!" << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  msg.str("");

	  //Determine name of the ace file.
	  std::ostringstream firmFileName;
	  //Used to index revision vector.
	  vector<u32>::const_iterator itFirm( lVersions.begin() );

 	  msg << "contents of firmware versions:\n" << std::hex;
 	  for (itFirm = lVersions.begin(); itFirm != lVersions.end(); ++itFirm) {
 	    msg << "0x" << *itFirm << "\t";
 	  }
 	  msg << std::dec << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << msg.str();
	  msg.str("");
	  //Important puts the iterator back to the beginning of the vector to find the correct firmware values!
	  itFirm = lVersions.begin();
	  for (vector<u32>::const_iterator itRev(lStoredRevisions.begin()); itRev != lStoredRevisions.end(); ++itRev) {
	    msg << "Current revision: " << *itRev << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    msg.str("");
	    if ( itRev == lStoredRevisions.begin() ) {
	      msg << "First revision." << std::endl;
	      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	      msg.str("");
	      //We are looking at the first global revision number, it is zero it must be written if not we must pad to that number.
	      if ( *itRev == 0) {
		//It is the zeroth revision, so save firmware details.
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff);
		msg << std::endl <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
		msg.str("");
	      } else {
		u32 paddingAdded(0);
		while (*itRev != paddingAdded) {
		  //Have a file name with all zeros!
		  if (7 < paddingAdded)
		    break;

		  //Pad the file name with some zeros until we want to write a valid set of firmware versions.
		  firmFileName << std::hex << std::setw(6) << std::setfill('0') << 0
			       << std::hex << std::setw(6) << std::setfill('0') << 0;
		  ++paddingAdded;
		}
		//Now there is sufficient padding, write the revisions to file.
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff);
		msg << std::endl << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
		firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
		msg << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec << std::endl;
		Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
		msg.str("");
	      }
	      msg << "firmFileName after itRev = begin: " << firmFileName.str() << std::endl;
	      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	      msg.str("");
	    } else if ( (*itRev) - 1 == *(itRev-1) ) {
	      //This revision number is one up from the previous so write it.
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg << std::endl << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec << std::endl;
	      msg << "firmFileName after (*itRev) - 1 == *(itRev-1): " << firmFileName.str() << std::endl;
	      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	      msg.str("");
	    } else if ( (*itRev) - 1 != *(itRev-1) ) {
	      //Need to pad from the between the last revision number and the current one we are on.
	      u32 paddingAdded( *(itRev-1) + 1 );
	      while (*itRev != paddingAdded) {
		//Have a file name with all zeros!
		if (7 < paddingAdded)
		  break;

		//Pad the file name with some zeros until we want to write a valid set of firmware versions.
		firmFileName << std::hex << std::setw(6) << std::setfill('0') << 0
			     << std::hex << std::setw(6) << std::setfill('0') << 0;
		++paddingAdded;
	      }
	      //Now we can write the firmware.
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::endl << std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec;
	      firmFileName << std::hex << std::setw(3) << std::setfill('0') << ((*(++itFirm))&0xfff);
	      msg <<  std::hex << std::setw(3) << std::setfill('0') << ((*itFirm)&0xfff) << std::dec << std::endl;
	      msg << "firmFileName after (*itRev) - 1 == *(itRev-1): " << firmFileName.str() << std::endl;
	      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	      msg.str("");
	    } else if (*itRev > 7) {
	      std::ostringstream errMsg;
	      errMsg << "Firmware revision is out of range: " << *itRev << std::endl;
	      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << msg.str();
	      errMsg.str("");
	      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, errMsg.str()));
	    } else {
	      std::ostringstream errMsg;
	      errMsg << "Unanticipated error condition. Current, previous and next revision number: "
		     << *itRev << " "
		     << *(itRev-1) << " "
		     << *(itRev+1) << std::endl;
	      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << msg.str();
	      msg.str("");
	      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, errMsg.str()));
	    }
	  }//end of itRev for

	  //Add extension.
	  firmFileName << ".ace";
	  // first find the ace file:
	  std::string lACEFilename(getenv("ENV_CMS_TK_FED9U_ROOT")+string("/Fed9UFirmwareFiles/")+firmFileName.str());
	  msg << "File name and path:\n"
	      << lACEFilename
	      << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	  msg.str("");

	  fed9Uwait(1000000);
	  // true means that we're actually going to do the write:

	  // lock a mutex to protect the process count update.
	  std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  int errLock ( pthread_mutex_lock(&multiFedAccessMutex) );
	  if (errLock) {
	    firmwareUploadInProcessCount-=1; // decrement on error cause we don't want to freez the process if one fails!	  
	    std::cout << "just failed to lock! Process Count = " <<firmwareUploadInProcessCount << "hardwareid= " << theLocalFedSettings.getFedHardwareId() << std::endl;
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!")));
	  }

	  std::cout << "Locked Mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  // increment the UploadProcessCount so we know how many feds are currently uploading firmware.
	  firmwareUploadInProcessCount+=1;	  
	  std::cout << "Process Count = " <<firmwareUploadInProcessCount << "hardwareid= " << theLocalFedSettings.getFedHardwareId() << std::endl;
	  std::cout << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  pthread_mutex_unlock(&multiFedAccessMutex);
	  std::cout << "Unlocked mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  // make the firmware upload! lengthy process!
	  std::string lRet(this->setCfImageFile(lACEFilename, false, true)); 
	  std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  errLock = pthread_mutex_lock(&multiFedAccessMutex);
	  if (errLock) {
	    firmwareUploadInProcessCount-=1; // decrement on error cause we don't want to freez the process if one fails!	  
	    std::cout << "just failed to lock! Process Count = " <<firmwareUploadInProcessCount << "hardwareid= " << theLocalFedSettings.getFedHardwareId() << std::endl;
	    THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!"))); 
	  }
	  std::cout << "Locked Mutex!!!!!!!!!!!!!!!! 2: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  // if we get the lock then decrement the process count.
	  firmwareUploadInProcessCount-=1;	  
	  std::cout <<__PRETTY_FUNCTION__ << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  pthread_mutex_unlock(&multiFedAccessMutex);
	  
	  std::cout << "just unlocked! Process Count = " <<firmwareUploadInProcessCount << "hardwareid= " << theLocalFedSettings.getFedHardwareId() << std::endl;
	  
	  std::cout << "Unocked Mutex!!!!!!!!!!!!!!!! 2: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  // wait for total number of uploading processes to end before we continue
	  while ( firmwareUploadInProcessCount ) {
	    fed9Uwait(100000);
	    std::cout << "Process Count = " <<firmwareUploadInProcessCount << std::endl;
	  }
	  std::cout << "All uploads complete!!! moving on! "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	  msg << "Firmware details =\n"<<lRet<<std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	  msg.str("");
	  try {
	    //Reboot from the first (default) revision stored on CF.
	    msg << "trying to reload from revision " << *(lStoredRevisions.begin() ) << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	    msg.str("");
	    std::cout <<__PRETTY_FUNCTION__ << "About to lock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	    // lock a mutex to make sure we don't reload firmware on more than one fed at a time!.
	    errLock = pthread_mutex_lock(&multiFedAccessMutex) ;
	    if (errLock)
	      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Unable to lock mutex when trying to update the firmware Upload process count!!!")));

	    this->reloadFpgaFirmwareFromCompactFlash( *(lStoredRevisions.begin()) );
	    fed9Uwait(5000000);
	    
	    // release the mutex to allow other feds to reload.
	    std::cout <<__PRETTY_FUNCTION__ << "About to unlock mutex!!!!!!!!!!!!!!!! 1: "  << theLocalFedSettings.getFedHardwareId() << std::endl;
	    pthread_mutex_unlock(&multiFedAccessMutex);

	    mHaveUploadedNewFirmware=true;



	    msg << "\n\nUploaded a new firmware!!!\n" << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	    msg.str("");
	  } catch (ICUtils::ICException& exc) {
	    // release the mutex to allow other feds to reload.
	    pthread_mutex_unlock(&multiFedAccessMutex);
	    mHaveUploadedNewFirmware=false;
	    msg << "Failed to reload firmware from the compact flash, upload most likely failed.\n"
		<< exc.what()
		<< std::endl;
	    Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << msg.str();
	    msg.str("");
	  }

	} else {
	  msg<<FED9U_FUNCTION<<" "<<__LINE__<<std::endl;
	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING)<< msg.str();
	  msg.str("");
	  // we've been here before:
	  THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, std::string("Firmware reloading has failed, manual intervention required.")));
	}
      }
    }
    catch (const std::exception &e) {
      std::ostringstream errMsg;
      errMsg<<FED9U_FUNCTION<<" "<<__LINE__<<std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING)<< errMsg.str();
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      std::ostringstream errMsg;
      errMsg<<FED9U_FUNCTION<<" "<<__LINE__<<std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING)<< errMsg.str();
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }

  }

  Fed9UVmeDevice& Fed9UVmeDevice::verifyVersions() throw (Fed9UVmeDeviceException) {
    // Now we get the Fed versions, both hard and soft and make sure that it corresponds to the description
    try {

    

      bool error = false;
      stringstream errorString;
      
	if ( theLocalFedSettings.getFeFirmwareVersion() != getFeFirmwareVersion() ) {
	  error = true;
	  errorString << "FE Firmware version of description ( "<< std::hex << theLocalFedSettings.getFeFirmwareVersion() << "  ) does not match that of the FED ( " << std::hex << getFeFirmwareVersion() <<" )\n";
	}
	if ( theLocalFedSettings.getBeFirmwareVersion() != getBeFirmwareVersion() ) {
	  error = true;
	  errorString << "BE Firmware version of description ( "<< std::hex << theLocalFedSettings.getBeFirmwareVersion() << "  ) does not match that of the FED ( " << std::hex << getBeFirmwareVersion() <<" )\n";
	}
	if ( theLocalFedSettings.getVmeFirmwareVersion() != getVmeFirmwareVersion() ) {
	  error = true;
	  errorString << "VME Firmware version of description ( "<< std::hex << theLocalFedSettings.getVmeFirmwareVersion() << "  ) does not match that of the FED ( " << std::hex << getVmeFirmwareVersion() << std::dec <<" )\n";
	}
	if ( theLocalFedSettings.getDelayFirmwareVersion() != getDelayFirmwareVersion() ) {
	  error = true;
	  errorString << "Delay Firmware version of description ( "<< std::hex << theLocalFedSettings.getDelayFirmwareVersion() << "  ) does not match that of the FED ( " << std::hex << getDelayFirmwareVersion() << std::dec <<" )\n";
	}
      
	if (!error ) {
	  if ( theLocalFedSettings.getFeFirmwareVersion() < FED9U_FE_FIRMWARE_MIN_VER ) {
	    error = true;
	    errorString << "FE Firware Version is not compatible with Software!\n";
	  }
	  if ( theLocalFedSettings.getBeFirmwareVersion() < FED9U_BE_FIRMWARE_MIN_VER ) {
	    error = true;
	    errorString << "BE Firware Version is not compatible with Software!\n";
	  }
	  if ( theLocalFedSettings.getVmeFirmwareVersion() < FED9U_VME_FIRMWARE_MIN_VER ) {
	    error = true;
	    errorString << "VME Firware Version is not compatible with Software!\n";
	  }
	  if ( theLocalFedSettings.getDelayFirmwareVersion() < FED9U_DELAY_FIRMWARE_MIN_VER ) {
	    error = true;
	    errorString << "DELAY Firware Version is not compatible with Software!\n";
	  }
	}
	if (error) {
	  errorString << "\n\n Also note that you must have at least the following versions loaded in your FED in order to be compatible with the software : \n"
		      << " Fe Firmware Version    :  " << std::hex << FED9U_FE_FIRMWARE_MIN_VER << "\n"
		      << " Be Firmware Version    :  " << std::hex << FED9U_BE_FIRMWARE_MIN_VER << "\n"
		      << " Vme Firmware Version   :  " << std::hex << FED9U_VME_FIRMWARE_MIN_VER << "\n"
		      << " Delay Firmware Version :  " << std::hex << FED9U_DELAY_FIRMWARE_MIN_VER << std::dec << std::endl;
	}
	ICUTILS_VERIFYX(!error,Fed9UVmeDeviceException).error().msg(errorString.str().c_str());
	
	return *this;

    }
    catch (const std::exception &e) {
      // if we get here then we should try again just incase we had a problem with a single transfer
      //try {
	//verifyVersions();
	//return *this;;
      //} catch (const std::exception &e2) {
	// failed a second attempt!!!
	//RETHROW(e2, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "!!!!Caught std::exception. Failed the second attempt to read registers!!!!"));
      //}
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVMEDEVICE, "Caught unknown exception."));
    }
  }


   void Fed9UVmeDevice::ControlCHandler(int) {
     std::ostringstream msg;
     msg << "Control-C detected!!!" << std::endl;
     if (handleControlC) {
       msg << "Enabling control C handling." << std::endl;
       controlCCaught = true;
     } else {
       msg << "No control C handling required, exiting." << std::endl;
       exit(0);
     }
     Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
     msg.str("");
   }

  Fed9UVmeDevice & Fed9UVmeDevice::setBlockModeUpload(bool value) throw (Fed9UVmeDeviceException)  {
    blockModeUpload_ = value;
    return * this;
  }


}
