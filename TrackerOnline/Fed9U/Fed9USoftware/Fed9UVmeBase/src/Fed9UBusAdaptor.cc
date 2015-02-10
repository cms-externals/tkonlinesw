/**Jonathan Fulcher*/
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
#include "Fed9UBusAdaptor.hh"
#include "Fed9UDescriptionException.hh"

#include <fstream>

#define FED9U_HAL_INTERFACE_MAX_ADAPTORS 10

namespace Fed9U {

  pthread_mutex_t Fed9UBusAdaptor::busAdaptorMutex = PTHREAD_MUTEX_INITIALIZER;

   void Fed9UBusAdaptor::resetBus() throw (Fed9UVmeBaseException)
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
          dynamic_cast<CAEN2718LinuxPCIBusAdapter*>(busAdaptor_)->resetBus();
#else
          dynamic_cast<CAENLinuxBusAdapter*>(busAdaptor_)->resetBus();
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

                                                                                              
 
    Fed9UBusAdaptor::Fed9UBusAdaptor( Fed9UHalBusAdaptor adaptorType, u32 crateNumber) : busAdaptorType_(adaptorType),crateNumber_(crateNumber) {

    //Protect ourselves against exceptions that could be thrown and prevent the mutex from being released.
    try {
      //See if we can acquire a mutex lock before we are allowed to update the number of people accessing the bus adaptor
      //or trying to create the static objects for the crate. Bad things happen if two threads try and create them at the same time.
      int errLock( pthread_mutex_lock(&busAdaptorMutex) );
      //If we got the lock or already have the lock.
      if (0 == errLock || EDEADLK == errLock) {
	// use this if we are using multiple adaptor cards in same pc

	  if (1==0) {
	    //do nothing
	  }
#ifdef BUILD_SBS620
	else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_SBS ) {
	  busAdaptor_ = new SBS620x86LinuxBusAdapter(crateNumber);
	}
#endif
	  
#ifdef BUILD_CAEN_PCI
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_PCI) {
#ifdef HAL_VER0306
	    busAdaptor_ = new CAEN2718LinuxPCIBusAdapter(crateNumber); 
#else
	    busAdaptor_ = new CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V2718,crateNumber,0,HAL::CAENLinuxBusAdapter::A3818); 
#endif
	  }
#endif
	  
#ifdef BUILD_CAEN_USB
	  else if (adaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_USB) {
#ifdef HAL_VER0306
	    busAdaptor_ = new CAEN1718LinuxUSBBusAdapter(crateNumber);
#else
	    busAdaptor_ = new CAENLinuxBusAdapter(HAL::CAENLinuxBusAdapter::V1718,crateNumber);
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
	    busAdapter = new VMEDummyBusAdapter();
	  }
#endif
	  

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
      std::string errMsg("HardwareAccessException caught while trying to construct Fed9UBusAdaptor.\n");
      //Clean up after construction.

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
      pthread_mutex_unlock(&busAdaptorMutex);
      //Rethrow the caught exception.
      RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, errMsg.c_str()));
    }
    catch (...) {
      std::string errMsg("Unknown exception caught while trying to construct Fed9UHalInterface.\n");
      //Clean up after construction.
      pthread_mutex_unlock(&busAdaptorMutex);
      //Rethrow the caught exception.
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UHALINTERFACE, errMsg.c_str()));
    }

  }//end constructor


  Fed9UBusAdaptor::~Fed9UBusAdaptor() {
    //Lock the shared resources before destructing the object.
    int errLock( pthread_mutex_lock(&busAdaptorMutex) );

    //If we got the lock or already have the lock.
    if (0 == errLock || EDEADLK == errLock) {
      //We have the lock.
      mLockedMutex = true;
	delete busAdaptor_;
    busAdaptor_ = NULL;
      }
      //Unlock the resources now we have destructed the object.
      pthread_mutex_unlock(&busAdaptorMutex);
      mLockedMutex = false;
    }
  }
  
  
  
  
  
  









