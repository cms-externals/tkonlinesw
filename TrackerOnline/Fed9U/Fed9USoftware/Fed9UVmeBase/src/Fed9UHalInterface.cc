/**Matthew Pearson, RAL - May 2003*/
/**This class contains methods to write and read to
   a FED using the HAL library. There are different ways to read
   and write, either to/from a single register, or from a block of 
   registers using a VME block read. This class works with the HAL
   address table, the name of which is specified in a constructor argument.
   One can also specifiy a base address and a crate number (default 0). An instance of this
   class can therefore only act as an interface for a single FED board.*/
#include <inttypes.h>
#include <stdint.h>

//<JEC date=12/10/07> KH patch for XDAQ RPM build
#include <errno.h>
//</JEC>

#include "Fed9UWait.hh"
#include "Fed9UHalInterface.hh"
#include "Fed9UDescriptionException.hh"

#include <fstream>

#define FED9U_HAL_INTERFACE_MAX_ADAPTORS 10

namespace Fed9U {

  // First we initialize the static pointers for the busAdaptor to null
  // then the first time we construct one of these classes they will be constructed  based upon which bus adaptor type we demand

  std::vector<VMEAddressTableASCIIReader *> Fed9UHalInterface::addressTableReader(FED9U_HAL_INTERFACE_MAX_ADAPTORS,reinterpret_cast<VMEAddressTableASCIIReader *>(NULL));
  std::vector<VMEAddressTable *> Fed9UHalInterface::addressTable(FED9U_HAL_INTERFACE_MAX_ADAPTORS,reinterpret_cast<VMEAddressTable *>(NULL));
  std::vector<VMEBusAdapterInterface *> Fed9UHalInterface::busAdapter(FED9U_HAL_INTERFACE_MAX_ADAPTORS,reinterpret_cast<VMEBusAdapterInterface *>(NULL));
  std::vector<u32> Fed9UHalInterface::countDevices(10,0); // use this if we want to have multiple bus adaptors for daisy chained or multiple pci cards, 
  //default max number of cards on one pc is set to 10 (this is way too high )
  pthread_mutex_t Fed9UHalInterface::busAdaptorMutex = PTHREAD_MUTEX_INITIALIZER;

  
  // u32 Fed9UHalInterface::countDevices = 0; // use this if we don't want to have multiple bus adaptors for daisy chained or multiple pci cards
  //Constructor.
  //The address table location base address are passed to the constructor. The crate number is also optionally passed.
  Fed9UHalInterface::Fed9UHalInterface(u32 baseAddress, const std::string& theAddressTable, Fed9UHalBusAdaptor adaptorType, unsigned short crateNumber) : fedv1Device(NULL) , adaptorNumber(crateNumber),busAdaptorType_(adaptorType) {

    //Protect ourselves against exceptions that could be thrown and prevent the mutex from being released.
    try {
      //    addressTableReader(theAddressTable.c_str()),
      //addressTable("FED9Uv1 address map", addressTableReader),
#ifndef BUILD_DUMMY
      // busAdapter(crateNumber),
#endif
      // fedv1Device(addressTable, busAdapter, baseAddress),
    
      // we add a count to the device count for the specific bus adaptor ( can have more than one if we are daisy chained or using multiple PCI cards in one PC.
      // countDevices += 1; // use this if we are not using multiple adaptor cards

      //See if we can acquire a mutex lock before we are allowed to update the number of people accessing the bus adaptor
      //or trying to create the static objects for the crate. Bad things happen if two threads try and create them at the same time.
      int errLock( pthread_mutex_lock(&busAdaptorMutex) );
      //If we got the lock or already have the lock.
      if (0 == errLock || EDEADLK == errLock) {
	// use this if we are using multiple adaptor cards in same pc
	countDevices[adaptorNumber] += 1;
     
	//<JRF data="28/11/2007">
	// added the check to see that the address table file exists here rather than in the Fed9Udescription Class
	std::ifstream addressTableFileTest(theAddressTable.c_str());
	ICUTILS_VERIFYX(addressTableFileTest,Fed9UDescriptionException)(theAddressTable).msg("Failed to open Address table").error().code(Fed9UDescriptionException::ERROR_NO_ADDRESS_TABLE);
    	// </JRF>

	//std::cout << " construct new hal interface! " << std::endl;
	if (!addressTableReader[adaptorNumber])
	  addressTableReader[adaptorNumber] = new VMEAddressTableASCIIReader(theAddressTable.c_str());

	if (!addressTable[adaptorNumber])
	  addressTable[adaptorNumber] = new VMEAddressTable("FED9Uv1 address map", *addressTableReader[adaptorNumber]);

	//std::cout << " crate number is set to : " << crateNumber << " and the adaptorNumber is set to : " << adaptorNumber << std::endl;
	//std::cout << " we have " << countDevices[adaptorNumber] << " devices for for this adaptor number" << std::endl; 


	// now we create the bus adaptor based upon adaptorType
	if (!busAdapter[adaptorNumber]) {
	  if (1==0) {
	    //do nothing
	  }
#ifdef BUILD_SBS620
	else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_SBS ) {
	  busAdapter[adaptorNumber] = new SBS620x86LinuxBusAdapter(crateNumber);
	}
#endif
	  
#ifdef BUILD_CAEN_PCI
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_PCI) {
#ifdef HAL_VER0306
	    busAdapter[adaptorNumber] = new CAEN2718LinuxPCIBusAdapter(crateNumber); 
#else
	    busAdapter[adaptorNumber] = new CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V2718,crateNumber,0,HAL::CAENLinuxBusAdapter::A3818); 
#endif
	  }
#endif
	  
#ifdef BUILD_CAEN_USB
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_USB) {
#ifdef HAL_VER0306
	    busAdapter[adaptorNumber] = new CAEN1718LinuxUSBBusAdapter(crateNumber);
#else
	    busAdapter[adaptorNumber] = new CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V1718,crateNumber);
#endif
	  } 
#endif
	  
#ifdef BUILD_NIVXI
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_VXI) {
	    ICUTILS_VERIFYX(1==0,Fed9UVmeBaseException).error().msg("VXI bus adaptor not support in this verison of hal");
	  }
#endif
	  
#ifdef BUILD_DUMMY
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_DUMMY) {
	    busAdapter[adaptorNumber] = new VMEDummyBusAdapter();
	  }
#endif
	  
	}
	if (!fedv1Device)
	  fedv1Device = new VMEDevice(*addressTable[adaptorNumber],*busAdapter[adaptorNumber],baseAddress);    
	
	//Release the mutex. No point in releasing it before the if's as we would have to perform them anyway to see if it can be released earlier!
	pthread_mutex_unlock(&busAdaptorMutex);
      }//if locked
      else {
 	//Failed to acquire a lock and as a result we should prevent this object from being created.
 	THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Failed to acquire a resource lock while creating the Fed9UHalInterface object."));
      }

    }//try
    catch (HardwareAccessException &exc) {
      //What the error was.
      std::string errMsg("HardwareAccessException caught while trying to construct Fed9UHalInterface.\n");
      //Clean up after construction.
      countDevices[adaptorNumber]-=1;

      if (fedv1Device) {
	delete fedv1Device;
	fedv1Device = NULL;
      }
      if (countDevices[adaptorNumber] == 0) {
	if (busAdapter[adaptorNumber] != NULL) {
	  delete busAdapter[adaptorNumber];
	  busAdapter[adaptorNumber] = NULL;
	}
	if (addressTable[adaptorNumber] != NULL) {
	  delete addressTable[adaptorNumber];
	  addressTable[adaptorNumber] = NULL;
	}
	if (addressTableReader[adaptorNumber]) {
	  delete addressTableReader[adaptorNumber];
	  addressTableReader[adaptorNumber] = NULL;
	}
      }
      //Release the mutex, if we have locked it.
      pthread_mutex_unlock(&busAdaptorMutex);
      //Rethrow the caught exception.
      RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, errMsg.c_str()));
    }
    catch (Fed9UVmeBaseException& exc) {
      std::cout << "Caught a Fed9UVmeBaseException" << std::endl;
      //Could be thrown if we fail to get a lock, shouldn't have been anything locked though.
      //pthread_mutex_unlock(&busAdaptorMutex);
      RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, ""));
    }
    catch (Fed9UDescriptionException& exc) {
      std::cout << "Caught a Fed9UDescriptionException when looking for the HAL Address Table File" << std::endl;
      //will be thrown if the address table file is not found
      RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "No HAL address table file!!!"));
    }
    catch (std::exception &exc) {
      std::string errMsg("std::exception caught while trying to construct Fed9UHalInterface.\n");
      //Clean up after construction.
      countDevices[adaptorNumber]-=1;

      if (fedv1Device) {
	delete fedv1Device;
	fedv1Device = NULL;
      }
      if (countDevices[adaptorNumber] == 0) {
	if (busAdapter[adaptorNumber] != NULL) {
	  delete busAdapter[adaptorNumber];
	  busAdapter[adaptorNumber] = NULL;
	}
	if (addressTable[adaptorNumber] != NULL) {
	  delete addressTable[adaptorNumber];
	  addressTable[adaptorNumber] = NULL;
	}
	if (addressTableReader[adaptorNumber]) {
	  delete addressTableReader[adaptorNumber];
	  addressTableReader[adaptorNumber] = NULL;
	}
      }
      //Release the mutex.
      pthread_mutex_unlock(&busAdaptorMutex);
      //Rethrow the caught exception.
      RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, errMsg.c_str()));
    }
    catch (...) {
      std::string errMsg("Unknown exception caught while trying to construct Fed9UHalInterface.\n");
      //Clean up after construction.
      countDevices[adaptorNumber]-=1;

      if (fedv1Device) {
	delete fedv1Device;
	fedv1Device = NULL;
      }
      if (countDevices[adaptorNumber] == 0) {
	if (busAdapter[adaptorNumber] != NULL) {
	  delete busAdapter[adaptorNumber];
	  busAdapter[adaptorNumber] = NULL;
	}
	if (addressTable[adaptorNumber] != NULL) {
	  delete addressTable[adaptorNumber];
	  addressTable[adaptorNumber] = NULL;
	}
	if (addressTableReader[adaptorNumber]) {
	  delete addressTableReader[adaptorNumber];
	  addressTableReader[adaptorNumber] = NULL;
	}
      }
      //Release the mutex.
      pthread_mutex_unlock(&busAdaptorMutex);
      //Rethrow the caught exception.
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, errMsg.c_str()));
    }

  }//end constructor


  Fed9UHalInterface::~Fed9UHalInterface() {
    //Lock the shared resources before destructing the object.
    int errLock( pthread_mutex_lock(&busAdaptorMutex) );

    //If we got the lock or already have the lock.
    if (0 == errLock || EDEADLK == errLock) {
      //We have the lock.
      mLockedMutex = true;
      countDevices[adaptorNumber]-=1;
      if (fedv1Device)
	delete fedv1Device;
    
      //    if (countDevices == 0 && false) {
      if (countDevices[adaptorNumber] == 0 && false) {//In a single thread environment it gets deleted everytime the calling object goes out of scope. Not what we want. Think of better test.
	delete busAdapter[adaptorNumber];
	delete addressTable[adaptorNumber];
	delete addressTableReader[adaptorNumber];
	busAdapter[adaptorNumber] = NULL;
	addressTable[adaptorNumber] = NULL;
	addressTableReader[adaptorNumber] = NULL;
      }
      //Unlock the resources now we have destructed the object.
      pthread_mutex_unlock(&busAdaptorMutex);
      mLockedMutex = false;
    }
  }


  //**************************************************************************************
  //Method which does a simple write to a FED register (specified by fedRegister). The data written is
  //in the data parameter.
  void Fed9UHalInterface::writeRegister(u32 data, const std::string &fedRegister, HalVerifyOption verifyFlag, u32 offset) throw (Fed9UVmeBaseException)
  {
    try {
#ifdef DEBUG_HAL_INTERFACE
      std::cout << "************ write register **************" << std::endl;
#endif
#if FED9U_XDAQ_VERSION >= 37
      fedv1Device->unmaskedWrite(fedRegister.c_str(), data, verifyFlag, static_cast<u16>(offset));
#else
      fedv1Device->unmaskedWrite(fedRegister.c_str(), data, verifyFlag, offset);
#endif
    }
    catch(HardwareAccessException &e) {
      std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::writeRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (std::exception &e) {
      std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::writeRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (...) {
      std::string theErrorMessage = "Unknown exception caught in Fed9UHalInterface::writeRegister. Register is: " + fedRegister;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    
  }

  /**Method which does a block write to a FED register (specified by fedRegister). The HalVerifyOption is an enum and can be set
     to either HAL_NO_VERIFY or HAL_DO_ICUTILS_VERIFY. The data to be written is specified in the buffer parameter. An offset can
     also be specified.*/
  void Fed9UHalInterface::blockWriteRegister(const std::string &fedRegister, 
					     const u8 * buffer, 
					     u32 offset,
					     u32 length, 
					     HalVerifyOption verifyFlag) throw (Fed9UVmeBaseException) {
    try {
#if FED9U_XDAQ_VERSION >= 37
      //<JEC date=13/9/07> KH change for SLC4/GCC3.4
      //      fedv1Device->writeBlock( fedRegister.c_str(), length, reinterpret_cast<char*>(buffer), verifyFlag, HAL_DO_INCREMENT, static_cast<u16>(offset) );
      fedv1Device->writeBlock( fedRegister.c_str(), length, reinterpret_cast<char*>(const_cast<u8*>(buffer)), verifyFlag, HAL_DO_INCREMENT, static_cast<u16>(offset) );
      //</JEC>
#else
       fedv1Device->writeBlock( fedRegister.c_str(), length, reinterpret_cast<char*>(buffer), verifyFlag, HAL_DO_INCREMENT, offset );
#endif
    }catch(HardwareAccessException &e) {
      std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::blockWriteRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (std::exception &e) {
      std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::blockWriteRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (...) {
      std::string theErrorMessage = "Unknown exception caught in Fed9UHalInterface::blockWriteRegister. Register is: " + fedRegister;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
  }


  //**************************************************************************************
  //Method which does a simple masked write to a FED register (specified by fedRegister). The data written is
  //in the data perameter.
  void Fed9UHalInterface::maskedWriteRegister(u32 data, const std::string &fedRegister, HalVerifyOption verifyFlag ) throw (Fed9UVmeBaseException)
  {
    try {
#ifdef DEBUG_HAL_INTERFACE
      std::cout << "************ write register **************" << std::endl;
#endif
      fedv1Device->write(fedRegister.c_str(), data, verifyFlag);
    }
    catch(HardwareAccessException &e) {
      std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::maskedWriteRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (std::exception &e) {
      std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::maskedWriteRegister. Register is: " + fedRegister;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    catch (...) {
      std::string theErrorMessage = "Unknown exception caught in Fed9UHalInterface::maskedWriteRegister. Register is: " + fedRegister;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
    }
    
  }
  
  //*********************************************************************************************
  //Method to write to the FED using HAL. The vector passed to this
  //will contain the command (in the first element) and data.
  //This writes to the ARGUMENT and WRITE registers (for serial commands to FED FE or BE).
  void Fed9UHalInterface::writeSerialCommand(const std::vector<u32> &commandLong) throw (Fed9UVmeBaseException)
  {
   u32 trials=10;
   do { //we must try again if the command fails
    try {
      ICUTILS_VERIFYX(commandLong.size()>=2,Fed9UVmeBaseException)(commandLong.size()).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Too short commandLong in Fed9UHalInterface::writeSerialCommand.");
      
      //Initially write to the argument area (at zero offset).
#if FED9U_XDAQ_VERSION >= 37
      unsigned int offset=0x0;
#else
      u32 offset=0x0;
#endif      
      //Loop over each 32-bit word in the vector of unsigned integers and 
      //write each one at a place 32-bits after the previous one (0x04=32 bits).
      //Also write the number of 32-bit words to the write area 
      //of the FED memory map (with offset equal to 0x800).
      
      std::vector<u32>::const_iterator i;
      for (i=commandLong.begin(); i!=commandLong.end(); ++i) {   //Loop over all the u32 words
	fedv1Device->unmaskedWrite("ARGUMENT",*i,HAL_NO_VERIFY,offset);
	offset = offset + 0x04; //Increment by 32-bits (ie. 4-bytes). 
      }
      //Clear the next register in the ARGUMENT area after the command+data has been written.
      fedv1Device->unmaskedWrite("ARGUMENT",0,HAL_NO_VERIFY,offset);
      offset = 0x0;
      
      //Subtract 1 from #no of words and write this to WRITE area.
      fedv1Device->unmaskedWrite("WRITE",commandLong.size()-1,HAL_NO_VERIFY,offset); 
      //std::cout << " length of command = " << dec << commandLong.size() << std::endl;

      // now we poll the fed to make sure that it is ready for the next command
      u32 done = 0, timeout = 0;
      
      do {
	timeout++;
	readRegister("STATUS_SERIAL", done);
	//std::cout << "polled serial status reg" << std::endl;
      } while (!done && timeout < 1000);
      ICUTILS_VERIFYX(timeout < 1000,Fed9UVmeBaseException)(timeout).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Timeout on serial command polling");
      trials = 0; //completed ok so let's go!
    }
    catch(HardwareAccessException &e) {
      // failed to make some transfer, try agani!!!
      trials -=1;
      if (trials > 0) {
      u32 done = 0, timeout = 0;

            do {
                timeout++;
                    readRegister("STATUS_SERIAL", done);
                        //std::cout << "polled serial status reg" << std::endl;
                              } while (!done && timeout < 1000);
                              ICUTILS_VERIFYX(timeout < 1000,Fed9UVmeBaseException)(timeout).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Timeout on serial command polling");
                                    
        continue;
        }
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::writeSerialCommand."));
    }
    catch (std::exception &e) {
      // failed to make some transfer, try agani!!!
      trials -=1;
      if (trials > 0) {
      u32 done = 0, timeout = 0;

            do {
                timeout++;
                    readRegister("STATUS_SERIAL", done);
                        //std::cout << "polled serial status reg" << std::endl;
                              } while (!done && timeout < 1000);
                                    ICUTILS_VERIFYX(timeout < 1000,Fed9UVmeBaseException)(timeout).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Timeout on serial command polling");
                                    
          continue;
                       }   
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"std::exception caught in Fed9UHalInterface::writeSerialCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"Unknown exception caught in Fed9UHalInterface::writeSerialCommand."));
    } 
    } while ( trials > 0) ;
  }
  

/**Method to write in block mode to the FED using HAL. The buffer passed to this
     will contain the complete block of serial commands to write to the fed, the length is limited to the maximum size of the serial command buffer.
     The structure of the data command block that this method handles is simply complete single commands packed one 
     after the other.  The commands are written exactly as they appear in this buffer, they are not separated in any way
     and no zero padding is applied between commands
     It writes to the ARGUMENT and WRITE registers (for serial commands to FED FE or BE) - see the HAL 
     address table for the location of these registers.*/
  
  void Fed9UHalInterface::blockWriteSerialCommand(const u8 * command, const u32 length) throw (Fed9UVmeBaseException) { 
    try {
      ICUTILS_VERIFYX(length<=FED9U_VME_SERIAL_COMMAND_BUFFER_SIZE,Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("command vector to large in Fed9UHalInterface::blockWriteSerialCommand.");

#if FED9U_XDAQ_VERSION >= 37
      u16 offset = 0;
#else
      u32 offset = 0;
#endif
      //std::cout << "Writing block in hal!!" << std::endl;
      blockWriteRegister(std::string("ARGUMENT_BLT"), command, offset, length*4, HAL_NO_VERIFY);
      //std::cout << "Wrote block in hal!!" << std::endl;
      fedv1Device->unmaskedWrite("WRITE", length-1, HAL_NO_VERIFY, 0); //TODO JF might not need to subtract 1 from the length!!!!!!!!!!!!11
      //std::cout << "Wrote serial command go in hal" << std::endl;

      // now we poll the fed to make sure that it is ready for the next command
      u32 done = 0, timeout = 0;
      
      do {
	timeout++;
	readRegister("STATUS_SERIAL", done);
	//std::cout << "polled serial status reg" << std::endl;
      } while (!done && timeout < 1000);
      ICUTILS_VERIFYX(timeout < 1000,Fed9UVmeBaseException)(timeout).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Timeout on serial command polling");

    }
    catch(HardwareAccessException &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"std::exception caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
    catch (...) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!! caught unknown exception!!!!!!!!!!!!!!!!!!!!" << std::endl;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"Unknown exception caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
  }



  /**Method to write in block mode to the FED using HAL. The buffer passed to this
     will contain the complete block of serial commands to write to the fed, the length is unlimited.
     This method takes this command buffer and splits it up into manageable chuncks which it loads one after the other.
     The structure of the data command block that this method handles is simply complete single commands packed one 
     after the other. however, the individual commands must all be the same number of words. this length is passed in as a parameter
     It writes to the ARGUMENT and WRITE registers (for serial commands to FED FE or BE) - see the HAL 
     address table for the location of these registers.*/
  void Fed9UHalInterface::blockWriteSerialCommandChunks(const u8 * command, const u32 length, const u32 individualCommandLength) throw (Fed9UVmeBaseException) { 

    u32 * subBufferBlockCommand = new u32[FED9U_VME_SERIAL_COMMAND_BUFFER_SIZE];
    try {
      // let's set up the variables required to define the structure of the individual blocks of commands
      // this code allows you to write variable size buffers of varying numbers of commands padded with zeros
#if FED9U_XDAQ_VERSION >= 37
      u16 offset = 0;
#else
      u32 offset = 0;
#endif

      // first we work out the optimum chunk size by progressively dividing the total command length 
      // by increasing integers until the block size is smaller than available VME buffer size and it is
      // also perfectly divisible by i with no remainder, thus ensuring equal chunk sizes and ensuring each block is a multiple of the individualcommandlength

      u32 totalblocksize;
      u32 i=1;
      while ( ( ( (totalblocksize = length / i) > FED9U_VME_SERIAL_COMMAND_BUFFER_SIZE ) 
		|| ( length % i ) != 0 
		|| ( totalblocksize % individualCommandLength) != 0 ) 
	      && i <= (length / individualCommandLength) ) {
	i++;
      }



      //u32 numbercommandsperbuffer = totalblocksize/individualCommandLength; // we have previously ensured that totalblocksize is divisible by individualCommandLength
      //      numbercommandsperbuffer = 1;
      //totalblocksize = 4;
      //std::cout << "individualCommandLength = " << individualCommandLength << "totalblocksize = " << totalblocksize << std::endl;
      u32 zerobufferspacerlength = 0;// (totalblocksize - numbercommandsperbuffer*individualCommandLength)/numbercommandsperbuffer;
      //std::cout << "zerobufferspacerlength= " << zerobufferspacerlength <<std::endl;

      // this is the size of the subblock not including the zero buffer space
      u32 subblocksize = ( totalblocksize / (individualCommandLength   + zerobufferspacerlength ) ) * individualCommandLength; 

      u32 numblocks = length / subblocksize + ( (length % subblocksize > 0) ?1:0);
      u32 lastblocksize = length % subblocksize;
      u32 allbutlastblocksize = subblocksize;
      //std::cout << "length = " << length << "numblocks = " << numblocks << ", subblocksize = " << subblocksize << "' lastblocksize = " << lastblocksize << std::endl; 
      for (u32 k = 0; k < numblocks; k++){
	offset = 0;
	u32 len=0;
	if ( (k == numblocks - 1) && (length % subblocksize > 0) )
	  subblocksize = lastblocksize;
	for (u32 i = 0; i < subblocksize; i++) {
	  //fedv1Device->unmaskedWrite("ARGUMENT",(reinterpret_cast<u32*>(command))[k*allbutlastblocksize+ i],HAL_DO_VERIFY,offset);
	  //<JEC date=13/9/07>  KH change for SLC4/GCC3.4
	  //	  subBufferBlockCommand[len] = (reinterpret_cast<u32*>(command))[k*allbutlastblocksize + i];
	  subBufferBlockCommand[len] = (reinterpret_cast<u32*>(const_cast<u8*>(command)))[k*allbutlastblocksize + i];
	  //</JEC>
	  offset = offset + 0x04; //Increment by 32-bits (ie. 4-bytes). 
	  len ++;
	  //std::cout << "length incremented to : " << len 
	  //       << " offset is " << offset 
	  //      << " last buffer content is: " << std::hex << subBufferBlockCommand[len-1] << std::endl;
	  if ((i+1)%individualCommandLength==0) {
	    //std::cout << "add zero words!!!" << std::endl;
	    for (u32 j=0; j<zerobufferspacerlength; j++) {
	      //fedv1Device->unmaskedWrite("ARGUMENT",0,HAL_DO_VERIFY,offset);
	      subBufferBlockCommand[len] = 0x00000000;
	      offset += 0x04;
	      if(i<subblocksize)
		len++;
	      //  std::cout << "length incremented to : " << len << " offset is " << offset 
	      //   << " last buffer content is: " << std::hex << subBufferBlockCommand[len - 1] << std::endl;
	    }
	    //	    std::cout << "length incremented to : " << len << std::endl;
	  }
          /*fedv1Device->unmaskedWrite("WRITE", 10, HAL_NO_VERIFY, 0); //TODO JF might not need to subtract 1 from the length!!!!!!!!!!!!
	    u32 done = 0;
	    do {
	    readRegister("STATUS_SERIAL", done);
	    } while (!done);*/
	}
	
	
	//Clear the next register in the ARGUMENT area after the command+data has been written.
	subBufferBlockCommand[len] = 0x00000000;
	
	/*if (k==0) {
	 std::cout << "we only show the first buffer block to write, which is:" << std::endl;
		  for (u32 l=0;l<len;l++){
	std::cout << hex << setw(8) <<  std::setfill('0') << subBufferBlockCommand[l] << dec << std::endl;
	 }
	 std::cout << "length of buffer command = " << len << std::endl;
	 }*/
	
	//std::cout << "just about to write block!" << std::endl;
	blockWriteSerialCommand(reinterpret_cast<u8*>(subBufferBlockCommand), len);
	//std::cout << "wrote block!" << std::endl;

      }
      if (subBufferBlockCommand)
	delete[] subBufferBlockCommand;
    }
    catch(HardwareAccessException &e) {
      if (subBufferBlockCommand)
	delete[] subBufferBlockCommand;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
    catch (std::exception &e) {
      if (subBufferBlockCommand)
	delete[] subBufferBlockCommand;
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"std::exception caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
    catch (...) {
      if (subBufferBlockCommand)
	delete[] subBufferBlockCommand;
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"Unknown exception caught in Fed9UHalInterface::blockWriteSerialCommand."));
    }
  }


  //*****************************************************************************************
  //HAL read method. This takes in a vector which contains the read command in the 1st element.
  //The length is used to calculate how many 32-bit words to read back (the length unit is bits, not bytes).
  //The boolean rightShift is used to decide if one right shifts the last element
  //of the argument vector (which contains the words read back from the FED) so that the
  //LSB is on the right. This method reads data from the FE or BE FPGA chips.
  std::vector<u32> Fed9UHalInterface::readSerialCommand(u32 commandLong, 
							     u32 length,
							     bool rightShift) throw (Fed9UVmeBaseException)
  {
    try {
      //Check for input errors
      ICUTILS_VERIFYX(length>=1,Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Length parameter less than 1 in Fed9UHalInterface::readSerialCommand.");
      //Comment out the following line if using the read_memory command in UserMenuInterface (just for testing!).
      ICUTILS_VERIFYX(commandLong!=0,Fed9UVmeBaseException)(commandLong).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("commandLong is zero in Fed9UHalInterface::readSerialCommand.");
      
      //Calculate the number of 32-bit words to read back
      u32 newLength = length;
      if ((newLength%32)==0) newLength--; //So that we don't read back an extra word.
      const unsigned short numberOf32bitWords = static_cast<int>(static_cast<float>(newLength + 32) / 32.0); 
      
      //Initially write to the read area with no offset (at address 0x804).
#if FED9U_XDAQ_VERSION >= 37
      u16 offset = 0;
#else
      u32 offset = 0;
#endif
      
      try {
	//Write the read command to the read area of FED memory
#ifdef DEBUG_HAL_INTERFACE
	std::cout << "************ write register **************" << std::endl;
#endif
	fedv1Device->unmaskedWrite("READ",commandLong,HAL_NO_VERIFY,offset);
      }
      catch(HardwareAccessException &e) {
	RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::readSerialCommand."));
      }
      
      std::vector<u32> readArguments;
      //Read back the arguments from the argument area of FED memory 
      //and increment in 32-bit steps (for a maximum number
      //of times defined by numberOf32bitWords. 
      offset = 0x0;
#if FED9U_XDAQ_VERSION >= 37
      unsigned int tempRead = 0;
#else
      u32 tempRead = 0;
#endif
      try {
	for (int i=1; i<=numberOf32bitWords; ++i) {
#ifdef DEBUG_HAL_INTERFACE
	  std::cout << "************ read register **************" << std::endl;
#endif
	  fedv1Device->unmaskedRead("ARGUMENT",&tempRead,offset);
	  readArguments.push_back(tempRead);
	  //	  std::cout << "serial command value read back = " << hex << tempRead << dec << std::endl;
	  offset = offset + 0x04; //Offset by 4 bytes.
	  tempRead = 0;
	}
      }
      catch(HardwareAccessException &e) {
	RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::readSerialCommand."));
      }
      
      if (rightShift) {
	readArguments[readArguments.size()-1] = readArguments[readArguments.size()-1] >> (32-(length%32));
      }
      
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"std::exception caught in Fed9UHalInterface::readSerialCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"Unknown exception caught in Fed9UHalInterface::readSerialCommand."));
    }
    
  }
  
  
  //****************************************************************************************************
  //This method is to read data from the CMS delay chips (CDCs). The command is accepted as a vector of
  //u32 integers, which should have only two elements. The CDC command section should be in
  //the first element and the FPGA command section should be in the second element.
  //The data is returned in a vector. The read command is passed to this method as well
  //as a parameter specified in a Fed9UVmeBase method telling it how many bits to read back. The boolean
  //parameter tells the method to right-shift the last element of the arguments read back, so that the
  //LSB is on the right.
  std::vector<u32> Fed9UHalInterface::readFromDelayChipCommand(std::vector<u32> command,
                                          u32 length,
                                          bool rightShift) throw (Fed9UVmeBaseException) {
    try {

      //Check for input errors
      ICUTILS_VERIFYX(length>=1,Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("Length parameter less than 1 in Fed9UHalInterface::readSerialCommand.");
      //Comment out the following line if using the read_memory command in UserMenuInterface (just for testing!).
      ICUTILS_VERIFYX( (command.size()!=0 && command.size()<3), Fed9UVmeBaseException)(command.size()).code(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE).error().msg("command vector size is zero or greater than 2 in Fed9UHalInterface::readSerialCommand.");

      //Calculate the number of 32-bit words to read back
      u32 newLength = length;
      if ((newLength%32)==0) newLength--; //So that we don't read back an extra word.
      const unsigned short numberOf32bitWords = static_cast<int>(static_cast<float>(newLength + 32) / 32.0); 
      
      try {
	//Write to the two areas of the VME FPGA that are reserved for read command >32bits.
#ifdef DEBUG_HAL_INTERFACE
	std::cout << "************ ext_read register **************" << std::endl;
#endif
	maskedWriteRegister(command[1], "EXT_READ_LSB", HAL_NO_VERIFY);
	maskedWriteRegister(command[0], "EXT_READ_MSB", HAL_NO_VERIFY);
      }
      catch(HardwareAccessException &e) {
	RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::readSerialCommand."));
      }

      std::vector<u32> readArguments;
      //Read back the arguments from the argument area of FED memory 
      //and increment in 32-bit steps (for a maximum number
      //of times defined by numberOf32bitWords. 
#if FED9U_XDAQ_VERSION >= 37
      unsigned int offset = 0x0;
      uint32_t tempRead = 0x0;
#else
      u32 offset = 0x0;
      u32 tempRead = 0x0;
#endif
      try {
	for (int i=1; i<=numberOf32bitWords; ++i) {
#ifdef DEBUG_HAL_INTERFACE
	  std::cout << "************ read register **************" << std::endl;
#endif
	  fedv1Device->unmaskedRead("ARGUMENT",&tempRead,offset);
	  readArguments.push_back(tempRead);
	  //std::cout << "serial command value read back = " << hex << tempRead << dec << std::endl;
	  offset = offset + 0x04; //Offset by 4 bytes.
	  tempRead = 0;
	}
      }
      catch(HardwareAccessException &e) {
	RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"HardwareAccessException caught in Fed9UHalInterface::readSerialCommand."));
      }

      if (rightShift) {
	readArguments[readArguments.size()-1] = readArguments[readArguments.size()-1] >> (32-(length%32));
      }
      
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"std::exception caught in Fed9UHalInterface::readSerialCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE,"Unknown exception caught in Fed9UHalInterface::readSerialCommand."));
    }
    
  }





  //**********************************************************************************************
  //Method which reads from the fedRegister register. The read value is returned in readArguments.
  //An offset can be specified (default is zero).
    void Fed9UHalInterface::readRegister(const std::string &fedRegister, u32 &readArguments, u32 offset) throw (Fed9UVmeBaseException)
      {
	try {
#ifdef DEBUG_HAL_INTERFACE
	  std::cout << "************ read register **************" << std::endl;
#endif
#if FED9U_XDAQ_VERSION >= 37
	  //<JEC date=13/9/07>  KH change for SLC4/GCC3.4
	  //	  fedv1Device->unmaskedRead(fedRegister.c_str(), &static_cast<uint32_t>(readArguments), static_cast<u16>(offset));
	  fedv1Device->unmaskedRead(fedRegister.c_str(), reinterpret_cast<uint32_t*>(&readArguments), static_cast<u16>(offset));
	  //</JEC>
#else
	  fedv1Device->unmaskedRead(fedRegister.c_str(), &readArguments, offset);
#endif
	}
	catch(HardwareAccessException &e) {
	  std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::readRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (std::exception &e) {
	  std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::readRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (...) {
	  THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UHalInterface::readRegister."));
	}
      }
    
    //**********************************************************************************************
    //Method which does a masked read from the fedRegister register. The read value is returned in readArguments.
    void Fed9UHalInterface::maskedReadRegister(const std::string &fedRegister, u32 &readArguments) throw (Fed9UVmeBaseException)
      {
	try {
#if FED9U_XDAQ_VERSION >= 37
	  //<JEC date=13/9/07>  KH change for SLC4/GCC3.4
	  //	  fedv1Device->read(fedRegister.c_str(), &static_cast<uint32_t>(readArguments));
	  fedv1Device->read(fedRegister.c_str(), reinterpret_cast<uint32_t*>(&readArguments));
	  //</JEC>
#else
	  fedv1Device->read(fedRegister.c_str(), &readArguments);
#endif
	}
	catch(HardwareAccessException &e) {
	  std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::maskedReadRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (std::exception &e) {
	  std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::maskedReadRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (...) {
	  THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UHalInterface::maskedReadRegister."));
	}
      }
    
    
    //**********************************************************************************************
    //Method which does a block read from the fedRegister register and modifies the data pointed at by readArguments. 
    //This is used to read data from an offset. The length will specify the size of the block.
    void Fed9UHalInterface::blockReadRegister(const std::string &fedRegister, 
					      char* const readArguments, 
					      u32 offset,
					      u32 length) throw (Fed9UVmeBaseException)
      {
	try {
	  //Set offset to zero for now to avoid warning. It may be used again in the future, so I don't want to change the interface....
	  offset = 0;
#ifdef DEBUG_HAL_INTERFACE
	  std::cout << "************ bLOCK READ register **************" << std::endl;
#endif
	  //Do a block read from an offset. Length will specify the size of the block to read.
	  fedv1Device->readBlock(fedRegister.c_str(), length, readArguments, HAL_DO_INCREMENT, offset);
	}
	catch(HardwareAccessException &e) {
	  std::string theErrorMessage = "HardwareAccessException caught in Fed9UHalInterface::blockReadRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (std::exception &e) {
	  std::string theErrorMessage = "std::exception caught in Fed9UHalInterface::blockReadRegister. Register is: " + fedRegister;
	  RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, theErrorMessage.c_str()));
	}
	catch (...) {
	  THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, "Unknown exception caught in Fed9UHalInterface::blockReadRegister."));
	}
      }


   void Fed9UHalInterface::resetBus() throw (Fed9UVmeBaseException) 
   {
     if (1==0) {
       //do nothing
     }
#ifdef BUILD_SBS620
     else if (busAdaptorType_ == FED9U_HAL_BUS_ADAPTOR_SBS ) {
     }
#endif
     
#ifdef BUILD_CAEN_PCI
     else if (busAdaptorType_ == FED9U_HAL_BUS_ADAPTOR_CAEN_PCI) {
#ifdef HAL_VER0306
       dynamic_cast<CAEN2718LinuxPCIBusAdapter*>(busAdapter[adaptorNumber])->resetBus(); 
#else
       dynamic_cast<CAENLinuxBusAdapter*>(busAdapter[adaptorNumber])->resetBus(); 	 
#endif
     }
#endif
     
#ifdef BUILD_CAEN_USB
     else if (busAdaptorType_ == FED9U_HAL_BUS_ADAPTOR_CAEN_USB) {
#ifdef HAL_VER0306
#else
#endif
     } 
#endif
     
#ifdef BUILD_NIVXI
     else if (busAdaptorType_ == FED9U_HAL_BUS_ADAPTOR_VXI) {
       ICUTILS_VERIFYX(1==0,Fed9UVmeBaseException).error().msg("VXI bus adaptor not support in this verison of hal");
     }
#endif
     
#ifdef BUILD_DUMMY
     else if (busAdaptorType_ == FED9U_HAL_BUS_ADAPTOR_DUMMY) {
     }
#endif
   }
}

  
  
  
  
  
  









