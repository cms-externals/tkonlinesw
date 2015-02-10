//Author: Matthew Pearson, RAL
//Date: May 2003

//Class for FED configuration.
//It contains a separate method for each configuration command.
//Inside each command method there are the command specific id's and
//lengths (which defines the length of the argument list)
#include <inttypes.h>
#include <stdint.h>

#include "Fed9UVmeBase.hh"
#include "Fed9UWait.hh"
#include "Fed9UAddress.hh"

//#include <cstdio>
#include <iomanip>
#include <iostream>
//#include <fstream>
//#include <sstream>
#include <sys/time.h>
#include <ctime>

//number of microseconds to wait after sending a reset, or changing clock
#define FED9U_RESET_DELAY 2500000UL

namespace Fed9U {
  using namespace HAL;
  using namespace std;
  //using std::ostringstream;
  using std::vector;
  using std::string;


  //Fed9UVmeBase constructor. It takes as input the FED base address and the file name of the HAL address table.
  //One can also optionally pass a crate number (which in effect selects the PCI-VME interface card in the PC). 
  Fed9UVmeBase::Fed9UVmeBase(u32 baseAddress, string theAddressTable,Fed9UHalBusAdaptor busAdaptorType,  unsigned short crateNumber) try
  : theFed9UHalInterface(baseAddress, theAddressTable, busAdaptorType, crateNumber), theStringConstructor(), commandBuffer(NULL) {
    if (!commandBuffer) {
      commandBuffer=new u8[10000];
    }
  }//end constructor block.
  //try
  catch (Fed9UVmeBaseException& exc) {
    std::ostringstream errMsg;
    errMsg << "Caught a Fed9UVmeBaseException during construction of Fed9UVmeBase object." << std::endl;
    RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, errMsg.str() ) );
  }
  catch (std::exception& exc) {
    std::ostringstream errMsg;
    errMsg << "Caught a std::exception during construction of Fed9UVmeBase object." << std::endl;
    RETHROW(exc, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, errMsg.str() ) );
  }
  catch (...) {
    std::ostringstream errMsg;
    errMsg << "Caught an unknown exception during construction of Fed9UVmeBase object." << std::endl;
    THROW( Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, errMsg.str() ) );
  }

  
  Fed9UVmeBase::~Fed9UVmeBase() {
    if (commandBuffer)
      delete[] commandBuffer;
  }

  //**************************************************************************************************
  //**************************************************************************************************
  //Command Methods
  //**************************************************************************************************
  //**************************************************************************************************

  //Set Opto reciever control bits. This method takes as input the fpga number, 
  //a flag to read or write (true=read, false=write), a 1-element 8-bit argument vector,
  //and a empty readArguments vector which is filled up in the case of a read. 
  //In the case of a read the argument vector will be a dummy empty vector.
  void Fed9UVmeBase::feCommandOptoControl(unsigned short selectedFpga, 
                                          bool read, 
                                          vector<u32> &arguments,
                                          vector<u32> &readArguments) throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 22; //5-bit designator (the command identification)   
      u32 length = 8;            //16-bit length (specifying length of argument)
      if (read) ++length;                  //(need to add 1 in the case of a READ)
      const unsigned short noOfArgElements = 1; //Number of expected argument elements.
      const u32 maxArg = 0xff; //Maximum value of each argument element.

      //Pass the command fragments to the Fed9UConstructCommand object. 
      //Also pass the vector which contains the arguments.
      //This method will modify the command vector so that it contains the command+arguments as a vector of 
      //u32s with the MSB on the left. If passing a 'false' instead of 'true' then
      //no bit-shifting will take place.
      vector<u32> command;
      //Check that the fpga number is consistent with a front end command.
      checkFpgaNumber(selectedFpga);
      //First check we have a valid argument vector. Check the number of element and the value
      //of each element.
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);

      //Either write or read using HAL. The read will return a vector of u32s.
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true); //The true means that we want the result to come back right shifted.
      }

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandOptoControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandOptoControl."));
    }

  }

  //**************************************************************************************************

  //Set FED operational mode (Processed raw data, zero suppression, virgin raw data, frame finding, scope mode.
  //For a write, the 1-element argument vector contains 5 relavent bits. Only one bit
  //of [4:2] should be set and only one bit of [1:0] should be set. In scope mode
  //only virgin raw data mode is supported.
  //Bit 4: Processed Raw Data Mode
  //Bit 3: Zero Suppression Mode
  //Bit 2: Virgin Raw Data Mode
  //Bit 1: Frame Finding Mode
  //Bit 0: Scope Mode
  void Fed9UVmeBase::feCommandLoadModeReg(unsigned short selectedFpga, 
                                          bool read, 
                                          vector<u32> &arguments,
                                          vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 16;
      u32 length = 5;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0x1f;
 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadModeReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadModeReg."));
    }

  }

void Fed9UVmeBase::feCommandLoadSuperModeReg(unsigned short selectedFpga, 
					     bool read, 
					     vector<u32> &arguments,
					     vector<u32> &readArguments) throw (Fed9UVmeBaseException) {
  try {
    //DEFINE COMMAND SPECIFIC NUMBERS
    const u32 designator = 30;
    u32 length = 8;
    if (read) ++length;
    const unsigned short noOfArgElements = 1; 
    const u32 maxArg = 0x3f;
    
    vector<u32> command;
    checkFpgaNumber(selectedFpga);
    checkArguments(arguments, noOfArgElements, maxArg, read);
    theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
    if (!read) {
      theFed9UHalInterface.writeSerialCommand(command);
    } 
    else {
      readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
    }
  }
  catch (std::exception &e) {
    RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadModeReg."));
  }
  catch (...) {
    THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadModeReg."));
  }
}


  //************************************************************************************************************

  //Method to load the number of words of Virgin Raw Data written into the output FIFOs after a scope
  //mode event. The 1-element argument vector will be in the range 0 to 0x3ff (ie. 10 bits). 
  void Fed9UVmeBase::feCommandLoadScopeLength(unsigned short selectedFpga, 
                                              bool read, 
                                              vector<u32> &arguments,
                                              vector<u32> &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 17;
      u32 length = 10;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0x3ff;
  
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadScopeLength."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadScopeLength."));
    }

  }

  //************************************************************************************************

  //Method to write or read the adc control bits. The most common operation will be
  //a read. There will be 24-bits in the argument vector (see FE FPGA documentation).
  //Each 4-bit section will set the control bits for an ADC pair (ie. for two channels).
  //Bits [0:3] are for channels 2 and 3,
  //Bits [4:7] are for channels 1 and 4,
  //Bits [8:11] are for channels 6 and 7,
  //Bits [12:15] are for channels 5 and 8,
  //Bits [16:19] are for channels 10 and 11,
  //Bits [20:23] are for channels 12 and 9.
  void Fed9UVmeBase::feCommandAdcControl(unsigned short selectedFpga, 
                                         bool read, 
                                         vector<u32> &arguments,
                                         vector<u32> &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 23;
      u32 length = 24;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0xffffff;
 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandAdcControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandAdcControl."));
    }

  }

  //**************************************************************************************************
  //Method to enable/disable individual APVs and fibres. For an fpga there are
  //12 APV pairs. The 1-element argument vector contains 24 relavent bits,
  //where each bit pair will enable both APVs (11), enable left one (10),
  //enable right one (01) or disable both (00). 
  void Fed9UVmeBase::feCommandLoadTick(unsigned short selectedFpga, 
                                       bool read, 
                                       vector<u32> &arguments,
                                       vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 2;
      u32 length = 24;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0xffffff;
 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadTick."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadTick."));
    }
    

  }

  //*************************************************************************************************
  //Method to select the fake event address. This determins which location and which
  //RAM is written to when the loadFakeEventData command is called. The 1-element argument
  //vector has 16 relavent bits. Bits [0:8] select the Fake Event RAM address.
  //Bit [9] selects one of the channel pairs
  //Bit [10-12] are reserved and should be set to zero 
  //Bit [13-15] As in Fake Event Ram Number
  // <000> selects the RAMs connected to fibre 0 and 1
  // <101> selects the RAMs connected to fibre 10 and 11 
  void Fed9UVmeBase::feCommandSetFakeEventAddress(unsigned short selectedFpga, 
                                            bool read, 
                                            vector<u32> &arguments,
                                            vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  { 
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 29;
      u32 length = 16;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0xffff;
 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandSetPedAddress."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandSetPedAddress."));
    }
    

  }

  //*************************************************************************************************
  //Send or read data from the Fake Event RAM (defined by setFakeEventAddress). Each RAM contains the Fake Event
  //for a pair of fibers.
  void Fed9UVmeBase::feCommandLoadFakeEventData(unsigned short selectedFpga, 
                                          bool read, 
                                          vector<u32> &arguments, 
                                          vector<u32> &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 28;
      u32 length = 10;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0x3ff;
      
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      //cout << hex << arguments[0] << endl;
      checkArguments(arguments, noOfArgElements, maxArg, read);
      
      //Construct command and read/write.
      ///cout << " Fed9UVmeBase::feCommandLoadFakeEventData: command is: " <<  endl;
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      /*for (int i=0;i<command.size();i++) {
	cout << "value " << i << " = " << hex << command[i] << dec << endl;
	}*/
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true); // make the right shift on the last element
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    
  }

  //*************************************************************************************************
  //Send or read data from the Fake Event RAM (defined by setFakeEventAddress) using block mode transfer for speed enhancement. Each RAM contains the Fake Event
  //for a pair of fibers.
  void Fed9UVmeBase::feCommandBlockLoadFakeEventData(unsigned short selectedFpga, 
						     bool read, 
						     vector<u32> arguments, 
						     vector<u32> readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 28;
      u32 length = 10;
      u32 individualCommandLength = 2;
      u32 numberOfU32Words = arguments.size() * 2 ; // this is the total number of u32 words including the serial command and payload for each of the set of commands
      if (read) ++length;
      checkFpgaNumber(selectedFpga);
      //Take these out for performance issues... 
        //const unsigned short noOfArgElements = arguments.size(); 
        //const u32 maxArg = 0x3ff;
        //checkArguments(arguments, noOfArgElements, maxArg, read);

      //Construct command and read/write.
      ICUTILS_VERIFYX(numberOfU32Words <= 5000,Fed9UVmeBaseException)(numberOfU32Words).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("buffer not large enough for the command argument vector");
 
      if (!read) {
	theStringConstructor.constructBlockWriteCommand(selectedFpga, designator, length, arguments, true, commandBuffer);
	// now we either write the block to the FED using block mode transfer, the following method will divide the block up into managable chunks
	theFed9UHalInterface.blockWriteSerialCommandChunks(commandBuffer,numberOfU32Words,individualCommandLength);

      } 
      else {
	
	// read bit will go here thanks to Jo!
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    
  }


  //*************************************************************************************************
  //Method to select the pedestal address. This determines which location and which
  //RAM is written to when the loadPedestalData command is called. The 1-element argument
  //vector has 12 relavent bits. Bits [11:9] select the pedestal RAM (000 selects RAM for
  //fibres 0 and 1, 101 selects RAM for fibres 11 and 12, 111 selects all six RAMs).
  //Bit [8] selects either the pedestal part of the RAM (0) or the cluster threshold
  //part of the RAM (1). Bits [7:0] select the strip number. To set the pedestals/clusters for
  //more than one strip, it is only nessesary to use this command once, and the address will 
  //automatically increment (only the 8 LSB's will increment).
  void Fed9UVmeBase::feCommandSetPedAddress(unsigned short selectedFpga, 
                                            bool read, 
                                            vector<u32> &arguments,
                                            vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  { 
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 14;
      u32 length = 12;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0xfff;
 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandSetPedAddress."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandSetPedAddress."));
    }
    

  }

  //*************************************************************************************************
  //Send or read data from the pedestal RAM (defined by set_ped_address). Each RAM contains the pedestals
  //and cluster thresholds for the four APVs connected to a pair of fibers. The data format changes
  //depending on pedestal address.
  //In the following, the strip range is 0-255, ie. two APVs (which is one fiber).
  //The command has the feature that one has to write to the same strip number for both fibers at 
  //the same time (eg. strip 2 in APV1 in fiber 0, and strip 2 in APV1 in fiber 1).
  //If setting the pedestal, the argument list is: Exclude strip from fiber 0 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 0
  //                                               Exclude strip from fiber 1 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 1
  //If setting cluster thresholds, the argument list is: 1st cluster threshold for fiber 0
  //                                                     2nd cluster threshold for fiber 0
  //                                                     1st cluster threshold for fiber 1
  //                                                     2nd cluster threshold for fiber 1
  //setCluster=false means set pedestal
  //setCluster=true  means set cluster thresholds
  void Fed9UVmeBase::feCommandLoadPedData(unsigned short selectedFpga, 
                                          bool read, 
                                          const vector<u32> &arguments, 
                                          vector<u32> &readArguments,
                                          bool setCluster)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 13;
      u32 length = 36;
      if (read) ++length;

      vector<u32> command, readArgumentsTemp;
      checkFpgaNumber(selectedFpga);
      //Form the pedData argument list if writing.
      vector<u32> newArguments;
 
      //cout << std::hex << "args1: " << arguments[0] << "\nargs2: " << arguments[1] 
      //	   << "\nargs3: " << arguments[2] << "\nargs4: " << arguments[3] << std::dec <<endl;
      
      if (!read) {
	packPedData(arguments, newArguments, setCluster);
      }
      //      cout << std::hex << newArguments[0] << '\n' << newArguments[1] << std::dec <<endl;
      //Construct command and read/write.
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, newArguments, false, command);
      //cout << std::hex << command[0] << '\n' << command[1] << '\n' << command[2]<< std::dec <<endl;
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArgumentsTemp = theFed9UHalInterface.readSerialCommand(command[0], --length, false); //(do not right bit-shift last element of readArguments)
	//Un-pack the read back arguments so that they are returned in the same format as they were passed to this method.
	unpackPedData(readArgumentsTemp, readArguments, setCluster);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    
  }


//*************************************************************************************************
  //Send or read data from the pedestal RAM (defined by set_ped_address). Each RAM contains the pedestals
  //and cluster thresholds for the four APVs connected to a pair of fibers. The data format changes
  //depending on pedestal address.
  //In the following, the strip range is 0-255, ie. two APVs (which is one fiber).
  //The command has the feature that one has to write to the same strip number for both fibers at 
  //the same time (eg. strip 2 in APV1 in fiber 0, and strip 2 in APV1 in fiber 1).
  //If setting the pedestal, the argument list is: Exclude strip from fiber 0 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 0
  //                                               Exclude strip from fiber 1 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 1
  //If setting cluster thresholds, the argument list is: 1st cluster threshold for fiber 0
  //                                                     2nd cluster threshold for fiber 0
  //                                                     1st cluster threshold for fiber 1
  //                                                     2nd cluster threshold for fiber 1
  //setCluster=false means set pedestal
  //setCluster=true  means set cluster thresholds
  void Fed9UVmeBase::feCommandBlockLoadPedData(unsigned short selectedFpga, 
                                          bool read, 
                                          vector<u32> &arguments, 
                                          vector<u32> &readArguments,
                                          bool setCluster)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 13;
      u32 length = 36;
      u32 individualCommandLength = 3;
      if (read) ++length;

      vector<u32> command, readArgumentsTemp;
      checkFpgaNumber(selectedFpga);
      //Form the pedData argument list if writing.
      vector<u32> newArguments;


      



      u32 argumentsLength = arguments.size();
      //cout << "arguementsLenghth = " << argumentsLength << endl;
      if (!read) {
	for (u32 i=0 ; i < argumentsLength ; i += 4) {
	  
	  //cout << std::hex << "args1: " << arguments[0] << "\nargs2: " << arguments[1] 
	  //    << "\nargs3: " << arguments[2] << "\nargs4: " << arguments[3] << std::dec <<endl;

	  packPedData(arguments, newArguments, setCluster); //<JRF> this method existed to do the packing so using it anyway.. each call here pushes back 2 new values
	  
	  // now push the first 4 elemets to the end of the vector then remove them and pack the next set of 4 arguments into the next two newArguments. 
	  //arguments.push_back(arguments[0]);
	  //arguments.push_back(arguments[1]);
	  //arguments.push_back(arguments[2]);
	  //arguments.push_back(arguments[3]);
	  //cout << std::hex << "newargs1: " << newArguments[i/2+0] << "\nnewargs2: " << newArguments[i/2+1] << std::dec <<endl;
	  arguments.erase( arguments.begin() , arguments.begin()+4 ); // erase the first 4 elements
	  
	}
	//cout << "arguementsLenghth = " << arguments.size() << "newarguments length = " << newArguments.size() << endl;

	u32 numberOfU32Words = (newArguments.size()/2) * individualCommandLength ; // this is the total number of u32 words including the serial command and payload for each command
	//Construct command and read/write.
	ICUTILS_VERIFYX(numberOfU32Words <= 5000,Fed9UVmeBaseException)(numberOfU32Words).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("buffer not large enough for the command argument vector");
	theStringConstructor.constructBlockWriteCommand(selectedFpga, designator, length, newArguments, false, commandBuffer);
	// now we write the block to the FED using block mode transfer, the following method will divide the block up into managable chunks
	theFed9UHalInterface.blockWriteSerialCommandChunks(commandBuffer,numberOfU32Words,individualCommandLength);
      } 
      else {
	readArgumentsTemp = theFed9UHalInterface.readSerialCommand(command[0], --length, false); //(do not right bit-shift last element of readArguments)
	//Un-pack the read back arguments so that they are returned in the same format as they were passed to this method.
	unpackPedData(readArgumentsTemp, readArguments, setCluster);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadPedData."));
    }
    
  }




  //*************************************************************************************************
  //Informs the median calculation which strips are valid for each APV.
  //The number specified by this command should be equal to the number of valid
  //strips specified by using the feCommandLoadPedData method.
  //Data should be input in the following format:
  //1st element of arguments will contain ffff, where the left most ff is for
  //the APV1 for fiber 1, and the right most ff is for APV2 for fiber 1.
  //The 2nd element of arguments is the same for fiber 2, etc.
  //Therefore, there are 12 elements in the input vector.
  void Fed9UVmeBase::feCommandSetNumberValid(unsigned short selectedFpga, 
                                             bool read, 
				             const vector<u32> &arguments,
                                             vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 15;
      u32 length = 192;
      if (read) ++length;
      const unsigned short noOfArgElements = 12; 
      const u32 maxArg = 0xffff;
  
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      //Take the 12 16-bit elements of arguments and put them into a 192-bit 
      //space in newArguments (ie. using 6 elements)
      vector<u32> newArguments;
      if (!read) {
	newArguments = theStringConstructor.mergeVectorElements(arguments, length);
      }
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, newArguments, false, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, false);
	readArguments = theStringConstructor.unpackVectorElements(readArguments, 16, maxArg, length);  //Unpack the read-back values if we are reading
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandSetNumberValid."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandSetNumberValid."));
    }
    
  }

  //*************************************************************************************************
  //Method to load the threshold values for the ticker blocks (ie. threshold for frame finding).
  //When writing the method expects an 12-element arguments vector, each element containing 5 relavent bits.
  //The first element will contain the threshold for fibre 1, the 12th element will contain
  //the threshold for fiber 12.
  void Fed9UVmeBase::feCommandLoadThresh(unsigned short selectedFpga, 
                                         bool read, 
                                         const vector<u32> &arguments,
                                         vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 4;
      u32 length = 60;
      if (read) ++length;
      const unsigned short noOfArgElements = 12; 
      const u32 maxArg = 0x1f;
  
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      //Take the 12 5-bit elements of arguments and put them into a 60-bit 
      //space in newArguments (ie. using two elements)
      vector<u32> newArguments;
      if (!read) {
	newArguments = theStringConstructor.mergeVectorElements(arguments, length);
      }
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, newArguments, false, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, false);
	readArguments = theStringConstructor.unpackVectorElements(readArguments, 5, maxArg, length);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadThresh."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadThresh."));
    }
    
  }

  //**************************************************************************************************
  //Method to load the median overide values for the common mode for each APV. When writing the method expects
  //an argument vector with 24 elements which each contain 10 relavent bits. The first elements will
  //contain the median for APV1 for fibre1, the 2nd element will contain the median for APV2 for fibre1, etc.
  void Fed9UVmeBase::feCommandLoadMedian(unsigned short selectedFpga, 
                                         bool read, 
                                         const vector<u32> &arguments,
                                         vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 10;
      u32 length = 240;
      if (read) ++length;
      const unsigned short noOfArgElements = 24; 
      const u32 maxArg = 0x3ff;

      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      //Take the 24 10-bit elements of arguments and put them into a 240-bit 
      //space in newArguments (ie. using 8 32-bit elements).  
      vector<u32> newArguments;
      if (!read) {
	newArguments = theStringConstructor.mergeVectorElements(arguments, length);
      }
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, newArguments, false, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, false);
	readArguments = theStringConstructor.unpackVectorElements(readArguments, 10, maxArg, length);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadMedian."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadMedian."));
    }
  }


  //**************************************************************************************************
  //Manually overide the automatically calculated common mode. The argument vector will be 1-element
  //which contains a 1 to enable or a 0 to disable the overide.
  void Fed9UVmeBase::feCommandMedianEnable(unsigned short selectedFpga, 
                                           bool read,
                                           vector<u32> &arguments,
                                           vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 11;
      u32 length = 1;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0x1;
  
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandMedianEnable."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandMedianEnable."));
    }
    
  }
  
  //**************************************************************************************************
  //<JEC date=16/03/07> new register for scaling down triggers to the FED
  void Fed9UVmeBase::feCommandFrameCapturePrescale(unsigned short selectedFpga, 
						   bool read, 
						   std::vector<u32> &arguments,
						   std::vector<u32> &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 19;
      u32 length = 16;
      if (read) ++length;
      const unsigned short noOfArgElements = 1; 
      const u32 maxArg = 0xFFFF;
      
      std::vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(arguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandFrameCapturePrescale."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandFrameCapturePrescale."));
    } 
  }
  //</JEC>

  //**************************************************************************************************
  //Perform a soft reset on the specified FPGA.
  void Fed9UVmeBase::feCommandSoftReset(unsigned short selectedFpga)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 12;
      u32 length = 1;
      const bool read = false;
      vector<u32> arguments(1, 1);
  
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      //Write the reset command to the FED.
      theFed9UHalInterface.writeSerialCommand(command);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandSoftReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandSoftReset."));
    }
    
  }

  //**************************************************************************************************
  //Method to set the delay values for the 4 fibers connected to one
  //of the CMS delay chips. This method takes as input a 2-bit chip number to address
  //a particular delay FPGA. The argument vector contains the 4 9-bit delay values
  //in the first 4 vector elements, and the chipNumber is contained in the 5th element.
  //chipNumber = 0 for chip 1 (fibers 1 to 4)
  //chipNumber = 1 for chip 2 (fibers 5 to 8)
  //chipNumber = 2 for chip 3 (fibers 9 to 12)
  //chipNumber = 3 for all chips (fibers 1 to 12)
  //Earlier versions of the delay chip firmware used a 5 bit fine skew and as such a
  //a switch has been applied to allow for this backward compatibility. Passing true
  //for fineSkew6bit uses a 6 bit fine skew value and false a 5 bit. No check if made
  //to ensure the appropiate firmware version is done at this level.
  //NOTE: This is a write only command.
  void Fed9UVmeBase::feCommandLoadSkew(unsigned short selectedFpga,  
                                       vector<u32> &arguments, bool fineSkew6bit)  throw (Fed9UVmeBaseException) 
  {
    try {
      checkFpgaNumber(selectedFpga);
      ICUTILS_VERIFYX(arguments[4]<4,Fed9UVmeBaseException)(arguments[4]).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("Invalid delay chip request in Fed9UVmeBase::feCommandLoadSkew. Valid values are 0,1,2 and 3");
      //DEFINE COMMAND SPECIFIC NUMBERS
      //These variables do not change depending on fine skew size.
      const u32 delayChipDesignator = 1;
      const bool delayChipRead = false;
      const unsigned short noOfArgElements = 4; 
      //These variable need to be updated if we are using the smaller fine skew value.
      //They are initialised to the 6 bit values...
      u32 delayChipLength = 40;
      u32 maxArg = 0x3ff;
      //then over ridden if we need the 5 bit settings.
      if (!fineSkew6bit) {
	//Some of the above values are specific to a 64bit fine skews
	//and need to be changed for a 32 bit fine skew.
	delayChipLength = 36;
	maxArg = 0x1ff;
      }

      //Extract the 2-bit chip number from the argument vector and put into new vector.
      u32 selectedDelayChip(arguments[4]);
      //load skew expects there to be only 4 elements.
      arguments.pop_back();

      //Form the CDC part of the command with the chip number in the argument list
      const u32 fpgaDesignator = 21;
      const u32 fpgaLength = 27+delayChipLength;
      const bool fpgaRead = false;
      //Don't create any arguments in the FPGA command leave that to the construction of the delay chip command.
      //It must have a size though.
      vector<u32> dummyVec(1,0);
      checkArguments(arguments, noOfArgElements, maxArg, fpgaRead);

      vector<u32> command;
      theStringConstructor.constructCommand(selectedFpga, fpgaRead, fpgaDesignator, fpgaLength, dummyVec, false, command); 
      //Merge the argument vector
      vector<u32> newArguments = theStringConstructor.mergeVectorElements(arguments, delayChipLength);
      //Add the rest of the command to the FE FPGA part, and then add the arguments.
      theStringConstructor.constructDelayChipCommand(command, selectedDelayChip, delayChipRead, delayChipDesignator, delayChipLength, newArguments);
      theFed9UHalInterface.writeSerialCommand(command);

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadSkew."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadSkew."));
    }
  }

  /**Method to set the period for 1/N arming of the spy channel. A command that instructs one or all of the delay
     chip to collect the next 1022 words of data on the requested fibre is sent every N event
     NOTE: This is a write only command.*/
  void Fed9UVmeBase::feCommandSpyPeriod(unsigned short selectedFpga,
                                             bool read,
                                             vector<u32> &arguments,
                                             vector<u32> &readArguments) throw (Fed9UVmeBaseException)
  {
    try  {
         //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 7;
      u32 length = 32;
      if (read) {
       ++length;
      }

      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, arguments, true, command);
      if (!read) {
       theFed9UHalInterface.writeSerialCommand(command);
      }
      else {
       readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandSpyPeriod."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandSpyPeriod."));
    }
  }


  //**************************************************************************************************
  //Method to arm the spy channel. A command that instructs one or all of the delay
  //chip to collect the next 1022 words of data on the requested fibre. It requires a FE FPGA and
  //chip number to specify which chip to record on.
  //chipNumber = 0 for chip 1 (fibers 1 to 4)
  //chipNumber = 1 for chip 2 (fibers 5 to 8)
  //chipNumber = 2 for chip 3 (fibers 9 to 12)
  //chipNumber = 3 for all chips (fibers 1 to 12)
  //NOTE: This is a write only command.
  //\todo - This command is untested. GJR 13/07/06.
  void Fed9UVmeBase::feCommandArmSpyChannel(unsigned short selectedFpga,
						     u32 selectedDelayChip) throw (Fed9UVmeBaseException)
  {
    try {
      checkFpgaNumber(selectedFpga);
      ICUTILS_VERIFYX(selectedDelayChip<4,Fed9UVmeBaseException)(selectedDelayChip).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("Invalid delay chip request in Fed9UVmeBase::feCommandArmSpyChannel. Valid values are 0,1,2 and 3");

      //Define variables specific to the delay chip command.
      const u32 delayChipDesignator = 7;
      const u32 delayChipLength = 1;  
      const bool delayChipRead = false;
      //Form the FE FPGA part of the command without any arguments. Formed later by delay chip command.
      const u32 fpgaDesignator = 21;
      const u32 fpgaLength = 27+delayChipLength;
      const bool fpgaRead = false;
      //Holds command to be sent to the FED.
      vector<u32> command;
      //Don't create any arguments in the FPGA command leave that to the construction of the delay chip command.
      //It must have a size though.
      vector<u32> dummyVec(1,0);
      theStringConstructor.constructCommand(selectedFpga, fpgaRead, fpgaDesignator, fpgaLength, dummyVec, false, command);
      //cout << " Spy Arm command 0 " << hex << command[0] << endl;
      //Add the rest of the command to the delay chip part. In this case the arguments are not required.
      theStringConstructor.constructDelayChipCommand(command, selectedDelayChip, delayChipRead, delayChipDesignator, delayChipLength, dummyVec);
      //Submit command to the FED.
      //cout << " Spy Arm command 1 " << hex << command[1] << " " << command[2]  << endl;
      theFed9UHalInterface.writeSerialCommand(command);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandArmSpyChannel."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandArmSpyChannel."));
    }
  }

  //**************************************************************************************************
  //Method to read the 1022 words of data read back from the fibres on the chosen delay chip.
  //selectedDelayChip = 0 for chip 1 (fibers 1 to 4)
  //selectedDelayChip = 1 for chip 2 (fibers 5 to 8)
  //selectedDelayChip = 2 for chip 3 (fibers 9 to 12)
  //NOTE: This a read only command and it is not possible to read back all chips simulataneously.
  //\todo - this command has not yet been tested. GJR 13/07/06.
  vector<u32> Fed9UVmeBase::feCommandFireSpyChannel(unsigned short selectedFpga,
						      u32 selectedDelayChip) throw (Fed9UVmeBaseException)
  {
    try {
      //Check FPGA and delay chip numbers are valid.
      checkFpgaNumber(selectedFpga);
      ICUTILS_VERIFYX(selectedDelayChip<3,Fed9UVmeBaseException)(selectedDelayChip).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("Invalid delay chip request in Fed9UVmeBase::feCommandFireSpyChannel. Valid values are 0,1 and 2");

      //These parameters are for the command as seen by the delay chips.
      const u32 delayChipDesignator = 8;
      // <NAC date=21/03/07>
      //const u32 delayChipLength = 40923;
      const u32 delayChipLength = 12000;
      // </NAC>
      const bool delayChipRead = true;
      //These parameters form the part of the command that tells the FE FPGA to dail through to the CDC.
      const u32 fpgaDesignator = 21;
      //27 is the length of FPGA specfic command + the length of delay chip command.
      const u32 fpgaLength = 27 + delayChipLength;
      //The FPGA requires a write command as it is writing to the delay chip.
      const bool fpgaRead = false;
      //Create an empty vector to hold the command.
      vector<u32> command;

      //Construct the part of the command such that the FE FPGA understands.
      //Don't add anything to the FPGA command arguments, these come from the delay chip command section. It must have a size!
      vector<u32> dummyVec(1,0);
      theStringConstructor.constructCommand(selectedFpga, fpgaRead, fpgaDesignator, fpgaLength, dummyVec, false, command);

      //Construct the delay chip command.
      theStringConstructor.constructDelayChipCommand(command, selectedDelayChip, delayChipRead, delayChipDesignator, delayChipLength, dummyVec);

      //Read back the spy channel data from the relevant FE FPGA.
      vector<u32> spyChannelData( theFed9UHalInterface.readFromDelayChipCommand(command, fpgaLength, true) );
      return spyChannelData;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandCdcFirmwareId."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadSkew."));
    }
  }


  //**************************************************************************************************
  //Method to read the firmware version that is currently in use by the CMS delay chips (CDCs).
  //It allows access to the four chips on each FE FPGA. The cdcNumber corresponds to the following CDC
  //and group of channels:
  //selectedDelayChip = 0 for chip 1 (fibers 1 to 4)
  //selectedDelayChip = 1 for chip 2 (fibers 5 to 8)
  //selectedDelayChip = 2 for chip 3 (fibers 9 to 12)
  //NOTE: This is a read only command and it is not possible to  read back all chips simulataneously.
  u32 Fed9UVmeBase::feCommandDelayChipFirmwareId(unsigned short selectedFpga,  
							   u32 selectedDelayChip)  throw (Fed9UVmeBaseException) 
  {
    try {
      //Check FPGA and delay chip numbers are valid.
      checkFpgaNumber(selectedFpga);
      ICUTILS_VERIFYX(selectedDelayChip<3,Fed9UVmeBaseException)(selectedDelayChip).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("Invalid delay chip request in Fed9UVmeBase::feCommandDelayFirmwareId. Valid values are 0,1 and 2");

      //These parameters are for the command as seen by the delay chips.
      const u32 delayChipDesignator = 6;
      const u32 delayChipLength = 32+3;//+3 to handle extra bits from serial stream.
      //The CDC is given a read only command.
      const bool delayChipRead = true;
      //These parameters form the part of the command that tells the FE FPGA to dail through to the CDC.
      const u32 fpgaDesignator = 21;
      //27 is the length of FPGA specfic command + the legnth of CDC command.
      const u32 fpgaLength = 27 + delayChipLength;
      //The FPGA requires a write command as it is writing to the CDC.
      const bool fpgaRead = false;
      //Create an empty vector to hold the command.
      vector<u32> command;

      //Construct the part of the command such that the FE FPGA understands.
      //Don't add anything to the FPGA command arguments. These are the CDC command section. It must have a size!
      vector<u32> dummyVec(1,0);
      theStringConstructor.constructCommand(selectedFpga, fpgaRead, fpgaDesignator, fpgaLength, dummyVec, false, command);

      //Construct the delay chip command.
      theStringConstructor.constructDelayChipCommand(command, selectedDelayChip, delayChipRead, delayChipDesignator, delayChipLength, dummyVec);

      //Read the firmware id from the relevant FE FPGA using the specific read from delay chip command.
      vector<u32> firmwareId( theFed9UHalInterface.readFromDelayChipCommand(command, fpgaLength, false) );
      unsigned short shift = 1;
      firmwareId[0] <<= shift;
      firmwareId[0] |= firmwareId[1]>>(32-shift);
      return firmwareId[0];
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandCdcFirmwareId."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadSkew."));
    }
  }


  //This command controls the LM82 temperature sensor from the Front End chips. 
  //The FE FPGA communicates 
  //with the sensor via an I2C protocol built into the firmware.
  //The arguments vector has two elements. The first element 
  //contains 8 relavent bits, which is the data sent to the LM82 in the case 
  //of a write. The 2nd element contains the 8-bit LM82 register number.
  //The i2cBusy and i2cError booleans will return a false if
  //the reading of the LM82 succeded (in the case of a write they
  //will both always return a false).
  //Note:
  //arguments[0] = data 8-bits
  //arguments[1] = register 8-bits.
  void Fed9UVmeBase::feCommandLm82Control(unsigned short selectedFpga,
                                          bool read, 
                                          const vector<u32> &arguments,
                                          u32 &readArguments,
                                          bool &i2cBusy,
                                          bool &i2cError)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 5;
      u32 length = 17; //This is a write only command, so we don't need to add 1 to the length. 
      const unsigned short noOfArgElements = 1; //The arguments elements will be merged before passing to sendReadWriteCommand.
      const u32 maxArg = 0x1ffff;//Maximum value of the merged arguments including the read/write bit (the 17th bit).
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0xff; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::feCommandLm82Control.");
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::feCommandLm82Control.");
 
      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is the 8-bit LM82 register address. Bit 16 is the read/write bit.
      u32 mergedArguments = (read << 16) + (arguments[1] << 8) + (arguments[0]);
      vector <u32> mergedVectorArguments;
      mergedVectorArguments.push_back(mergedArguments);

      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      //First check we have a valid argument vector. Check the number of element and the value
      //of each element.
      checkArguments(mergedVectorArguments, noOfArgElements, maxArg, read);
      //Here the read boolean is false because we are writting first before reading with the different command.
      theStringConstructor.constructCommand(selectedFpga, false, designator, length, mergedVectorArguments, true, command);
      //If we are reading we have to write the read command first, and then read the FE LM82 Status command,
      //which is a private method.
      theFed9UHalInterface.writeSerialCommand(command); 
      //Set a delay to allow for the I2C to do its job.
    fed9Uwait(500);  
    if (read) {
	feCommandLm82Status(selectedFpga, readArguments, i2cBusy, i2cError);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLm82Control."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLm82Control."));
    }
  }

  //This is a function which reads the LM82 status from the FE. It is a read-only FE FPGA command.
  //This command is used inside the feCommandLm82Control command.
  //Two booleans are passed to check for a busy or an error in the I2C system.
  //This is a private method.
  void Fed9UVmeBase::feCommandLm82Status(unsigned short selectedFpga,
                                         u32 &readArguments,
                                         bool &i2cBusy, 
                                         bool &i2cError)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 27;
      const u32 length = 16; //This is a read only command, so do not add 1 here. See inside sendReadOnlyCommand.
      const u32 mask = 0xFFFF;

      checkFpgaNumber(selectedFpga);
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
      //Test for a busy or an error
      if (readArguments & 0x100) i2cBusy = true;
      if (readArguments & 0x200) i2cError = true;
      //Just send back the first 8 bits (0-7), which are the data bits. Bits 8 and 9 are
      //the error bits which are tested for above.
      readArguments = readArguments & 0xFF;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLm82Status."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLm82Status."));
    }
    
  }


  //This command controls the 12 Channel, 8-bit TrimDACs (the AD8802). The DAC is used to
  //convert digital configuration signals that can be used to control the
  //gain of the ADCs at the Front-End. The FE FPGA communicates 
  //with the DAC via a serial protocol (see AD8802 datasheet) built into
  //the firmware. The arguments vector has two elements. The first element 
  //contains 8 relavent bits, which is the data sent to the TrimDAC in the case 
  //of a write. The 2nd element contains the 4-bit TrimDAC register number.
  //By passing a 1 for the reset boolean will reset the chip,
  //passing a 1 for the shutdown boolean will shutdown the chip.
  //This is WRITE ONLY command.
  //Note:
  //arguments[0] = data 8-bits
  //arguments[1] = register 4-bits.*/
  void Fed9UVmeBase::feCommandTrimDacControl(unsigned short selectedFpga,
                                             vector<u32> &arguments,
                                             bool reset,
                                             bool shutdown) throw (Fed9UVmeBaseException)  
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 6;
      u32 length = 14; //This is a write only command, so we don't need to add 1 to the length. 
      const bool read = false; //This is a write only command. 
      const unsigned short noOfArgElements = 1; //The arguments elements will be merged before passing to sendReadWriteCommand.
      const u32 maxArg = 0x3fff;//Maximum value of the merged arguments.
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0xf; //Maximum value of the register.

      //Check input arguments. If doing a reset or shutdown, then create two 0 elements for the arguments.
      if ((reset)||(shutdown)) {
	if (arguments.size()!=0) arguments.clear();
	arguments.push_back(0);
	arguments.push_back(0);
      } else {
	ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::feCommandTrimDacControl.");
	ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::feCommandTrimDacControl.");
      }

      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits [0:7],
      //then there is the 4-bit DAC register address [8:11]. Bit 12 is the shutdown bit and bit 13
      //is the reset bit.
      u32 mergedArguments = (reset << 13) + (shutdown << 12) + (arguments[1] << 8) + (arguments[0]);
      vector <u32> mergedVectorArguments(1, mergedArguments);

      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      checkArguments(mergedVectorArguments, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, mergedVectorArguments, true, command);
      theFed9UHalInterface.writeSerialCommand(command);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandTrimDacControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandTrimDacControl."));
    }

  }

  /**This command complements the 10-bit data word which enters the 
     FE FPGA. It turns all the 1's to 0's and visa versa. It can be
     used if the inverter stage of the APVs is turned off. The command only needs 
     the FPGA number, a read/write flag and a unsinged long (where the 12 LSBs are used
     to set the complement, 0=off 1=on).*/
  void Fed9UVmeBase::feCommandLoadComplement(unsigned short selectedFpga,
                                             bool read,
                                             u32 arguments,
                                             u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 26;
      u32 length = 12;
      if (read) {
	++length;
      }

      vector<u32> command;
      vector<u32> vectorArguments(1, arguments);
      vector<u32> tempReadArguments;
      checkFpgaNumber(selectedFpga);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, vectorArguments, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	tempReadArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
	readArguments = tempReadArguments[0];
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandLoadComplement."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandLoadComplement."));
    }

  }


  //This method will cause the specifed fpga to output on command_out a serial packet showing 
  //the current occupancy of the FIFOs. The methods is read only and returns a 12 element 
  //argument vector, where each element has a maximum value of 0xfff.
  //NOTE: This is a read only command.
  // JEC 10-05-06:  Modified this method.
  // register output is padded on the left with a couple of zeros.  Remove before the vector is unpacked.
  // In call to unpacking method, length needs to be decremented again,as 12 x 13 = 156 and originally length = 156 + 2
  void Fed9UVmeBase::feCommandMonitor(unsigned short selectedFpga, vector<u32> &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 20;
      u32 length = 158;//length = 156 + 2 because a read only method. 
      const unsigned short noOfArgElements = 12;
      const u32 maxArg = 0x1fff; //This is 0x1fff to cope with the fact that the FED pads the numbers with 1 on the left. Treat the arguments as 13 bits rather than 12, then just lop off the MSB. 
      const bool read = true;

      vector<u32> dummy; 
      vector<u32> command;
      checkFpgaNumber(selectedFpga);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, dummy, false, command);
      readArguments = theFed9UHalInterface.readSerialCommand(command[0], --length, false);
      // register comes complete with padding on the left, therefore before unpacking remove them from all elements
      for (unsigned int i = 0; i < readArguments.size(); i++) {
	readArguments[i] <<= 2;
      }
      // decrement length again, as actually command length is 156
      readArguments = theStringConstructor.unpackVectorElements(readArguments, 13, maxArg, --length);
      unsigned short mask = 0xfff; 
      for (int i=0; i<=noOfArgElements; i++) {       //each element has a 1 in the MSB position
	readArguments[i] = readArguments[i] & mask;  //this is how the command is structured and needs to be removed for the user to read (ie. remove the padding 1)
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandMonitor."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandMonitor."));
    }
  }

  //This method is a read only command. It reads the firmware ID register.
  void Fed9UVmeBase::feCommandFirmwareIdReg(unsigned short selectedFpga, 
                                            u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 3;
      const u32 length = 32; //This is a read only command, so do not add 1 here. See inside sendReadOnlyCommand.
      checkFpgaNumber(selectedFpga);
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandFirmwareIdReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandFirmwareIdReg."));
    }
  }


  // JEC 31-05-06
  // This method is a read only command.  It provides the number of triggers in the Fake Event FIFO
  void Fed9UVmeBase::feCommandFakeEventTriggerFifo(unsigned short selectedFpga, 
						   u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 18;
      const u32 length = 10; //This is a read only command, so do not add 1 here. See inside sendReadOnlyCommand.
      checkFpgaNumber(selectedFpga);
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::feCommandFakeEventTriggerFifo."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::feCommandFakeEventTriggerFifo."));
    }
  }


  //******************************************************************************************************
  //******************************************************************************************************

  //BACK-END FPGA COMMANDS. These commands do not need to take in an fpga number, as this is fixed.

  //This command selects the trigger source for the back-end FPGA. The arguments input
  //will have 3 relavent bits, which mean:
  //001=TTC Trigger (this is normal running mode)
  //010=Backplane trigger
  //100=Software tigger.
  //Only one of these bits should be set at any one time.  
  void Fed9UVmeBase::beCommandTriggerSelect(bool read, 
                                            u32 arguments,
                                            u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 1;
      u32 length = 3;
      const unsigned short noOfArgElements = 1; //This is the number of 32-bit elements.
      const u32 maxArg = 0x4; //Maximum value is 4, since only 100, 010 and 001 are allowed.

      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTriggerSelect."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTriggerSelect."));
    }
  }
 
  //This command sets the operating mode of the BE FPGA. It can be either in
  //scope mode, or frame finding mode (not both at the same time). The arguments
  //input is 1 or 2, where:
  //1=Scope Mode
  //2=Virgin Raw Data Mode
  //6=Processed Raw Data Mode
  //10=Zero Suppression Mode
  void Fed9UVmeBase::beCommandModeRegister(bool read, 
                                           u32 arguments,
                                           u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 2;
      u32 length = 4;
      const unsigned short noOfArgElements = 1;
      //The maximum value of 10 comes from the setting for zero suppression mode,
      //however not all values from 0x0 to 0xd are allowed.
      const u32 maxArg = 0xd;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandModeRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandModeRegister."));
    }
  }
  
  //This is a I2C command to the TTCrx chip. The I2C protocol is inside the
  //firmware. The arguments vector has two elements. The first element 
  //contains 8 relavent bits, which is the data sent to the TTCrx in the case 
  //of a write. The 2nd element contains the 5-bit TTCrx register number.
  //The i2cBusy and i2cError booleans will return a false if
  //the reading of the TTCrx succeded (in the case of a write they
  //will both always return a false).
  //Note:
  //arguments[0] = data 8-bits
  //arguments[1] = register 5-bits
  void Fed9UVmeBase::beCommandTtcrxControl(bool read, 
                                           const vector<u32> &arguments,
                                           u32 &readArguments,
                                           bool &i2cBusy,
                                           bool &i2cError)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 3;
      u32 length = 16;
      const unsigned short noOfArgElements = 1; //The arguments elements will be merged before passing to sendBEReadWriteCommand.
      const u32 maxArg = 0x3fff;//Maximum value of the merged arguments (bit 14 is not used and bit 15 should be zero in this method, since when it is 1 the ttcrx is reset.)
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0x1f; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::beCommandTtcrxControl.");
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::beCommandTtcrxControl.");
 
      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is 1-bit read/write bit, then there is the 5-bit TTC register address. Bit 14 is
      //not used and bit 15 (the reset bit) is always zero for this command.
      u32 mergedArguments = 0;
      mergedArguments = (arguments[1] << 9) + (read << 8) + (arguments[0]);

      //The read boolean here is fixed as false, as this is the case for the write and the read,
      //because for the read we have to write the read command first (and then read the TTC Status command,
      //which is a private method).
      readArguments = sendBEReadWriteCommand(designator, length, false, mergedArguments, noOfArgElements, maxArg);
      //Set a delay to allow for the I2C to do its job.
    fed9Uwait(500);
      if (read) {
	beCommandTtcrxStatus(readArguments, i2cBusy, i2cError);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcrxControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcrxControl."));
    }
    
  }

  //This method sends a reset signal to the TTCrx. This is an internal
  //reset. An alternative reset can be performed by the beCommandTtcrxExternalReset.
  //This internal reset command sends a I2C reset command to the TTCrx.
  void Fed9UVmeBase::beCommandTtcrxInternalReset(void)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 3; //Same command as the beCommandTtcrxControl method.
      u32 length = 16;
      const unsigned short noOfArgElements = 1;

      u32 arguments = 1 << 15; //Set the 16th bit of the arguments integer to 1. This will do the reset.
      const u32 maxArg = arguments;

      u32 dummy;
      dummy = sendBEReadWriteCommand(designator, length, false, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcrxInternalReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcrxInternalReset."));
    }
  }

  //This method sends a reset signal to the TTCrx. This is an external
  //reset. An alternative reset can be performed by the beCommandTtcrxInternalReset.
  //This external reset command drives the reset pin of the TTCrx chip low.
  void Fed9UVmeBase::beCommandTtcrxExternalReset(void)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 11; //Same command as the beCommandTtcrxControl method.
      u32 length = 1;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0;

      u32 arguments = 0;

      u32 dummy;
      dummy = sendBEReadWriteCommand(designator, length, false, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcrxExternalReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcrxExternalReset."));
    }
  }

  
  //This command writes or reads to the test register. The input
  //argument is 32-bits long, where each bit represents the status of
  //a particular register. This command is only used for testing.
  void Fed9UVmeBase::beCommandTestReg(bool read, 
                                      u32 arguments,
                                      u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 4;
      u32 length = 32;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFFFFFFF; 
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTestReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTestReg."));
    }
  }
  
  //This command is used to enable or disable a front end input to the
  //back-end chip. The arguments input has 8 relavent bits, where 
  //each bit can be set to 0 or 1 to disable or enable an FPGA (0=disable, 1=enable).
  //The least significant bit is for FPGA 1, the most significant bit
  //is for FPGA 8. 
  void Fed9UVmeBase::beCommandFeEnableReg(bool read, 
                                          u32 arguments,
                                          u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 5;
      u32 length = 8;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFF; 
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFeEnableReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFeEnableReg."));
    }
  }  

  //This command selects the readout from the QDR memory chips. The arguments
  //input can be either 1 or 2, where:
  //1=S-LINK Readout
  //2=VME BUS Readout.
  //Only one of these can be set at the same time.
  void Fed9UVmeBase::beCommandBeReadoutControl(bool read, 
                                               u32 arguments,
                                               u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 6;
      u32 length = 2;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0x2; //Maximum value is 2, since only 10 and 01 are allowed.
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandBeReadoutControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBeReadoutControl."));
    }
  }


  //This command can start or stop a FED run. The arguments input
  //is two bits. 
  //The least significant bit controls the trigger:
  //0=ignore trigger
  //1=accept trigger
  //The most significant bit controls the acceptance of frames and headers:
  //0=ignore frames and headers
  //1=accept frames and headers
  //One should only set either 11 or 00.
  void Fed9UVmeBase::beCommandBeRunControl(bool read, 
                                           u32 arguments,
                                           u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 7;
      u32 length = 2;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0x3;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandBeRunControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBeRunControl."));
    }
  }
  
  //This command generates a software trigger to the FE FPGAs.
  void Fed9UVmeBase::beCommandSoftwareTrigger(void)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 8;
      u32 length = 1;
      const bool read = false; //There is no read for this command.
      const u32 arguments = 1;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0x1;
      u32 dummy;
      dummy = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSoftwareTrigger."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSoftwareTrigger."));
    }
  }
  
  //This command applies a soft reset to the BE FPGA. This only resets the counters. It
  //does not affect the DCMs and control registers.
  void Fed9UVmeBase::beCommandSoftReset(void)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 9;
      u32 length = 1;
      const bool read = false; //There is no read for this command.
      const u32 arguments = 1;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0x1;
      u32 dummy;
      dummy = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSoftReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSoftReset."));
    }
  }
  
  //This command can set or read the FED ID register. It is
  //a 12-bit number.
  void Fed9UVmeBase::beCommandFedIdReg(bool read, 
                                       u32 arguments,
                                       u32 &readArguments) throw (Fed9UVmeBaseException)  
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 10;
      u32 length = 12;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFF;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFedIdReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFedIdReg."));
    }
  }


  //This command controls the LM82 temperature sensor. The BE FPGA communicates 
  //with the sensor via an I2C protocol built into the firmware.
  //The arguments vector has two elements. The first element 
  //contains 8 relavent bits, which is the data sent to the LM82 in the case 
  //of a write. The 2nd element contains the 8-bit LM82 register number.
  //The i2cBusy and i2cError booleans will return a false if
  //the reading of the LM82 succeded (in the case of a write they
  //will both always return a false).
  //Note:
  //arguments[0] = data 8-bits
  //arguments[1] = register 8-bits
  void Fed9UVmeBase::beCommandLm82Control(bool read, 
                                          const vector<u32> &arguments,
                                          u32 &readArguments,
                                          bool &i2cBusy,
                                          bool &i2cError)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 12;
      u32 length = 17;
      const unsigned short noOfArgElements = 1; //The arguments elements will be merged before passing to sendBEReadWriteCommand.
      const u32 maxArg = 0x1ffff;//Maximum value of the merged arguments including the read/write bit (the 17th bit).
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0xff; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::beCommandLm82Control.");
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::beCommandLm82Control.");

      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is the 8-bit LM82 register address. Bit 16 is the read/write bit.
      u32 mergedArguments = 0;
      mergedArguments = (read << 16) + (arguments[1] << 8) + (arguments[0]);
      //The read boolean here is fixed as false, as this is the case for the write and the read,
      //because for the read we have to write the read command first (and then read the LM82 Status command,
      //which is a private method).
      readArguments = sendBEReadWriteCommand(designator, length, false, mergedArguments, noOfArgElements, maxArg);
      //Set a delay to allow for the I2C to do its job.
      fed9Uwait(500);
        if (read) {
	beCommandLm82Status(readArguments, i2cBusy, i2cError);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandLm82Control."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandLm82Control."));
    }
  }

  //These following BE commands are all read only. They return a value by reference. 
  //(Note: one needs to add 2 to the length of these commands, and also read back                                                                                                                              
  //an extra bit (this means reading back an extra word for a 32-bit length). This is
  //neccesary because the words are shifted by one to the right.)                                                                                                                                              
 //This command reads the BE FPGA debug register. This register can contain various things depending on what is being debugged.                    
   void Fed9UVmeBase::beCommandDebugRegister(u32 &readArguments)  throw (Fed9UVmeBaseException)
   {
      try {
           //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                        
          const u32 designator = 13;
          const u32 length = 32;
          const unsigned short selectedFpga = 16;
          readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
       }
      catch (std::exception &e) {
         RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandExtraEventRegister."));
       }
        catch (...) {
          THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBeExtraEventRegister."));
       }
  }
                                                                         
  //This command reads the BE FPGA Extra Event  register. Thius contains the last event number and the FE Flags when the extra event was detected                    
  void Fed9UVmeBase::beCommandExtraEventRegister(u32 &readArguments)  throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                        
      const u32 designator = 12;
      const u32 length = 32;
      const unsigned short selectedFpga = 16;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandExtraEventRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBeExtraEventRegister."));
    }
  }

  //These following BE commands are all read only. They return a value by reference.
  //(Note: one needs to add 2 to the length of these commands, and also read back
  //an extra bit (this means reading back an extra word for a 32-bit length). This is
  //neccesary because the words are shifted by one to the right.)

  //This command reads the BE FPGA status register.
  void Fed9UVmeBase::beCommandBeStatusRegister(u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 16;
      const u32 length = 32;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandBeStatusRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBeStatusRegister."));
    }
  }

  //This command reads the Level 1 accept counter.
  void Fed9UVmeBase::beCommandReadL1aCount(u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 17;
      const u32 length = 24;
      const u32 mask = 0xFFFFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandReadL1aCount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandReadL1aCount."));
    }
  }

  //This command reads the bunch crossing counter.
  void Fed9UVmeBase::beCommandReadBxCount(u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 18;
      const u32 length = 12;
      const u32 mask = 0xFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandReadBxCount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandReadBxCount."));
    }
  }


  //This command reads the number of frames stored in the QDRs.
  void Fed9UVmeBase::beCommandQdrFrameCounter(u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 19;
      const u32 length = 24;
      const u32 mask = 0xFFFFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandQdrFrameCounter."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandQdrFrameCounter."));
    }
  }

  //This command reads the firmware ID register.
  void Fed9UVmeBase::beCommandFirmwareIdReg(u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 21;
      const u32 length = 32;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
      
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFirmwareIdReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFirmwareIdReg."));
    }
  }

  //This command reads the number of unread words stored in the QDR.
  void Fed9UVmeBase::beCommandQdrDataCounter(u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 22;
      const u32 length = 18;
      const u32 mask = 0x3FFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandQdrDataCounter."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandQdrDataCounter."));
    }
  }

  //This command reads the total number of frames written to the QDR.
  void Fed9UVmeBase::beCommandTotalFrameCount(u32 &readArguments)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 23;
      const u32 length = 24;
      const u32 mask = 0xFFFFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTotalFrameCount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTotalFrameCount."));
    }
  }

  //This command reads the TTCrx status. It is a private method used by the
  //public method beCommandTtcrxControl. Two booleans are passed to
  //check for a busy or an error in the I2C system.
  //This method is private and is used by the beCommandTtcrxControl method. 
  void Fed9UVmeBase::beCommandTtcrxStatus(u32 &readArguments, bool &i2cBusy, bool &i2cError)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 24;
      const u32 length = 16;
      const u32 mask = 0xFFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
      //Test for a busy or an error
      if (readArguments & 0x100) i2cBusy = true;
      if (readArguments & 0x200) i2cError = true;
      //Just send back the first 8 bits (0-7), which are the data bits. Bits 8 and 9 are
      //the error bits which are tested for above.
      readArguments = readArguments & 0xFF;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcrxStatus."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcrxStatus."));
    }
  }

  //This is a function which reads the LM82 status. It is a read-only BE FPGA command.
  //This command is used inside the beCommandLm82Control command.
  //Two booleans are passed to check for a busy or an error in the I2C system.
  //This method is private and is used by the beCommandLm82Control method.
  void Fed9UVmeBase::beCommandLm82Status(u32 &readArguments, bool &i2cBusy, bool &i2cError)  throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 25;
      const u32 length = 16;
      const u32 mask = 0xFFFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
      //Test for a busy or an error
      if (readArguments & 0x100) i2cBusy = true;
      if (readArguments & 0x200) i2cError = true;
      //Just send back the first 8 bits (0-7), which are the data bits. Bits 8 and 9 are
      //the error bits which are tested for above.
      readArguments = readArguments & 0xFF;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandLm82Status."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandLm82Status."));
    }
  }

  //New BE commands April 2005.                                                                                                                  

  /**This command can read or write to the DAQ register in the BE FPGA. This is a 32-bit word that                           
     is also written into the FED buffer header when running in full debug mode. When writing one                                         
     should set the arguments input to the value one wants to write. When reading the readArguments input
     will be set equal to the value read from the register.*/
  void Fed9UVmeBase::beCommandDaqRegister(bool read,
					  u32 arguments,
					  u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                 
      const u32 designator = 13;
      u32 length = 32;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFFFFFFF; 
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandDaqRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandDaqRegister."));
    }
  }


  //<JRF date="18/08/2006">
  /**This command can read or write to the DAQ register Two in the BE FPGA. This is a 32-bit word that                           
     is also written into the FED buffer header when running in full debug mode. When writing one                                         
     should set the arguments input to the value one wants to write. When reading the readArguments input
     will be set equal to the value read from the register.*/
  void Fed9UVmeBase::beCommandDaqRegisterTwo(bool read,
					  u32 arguments,
					  u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                 
      const u32 designator = 3;
      u32 length = 32;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFFFFFFF; 
      const unsigned short selectedFpga = 16;
      readArguments = sendBEReadWriteCommand(selectedFpga, designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandDaqRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandDaqRegister."));
    }
  }

  /**This command can read or write the tracker header format type flag. This determins the header format
     of the FED data buffer.
     1=Full debug mode
     2=APV error mode
     This flag is also written in the tracker special header word in every FED event buffer.
     The length of the arguments is 4-bits, although currently (as of April 2005) only the first
     two bits are relevant.*/
  void Fed9UVmeBase::beCommandHeaderFormatType(bool read,
					       u32 arguments,
					       u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                   
      const u32 designator = 14;
      u32 length = 4;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0x2;                                      
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandHeaderFormatType."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandHeaderFormatType."));
    }
  }

  /**This command can read or write the bunch crossing offset number. This offset is applied to the Bx counter
     at the start of each LHC orbit and is also applied to the Bx number witten to the FED data buffer header.
     The offset is a 12-bit number. An input of arguments=1 will provide an offset of 1 to the Bx number, an input
     of 0 will provide no offset.*/
  void Fed9UVmeBase::beCommandBunchCrossingOffset(bool read,
						  u32 arguments,
						  u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                        
      const u32 designator = 15;
      u32 length = 12;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFF;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::Fed9UBunchCrossingOffset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::Fed9UBunchCrossingOffset."));
    }
  }

  /**This command reads the TTC individually addressed command sent to the FED over TTC channel B. 
     There are 39 bits in total (see TTC documentation for detailed information). 
     Bits 6-0 = Check word 
     Bits 14-7 = Data word
     Bits 22-15 = Sub address word
     Bit 23 = set to 1
     Bit 24 = 0 or 1 (0=internal, 1=external)
     Bits 38-25 = TTCrx address word.*/
  void Fed9UVmeBase::beCommandTtcIndividuallyAddressedCommand(vector<u32> &readArguments)  throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                           
      const u32 designator = 26;
      const u32 length = 39;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcIndividuallyAddressedCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcIndividuallyAddressedCommand."));
    }   
  }

  /**This command reads the TTC broadcast command sent to the FED over TTC channel B. There are 13 bits          
   in the command.                                                                                                                  
   Bits 4-0 = Check word                                                                                                    
   Bits 12-5 = command / data.*/
  void Fed9UVmeBase::beCommandTtcBroadcastCommand(u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                       
      const u32 designator = 27;
      const u32 length = 13;
      const u32 mask = 0x1FFF;
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcBroadcastCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcBroadcastCommand."));
    }
  }

  /**This command reads or writes the DAQ Event type register. This number is placed in the DAQ header 
     in every FED data buffer. It is 4 bits.*/
  void Fed9UVmeBase::beCommandDaqEventType(bool read,
					   u32 arguments,
					   u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                             
      const u32 designator = 30;
      u32 length = 4;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xF;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandDaqEventType."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandDaqEventType."));
    }
  }

  /**This command reads or writes the DAQ FOV register. This number is placed in the DAQ header in 
     every FED data buffer. It is 4 bits.*/
  void Fed9UVmeBase::beCommandDaqFov(bool read,
				     u32 arguments,
				     u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                
      const u32 designator = 31;
      u32 length = 4;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xF;
      readArguments = sendBEReadWriteCommand(designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandDaqFov."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandDaqFov."));
    }
  }

  void Fed9UVmeBase::beCommandBunchCrossingOrbitTotal(bool read,
						      u32 arguments,
						      u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                              
      const u32 designator = 1;
      const u32 length = 12;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFF;
      //This is one the BE commands that has spilled over into a new fpga number (which is 16 rather than 10).
      const unsigned short selectedFpga = 16;
      readArguments = sendBEReadWriteCommand(selectedFpga, designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandBunchCrossingOrbitTotal."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBunchCrossingOrbitTotal."));
    }
  }

  //<JEC date=16/03/07> new register for scaling down triggers to the FED
  void Fed9UVmeBase::beCommandFrameCapturePrescale(bool read,
						   u32 arguments,
						   u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 4;
      const u32 length = 16;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFFF;
      //This is one the BE commands that has spilled over into a new fpga number (which is 16 rather than 10).
      const unsigned short selectedFpga = 16;
      readArguments = sendBEReadWriteCommand(selectedFpga, designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFrameCapturePrescale."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFrameCapturePrescale."));
    }
  }
  //</JEC>


  /**This command reads the bunch crossing total - the value of the Bx counter expected on recipt of a new orbit signal.
     It is a 12-bit number. It should be the same at the end of each orbit. It depends on the value of the Bx offset set.*/
  void Fed9UVmeBase::beCommandBunchCrossingOrbitTotal(u32 &readArguments)  throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                              
      const u32 designator = 1;
      const u32 length = 12;
      //not used //const unsigned short noOfArgElements = 1;
      const u32 mask = 0xFFF;
      //This is one the BE commands that has spilled over into a new fpga number (which is 16 rather than 10).
      const unsigned short selectedFpga = 16;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandBunchCrossingOrbitTotal."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandBunchCrossingOrbitTotal."));
    }
  }

  /**Returns the contents of the FMM test register.  It is a 4-bit number which is used to test the signals to the FMM
     manually.  This register is enabled via bit 5 of the main test register, but should not be enabled during normal
     operation.*/
  // JEC 6/3/06 new test register added
  void Fed9UVmeBase::beCommandFmmTestReg(bool read, 
  					 u32 arguments,
  					 u32 &readArguments)   throw (Fed9UVmeBaseException)
  {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 2;
      u32 length = 4;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xF; 
      // This is one of the BE commands that has spilled over into a new fpga number (16 rather than 10)
      const unsigned short selectedFpga = 16;
      readArguments = sendBEReadWriteCommand(selectedFpga, designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFmmTestReg."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFmmTestReg."));
    }
  }

  /**This command reads the tracker header monitor. This contains The L1A ID and all the front end frame sync out packets.
     The length is 664 bits.                                                                                                        
     Bits 663-640 = L1A ID                                                                                                         
     Bits 639-560 = FE 1 frame sync out packet                                                                                       
     Bits 559-480 = FE 2 frame sync out packet                                                                                       
     Bits 479-400 = FE 3 frame sync out packet                                                                                       
     Bits 399-320 = FE 4 frame sync out packet                                                                                       
     Bits 319-240 = FE 5 frame sync out packet                                                                                         
     Bits 239-160 = FE 6 frame sync out packet                                                                                          
     Bits 159-80 = FE 7 frame sync out packet                                                                                         
     Bits 79-0 = FE 8 frame sync out packet                                                                                          
     Within each frame sync out packet we have:                                                                                    
     Bits 79-72 = Majority APV pipeline address                                                                                     
     Bits 71-66 = Fibre 12 status word                                                                                          
     Bits 65-60 = Fibre 11 status word                                                                                      
     Bits 59-54 = Fibre 10 status word                                                                                   
     Bits 53-48 = Fibre 9 status word                                                                                          
     Bits 47-42 = Fibre 8 status word                                                                                                  
     Bits 41-36 = Fibre 7 status word                                                                                               
     Bits 35-30 = Fibre 6 status word                                                                                             
     Bits 29-24 = Fibre 5 status word                                                                                               
     Bits 23-18 = Fibre 4 status word                                                                                           
     Bits 17-12 = Fibre 3 status word                                                                                          
     Bits 11-6 = Fibre 2 status word                                                                                              
     Bits 5-0 = Fibre 1 status word.*/
  void Fed9UVmeBase::beCommandTrackerHeaderMonitor(vector<u32> &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                        
      const u32 designator = 28;
      const u32 length = 664;                                    
      const unsigned short selectedFpga = 10;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTrackerHeaderMonitor."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTrackerHeaderMonitor."));
    }
  }

  /**This method read the long_read_1 register. It is 64 bits. This register also holds:
     bit 15 = TTS Read
     bit 14 = TTS Warn Overflow
     bit 13 = TTS Busy
     bit 12 = TTS Out of Sync
     bits 11-0 = Bx counter value at orbit.*/
  void Fed9UVmeBase::beCommandLongReadRegister(vector<u32> &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                 
      const u32 designator = 29;
      const u32 length = 64;
      const unsigned short selectedFpga = 10;

      // length of command depends on firmware version
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandLongReadRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandLongReadRegister."));
    }
  }

  void Fed9UVmeBase::beCommandLongReadRegister(u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                 
      const u32 designator = 29;
      const u32 length = 32;
      const unsigned short selectedFpga = 10;
      
      // length of command depends on firmware version
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, 0x0);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandLongReadRegister."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandLongReadRegister."));
    }
  }
  


  void Fed9UVmeBase::beCommandSpyArmTtcBData(bool read,
					     u32 arguments,
					     u32 &readArguments) throw (Fed9UVmeBaseException) {

    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 5;
      const u32 length = 16;
      const unsigned short noOfArgElements = 1;
      const u32 maxArg = 0xFFFF;
      //This is one the BE commands that has spilled over into a new fpga number (which is 16 rather than 10).
      const unsigned short selectedFpga = 16;
      readArguments = sendBEReadWriteCommand(selectedFpga, designator, length, read, arguments, noOfArgElements, maxArg);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandFrameCapturePrescale."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandFrameCapturePrescale."));
    }
  }

  // <NAC date="25/06/2009"> Added second spy L1A/TotalEvent register commands
  void Fed9UVmeBase::beCommandSpyTotalEventCount( u32 &readArguments, u32 length) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 7;
      //const u32 length = 24; note that length now depends on the firmware version so this is passed in based upon which version we are using
      const unsigned short selectedFpga = 16;
      u32 mask;

      if (length == 32 ) 
        mask = 0x0;
      else 
        mask = 0x00FFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyTotalEventCount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyTotalEventCount."));
    }
  }

  void Fed9UVmeBase::beCommandSpyL1ACount( u32 &readArguments, u32 length) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 6;
      //const u32 length = 24; note that length now depends on the firmware version so this is passed in based upon which version we are using
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0xFFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }
  // </NAC>

  void Fed9UVmeBase::beCommandTotalEventCount32( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                  
      const u32 designator = 9;
      const u32 length = 32;          
      const unsigned short selectedFpga = 16;
      u32 mask;

      if (length == 32 )
        mask = 0x0;
      else
        mask = 0x00FFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyTotalEventCount.")\
	      );
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyTotalEventCount."));
    }
  }

  void Fed9UVmeBase::beCommandTotalL1ACount32( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                  
      const u32 designator = 8;
      const u32 length = 32;           
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0xFFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }
                                                                                                                                              
  void Fed9UVmeBase::beCommandTotalEventOutCount( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                                                                               
      const u32 designator = 15;
      const u32 length = 24;
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0xFFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }

  void Fed9UVmeBase::beCommandFeFrameLengthCount( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS
      const u32 designator = 16;
      const u32 length = 24;
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0xFFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }

  void Fed9UVmeBase::beCommandDummyEventCount( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                                                                               
      const u32 designator = 17;
      const u32 length = 24;
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0xFFFFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }
  void Fed9UVmeBase::beCommandPayloadLength( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                   
      const u32 designator =19;                                                   
      const u32 length = 18;                                                                                                                         
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0x3FFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }
  void Fed9UVmeBase::beCommandPayloadDownCount( u32 &readArguments) throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                                                                                                                      
      const u32 designator = 20;
      const u32 length = 18;                                                                                                               
      const unsigned short selectedFpga = 16;
      u32 mask;
      if (length == 32 )
        mask = 0x0;
      else
        mask = 0x3FFFF;

      readArguments = sendReadOnlyCommand(selectedFpga, designator, length, mask);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandSpyL1ACount."));
    }
  }

  void Fed9UVmeBase::beCommandDebugRegTwo(vector<u32> &readArguments)  throw (Fed9UVmeBaseException) {
    try {
      //DEFINE COMMAND SPECIFIC NUMBERS                                                                                                                                                                                                                               
      const u32 designator = 18;
      const u32 length = 52;
      const unsigned short selectedFpga = 16;
      readArguments = sendReadOnlyCommand(selectedFpga, designator, length);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::beCommandTtcIndividuallyAddressedCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::beCommandTtcIndividuallyAddressedCommand."));
    }
  }



  //**************************************************************************************************
  //**************************************************************************************************
  //VME-FPGA Commands. These do not need to specify an fpga number, as the
  //VME commands are in the form of direct writes to command specific areas
  //of VME memory. See the HAL address table for more information.

  /**This command will reset the FED.*/
  void Fed9UVmeBase::vmeCommandSoftwareReset(bool doWait)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.writeRegister(1, "FED_RESET");
      if (doWait)
	fed9Uwait(FED9U_RESET_DELAY);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
  }

  /**This command will reset the FED VME Vlink counters only and will not perform a full logic reset on the fed.*/
  void Fed9UVmeBase::vmeCommandVLinkReset(void)   throw (Fed9UVmeBaseException) 
  {
    try {
      cout << "VLink REset performed here!!!!!!!!" << endl;
      theFed9UHalInterface.writeRegister(1, "VLINK_SW_RST");
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
  }

  void  Fed9UVmeBase::vmeCommandResetBus() throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.resetBus();
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
  }

  
  /**This command inhibits the VME BUS reset.*/
  void Fed9UVmeBase::vmeCommandResetBusInhibit(bool value) throw (Fed9UVmeBaseException) {
    try {
      cout << "Inhibiting bus reset!!!!!!!!" << endl;
      theFed9UHalInterface.writeRegister(value?1:0, "SYS_RST_INHIBIT");
      cout << "Inhibited bus reset!!!!!!!!" << endl;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSoftwareReset."));
    }
                                                  
  }

    

  /**This command will read the the FED board specifier, should be 0x00000FED.*/
  void Fed9UVmeBase::vmeCommandReadFedBoardSpecifier(u32& readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("FED_BOARD_SPECIFIER", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandReadFEDSpecifier."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandReadFEDSpecifier."));
    }
  }

  /**This command reset the readout control status.*/
  void Fed9UVmeBase::vmeCommandResetControlStatus(void)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.writeRegister(0, "ROUT_CSR");
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandResetControlStatus."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandResetControlStatus."));
    }
  }

  /**This command checks for an event. Returns 1 if an event is present. Returns 0 if empty and ready to read next event.*/
  void Fed9UVmeBase::vmeCommandReadoutBufferStatus(u32& readArguments)  throw (Fed9UVmeBaseException)  
  {
    try {
      theFed9UHalInterface.readRegister("ROUT_CSR", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandReadoutBufferStatus."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandReadoutBufferStatus."));
    }
  }

  /**This command reads the event counter.*/
  void Fed9UVmeBase::vmeCommandReadEventCounter(u32& readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("ROUT_EVT_CTR", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandReadEventCounter."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandReadEventCounter."));
    }
  }

  /**This command reads the contents of the buffer block in the RAM.*/
  void Fed9UVmeBase::vmeCommandReadBufferBlock(u32* readArguments, u32 offset)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("ROUT_BRAM", *readArguments, offset);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandReadBufferBlock."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandReadBufferBlock."));
    }
  }

  /**This command performs a block read of the contents of the buffer block in the RAM.*/
  void Fed9UVmeBase::vmeCommandBlockReadBufferBlock(u32 length, char* const readArguments, u32 offset)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.blockReadRegister("ROUT_BRAM_BLT", readArguments, offset, length);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandBlockReadBufferBlock."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandBlockReadBufferBlock."));
    }
  }

  /**This command reads the length of the events in the buffer. Returns the number of 32-bit words in event (could be more than one Vlink RAM).*/
  void Fed9UVmeBase::vmeCommandReadBufferLength(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("ROUT_BUF_LEN", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandReadBufferLength."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandReadBufferLength."));
    }
  }

  /** This command selects the clock. A 1 will select the on-board crystal clock, 2 the
      TTC clock and 4 the VME backplane clock. If the clock is successfully changed
      then a fed soft reset is performed. */
  void Fed9UVmeBase::vmeCommandSelectClock(bool read, 
                                           u32 arguments,
                                           u32 &readArguments,
					   bool doWait)   throw (Fed9UVmeBaseException) 
  {
    try {
      if (read) {
	theFed9UHalInterface.maskedReadRegister("CLOCK_SEL", readArguments);
      } else {
	theFed9UHalInterface.maskedWriteRegister(arguments, "CLOCK_SEL", HAL_NO_VERIFY);
	if (doWait)	
	  fed9Uwait(FED9U_RESET_DELAY);
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSelectClock."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSelectClock."));
    }
  }

  /**This command will read the VME status.*/
  void Fed9UVmeBase::vmeCommandStatus(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("VME_STATUS", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandStatus."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandStatus."));
    }
  }

  /**This command will read the number of TTC clock cycles.*/
  void Fed9UVmeBase::vmeCommandTtcClockCounter(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.maskedReadRegister("TTC_CLK_CTR", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandTtcClockCounter."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandTtcClockCounter."));
    }
  }

  /**This command will read the number of backplane clock cycles.*/
  void Fed9UVmeBase::vmeCommandBackplaneClockCounter(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.maskedReadRegister("BP_CLK_CTR", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandBackplaneClockCounter."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandBackplaneClockCounter."));
    }  
  }

  /**This method will read the VME firmware ID register.*/
  void Fed9UVmeBase::vmeCommandFirmwareID(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("FIRMWARE_ID", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandFirmwareID."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandFirmwareID."));
    } 
  }

  /**This method can read the STATUS_SERIAL register. 0xffffffff if free, 0x0 if busy.*/
  void Fed9UVmeBase::vmeCommandStatusSerial(u32 &readArguments)   throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.readRegister("STATUS_SERIAL", readArguments);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandStatusSerial."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandStatusSerial."));
    } 
  }

  //This command controls the EPROM via the VME FPGA. The VME FPGA communicates 
  //with the sensor via an I2C protocol built into the firmware.
  //The arguments vector has two elements. The first element 
  //contains 8 relavent bits, which is the data sent to the EPROM in the case 
  //of a write. The 2nd element contains the 11-bit EPROM register number.
  //The i2cBusy and i2cError booleans will return a false if
  //the reading of the EPROM succeded (in the case of a write they
  //will both always return a false).
  //Note:
  //arguments[0] = data 8-bits
  //arguments[1] = register 11-bits
  void Fed9UVmeBase::vmeCommandEpromControl(bool read, 
					    const vector<u32> &arguments,
					    u32 &readArguments,
					    bool &i2cBusy,
					    bool &i2cError)   throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE INPUT LIMITS
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0x7ff; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::vmeCommandEpromControl.");
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::vmeCommandEpromControl.");

      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is the 11-bit EPROM register address. The 20th bit is the read/write bit.
      u32 mergedArguments = 0;
      mergedArguments = (read << 19) + (arguments[1] << 8) + (arguments[0]);

      //The read boolean here is fixed as false, as this is the case for the write and the read,
      //because for the read we have to write the read command first (and then read the LM82 Status command,
      //which is a private method).
      theFed9UHalInterface.maskedWriteRegister(mergedArguments, "EPROM_WRITE");
      //Set a delay to allow for the I2C to do its job.
    fed9Uwait(500);
      if (read) {
	u32 tempReadArguments = 0;
	theFed9UHalInterface.maskedReadRegister("EPROM_READ", tempReadArguments);
	readArguments = tempReadArguments & 0xff; //Get the 8-bit data
	i2cBusy = (tempReadArguments >> 8) & 0x1; //Get the I2C Busy Flag
	i2cError = (tempReadArguments >> 9) & 0x1; //Get the I2C Error Flag
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandEPROMControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandEPROMControl."));
    }
    
  }

  //This command controls the LM82 via the VME FPGA. The VME FPGA communicates with the sensor via an 
  //I2C protocol built into the firmware. The arguments vector has two elements. The first element 
  //contains 8 relavent data bits, which is the data sent to the LM82 in the case of a write. The 2nd element 
  //contains the 8-bit LM82 register number. If one is reading then only the register number is important.
  //The i2cBusy and i2cError booleans will return a false if the reading of the LM82 succeded 
  //(in the case of a write they will both always return a false).
  //Note: arguments[0] = data 8-bits
  //      arguments[1] = register 8-bits
  void Fed9UVmeBase::vmeCommandLm82Control(bool read, 
					   const vector<u32> &arguments,
					   u32 &readArguments,
					   bool &i2cBusy,
					   bool &i2cError)  throw (Fed9UVmeBaseException)  
  {
    try {
      //DEFINE INPUT LIMITS
      const u32 maxArgData = 0xff; //Maximum value of the data.
      const u32 maxArgReg = 0xff; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[0]<=maxArgData,Fed9UVmeBaseException)(arguments[0])(maxArgData).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[0] exceeds maximum value in Fed9UVmeBase::vmeCommandLm82Control.");
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::vmeCommandLm82Control.");

      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is the 8-bit LM82 register address. The 17th bit is the read/write bit.
      u32 mergedArguments = 0;
      mergedArguments = (read << 16) + (arguments[1] << 8) + (arguments[0]);

      //The read boolean here is fixed as false, as this is the case for the write and the read,
      //because for the read we have to write the read command first (and then read the LM82 Status command,
      //which is a private method).
      //std::time_t result = std::time(NULL);
      //std::cout << std::ctime(&result) << " In vmeCommandLm82Control about to writeRegister()" << std::endl;
      theFed9UHalInterface.maskedWriteRegister(mergedArguments, "LM82_CONTROL");
      //result = std::time(NULL);
      //std::cout << std::ctime(&result) << " In vmeCommandLm82Control completed writeRegister()" << std::endl;

      //Set a delay to allow for the I2C to do its job.
      fed9Uwait(500);
        if (read) {
	u32 tempReadArguments = 0;
    //result = std::time(NULL);
    //std::cout << std::ctime(&result) << " In vmeCommandLm82Control about to readRegister()" << std::endl;
	theFed9UHalInterface.maskedReadRegister("LM82_STATUS", tempReadArguments);
    //result = std::time(NULL);
    //std::cout << std::ctime(&result) << " In vmeCommandLm82Control completed readRegister()" << std::endl;
	readArguments = tempReadArguments & 0xff; //Get the 8-bit data
	i2cBusy = (tempReadArguments >> 8) & 0x1; //Get the I2C Busy Flag
	i2cError = (tempReadArguments >> 9) & 0x1; //Get the I2C Error Flag
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandLm82Control."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandLm82Control."));
    }
  }

  //This command controls the voltage monitor via the VME FPGA. The VME FPGA communicates with the sensor via an 
  //I2C protocol built into the firmware. The arguments vector has two elements. The first element 
  //contains 8 relavent data bits, which is the data sent to the voltage monitor in the case of a write. The 2nd element 
  //contains the 8-bit volatage monitor register number. If one is reading then only the register number is important.
  //The i2cBusy and i2cError booleans will return a false if the reading of the voltage monitor succeded 
  //(in the case of a write they will both always return a false).
  //Note: arguments[0] = data 8-bits
  //      arguments[1] = register 8-bits
  void Fed9UVmeBase::vmeCommandVoltageMonitorControl(bool read, 
						     const vector<u32> &arguments,
						     u32 &readArguments,
						     bool &i2cBusy,
						     bool &i2cError)   throw (Fed9UVmeBaseException) 
  {
    try {
      //DEFINE INPUT LIMITS
      const u32 maxArgData = 0xff; //Use this to mask data.
      const u32 maxArgReg = 0xff; //Maximum value of the register.

      //Set ttcrx busy and error flags to false
      i2cBusy = false;
      i2cError = false;

      //Check input arguments
      ICUTILS_VERIFYX(arguments[1]<=maxArgReg,Fed9UVmeBaseException)(arguments[1])(maxArgReg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments[1] exceeds maximum value in Fed9UVmeBase::vmeCommandVoltageMonitorControl.");

      //Merge the arguments into one 32-bit integer. On the LSB side there are the 8 data bits,
      //then there is the 8-bit voltage monitor register address. The 17th bit is the read/write bit.
      u32 mergedArguments = 0;
      mergedArguments = (read << 16) + (arguments[1] << 8) + (arguments[0]&maxArgData);

      //The read boolean here is fixed as false, as this is the case for the write and the read,
      //because for the read we have to write the read command first (and then read the voltage monitor Status command,
      //which is a private method).
      theFed9UHalInterface.maskedWriteRegister(mergedArguments, "V_MON_CONTROL");
      //Set a delay to allow for the I2C to do its job.
    fed9Uwait(500);
      if (read) {
	u32 tempReadArguments = 0;
	theFed9UHalInterface.maskedReadRegister("V_MON_STATUS", tempReadArguments);
	readArguments = tempReadArguments & 0xff; //Get the 8-bit data
	i2cBusy = (tempReadArguments >> 8) & 0x1; //Get the I2C Busy Flag
	i2cError = (tempReadArguments >> 9) & 0x1; //Get the I2C Error Flag
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandVoltageMonitorControl."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandVoltageMonitorControl."));
    }
  }

  //Enables one to write to the System ACE data buffers. 
  //The offset is the offset (in bytes) from the SYS_ACE_DATA register (see HAL address table). 
  //The default offset is zero.
  void Fed9UVmeBase::vmeCommandSysAceDataWrite(u32 arguments, u32 offset) throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.writeRegister(arguments, "SYS_ACE_DATA", HAL_NO_VERIFY, offset << 2);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSysAceDataWrite."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSysAceDataWrite."));
    }
  }
  
  //Enables one to read from the System ACE data buffers, one byte at a time. 
  //The offset is the offset (in bytes) from the SYS_ACE_DATA register (see HAL address table). 
  //The default offset is zero.
  u32 Fed9UVmeBase::vmeCommandSysAceDataRead(u32 offset) throw (Fed9UVmeBaseException) 
  {
    try {
      u32 readArguments = 0;
      theFed9UHalInterface.readRegister("SYS_ACE_DATA", readArguments, offset << 2);
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSysAceDataRead."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSysAceDataRead."));
    }
    
  }

  //Enables one to write to the System ACE control registers.
  //The control registers occupy 0x880 to 0x8ff inclusive. The default offset is zero.
  void Fed9UVmeBase::vmeCommandSysAceControlWrite(u32 arguments, u32 offset) throw (Fed9UVmeBaseException) 
  {
    try {
      theFed9UHalInterface.writeRegister(arguments, "SYS_ACE_CONTROL", HAL_NO_VERIFY, offset << 2);
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSysAceControlWrite."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSysAceControlWrite."));
    }
  }

  //Enables one to read from the System ACE control registers.
  //The control registers occupy 0x880 to 0x8ff inclusive. The default offset is zero.
  u32 Fed9UVmeBase::vmeCommandSysAceControlRead(u32 offset) throw (Fed9UVmeBaseException) 
  {
    try {
      u32 readArguments = 0;
      theFed9UHalInterface.readRegister("SYS_ACE_CONTROL", readArguments, offset << 2);
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::vmeCommandSysAceControlRead."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::vmeCommandSysAceControlRead."));
    }
  }

  //****************************************************************************************************
  //****************************************************************************************************
  //Private utility methods
  //****************************************************************************************************
  //****************************************************************************************************

  //****************************************************************************************************
  //Method to pack the pedData (in the feCommandLoadPedData command) into the correct FED writable format.
  //In the following, the strip range is 0-255, ie. two APVs (which is one fiber).
  //The command has the feature that one has to write to the same strip number for both fibers at 
  //the same time (eg. strip 2 in APV1 in fiber 0, and strip 2 in APV1 in fiber 1).
  //If setting the pedestal, the argument list is: Exclude strip from fiber 0 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 0
  //                                               Exclude strip from fiber 1 in CM calculation (0=yes, 1=no)
  //                                               Pedestal value for fiber 1
  //If setting cluster thresholds, the argument list is: 1st cluster threshold for fiber 0
  //                                                     2nd cluster threshold for fiber 0
  //                                                     1st cluster threshold for fiber 1
  //                                                     2nd cluster threshold for fiber 1
  //setCluster=false means set pedestal
  //setCluster=true  means set cluster thresholds
  void Fed9UVmeBase::packPedData(const vector<u32> &arguments, 
                                 vector<u32> &newArguments, 
                                 bool setCluster)   throw (Fed9UVmeBaseException) 
  {
    try {
      //Check for input errors
      if (!setCluster) {
	//Check for input errors
	/*for (int i=0; i<=3; i++) {
	  u32 argLimit = 1;
	  if ((i==1)||(i==3)) argLimit = 0x3ff;
	  ICUTILS_VERIFYX(arguments[i]<=argLimit,Fed9UVmeBaseException)(i)(arguments[i]).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("An element of the arguments vector has incorrect value in Fed9UVmeBase::packPedData.");
	  }*/
	newArguments.push_back((arguments[2]<<(32-8))); 
	u32 newArgsLastIndex = newArguments.size()-1;
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[3] << (32-18));
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[0] << (32-26));
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[1] >> 4); //Put 6 MSB of arguments[1] into last 6 elements in newArguments[0] 
	newArguments.push_back(arguments[1] << (32-4)); //Put 4 LSBs of arguments[1] into first 4 elements of newArguments[1].
      }
      else if (setCluster) {
	for (int i=0; i<=3; i++) {
	  ICUTILS_VERIFYX(arguments[i]<=0xff,Fed9UVmeBaseException)(i)(arguments[i]).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("An element of the arguments vector has incorrect value in Fed9UVmeBase::packPedData.");
	}
	newArguments.push_back((arguments[3]<<(32-9)));
	u32 newArgsLastIndex = newArguments.size()-1;
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[2] << (32-18));
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[1] << (32-27));
	newArguments[newArgsLastIndex] = newArguments[newArgsLastIndex] + (arguments[0] >> 4); //Put 4 MSB of arguments[0] into last 4 elements in newArguments[0]
	newArguments.push_back(arguments[0]<<(32-4)); //Put 4 LSBs of arguments[0] into first 4 elements of newArguments[1].
      }    
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::packPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::packPedData."));
    }
  }

  //****************************************************************************************************
  //Method to unpack the pedData (in the loadPedPata command) from the correct FED writable format into 
  //the format in which the data is passed into loadPedData for a write.
  void Fed9UVmeBase::unpackPedData(const vector<u32> &readArgumentsTemp, 
                                   vector<u32> &readArguments, 
                                   bool setCluster)  throw (Fed9UVmeBaseException)  
  { 
    try {
      ICUTILS_VERIFYX(readArgumentsTemp.size()==2,Fed9UVmeBaseException)(readArgumentsTemp.size()).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("readArgumentsTemp vector is incorrect size in Fed9UVmeBase::unpackPedData.");
      u32 tempReadArgument = 0;
      readArguments.clear();
      if (!setCluster) {
	readArguments.push_back((readArgumentsTemp[0] >> 6) & 1); //Extract the fiber 0 valid strip bit by shifting by 6 and masking by 1.
	tempReadArgument = readArgumentsTemp[1] >> (32-4); //Get fiber 0 value by taking the 4MSBs of the 2nd element and put in 4LSBs.
	tempReadArgument = tempReadArgument + ((readArgumentsTemp[0] & 0x3F) << 4); //Now add in the 6LSB of 1st element to the left of the existing 4 bits in readArguments_2.
	readArguments.push_back(tempReadArgument);
	readArguments.push_back((readArgumentsTemp[0] >> 24) & 1); //Extract the fiber 1 valid strip bit by shifting by 24 and masking by 1.
	readArguments.push_back((readArgumentsTemp[0] >> 14) & 0x3FF); //Extract the fiber 1 value by shifting by 14 and masking by 10 1's.
      }
      else if (setCluster) {
	tempReadArgument = readArgumentsTemp[1] >> (32-4); //Get 4LSB of fiber 0 threshold 1
	tempReadArgument = tempReadArgument + ((readArgumentsTemp[0] & 0xF)<<4); //Get 4MSB of fiber 0 threshold 1
	readArguments.push_back(tempReadArgument);
	readArguments.push_back((readArgumentsTemp[0] >> 5) & 0xFF); //Get fiber 0 threshold 2
	readArguments.push_back((readArgumentsTemp[0] >> 14) & 0xFF); //Get fiber 1 threshold 1
	readArguments.push_back((readArgumentsTemp[0] >> 23) & 0xFF); //Get fiber 1 threshold 2
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::unpackPedData."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::unpackPedData."));
    }
  }


  //**************************************************************************************************
  //Method that is used for the FE FPGA commands only. It checks that the FPGA number
  //which is passed to the function is valid for a FE FPGA command.
  void Fed9UVmeBase::checkFpgaNumber(unsigned short selectedFpga)   throw (Fed9UVmeBaseException) 
  {
    //Check if Front End commands are using a valid FPGA number. Valid numbers are 1 to 8, or 15 (not 10, which is just for the BE FPGA, and not 9 which is used for the software trigger.).
    //  ICUTILS_VERIFYX(((selectedFpga>=1)&&(selectedFpga<=8))||(selectedFpga==15),Fed9UVmeBaseException)(selectedFpga).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("The selectedFpga number is inconsistent with the FE commands in Fed9UVmeBase::checkFpgaNumber.");
    ICUTILS_VERIFYX(((selectedFpga>=1)&&(selectedFpga<=8))||(selectedFpga==15),Fed9UVmeBaseException)(Fed9UAddress().setFedFeUnit(selectedFpga).getExternalFedFeUnit()).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("The selectedFpga number is inconsistent with the FE commands in Fed9UVmeBase::checkFpgaNumber.");

  }


  //***************************************************************************************************
  //Method to check the number and range of each input argument vector. This method takes
  //as input the argument vector, the number of expected elements in the argument vector and the 
  //expected maximum value of each element.
  //An error is thrown if either of these are exceeded. This method only applies when writing to the FED.
  //The selectedFpga and fe are passed to this method to check that the fpga number is consistent
  //with a front-end FPGA command.
  void Fed9UVmeBase::checkArguments(const vector<u32> &arguments,
                                    unsigned short noOfArguments, 
                                    u32 maxArg, 
                                    bool read)   throw (Fed9UVmeBaseException) 
  {
    try {
      //Check data
      if (!read) {
	ICUTILS_VERIFYX(arguments.size()==noOfArguments,Fed9UVmeBaseException)(arguments.size())(noOfArguments).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments vector is incorrect size in Fed9UVmeBase::checkArguments.");
	vector<u32>::const_iterator i;
	for (i=arguments.begin(); i!=arguments.end(); i++) {
	  ICUTILS_VERIFYX(*i<=maxArg,Fed9UVmeBaseException)(*i)(maxArg).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("arguments vector element exceeds maximum value in Fed9UVmeBase::checkArguments.");
	}
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::checkArguments."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::checkArguments."));
    }
  }


  //****************************************************************************************************
  //This method contains some common lines of code used in the read/write BE commands
  u32 Fed9UVmeBase::sendBEReadWriteCommand(u32 designator,
                                                     u32 length,
                                                     bool read,
                                                     u32 arguments,
                                                     unsigned short noOfArgElements,
                                                     u32 maxArg)   throw (Fed9UVmeBaseException) 
  {
    try {
      if (read) length++;
      unsigned short selectedFpga;
      //the if statement has been added for the software trigger as the firmware require fpga = 9. 11/08/03 GJR
      if (designator == 8) {
	selectedFpga = 9; //this is the case for the software trigger.
      } else {
	selectedFpga = 10; //This is always 10 for the BE-FPGA.
      }    

      vector<u32> argumentsVector(1, arguments);
      vector<u32> command;
      u32 readArguments = 0;
      
      //cout << "argumentsVector[0]: " << argumentsVector[0] << endl;
      
      checkArguments(argumentsVector, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, argumentsVector, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	vector<u32> readArgumentsVector = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
	readArguments = readArgumentsVector[0];
      }   
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::sendBEReadWriteCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::sendBEReadWriteCommand."));
    }
  }

  // JEC 6/3/06
  // overloaded method
  u32 Fed9UVmeBase::sendBEReadWriteCommand(unsigned short selectedFpga,
						     u32 designator,
                                                     u32 length,
                                                     bool read,
                                                     u32 arguments,
                                                     unsigned short noOfArgElements,
                                                     u32 maxArg)   throw (Fed9UVmeBaseException) 
  {
    try {
      if (read) length++;
      vector<u32> argumentsVector(1, arguments);
      vector<u32> command;
      u32 readArguments = 0;
      
      //cout << "argumentsVector[0]: " << argumentsVector[0] << endl;
      
      checkArguments(argumentsVector, noOfArgElements, maxArg, read);
      theStringConstructor.constructCommand(selectedFpga, read, designator, length, argumentsVector, true, command);
      if (!read) {
	theFed9UHalInterface.writeSerialCommand(command);
      } 
      else {
	vector<u32> readArgumentsVector = theFed9UHalInterface.readSerialCommand(command[0], --length, true);
	readArguments = readArgumentsVector[0];
      }   
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::sendBEReadWriteCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::sendBEReadWriteCommand."));
    }
  } 


  //*****************************************************************************************************
  //This method contains some common lines of code used in the read_only commands for the BE AND FE FPGAs.
  //It returns the number read from the register. If the number being read is <32-bits
  //one has to pass a mask equal to the maximum value that number can have.
  //If the length >= 32 then one can pass a dummy mask.
  //NOTE:: This method is overloaded. There is another of the same same which returns a vector
  //in the case of length>32.
  u32 Fed9UVmeBase::sendReadOnlyCommand(unsigned short selectedFpga,
                                                  u32 designator,
                                                  u32 length,
                                                  u32 mask)   throw (Fed9UVmeBaseException) 
  {
    try {
      //Check the length, to make sure that this method isn't being called when length>32.                                                        
      ICUTILS_VERIFYX(length<=32,Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("The length is >32 in the overloaded version of Fed9UVmeBase::sendReadOnlyCommand -  the one that expects a length<=32.");
      const bool read = true;
      vector<u32> argumentsVector;
      vector<u32> command;
      u32 readArguments = 0;

      //For these read only commands we have to add 2 to the length.
      theStringConstructor.constructCommand(selectedFpga, read, designator, length+2, argumentsVector, true, command);
      //Only add 1 to the length as we need to read back an extra bit.
      vector<u32> readArgumentsVector = theFed9UHalInterface.readSerialCommand(command[0], length+1, false);

      if (length==32) {
	//If the length of the command is 32 then the data is in >1 vector elements.                                              
	//To reorganise the data so it is in the right order, we use the reOrganiseReadOnlyVector method.                  
	//It takes the MSB of the n+1 element and puts it as the LSB of the nth element (shifting the              
	//nth element to the left by 1).                                                                                     
	//It also right shifts any remaining bits in the last element of the vector                                       
	//(we need to do this if we have used a 'false' parameter in the call to readSerialCommand
	//Also, the 'false' parameter has to be used in readSerialCommand if we are then going 
	//to use reOrganiseReadOnlyVector).
	//NOTE: If the length is >32 (and not equal to 32) then the other version of this sendReadOnlyCommand should be used,
	//which returns a vector<u32>.
	reOrganiseReadOnlyVector(readArgumentsVector, length);
	readArguments = readArgumentsVector[0]; //There should only be one vector element at this point, since the length==32.
	ICUTILS_VERIFYX(readArgumentsVector.size()==1, Fed9UVmeBaseException)(readArgumentsVector.size()).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("In sendReadOnlyCommand the readArgumentsVector.size() is not equal to 1. This method should only be called when the length<=32, and at this point in the method the vector should be of size 1.");
      }
      //Else for a <32-bit number 
      else {
	//Need to right shift here since we have used a 'false' parameter in the call to readSerialCommand above.
	readArgumentsVector[0] = readArgumentsVector[0] >> (32-((length+1)%32));
	readArguments = readArgumentsVector[0] & mask;  //Mask it so as to chop off the MSB (is it really neccessary?)
      }
      return readArguments;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::sendReadOnlyCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::sendReadOnlyCommand."));
    }   
  }

  //This method contains some common lines of code used in the read_only commands for the BE AND FE FPGAs.                                  
  //It returns the number read from the register.                                                                                      
  //NOTE:: This method is overloaded. There is another of the same same which returns an u32
  //in the case of length<=32. This method is meant to be used when the length>32.                                       
  vector<u32> Fed9UVmeBase::sendReadOnlyCommand(unsigned short selectedFpga,
							  u32 designator,
							  u32 length)   throw (Fed9UVmeBaseException)
  {
    try {
      //Check the length, to make sure that this method isn't being called when length<=32.
      ICUTILS_VERIFYX(length>32,Fed9UVmeBaseException)(length).code(Fed9UVmeBaseException::ERROR_FED9UVMEBASE).error().msg("The length is <=32 in the overloaded version of Fed9UVmeBase::sendReadOnlyCommand -  the one that expects a length>32.");
      const bool read = true;
      vector<u32> argumentsVector;
      vector<u32> command;
      
      //For these read only commands we have to add 2 to the length.                                                      
      theStringConstructor.constructCommand(selectedFpga, read, designator, length+2, argumentsVector, true, command);
      //Only add 1 to the length as we need to read back an extra bit.                                                         
      vector<u32> readArgumentsVector = theFed9UHalInterface.readSerialCommand(command[0], length+1, false);
      //If the length of the command is >= 32 then the data is in >1 vector elements.                                    
      //To reorganise the data so it is in the right order, we use the reOrganiseReadOnlyVector method.                    
      //It takes the MSB of the n+1 element and puts it as the LSB of the nth element (shifting the                       
      //nth element to the left by 1).                                                                                       
      //It also right shifts any remaining bits in the last element of the vector                                      
      //(we need to do this if we have used a 'false' parameter in the call to readSerialCommand.                   
      //Also, the 'false' parameter has to be used in readSerialCommand if we are then going                                    
      //to use reOrganiseReadOnlyVector).                                                                                       
      reOrganiseReadOnlyVector(readArgumentsVector, length);
      return readArgumentsVector;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::sendReadOnlyCommand."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::sendReadOnlyCommand."));
    }
  }


  /**Private method to reorganise data read during a read only command. This method is useful when the length>=32.
     The method modifies the input vector. It left shifts each word by 1 bit and moves the MSB that has been shifted out
     into the LSB of the lower word. This is done for all words. For the first word, it ignores the MSB that has been shifted out.
     NOTE: this method depends on the top word of the vector being on the left hand side (ie. left shifted).*/ 
  void Fed9UVmeBase::reOrganiseReadOnlyVector(vector<u32> &readArgumentsVector,
					      u32 length) throw (Fed9UVmeBaseException) {
    try {

      u32 holder = 0;
      /*     for (unsigned int i=1; i<(readArgumentsVector.size()-1); ++i) {
	     holder = (readArgumentsVector[i] >> 31) & 0x1;
	     readArgumentsVector[i-1] = (readArgumentsVector[i-1] << 1) & 0xFFFFFFFE;
	     readArgumentsVector[i-1] = readArgumentsVector[i-1] | holder;
	     }*/
      for (unsigned int i=0; i<(readArgumentsVector.size()-1); ++i) {
	readArgumentsVector[i] = (readArgumentsVector[i] << 1) & 0xFFFFFFFE;
	readArgumentsVector[i] = readArgumentsVector[i] | (readArgumentsVector[i+1] >> 31);
      }
      //Now right shift the last element of the vector                                                                                     
      readArgumentsVector[readArgumentsVector.size()-1] = readArgumentsVector[readArgumentsVector.size()-1] >> (32-((length+1)%32));
      //If the original length was a multiple of 32 then we will now have a redundant last element of the vector. 
      //We now pop this last element
      if ((length%32) == 0) {
	readArgumentsVector.pop_back();
      }

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "std::exception caught in Fed9UVmeBase::reOrganiseReadOnlyVector."));
    }
    catch (...) {
      THROW(Fed9UVmeBaseException(Fed9UVmeBaseException::ERROR_FED9UVMEBASE, "Unknown exception caught in Fed9UVmeBase::reOrganiseReadOnlyVector."));
    }
  }

}














