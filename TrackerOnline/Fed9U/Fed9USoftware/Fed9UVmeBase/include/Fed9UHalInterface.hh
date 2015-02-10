/**Matthew Pearson, RAL - May 2003*/

/**This class contains methods to write and read to
   a FED using the HAL library. There are different ways to read
   and write, either to/from a single register, or from a block of 
   registers using a VME block read. This class works with the HAL
   address table, the name of which is specified in a constructor argument.
   One can also specifiy a base address and a crate number (default 0). An instance of this
   class can therefore only act as an interface for a single FED board.*/

#ifndef _Fed9UHalInterface_H_
#define _Fed9UHalInterface_H_

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

class Fed9UHalInterface
{
public:
 
  /**Constructor. This takes as input the base address of the FED and the name of the HAL address table file. One can also
     optionally pass a crate number (which in effect selects the PCI-VME interface card in the PC). */

  Fed9UHalInterface(u32 baseAddress, const std::string& theAddressTable, Fed9UHalBusAdaptor adaptorType, unsigned short crateNumber);

  ~Fed9UHalInterface();

  /**Method which does a simple write to a FED register (specified by fedRegister). The HalVerifyOption is an enum and can be set
     to either HAL_NO_VERIFY or HAL_DO_ICUTILS_VERIFY. The data to be written is specified in the data parameter. An offset can
     also be specified.*/
  void writeRegister(u32 data, const std::string &fedRegister, HalVerifyOption verifyFlag = HAL_NO_VERIFY, u32 offset = 0) throw (Fed9UVmeBaseException);

  /**Method which does a block write to a FED register (specified by fedRegister). The HalVerifyOption is an enum and can be set
     to either HAL_NO_VERIFY or HAL_DO_ICUTILS_VERIFY. The data to be written is specified in the buffer parameter. An offset can
     also be specified.*/
  void blockWriteRegister(const std::string &fedRegister, 
			  const u8 *  buffer, 
			  u32 offset,
			  u32 length, 
			  HalVerifyOption verifyFlag = HAL_NO_VERIFY) throw (Fed9UVmeBaseException);
  
 
  /**Method which does a simple masked write to a FED register (specified by fedRegister). */
  void maskedWriteRegister(u32 data, const std::string &fedRegister, HalVerifyOption verifyFlag = HAL_NO_VERIFY) throw (Fed9UVmeBaseException);

  /**Method to write to the FED using HAL. The vector passed to this
     will contain the command (in the first element) and data in subsequent elements.
     This writes to the ARGUMENT and WRITE registers (for serial commands to FED FE or BE) - see the HAL 
     address table for the location of these registers.*/
  void writeSerialCommand(const std::vector<u32> &command) throw (Fed9UVmeBaseException);

  /**Method to write in block mode to the FED using HAL. The vector passed to this
     will contain the command (in the first element) and data in subsequent elements.
     This writes to the ARGUMENT and WRITE registers (for serial commands to FED FE or BE) - see the HAL 
     address table for the location of these registers.*/
  void blockWriteSerialCommandChunks(const u8 * command, const u32 length, const u32 individualCommandLength) throw (Fed9UVmeBaseException);

  void blockWriteSerialCommand(const u8 * command, const u32 length) throw (Fed9UVmeBaseException);

  /**This method is used to read data from the FED FE or BE. The command is accepted as a u32
     integer and is used in the majoritory of cases where the command is less than 32 bits long. The data is
     returned in a vector. The read command is passed to this method as well as a parameter specified in a 
     Fed9UVmeBase method telling it how many bits to read back. The boolean parameter tells the method to
     right-shift the last element of the arguments read back, so that the LSB is on the right.
     This method reads data from the FE or BE FPGA chips.*/
  std::vector<u32> readSerialCommand(u32 commandLong, 
					       u32 length,
					       bool rightShift) throw (Fed9UVmeBaseException);
  
   /**This method is to read data from the CMS delay chips (CDCs). The command is accepted as a vector of
     u32 integers, which should have only two elements. The CDC command section should be in
     the first element and the FPGA command section should be in the second element.
     The data is returned in a vector. The read command is passed to this method as well
     as a parameter specified in a Fed9UVmeBase method telling it how many bits to read back. The boolean
     parameter tells the method to right-shift the last element of the arguments read back, so that the
     LSB is on the right.*/
  std::vector<u32> readFromDelayChipCommand(std::vector<u32> command, 
						      u32 length,
						      bool rightShift) throw (Fed9UVmeBaseException);

  /**Method which reads from the fedRegister register. The read value is returned in readArguments.
     An offset can be specified.*/
  void readRegister(const std::string &fedRegister, u32 &readArguments, u32 offset = 0) throw (Fed9UVmeBaseException);

  /**Method which does a masked read from the fedRegister register. The read value is returned in readArguments.*/
  void maskedReadRegister(const std::string &fedRegister, u32 &readArguments) throw (Fed9UVmeBaseException);

  /**Method which does a block read from the fedRegister register and modifies the data pointed at by readArguments. 
     This is used to read data from an offset. The length will specify the size of the block.*/
  void blockReadRegister(const std::string &fedRegister, 
                         char* const readArguments, 
                         u32 offset,
                         u32 length) throw (Fed9UVmeBaseException);
  void resetBus() throw (Fed9UVmeBaseException);

private:
  static std::vector<VMEAddressTableASCIIReader *> addressTableReader;
  static std::vector<VMEAddressTable *>  addressTable;
  static std::vector<VMEBusAdapterInterface *> busAdapter;
  static std::vector<u32> countDevices;
  static pthread_mutex_t busAdaptorMutex;

  VMEDevice* fedv1Device; 
  u32 adaptorNumber;
  bool mLockedMutex;
  Fed9UHalBusAdaptor busAdaptorType_;
};

}

#endif



