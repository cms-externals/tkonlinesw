/**Jonathan Fulcher 2011*/

/**This class contains methods to write and read to
   a FED using the HAL library. There are different ways to read
   and write, either to/from a single register, or from a block of 
   registers using a VME block read. This class works with the HAL
   address table, the name of which is specified in a constructor argument.
   One can also specifiy a base address and a crate number (default 0). An instance of this
   class can therefore only act as an interface for a single FED board.*/

#ifndef _Fed9UBusAdaptor_H_
#define _Fed9UBusAdaptor_H_

#include "TypeDefs.hh"
#include "Fed9UVmeBaseException.hh"

#if FED9U_XDAQ_VERSION > 37
#include "hal/VMEDevice.hh"
#include "hal/VMEAddressTable.hh"
#include "hal/VMEAddressTableASCIIReader.hh"
#include "hal/VMEBusAdapterInterface.hh"
#else
#include "VMEDevice.hh"
#include "VMEAddressTable.hh"
#include "VMEAddressTableASCIIReader.hh"
#include "VMEBusAdapterInterface.hh"
#endif

#include <pthread.h>
#include <string>
#include <vector>

#define FED9U_BUILD_BUS_ERR

#ifdef BUILD_DUMMY
#if FED9U_XDAQ_VERSION > 37
#include "hal/VMEDummyBusAdapter.hh"
#else
#include "VMEDummyBusAdapter.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#ifdef BUILD_NIVXI
#if FED9U_XDAQ_VERSION > 37
#include "hal/MXI2x86LinuxBusAdapter_test.hh"
#else
#include "MXI2x86LinuxBusAdapter_test.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#ifdef BUILD_SBS620
#if FED9U_XDAQ_VERSION > 37
#include "hal/SBS620x86LinuxBusAdapter.hh"
#else
#include "SBS620x86LinuxBusAdapter.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#ifndef HAL_VER0306

#if ( defined(BUILD_CAEN_PCI) || defined(BUILD_CAEN_USB) )
#if FED9U_XDAQ_VERSION > 37
#include "hal/CAENLinuxBusAdapter.hh"
#else
#include "CAENLinuxBusAdapter.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#else

#ifdef BUILD_CAEN_PCI
#if FED9U_XDAQ_VERSION > 37
#include "hal/CAEN2718LinuxPCIBusAdapter.hh"
#else
#include "CAEN2718LinuxPCIBusAdapter.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#ifdef BUILD_CAEN_USB
#if FED9U_XDAQ_VERSION > 37
#include "hal/CAEN1718LinuxUSBBusAdapter.hh"
#else
#include "CAEN1718LinuxUSBBusAdapter.hh"
#endif
#undef FED9U_BUILD_BUS_ERR
#endif

#endif

#ifdef FED9U_BUILD_BUS_ERR 
#error unable to define bus adaptor
#endif

namespace Fed9U {

#ifndef HAL_VER0306
  using namespace HAL;
#endif

  //using.*std::vector;
  //using.*std::string;

class Fed9UBusAdaptor
{
public:
 
  /**Constructor. This takes as input the base address of the FED and the name of the HAL address table file. One can also
     optionally pass a crate number (which in effect selects the PCI-VME interface card in the PC). */

  Fed9UBusAdaptor( Fed9UHalBusAdaptor adaptorType,u32 crateNumber );

  ~Fed9UBusAdaptor();

  void resetBus() throw (Fed9UVmeBaseException);

private:
  VMEBusAdapterInterface * busAdaptor_;
  bool mLockedMutex;
  Fed9UHalBusAdaptor busAdaptorType_;
  u32 crateNumber_;
  static pthread_mutex_t busAdaptorMutex;
    
};

}

#endif



