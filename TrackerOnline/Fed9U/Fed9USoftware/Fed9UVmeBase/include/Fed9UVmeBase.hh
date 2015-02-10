/**Matthew Pearson, RAL - May 2003*/

/** This Fed9UVmeBase class contains methods which enable FED configuration. 
    This class is low level and should not be used directly by a user. It contains 
    methods to mirror the firmware in the FED FPGA chips. So for every firmware command
    there is a corresponding method in this class.
    Every method requires an input telling it which FED FPGA to address (1-8 are the FE FPGAs,
    10 is the BE FPGA and 15 is for all the FE FPGAs). Most methods also take as input a 
    read/write flag and a vector holding the data to write.
    A reference to another vector is passed to each command method
    which will fill with data in the case of a FED read.
    Each method can throw an exception in the form of a Fed9UVmeBaseException object.

    For more detailed documentation see the FED Front End FPGA documents.*/ 

#ifndef _Fed9UVmeBase_H_
#define _Fed9UVmeBase_H_

#include"Fed9UHalInterface.hh"
#include"Fed9UConstructCommand.hh"
#include"Fed9UVmeBaseException.hh"

#include<string>
#include<vector>
//#include<sstream>
//#include <iosfwd>

namespace Fed9U {

  //using.*std::vector;
  //using.*std::string;

class Fed9UVmeBase
{
public:
  /** Fed9UVmeBase constructor. It takes as input the FED base address and the file name of the HAL address table.
      One can also optionally pass a crate number (which in effect selects the PCI-VME interface card in the PC). */
  Fed9UVmeBase(u32 baseAddress, std::string theAddressTable, Fed9UHalBusAdaptor busAdaptorType, unsigned short crateNumber);
  ~Fed9UVmeBase();
  /**FRONT-END FPGA COMMAND*/

  /**Set Opto reciever control bits. This method takes as input the fpga number, 
  a flag to read or write (true=read, false=write), a 1-element argument vector,
  and a empty readArguments vector which is filled up in the case of a read.*/
  void feCommandOptoControl(unsigned short selectedFpga, 
                            bool read, 
                            std::vector<u32> &arguments,
                            std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);
  
  /**Set FED operational mode (Processed raw data, zero suppression, virgin raw data, frame finding, scope mode.
  For a write, the 1-element argument vector contains 5 relavent bits. Only one bit
  of [4:2] should be set and only one bit of [1:0] should be set. In scope mode
  only virgin raw data mode is supported.
  Bit 4: Processed Raw Data Mode
  Bit 3: Zero Suppression Mode
  Bit 2: Virgin Raw Data Mode
  Bit 1: Frame Finding Mode
  Bit 0: Scope Mode.*/  
  void feCommandLoadModeReg(unsigned short selectedFpga, 
                            bool read, 
                            std::vector<u32> &arguments,
                            std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Set FED super mode (Fake Event mode, Zero Suppressed Lite or Normal)
  For a write, the 1-element argument vector contains 3 relavent bits. 
  Bit 2: Resserved
  Bit 1: Zero Suppressed Lite
  Bit 0: Fake Event*/  
  void feCommandLoadSuperModeReg(unsigned short selectedFpga, 
                            bool read, 
                            std::vector<u32> &arguments,
                            std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);


  /**Set the number of words of virgin raw data written into the output FIFOs after a scope mode event.
  The 1-element argument vector will be in the range 0 to 0x3ff (ie. 10 bits).*/
  void feCommandLoadScopeLength(unsigned short selectedFpga, 
                                bool read, 
                                std::vector<u32> &arguments,
                                std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Method to write or read the adc control bits. The most common operation will be
  a read. There will be 24-bits in the argument vector (see FE FPGA documentation).
  Each 4-bit section will set the control bits for an ADC pair (ie. for two channels).
  Bits [0:3] are for channels 2 and 3,
  Bits [4:7] are for channels 1 and 4,
  Bits [8:11] are for channels 6 and 7,
  Bits [12:15] are for channels 5 and 8,
  Bits [16:19] are for channels 10 and 11,
  Bits [20:23] are for channels 12 and 9.*/
  void feCommandAdcControl(unsigned short selectedFpga, 
                           bool read, 
                           std::vector<u32> &arguments,
                           std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Enable/Disable both or one of the APVs connected to each fiber.
  For an fpga there are 12 APV pairs. The 1-element argument vector contains 24 relavent bits,
  where each bit pair will enable both APVs (11), enable left one (10),
  enable right one (01) or disable both (00).*/
  void feCommandLoadTick(unsigned short selectedFpga, 
                         bool read, 
                         std::vector<u32> &arguments,
                         std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /*************************************************************************************************
   *Method to select the fake event address. This determins which location and which
   *RAM is written to when the loadFakeEventData command is called. The 1-element argument
   *vector has 16 relavent bits. Bits [0:8] select the Fake Event RAM address.
   *Bit [9] selects one of the channel pairs
   *Bit [10-12] are reserved and should be set to zero 
   *Bit [13-15] As in Fake Event Ram Number
   * <000> selects the RAMs connected to fibre 0 and 1
   * <101> selects the RAMs connected to fibre 10 and 11 
   ********/
  void feCommandSetFakeEventAddress(unsigned short selectedFpga, 
                                            bool read, 
                                            std::vector<u32> &arguments,
                                            std::vector<u32> &readArguments)  throw (Fed9UVmeBaseException) ;

  /*************************************************************************************************
   *Send or read data from the Fake Event RAM (defined by setFakeEventAddress). Each RAM contains the Fake Event
   *for a pair of fibers.
   */
  void feCommandLoadFakeEventData(unsigned short selectedFpga, 
				  bool read, 
				  std::vector<u32> &arguments, 
				  std::vector<u32> &readArguments)   throw (Fed9UVmeBaseException);
  
  /*************************************************************************************************
   *Send or read data from the Fake Event RAM (defined by setFakeEventAddress) using block mode transfer for speed enhancement. Each RAM contains the Fake Event
   *for a pair of fibers.
   */
  void feCommandBlockLoadFakeEventData(unsigned short selectedFpga, 
				       bool read, 
				       std::vector<u32> arguments, 
				       std::vector<u32> readArguments)   throw (Fed9UVmeBaseException);
  

  /**Method to select the pedestal address. This determins which location and which
     RAM is written to when the load_pedestal_data command is called. The 1-element argument
  vector has 12 relavent bits. Bits [11:9] select the pedestal RAM (000 selects RAM for
  fibres 0 and 1, 101 selects RAM for fibres 11 and 12, 111 selects all six RAMs).
  Bit [8] selects either the pedestal part of the RAM (0) or the cluster threshold
  part of the RAM (1). Bits [7:0] select the strip number.To set the pedestals/clusters for
  more than one strip, it is only nessesary to use this command once, and the address will 
  automatically increment (only the 8 LSB's will increment).*/
  void feCommandSetPedAddress(unsigned short selectedFpga, 
                              bool read, 
                              std::vector<u32> &arguments,
                              std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Send or read data from the pedestal RAM (defined by set_ped_address). Each RAM contains the pedestals
  and cluster thresholds for the four APVs connected to a pair of fibers. The data format changes
  depending on pedestal address. 
  In the following, the strip range is 0-255, ie. two APVs (which is one fiber).
  The command has the feature that one has to write to the same strip number for both fibers at 
  the same time (eg. strip 2 in APV0 in fiber 0, and strip 2 in APV0 in fiber 1).
  If setting the pedestal, the argument list is: Exclude strip from fiber 0 in CM calculation (0=yes, 1=no)
                                                 Pedestal value for fiber 0
                                                 Exclude strip from fiber 1 in CM calculation (0=yes, 1=no)
                                                 Pedestal value for fiber 1
  If setting cluster thresholds, the argument list is: 1st cluster threshold for fiber 0
                                                       2nd cluster threshold for fiber 0
                                                       1st cluster threshold for fiber 1
                                                       2nd cluster threshold for fiber 1
  setCluster=false means set pedestal
  setCluster=true  means set cluster thresholds*/
  void feCommandLoadPedData(unsigned short selectedFpga, 
                            bool read, 
                            const std::vector<u32> &arguments, 
                            std::vector<u32> &readArguments,
                            bool setCluster) throw (Fed9UVmeBaseException);

/**Send or read data from the pedestal RAM (defined by set_ped_address). Each RAM contains the pedestals
  and cluster thresholds for the four APVs connected to a pair of fibers. The data format changes
  depending on pedestal address. 
  In the following, the strip range is 0-255, ie. two APVs (which is one fiber).
  The command has the feature that one has to write to the same strip number for both fibers at 
  the same time (eg. strip 2 in APV0 in fiber 0, and strip 2 in APV0 in fiber 1).
  If setting the pedestal, the argument list is: Exclude strip from fiber 0 in CM calculation (0=yes, 1=no)
                                                 Pedestal value for fiber 0
                                                 Exclude strip from fiber 1 in CM calculation (0=yes, 1=no)
                                                 Pedestal value for fiber 1
  If setting cluster thresholds, the argument list is: 1st cluster threshold for fiber 0
                                                       2nd cluster threshold for fiber 0
                                                       1st cluster threshold for fiber 1
                                                       2nd cluster threshold for fiber 1
  setCluster=false means set pedestal
  setCluster=true  means set cluster thresholds*/
  void feCommandBlockLoadPedData(unsigned short selectedFpga, 
				 bool read, 
				 std::vector<u32> &arguments, 
				 std::vector<u32> &readArguments,
				 bool setCluster) throw (Fed9UVmeBaseException);



  /**Informs the median calculation which strips are valid for each APV.
  The number specified by this command should be equal to the number of valid
  strips specified by using the feCommandLoadPedData method.
  Data should be input in the following format:
  1st element of arguments will contain ffff, where the left most ff is for
  the APV0 for fiber 1, and the right most ff is for APV1 for fiber 1.
  The 2nd element of arguments is the same for fiber 2, etc.
  Therefore, there are 12 elements in the input vector.*/
  void feCommandSetNumberValid(unsigned short selectedFpga, 
                               bool read, 
			       const std::vector<u32> &arguments,
                               std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Method to load the threshold values for the ticker blocks (ie. threshold for frame finding).
     When writing the method expects an 12-element arguments vector, each element containing 5 relavent bits.
     The first element will contain the threshold for fibre 1, the 12th element will contain
     the threshold for fiber 12.*/
  void feCommandLoadThresh(unsigned short selectedFpga, 
                           bool read, 
                           const std::vector<u32> &arguments,
                           std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Method to load the median overide values for the common mode for each APV. When writing the method expects
     an argument vector with 24 elements which each contain 10 relavent bits. The first elements will
     contain the median for APV0 for fibre1, the 2nd element will contain the median for APV1 for fibre1, etc.*/
  void feCommandLoadMedian(unsigned short selectedFpga, 
                           bool read, 
                           const std::vector<u32> &arguments,
                           std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Manually overide the automatically calculated common mode. The argument vector will be 1-element
     which contains a 1 to enable or a 0 to disable the overide.*/
  void feCommandMedianEnable(unsigned short selectedFpga, 
                             bool read,
                             std::vector<u32> &arguments,
                             std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**Perform a soft reset on the specified FPGA*/
  void feCommandSoftReset(unsigned short selectedFpga) throw (Fed9UVmeBaseException);

  /**Loads the delay for the 4 fibers connected to one of the delay FPGAs. This method takes the
  4 delay values in the first 4 elements of the input vector. The 5th element contains the
  delay FPGA chip number. The chipNumber = 0-2, or 3 for all chips. Earlier versions of the
  delay chip firmware used a 5 bit fine skew and as such a switch has been applied to 
  allow for this backward compatibility. Passing true for fineSkew6bit uses a 6 bit fine
  skew value and false a 5 bit. No check if made to ensure the appropiate firmware version
  is done at this level.
  NOTE: This is a write only command.*/
  void feCommandLoadSkew(unsigned short selectedFpga,  
                         std::vector<u32> &arguments,
			 bool fineSkew6bit) throw (Fed9UVmeBaseException);

  /**Method to set the period for 1/N arming of the spy channel. A command that instructs one or all of the delay
     chip to collect the next 1022 words of data on the requested fibre is sent every N event
     NOTE: This is a write only command.*/
  void feCommandSpyPeriod(unsigned short selectedFpga,
                                             bool read,
                                             std::vector<u32> &arguments,
                                             std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);



  /**Method to arm the spy channel. A command that instructs one or all of the delay
     chip to collect the next 1022 words of data on the requested fibre. It requires a FE FPGA and
     chip number to specify which chip to record on. The chipNumber = 0-2, or 3 for all chips.
     NOTE: This is a write only command.*/
  void feCommandArmSpyChannel(unsigned short selectedFpga,
				       u32 selectedDelayChip) throw (Fed9UVmeBaseException);


  /**Method to read the 1022 words of data read back from the fibres on the chosen delay chip.
     The chipNumber = 0-2.
     NOTE: This a read only command and it is not possible to read back all chips simulataneously.*/
  std::vector<u32> feCommandFireSpyChannel(unsigned short selectedFpga,
						u32 selectedDelayChip) throw (Fed9UVmeBaseException);


  /**Method to read the firmware version that is currently in use by the CMS delay chips (CDCs).
     Takes the CDC chip number on the FE FPGA that is to be addressed. Valid chip numbers are 0-2.
     NOTE: This is a read only command.*/
  u32 feCommandDelayChipFirmwareId(unsigned short selectedFpga,  
                                          u32 cdcNumber)  throw (Fed9UVmeBaseException);

  /**This command controls the LM82 temperature sensor from the Front End chips. 
     The FE FPGA communicates 
     with the sensor via an I2C protocol built into the firmware.
     The arguments vector has two elements. The first element 
     contains 8 relavent bits, which is the data sent to the LM82 in the case 
     of a write. The 2nd element contains the 8-bit LM82 register number.
     The i2cBusy and i2cError booleans will return a false if
     the reading of the LM82 succeded (in the case of a write they
     will both always return a false).
     Note:
     arguments[0] = data 8-bits
     arguments[1] = register 8-bits. */
  void feCommandLm82Control(unsigned short selectedFpga,
                            bool read, 
                            const std::vector<u32> &arguments,
                            u32 &readArguments,
                            bool &i2cBusy,
                            bool &i2cError) throw (Fed9UVmeBaseException);

  /**This command controls the 12 Channel, 8-bit TrimDACs (the AD8802). The DAC is used to
     convert digital configuration signals that can be used to control the
     gain of the ADCs at the Front-End. The FE FPGA communicates 
     with the DAC via a serial protocol (see AD8802 datasheet) built into
     the firmware. The arguments vector has two elements. The first element 
     contains 8 relavent bits, which is the data sent to the TrimDAC in the case 
     of a write. The 2nd element contains the 4-bit TrimDAC register number.
     By passing a 1 for the reset boolean will reset the chip,
     passing a 1 for the shutdown boolean will shutdown the chip.
     This is WRITE ONLY command.
     Note:
     arguments[0] = data 8-bits
     arguments[1] = register 4-bits.*/
  void feCommandTrimDacControl(unsigned short selectedFpga,
                               std::vector<u32> &arguments,
                               bool reset,
                               bool shutdown) throw (Fed9UVmeBaseException);

  /**This command complements the 10-bit data word which enters the 
     FE FPGA. It turns all the 1's to 0's and visa versa. It can be
     used if the inverter stage of the APVs is turned off. The command only needs 
     the FPGA number, a read/write flag and a unsinged long (where the 12 LSBs are used
     to set the complement, 0=off 1=on).*/
  void feCommandLoadComplement(unsigned short selectedFpga,
                               bool read,
                               u32 arguments,
                               u32 &readArguments) throw (Fed9UVmeBaseException);
  
  /**This method will cause the specified fpga to output on command_out a serial packet showing the 
  current occupancy of the FIFOs. THIS COMMAND IS NOT IMPLEMENTED YET!!!!*/
  void feCommandMonitor(unsigned short selectedFpga, 
                        std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);


  /**This method is a read only command. It reads the firmware ID register.*/
  void feCommandFirmwareIdReg(unsigned short selectedFpga, 
                              u32 &readArguments) throw (Fed9UVmeBaseException);

  // JEC 30-05-06
  /**This is a read only command for accessing the register which contains the number of triggers in the Fake Event FIFO.*/
  void feCommandFakeEventTriggerFifo(unsigned short selectedFpga, 
				     u32 &readArguments)   throw (Fed9UVmeBaseException);

  //<JEC date=16/03/07> 
  /** This command applies a prescale to the triggers coming in to the FE-FPGAs.  The default value is zero, which means that
   all triggers are received and stored.  As an example, a value of 4 would mean that every 5th trigger will be received and stored.
   This register is intended for internal diagnostic testing.  IT MUST BE USED IN CONJUNCTION WITH THE CORRESPONDING BE-FPGA COMMAND !*/
  void feCommandFrameCapturePrescale(unsigned short selectedFpga, 
				     bool read, 
				     std::vector<u32> &arguments,
				     std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);
  //</JEC>
  
  /**BACK-END FPGA COMMANDS. These commands do not need to take in an fpga number,
  as this is fixed. None of the read/write commands take in vectors as arguments,
  as there is no length>32 bits.*/

  /**This command selects the trigger source for the back-end FPGA. The arguments input
     will have 3 relavent bits, which mean:
     001=TTC Trigger (this is normal running mode)
     010=Backplane trigger
     100=Software tigger.
     Only one of these bits should be set at any one time.*/ 
  void beCommandTriggerSelect(bool read, 
                              u32 arguments,
                              u32 &readArguments) throw (Fed9UVmeBaseException);
  
  /**This command sets the operating mode of the BE FPGA. It can be either in
     scope mode, or frame finding mode (not both at the same time). The arguments
     input is 1 or 2, where:
     1=Scope Mode
     2=Frame Finding Mode.*/
  void beCommandModeRegister(bool read, 
                             u32 arguments,
                             u32 &readArguments) throw (Fed9UVmeBaseException);
   
  /**This is a I2C command to the TTCrx chip. The I2C protocol is inside the
     firmware. The arguments vector has two elements. The first element 
     contains 8 relavent bits, which is the data sent to the TTCrx in the case 
     of a write. The 2nd element contains the 5-bit TTCrx register number.
     The i2cBusy and i2cError booleans will return a false if
     the reading of the TTCrx succeded (in the case of a write they
     will both always return a false).
     Note:
     arguments[0] = data 8-bits
     arguments[1] = register 5-bits.*/
  void beCommandTtcrxControl(bool read, 
                             const std::vector<u32> &arguments,
                             u32 &readArguments,
                             bool &i2cBusy,
                             bool &i2cError) throw (Fed9UVmeBaseException);

  /**This method sends a reset signal to the TTCrx. This is an internal
     reset. An alternative reset can be performed by the beCommandTtcrxExternalReset.
     This internal reset command sends a I2C reset command to the TTCrx.*/
  void beCommandTtcrxInternalReset(void) throw (Fed9UVmeBaseException);

  /**This method sends a reset signal to the TTCrx. This is an external
     reset. An alternative reset can be performed by the beCommandTtcrxInternalReset.
     This external reset command drives the reset pin of the TTCrx chip low.*/
  void beCommandTtcrxExternalReset(void) throw (Fed9UVmeBaseException);
  
  /**This command writes or reads to the test register. The input
     argument is 32-bits long, where each bit represents the status of
     a particular register. This command is only used for testing.*/
  void beCommandTestReg(bool read, 
                        u32 arguments,
                        u32 &readArguments) throw (Fed9UVmeBaseException);
   
  /**This command is used to enable or disable a front end input to the
     back-end chip. The arguments input has 8 relavent bits, where 
     each bit can be set to 0 or 1 to disable or enable an FPGA (0=disable, 1=enable).
     The least significant bit is for FPGA 1, the most significant bit
     is for FPGA 8.*/
  void beCommandFeEnableReg(bool read, 
                            u32 arguments,
                            u32 &readArguments) throw (Fed9UVmeBaseException);
  
  /**This command selects the readout from the QDR memory chips. The arguments
     input can be either 1 or 2, where:
     1=S-LINK Readout
     2=VME BUS Readout.
     Only one of these can be set at the same time.*/
  void beCommandBeReadoutControl(bool read, 
                                 u32 arguments,
                                 u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command can start or stop a FED run. The arguments input is two bits. 
  The least significant bit controls the trigger:
  0=ignore trigger
  1=accept trigger
  The most significant bit controls the acceptance of frames and headers:
  0=ignore frames and headers
  1=accept frames and headers
  One should only set either 11 or 00.*/
  void beCommandBeRunControl(bool read, 
                             u32 arguments,
                             u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command generates a software trigger to the FE FPGAs.*/
  void beCommandSoftwareTrigger(void) throw (Fed9UVmeBaseException);

  /**This command applies a soft reset to the BE FPGA. This only resets the counters. It
     does not affect the DCMs and control registers.*/
  void beCommandSoftReset(void) throw (Fed9UVmeBaseException);

  /**This command can set or read the FED ID register. It is
     a 12-bit number.*/
  void beCommandFedIdReg(bool read, 
                         u32 arguments,
                         u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command controls the LM82 temperature sensor. The BE FPGA communicates 
     with the sensor via an I2C protocol built into the firmware.
     The arguments vector has two elements. The first element 
     contains 8 relavent bits, which is the data sent to the LM82 in the case 
     of a write. The 2nd element contains the 8-bit LM82 register number.
     The i2cBusy and i2cError booleans will return a false if
     the reading of the LM82 succeded (in the case of a write they
     will both always return a false).
     Note:
     arguments[0] = data 8-bits
     arguments[1] = register 8-bits.*/
  void beCommandLm82Control(bool read, 
                            const std::vector<u32> &arguments,
                            u32 &readArguments,
                            bool &i2cBusy,
                            bool &i2cError) throw (Fed9UVmeBaseException);

  /**These following BE commands are all read only. They return an integer by reference.*/
  
  /**This command reads the BE FPGA debug register.*/
  void beCommandDebugRegister(u32 &readArguments) throw (Fed9UVmeBaseException);
    
  /**This command reads the BE FPGA status register.*/
  void beCommandExtraEventRegister(u32 &readArguments) throw (Fed9UVmeBaseException);
    
  /**This command reads the BE FPGA status register.*/
  void beCommandBeStatusRegister(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the Level 1 accept counter.*/
  void beCommandReadL1aCount(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the bunch crossing counter.*/
  void beCommandReadBxCount(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the number of frames stored in the QDRs.*/
  void beCommandQdrFrameCounter(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the firmware ID register.*/
  void beCommandFirmwareIdReg(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the number of unread words stored in the QDR.*/
  void beCommandQdrDataCounter(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the total number of frames written to the QDR.*/
  void beCommandTotalFrameCount(u32 &readArguments) throw (Fed9UVmeBaseException);

  //New BE commands April 2005.

  /**This command can read or write to the DAQ register in the BE FPGA. This is a 32-bit word that
     is also written into the FED buffer header when running in full debug mode. When writing one
     should set the arguments input to the value one wants to write. When reading the readArguments input
     will be set equal to the value read from the register.*/
  void beCommandDaqRegister(bool read,
			    u32 arguments,
			    u32 &readArguments) throw (Fed9UVmeBaseException);
			    
  //<JRF data="18/08/2006">
  /**This command can read or write to the DAQ register two in the BE FPGA. This is a 32-bit word that
     is also written into the FED buffer header when running in full debug mode. When writing one
     should set the arguments input to the value one wants to write. When reading the readArguments input
     will be set equal to the value read from the register.*/
  void beCommandDaqRegisterTwo(bool read,
			    u32 arguments,
			    u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command can read or write the tracker header format type flag. This determins the header format                
     of the FED data buffer.                                                                                                               
     1=Full debug mode                                                                                                                     
     2=APV error mode                                                                                                                  
     This flag is also written in the tracker special header word in every FED event buffer.                                                    
     The length of the arguments is 4-bits, although currently (as of April 2005) only the first                                          
     two bits are relevant.*/
  void beCommandHeaderFormatType(bool read,
				 u32 arguments,
				 u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command can read or write the bunch crossing offset number. This offset is applied to the Bx counter                           
     at the start of each LHC orbit and is also applied to the Bx number witten to the FED data buffer header.                              
     The offset is a 12-bit number. An input of arguments=1 will provide an offset of 1 to the Bx number, an input                              
     of 0 will provide no offset.*/
  void beCommandBunchCrossingOffset(bool read,
				    u32 arguments,
				    u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the TTC individually addressed command sent to the FED over TTC channel B.                         
     There are 39 bits in total (see TTC documentation for detailed information).                                                   
     Bits 6-0 = Check word                                                                                                       
     Bits 14-7 = Data word                                                                                                        
     Bits 22-15 = Sub address word                                                                                                 
     Bit 23 = set to 1                                                                                                          
     Bit 24 = 0 or 1 (0=internal, 1=external)                                                                               
     Bits 38-25 = TTCrx address word.*/
  void beCommandTtcIndividuallyAddressedCommand(std::vector<u32> &readArguments)  throw (Fed9UVmeBaseException);

  /**This command reads the TTC broadcast command sent to the FED over TTC channel B. There are 13 bits
   in the command. 
   Bits 4-0 = Check word
   Bits 12-5 = command / data.*/
  void beCommandTtcBroadcastCommand(u32 &readArguments) throw (Fed9UVmeBaseException);

   /**This command reads or writes the DAQ Event type register. This number is placed in the DAQ header 
     in every FED data buffer. It is 4 bits.*/
  void beCommandDaqEventType(bool read,
			     u32 arguments,
			     u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads or writes the DAQ FOV register. This number is placed in the DAQ header in
     every FED data buffer. It is 4 bits.*/
  void beCommandDaqFov(bool read,
		       u32 arguments,
		       u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the bunch crossing total - the value of the Bx counter expected on recipt of a new orbit signal.
     It is a 12-bit number. It should be the same at the end of each orbit. It depends on the value of the Bx offset set.*/
  void beCommandBunchCrossingOrbitTotal(u32 &readArguments)  throw (Fed9UVmeBaseException);
  void beCommandBunchCrossingOrbitTotal(bool read,
					u32 arguments,
					u32 &readArguments) throw (Fed9UVmeBaseException);

  //<JEC date=16/03/07> 
  /** This command applies a prescale to the triggers coming in to the BE-FPGA.  The default value is zero, which means that
      all triggers are received and stored.  As an example, a value of 4 would mean that every 5th trigger will be received and stored.
      This register is intended for internal diagnostic testing.  IT MUST BE USED IN CONJUNCTION WITH THE CORRESPONDING FE-FPGA COMMAND !*/
  void beCommandFrameCapturePrescale(bool read,
				     u32 arguments,
				     u32 &readArguments) throw (Fed9UVmeBaseException);
  //</JEC>

  // JEC 8/3/06
  void beCommandFmmTestReg(bool read, 
  			   u32 arguments,
  			   u32 &readArguments)   throw (Fed9UVmeBaseException);

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
  void beCommandTrackerHeaderMonitor(std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);

  /**This method read the long_read_1 register. This register also holds:
     bit 15 = TTS Read                                                                                                                      
     bit 14 = TTS Warn Overflow                                                                                                             
     bit 13 = TTS Busy                                                                                                                      
     bit 12 = TTS Out of Sync                                                                                                               
     bits 11-0 = Bx counter value at orbit.
     The length was originally 64 bits, but was shortened to 32 bits so that it could be stored in the event record.
  */
  void beCommandLongReadRegister(std::vector<u32> &readArguments) throw (Fed9UVmeBaseException);
  void beCommandLongReadRegister(u32 &readArguments) throw (Fed9UVmeBaseException);
    
 //<JRF data="06/06/2008">
  /**This command can read or write to the Spy Arm TTC B Data register in the BE FPGA. This is a 16-bit word that
     contains the B channel command data which is required to be sent in order to arm the spy channel, note this value
     should be set carefully and assigned by the system, so that other B channel commands do not arm the spy channel.*/
  void beCommandSpyArmTtcBData(bool read,
			    u32 arguments,
			    u32 &readArguments) throw (Fed9UVmeBaseException);

  // <NAC date="25/06/2009"> Added second spy L1A/QDR register commands
  /**This command reads the last total event count latched when a TTC b channel spy arm command was received by the fed.
     This is a BE FPGA register. This is a 24-bit word that
  */
  void beCommandSpyTotalEventCount( u32 &readArguments, u32 length) throw (Fed9UVmeBaseException);

  /**This command reads the last L1A count latched when a TTC b channel spy arm command was received by the fed.
     This is a BE FPGA register. This is a 24-bit word that
  */
  void beCommandSpyL1ACount( u32 &readArguments, u32 length) throw (Fed9UVmeBaseException);

  // </NAC>

  // <JRF date="25/07/2012"> Added second 32 bit trigger and event counter
    /**
         This is a BE FPGA command to read a 32 bit counter for the total event ocunt which is not reset on receipt of an ECO 
           */
  void beCommandTotalEventCount32( u32 &readArguments) throw (Fed9UVmeBaseException);
  /**
        This is a BE FPGA command to read a 32 bit counter for the total event ocunt which is not reset on receipt of an ECO 
  */                     
  void beCommandTotalL1ACount32( u32 &readArguments) throw (Fed9UVmeBaseException);

  void beCommandTotalEventOutCount( u32 &readArguments) throw (Fed9UVmeBaseException);

  void beCommandFeFrameLengthCount( u32 &readArguments) throw (Fed9UVmeBaseException);
 
  void beCommandDummyEventCount( u32 &readArguments) throw (Fed9UVmeBaseException);

  void beCommandPayloadDownCount( u32 &readArguments) throw (Fed9UVmeBaseException);

  void beCommandPayloadLength( u32 &readArguments) throw (Fed9UVmeBaseException);

  void beCommandDebugRegTwo(std::vector<u32> &readArguments)  throw (Fed9UVmeBaseException);


// </JRF>


  /**VME-FPGA Commands. These do not need to specify an fpga number, as the
     VME commands are in the form of direct writes to command specific areas
     of VME memory. See the HAL address table for more information.*/

  /**This command will read the the FED board specifier, should be 0x00000FED.*/
  void vmeCommandReadFedBoardSpecifier(u32& readArguments)   throw (Fed9UVmeBaseException);
  
  /**This command will reset the FED.*/
  void vmeCommandSoftwareReset(bool doWait=true) throw (Fed9UVmeBaseException);
  
  /**This command will reset the FED VME Vlink counters only and will not perform a full logic reset on the fed.*/
  void vmeCommandVLinkReset(void)   throw (Fed9UVmeBaseException);

  /**This command resets the VME BUS.*/
  void vmeCommandResetBus() throw (Fed9UVmeBaseException);
  
  /**This command inhibits the VME BUS reset.*/
  void vmeCommandResetBusInhibit(bool value) throw (Fed9UVmeBaseException);

  /**This command resets the readout control status.*/
  void vmeCommandResetControlStatus(void) throw (Fed9UVmeBaseException);

  /**This command checks for an event. Returns 1 if an event is present.*/
  void vmeCommandReadoutBufferStatus(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the event counter.*/
  void vmeCommandReadEventCounter(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command reads the contents of the buffer block in the RAM.*/
  void vmeCommandReadBufferBlock(u32 *readArguments, u32 offset) throw (Fed9UVmeBaseException);

  /**This command performs a block read of the contents of the buffer block in the RAM.*/
  void vmeCommandBlockReadBufferBlock(u32 length, char * const readArguments, u32 offset) throw (Fed9UVmeBaseException);

  /**This command reads the length of the events in the buffer.*/
  void vmeCommandReadBufferLength(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command selects the clock. A 1 will select the on-board crystal, 2 the
     TTC clock and 4 the VME backplane clock. If the clock is successfully changed
     then a fed reset is performed.*/
  void vmeCommandSelectClock(bool read, 
                             u32 arguments,
                             u32 &readArguments,
			     bool doWait=true) throw (Fed9UVmeBaseException);

  /**This command will read the VME status.*/
  void vmeCommandStatus(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command will read the TTC clock counter.*/
  void vmeCommandTtcClockCounter(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This command will read the backplane clock counter.*/
  void vmeCommandBackplaneClockCounter(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This method will read the VME firmware ID register.*/
  void vmeCommandFirmwareID(u32 &readArguments) throw (Fed9UVmeBaseException);

  /**This method can read the STATUS_SERIAL register.*/
  void vmeCommandStatusSerial(u32 &readArguments) throw (Fed9UVmeBaseException); 

  /**This command controls the EPROM via the VME FPGA. The VME FPGA communicates with the EPROM via an 
     I2C protocol built into the firmware. The arguments vector has two elements. The first element 
     contains 8 relavent data bits, which is the data sent to the EPROM in the case of a write. The 2nd element 
     contains the 11-bit EPROM register number. If one is reading then only the register number is important.
     The i2cBusy and i2cError booleans will return a false if the reading of the EPROM succeded 
     (in the case of a write they will both always return a false).
     Note: arguments[0] = data 8-bits
           arguments[1] = register 11-bits*/
  void vmeCommandEpromControl(bool read, 
                              const std::vector<u32> &arguments, 
                              u32 &readArguments,
                              bool &i2cBusy,
                              bool &i2cError) throw (Fed9UVmeBaseException);

  /**This command controls the LM82 via the VME FPGA. The VME FPGA communicates with the sensor via an 
     I2C protocol built into the firmware. The arguments vector has two elements. The first element 
     contains 8 relavent data bits, which is the data sent to the LM82 in the case of a write. The 2nd element 
     contains the 8-bit LM82 register number. If one is reading then only the register number is important.
     The i2cBusy and i2cError booleans will return a false if the reading of the LM82 succeded 
     (in the case of a write they will both always return a false).
     Note: arguments[0] = data 8-bits
           arguments[1] = register 8-bits*/
  void vmeCommandLm82Control(bool read,
                             const std::vector<u32> &arguments, 
                             u32 &readArguments,
                             bool &i2cBusy,
                             bool &i2cError) throw (Fed9UVmeBaseException);

  /**This command controls the voltage monitor via the VME FPGA. The VME FPGA communicates with the sensor via an 
     I2C protocol built into the firmware. The arguments vector has two elements. The first element 
     contains 8 relavent data bits, which is the data sent to the voltage monitor in the case of a write. The 2nd element 
     contains the 8-bit volatage monitor register number. If one is reading then only the register number is important.
     The i2cBusy and i2cError booleans will return a false if the reading of the voltage monitor succeded 
     (in the case of a write they will both always return a false).
     Note: arguments[0] = data 8-bits
           arguments[1] = register 8-bits*/
  void vmeCommandVoltageMonitorControl(bool read,
                                       const std::vector<u32> &arguments, 
                                       u32 &readArguments,
                                       bool &i2cBusy,
                                       bool &i2cError) throw (Fed9UVmeBaseException);

  /**Enables one to write to the System ACE data buffers.
     The offset is the offset (in bytes) from the SYS_ACE_DATA register (see HAL address table). 
     The default offset is zero. The memory space used is 0x900 to 0x97f inclusive. Memory is a FIFO.*/
  void vmeCommandSysAceDataWrite(u32 arguments, u32 offset = 0) throw (Fed9UVmeBaseException);
  
  /**Enables one to read from the System ACE data buffers, one byte at a time. 
     The offset is the offset (in bytes) from the SYS_ACE_DATA register (see HAL address table). 
     The default offset is zero. The memory space used is 0x900 to 0x97f inclusive. Memory is a FIFO.*/
  u32 vmeCommandSysAceDataRead(u32 offset = 0) throw (Fed9UVmeBaseException);

  /**Enables one to write to the System ACE control registers.
     The control registers occupy 0x880 to 0x8ff inclusive. The default offset is zero.*/
  void vmeCommandSysAceControlWrite(u32 arguments, u32 offset = 0) throw (Fed9UVmeBaseException);

  /**Enables one to read from the System ACE control registers.
     The control registers occupy 0x880 to 0x8ff inclusive. The default offset is zero.*/
  u32 vmeCommandSysAceControlRead(u32 offset = 0) throw (Fed9UVmeBaseException);

private:
  Fed9UHalInterface theFed9UHalInterface;
  Fed9UConstructCommand theStringConstructor;
  u8 * commandBuffer;

  /**Method to pack the ped_data (in the load_ped_data command) into the correct FED writable format.*/
  void packPedData(const std::vector<u32> &arguments, 
                   std::vector<u32> &newArguments, 
                   bool setCluster) throw (Fed9UVmeBaseException);

  /**Method to unpack the ped_data (in the load_ped_data command) from the correct FED writable format into 
  the format in which the data is passed into load_ped_data for a write.*/
  void unpackPedData(const std::vector<u32> &readArgumentsTemp, 
                     std::vector<u32> &readArguments, 
		     bool setCluster) throw (Fed9UVmeBaseException);

  /**Method that is used for the FE FPGA commands only. It checks that the FPGA number
     which is passed to the function is valid for a FE FPGA command.*/
  void checkFpgaNumber(unsigned short selectedFpga) throw (Fed9UVmeBaseException);

  /**Method to check the number and range of each input argument vector. This method takes
  as input the number of expected element in the argument vector and the expected maximum value.
  An error is thrown if either of these are exceeded. This method only applies when writing to the FED.
  The selectedFpga and fe are passed to this method to check that the fpga number is consistent
  with a front-end FPGA command.*/
  void checkArguments(const std::vector<u32> &arguments,
                      unsigned short noOfArguments, 
                      u32 maxArg, 
                      bool read) throw (Fed9UVmeBaseException);

  /**This method contains some common lines of code used in the read/write BE commands.
     It returns a number which can be ignored in the case of a write.*/
  u32 sendBEReadWriteCommand(u32 designator,
                                       u32 length,
                                       bool read,
                                       u32 arguments,
                                       unsigned short noOfArgElements,
                                       u32 maxArg) throw (Fed9UVmeBaseException);

  // JEC 6/3/06
  // overloaded version of the previous method - allows the FPGA number to be passed in
  // as an argument
  u32 sendBEReadWriteCommand(unsigned short selectedFpga,
				       u32 designator,
				       u32 length,
				       bool read,
				       u32 arguments,
				       unsigned short noOfArgElements,
				       u32 maxArg)   throw (Fed9UVmeBaseException);

  /**This method contains some common lines of code used in the read_only commands for the BE AND FE FPGAs.
  It returns the number read from the register. If the number being read is <32-bits one has to pass a mask
  equal to the maximum value that number can have. If the length >= 32 then one can pass a dummy mask.
  NOTE:: This method is overloaded. There is another of the same same which returns a vector in the case of length>32.*/
  u32 sendReadOnlyCommand(unsigned short selectedFpga,
                                      u32 designator,
                                      u32 length,
                                      u32 mask) throw (Fed9UVmeBaseException);

  /**This method contains some common lines of code used in the read_only commands for the BE AND FE FPGAs.                             
  It returns the number read from the register.                                                                                        
  NOTE:: This method is overloaded. There is another of the same same which returns an u32                                        
  in the case of length<=32. This method is meant to be used when the length>32.*/
  std::vector<u32> sendReadOnlyCommand(unsigned short selectedFpga,
					    u32 designator,
					    u32 length)   throw (Fed9UVmeBaseException);


  /**This is a function which reads the TTCrx status. It is a read-only BE FPGA command.
     This command is used inside the beCommandTtcrxControl command. 
     Two booleans are passed to check for a busy or an error in the I2C system.*/
  void beCommandTtcrxStatus(u32 &readArguments, 
                            bool &i2cBusy, 
                            bool &i2cError) throw (Fed9UVmeBaseException);

  /**This is a function which reads the LM82 status from the BE. It is a read-only BE FPGA command.
     This command is used inside the beCommandLm82Control command.
     Two booleans are passed to check for a busy or an error in the I2C system.*/
  void beCommandLm82Status(u32 &readArguments,
                           bool &i2cBusy, 
                           bool &i2cError) throw (Fed9UVmeBaseException);

  /**This is a function which reads the LM82 status from the FE. It is a read-only FE FPGA command.
     This command is used inside the beCommandLm82Control command.
     Two booleans are passed to check for a busy or an error in the I2C system.*/
  void feCommandLm82Status(unsigned short selectedFpga,
                           u32 &readArguments,
                           bool &i2cBusy, 
                           bool &i2cError) throw (Fed9UVmeBaseException);

  /**Private method to reorganise data read during a read only command. This method is useful when the length>32.                
     The method modifies the input vector. It left shifts each word by 1 bit and moves the MSB that has been shifted out                    
     into the LSB of the lower word. This is done for all words. For the first word, it ignores the MSB that has been shifted out.  
     NOTE: this method depends on the top word of the vector being on the left hand side (ie. left shifted).*/
  void reOrganiseReadOnlyVector(std::vector<u32> &readArgumentsVector,
				u32 length) throw (Fed9UVmeBaseException);

};

}

#endif
