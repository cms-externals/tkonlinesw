#include <inttypes.h>
#include <stdint.h>
#include "Fed9UCrateStatusProbe.hh"
#ifdef TBHARDWARE
#include "Fed9UDevice.hh"
#include "Fed9UWait.hh"

#endif

#include "Fed9ULogTemplate.hh"

#include <sstream>
#include <pthread.h>

#include "Fed9UVmeBaseException.hh"
#include "Fed9UBusAdaptor.hh"
namespace Fed9U
{ 

  u32 Fed9UCrateStatusProbe::mRunningProbe(0);

//   Fed9UCrateStatusProbe::Fed9UCrateStatusProbe() : mRunningProbe(0) {
//   }

  Fed9UCrateStatusProbe::~Fed9UCrateStatusProbe() {
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << "Calling destructor." << std::endl;
  }

  void Fed9UCrateStatusProbe::probeCrate(Fed9UDescription * pDesc, unsigned long start, unsigned long end, bool KeepFeds, bool KeepDescriptions, bool showTemps, bool useThreads, bool adcsOff) {
    std::ostringstream msg;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << "adcsOff = " << (adcsOff?"true":"false") << "\n";
    if(start>end){
      unsigned long temp=start;
      start=end;
      end=temp;
    }
#ifdef TBHARDWARE
    //using Fed9U::Fed9UDescription;
    //using Fed9U::Fed9UDevice;
    //using std::exception;

    // using Fed9U::Fed9UDescription;

    bool deleteDesc=false;
    if( !pDesc ) {
      pDesc=new Fed9UDescription();
      deleteDesc = true;
    }
#endif

    //before probing the crate we perform a bus reset
    /*Fed9UBusAdaptor busAdaptor(FED9U_HAL_BUS_ADAPTOR_CAEN_PCI,pDesc->getCrateNumber());//crateNumber=0 in this case, noet also this only works if you are using the CAEN_PCI bus adaptor!!!
    busAdaptor.resetBus();
    usleep(6000000); //wait long enough for the FEDs to boot.  
    */

    //Used to hold the all the FEDs that are discovered in the crate.
    u32 numberSlotsToProbe( (end-start)+1 );
    //Make a note of the total threads to run, so we know when they are all loaded.
    u32 totalThreads;
    if (useThreads) {
      //In threaded environment it is the number of slots we want.
      totalThreads = numberSlotsToProbe;
    } else {
      //In a none threaded environment it will just be one as there is only the main thread running.
      totalThreads = 1;
    }

    //Used to store all the FEDs that are created.
    vector<Fed9UDevice**> fedList;//Use pop back can't initialise things.(numberSlotsToProbe, new Fed9UDevice*(NULL) );//NEED TO DELETE THESE!
    vector<Fed9UDevice**>::iterator fedListIt( fedList.begin() );//Nothing in the vector so not really a worry.

    try {
      //Check each slot number, optionally in a thread.
      //for (fedListIt = fedList.begin(); fedListIt != fedList.end(); ++fedListIt, ++slot) {//Can't use this the iterator is invalidated by the pop back.

      bool allThreadsInitd(false);

      if (!useThreads)
	allThreadsInitd = true; // This line is required in non-threaded running to prevent the method from getting stuck waiting for this flag to be true.
      
      // <NAC date="24/05/2007"> used to join threads if using threads
      std::vector<pthread_t> threads;
      // </NAC>

      for (u32 slot(start); slot <= end; ++ slot) {
      
	msg << "Launching threads to probe slot " << slot << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	msg.str("");

	//Create the area to place the FED object once done with it.
	//This new'd memory doesn't have to be deleted in the case of a thread throwing an exception as the main program will never know.
	//Just delete the memory in the normal course of events, i.e. don't try an delete in the matching catch for this try block.
	fedList.push_back( new Fed9UDevice*(NULL) );
	//Gather all the arguments together.
	void* arg[] = { pDesc, fedList[slot-start], &slot, &KeepFeds, &showTemps, &mRunningProbe, &allThreadsInitd, &adcsOff };

	//Don't launch another thread until we have constructed the arguments for the last one.
	u32 currentMRunningProbe = mRunningProbe;

	if (useThreads) {
	  //Create the thread and initialise the attributes.
	  pthread_t tid;
	  pthread_attr_t attr;
	  pthread_attr_init(&attr);
	  //Launch the thread.
          // <NAC date="23/05/2007">
          int result = pthread_create(&tid, &attr, &probeSlot, arg); 
          if (result) {
            std::stringstream msg;
            msg << "Thread creation failed with exit code " << result << ". Stopping probe. " << std::endl;
            Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << msg.str();
#ifdef TBHARDWARE
            if (deleteDesc)
              delete pDesc;
#endif
            for (fedListIt = fedList.begin(); fedListIt != fedList.end(); ++fedListIt) {
              //Delete the FEDs
              if (*(*fedListIt) != NULL) {
                //Delete the Fed9UDevice object.
                delete *(*fedListIt);
                *(*fedListIt) = NULL;
              }
            } //for fedListItr
            return;
          }// if result
          threads.push_back(tid);
          // </NAC>
	  //Wait until the thread has constructed its arguments and told us, before we move onto create the next thread.
	  //There can be some serious problems with the thread trying to launch itself with the next threads arguments, due to pointers being used.
	  while (currentMRunningProbe == mRunningProbe) {
            std::stringstream msg;
            msg << "Waiting for thread on slot " << slot << std::endl;
            Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    fed9Uwait(10);
          }
	} else {//Not using threads to just call the function...
	  probeSlot(arg);
	}

	msg << "Finished with this slot." << std::endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	msg.str("");

      } // for fedListIt

      msg << "Waiting for probe to complete." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");
      //Wait for the probing to complete.
      while (mRunningProbe != 0) {
	//Check if our threads are done...
	if (totalThreads <= mRunningProbe) 
	  allThreadsInitd = true;
	fed9Uwait(100);
      }
      
      // <NAC date="24/05/2007"> join threads to make sure we don;t continue until they exit
      for (std::vector<pthread_t>::iterator i = threads.begin(); i != threads.end(); i++) {
        void** pNULL = NULL;
        pthread_join(*i,pNULL);
      }
      // </NAC>

      msg << "All threads exited." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");

    }
    catch (ICUtils::ICException& e) {
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught ICUtils::ICException.\n" << e.what() << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    }
    catch (std::exception& e) {
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught std::exception.\n" << e.what() << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    }
    catch (...) {
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught an unknown exception." << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    }

    for (fedListIt = fedList.begin(); fedListIt != fedList.end(); ++fedListIt) {
      //If we want to keep the FEDs then better save them.
      if (KeepFeds) {
	//Check we actually have a Fed9UDevice*
	if (*fedListIt != NULL) {
	  Fed9UDevice* pFed( *(*fedListIt) );

	  //Now check we have a pointer to a Fed9UDevice object.
	  if (pFed != NULL) {

	    msg << "Creating a FED entry." << std::endl; 
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	    msg.str("");

	    //
	    // serial number container
	    //
	    unsigned long sn( static_cast<unsigned long>(pFed->getFedHardwareId()) );
	    msg << "getFedHardwareId completed" << "\n";
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	    msg.str("");

	    //
	    // slot this FED is in.
	    //
	    const u32 slot( pFed->getFed9UVMEDeviceDescription().getSlotNumber() );

	    //
	    // then add detected fed, as a Fed9UEntry to the container
	    // 
	    Fed9UEntry fe(sn, mSlotBAMap[slot], slot);

#ifdef TBHARDWARE
	    msg <<  " pFed is " << pFed << " just about to add ABC pointer to Fed Entry" << "\n";
	    fe.setFed9UABC(reinterpret_cast<Fed9UABC*>(pFed));
	    msg << " added entry" << "\n";
	    if(KeepDescriptions) {
	      msg <<  " pFed is " << pFed << " just about to add ABC pointer to Fed Entry" << "\n";
	      fe.setFed9UABC(reinterpret_cast<Fed9UABC*>(new Fed9UDescription(pFed->getFed9UVMEDeviceDescription())));
	      msg << " added entry" << "\n";
	    }
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    msg.str("");

#endif	
	    //if(sn>1)
	    //  fe.setTestFlag(false);
	    appendFed9UEntry(fe);
	    msg << " appended F Entry " << "\n";
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
	    msg.str("");
#ifdef TBHARDWARE
	    if(pFed!=NULL && !KeepFeds){
	      delete pFed;
	      pFed=NULL;
	    }
#endif

	  }//if (pFed != NULL)
	  else {
	    msg << "No FED to keep!" << std::endl;
	    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	    msg.str("");
	  }

	}//if (*fedListIt != NULL)
	else {
	  msg << "No pointer to the pointer to the Fed9UDevice object!" << std::endl;
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
	  msg.str("");
	}
      }//if (KeepFeds)
      else {
	//Delete the FEDs
	if (*(*fedListIt) != NULL) {
	  //Delete the Fed9UDevice object.
	  delete *(*fedListIt);
	  *(*fedListIt) = NULL;
	}
      }//else for if (KeepFeds)

      //Now someone else has responsibility for the FED we can delete our new's Fed9UDevice*
      if (*fedListIt != NULL) {
	delete *fedListIt;
	*fedListIt = NULL;
      }
    }//fedList loop

#ifdef TBHARDWARE
    if (deleteDesc)
      delete pDesc;
    
#endif
    
    resetIterator();
  }
  
  void* Fed9UCrateStatusProbe::probeSlot(void* param) {
    //Check we are not given a NULL description pointer.
    Fed9U::Fed9UDescription* pDesc( reinterpret_cast<Fed9U::Fed9UDescription*>( reinterpret_cast<void**>(param)[0] ) );
    if (pDesc==NULL) {
      std::stringstream msg;
      msg << "Don't have a valid description object. Can't probe slot."  << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << msg.str();
      //Don't need to do anything to running probe first as it is not yet created.
      return NULL;
    }
    Fed9U::Fed9UDescription lDesc(*pDesc);
    Fed9U::Fed9UDevice** ppFed( reinterpret_cast<Fed9U::Fed9UDevice**>( reinterpret_cast<void**>(param)[1] ) );
    const Fed9U::u32 slot( *(reinterpret_cast<Fed9U::u32*>( reinterpret_cast<void**>(param)[2] ) ) );
    const bool KeepFeds( *(reinterpret_cast<bool*>( reinterpret_cast<void**>(param)[3] ) ) );
    const bool showTemps( *(reinterpret_cast<bool*>( reinterpret_cast<void**>(param)[4] ) ) );
    Fed9U::u32* runningProbe( reinterpret_cast<Fed9U::u32*>( reinterpret_cast<void**>(param)[5] ) );
    bool* allThreadsInitd( reinterpret_cast<bool*>( reinterpret_cast<void**>(param)[6] ) );
    const bool adcsOff( *(reinterpret_cast<bool*>( reinterpret_cast<void**>(param)[7] ) ) );

    //Update the number of instances of member function.
    *runningProbe += 1;

    std::ostringstream msg;

    msg << "Waiting for other threads, currently in slot " << slot << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
    msg.str("");

    //Need to ensure that all the other threads have launched before we can move this thread and std::decrement the number running.
    //Thread creator needs to ensure we have reached this point before it can create another thread.
    while ( !(*allThreadsInitd) )
      fed9Uwait(100);

    try {

      std::ostringstream msg;
      msg << "Slot " << slot << " has started." << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");

      //If there is a FED on the end of the slot there is a problem.
      if (*ppFed!=NULL) {
	msg << std::dec << "Not got a null FED! in slot " << slot << ". Pointer address: " << std::hex << *ppFed << std::dec << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << msg.str();
	msg.str("");
	*runningProbe -= 1;
	return NULL;
      }

      lDesc.setBaseAddress(mSlotBAMap[slot]);
      msg<<std::dec<<"trying base address 0x"<<std::hex<<lDesc.getBaseAddress()<<std::dec<<"\n";
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << msg.str();
      msg.str("");

#ifdef TBHARDWARE	
      using namespace Fed9U;

      try{
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED)<<"Instantiating Fed9UDevice:\n ";
	*ppFed=new Fed9UDevice(lDesc,lDesc.getCrateNumber());
	ICUTILS_VERIFY((*ppFed)->getIsFed())((*ppFed)->getIsFed());
	(*ppFed)->getFedId();

	if (adcsOff) {
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<"turn off FED and ADCs!!!\n ";
	  (*ppFed)->stop(); // we stop the fed now to switch off the ADCs.
	} else {
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL)<<"turn On FED and ADCs!!!\n ";
	  (*ppFed)->start(); // we start the fed now to switch on the ADCs.
	}
      }

      catch (Fed9U::Fed9UVmeBaseException& e) {
	std::ostringstream errMsg;
	errMsg << "Lock failed to be acquired." << e.what() << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << errMsg.str();
      }
      catch(ICUtils::ICException & exc) {
	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	std::ostringstream errMsg;
	errMsg << std::dec << "caught ICException & \n Assume no fed in slot " << slot << "." << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_EXPECTED) << errMsg.str();
 	if ( string::npos == string(exc.what()).find("Could not read from ") ) {
 	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << exc.what();
	}

	*runningProbe -= 1;
	return NULL;
      }
      catch(std::exception & exc){
	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	std::ostringstream errMsg;
	errMsg << std::dec << "caught std::exception & \n Assume no fed in slot " << slot << "." << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
 	if ( string::npos == string(exc.what()).find("Could not read from ") ) {
 	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << exc.what();
	}
	*runningProbe -= 1;
	return NULL;
      }
      catch(...) {
	std::ostringstream errMsg;
	errMsg << std::dec << "Caught an unknown exception, assume no fed in slot " << slot << "." << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	*runningProbe -= 1;
	return NULL;
      }

      try {
	msg << std::dec;
	msg << "  !!Found a CMS Tracker Fed in crate " <<std::dec<<lDesc.getCrateNumber()<<", slot "<<std::dec<<slot<<"\n";
	msg << "  !!FED Hardware Id " << std::dec << (*ppFed)->getFedHardwareId() << "\n";
	msg << "  !!FED Version " << std::dec << (*ppFed)->getFed9UVMEDeviceDescription().getFedVersion() << "\n";
	msg << "  !!Eprom Version " << std::dec <<(*ppFed)->getFed9UVMEDeviceDescription().getEpromVersion() << "\n";
	msg << "  !!FE Firmware Version "<< std::hex << (*ppFed)->getFed9UVMEDeviceDescription().getFeFirmwareVersion()<< "\n";
	msg << "  !!BE Firmware Version "<< std::hex << (*ppFed)->getFed9UVMEDeviceDescription().getBeFirmwareVersion()<< "\n";
	msg << "  !!VME Firmware Version "<< std::hex << (*ppFed)->getFed9UVMEDeviceDescription().getVmeFirmwareVersion() << "\n";
	msg << "  !!Delay Firmware Version "<< std::hex << (*ppFed)->getFed9UVMEDeviceDescription().getDelayFirmwareVersion() << "\n\n";
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	msg.str("");
      }
      catch(ICUtils::ICException & exc) {
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << FED9U_FUNCTION;

	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	std::ostringstream errMsg;
	errMsg << std::dec << "caught ICException & \n Problem retrieving firmware versions or hardware ID. \n"
	       << exc.what() << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
	*runningProbe -= 1;
	return NULL;
      }
      catch(std::exception & exc){
	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	std::ostringstream errMsg;
	errMsg << std::dec << "caught std::exception & \n Problem retrieving firmware versions or hardware ID. \n"
	       << exc.what() << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
	*runningProbe -= 1;
	return NULL;
      }
      catch(...) {
	std::ostringstream errMsg;
	errMsg << std::dec << "Caught an unknown exception, problem retrieving firmware versions or hardware ID." << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
	if((*ppFed)!=NULL){
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}

	*runningProbe -= 1;
	return NULL;
      }

      u32 maxTemp = 22;
      if (showTemps) {
	msg << "  !!Temperatures for Fed in Slot " << std::dec << slot << ", fed hardware ID = " << std::hex << (*ppFed)->getFedHardwareId() << "\n";;
	Fed9UTempControlInfo tempControl;
	try{
	  
	  for (int i = 0 ; i < FEUNITS_PER_FED; i++ ) {
	    tempControl = (*ppFed)->getTempControlInfo(Fed9UAddress().setFedFpga(i));
	    //if ( tempControl.getLm82Actual() < maxTemp ) continue;
	    msg << std::dec 
		   << "  !!FeUnit " 
		   << static_cast<u16>(Fed9UAddress().setFedFeUnit(i).getExternalFedFeUnit()) 
		   << ", LM82 = " 
		   <<  tempControl.getLm82Actual() << " deg "
		   << ", FPGA = " 
		   <<  tempControl.getFpgaActual() << " deg "
		   << ", Shutdown temperature = " 
		   << tempControl.getCritical() << " deg "
		   << ", Id = " 
		   <<  tempControl.getId() << " "
		   << ", Status = " 
		   <<  tempControl.getStatus() << "\n";
	  }
	  tempControl = (*ppFed)->getTempControlInfo(Fed9UAddress().setFedFpga(Fed9UAddress::BACKEND));
	  //if ( tempControl.getLm82Actual() >= maxTemp) {
	    msg << std::dec 
		      << "  !!BE LM82 = " 
		      << tempControl.getLm82Actual() << " deg "
		      << ", FPGA = "
		      << tempControl.getFpgaActual() << " deg "
		      << ", Shutdown temperature = " 
		      << tempControl.getCritical() << " deg "
		      << ", Id = " 
		      << tempControl.getId() 
		      << ", Status = " 
		      << tempControl.getStatus()
		      << "\n";
	    //}
	  tempControl = (*ppFed)->getTempControlInfo(Fed9UAddress().setFedFpga(Fed9UAddress::VME));
	  //if ( tempControl.getLm82Actual() >= maxTemp) {
	    msg << std::dec 
		      << "  !!VME LM82 = " 
		      << tempControl.getLm82Actual()
		      << ", FPGA = "
		      << tempControl.getFpgaActual()
		      << ", Shutdown temperature = " 
		      << tempControl.getCritical() << " deg "
		      << ", Id = " 
		      << tempControl.getId()
		      << ", Status = " 
		      << tempControl.getStatus()
		      << "\n";
	    //}
	  msg << "\n";
	  Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
	  Fed9UMessage<Fed9ULogLevel>(FED9U_LOG_LEVEL_DEBUG) << msg.str();

	  msg.str("");
	}
	catch(ICUtils::ICException & exc){
	  std::ostringstream errMsg;
	  errMsg << std::dec << "Caught an ICExcetion, problem while reading temperatures:\n" << exc.what() << std::endl;
	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

	  if((*ppFed)!=NULL){
	    delete (*ppFed);
	    (*ppFed)=NULL;
	  }

	  *runningProbe -= 1;
	  return NULL;
	}
	catch(std::exception & exc){
	  std::ostringstream errMsg;
	  errMsg << std::dec << "Caught a std::excetion, problem while reading temperatures:\n" << exc.what() << std::endl;
	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

	  if((*ppFed)!=NULL){
	    delete (*ppFed);
	    (*ppFed)=NULL;
	  }

	  *runningProbe -= 1;
	  return NULL;
	}
	catch(...) {
	  std::ostringstream errMsg;
	  errMsg << std::dec << "Caught an unknown exception, problem while reading the temperatures." << std::endl; 
	  Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

	  if((*ppFed)!=NULL){
	    delete (*ppFed);
	    (*ppFed)=NULL;
	  }

	  *runningProbe -= 1;
	  return NULL;
	}
      }

      //Delete the Fed9UDevice if required.
      if (!KeepFeds) {
	if ((*ppFed)!=NULL) {
	  delete (*ppFed);
	  (*ppFed)=NULL;
	}
      }

#endif

      //Tell it the thread is done.
      *runningProbe -= 1;

      return NULL;

    }
    catch(ICUtils::ICException & exc){
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught ICException & \n Problem while probing the crate!\n"
	     << exc.what() << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

      if((*ppFed)!=NULL){
	delete (*ppFed);
	(*ppFed)=NULL;
      }

      *runningProbe -= 1;
      return NULL;
    }
    catch(std::exception & exc){
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught std::exception & \n Problem while probing the crate!\n"
		<< exc.what() << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

      if((*ppFed)!=NULL){
	delete (*ppFed);
	(*ppFed)=NULL;
      }

      *runningProbe -= 1;
      return NULL;
    }
    catch (...) {
      std::ostringstream errMsg;
      errMsg << std::dec << "Caught a mysterious unknown exception... Problem while probing the crate!\n" << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();

      if((*ppFed)!=NULL){
	delete (*ppFed);
	(*ppFed)=NULL;
      }

      *runningProbe -= 1;
      return NULL;
    }
  }//probeSlot

}//namespace Fed9U
