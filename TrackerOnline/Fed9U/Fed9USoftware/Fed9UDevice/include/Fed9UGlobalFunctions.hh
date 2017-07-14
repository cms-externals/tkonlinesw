namespace Fed9U {
  //**********************************************************************************************
  //Function to run a trim dac cal on the said fed
  //this method is only called by a new thread and has a counter to note how many threads are running
  //you can then wait for all trim Dac Cals to finish by checking a boolean that tells the thread
  //everyone else is done and it is time to start the trimDAC cal.
  //Finally a pointer to an ostringstream is provided so that the print out can be sent to and the main
  //thread will write all print out to cout in a thread safe way.
  void * trimDacCal(void * param) {
    
    
    Fed9UDescription * desc = reinterpret_cast<Fed9UDescription*>(reinterpret_cast<void**>(param)[0]);
    bool* allThreadsInitd = reinterpret_cast<bool*>(reinterpret_cast<void**>(param)[1]);
    u32* runningTrimDacCal = reinterpret_cast<u32*>(reinterpret_cast<void**>(param)[2]);
    (*runningTrimDacCal)+=1;
    
    //This string stream is used to write collate messages that should be written in an atomic way.
    //Each of our writes to the gFed9UOut stream are atomic, but no guaranty that the subsequent stream
    //writes will be in the appropriate order across the FEDs.
    std::ostringstream mess;


    while (!allThreadsInitd) {
      fed9Uwait(10);
      mess << "I'm waiting! " <<  static_cast<u16>(desc->getSlotNumber()) << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << mess.str();
      mess.str("");
    }
    mess << "Running on slot " << static_cast<u16>(desc->getSlotNumber()) << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");
    
    mess << "entering trim dac thread with description pointer =  " << hex << desc << dec << ", fedid = ";
    mess << desc->getFedId() << '\n';
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");
    try {
      //Construct a Fed9UDevice object from the description that we have been given.
      //This gives us access to the FED.
      Fed9UDevice Fed(*desc);
      mess << "constructed fed in thread " << *runningTrimDacCal << '\n';
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
      mess.str("");
      //Now initialise our FED, setting it up with the appropriate run parameters.
      Fed.init();
      mess << "Init" << endl;
      Fed.initAuxiliaryChips();
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
      mess.str("");
      //Turn on the block mode readout for faster transfers.
      Fed.setBlockModeReadout(true); 
      mess << "about to run trimdac" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
      mess.str("");

      //Now perform the trimDAC calibration.
      try {
       	mess << "run trimdac" << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
	mess.str("");
	
	Fed.trimDACInternalCalibration();
	//Update the description we have been given with the values that we saved during the calibration run.
	*desc=Fed.getFed9UVMEDeviceDescription();
      } catch (ICUtils::ICException & exc ) {
	std::cout << exc.what() << std::endl;
	// note this exception does not mean we should ignore the description created, we get this even if a few channels fail
	//Update the description we have been given with the values that we saved during the calibration run.
	*desc=Fed.getFed9UVMEDeviceDescription();
      }    


    } catch ( ICUtils::ICException & exc) {
      std::ostringstream errMsg;
      errMsg <<  "Failed trim Dac Calibration for FED in slot " << static_cast<u16>(desc->getSlotNumber()) << ".  --- " <<  exc.what() << endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    } catch ( ... ) {
      std::ostringstream errMsg;
      errMsg <<  "Failed trim Dac Calibration for FED in slot " << static_cast<u16>(desc->getSlotNumber()) << ". unknown exception!!!" << endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    }
    mess << "!!!!Finished thread!!!!\n";
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");
    (*runningTrimDacCal)-=1;
    return NULL;
  }




  //**********************************************************************************************                                                                                                                                                                              
  //Function to update the firmware on the fed from an .ace file                                                                                                                                                                                                                   
  //this method is only called by a new thread and has a counter to note how many threads are running                                                                                                                                                                           
  //you can then wait for all firmware uploads to finish by checking a boolean that tells the thread                                                                                                                                                                           
  //everyone else is done.                                                                                                                                                                                                         
  //Finally a pointer to an ostringstream is provided so that the print out can be sent to and the main                                                                                                                                                                      
  //thread will write all print out to cout in a thread safe way.                                                                                                                                                                                                                      
  void * fedFirmwareUpdate(void * param) {


    Fed9UDescription * desc = reinterpret_cast<Fed9UDescription*>(reinterpret_cast<void**>(param)[0]);
    bool* allThreadsInitd = reinterpret_cast<bool*>(reinterpret_cast<void**>(param)[1]);
    u32* runningFirmwareUpdate = reinterpret_cast<u32*>(reinterpret_cast<void**>(param)[2]);
    string* firmwareFileName =  reinterpret_cast<string*>(reinterpret_cast<void**>(param)[3]);
    (*runningFirmwareUpdate)+=1;

    //This string stream is used to write collate messages that should be written in an atomic way.                                                                                                                                                                                
    //Each of our writes to the gFed9UOut stream are atomic, but no guaranty that the subsequent stream                                                                                                                                                                           
    //writes will be in the appropriate order across the FEDs.                                                                                                                                                                                                                   
    std::ostringstream mess;


    while (!allThreadsInitd) {
      fed9Uwait(10);
      mess << "I'm waiting! " <<  static_cast<u16>(desc->getSlotNumber()) << std::endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << mess.str();
      mess.str("");
    }
    mess << "Running Firmware Update on slot " << static_cast<u16>(desc->getSlotNumber()) << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");

    mess << "entering firmware update thread with description pointer =  " << hex << desc << dec << ", fedid = ";
    mess << desc->getFedId() << '\n';
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");
    try {
      //Construct a Fed9UDevice object from the description that we have been given.                                                                                                                                                                                                
      //This gives us access to the FED.                                                                                                                                                                                                                                            
      Fed9UDevice Fed(*desc,desc->getCrateNumber());
      mess << "constructed fed in thread " << *runningFirmwareUpdate << '\n';
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
      mess.str("");
      mess << "about to upload new firwmare" << endl;
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
      mess.str("");

      //Now perform the firmware update.                                                                                                                                                                                                                                           
      try {
        mess << "run firmware update" << endl;
        Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
        mess.str("");

        string firmwareDetails(Fed.setCfImageFile(*firmwareFileName));

	mess << "Fed Firmware Update on slot " << static_cast<u16>(desc->getSlotNumber()) << " done! ... : " << firmwareDetails << endl;
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << mess.str();
        mess.str("");

      } catch (ICUtils::ICException & exc ) {
	       std::cout << exc.what() << std::endl;

      }


    } catch ( ICUtils::ICException & exc) {
      std::ostringstream errMsg;
      errMsg <<  "Failed firmware update for FED in slot " << static_cast<u16>(desc->getSlotNumber()) << ".  --- " <<  exc.what() << endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    } catch ( ... ) {
      std::ostringstream errMsg;
      errMsg <<  "Failed firmware update for FED in slot " << static_cast<u16>(desc->getSlotNumber()) << ". unknown exception!!!" << endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << errMsg.str();
    }
    mess << "!!!!Finished thread!!!!\n";
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << mess.str();
    mess.str("");
    (*runningFirmwareUpdate)-=1;
    return NULL;
  }







  /**
   * \brief Structure to hold the parameters for the global Fed9U function formatCfFirmware.
   *
   * The structure contains the following data members.
   * \li 0 Fed9UDescription* Pointer to the Fed9UDescription that is used on the crate.
   * \li 1 ostringstream* Pointer to an output string stream where all the messages from the FED can be written to.
   * \li 2 unsigned long* Pointer to an unsigned long that keeps track of how many times the function has been created.
   *       Used to keep track of how many threads are running.
   */
  struct Fed9UFormatParam {
    /*public:*/
    /**
     * \name Fed9UFormatParam data members.
     */
    //@{
    Fed9UDescription* mDesc;
    u32* mRunningFormatCfFirmware;
    //@}

    /**
     * \brief Constructor.
     *
     * Initialises the data members to the given values.
     */
    Fed9UFormatParam(Fed9UDescription* desc, u32* runningFormatCfFirmware) :
      mDesc(desc), mRunningFormatCfFirmware(runningFormatCfFirmware)
    { }
  };

  /**
   * \brief Used to format a compact flash firmware image on a specific FED in a crate, designed to be used with the pthread library.
   * \param param This is a void* that points to a Fed9UFormatParam struct containing the parameters for this function.
   * \return void* Enforced return type by the pthread library.
   *
   * This global Fed9U function is thread safe and keeps track of how many times it exists in separate threads.
   */
  void* formatCfFirmware(void* param) {
    //Initialise the local parameters from the void pointer.
    Fed9UFormatParam* dataStruct( reinterpret_cast<Fed9UFormatParam*>(param) );

    //Increment the number of running functions.
    ++*(dataStruct->mRunningFormatCfFirmware);

    //Try and create a FED.
    Fed9UDevice* pFed(NULL);
    try {
      pFed = new Fed9UDevice( *(dataStruct->mDesc) );
      //Format the firmware.
      if (pFed != NULL)
	pFed->setEraseCfImage();

    } catch ( ICUtils::ICException & exc) {
      std::ostringstream errMsg;
      errMsg <<  "Caught an ICUtils::ICException. Failed to format the CF card." << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << errMsg.str();
    } catch ( ... ) {
      std::ostringstream errMsg;
      errMsg << "Caught an unknown exception. Failed to format the CF card." << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_CRITICAL) << errMsg.str();
    }

    //Delete the Fed9UDevice we had.
    if (pFed != NULL) {
      delete pFed;
      pFed= NULL;
    }

    std::ostringstream msg;
    msg << "Sucessfully formatted the FED in slot " << static_cast<u16>( dataStruct->mDesc->getSlotNumber() ) << endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_MINIMAL) << msg.str();
    msg.str("");

    //Done, decrement the number of the threads running and then return.
    --*(dataStruct->mRunningFormatCfFirmware);
    return NULL;
  }//formCfFirmware(void* param)


}//namespace Fed9U
