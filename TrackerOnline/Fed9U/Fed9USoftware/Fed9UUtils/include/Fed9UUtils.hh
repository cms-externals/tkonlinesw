// #####################################################################################
// # Copyright (c) 2004 Imperial College London, Rutherford Appleton Laboratory        #
// # (Jonathan Fulcher, Emlyn Corrin, Matthew Pearson, Gareth Rogers, Matt Noy)        #
// #                                                                                   #
// # Permission is hereby granted, free of charge, to any person obtaining a copy      #
// # of this software and associated documentation files (the "Software"), to use      #
// # and modify for personal use only. No rights are given to deal in the Software,    #
// # publish, distribute, sublicense, and/or sell copies of the Software.              #
// # Support will be available from the authors to anyone using unmodified versions    #
// # of the Software. If the user modifies any part of the Software, no liability      #
// # is accepted on the part of the author and no support will be hitherto available.  #
// #                                                                                   #
// # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
// # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
// # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
// # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, #
// # WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN   #
// # CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
// #####################################################################################
#ifndef H_TypeDefs
#define H_TypeDefs

#define FED9U_FUNCTION __PRETTY_FUNCTION__ //!< Used in debug to give the name of the function called.
#define FED9U_DEBUG_BREAK __asm("int3")    //!< Used to force the execution into the debugger and to break.

#ifdef DAQ_VERSION_2
#define FED9U_INFO(x)     LOG4CPLUS_INFO(logger_, (x) )    //!< Provided by the DAQ framework to log run information, only provided by DAQ version 2.
#define FED9U_ERROR(x,y)  LOG4CPLUS_ERROR(logger_,(x + y)) //!< Provided by the DAQ framework to log error information, only provided by DAQ version 2.
#else
#define FED9U_INFO(x)  XDAQ_NOTE((x)) //!< Provided by older versions of the DAQ framework for information logging.
#define FED9U_ERROR(x,y) ims_->createErrorMessage(TFP_NONEXT_ERROR,                             \
						  time(0),                                      \
						  SOFTWAREERRORCODE,                            \
						  FEDSUPERVISORSYSTEM,                          \
						  TRACKERFEDPMCSYSTEM,                          \
						  x,                                            \
						  y);                                           \
                         ims_->emit();                                                          \
                         XDAQ_RAISE(xdaqException,y) //!< Provided by older versions of the DAQ framework for error logging.
#endif

#include <iosfwd>
#include <iostream>
#include <string>



#define FED9U_EPROM_VERSION_1 1 //!< The first version of EPROM data had only the fed serial number at offset 516.

#define FED9U_EPROM_VERSION_2 2 //!< The second version of eprom data has test results and serial number at offset 1536.
                                //!< This can be used to offset into the EPROM to read that data.
#define FED9U_EPROM_INDUSTRY_RESULTS_OFFSET 1536//!< This can be used to offset into the EPROM to read the industry results from the EPROM
                                                //!< saved at this location in EPROM versions 2 only.
#define FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET 516 //!< This can be used to offset into the EPROM to read the serial number in version 1 EPROMS only.


#define FED9U_VME_SERIAL_COMMAND_BUFFER_SIZE 511 //!< This is the size of the VME serial command buffer in number of 32 bit words.

/**
 * \name Compatible firmware versions.
 *
 * The Fed9U software project is only compatible with firmware versions equal to or above the following.
 */
//@{
#define FED9U_FE_FIRMWARE_MIN_VER 0X10000000    //!< Front end firmware minimum version.
#define FED9U_BE_FIRMWARE_MIN_VER 0X10000000    //!< Backend (BE) firmware minimum version.
#define FED9U_VME_FIRMWARE_MIN_VER 0X10000000   //!< VME firmware minimum version.
#define FED9U_DELAY_FIRMWARE_MIN_VER 0X10000000 //!< CMS delay chip (CDC or delay FPGA) firmware minimum version.
//@}

#define FED9U_DELAY_FIRMWARE_VERSION_6_BIT 0x24000224 //!< The 6 bit fine delay skew setting is only compatable with firmware above this version.

/**
 * \brief The Fed9U namespace encapsulates the whole of the Fed9U software libraries.
 */
namespace Fed9U {

  /**
   * \brief Returns the version of the Fed9UUtils library.
   * \return string Contains the version number in the form major.minor.
   */
  std::string getFed9UUtilsVersion();

  /**
   * \brief  Returns the major Fed9UUtils library versiion.
   * \return unsigned The major version.
   */
  unsigned    getFed9UUtilsVersionMajor();

  /**
   * \brief  Returns the minor Fed9UUtils library versiion.
   * \return unsigned The minor version.
   */
  unsigned    getFed9UUtilsVersionMinor();
  
  /**
   * \brief A list of enumerations to define so numbers that are commonly used in a FED.
   */
  enum {
    FED_STRIP_HIT_THRESHOLD_HIGH = 5,
    FED_STRIP_HIT_THRESHOLD_LOW = 2,

    FEUNITS_PER_FED = 8,      //!< Front End (FE) unit modules on a FED. Each FE unit is defined as the collection of units in the readout chain between
                              //!< the optical input and FE FPGA e.g. OptoRx, ADCs, delay chips.
    CHANNELS_PER_FEUNIT = 12, //!< Channel on each FE unit.
    APVS_PER_CHANNEL = 2,     //!< APVs per channel.
    STRIPS_PER_APV = 128,     //!< Strips per APV.

    CHANNELS_PER_FED = CHANNELS_PER_FEUNIT * FEUNITS_PER_FED, //!< Total channels on a FED.
    APVS_PER_FEUNIT = APVS_PER_CHANNEL * CHANNELS_PER_FEUNIT, //!< Number of APVs on a FE unit.
    APVS_PER_FED = APVS_PER_FEUNIT * FEUNITS_PER_FED,         //!< Total number of APVs on a FED.
    STRIPS_PER_CHANNEL = STRIPS_PER_APV * APVS_PER_CHANNEL,   //!< Number of strips on a channel.
    STRIPS_PER_FED = STRIPS_PER_CHANNEL * CHANNELS_PER_FED,    //!< Total number of strips on a FED.
    
    CHANNELS_PER_DELAY_CHIP = 4, //!< Channels on each delay chip.
    DELAY_CHIPS_PER_FED = CHANNELS_PER_FED / CHANNELS_PER_DELAY_CHIP, //!< Delay chips per FED.
    DELAY_CHIPS_PER_FEUNIT = CHANNELS_PER_FEUNIT / CHANNELS_PER_DELAY_CHIP, //!< Delay chips per FE unit.

    BUNCH_CROSSING_ORBIT_TOTAL = 3564,
    BUNCH_CROSSING_OFFSET = 1
  };
  
  enum {
    SPY_DELAY_CHIP_PAYLOAD_SIZE_IN_BYTES = 376*4,                                               //!< 376 32bit words
    SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES = SPY_DELAY_CHIP_PAYLOAD_SIZE_IN_BYTES+8,               //!< Extra 8 bytes for counters
    SPY_DELAYCHIP_DATA_OFFSET_IN_BITS = 44,                                                     //!< Offset to start of data
    SPY_SAMPLES_PER_CHANNEL = ( (SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES * 8) - SPY_DELAYCHIP_DATA_OFFSET_IN_BITS ) / 10 / CHANNELS_PER_DELAY_CHIP,
                                                                                                //!< Number of 10 bit samples per channel
    SPY_BUFFER_SIZE_IN_BYTES = SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES * DELAY_CHIPS_PER_FED + 32    //!< Delaychip data + 8 bytes header for counters
                                                                                                //!< + 8 bytes for TK special header + 16 bytes for DAQ header/trailer
    /*SPY_BUFFER_SIZE_IN_BYTES = //size of delay chips + extra 64bit word per delaychip for L1 and total event counts after read
                               (SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES+8) * DELAY_CHIPS_PER_FED
                               //plus 1 64bit word for tk special header word, 1 64bit word for initial L1 and total event counts and 2 64bit words for DAQ header/trailer
                               + 32
    SPY_SAMPLES_PER_CHANNEL = 300,
    //each sample is 10 bits
    SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES = ((SPY_SAMPLES_PER_CHANNEL*10)/8)*CHANNELS_PER_DELAY_CHIP,
    //include padding to 64bit words
    SPY_BUFFER_SIZE_IN_BYTES = //delay chip buffer size in bytes rounded to nearest 64bit word times number of delay chips
                               (SPY_DELAY_CHIP_BUFFER_SIZE_IN_BYTES+4) * DELAY_CHIPS_PER_FED
                               //plus 1 64bit word for tk special header word, 1 64bit word for QDR count and 2 64bit word for DAQ header/trailer
                               + 8 + 16*/
  };

  /**
   * \brief  Data mode describes the basic type of data you have here, fake, real or legacy data.
   *
   * Events can either be generated by the optical inputs, real data, or from the FE units, fake data and also support for an
   * old event format may be needed by some areas of the project. The following values are defined to distinguish between the
   * different type of event can be presented by the FED:
   * \li FED9U_EVENT_MODE_REAL Here real data has been processed from the optical links in the final format.
   * \li FED9U_EVENT_MODE_FAKE_FULL Here fake data is used, but it has likely been loaded from a file containing an old event and as such
   *     unique data is present on the each channel.
   * \li FED9U_EVENT_MODE_FAKE_SINGLE Here the fake data used has likely been loaded from a single .apv file that only contains data for two
   *     APVs and as such the data seen on the channels will be a duplicate of this data.
   * \li FED9U_EVENT_MODE_LEGACY Data taken in the 2004/05 test beam had a different header format and as such support for this legacy event
   *     type is required in some areas of the project.
   *
   */
  enum Fed9UEventDataMode { FED9U_EVENT_MODE_REAL, FED9U_EVENT_MODE_FAKE_FULL, FED9U_EVENT_MODE_FAKE_SINGLE, FED9U_EVENT_MODE_LEGACY };

  //<JEC date=11/4/07> new enum to handle 32-bit swapping in DAQ header and trailer
  enum Fed9UEventFormatType { FED9U_EVENT_FORMAT_OLD_VME, FED9U_EVENT_FORMAT_STANDARD };
  //</JEC>

  /**
   * \brief An enumeration defining the bus adaptors that can be used to communicate with the FED.
   *
   * There are five bus adaptors that the FED software can load drivers for:
   * \li FED9U_HAL_BUS_ADAPTOR_SBS SBS bus adaptor.
   * \li FED9U_HAL_BUS_ADAPTOR_CAEN_PCI Caen pci adaptor.
   * \li FED9U_HAL_BUS_ADAPTOR_CAEN_USB USB Caen adaptor.
   * \li FED9U_HAL_BUS_ADAPTOR_VXI VXI adaptor
   * \li FED9U_HAL_BUS_ADAPTOR_DUMMY Used when faking the presence of a FED.
   * \todo get a bit more information on each bus adaptor.
   */
  enum Fed9UHalBusAdaptor     { FED9U_HAL_BUS_ADAPTOR_SBS, FED9U_HAL_BUS_ADAPTOR_CAEN_PCI,
				FED9U_HAL_BUS_ADAPTOR_CAEN_USB, FED9U_HAL_BUS_ADAPTOR_VXI, FED9U_HAL_BUS_ADAPTOR_DUMMY};

  /**
   * \brief Selects the data acquision (DAQ) mode of the FED, where to find triggers or frames and how much processing to perform.
   *
   * There are four possible DAQ modes:
   * \li FED9U_MODE_PROCESSED_RAW Detects frames and only pedestal subtraction and strip reordering is performed.
   * \li FED9U_MODE_ZERO_SUPPRESSED Detects frames and performs full processing,
   *    strip reordering, pedestal and common mode subtraction as well as cluster finding.
   * \li FED9U_MODE_VIRGIN_RAW Detects frames and performs no event processing.
   * \li FED9U_MODE_SCOPE Detects triggers, no data processing is performed and the ticks marks and frame are present in the event.
   *    The amount of data recorded is deteermined by the scope length setting (Fed9UABC::setScopeLength).
   * There is an additional mode (FED9U_MODE_SPY) used in Fed9UEvent to signal that the event contins spy channel data.
   */
  enum Fed9UDaqMode     { FED9U_MODE_PROCESSED_RAW, FED9U_MODE_ZERO_SUPPRESSED, 
		          FED9U_MODE_VIRGIN_RAW, FED9U_MODE_SCOPE, FED9U_MODE_SPY, FED9U_MODE_NONE };

  /**
   * \brief Specifies an addition data acquision (DAQ) mode to the DAQ modes defined in Fed9UDaqMode.
   *
   * All super modes can be set with standard modes, however zero suppressed lite mode only has any effect when zero suppressed mode is chosen.
   * Fake events must also be configure (see Fed9UVmeDevice::initFakeEvent). There are four DAQ super modes:
   * \li FED9U_SUPER_MODE_FAKE Allow the FE units to send fake events, which are processed according to the DAQ mode.
   * \li FED9U_SUPER_MODE_ZERO_LITE A version of the zero suppression mode that has a smaller event size.
   * \li FED9U_SUPER_MODE_FAKE_ZERO_LITE Fake events are sent in lite zero suppression mode.
   * \li FED9U_SUPER_MODE_NORMAL No changes are made to the DAQ mode and the FED expects events from the optical inputs.
   */
  enum Fed9UDaqSuperMode     { FED9U_SUPER_MODE_FAKE, FED9U_SUPER_MODE_ZERO_LITE, FED9U_SUPER_MODE_FAKE_ZERO_LITE, FED9U_SUPER_MODE_NORMAL, // default modes strip top two highest bits in ZS
							   FED9U_SUPER_MODE_FAKE_HI_LO, FED9U_SUPER_MODE_ZERO_LITE_HI_LO, FED9U_SUPER_MODE_FAKE_ZERO_LITE_HI_LO, FED9U_SUPER_MODE_NORMAL_HI_LO,	//HI_LO indicates stripping 1 hi and 1 low BIT for ZS
							   FED9U_SUPER_MODE_FAKE_LO, FED9U_SUPER_MODE_ZERO_LITE_LO, FED9U_SUPER_MODE_FAKE_ZERO_LITE_LO, FED9U_SUPER_MODE_NORMAL_LO, // LO only indicates stripping lowest two bits in ZS
							   FED9U_SUPER_MODE_FAKE_10, FED9U_SUPER_MODE_NORMAL_10, FED9U_SUPER_MODE_ZERO_LITE_10, FED9U_SUPER_MODE_FAKE_ZERO_LITE_10 }; // 10 bit readout we no longer strip any bits in ZS and in VR we pack 10 bits instead of 16 to reduce event size

  /**
   * \brief Defines the types of trigger that the FED can use.
   *
   * There are four possible trigger sources:
   * \li FED9U_TRIG_NONE The FED is told there is no source for the trigger.
   * \li FED9U_TRIG_TTC  TTC is used for the triger source.
   * \li FED9U_TRIG_BACKPLANE The VME backplane is used to provide a trigger.
   * \li FED9U_TRIG_SOFTWARE Fed9UVmeDevice::sendSoftwareTrigger can be used to send an event trigger to the FED.
   */
  enum Fed9UTrigSource  { FED9U_TRIG_NONE          = 0, FED9U_TRIG_TTC             = 1,
			  FED9U_TRIG_BACKPLANE     = 2, FED9U_TRIG_SOFTWARE        = 4 };

  /**
   * \brief The clock that the FED is using.
   *
   * There are three possible sources of the clock:
   * \li FED9U_CLOCK_INTERNAL Default clock source that is present on the FED.
   *    There must always be a clock source and in the event of a lost clock the FED defaults to this source.
   * \li FED9U_CLOCK_TTC The clock provided by the TTC system.
   * \li FED9U_CLOCK_BACKPLANE A clock provided by the VME crate back plane.
   *    Useful for test where the TTC is not present and an external clock is required as no extra hardware is required.
   */
  enum Fed9UClockSource { FED9U_CLOCK_INTERNAL     = 1, FED9U_CLOCK_TTC            = 2,
			  FED9U_CLOCK_BACKPLANE    = 4 };

  /**
   * \brief Defines the possible readout routes for the data from the FED.
   *
   * There are three possible routes to readout the events from the FED:
   * \li FED9U_ROUTE_NONE The data will not be readout and lost.
   * \li FED9U_ROUTE_SLINK64 S-link 64 is used for data readout. This is the prefered and fastest mode.
   * \li FED9U_ROUTE_VME Uses the VME to readout, slow but requires no extra hardware for the readout.
   */
  enum Fed9UReadRoute   { FED9U_ROUTE_NONE         = 0, FED9U_ROUTE_SLINK64        = 1,
			  FED9U_ROUTE_VME          = 2 };

  /**
   * \brief Defines the header format that is used in the event.
   *
   * There are two possible header types:
   * \li FED9U_HEADER_FULLDEBUG All the APV header information is included in the header.
   * \li FED9U_APVERROR Only the APV error flags are contained in the header.
   */
  enum Fed9UHeaderFormat {FED9U_HEADER_FULLDEBUG = 1, FED9U_HEADER_APVERROR = 2};


  /**
   * \brief Defines the possible fill of the FED FIFOs.
   *
   * The different FIFOs on the FED can be refered to using the Fed9UFifoStatus enumeration.
   * There are four possible fill levels:
   * \li FED9U_FIFO_EMPTY No event data present in the FIFO.
   * \li FED9U_FIFO_PARTIAL The FIFO is partially full.
   * \li FED9U_FIFO_FULL FIFO is full, event data will be lost.
   * \li FED9U_FIFO_NOT_EMPTY There is data in the FIFO.
   * \todo get better definitions of how full the FIFOs are.
   */
  enum Fed9UFifoStatus { FED9U_FIFO_EMPTY          = 0, FED9U_FIFO_PARTIAL         = 1,
			 FED9U_FIFO_FULL           = 2, FED9U_FIFO_NOT_EMPTY       = 3,
			 FED9U_FIFO_PARTIAL_EMPTY  = 4, FED9U_FIFO_HALF_FULL       = 5};

  /**
   * \brief Used to select the various FED FIFOs for status retrieval.
   *
   * There are seven FIFOs that can be addressed on the FED:
   * \li FED9U_FIFO_QDR
   * \li FED9U_FIFO_FRAME_ADDRESS
   * \li FED9U_FIFO_TOTAL_LENGTH
   * \li FED9U_FIFO_TRACKER_HEADER
   * \li FED9U_FIFO_L1A_BX
   * \li FED9U_FIFO_EVENT_LENGTH
   * \li FED9U_FIFO_FEFPGA
   *
   * \todo - what are these for?
   */
  enum Fed9UFifoType { FED9U_FIFO_QDR = 0, FED9U_FIFO_FRAME_ADDRESS = 1, FED9U_FIFO_TOTAL_LENGTH = 2, FED9U_FIFO_TRACKER_HEADER = 3,
		       FED9U_FIFO_L1A_AND_BX = 4, FED9U_FIFO_EVENT_LENGTH = 5, FED9U_FIFO_FEFPGA = 6 };

  /**
   * \brief This enum is used to define how much of the EPROM memory is read only.
   *
   * FED9U_EPROM_NONE   - none of the memory is protected.
   * FED9U_EPROM_QUATER - the upper quater (1536 - 2046) is protected.
   * FED9U_EPROM_HALF   - the upper half (1024 - 2046) is protected.
   * FED9U_EPROM_ALL    - all the memory is write protected.
   */
  enum Fed9UEpromReadOnlyLevel { FED9U_EPROM_NONE, FED9U_EPROM_QUATER, FED9U_EPROM_HALF, FED9U_EPROM_ALL };

  enum Fed9UEpromArea {EPROM_WRITE_PROTECTED = 1536}; //< This number sets the byte at which the level of the EPROM write protected area begins.

  /**
   * \brief This enum is used to read set bits in the voltage monitor status register. It is used as a mask
   *        to decode the status word returned in Fed9UVoltageMonitor::getStatusRegister.
   *
   * The status bit that can be read by each enumeration:
   * \li FED9U_VOLT_STATUS_2POINT5 Will determine whether the flag for the 2.5V out of bounds is set.
   * \li FED9U_VOLT_STATUS_3POINT3 Will determine whether the flag for the 3.3V out of bounds is set.
   * \li FED9U_VOLT_STATUS_5 Will determine whether the flag for the 5V out of bounds is set.
   * \li FED9U_VOLT_STATUS_12 Will determine whether the flag for the 12V out of bounds is set.
   * \li FED9U_VOLT_STATUS_CORE Will determine whether the flag for the core voltage out of bounds is set.
   * \li FED9U_VOLT_STATUS_SUPPLY Will determine whether the flag for the supply out of bounds is set.
   * \li FED9U_VOLT_STATUS_INTERNAL_TEMP Will determine whether the flag for the internal temperature of
   *    the voltage montior out of bounds is set.
   * \li FED9U_VOLT_STATUS_EXTERNAL_TEMP Will determine whether the flag for the external temperature out of bounds is set.
   * \li FED9U_VOLT_STATUS_NO_EXTERNAL Will determine whether the flag for a present external temperature reading is set or not.
   */
  enum Fed9UVoltageStatus { FED9U_VOLT_STATUS_2POINT5 = 0x001, FED9U_VOLT_STATUS_3POINT3 = 0x002,
			    FED9U_VOLT_STATUS_5       = 0x004, FED9U_VOLT_STATUS_12      = 0x008,
			    FED9U_VOLT_STATUS_CORE    = 0x010, FED9U_VOLT_STATUS_SUPPLY  = 0x020,
			    FED9U_VOLT_STATUS_INTERNAL_TEMP    = 0x040, FED9U_VOLT_STATUS_EXTERNAL_TEMP = 0x080,
			    FED9U_VOLT_STATUS_NO_EXTERNAL_TEMP = 0x100 };

  /**
   * \brief This enumeration is used to mask the returned u32 from the status register in the Fed9UTempControlInfo
   *        class in order to check the status bits on the temperature limits.
   *
   * The possible enumerations are as follows:
   * \li FED9U_TEMP_STATUS_LM82_HIGH Mask for temperature chip out temperature limits.
   * \li FED9U_TEMP_STATUS_FPGA_HIGH Mask for FPGA connected to temperature chip is out of temperature limits.
   * \li FED9U_TEMP_STATUS_LM82_CRIT Mask for temperature chip has exceeded critical limits, only set if shut down at critical temperature limits is disabled.
   * \li FED9U_TEMP_STATUS_FPGA_CRIT Mask for FPGA chip has exceeded critical temperature limit, only set if shut down at critical temperature limits is disabled.
   * \li FED9U_TEMP_STATUS_DISCONNETED Mask for FPGA chip is disconneted.
   */
  enum Fed9UTemperatureStatus { FED9U_TEMP_STATUS_LM82_HIGH = 0x01, FED9U_TEMP_STATUS_FPGA_HIGH = 0x02,
				FED9U_TEMP_STATUS_LM82_CRIT = 0x04, FED9U_TEMP_STATUS_FPGA_CRIT = 0x08,
				FED9U_TEMP_STATUS_FPGA_DISCONNETED = 0x10 };

  /**
   * \brief These enum masks values can be used to enable the bunch and event counters.
   *
   * These are used in conjunction with the method Fed9UTtcrxDevice::setCounterOperation and can be set by using a bitwise OR.
   * \li FED9U_TTCRX_ENABLE_BUNCH_CTR Enables the bunch counter on the TTCrx chip.
   * \li FED9U_TTCRX_ENABLE_EVENT_CTR Enables the event counter on the TTCrx chip.
   */
  enum Fed9UTtcrxCntrOperation { FED9U_TTCRX_ENABLE_BUNCH_CTR = 0x01, FED9U_TTCRX_ENABLE_EVENT_CTR = 0x02 };

  /**
   * \brief Use to decode the configuration register settings on the TTCrx chip by masking the returned word.
   *
   * They correspond to a read only register on the TTCrx and as such once the return value is decoded it should
   * correspond to the default values give at the of the description for each setting. This is not the value of
   * the enum!
   * \li FED9U_TTCRX_CONFIG_SEL_TEST_PD Selects the external test signal for enabling
   *    the PPL phase detector. Default is 0 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_SEL_INPUT_A When 1 selects the inputs from option link,
   *    otherwise test_in. Default is 1 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_PLL_RESET Assert PLL test reset line. Default 0 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_DLL_RESET Assert DLL test reset line. Default 0 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR If 1 disables the automatic frequency increase
   *    after a PLL reset. Default 0 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG If 1 disbale watchdog circuit. Default 0 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION - Enable hamming error correction on
   *    incoming data stream. Default 1 on the TTCrx.
   * \li FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS Enables test input/outputs. Default 0 on the TTCrx.
   */
  enum Fed9UTtcrxConfig { FED9U_TTCRX_CONFIG_SEL_TEST_PD = 0x01, FED9U_TTCRX_CONFIG_SEL_INPUT_A = 0x02,
			  FED9U_TTCRX_CONFIG_PLL_RESET   = 0x04, FED9U_TTCRX_CONFIG_DLL_RESET   = 0x08,
			  FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR = 0x10,  FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG = 0x20,
			  FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION = 0x40, FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS = 0x80 };

  /**
   * \brief Used to decode the status word read back from the TTCrx chip.
   *
   * They correspond to a read only register on the TTCrx and as such once the return value is decoded it should
   * correspond to the condition as indicated after the enum members explained below. This is not the value of
   * the enum!
   * The flags that can be set are as follows :-
   * \li FED9U_TTCRX_STATUS_AUTO_RESET A 1 indicates an automatic reset has occurred due to a
   *    timeout condition in the watchdog circuit.
   * \li FED9U_TTCRX_STATUS_FRAME_SYNCH A 1 indicates that channel B is synchronized to the
   *    data stream.
   * \li FED9U_TTCRX_STATUS_DLL_READY A 1 indicates the High-Resolution phase shiftes are
   *    working properly.
   * \li FED9U_TTCRX_STATUS_PLL_READY A 1 indicates that the clock and data recovery circuit
   *    is locked on the incoming data stream.
   */
  enum Fed9UTtcrxStatus { FED9U_TTCRX_STATUS_AUTO_RESET = 0x1, FED9U_TTCRX_STATUS_FRAME_SYNCH = 0x2,
			  FED9U_TTCRX_STATUS_DLL_READY  = 0x4, FED9U_TTCRX_STATUS_PLL_READY   = 0x8 };

  /**
   * \brief Used to define the levels of debug print out available from the Fed9U software library.
   *
   * The values available are as follows :-
   * \li FED9U_DEBUG_LEVEL_NONE No debug statements are printed.
   * \li FED9U_DEBUG_LEVEL_MINIMAL Only the most important debug statements are print.
   *     These tend to refer updates in the status of functions that do multiple tasks.
   * \li FED9U_DEBUG_LEVEL_INTERMEDIATE This provides more detailed information about the function progression
   *     and can help in determining problems with the operation. Also minimal printout statements will be displayed.
   * \li FED9U_DEBUG_LEVEL_DETAILED Used primarily for debugging the function itself and will typically only be
   *     required if an almost line by line function execution detail is required. All other printout levels would
   *     be printed as well.
   * \li FED9U_DEBUG_LEVEL_SUPER_DETAILED This level of debug is to be set for things that will likely generate a huge
   *     amount of printing out, such as when polling for a condition, and hence are usually only ever required once
   *     even during the debugging stages.
   *
   * These debug levels are used with the static global variable sgDebugLevel to set the output level that can be
   * varied at run time to determine what level of debug is to be printed. It is also used at compile time to specify
   * the level of the debug statement that is being sent to the Fed9UDebugLog, which will only print the statements
   * if they exceed the envirnomentally debug level.
   */
  enum Fed9UDebugLevel { FED9U_DEBUG_LEVEL_NONE=0, FED9U_DEBUG_LEVEL_MINIMAL=1, FED9U_DEBUG_LEVEL_INTERMEDIATE=2,
			 FED9U_DEBUG_LEVEL_DETAILED=3, FED9U_DEBUG_LEVEL_SUPER_DETAILED=4 };

  /**
   * \brief A typedef for the enumerate type Fed9UDebugLevel.
   *
   * The typedef serves to provide a clearer meaning when used to construct Fed9UMessage objects.
   */
  typedef Fed9UDebugLevel Fed9UDebug;

  /**
   * \brief Used to define the level of error print out available from the Fed9U software library.
   *
   * The values available are as follows :-
   * \li FED9U_ERROR_LEVEL_CRITICAL Only those errors that will halt the running of an function and were unable to be
   *     corrected will be printed.
   * \li FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED Refers to errors that should not have happened, but they did happen, but we were
   *     we were prepared for them. Hence they were corrected and the function continued in some limited good state. Would also
   *     cause critical errors to be printed.
   * \li FED9U_ERROR_LEVEL_EXPECTED There are some exceptions that will be caught, that were expected to be thrown and although
   *     indicate a problem in a called function do not necessarily mean that the calling function can not continue in a perfectly
   *     stable state.
   * \li FED9U_ERROR_LEVEL_WARN Printed when any exception occurs. Can be used to trace errors up the throw chain or
   *     refer to those errors that may be induced that was an expected result of a function call and hence the original
   *     function will continue along one of its normal execution paths. Also will print out all other errors.
   *
   * The error levels are used with the static global variable sgErrorLevel to determine what level of error a User might
   * wish to see, that can be varied at run time. It is also used with the stream class Fed9UErrorLog to set what level
   * the error being streamed is, and thus may not need to be printed depending on the global level.
   */
  enum Fed9UErrorLevel { FED9U_ERROR_LEVEL_CRITICAL=0, FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED=1, FED9U_ERROR_LEVEL_EXPECTED=2, FED9U_ERROR_LEVEL_WARNING=3 };

  /**
   * \brief A typedef for the enumerate type Fed9UDebugLevel.
   *
   * The typedef serves to provide a clearer meaning when used to construct Fed9UMessage objects.
   */
  typedef Fed9UErrorLevel Fed9UError;

  /**
   * \brief Used to set the level of detail that is to be sent to the Fed9ULog file.
   *
   * The following levels of detail can be set :-
   * \li FED9U_LOG_LEVEL_NONE No output is sent to the file at all, and it constructed with this vaule it will not be created
   *     till the first log message is sent with a higher message level.
   * \li FED9U_LOG_LEVEL_ERROR Those log messages defined as errors are sent to the log.
   * \li FED9U_LOG_LEVEL_DEBUG Error messages and debug important debug statments, displaying typically important steps in a
   *     function call process are sent to the message.
   * \li FED9U_LOG_LEVEL_DETAILED All log messages sent to the stream will be written to the log file.
   *
   * These log levels are used in conjuction with the Fed9ULog class and static global sgLogLevel. Log messages are streamed to
   * the Fed9ULog object, with a Fed9ULogLevel defined for the message and then if the global log level is at or higher than
   * the message level it will be sent to the log.
   */
  enum Fed9ULogLevel   { FED9U_LOG_LEVEL_NONE=0, FED9U_LOG_LEVEL_ERROR=1, FED9U_LOG_LEVEL_DEBUG=2, FED9U_LOG_LEVEL_DETAILED=3 };

  /**
   * \brief A typedef for the enumerate type Fed9UDebugLevel.
   *
   * The typedef serves to provide a clearer meaning when used to construct Fed9UMessage objects.
   */
  typedef Fed9ULogLevel Fed9ULogFile;

  /**
   * \brief A static global variable to define the level of debug printout that is sent to the debug stream.
   *
   * This is initialised to the FED9U_DEBUG_LEVEL_MINIMAL upon program start up (in Fed9ULogTemplate.cc), which means no messages will be sent to the debug
   * stream. It can be set to any level at run time, which will immediately cause debug messages of that level be sent to the
   * debug stream.
   */
  //extern Fed9UDebugLevel sgDebugLevel;

  /**
   * \brief A static global variable to define the level of error printout that is sent to the error stream.
   *
   * This is initialised to the FED9U_ERROR_LEVEL_CRITICAL upon program start up (in Fed9ULogTemplate.cc), which means only erorr messages defined as critical
   * will be sent to the debug stream. It can be set to any level at run time, which will immediately cause debug messages of that
   * level be sent to the error stream.
   */
  //extern Fed9UErrorLevel sgErrorLevel;

  /**
   * \brief A static global variable the defines what level of log messages are to be sent to the Fed9U file log.
   *
   * This is initialised to FED9U_LOG_LEVEL_NONE upon program start up (in Fed9ULogTemplate.cc), which means no log file is created until the
   * log level is set to a higher value and the first message is sent to the log. It can be changed to any level at
   * run time, which will cause the log messages of that level to be immediately written to the stream.
   */
  //extern Fed9ULogLevel sgLogLevel;

/**
 * \brief This macro creates inline functions for input and output streams for an enumeration.
 * \param name Name of the enumeration to be made streamable.
 */
#define MAKE_ENUM_STREAMABLE(name) \
inline std::ostream& operator<<(std::ostream& os, name   value) { return os << static_cast<long>(value); } \
inline std::istream& operator>>(std::istream& is, name & value) { long t; is >> t; value = static_cast<name>(t); return is; }
  MAKE_ENUM_STREAMABLE(Fed9UHalBusAdaptor)
  MAKE_ENUM_STREAMABLE(Fed9UDaqMode)
  MAKE_ENUM_STREAMABLE(Fed9UDaqSuperMode)
  MAKE_ENUM_STREAMABLE(Fed9UTrigSource)
  MAKE_ENUM_STREAMABLE(Fed9UClockSource)
  MAKE_ENUM_STREAMABLE(Fed9UReadRoute)
  MAKE_ENUM_STREAMABLE(Fed9UEpromReadOnlyLevel)
  MAKE_ENUM_STREAMABLE(Fed9UVoltageStatus)
  MAKE_ENUM_STREAMABLE(Fed9UTemperatureStatus)
  MAKE_ENUM_STREAMABLE(Fed9UTtcrxCntrOperation)
  MAKE_ENUM_STREAMABLE(Fed9UTtcrxStatus)
  MAKE_ENUM_STREAMABLE(Fed9UHeaderFormat)
  MAKE_ENUM_STREAMABLE(Fed9UFifoStatus)
  MAKE_ENUM_STREAMABLE(Fed9UFifoType)
  //Don't want people to be able to accidentally write a level to a stream and it not have the desired effect.
  //MAKE_ENUM_STREAMABLE(Fed9UDebugLevel);
  //MAKE_ENUM_STREAMABLE(Fed9UErrorLevel);
  //MAKE_ENUM_STREAMABLE(Fed9ULogLevel);

#ifdef CMS_TK_64BITS

#include <stdint.h>

  typedef int8_t  i8;  ///< Signed 8-bit integer.
  typedef int16_t i16; ///< Signed 16-bit integer.
  typedef int32_t  i32; ///< Signed 32-bit integer.

  typedef uint8_t  u8;  ///< Unsigned 8-bit integer.
  typedef uint16_t u16; ///< Unsigned 16-bit integer.
  typedef uint32_t  u32; ///< Unsigned 32-bit integer.
  typedef uint64_t  u64; ///< Unsigned 32-bit integer.

#else 
  #error CMS_TK_64BITS not set
  typedef signed char  i8;  ///< Signed 8-bit integer.
  typedef signed short i16; ///< Signed 16-bit integer.
  typedef signed long  i32; ///< Signed 32-bit integer.

  typedef unsigned char  u8;  ///< Unsigned 8-bit integer.
  typedef unsigned short u16; ///< Unsigned 16-bit integer.
  typedef unsigned long  u32; ///< Unsigned 32-bit integer.
  typedef unsigned long long  u64; ///< Unsigned 32-bit integer.


#endif
 

#define STRINGIZE_2(x) #x
#define STRINGIZE(x) STRINGIZE_2(x)

#define NO_DELAY_SKEWS 32 //!< \depreciated there are now 64 or 32 delay skews depending on firmware version.

}//namespace Fed9U

#endif // H_TypeDefs
#ifndef H_Fed9UFileHelpers
#define H_Fed9UFileHelpers

#include <string>

namespace Fed9U {

  /**
   * \brief  Function in the "global" Fed9U namespace to open a file for reading/writting.
   * \param  filename String containing the name and path of the file to be opened.
   * \param  newfile Returned by reference, the opened file is a new file if true.
   * \return int An identifier for the file that was opened.
   * \throw  ICException This error is thrown if there is any problems opening the file.
   */
  int Fed9UFile_open(const std::string & filename, bool & newfile);

  /**
   * \brief Function in the "global" Fed9U namespace to close a file opened by Fed9UFile_open.
   * \param fd Identifier of the file to be closed.
   */
  void Fed9UFile_close(int fd);

  /**
   * \brief  Function in the "global" Fed9U namespace to read from a file opened by Fed9UFile_open and returns its contents as a string.
   * \param  fd Identifier of the file to be read from.
   * \return string Contains the file data.
   */
  std::string Fed9UFile_read(int fd);

  /**
   * \brief Function in the "global" Fed9U namespace to overwrite the data in a file opened by Fed9UFile_open with data specified by the User.
   * \param fd Identifier of the file to be written to.
   * \param data Constant reference to the data that is to be written.
   */
  void Fed9UFile_overwrite(int fd, const std::string & data);

}

#endif // H_Fed9UFileHelpers
#ifndef H_Fed9UWait
#define H_Fed9UWait

namespace Fed9U {

  /**
   * \brief Waits for a number of microseconds before returning.
   * \param microsec Number of microseconds to waits for. Defaulted to zero.
   */
  void fed9Uwait(unsigned long microsec = 0);

  /**
   * \brief Waits for a time period specified in seconds and microseconds.
   * \param seconds Number of seconds to wait for.
   * \param microsec Number of microseconds to wait for.
   */
  void fed9Uwait(unsigned long seconds, unsigned long microsec);
  
}

#endif // H_Fed9UWait
#ifndef H_Fed9UChannelInterface
#define H_Fed9UChannelInterface


#define FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT false //!< Default behaviour whether to count from the top, internal numbering first channel at top.
                                                             //!< or to count from the bottom, external number last channel at the top.
#define FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT 1         //!< Default behaviour whether to start counting from one or zero.

namespace Fed9U {

  class Fed9UAddress;
#ifdef EVENT_STREAMLINE
  class Fed9UEventStreamLine;
#else
  class Fed9UEvent;
#endif

  /**
   * \brief FED Channel Interface class.
   *
   * This class simply provides an abstracted interface for the user to set the desired channel. The channel can be set on a
   * variety of granularities. Setting the FED channel, that also defines the FE unit channel number and FE unit number, which
   * can be done either on construction or using setter methods. The FE unit channel and unit number can also be set individually
   * after default construction using setter methods. It also allows the user to count in 0 or 1 based and from top to bottom or
   * bottom to top, depending on whether they use the internal (zeroth/first channel at the top) or external (first channel at the
   * bottom) numbering schemes. Top and bottom refer to the physical top and bottom of the FED.
   */
  class Fed9UChannelInterface {
  public:

    /**\name Constructors*/
    //@{

    /**
     * \brief Default constructor.
     *
     * Sets the channel to zero and the counting to FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT and the counting base
     * to FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT.
     */
    Fed9UChannelInterface();

    /**
     * \brief Constructor that allows FED channel, counting base and count direction to be set upon construction.
     * \param fedChannel Channel on the FED that is to be set, in either external or internal number scheme, this is defined by next two arguments.
     * \param countingBase Zero or one are the two allowed numbers to start counting from. Defaulted to class default as defined by
     *        FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT.
     * \param fromTop True if counting starts from the top, as in the internal numbering system, or false if the counting starts from the bottom
     *        as in the external numbering scheme. 
     */
    Fed9UChannelInterface(u8 fedChannel, u8 countingBase = FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT, bool fromTop = FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT);
    //@}

    /**\name Getters*/
    //@{

    /**
     * \brief  Each FED has 96 channels that are individually addressable.
     * \return unsigned char FED channel number selected.
     */
    u8 getChannel() const;

    /**
     * \brief  Each FED has 8 FE units that are individually addressable.
     * \return unsigned char FE unit number.
     */
    u8 getFeUnit() const;

    /**
     * \brief  Within each FE unit there are 12 channels that can be selected.
     * \return unsigned char FE unit channel number.
     */
    u8 getFeUnitChannel() const;

    /**
     * \brief  Counting can either begin from zero or one.
     * \return unsigned char Returns the base used for counting.
     */
    u8 getBase();

    /**
     * \brief  Counting can either start from the top of the FED or the bottom. Top is internal number, bottom external numbering.
     * \return bool True indicates counting is from the top and false from the bottom.
     */
    bool getCountFromTop();
    //@}

    /**\name Setters*/
    //@{

    /**
     * \brief Each FED has 96 channels that are individually addressable.
     * \param channel FED channel number to be selected.
     */
    Fed9UChannelInterface & setChannel(u8 channel);

    /**
     * \brief Each FED has 8 FE units that are individually addressable.
     * \param feUnit FE unit number to be used.
     */
    Fed9UChannelInterface & setFeUnit(u8 feUnit);

    /**
     * \brief Within each FE unit there are 12 channels that can be selected.
     * \param feUnitChannel FE unit channel number to be used.
     */
    Fed9UChannelInterface & setFeUnitChannel(u8 feUnitChannel);

    /**
     * \brief Counting can either begin from zero or one.
     * \param base Sets the base used for counting.
     */
    Fed9UChannelInterface & setBase(u8 base);

    /**
     * \brief Counting can either start from the top of the FED or the bottom. Top is internal number, bottom external numbering.
     * \param fromTop True indicates counting is from the top and false from the bottom.
     */
    Fed9UChannelInterface & setCountFromTop(bool fromTop);
    //@}


    friend class Fed9UAddress;
#ifdef EVENT_STREAMLINE
    friend class Fed9UEventStreamLine;
#else
    friend class Fed9UEvent;
#endif

  private:

    /**
     * \brief  There are 96 channels on a FED and the internal number scheme starts counting from zero at the top of the FED.
     * \return unsigned char The channel number selected according to the internal number scheme.
     */
    u8  getChannelInternal() const;

    /**
     * \brief There are 96 channels on a FED and the internal number scheme starts counting from zero at the top of the FED.
     * \param channel The channel number to be selected according to the internal number scheme.
     */
    Fed9UChannelInterface & setChannelInternal(u8 channel);

    u8 _channel;   //!< FED channel number, stored in internal numbering format (from top, base 0).
    u8 _base;      //!< Base the User has selected channel counting to start from.
    bool _fromTop; //!< Whether the counting should begin from the top or bottom of the FED channels.
  };

}

#endif // H_Fed9UChannelInterface
#ifndef H_Fed9UChannelAddress
#define H_Fed9UChannelAddress


namespace Fed9U {

  /**
   * FED front-end address conversion class.
   *
   * Front-end address class, can point to any area of the FED front-end, from a front-end unit,
   * through any fibre channel, an individual APV, or any strip within an APV. \n
   * The different granularities are achieved by setting a value of UNSPECIFIED in all
   * unrequired levels. \n
   * It can also point to the BE and VME FPGAs, which do not containing any subunits.
   * Example: \n
   * To get the FED channel number of channel 11 in unit 3: \n
\verbatim
u8 fedChannelNumber = Fed9UAddress(3, 11).getFedChannel();
\endverbatim
   * It is also possible to create an Fed9UAddress object from the FED channel number: \n
\verbatim
Fed9UAddress fedChannel = Fed9UAddress(123, 0);
u8 fedFeUnit = fedChannel.getFedFeUnit();
u8 moduleChannel = fedChannel.getFeUnitChannel();
\endverbatim
   * The setters return a reference to the original object, so that multiple sets can be chained together.
   *
   * The second part to this class is an interface to an external numbering system. If the external numbering 
   * system differs from the internal. In the example of the FED the external numbering for channels runs from 
   * 1 to 96 from bottom to top but the internal numbering runs from top to bottom 0 to 95. Also the numbering of the 
   * Front End Units (FEUnits) runs from 1 to 8 bottom to top externally, and 0 to 7 top to bottom internally.
   * Users must be aware of this difference and therefore make sure they know when they are using external methods.
   * all external methods are marked with External Keyword. So getFEUnitChannel() will get you the internal representation
   * and getExternalFEUnitChannel() will get you the user defined external numbering for that channel.
   * if the channel in question is internally numbered as 5 then the external FedChannel will be 91 for final CMS Tracker FED 
   * numbering.
   * WARNING, if you change the base to anything other than 0 or 1 then there will be conflicts between BACKEND fpga number and FeUnit number since this should be 9 for both. This software is not designed to work for any other bases than 0 and 1.
   ******************************************/

#define FED9U_ADDRESS_COUNT_FROM_TOP false
#define FED9U_ADDRESS_COUNT_BASE 1

  class Fed9UAddress {
  public:
    /**
     * Defines the values for a few specific addresses on the FED.
     *
     * \li UNSPECIFIED Used internally if an address is not set, most commonly when that level of granularity is not required.
     * \li FEBROADCAST Tells the FED that the command should be issued to all FE units.
     * \li BACKEND The BE FPGA address, used for commands that can be issued to all FPGAs.
     * \li VME The VME FPGA address, used for commands that can be issued to all FPGAs.
     */
    enum { UNSPECIFIED = 0xFF, FEBROADCAST = 14, BACKEND = 9, VME = 10 };

    /** \name Constructors */
    //@{

    /**
     * \brief Default constructor. Sets all values to unspecified and counts from bottom, with a base of one by default.
     *
     * Used to create an empty address, that can be updated using the setter member functions.
     */
    Fed9UAddress();

    /**
     * \brief A constructor provided to allow a FED channel address and further granularities to be set upon construction.
     * \param fedChannel FED channel that is to be used, will also set the FE unit and FE unit channel.
     * \param channelApv APV on the channel to be set. Defaulted to UNSPECIFIED, not required at all granularities.
     * \param apvStrip Strip on a given APV, as defined by channelApv. Defaulted to UNSPECIFIED, only required at lowest granularities.
     */
    Fed9UAddress(u8 fedChannel, u8 channelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);

    /**
     * \brief A constructor provided to allow the counting direction to be specfied and the base to be set.
     * \param bool Boolean true will start the counting from the top of the FED, internal numbering. A boolean
     *        false will start the numbering from the bottom, external number.
     * \param u16 Sets the counting base, 0 or 1 are the only two valid bases. External numbering uses 1 as the first bottom FE unit
     *        and internal numbering uses 0 for the top FE unit.
     *
     * The counting base also affects the channel, APV and strip numbering as well as the FE unit number.
     */
    Fed9UAddress(bool, u16);

    //@}
    /** \name Getters */
    //@{

    /**
     * \brief  Returns which of the ten FED FPGA is being addressed.
     * \return unsigned char FED FGPA number, internal numbering system.
     *
     * FED FPGAs include FE, BE, VME and broadcast settings. These latter three are defined in the class enumeration.
     */
    u8  getFedFpga() const;

    /**
     * \brief  Returns which of the eight FED FE units is being addressed.
     * \return unsigned char FED FE unit number, internal numbering system.
     */
    u8  getFedFeUnit() const;

    /**
     * \brief  Returns which of the 12 FE channels is being addressed.
     * \return unsigned char FED channel number, internal numbering system.
     */
    u8  getFeUnitChannel() const;
    
    // <NAC date=28/03/2007> DelayChip
    /**
     * \brief  Returns which of the 3 delay chips is being addressed.
     * \return unsigned char FED FE unit delay chip number (0-3, top to bottom).
     */
    u8  getFeUnitDelayChip() const;
    
    /**
     * \brief  Returns which of the 24 delay chips is being addressed.
     * \return unsigned char FED delay chip number.
     */
    u8 getFedDelayChip() const;
    // </NAC>

    /**
     * \brief  Returns which of the two channel APVs are being addressed.
     * \return unsigned char Channel APV number, internal numbering system.
     */
    u8  getChannelApv() const;

    /**
     * \brief  Returns which of the 128 strips on an APV is being addressed.
     * \return unsigned char APV strip unit number, internal numbering system.
     */
    u8  getApvStrip() const;

    /**
     * \brief  Returns which of the 96 FED channel is being addressed.
     * \return unsigned char FED channel number, internal numbering system.
     */
    u8  getFedChannel() const;

    /**
     * \brief  Returns which of the 192 FED APVs is being addressed.
     * \return unsigned char FED APV number, internal numbering system.
     */
    u8  getFedApv() const;

    /**
     * \brief  Returns which of the FEDs 24,576 strips are being addressed.
     * \return unsigned char FED strip number, internal numbering system.
     */
    u16 getFedStrip() const;
    /**
     * \brief  Returns which of the 24 FE unit APVs is being addressed.
     * \return unsigned char FE unit APV number, internal numbering system.
     */
    u8  getFeUnitApv() const;

    /**
     * \brief  Returns which of the FEDs 48 channel pairs is being addressed.
     * \return unsigned char FED channel pair number, internal numbering system.
     */
    u8  getFedChannelPair() const;

    /**
     * \brief  Returns which of theis six FE unit channel pairs is being addressed.
     * \return unsigned char FE unit channel pair number, internal numbering system.
     */
    u8  getFeUnitChannelPair() const;

    /**
     * \brief  Returns which of the channels from the channel pair is being addressed.
     * \return unsigned char Channel from the channel pair number, internal number system.
     */
    u8  getChannelPairChannel() const;

    /**
     * \brief  Returns which of the channels in the pair is being addressed in the external numbering scheme.
     * \return unsigned char External channel pair number.
     */
    u8  getExternalChannelPairChannel() const;

    /**
     * \brief  Returns which of the 48 FED channel pairs is being addressed in the external numbering scheme.
     * \return unsigned char External FED channel pair number.
     */
    u8  getExternalFedChannelPair() const;

    /**
     * \brief  Returns which of the six FE unit channel pairs is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel pair number.
     */
    u8  getExternalFeUnitChannelPair() const;

    /**
     * \brief  Returns which of the eight FE unit is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel pair number.
     */
    u8  getExternalFedFeUnit() const;

    /**
     * \brief  Returns which of the 12 FE unit channels is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel number.
     */
    u8  getExternalFeUnitChannel() const;

    /**
     * \brief  Returns which of the two channel APVs is being addressed in the external numbering scheme.
     * \return unsigned char External channel APV number.
     */
    u8  getExternalChannelApv() const;

    /**
     * \brief  Returns which of the 128 APV strips is being addressed in the external numbering scheme.
     * \return unsigned char External APV strip number.
     */
    u8  getExternalApvStrip() const;

    /**
     * \brief  Returns which of the 96 FED channels is being addressed in the external numbering scheme.
     * \return unsigned char External FED channel number.
     */
    u8  getExternalFedChannel() const; 

    /**
     * \brief  Returns which of the 192 FED APVs is being addressed in the external numbering scheme.
     * \return unsigned char External FED APV number.
     */
    u16 getExternalFedApv() const;

    /**
     * \brief  Returns which of the 24,576 FED strips is being addressed in the external numbering scheme.
     * \return unsigned char External FED strip number.
     */
    u16 getExternalFedStrip() const;

    /**
     * \brief  Returns which of the 48 FE unit APVs is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit APV number.
     */
    u8  getExternalFeUnitApv() const;

    /**
     * \brief  Returns the counting base used by the class.
     * \return unsigned short Counting base.
     */
    u16 getCountBase() const;

    /**
     * \brief  Whether the class is counting from the top or bottom FE unit.
     * \return bool Boolean true it is counting from the top, internal numbering; boolean flase from the bottom, external numbering.
     */
    bool getCountFromTop() const;

    /**
     * \brief  This method allows you to get the feUnit number based upon the feFirmware Address.
     * \return unsigned char FE firmware FE unit number.
     *
     * The FED firmware numbering runs from 1 throuh 8 and 15, than from 0 through 7 as in software internal
     * or 8 through 1 in external numbering is used inside the Fed9UVmeBase and below. It should be used when
     * passing fpga numbers to Fed9UVmeBase.
     */
    u8 getFirmwareFedFeUnit() const;
      
    //@}
    /** \name Setters */
    //@{

    /**
     * \brief  Allows the setting of up to the full range of address granularity from FE unit through FE unit channel, channel APV and finally APV strip.
     * \param  fedFeUnit FE unit address that is to be set. Range: 0-7.
     * \param  feUnitChannel FE unit channel that is to be addressed. Range: 0-11. Default UNSPECIFIED.
     * \param  channelApv APV on the channel that is to be addressed. Range: 0-1. Default UNSPECIFIED.
     * \param  apvStrip Strip number on the given APV to be addressed. Range:0-127. Default UNSPECIFIED.
     * \return Self reference
     *
     * Allow the address to be set at each granularity stage, but it is only required to be set to the FED FE unit level with the rest being left UNSPECIFIED.
     */
    Fed9UAddress& setAddress(u8 fedFeUnit, u8 feUnitChannel = UNSPECIFIED,
			  u8 channelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);

    /**
     * \brief  Sets the FED FPGA to be addressed in the internal numbering scheme.
     * \param  fedFpga FED FPGA number. Range:0-7, BACKEND, VME and BROADCAST.
     * \return Self reference.
     */
    Fed9UAddress& setFedFpga(u8 fedFpga);

    /**
     * \brief  Sets the FED FE unit to be addressed in the internal numbering scheme.
     * \param  value FED FE unit number. Range:0-7 and BROADCAST.
     * \return Self reference.
     */
    Fed9UAddress& setFedFeUnit(u8 value);

    /**
     * \brief  Sets the FED FE unit channel to be addressed in the internal numbering scheme.
     * \param  value FED FE unit channel number. Range:0-11.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitChannel(u8 value);
    
    // <NAC date=28/03/2007>  DelayChip
    /**
     * \brief  Sets the FED FE unit delay chip to be addressed.
     * \param  value FED FE unit delay chip number. Range:0-3.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitDelayChip(u8 value);
    
    /**
     * \brief  Sets the FED delay chip to be addressed.
     * \param  value FED delay chip number. Range:0-23.
     * \return Self reference.
     */
    Fed9UAddress& setFedDelayChip(u8 value);
    // </NAC>

    /**
     * \brief  Sets the channel APV to be addressed in the internal numbering scheme.
     * \param  value Channel APV number. Range:0-1.
     * \return Self reference.
     */
    Fed9UAddress& setChannelApv(u8 value);

    /**
     * \brief  Sets the APV strip to be addressed in the internal numbering scheme.
     * \param  value APV strip number. Range:0-127.
     * \return Self reference.
     */
    Fed9UAddress& setApvStrip(u8 value);

    /**
     * \brief  Sets the FED channel to be addressed in the internal numbering scheme.
     * \param  fedChannel FED channel number. Range:0-95.
     * \return Self reference.
     */
    Fed9UAddress& setFedChannel(u8 fedChannel);

    /**
     * \brief  Sets the FED APV to be addressed in the internal numbering scheme.
     * \param  fedApv FED APV number. Range:0-191.
     * \return Self reference.
     */
    Fed9UAddress& setFedApv(u8 fedApv);

    /**
     * \brief  Sets the FED strip to be addressed in the internal numbering scheme.
     * \param  fedStrip FED strip number. Range:0-24,576.
     * \return Self reference.
     */
    Fed9UAddress& setFedStrip(u16 fedStrip);

    /**
     * \brief  Sets the FE unit APV to be addressed in the internal numbering scheme.
     * \param  feUnitApv FE unit APV number. Range:0-47.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitApv(u8 feUnitApv);

    /**
     * \brief  Sets the FED channel pair to be addressed in the internal numbering scheme.
     * \param  fedChannelPair FED channel pair number. Range:0-47.
     * \return Self reference.
     */
    Fed9UAddress& setFedChannelPair(u8 fedChannelPair);
    /**
     * \brief  Sets the FE unit channel pair to be addressed in the internal numbering scheme.
     * \param  feUnitChannelPair FE unit channel pair number. Range:0-5.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitChannelPair(u8 feUnitChannelPair);

    /**
     * \brief  Sets the channel in the channel pair to be addressed in the internal numbering scheme.
     * \param  channel Channel in the channel pair number. Range:0-1.
     * \return Self reference.
     */
    Fed9UAddress& setChannelPairChannel(u8 channel);

    /**
     * \brief  Sets the channel from the channel pair to be addressed in the external numbering scheme.
     * \param  channel External channel in the channel pair. Range:1-2.
     * \return Self reference.
     */
    Fed9UAddress& setExternalChannelPairChannel(u8 channel);

    /**
     * \brief  Sets the FED channel pair to be addressed in the external numbering scheme.
     * \param  fedChannelPair FED channel pair number. Range:1-48.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedChannelPair(u8 fedChannelPair);

    /**
     * \brief  Sets the FE unit channel pair to be addressed in the external numbering scheme.
     * \param  feUnitChannelPair FE unit channel pair number. Range:1-6.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitChannelPair(u8 feUnitChannelPair);

    /**
     * \brief  Sets the FED FE unit to be addressed in the external numbering scheme.
     * \param  value FED FE unit number. Range:1-8.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedFeUnit(u8 value);

    /**
     * \brief  Sets the FE unit channel to be addressed in the external numbering scheme.
     * \param  value FE unit channel number. Range:1-12.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitChannel(u8 value);

    /**
     * \brief  Sets the channel APV to be addressed in the external numbering scheme.
     * \param  value Channel APV number. Range:1-2.
     * \return Self reference.
     */
    Fed9UAddress& setExternalChannelApv(u8 value);

    /**
     * \brief  Sets the APV strip to be addressed in the external numbering scheme.
     * \param  value APV strip number. Range:1-128.
     * \return Self reference.
     */
    Fed9UAddress& setExternalApvStrip(u8 value);

    /**
     * \brief  Sets the FED channel to be addressed in the external numbering scheme.
     * \param  value FED channel number. Range:1-92.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedChannel(u8 value); 

    /**
     * \brief  Sets the FED APV to be addressed in the external numbering scheme.
     * \param  value FED APV number. Range:1-192.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedApv(u16 value); 

    /**
     * \brief  Sets the FED strip to be addressed in the external numbering scheme.
     * \param  value FED strip number. Range:1-24,576.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedStrip(u16 value); 

    /**
     * \brief  Sets the FE unit APV to be addressed in the external numbering scheme.
     * \param  feUnitApv FE unit APV number. Range:1-24.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitApv(u8 feUnitApv);
    
    
    /** 
     * \brief  This method allows you to set the feUnit number based upon the feFirmware Address.
     * \param  feFpga FE FPGA number. Range:1-8.
     * \return Self reference.
     *
     * The FED firmware numbering runs from 1 throuh 8 and 15, rather than from 0 through 7 as in software internal
     * or 8 through 1 in external numbering. This is used inside the Fed9UVmeBase and below. It should be used when
     * passing FPGA numbers to Fed9UVmeBase.
     */
    Fed9UAddress& setFirmwareFedFeUnit(const u8 feFpga);

    //@}

  private:
    u8 _femod;          //!< Stores the FE unit number.
    u8 _fechan;         //!< Stores the FE channel number.
    u8 _apv;            //!< Stores the channel APV number.
    u8 _strip;          //!< Stores the APV strip number.
    bool _countFromTop; //!< Whether the first FE unit is counted from the top.
    u16 _countBase;     //!< 0 or 1 depending on the counting base.
    // <NAC date=28/03/2007>
    bool _broadcastDelayChip;	//!< Controls whether delay chip is calculate from channel number or set to 3 (broadcast to delaychips on one FeUnit)
    // <NAC>
  };

}

#endif // H_Fed9UChannelAddress
#ifndef H_Fed9UBeStatus
#define H_Fed9UBeStatus


#include <iostream>
#include <sstream>
#include <string>


namespace Fed9U {

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  Describes the contents of the Be Status Register.
   * \author Jonathan Fulcher
   *
   * It has methods to return the individual values or a string listing any errors that have occured.
   * It has no access to the actual FED registers and must be constructed using a previously read value
   * from the FED.
   */
  class Fed9UBeStatus {

  public:

    /**
     * \brief Constructor.
     * \param status An unsigned integer that has been read from the FED BE FPGA status register.
     */
    Fed9UBeStatus( u32 status): _status(status){}
    
    /**
     * \brief Destructor.
     */
    ~Fed9UBeStatus() {}
    

    /**
     * \brief  Returns a string which describes any errors which are set within the be status register
     * \return string The string containing the error list.
     */
    std::string getStatusString() const  {
      std::stringstream info;
      info << "Be Status Register Info: "
	   << ( getInternalFreeze()?"Internal Freeze! ":"" )
	   << ( getSLinkDown()?"S-link Link Down! ":"" )
	   << ( getSLinkFull()?"S-Link Link Full! ":"" )
	   << ( getBackPressure()?"Back Pressure Flag Enabled! ":"")
	   << ( getBxErrorAtOrbitReset()?"BX Error At Orbit Reset! ":"")
	   << ( getTtcReady()?"TTC Ready! ":"TTC Not Ready! ")
	   << ( getTrackerHeaderMonitorDataReady()?"Tracker Header Monitor Data Ready! ":"Tracker Header Monitor Data NOT Ready! ")
	   << ( getQdrFull()?"QDR Full! ":"")
	   << ( getFrameAddressFifoFull()?"Frame Address FIFO Full! ":"")
	   << ( getTotalLengthFifoFull()?"Total Length FIFO Full! ":"")
	   << ( getTrackerHeaderFifoFull()?"Tracker Header FIFO Full! ":"")
	   << ( getL1ABxFifoFull()?"L1A and BX FIFO Full! ":"")
	   << ( getFeEventLengthFifoFull()?"FE Event Length FIFO Full! ":"")
	   << ( getFeFpgaFull()?"Front End FPGA Full! ":"")
	   << ( getQdrPartialFull()?"QDR Partially Full! ":"")
	   << ( getFrameAddressFifoPartialFull()?"Frame Address Fifo Partially Full! ":"")
	   << ( getTotalLengthFifoPartialFull()?"Total Length Fifo Partially Full! ":"")
	   << ( getTrackerHeaderFifoPartialFull()?"Tracker Header FIFO Partially Full! ":"")
	   << ( getL1ABxFifoPartialFull()?"L1A & BX FIFO Partially Full! ":"")
	   << ( getFeEventLengthFifoPartialFull()?"FE Event Length FIFO Partially Full! ":"")
	   << ( getFeFpgaPartialFull()?"FE Fpga Partially Full! ":"")
	   << ( getQdrEmpty()?"QDR Empty! ":"")
	   << ( getFrameAddressFifoEmpty()?"Frame Address FIFO Empty! ":"" )
	   << ( getTotalLengthFifoEmpty()?"Total Length FIFO Empty! ":"") 
	   << ( getTrackerHeaderFifoEmpty()?"Tracker Header FIFO Empty! ":"")
	   << ( getL1ABxFifoEmpty()?"L1A & BX FIFO Empty! ":"")
	   << ( getFeEventLengthFifoEmpty()?"Fe Event Length FIFO Empty! ":"")
	   << ( getFeFpgaEmpty()?"FE FPGA Empty! ":"")
	   << std::endl;
      return info.str();
    }

    /**
     * \name Getters
     *
     * All return booleans and a boolean true indicates that the error condition is present and boolean false it is not.
     */
    //@{

    /**
     * \brief  When the FIFO becomes full the internal freeze is asserted to prevent over writing of event data.
     * \return bool Boolean true if the internal freeze is present, false if not.
     *
     * Once the internal freeze has been asserted the BE FPGA must be issued a soft reset command to reset of the counters
     */
    bool getInternalFreeze() const {
      return (_status >> 1) & 0x1;
    }

    /**
     * \brief  Status of the S-link connection.
     * \return bool Boolean true if the S-link is down, false if not.
     */
    bool getSLinkDown() const {
      return (_status >> 2) & 0x1;
    }

    /**
     * \brief  A status bit to indicate when the S-link event buffer is full.
     * \return bool Boolean true if the S-link buffer is full, false if not.
     */
    bool getSLinkFull() const {
      return (_status >> 3) & 0x1;
    }

    /**
     * \brief  When either the VME or S-link event readout buffers become full, the back pressure signal is asserted until that event fragment is readout.
     * \return bool Boolean true if back pressure is asserted, false if not.
     */
    bool getBackPressure() const {
      return (_status >> 4) & 0x1;
    }

    /**
     * \brief  Indicates if there was an error with the bunch crossing when an orbit reset was issued.
     * \return bool Boolean true if there an error is present, false if not.
     *
     * In order for this flag to operate correctly the bunch crossing offset must be set before the bunch crossing total is set.
     * This flag is clear once the bunch crossing total has been read.
     */
    bool getBxErrorAtOrbitReset() const {
      return (_status >> 5) & 0x1;
    }

    /**
     * \brief  Indicates the status of the TTC
     * \return bool Boolean true if the TTC is ready, false if not.
     */
    bool getTtcReady() const {
      return (_status >> 6) & 0x1;
    }

    /**
     * \brief  Monitors the construction of the tracker header, in order to know when it has been constructed correctly.
     * \return bool Boolean true if the header data is ready, false if not.
     */
    bool getTrackerHeaderMonitorDataReady() const {
      return (_status >> 7) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is full, false if not.
     */
    bool getQdrFull()  const {
      return (_status >> 8) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getFrameAddressFifoFull() const  {
      return (_status >> 9) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getTotalLengthFifoFull() const {
      return (_status >> 10) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getTrackerHeaderFifoFull() const {
      return (_status >> 11) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getL1ABxFifoFull() const {
      return (_status >> 12) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getFeEventLengthFifoFull() const {
      return (_status >> 13) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if a FE FPGA is full, false if not.
     */
    bool getFeFpgaFull() const {
      return (_status >> 14) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is partially (87%) full, false if not.
     */
    bool getQdrPartialFull() const {
      return (_status >> 16) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getFrameAddressFifoPartialFull() const {
      return (_status >> 17) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getTotalLengthFifoPartialFull() const {
      return (_status >> 18) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getTrackerHeaderFifoPartialFull() const {
      return (_status >> 19) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getL1ABxFifoPartialFull() const {
      return (_status >> 20) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getFeEventLengthFifoPartialFull() const {
      return (_status >> 21) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if the FE FPGA is partially (87%) full, false if not.
     */
    bool getFeFpgaPartialFull() const {
      return (_status >> 22) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is empty, false if not.
     */
    bool getQdrEmpty() const {
      return (_status >> 24) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getFrameAddressFifoEmpty() const {
      return (_status >> 25) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getTotalLengthFifoEmpty() const {
      return (_status >> 26) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getTrackerHeaderFifoEmpty() const {
      return (_status >> 27) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getL1ABxFifoEmpty() const {
      return (_status >> 28) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getFeEventLengthFifoEmpty() const {
      return (_status >> 29) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if the FE FPGA buffer is empty, false if not.
     */
    bool getFeFpgaEmpty() const {
      return (_status >> 30) & 0x1;
    }
    //@}

  private:
    u32 _status; //!< Holds the status word, from which the status bits are decoded.
  };

  //using.*std::istream;
  //using.*std::ostream;

  std::ostream& operator<<(std::ostream& os, const Fed9UBeStatus& sd);
  
}

#endif // H_Fed9UBeStatus
#ifndef H_Fed9UStripDescription
#define H_Fed9UStripDescription

#include "ICAssert.hh"

#include <iosfwd>

namespace Fed9U {

  /**
   * \brief  Describes a strip.
   * \author Jonanthan Fulcher, IC.
   *
   * Contains the pedestal, high and low cluster threshold, and the disable setting for a given strip. These values are what they FED requires
   * to perform its signal processing when in processed or zero suppressed (lite) mode. This class provides an abstraction from these base
   * values and instead uses a base lines noise setting to determine by what factor the threshold should be above this noise level.
   * It provides high and low threshold factors to store this information. It has two constructors, however the default constructor
   * should be used (with or without arguments) in preference as this contains the noise and threshold factors, rather than the threshold
   * values. The second constructor converted the high and low thresholds into noise and uses the default values FED_STRIP_HIT_THRESHOLD_HIGH
   * and FED_STRIP_HIT_THRESHOLD_LOW as defined in TypeDefs.hh.
   */
  class Fed9UStripDescription {
  public:

    /**\name Constructors and destructor.*/
    //@{

    /**
     * \brief Constructor.
     *
     * \param pedestal Pedestal value for that strip. Only valid during processed raw, zero suppressed and zero suppressed lite modes.
     * \param highThreshold High threshold value for that strip. Defines the signal hight of single strip clusters in the zero suppression mode.
     * \param lowThreshold Low threshold value for that strip. Defines the signal hight of multi-strip clusters in zero suppression mode.
     * \param disable True if the strip is disabled, false if enabled.
     *
     * Initialises the settings for strip given in the constructor parameter list and sets the noise to the high threshold divided by
     * FED_STRIP_HIT_THRESHOLD_HIGH as defined in TypeDefs.hh. It is recommended not to use this constructor and to use the default
     * one provided instead.
     */
    Fed9UStripDescription(i16 pedestal,
			  i16 highThreshold,
			  i16 lowThreshold,
			  bool disable
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(FED_STRIP_HIT_THRESHOLD_HIGH),
      _lowThresholdFactor(FED_STRIP_HIT_THRESHOLD_LOW),
      _noise( (static_cast<float>(highThreshold)/static_cast<float>(FED_STRIP_HIT_THRESHOLD_HIGH)<=51.1)?(static_cast<float>(highThreshold)/static_cast<float>(FED_STRIP_HIT_THRESHOLD_HIGH)):51.1),
      _disable(disable) {}
  
    /**
     * \brief Default constructor.
     *
     * \param pedestal Pedestal value for that strip. Default 0.
     * \param highThresholdFactor High threshold factor value for that strip. Defines the signal hight of single strip clusters in the zero suppression mode,
     *        relative to the noise level. Default FED_STRIP_HIT_THRESHOLD_HIGH, as defined in TypeDefs.hh.
     * \param lowThresholdFactor Low threshold factor value for that strip. Defines the signal hight of multi-strip clusters in zero suppression mode,
     *        relative to the noise level. Default FED_STRIP_HIT_THRESHOLD_LOW, as defined in TypeDefs.hh.
     * \param noise Default 0.
     * \param disable True if the strip is disabled, false if enabled. Default false.
     *
     * Can either be used as a default constructor as all parameters have sensible default values or used to construct all the strip settings.
     * The threshold factors are used to define the high and low thresholds required by the FEDs by setting them to a value of the factor times
     * the noise level.
     */
    Fed9UStripDescription(i16 pedestal = 0,
			  float highThresholdFactor = FED_STRIP_HIT_THRESHOLD_HIGH,
			  float lowThresholdFactor = FED_STRIP_HIT_THRESHOLD_LOW,
			  float noise = 0.0,
			  bool disable = false
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(highThresholdFactor),
      _lowThresholdFactor(lowThresholdFactor),
      _noise(noise<=51.1?noise:51.1),
      _disable(disable) {}

    /**
     * \brief Destructor.
     */   
    ~Fed9UStripDescription() {}
    //@}

    /**\name Getters */
    //@{

    /**
     * \brief  Returns the pedestal value for that strip.
     * \return signed long.
     */
    i16 getPedestal() const { return _pedestal; }

    /**
     * \brief  Returns the high threshold value for that strip.
     * \return signed short.
     */
    i16 getHighThreshold() const { return _noise==0?0xFF:(static_cast<i16>(static_cast<float>(_highThresholdFactor)*_noise+0.5 ) >0xFF?0xFF:static_cast<i16>(static_cast<float>(_highThresholdFactor)*_noise+0.5 )); }

    /**
     * \brief  Returns the low threshold value for that strip.
     * \return signed short.
     */
    i16 getLowThreshold() const { return _noise==0?0xFF:(static_cast<i16>(static_cast<float>(_lowThresholdFactor)*_noise+0.5) >0xFF?0xFF:static_cast<i16>(static_cast<float>(_lowThresholdFactor)*_noise+0.5)); }

    /**
     * \brief  Returns the high threshold factor for that strip.
     * \return float.
     */
    float getHighThresholdFactor() const { return _highThresholdFactor; }

    /**
     * \brief  Returns the low threshold factor for that strip.
     * \return float.
     */
    float getLowThresholdFactor() const { return _lowThresholdFactor; }

    /**
     * \brief  Returns the noise value for that strip.
     * \return float.
     */
    float getNoise() const { return _noise; }

    /**
     * \brief  Returns the disable settings for that strip.
     * \return boolean.
     */
    bool getDisable() const { return _disable; }
    //@}

    /** \name Setters */
    //@{
    /**
     * \brief Sets the pedestal value for that strip.
     * \param value Pedestal value to be set.
     */
    void setPedestal(i16 value) { _pedestal = value; }

    /**
     * \brief Sets the high threshold factor value for that strip.
     * \param value High threshold factor to be set.
     */
    void setHighThresholdFactor(float value) { _highThresholdFactor = value; }

    /**
     * \brief Sets the low threshold factor value for that strip.
     * \param value Low threshold factor for that strip.
     */
    void setLowThresholdFactor(float value) { _lowThresholdFactor = value; }

    /**
     * \brief Sets the high threshold value for that strip.
     * \param value High threshold to be set.
     */
    void setHighThreshold(i16 value) { 
      if( _noise > 0 )
	_highThresholdFactor = static_cast<float>(value)/_noise; 
      else 
	_highThresholdFactor = 0;
    }

    /**
     * \brief Sets the low threshold value for that strip.
     * \param value Low threshold for that strip.
     */
    void setLowThreshold(i16 value) {    
      if ( _noise > 0 )
	_lowThresholdFactor = static_cast<float>(value)/_noise; 
      else
	_lowThresholdFactor = 0;
    }
    
    /**
     * \brief Sets the noise level for that strip.
     * \param value Noise value to be set.
     */
    void setNoise(float value) { value <= 51.1? _noise = value: _noise=51.1; }

    /**
     * \brief Set whether this strip is disabled or not.
     * \param value True disables the strip, false enables it.
     */
    void setDisable(bool value) { _disable = value; }
    //@}

    /**
     * \brief Loads the strip settings from an input stream.
     * \param is Input stream to load the parameters from.
     */
    void load(std::istream& is);

    /**
     * \brief Writes the strip settings to an output stream.
     * \param os The output stream to write the parameters to.
     */
    void save(std::ostream& os) const;

  private:
    i16	_pedestal;              //!< Holds the value of the pedestal.
    float _highThresholdFactor; //!< Holds the value of the low threshold factor.
    float _lowThresholdFactor;  //!< Holds the value of the high threshold factor.
    float _noise;               //!< Holds the value of the noise.
    bool _disable;              //!< Holds the disable setting.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UStripDescription& l, const Fed9UStripDescription& r);
    // </NAC>
  };
  
  // <NAC date="24/04/2007"> operator to compare stripDescriptions
  /**
   * \brief  == operator to allow Fed9UStripDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UStripDescription& l, const Fed9UStripDescription& r);
  /**
   * \brief  != operator to allow Fed9UStripDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UStripDescription& l, const Fed9UStripDescription& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief  Overload of the operator<< which is used to write the strip settings to an output stream.
   * \param  os Output stream to write the settings to.
   * \param  sd Fed9UStripDescription settings that are to be written to the stream.
   * \return std::ostream Reference to the output stream the setting were written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UStripDescription& sd);

  /**
   * \brief  Overload of the operator>> which is used to read the strip settings from an input stream.
   * \param  is Input stream to read the settings from.
   * \param  sd Fed9UStripDescription settings that are to be written to the stream.
   * \return std::istream Reference to the input stream the setting were read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UStripDescription& sd);

}

#endif // H_Fed9UStripDescription
#ifndef H_Fed9UStrips
#define H_Fed9UStrips

#include <iosfwd>
#include <vector>


namespace Fed9U {

  /**
   * \brief  Description of all the strips in a FED.
   * \author Jonathan Fulcher
   *
   * Stores a Fed9UStripDescription for each APV strip in a FED.
   */

  class Fed9UStrips {
  public:

    /**
     * \Default constructor.
     *
     * Initialises each strip on the FED to have its default values.
     */
    Fed9UStrips();

    /**
     * \brief Destructor.
     */
    ~Fed9UStrips();

    /**
     * \brief  Returns a modifiable reference specific strip on a FED.
     * \param  fedStrip The FED strip that is to be retrieved.
     * \return Fed9UStripDescription&
     */
    Fed9UStripDescription& getStrip(Fed9UAddress fedStrip);

    /**
     * \brief  Returns a constant reference to a specific strip on a FED.
     * \param  fedStrip The FED strip that is to be retrieved.
     * \return Fed9UStripDescription&
     */
    const Fed9UStripDescription& getStrip(Fed9UAddress fedStrip) const;

    /**
     * \brief  Returns a vector containing a copy of all the strip descriptions on a given FED APV.
     * \param  fedApv Fed9UAddress class containing the address of the specific APV to be referenced.
     * \return vector<Fed9UStripDescription>
     */
    std::vector<Fed9UStripDescription> getApvStrips(Fed9UAddress fedApv) const;

    /**
     * \brief Sets the settings for a given strip on the FED.
     * \param fedStrip Address of the FED strip to be updated.
     * \param value Constant reference to the Fed9UStripDescription settings that are to be written to that strip.
     */
    void setStrip(Fed9UAddress fedStrip, const Fed9UStripDescription& value);

    /**
     * \brief  Sets all the strip descriptions on a given FED APV.
     * \param  fedApv Fed9UAddress class containing the address of the specific APV to be referenced.
     * \param  values Constant reference to a vector contain the settings for all the values on a given FED APV.
     */
    void setApvStrips(Fed9UAddress fedApv, const std::vector<Fed9UStripDescription> & values);

    /**
     * \brief Loads all the strip settings on a FED from an input stream.
     * \param is Reference to the input stream the settings are to be loaded from.
     */
    void loadStrips(std::istream& is);

    /**
     * \brief Saves all the strip settings on a FED to an input stream.
     * \param os Reference to the output stream where the settings are to be written to.
     */
    void saveStrips(std::ostream& os) const;

    /**
     * \brief Loads each strip one the FED with a default constructed Fed9UDescription object.
     */
    void loadDefaultStrips();

  private:
    //std::vector< std::vector< std::vector< std::vector<Fed9UStripDescription> > > > _strips;
    Fed9UStripDescription _strips[STRIPS_PER_FED];//!< Vector used to store a Fed9UStripDescription for each object on a FED.
    
  };
  
  // <NAC date="24/04/2007"> operator to compare strips
  /**
   * \brief  == operator to allow Fed9UStrips to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UStrips& l, const Fed9UStrips& r);
  /**
   * \brief  != operator to allow Fed9UStrips to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UStrips& l, const Fed9UStrips& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief  Overload of the operator>> which is used to read the strip settings from an input stream.
   * \param  is Input stream to read the settings from.
   * \param  fs Fed9UStrips that are to be read from the stream.
   * \return std::istream Reference to the input stream the setting were read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UStrips& fs);
  
  /**
   * \brief  Overload of the operator<< which is used to write the strip settings to an output stream.
   * \param  os Output stream to write the settings to.
   * \param  fs Fed9UStrips that are to be written to the stream.
   * \return std::ostream Reference to the output stream the setting were written to.
   */
  std::ostream& operator<<(std::ostream&, const Fed9UStrips&);

}

#endif // H_Fed9UStrips
#ifndef H_Fed9UTempControl
#define H_Fed9UTempControl

#include <iostream>

namespace Fed9U {

  /**
   * \brief A class that stores all the read write settings on an LM82 temperature monitor that monitor each FPGA on the FED.
   */
  class Fed9UTempControl {
  public:

    /**
     * \brief Copy constructor.
     * \param temp Fed9UTempControl object to be copied.
     *
     * Creates a new Fed9UTempControl object by copying the settings in an existing object.
     */
    Fed9UTempControl(const Fed9UTempControl& temp) : 
      _lm82High(temp.getLm82High()), _fpgaHigh(temp.getFpgaHigh()), _critical(temp.getCritical())
    { }

    /**
     * \brief Default constructor.
     * \param lm82High The is the temperature at which the LM82 will flag its own temperature as being too high. Default 127oC.
     * \param fpgaHigh The is the temperature at which the LM82 will flag the FPGA it is monitoring as its temperature as being too high. Default 127oC.
     * \param critical The temperature at which the LM82 will cause the FED to shut down to prevent either itself or an FPGA from over heating. Default 127oC.
     */
    Fed9UTempControl(u32 lm82High = 127, u32 fpgaHigh = 127, u32 critical = 127) :
      _lm82High(lm82High), _fpgaHigh(fpgaHigh), _critical(critical)
    { }

    /**
     * \brief  Returns the high temperature for the LM82 chip.
     * \return unsigned long.
     */
    u32 getLm82High() const { return _lm82High; }

    /**
     * \brief  Returns the high temperature for the FPGA chip that is being monitored.
     * \return unsigned long.
     */
    u32 getFpgaHigh() const { return _fpgaHigh; }

    /**
     * \brief  Returns the critical temperature for either the LM82 or its FPGA.
     * \return unsigned long.
     */
    u32 getCritical() const { return _critical; }

    /**
     * \brief  Sets the high point of the LM82 chip.
     * \param  lm82High High point of the LM82 to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setLm82High(u32 lm82High) { _lm82High = lm82High; return *this; }

    /**
     * \brief  Sets the high point of the FPGA being monitored.
     * \param  fpgaHigh High point of the FPGA being monitored to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setFpgaHigh(u32 fpgaHigh) { _fpgaHigh = fpgaHigh; return *this; }

    /**
     * \brief  Sets the critical temperature of the LM82 and FPGA.
     * \param  critical Critical temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setCritical(u32 critical) { _critical = critical; return *this; }

  private:
    u32 _lm82High, _fpgaHigh, _critical; //!< Private data members to store the class settings.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UTempControl& l, const Fed9UTempControl& r);
    // </NAC>
  };
  
  // <NAC date="24/04/2007"> operator to compare TempControls
  /**
   * \brief  == operator to allow Fed9UTempControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  inline bool operator == (const Fed9UTempControl& l, const Fed9UTempControl& r)
  {
    if (l._lm82High != r._lm82High) return false;
    if (l._fpgaHigh != r._fpgaHigh) return false;
    if (l._critical != r._critical) return false;
    return true;
  }
  /**
   * \brief  != operator to allow Fed9UTempControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTempControl& l, const Fed9UTempControl& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief A class that contains all the read/write and read only settings on an LM82 temperature monitor.
   *
   * Derived from the class Fed9UTempControl, it extends the interface to include the read only settings on the LM82 chip as well.
   */
  class Fed9UTempControlInfo : public Fed9UTempControl {
  public:

    /**
     * \brief Defuault constructor.
     *
     * Initialises the Fed9UTempControl settings to their default values and the LM82 and FPGA actual temperatures to zero, chip ID to zero
     * and its status to zero.
     */
    Fed9UTempControlInfo():Fed9UTempControl(),_lm82Actual(0), _fpgaActual(0), _id(0), _status(0)
    {}

    /**
     * \brief Constructor.
     * \param lm82High The is the temperature at which the LM82 will flag its own temperature as being too high.
     * \param fpgaHigh The is the temperature at which the LM82 will flag the FPGA it is monitoring as its temperature as being too high.
     * \param critical The temperature at which the LM82 will cause the FED to shut down to prevent either itself or an FPGA from over heating.
     * \param lm82Actual The actual temperature of the LM82 object.
     * \param fpgaActual The actual temperature of the monitored FPGA.
     * \param id ID of the LM82 temperature monitor.
     * \param status Status of the LM82 temperature monitor.
     *
     * This constructor initialises all the LM82 temperature monitor parameters explicitly.
     */
    Fed9UTempControlInfo(u32 lm82High, u32 fpgaHigh, u32 critical,
                         u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(lm82High, fpgaHigh, critical),
      _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }

    /**
     * \brief Constructor.
     * \param temp Fed9UTempControl object to initialise the base class through the copy constructor.
     * \param lm82Actual The actual temperature of the LM82 object.
     * \param fpgaActual The actual temperature of the monitored FPGA.
     * \param id ID of the LM82 temperature monitor.
     * \param status Status of the LM82 temperature monitor.
     */
    Fed9UTempControlInfo( const Fed9UTempControl& temp, u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(temp), _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }

    /**
     * \brief  Returns the actual temperature of the LM82 chip.
     * \return unsigned long.
     */
    u32 getLm82Actual() const { return _lm82Actual; }

    /**
     * \brief  Returns the actual temperature of the FPGA being monitored.
     * \return unsigned long.
     */
    u32 getFpgaActual() const { return _fpgaActual; }

    /**
     * \brief  Returns the ID of the LM82 chip.
     * \return unsigned long.
     */
    u32 getId() const { return _id; }

    /**
     * \brief  Returns the status of the LM82 chip. Can be decoded with the enum Fed9UTempMonitorStatus.
     * \return unsigned long.
     */
    u32 getStatus() const { return _status; }

    /**
     * \brief  Sets the actual temperature of the LM82.
     * \param  lm82Actual Temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setLm82Actual(u32 lm82Actual) { _lm82Actual = lm82Actual; return *this; }

    /**
     * \brief  Sets the actual temperature of the monitored FPGA.
     * \param  fpgaActual Temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setFpgaActual(u32 fpgaActual) { _fpgaActual = fpgaActual; return *this; }

    /**
     * \brief  Sets the ID of the LM82 the object refers to.
     * \param  id ID to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setId(u32 id) { _id = id; return *this; }

    /**
     * \brief  Sets the status of the LM82 chip.
     * \param  status Status value to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setStatus(u32 status) { _status = status; return *this; }

  private:
    u32 _lm82Actual, _fpgaActual, _id, _status; //!< Private data members to store the class settings.
    
  };

  /**
   * \brief  Overload of the operator<< which allows a Fed9UTempControl object to be written to an output stream.
   * \param  os The output stream to be written to.
   * \param  data Fed9UTempControl object to be written to file.
   * \return std::ostream Reference to the output stream that was written to.
   */
  inline std::ostream& operator << (std::ostream & os, const Fed9UTempControl& data) {
    return os << data.getLm82High() 
              << '\t' 
              << data.getFpgaHigh() 
              << '\t' << data.getCritical() 
              << '\n';
  }
  
  /**
   * \brief  Overload of the operator<< which allows a Fed9UTempControlInfo object to be written to an output stream.
   * \param  os The output stream to be written to.
   * \param  data Fed9UTempControlInfo object to be written to file.
   * \return std::ostream Reference to the output stream that was written to.
   */
  inline std::ostream& operator << (std::ostream & os, const Fed9UTempControlInfo& data) {
    return os //<< reinterpret_cast<const Fed9UTempControl&>(data) //removed this cause we don't really need to see the cut of temps and high temps. we only want to see the actual temps.
              << data.getLm82Actual() 
              << '\t' 
              << data.getFpgaActual()
              << '\t' 
              << data.getId() 
              << '\t' 
              << data.getStatus()
              << '\n';
  }
  
  /**
   * \brief  Overload of the operator>> which allows a Fed9UTempControl object to be read from an input stream.
   * \param  is The input stream to be read from.
   * \param  data Fed9UTempControl object to be written to file.
   * \return std::istream Reference to the output stream that was written to.
   */
  inline std::istream& operator >> (std::istream & is, Fed9UTempControl& data) {
    u32 temp;
    is >> temp;
    data.setLm82High(temp);
    is >> temp;
    data.setFpgaHigh(temp);
    is >> temp;
    data.setCritical(temp);
    return is;
  }
  
  /**
   * \brief  Overload of the operator>> which allows a Fed9UTempControlInfo object to be read from an input stream.
   * \param  is The input stream to be read from.
   * \param  data Fed9UTempControlInfo object to be written to file.
   * \return std::istream Reference to the output stream that was written to.
   */
  inline std::istream& operator >> (std::istream & is, Fed9UTempControlInfo& data) {
    u32 temp;
    is >> reinterpret_cast<Fed9UTempControl&>(data);
    is >> temp;
    data.setLm82Actual(temp);
    is >> temp;
    data.setFpgaActual(temp);
    is >> temp;
    data.setId(temp);
    is >> temp;
    data.setStatus(temp);
    return is;
  }
  
}
#endif // H_Fed9UTempControl
#ifndef H_Fed9UFrontEndDescription
#define H_Fed9UFrontEndDescription


#include <iosfwd>

namespace Fed9U {

  struct Fed9UAdcControls {
    //public:
    Fed9UAdcControls(bool dfsen = false, bool dfsval = false, bool s1 = false, bool s2 = false)
      : _dfsen(dfsen), _dfsval(dfsval), _s1(s1), _s2(s2) { }
    /**
     * \brief  Overloaded == operator to allow Fed9UAdcControls to be compared
     * \param  A LHS
     * \param  B RHS
     * \return bool, true or false for == comparison
     */
    // <NAC date="24/04/2007"> added const
    bool operator==(const Fed9UAdcControls& A) const;
    // </NAC>
    bool _dfsen;
    bool _dfsval;
    bool _s1;
    bool _s2;
  };


  /**
   * \brief  Description of a FED front-end module.
   * \author Jonathan Fulcher, IC.
   *
   * It contains all the settings that are associated with a FE module and forms part of the Fed9UDescription,
   * which stores the settings for the whole FED. It also provides streamers to allow the settings to be read/written
   * to/from a stream, typically a file.
   */
  class Fed9UFrontEndDescription {

  public:

    /**
     * \brief Default Constructor.
     *
     * \param fine Sets the fine delay that is applied to the FE unit channels. Range:0-24.
     * \param coarse Sets the coarse delay that is applied to the FE unt channels. Range:0-15.
     * \param trim Sets the trimDAC offset for each FE unit channel. Range:0-255.
     * \param opto Sets the input and output offset for the OptoRxs on the FE unit channels. Input offset range:0xf, output offset range:0-3.
     *             Input offset in first four bits, output in the 5th and 6th bits. Range:0-0x3f.
     * \param cap Sets the capacitor value for the OptoRxs on the FE unit channels. Range:0-3.
     * \param apvDisable Sets the APV disable flags for the FE unit APVs. Boolean true disabled, false enabled.
     * \param apvFakeEventDisable Set the fake event disable flag for the FE unit APVs. Boolean true disabled, false enabled.
     * \param median Sets the common mode median values for APVs on the FE unit APVs. Range:0-1023.
     * \param medianDisable Sets the common mode median disable flags on the FE unit APVs. Boolean true disabled, false enabled.
     * \param channelThreshold Sets the threshold value for the optical input signal on the ADCs. Range:0-992, steps 32.
     * \param channelBufferOccupancy Sets the occupancy level of the buffers storing the event buffers.
     * \param adcControls Sets the ADC control bits, using the Fed9UADCControls struct.
     * \param feUnitDisable Sets the FE unit disable flag. Boolean true disabled, false enabled.
     * \param tempControl Sets the temperature controls for that FE unit, using the Fed9UTempControl class.
     * \param complement Sets the complement incoming data flag. Boolean true disabled, false enabled.
     * \param fakeEventRandomSeed Sets the seed for the fake events random noise.
     * \param fakeEventRandomMask Sets the mask for the fake events.
     *
     * All the settings have been given default values, such that a default constructed object will have settings that a FED can
     * be run with. Further for values that can be unique for channels or APVs are all set to the same constructor passed value,
     * but can be updated through setters in the Fed9UDescription class. This class only provides a container for the settings and not access.
     */
    Fed9UFrontEndDescription(u16 fine = 0, 
			     u16 coarse = 0, 
			     u16 trim = 0, 
			     u16 opto = 0, 
			     u16 cap = 0, 
			     bool apvDisable = false,
			     //<GR date=27/07/2006> Added APV fake event disable
			     bool apvFakeEventDisable = false,
			     //</GR>
			     u16 median = 0, 
			     bool medianDisable = true,
			     u16 channelThreshold = 0,
			     u16 channelBufferOccupancy = 0,
			     const Fed9UAdcControls & adcControls = Fed9UAdcControls(false,false,false,false),
			     bool feUnitDisable = 0,
			     const Fed9UTempControl & tempControl = Fed9UTempControl(),
                             bool complement = true,
			     u16 fakeEventRandomSeed = 0,
			     u16 fakeEventRandomMask = 0
			     );

    /**
     * \brief Destructor.
     */
    ~Fed9UFrontEndDescription() {}

    u16 _fineDelay[CHANNELS_PER_FEUNIT];                //!< Array to contain the fine delay settings for the FE unit channels.
    u16 _coarseDelay[CHANNELS_PER_FEUNIT];              //!< Array to contain the coarse delay settings for the FE unit channels.
    u16 _trimDacOffset[CHANNELS_PER_FEUNIT];            //!< Array to contain the trimDAC offset settings for the FE unit channels.
    u16 _channelThreshold[CHANNELS_PER_FEUNIT];         //!< Array to contain the ADCs threshold settings for the FE unit channels.
    u16 _channelBufferOccupancy[CHANNELS_PER_FEUNIT];   //!< \todo - remove this and update the streamers and description version?
    Fed9UAdcControls _adcControls[CHANNELS_PER_FEUNIT/2]; //!< Array to contain the 6 ADC settings for the FE unit channel pairs. note the pairs are not the same pairs as with random seed and mask settings.
    bool _complement[CHANNELS_PER_FEUNIT];              //!< Array to contain the complement flag settings for the FE unit channels.
    
    bool _apvDisable[APVS_PER_FEUNIT];                  //!< Array to contain the APV disable flag settings for the FE unit APVs.
    //<GR date=27/07/2006> Added APV fake event disable
    bool _apvFakeEventDisable[APVS_PER_FEUNIT];         //!< Array to contain the APV fake event disable flag settings for the FE unit APVs.
    //</GR>
    u16 _optoRxOffset;                                  //!< Array to contain the OptoRx offset settings for the FE unit.
    u16 _optoRxCapacitor;                               //!< Array to contain the OptoRx capacitor settings for the FE unit.
    u16 _medianOverride[APVS_PER_FEUNIT];               //!< Array to contain the common mode median settings for the FE unit APVs.
    bool _medianOverrideDisable;                        //!< Array to contain the common mode median override disable flag settings for the FE unit.
    bool _feUnitDisable;                                //!< Array to contain the FE unit disable flag settings for the FE unit.
    Fed9UTempControl _tempControl;                      //!< Array to contain the temperature monitor control settings for the FE unit.
    u16 _fakeEventRandomSeed[CHANNELS_PER_FEUNIT/2];    //!< Array to contain the fake event random seed settings for the FE unit channel pairs.
    u16 _fakeEventRandomMask[CHANNELS_PER_FEUNIT/2];    //!< Array to contain the fake event random mask settings for the FE unit channel pairs.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r);
    // </NAC>
    
  };

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  Overloaded stream operator to allow Fed9UFrontEndDescriptions to be saved to a stream
   * \param  os Output stream to save the object to.
   * \param  desc Fed9UFrontEndDescription object to save to the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UFrontEndDescription& desc);

  /**
   * \brief  Overloaded stream operator to allow Fed9UFrontEndDescriptions to be read from a stream
   * \param  is Input stream to create an object from.
   * \param  desc Fed9UFrontEndDescription object to load from the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::istream& operator>>(std::istream& is, Fed9UFrontEndDescription& desc);

  /**
   * \brief  Overloaded stream operator to allow Fed9UAdcControls to be saved to a stream
   * \param  os Output stream to save the object to.
   * \param  cont Fed9UAdcControls object to save to the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UAdcControls& cont);

  /**
   * \brief  Overloaded stream operator to allow Fed9UAdcControlss to be read from a stream
   * \param  is Input stream to create an object from.
   * \param  cont Fed9UAdcControls object to load from the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::istream& operator>>(std::istream& is, Fed9UAdcControls& cont);
  
  // <NAC date="24/04/2007"> operator to compare front end descriptions
  /**
   * \brief  == operator to allow Fed9UFrontEndDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r);
  /**
   * \brief  != operator to allow Fed9UFrontEndDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r) { return !(l==r); }
  /**
   * \brief  != operator to allow Fed9UAdcControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UAdcControls& l, const Fed9UAdcControls& r) { return !(l==r); }
  // </NAC>
  
}

#endif // H_Fed9UFrontEndDescription
#ifndef H_Fed9UTtcrxDescription
#define H_Fed9UTtcrxDescription


#include <vector>

namespace Fed9U {

  /**
   * This structure holds a bool value for each type of reset that can be performed on the TTCrx.
   * Setting that value true will cause that reset to be performed, false and it will not.
   *
   * The errorCounters, bunchCount, eventCount and status variables will reset the error counter,
   * bunch counter, event counter and status registers respectivily.
   */
  struct Fed9UTtcrxCounterReset {
    Fed9UTtcrxCounterReset(bool errorCounters = false, bool bunchCount = false, bool eventCount = false, bool status = false) :
      _errorCounters(errorCounters), _bunchCount(bunchCount), _eventCount(eventCount), _status(status) {}
    bool _errorCounters;
    bool _bunchCount;
    bool _eventCount;
    bool _status;
  };// End of Fed9UTtcrxReset.

  /**
   * \brief  A class used to provide a software representation of the TTCrx chip.
   * \author Gareth Rogers, RAL.
   * \date   2004/03.
   *
   * The class provides a representation of the all the read and writable registers
   * on the TTCrx chip. More information about the TTCrx chip can be found in the
   * TTCrx Reference Manual: A Timing, Trigger and Control Receiver ASIC for LHC Detectors. 
   */
  class Fed9UTtcrxDescription {

    /**
     * The Fed9UTtcrxDescription public interface.
     */
  public:

    /**
     * \biref This constructor will create a Fed9UTtcrxDescription either with User defined values
     *        or the default values.
     *
     * \param counterReset Sets which counters are to be reset when the object is loaded into the FED TTCrx chip registers.
     * \param l1AcceptOneCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param brcstStrTwoCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param clockDesOneFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param clockDesTwoFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param dllPumpCurrent Sets the value of the pump current to be used in the delay lock loop chips. Range:1-7, default 2.
     * \param pllPumpCurrent Sets the value of the pupm current to be used in the delay lock loop chips. Range:1-7, default 3.
     * \param iacId Sets the individually addressed command ID. Range:0-16383, default 0.
     * \param i2cId Sets the I2C ID. Range:0-63, default 0.
     * \param counterOperation Set which counters are enabled. Default FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
     * \param disableHammingChecking Used to disable the hamming error checking. Default false.
     * \param selectClockTwo Used to select the second clock. Default false.
     * \param disableClockTwo Used to disable the second clock. Default true.
     * \param disableClockL1Accept Used to disable the level 1 accept signal. Default false.
     * \param disableParrallelBus Used to disable the parrallel bus. Default true.
     * \param disableSerialB Used to disable the serial B line. Default true.
     * \param disableNonDeskewedClock Disables the none deskewed clock. Default false.
     *
     * All the defaults are set to their hardware defaults, except the fine delay settings.
     */
    Fed9UTtcrxDescription(Fed9UTtcrxCounterReset counterReset = Fed9UTtcrxCounterReset(false,false,false,false),
			  u16 l1AcceptOneCoarseDelay = 0,
			  u16 brcstStrTwoCoarseDelay = 0,
			  u16 clockDesOneFineDelay = 0,
			  u16 clockDesTwoFineDelay = 0,
			  u16 dllPumpCurrent = 2,
			  u16 pllPumpCurrent = 3,
			  u16 iacId = 0,
			  u16 i2cId = 0,
			  u16 counterOperation = FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
			  bool disableHammingChecking  = false,
			  bool selectClockTwo  = false,
			  bool disableClockTwo = true,
			  bool disableClockL1Accept    = false,
			  bool disableParrallelBus     = true,
			  bool disableSerialB  = true,
			  bool disableNonDeskewedClock = false);

    /**
     * \brief  This is used to hold the information about which reset is required on the TTCrx.
     * \param  counterReset A Fed9UTtcrxCounterReset struct that determines which counters will be reset.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setCounterReset(const Fed9UTtcrxCounterReset& counterReset);

    /**
     * \brief  Returns a TTCrx reset structure which contains the settings for which resets
     *         are require on the TTCrx.
     * \return Fed9UTtcrxCounterReset. 
     */
    Fed9UTtcrxCounterReset getCounterReset() const;

    /**
     * \brief  This set the total amount of delay skew for the L1Accept, broadcast strobe 1
     *         signal, the bunch and event counter reset signals and the system broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * It sets the fine delay for either the 40MHz deskewed clock one or two depending on which the signals are
     * synchronised to. This is chosen by the either selecting or deselecting the deskew clock two.
     */
    Fed9UTtcrxDescription& setL1AcceptDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * \brief  This will set the total amount of delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * It sets the fine delay for either the 40MHz deskewed clock one or two depending on which
     * the signals are synchronised to, which can be selected using setDeskewedClockTwoSelected.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * \brief  This will set the coarse delay skew for the L1Accept, broadcast strobe 1 signal,
     *         bunch and event counter reset signals and the system broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setL1AcceptCoarseDelay(u16 coarseDelay);

    /**
     * \brief This will return the coarse delay skew for the deskewed 40MHz clock one.
     *        This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     *        event counter reset signals and the system broadcast command.
     * \return unsigned short.
     */
    u16 getL1AcceptCoarseDelay() const;

    /**
     * \brief  This will set the coarse delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoCoarseDelay(u16 coarseDelay);

    /**
     * \brief  This will set the coarse delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \return unsigned short.
     */
    u16 getBrcstStrTwoCoarseDelay() const;

    /**
     * \brief This will set the fine delay skew for the deskewed 40MHz clock one.
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if the second clock is deselected.
     */
    Fed9UTtcrxDescription& setClockDesOneFineDelay(u16 fineDelay);

   /**
     * \brief  This will return the fine delay skew for the deskewed 40MHz clock one.
     * \return unsigned short.
     *
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if the second deskewed clock is deselected.
     */
    u16 getClockDesOneFineDelay() const;

    /**
     * \brief  This will set the fine delay skew for the deskewed 40MHz clock two.
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled by passing a true to the
     * setDeskewedClock2Selected method.
     */
    Fed9UTtcrxDescription& setClockDesTwoFineDelay(u16 fineDelay);

    /**
     * \brief  This will return the fine delay skew for the deskewed 40MHz clock two.
     * \return unsigned short.
     *
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled. This can be checked using the method
     * getDeskewedClock2Selected. If a true is returned it is enabled.
     */
    u16 getClockDesTwoFineDelay() const;

    /**
     * \brief  Sets the charge pump current used in the delay-lock loops of the high
     *         resolution phase shifters.
     * \param  pumpCurrent Current to be set. Range:1-7.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setDllPumpCurrent(u16 pumpCurrent);

    /**
     * \brief  Returns the charge pump current used in the delay-lock loops of the high
     *         resolution phase shifters.
     * \return unsigned short.
     */
    u16 getDllPumpCurrent() const;

    /**
     * \brief  Sets the charge pump current used in the phase-lock loops which provide
     *         the clock recovery.
     * \param  pumpCurrent Pump current to use. Range:0-7.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setPllPumpCurrent(u16 pumpCurrent);

    /**
     * \brief  Returns the charge pump current used in the phase-lock loops which provide
     *         the clock recovery.
     * \return unsigned short.
     */
    u16 getPllPumpCurrent() const;

    /**
     * \brief  This sets the 14 bit individually addressed command (IAC) ID.
     * \param  id ID number the IAC is to be set to. Range:0-16383.
     * \return Self reference.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link.
     */
    Fed9UTtcrxDescription& setIacId(u16 id);

    /**
     * \brief  This returns the 14 bit individually addressed command (IAC) ID.
     * \return unsigned short.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link.
     */
    u16 getIacId() const;

    /**
     * \brief  This sets the 6 bit I2C ID.
     * \param  id I2C ID to be set. Range:0-63.
     * \return Self reference.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     */
    Fed9UTtcrxDescription& setI2cId(u16 id);

    /**
     * \brief  This returns the 6 bit I2C ID.
     * \return unsigned short.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     */
    u16 getI2cId() const;

    /**
     * \brief  This is used to control enable or disable the bunch and event counter operations.
     * \param  counterOpeation Counters that are to be enabled or disabled.
     * \return Self reference.
     *
     * The individual settings can be enabled or disabled using the enum
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh. Both flags can be set using a bitwise OR
     * and any flag not set will be disabled. The bunch and event counter settings are used in
     * conjuction to specify the mode of the level 1 trigger sequence. For more information see
     * the TTCrx Reference Manual chapter 6.
     */
    Fed9UTtcrxDescription& setCounterOperation(u16 counterOperation);

    /**
     * \brief  Returns the enable or disable flags for the bunch and event counter operations.
     * \return unsigned short.
     *
     * The individual settings can be checked by using the masks provided in Fed9UTtcrxCtrOperation.
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6. Disabling these can be used to save power.
     */
    u16 getCounterOperation() const;

    /**
     * \brief  This can be used to enable or disable the hamming error checking.
     * \param  disableHammingChecking True disables the checking, false will enable it.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setHammingCheckingDisable(bool disableHammingChecking);

    /**
     * \brief  Returns the enable state of the hamming error checking.
     * \return Boolean.
     */
    bool getHammingCheckingDisable() const;

    /**
     * \brief  This selects the 40MHz deskewed clock two. If selected it is used to sychronise
     *         the broadcast strobe 2 signal and the User broadcast command.
     * \param  selectClockTwo True selects the clock, false deselects it.
     * \return Self reference.
     *
     * This relates to which signals will have a fine skew applied to them, depending on whether they are
     * synchronised with clock two.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Selected(bool selectClockTwo);

    /**
     * \brief  Returns the selected state of the deskewed clock two.
     * \return boolean.
     *
     * If a true is returned then the broadcast strobe 2 signal and the user broadcast
     * command are synchronised to the 40MHz deskewed clock two.
     */
    bool getDeskewedClock2Selected() const;

    /**
     * \brief  Used to disable the second deskewed clock.
     * \param disableClockTwo If a true is passed it will be disabled, false enabled.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Disable(bool disableClockTwo);

    /**
     * \brief  Returns the disabled state of the deskewed clock two.
     * \return Boolean.
     */
    bool getDeskewedClock2Disable() const;

    /**
     * \brief  Disables the L1Accept clock output.
     * \param  l1AcceptDisable True will disable the output, false enable it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setClockL1AcceptDisable(bool l1AcceptDisable);

    /**
     * \brief  Returns the setting on the L1Accept clock disable flag.
     * \return Self reference.
     */
    bool getClockL1AcceptDisable() const;

    /**
     * \brief  Used to disable the non-deskewed 40MHz clock output.
     * \param  disable True will disable the clock, false enable it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setNonDeskewedClockDisable(bool disable);

    /**
     * \brief  Returns the disable state of the non-deskewed 40MHz clock.
     * \return Boolean.
     */
    bool getNonDeskewedClockDisable() const;

    /**
     * \brief  Used to disable the parrallel bus output.
     * \param  disable True disables the output, false enables it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setParrallelOutputDisable(bool disable);

    /**
     * \brief  Returns the disable state of the parrallel bus output.
     * \return Boolean.
     */
    bool getParrallelOutputDisable() const;

    /**
     * \brief  Sets the disable flag for the serial B output.
     * \param  disable True disables the output, false enables it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setSerialBDisable(bool disable);

    /**
     * \brief  Returns the disable status of the serial B output.
     * \return Boolean.
     */
    bool getSerialBDisable() const;

    /**
     * Some private data members to define the state of the TTCrx.
     */
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    static const unsigned short CONTROL_REGISTER_SIZE = 8;
    // </NAC>
  private:
    Fed9UTtcrxCounterReset _counterReset;
    u16 _l1AcceptCoarseDelay, _brcstStrTwoCoarseDelay, _clockDesOneFineDelay, _clockDesTwoFineDelay;
    u16 _dllPumpCurrent, _pllPumpCurrent, _iacId, _i2cId;
    u16 _counterOperation;
    bool _disableHammingChecking, _selectClockTwo, _disableClockTwo, _disableClockL1Accept;
    bool _disableParrallelBus, _disableSerialB, _disableNonDeskewedClock;
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    //std::vector<bool> _controlRegister;
    bool _controlRegister[CONTROL_REGISTER_SIZE];
    // </NAC>
    
    
    // <NAC date="24/04/2007">
    friend  bool operator == (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r);
    // </NAC>

  }; //End of class Fed9UTtcrxDescription declaration.

  /**
   * \brief  This overload of operator<< will allow the Fed9UTtcrxDescription to be written to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UTtcrxDescription class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UTtcrxDescription& data);

  /**
   * \brief  This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UTtcrxDescription class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescription& data);
  
  // <NAC date="24/04/2007"> operator to compare Ttcrx descriptions
  /**
   * \brief  == operator to allow Fed9UTtcrxDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r);
  /**
   * \brief  != operator to allow Fed9UTtcrxDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r) { return !(l==r); }
    /**
   * \brief  == operator to allow Fed9UTtcrxCounterResets to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UTtcrxCounterReset& l, const Fed9UTtcrxCounterReset& r);
  /**
   * \brief  != operator to allow Fed9UTtcrxCounterResets to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTtcrxCounterReset& l, const Fed9UTtcrxCounterReset& r) { return !(l==r); }
  // </NAC>

}      //End of Fed9U namespace.
#endif //H_Fed9UTtcrxDescription
#ifndef H_Fed9UTtcrxDescriptionInfo
#define H_Fed9UTtcrxDescriptionInfo


#include <vector>

namespace Fed9U {

/**
 * \brief  A class used to provide a software representation of the TTCrx chip.
 * \author Gareth Rogers.
 * \date   2004/03
 *
 * This is a derived class from the Fed9UTtcrxDescription. It contains in addition
 * to the read and writable settings the read only settings. The purpose of this
 * class is to read back settings from the TTCrx. It can be used to configure the
 * TTCrx as it is a derived class of Fed9UTtcrxDescription, however the methods
 * provided in this class can only hold read only values and any information set by
 * the user will not be written to the TTCrx.
 */

  class Fed9UTtcrxDescriptionInfo : public Fed9UTtcrxDescription {

  public:

    /**
     * \brief Default constructor.
     * \param counterReset Sets which counters are to be reset when the object is loaded into the FED TTCrx chip registers.
     * \param l1AcceptOneCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param brcstStrTwoCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param clockDesOneFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param clockDesTwoFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param dllPumpCurrent Sets the value of the pump current to be used in the delay lock loop chips. Range:1-7, default 2.
     * \param pllPumpCurrent Sets the value of the pupm current to be used in the delay lock loop chips. Range:1-7, default 3.
     * \param iacId Sets the individually addressed command ID. Range:0-16383, default 0.
     * \param i2cId Sets the I2C ID. Range:0-63, default 0.
     * \param counterOperation Set which counters are enabled. Default FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
     * \param disableHammingChecking Used to disable the hamming error checking. Default false.
     * \param selectClockTwo Used to select the second clock. Default false.
     * \param disableClockTwo Used to disable the second clock. Default true.
     * \param disableClockL1Accept Used to disable the level 1 accept signal. Default false.
     * \param disableParrallelBus Used to disable the parrallel bus. Default true.
     * \param disableSerialB Used to disable the serial B line. Default true.
     * \param disableNonDeskewedClock Disables the none deskewed clock. Default false.
     * \param bunchCount Number of bunch that have occured since the last reset. Range:0-4095, default 0.
     * \param eventCount Number of event that have been triggered since the last reset. Range:0-16777215, default 0.
     * \param singleBitErrorCount Number of errors that have occurred to single bits in the transmission since the last reset.
     *        Range:0-65535, default 0.
     * \param doubleBitErrorCount Number of errors that have occurred to two consequtive bits in the transmission since the last reset.
     *        Range:0-255, default 0.
     * \param seuErrorCount Number of single event upsets that the TTCrx corrected for since the last reset. Range:0-255, default 0.
     * \param masterModeA Determines the mode of running of channel A. Range:0-2, default 0.
     * \param masterModeB Determines the mode of running of channel B. Range:0-2, default 0.
     * \param testOutputs Value of the TTCrx test register used for debugging the chip. Range:0-7, default 5.
     * \param numbClockCycles Determines the number of clock cycles after frequency phase detection will stop. Range:0-7, default 7.
     * \param configSettings Configuration register that contains some of the debug settings for the TTCrx. Set using Fed9UTtcrxConfig enum.
     *        Range:0-255, default 66.
     * \param statusRegister Status register that contains the settings on the TTCrx. Set using Fed9UTtcrxStatus enum. Range:0-15, default 0.
     *
     * Allows a either completely initialised object to be created. Alternatively a the default values can be used
     * and a blank object created and used.
     */
    Fed9UTtcrxDescriptionInfo(Fed9UTtcrxCounterReset counterReset = Fed9UTtcrxCounterReset(false, false, false, false),
			      u16 clockDesOneCoarseDelay = 0,
			      u16 clockDesOneFineDelay   = 0,
			      u16 clockDesTwoCoarseDelay = 0,
			      u16 clockDesTwoFineDelay   = 0,
			      u16 dllPumpCurrent = 2,
			      u16 pllPumpCurrent = 3,
			      u16 iacId = 0,
			      u16 i2cId = 0,
			      u16 counterOperation = FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
			      bool disableHammingChecking  = false,
			      bool selectClockTwo  = false,
			      bool disableClockTwo = true,
			      bool disbaleClockL1Accept    = false,
			      bool disableParrallelBus = true,
			      bool disableSerialB      = true,
			      bool disableNonDeskewedClock = false,
			      u16 bunchCount = 0,
			      u32 eventCount = 0,
			      u16 singleBitErrorCount = 0,
			      u16 doubleBitErrorCount = 0,
			      u16 seuErrorCount = 0,
			      u16 masterModeA = 0,
			      u16 masterModeB = 0,
			      u16 testOutputs = 5,
			      u16 numbClockCycles = 7,
			      u16 configSettings  = 66,
			      u16 statusRegister  = 0);

    /**
     * \brief  This sets the number of bunch crossing that have occured since the last reset.
     * \param  bunchCount Number of bunch crossing to be set. Range:0-4095.
     * \return Self reference.
     *
     * It counts the number of clock signals that are received by the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setBunchCounter(u16 bunchCount);

    /**
     * \brief  Returns the number of bunch crossing that have occured since the last reset.
     * \return unsigned short.
     *
     * It counts the number of clock signals that are received by the TTCrx.
     */
    u16 getBunchCounter() const;

    /**
     * \brief  This sets the number of events that have been received since the last reset.
     * \param  eventCount Number of events to be set.
     * \return Self reference.
     *
     * It counts the number of trigger signals that are received by the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setEventCounter(u32 eventCount);

    /**
     * \brief  Returns the number of events that have been received since the last reset.
     * \return unsigned long.
     *
     * It counts the number of trigger signals that are received by the TTCrx.
     */
    u32 getEventCounter() const;

    /**
     * \brief  This stores the number of single bit errors that are recignised and corrected by the
     *         receiver's Hamming decoder.
     * \param  singleBitErrorCount Error count to be set. Range:0-65355.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setSingleBitErrorCount(u16 singleBitErrorCount);

    /**
     * \brief  Returns the number of single bit errors that are recignised and corrected by the
     *         receiver's Hamming decoder.
     * \return unsigned short.
     */
    u16 getSingleBitErrorCount() const;

    /**
     * \brief  Stores the number of double bit Hamming and frame errors that the TTCrx recognises.
     * \param  doubleBitErrorCount Number of double bit errors to be set. Range:0-255.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setDoubleBitErrorCount(u16 doubleBitErrorCount);

    /**
     * \brief  Returns the number of double bit Hamming and frame errors.
     * \return unsigned short.
     */
    u16 getDoubleBitErrorCount() const;

    /**
     * \brief  Stores the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * \param  seuErrorCount Number of SEU to be set. Range:0-255.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setSeuErrorCount(u16 seuErrorCount);

    /**
     * \brief  Returns the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * \return unsigned short.
     */
    u16 getSeuErrorCount() const;

    /**
     * \brief  This determines the overall operating mode of the TTCrx.
     * \param  masterMode Master mode of channel A to be set. Range:0-2.
     * \return Self reference.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * A. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeA(u16 masterMode);

    /**
     * \brief  Returns the overall operating mode of the TTCrx.
     * \return unsigned short.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * A. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeA() const;

    /**
     * \brief  This determines the overall operating mode of the TTCrx.
     * \param  masterMode Master mode of channel B to be set. Range:0-2.
     * \return Self reference.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeB(u16 masterMode);

    /**
     * \brief  Returns the overall operating mode of the TTCrx.
     * \return unsigned short.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeB() const;

    /**
     * \brief  This is a read only register, which selects the test outputs.
     * \param  testOutputs Value of the test outputs to set. Range:0-7.
     * \return Self reference.
     *
     * This register is used for testing and debugging the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setTestOutputs(u16 testOutputs);

    /**
     * \brief  Returns the settings for the test outputs.
     * \return unsigned short.
     */
    u16 getTestOutputs() const;

    /**
     * \brief  This is a read only register, which selects the number of clock
     *         cycles after which the frequency phase detection will stop.
     * \param  numbClockCycles Sets the number of clock cycles to stop frequency phase detection.
     * \return Self reference.
     *
     * It is a testing and debugging register.
     */
    Fed9UTtcrxDescription& setFreqDetectCount(u16 numbClockCycles);

    /**
     * \brief  Returns the number of clock cycles after which the frequency phase detection will stop.
     * \return unsigned short.
     *
     * It is a testing and debugging register.
     */
    u16 getFreqDetectCount() const;

    /**
     * \brief  This is a read only configuration register. It contains some testing and debugging settings.
     * \param  configSettings Sets the configuration register settings.
     * \return Self reference.
     *
     * The enum Fed9UTtcrxConfig can be used to set these bits. As these are read only on the TTCrx,
     * what is set should correspond to what is on the FED and the defauls given in the enum documentation.
     * Any setting not set will be cleared.
     */
    Fed9UTtcrxDescription& setConfigSettings(u16 configSettings);

    /**
     * \brief  Returns the settings on the configuration register. They can be decoded using the Fed9UTtcrxConfig enum.
     * \return unsigned short.
     */
    u16 getConfigSettings() const;

    /**
     * \brief  The current status of the internal monitoring signals.
     * \param  statusRegister Value of the status register set using the Fed9UTtcrxStatus enum.
     * \return Self reference.
     *
     * The flags are set using the Fed9UTtcrxStatus enum. Multiple flags can be use by using a
     * a bitwise OR. Any flag not set will be cleared.
     */
    Fed9UTtcrxDescriptionInfo& setStatusRegister(u16 statusRegister);

    /**
     * \brief  Returns the status of of internal monitoring signals.
     * \return unsigned short.
     *
     * The returned unsigned short can be masked with the Fed9UTtcrxStatus enum to check for set flags
     * and the meaning of each flag is given with the Fed9UTtcrxStatus enum documentation.
     */
    u16 getStatusRegister() const;

    /**
     * Some private data members to define the state of the TTCrx.
     */
  private:
    u16 _bunchCount;
    u32 _eventCount;
    u16 _singleBitErrorCount, _doubleBitErrorCount, _seuErrorCount, _masterModeA, _masterModeB;
    u16 _testOutputs, _numbClockCycles, _configSettings, _statusRegister;
  };//End of class Fed9UTtcrxDescriptionInfo declaration.

  /**
   * \brief  This overload of operator<< will allow the Fed9UTtcrxDescriptionInfo to be written to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UTtcrxDescriptionInfo class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UTtcrxDescriptionInfo& data);

  /**
   * \brief  This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UTtcrxDescriptionInfo class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescriptionInfo& data);

}      //End of Fed9U namespace
#endif //H_Fed9UTtcrxDescriptionInfo
#ifndef H_Fed9UVoltageControl
#define H_Fed9UVoltageControl


#include <vector>

namespace Fed9U {

  /**
   * \brief  A class to provide a software representation of the ADM1025 voltage monitor.
   * \author Gareth Rogers, RAL.
   * \date   2004/04.
   *
   * This class can be used to configure the read and write registers on the ADM1025
   * voltage montior. It can monitor a 2.5V, the processor core voltage, 3.3V, 12V and its supply voltage as well
   * as it own and an external temperature.  All voltages are accurate to 3 decimal places.
   */
  class Fed9UVoltageControl {

  public:

    /**
     * \brief Default constructor.
     *
     * \param standby Set the voltage monitor into a stand by mode where no values are read or monitored. Default false.
     * \param resetStatusRegister A true will reset the voltage monitor when the settings are loaded. Default false.
     * \param max2Point5Volt Maximum value the 2.5V line can reach before it is flagged. Range:0-3.320, default 3.320.
     * \param min2Point5Volt Minium value the 2.5V line can drop to before it is flagged. Range:0-3.320, default 0.
     * \param max3Point3Volt Maximum value the 3.3V line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param min3Point3Volt Minium value the 3.3V line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param max5Volt Maximum value the 5V line can reach before it is flagged. Range:0-6.640, default 6.640.
     * \param min5Volt Minium value the 5V line can drop to before it is flagged. Range:0-6.640, default 0.
     * \param max12Volt Maximum value the 12V line can reach before it is flagged. Range:0-15.938, default 15.938.
     * \param min12Volt Minium value the 12V line can drop to before it is flagged. Range:0-15.938, default 0.
     * \param maxCoreVoltage Maximum value the core voltage line can reach before it is flagged. Range:0-2.988, default 2.988.
     * \param minCoreVoltage Minium value the core voltage line can drop to before it is flagged. Range:0-2.988, default 0.
     * \param maxSupplyVoltage Maximum value the supply voltage line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param minSupplyVoltage Minium value the supply voltage line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param maxExternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minExternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default -128.
     * \param maxInternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minInternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default 0.
     * \param tempOffset Offset that is to be applied to the internal temperature readings., default 0.
     * \param offsetInternalTemp Whether the internal or external temperature offset value is to be added to the temperature readings. Default true.
     *
     * A Fed9UVoltageControl object can be default constructed. If this is the case then all limits
     * will be set to their maximum values and no resets will be flagged.
     */
    Fed9UVoltageControl(bool standBy = false,
			bool resetStatusRegister    = false,
			float max2Point5Volt  = 3.320,
			float min2Point5Volt  = 0,
			float max3Point3Volt  = 4.383,
			float min3Point3Volt  = 0,
			float max5Volt        = 6.640,
			float min5Volt        = 0,
			float max12Volt       = 15.938,
			float min12Volt       = 0,
			float maxCoreVoltage  = 2.988,
			float minCoreVoltage  = 0,
			float maxSupplyVoltage = 4.383,
			float minSupplyVoltage = 0,
			i16 maxExternalTemp    = 127,
			i16 minExternalTemp    = -128,
			i16 maxInternalTemp    = 127,
			i16 minInternalTemp    = 0,
			i16 tempOffset = 0,
			bool offsetInternalTemp = true);

    /**
     * \brief  This can be used to place the voltage controller in a stand by mode which does not perform
     *         any measurements or conversion and can be used to save power when the voltage controller is
     *         not needed.
     * \param  standBy True puts the voltage monitor into standby, false takes it out.
     * \return Self reference.
     *
     * This will not reset the chip, all register values will be kept.
     */
    Fed9UVoltageControl& setStandBy(bool standBy);

    /**
     * \brief  Returns the running state of the voltage controller, in standby or monitoring.
     * \return Boolean
     *
     * This will not reset the chip, all register values will be kept.
     */
    bool getStandBy() const;

    /**
     * \brief  This provides the ability to reset the status registers to their hardware defaults.
     * \param  reset Passing a true will flag the status register for a reset, a false will not perform a reset.
     * \return Self reference.
     */
    Fed9UVoltageControl& setResetStatusRegister(bool reset);

    /**
     * \brief  Returns the state of the reset status register flag.
     * \return Boolean.
     */
    bool getResetStatusRegister() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 2.5V reading.
     * \param  voltage Upper limit to be set. Range:0-3.320V.
     * \return Self reference.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageControl& set2Point5VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 2.5V reading.
     * \return float.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     */
    float get2Point5VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 2.5V reading.
     * \param  voltage Limit to be set. Range:0-3.320V.
     * \return Self reference.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set2Point5VoltMin(float voltage);

    /**
     * Returns the lower limit warning voltage for the 2.5V reading.
     * \return float.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     */
    float get2Point5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 3.3V reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set3Point3VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 3.3V reading.
     * \return float.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     */
    float get3Point3VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 3.3V reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set3Point3VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 3.3V reading.
     * \return float.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     */
    float get3Point3VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 5V reading.
     * \param  voltage Limit to be set. Range:0-6.640V.
     * \return Self reference.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set5VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 5V reading.
     * \return float.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     */
    float get5VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 5V reading.
     * \param  voltage Limit to be set. Range:0-6.640V.
     * \return Self reference.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set5VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 5V reading.
     * \return float.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     */
    float get5VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 12V reading.
     * \param  voltage Limit to be set. Range:0-15.938V.
     * \return Self reference.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set12VoltMax(float voltage);

    /**
     * Returns the upper limit warning voltage for the 12V reading.
     * \return float.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     */
    float get12VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 12V reading.
     * \param  voltage Limit to be set. Range:0-15.938V.
     * \return Self reference.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set12VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 12V reading.
     * \return float.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     */
    float get12VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the core voltage reading.
     * \param  voltage Limit to be set. Range:0-2.988V.
     * \return Self reference.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setCoreVoltageMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the core voltage reading.
     * \return float.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     */
    float getCoreVoltageMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the core voltage reading.
     * \param  voltage Limit to be set. Range:0-2.988V.
     * \return Self reference.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setCoreVoltageMin(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the core voltage reading.
     * \return float.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     */
    float getCoreVoltageMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the supply voltage reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setSupplyVoltageMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the supply voltage reading.
     * \return float.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    float getSupplyVoltageMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the supply voltage reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setSupplyVoltageMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the supply voltage reading.
     * \return float.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    float getSupplyVoltageMin() const;

    /**
     * \brief  This sets the upper limit warning temperature for the external temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127oC.
     * \return Self reference.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setExternalTempMax(i16 temperature);

    /**
     * \brief  Returns the upper limit warning temperature for the external temperature reading.
     * \return signed long.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMax() const;

    /**
     * \brief  This sets the lower limit warning temperature for the external temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127oC.
     * \return Self reference.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setExternalTempMin(i16 temperature);

    /**
     * \brief  Returns the lower limit warning temperature for the external temperature reading.
     * \return signed long.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     */
    i16 getExternalTempMin() const;

    /**
     * \brief  This sets the upper limit warning temperature for the internal temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127.
     * \return Self reference.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setInternalTempMax(i16 temperature);

    /**
     * \brief  Returns the upper limit warning temperature for the internal temperature reading.
     * \return signed long.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     */
    i16 getInternalTempMax() const;

    /**
     * \brief  This sets the lower limit warning temperature for the internal temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127.
     * \return Self reference.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setInternalTempMin(i16 temperature);

    /**
     * \brief  Returns the lower limit warning temperature for the internal temperature reading.
     * \return signed long.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     */
    i16 getInternalTempMin() const;

    /**
     * \brief  The temperature measurement can have an offset added to the measured value. The offset can be
     *         added to either the external or the internal offset, but not to both.
     * \param  temperature Offset to be set. Range:-128 to 127.
     * \param  internal True offset is added to the internal measurement, false the external.
     * \return Self reference.
     */
    Fed9UVoltageControl& setTempOffset(i16 offset, bool internal);

    /**
     * \brief  Returns the offset that is added to either the internal or external temperature measurements.
     * \return signed long.
     */
    i16 getTempOffset() const;

    /**
     * \brief  Returns true if the offset is added to either the internal or false if added to the external temperature measurements.
     * \return Boolean.
     */
    bool getOffsetTempSelect() const;

    /**
     * Some private data members used to store the state of a Fed9UVoltageControl object.
     */
  private:
    bool standBy_, resetStatusRegister_;
    float max2Point5Volt_, min2Point5Volt_, max3Point3Volt_, min3Point3Volt_;
    float max5Volt_, min5Volt_, max12Volt_, min12Volt_, maxCoreVoltage_, minCoreVoltage_;
    float maxSupplyVoltage_, minSupplyVoltage_;
    i16 maxExternalTemp_, minExternalTemp_, maxInternalTemp_, minInternalTemp_, offset_;
    bool internal_;
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r);
    // </NAC>

  };// End of class Fed9UVoltageControl
  
  // <NAC date="24/04/2007"> operator to compare Volatge Monitors
  /**
   * \brief  == operator to allow Fed9UVoltageControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r);
  /**
   * \brief  != operator to allow Fed9UVoltageControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r) { return !(l==r); }
  // </NAC>

  /**
   * \brief  This overload of operator<< can used to write a Fed9UVoltageControl to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UVoltageControl class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UVoltageControl& data);

  /**
   * \brief  This overload of operator>> can used to read a Fed9UVoltageControl from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UVoltageControl class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream & is, Fed9UVoltageControl& data);
  
}// End of Fed9U namespace
#endif// H_Fed9UVoltageControl
#ifndef H_Fed9UVoltageControlInfo
#define H_Fed9UVoltageControlInfo


#include <vector>

namespace Fed9U {

  /**
   * \brief  A class to provide a software representation of the ADM1025 voltage monitor.
   * \author Gareth Rogers, RAL.
   * \date   2004/04.
   *
   * This class is designed to read back all the read only settings on the voltage
   * monitor. It inherits from Fed9UVoltageControl, so it can also read the read
   * write settings on the ADM1025 voltage controller. All voltages are accurate
   * to 3 decimal places.
   */
  class Fed9UVoltageControlInfo : public Fed9UVoltageControl {

  public:

    /**
     * \brief Constructor.
     *
     * \param standby Set the voltage monitor into a stand by mode where no values are read or monitored. Default false.
     * \param resetStatusRegister A true will reset the voltage monitor when the settings are loaded. Default false.
     * \param max2Point5Volt Maximum value the 2.5V line can reach before it is flagged. Range:0-3.320, default 3.320.
     * \param min2Point5Volt Minium value the 2.5V line can drop to before it is flagged. Range:0-3.320, default 0.
     * \param max3Point3Volt Maximum value the 3.3V line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param min3Point3Volt Minium value the 3.3V line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param max5Volt Maximum value the 5V line can reach before it is flagged. Range:0-6.640, default 6.640.
     * \param min5Volt Minium value the 5V line can drop to before it is flagged. Range:0-6.640, default 0.
     * \param max12Volt Maximum value the 12V line can reach before it is flagged. Range:0-15.938, default 15.938.
     * \param min12Volt Minium value the 12V line can drop to before it is flagged. Range:0-15.938, default 0.
     * \param maxCoreVoltage Maximum value the core voltage line can reach before it is flagged. Range:0-2.988, default 2.988.
     * \param minCoreVoltage Minium value the core voltage line can drop to before it is flagged. Range:0-2.988, default 0.
     * \param maxSupplyVoltage Maximum value the supply voltage line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param minSupplyVoltage Minium value the supply voltage line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param maxExternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minExternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default -128.
     * \param maxInternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minInternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default 0.
     * \param tempOffset Offset that is to be applied to the internal temperature readings., default 0.
     * \param offsetInternalTemp Whether the internal or external temperature offset value is to be added to the temperature readings. Default true.
     * \param actual2Point5Volt Sets the actual 2.5V reading taken by the by the monitor. Default 0.
     * \param actual3Point3Volt Sets the actual 3.3V reading taken by the monitor. Default 0.
     * \param actual5Volt  Sets the actual 5V reading taken by the monitor. Default 0.
     * \param actual12Volt Sets the actual 12V reading taken by the monitor. Default 0.
     * \param actualCoreVoltage Sets the actual core voltage reading taken by the monitor. Default 0.
     * \param actualSupplyVoltage Sets the actual supply voltage reading taken by the monitor. Default 0.
     * \param actualExternalTempSets the actual external temperature measurement taken by the monitor. Default 0.
     * \param actualInternalTemp Sets the actual internal temperature measurement taken by the monitor. Default 0.
     * \param manufacturesId Sets the manufactures ID of the voltage monitor. Default 0.
     * \param steppingId Sets the stepping ID of the voltage monitor. Default 0.
     * \param statusRegister Sets the status register bits of the voltage monitor. Default 0.
     *
     * A Fed9UVoltageControlInfo object can be default constructed. If this is the case then all limits
     * will be set to their maximum values and no resets will be flagged.
     */
    Fed9UVoltageControlInfo(bool standBy = false,
			    bool resetStatusRegister    = false,
			    float max2Point5Volt  = 3.320,
			    float min2Point5Volt  = 0,
			    float max3Point3Volt  = 4.383,
			    float min3Point3Volt  = 0,
			    float max5Volt        = 6.640,
			    float min5Volt        = 0,
			    float max12Volt       = 15.938,
			    float min12Volt       = 0,
			    float maxCoreVoltage  = 2.988,
			    float minCoreVoltage  = 0,
			    float maxSupplyVoltage = 4.383,
			    float minSupplyVoltage = 0,
			    i16 maxExternalTemp    = 127,
			    i16 minExternalTemp    = 0,
			    i16 maxInternalTemp    = 127,
			    i16 minInternalTemp    = 0,
			    i16 tempOffset = 0,
			    bool offsetInternalTemp   = false,
			    float actual2Point5Volt   = 0,
			    float actual3Point3Volt   = 0,
			    float actual5Volt  = 0,
			    float actual12Volt = 0,
			    float actualCoreVoltage   = 0,
			    float actualSupplyVoltage = 0,
			    i16 actualExternalTemp  = 0,
			    i16 actualInternalTemp  = 0,
			    u32 manufacturesId = 0,
			    u32 steppingId     = 0,
			    u16 statusRegister = 0);

    /**
     * \brief  This sets the actual voltage on the 2.5V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual2Point5Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 2.5V.
     * \return float.
     */
    float getActual2Point5Volt() const;

    /**
     * \brief  This sets the actual voltage on the 3.3V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual3Point3Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 3.3V.
     * \return float.
     */
    float getActual3Point3Volt() const;

    /**
     * \brief  This sets the actual voltage on the 5V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual5Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 5V.
     * \return float.
     */
    float getActual5Volt() const;

    /**
     * \brief  This sets the actual voltage on the 12V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual12Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 12V.
     * \return float.
     */
    float getActual12Volt() const;

    /**
     * \brief  This sets the actual voltage on the processore core voltage.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualCoreVoltage(float voltage);

    /**
     * \brief  Returns the actual voltage on the processore voltage.
     * \return float.
     */
    float getActualCoreVoltage() const;

    /**
     * \brief  This sets the actual voltage on the 3.3V supply voltage.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualSupplyVoltage(float voltage);

    /**
     * \brief  Returns the actual voltage on the 3.3V supply voltage.
     * \return float.
     */
    float getActualSupplyVoltage() const;

    /**
     * \brief  This sets the current external temperature.
     * \param  temp Temperature reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualExternalTemp(i16 temp);

    /**
     * \brief  Returns the current external temperature.
     * \return signed short.
     */
    i16 getActualExternalTemp() const;

    /**
     * \brief  This sets the actual internal temperature.
     * \param  temp Temperature reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualInternalTemp(i16 temp);

    /**
     * \brief  Returns the actual internal temperature.
     * \return signed short.
     */
    i16 getActualInternalTemp() const;

    /**
     * \brief  This sets the manufactures ID.
     * \param  manufacturesId Sets the voltage monitor ID as read from the chip.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setManufacturesId(u32 manufacturesId);

    /**
     * \brief  Returns the manufactures ID.
     * \return unsigned long.
     */
    u32 getManufacturesId() const;

    /**
     * \brief  This sets the stepping ID.
     * \param  steppingId Set the voltage monitor ID as read from the chip.
     */
    Fed9UVoltageControlInfo& setSteppingId(u32 steppingId);

    /**
     * \brief  Returns the stepping ID.
     * \return unsigned long.
     */
    u32 getSteppingId() const;

    /**
     * \brief  This sets all the settings in the status register. The enum Fed9UVoltageStatus can be
     *         used to set the flags for the voltage readings.
     * \param  statusRegister Contains the status bits that are to be set.
     * \return Self reference.
     *
     * A bitwise OR can be used to set multiple flag. Any flag not set will be cleared. Further details
     * about the meaning of the bits can be found in the enum Fed9UVoltageStatus documentation.
     */
    Fed9UVoltageControlInfo& setStatusRegister(u16 statusRegister);

    /**
     * \brief  Returns an unsigned short that contains the status bits for the voltage monitor chip.
     * \return unsigned short.
     *
     * The status bits can be decoded with the Fed9UVoltageStatus enum and an explanation of what each
     * bit means can be found in the Fed9UVoltageStatus enum documentation.
     */
    u16 getStatusRegister() const;

    /**
     * Some private data members used to store the state of a Fed9UVoltageControl object.
     */
  private:

    float actual2Point5Volt_, actual3Point3Volt_, actual5Volt_, actual12Volt_;
    float actualCoreVoltage_, actualSupplyVoltage_;
    i16 actualExternalTemp_, actualInternalTemp_;
    u32 manufacturesId_, steppingId_;
    u16 statusRegister_;

  };// End of class Fed9UVoltageControlInfo.

  /**
   * \brief  This overload of operator<< can used to write a Fed9UVoltageControlInfo to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UVoltageControlInfo class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UVoltageControlInfo& data);

  /**
   * \brief  This overload of operator>> can used to read a Fed9UVoltageControlInfo from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UVoltageControlInfo class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream & is, Fed9UVoltageControlInfo& data);

}// End of Fed9U namespace.
#endif// H_Fed9UVoltageControlInfo.
#ifndef H_Fed9UEpromDescription
#define H_Fed9UEpromDescription

//Author: Gareth Rogers
//Date  : April 2004

/**
 * 
 */


namespace Fed9U {

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  A class that provides the ability to configure the EPROM on the FED.
   * \author Gareth Rogers, RAL
   * \date   04/2004
   *
   * Fed9UEpromDescription contains 2KB of memory that can be formatted as required by the FED. This class also supports an EPROM version number
   * which allows EPROM data to be saved in different formats, according to which version of events is used. This class has no knowledge however
   * of how the write protected areas of the FED are setup. It assumes all is read/writable, however this is not the case on the FED. The class
   * currently supports versions 1 and 2.
   */
  class Fed9UEpromDescription {

  public:

    /**
     * \brief Constructor.
     * \param version EPROM version number. The structure of the saved data in the EPROM varies depending on the version of the EPROM used.
     *
     * It takes a pointer to a 2KB block of memory of which a local copy is made. The data pointer is defaulted to a null pointer and
     * if null a blank 2KB of memory will be created.
     */
    explicit Fed9UEpromDescription(u32 version, const u8* const data = 0);

    /**
     * \brief  This allows you write to a single byte of memory.
     * \param  offset Offset in to EPROM memory where the byte should be written. Range:0x000-0x7ff.
     * \param  data Byte that is to be written to memory. Range:0-0xff.
     * \return Self reference.
     */
    Fed9UEpromDescription& setSingleByte(u32 offset, u8 data);

    /**
     * \brief  This allows you to read a single byte of memory.
     * \param  offset Offset in to EPROM memory where the byte should be read. Range:0x000-0x7ff.
     * \return unsigned char Byte that is read back from that area of the memory.
     */
    u8 getSingleByte(u32 offset) const;

    /**
     * \brief  Each FED has a serial number to uniquely identify it.
     * \param  serialNumber Serial number of the FED.
     * \return Self reference.
     */
    Fed9UEpromDescription& setFedSerialNumber(u32 serialNumber);

    /**
     * \brief  Each FED has a serial number to uniquely identify it.
     * \return unsigned long Serial number for the FED.
     */
    u32 getFedSerialNumber() const;

    /**
     * \brief  An overloaded version of the operator[] which provides the same functionality as set/getEpromSingleByte.
     * \param  offset Offset in to EPROM memory where the byte should be written/read. Range:0x000-0x7ff.
     * \return unsigned char Writeable reference to the byte at the position specified by offset.
     *
     * The reference will give both the value of the byte currently in that location and allow the User to write to it.
     */
    u8& operator[](u32 offset);

    /**
     * \brief  Another overloaded version of the operator[], this however this returns a copy of the byte and as such is read only.
     * \param  offset Offset in to EPROM memory where the byte should be written. Range:0x000-0x7ff.
     * \return unsigned char Copy of the byte at the position specified by offset.
     */
    u8 operator[](u32 offset) const;

    /**
     * \brief  Returns the size of the EPROM memory in bytes.
     * \return unsigned long Size of the EPROM memory.
     */
    u32 getEpromSize() const;
    
    /**
     * \brief  Sets the version of the eprom.
     * \param  version Version of the EPROM that is to be set.
     * \return Self reference.
     */
    Fed9UEpromDescription& setEpromVersion(const u32 version);

    /**
     * \brief  Returns the version of the EPROM.
     * \return unsigned long Version number of the EPROM memory format.
     */
    u32 getEpromVersion() const;

  private:

    /**
     * \brief Size of the EPROM in bytes.
     *
     * The useable EPROM memory is actually 1 byte less than the total size as the last byte of the EPROM is used to set the write
     * protection level and cannot be used for storage.
     */
    static const u32 epromSize_ = 2048;

    u8 epromData_[epromSize_]; //!< This holds the EPROM memory, which is set to be as large as the eprom size.
    u32 epromVersion_;         //!< Stores the version of the EPROM that we are currently using.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r);
    // </NAC>

  };// End of class Fed9UEpromDescription
  
  // <NAC date="24/04/2007"> operator to compare EpromDescriptions
  /**
   * \brief  == operator to allow Fed9UEpromDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r);
  /**
   * \brief  != operator to allow Fed9UEpromDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r) { return !(l==r); }
  // </NAC>

  /**
   * \brief  This overload of operator<< can used to write the entire EPROM memory to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data EPROM object that is to be written to the stream.
   * \return ostream Reference to the output stream that the EPROM data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UEpromDescription& data);

  /**
   * \brief  This overload of operator>> can used to read the entire EPROM memory to from an input stream.
   * \param  is Input stream the data is read from.
   * \param  data EPROM object that is filled with the data in the stream.
   * \return istream Reference to the input stream that the EPROM data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UEpromDescription& data);

}// End of Fed9U namespace
#endif// H_Fed9UEpromDescription
#ifndef H_Fed9UABC
#define H_Fed9UABC

#include "ICExImpl.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace Fed9U {

//using.*std::string;

  /**
   * \brief Abstract Base Class to a generic FED.
   * \author Jonathan Fulcher Matt Noy
   * \date Created 14/05/2004
   *
   * This class defines an interface to a generic FED device. It is used to ensure consistancy
   * between interfaces for real FEDs and software replicas.
   */
  class Fed9UABC {

  public:
    /**
     * \brief Class destructor.
     */
    virtual ~Fed9UABC() {}

    /**
     * \name Pure virtual methods.
     *
     * These methods are required to be implemented by any derived class, such that it provides
     * the minimum number of methods that will provide a complete interface to the FED.
     */
    //@{
  public:

    /**
     * \brief  Used to set the FED coarse and fine delay settings for a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  coarseDelay Number of fine steps to delay by. Range: 0-15.
     * \param  fineDelay Number of fine steps to delay by. Range: 0-24.
     * \return Self reference.
     *
     * A FED channel can be skewed by upto 400 nano seconds in nano second steps. The coarse delay
     * represents delays by whole bunch crossings (25ns) of the incoming channel data and the fine delay
     * integer values between coarse delay. Each fine delay value corresponds to a nano second, however
     * as it corresponds to the number of fine steps past a coarse setting it is limited to a
     * maximum of 24.
     */
    virtual Fed9UABC& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) = 0;

    /**
     * \brief  Used to set the fine delay on a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  fineDelay Number of fine steps to delay by. Range: 0-24.
     * \return Self reference.
     *
     * Each fine delay value corresponds to a nano second delay of the incoming channel data,
     * however as it provides the number of fine steps to delay between coarse delay settings
     * it is limit to a maximum of 24.
     */
    virtual Fed9UABC& setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay) = 0;

    /**
     * \brief  Used to set the coarse delay on a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  coarseDelay Number of coarse steps to delay by. Range: 0-15.
     * \return Self reference.
     *
     * Each coarse delay value will delay the incoming channel data by whole bunch crossing (25ns)
     * steps.
     */
    virtual Fed9UABC& setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay) = 0;

    /**
     * \brief  Sets the amount by which to offset the output from the TrimDACs to the ADC inputs.
     * \param  fedChannel FED channel to which the offset is to be applied.
     * \param  offset Value of the offset to be set. Range: 0-255.
     * \return Self reference.
     */
    virtual Fed9UABC& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset) = 0;

    /**
     * \brief  Set the OptoRx input offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  inputOffset A value that specifies the offset of the input signal to the OptoRx.
     *         Range:0-15
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 inputOffset) = 0;

    /**
     * \brief  Returns the OptoRx input offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the input signal to the OptoRx.
     *         Range:0-15
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const = 0;


    /**
     * \brief  Set the OptoRx output offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  outputOffset A value that specifies the offset of the input signal to the OptoRx.
     *         Range: 0-3.
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 outputOffset) = 0;

    /**
     * \brief  Returns the OptoRx output offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the input signal to the OptoRx.
     *         Range: 0-3.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Set the OptoRx capacitor setting.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  capacitor A value that specifies the offset of the capacitor setting to the OptoRx.
     *         Range: 0-3.
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 capacitor) = 0;

    /**
     * \brief  Returns the OptoRx capacitor setting.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the capacitor setting to the OptoRx.
     *         Range: 0-3.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Disables an APV from header dection and also removes its data from the output.
     * \param  fedApv The address of FED APV that should be enabled/disabled.
     * \param  disable States whether to disable, boolean true, or enable, boolean false, an APV.
     * \return Self reference.
     *
     * \todo check if anyone figured out if this worked.
     */
    virtual Fed9UABC& setApvDisable(const Fed9UAddress& fedApv, bool disable) = 0;

    /**
     * \brief  Disables an APV from header dection and also removes its data from the output.
     * \param  fedApv The address of FED APV that should be enabled/disabled.
     * \return bool States whether it is disable, boolean true, or enable, boolean false, an APV.
     */
    virtual bool getApvDisable(const Fed9UAddress& fedApv) const = 0;

    /**
     * \brief  Used to disable FED common mode median calculation and over ride with User defined value.
     * \param  fedFeUnit FE unit whose settings are to be modified.
     * \param  medianOverrideDisable Boolean true if the values are to be disabled, boolean false if enabled.
     * \param  medianOverride A vector containing 24 elements to set the common mode median values to on each
     *         APV. Range: 0-1023.
     * \return Self reference.
     */
    virtual Fed9UABC& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, std::vector<u16> medianOverride) = 0;

    /**
     * \brief  Used to disable FED common mode median calculation and over ride with User defined value.
     * \param  fedFeUnit FE unit whose settings are to be modified.
     * \param  medianOverrideDisable Boolean true if the values are to be disabled, boolean false if enabled. Value returned by reference.
     * \return std::vector<u16> A vector containing 24 elements containing the common mode median values to on each APV. Range: 0-1023.
     */
    virtual std::vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const = 0;

    /**
     * \brief  Sets the number of words of raw data that are written when running in scope mode.
     * \param  numberOfWords The number of 32 bit words to be written.
     * \return Self reference.
     */
    virtual Fed9UABC& setScopeLength(u16 numberOfWords) = 0;

    /**
     * \brief  Returns the number of words of raw data that are written when running in scope mode.
     * \return unsigned short The number of 32 bit words to be written.
     */
    virtual u16 getScopeLength() const = 0;

    /**
     * \brief  Sets the threshold values for the APV tick marks as seen by the tickers.
     * \param  fedChannel Channel on the FED that the threshold should address.
     * \param  channelThreshold Value of the threshold to be set. Range: 0-992, steps 32.
     * \return Self reference.
     *
     * The threshold value can only be set in steps of 32. The value given refers to the value
     * of the threshold from the 10bit (1023) ADC input. As it can only be set in steps of 32
     * any value that is not a multiple of 32 will be rounded to the nearest whole multiple of 32.
     */
    virtual Fed9UABC& setFrameThreshold(const Fed9UAddress& fedChannel, u16 channelThreshold) = 0;

    /**
     * \brief  Returns the threshold values for the APV tick marks as seen by the tickers.
     * \param  fedChannel Channel on the FED that the threshold should address.
     * \return unsigned short Value of the threshold to be set. Range: 0-992, steps 32.
     *
     * The threshold value can only be set in steps of 32. The value returned refers to the value
     * of the threshold from the 10bit (1023) ADC input. As it can only be set in steps of 32
     * any value that is not a multiple of 32 will be rounded to the nearest whole multiple of 32.
     */
    virtual u16 getFrameThreshold(const Fed9UAddress& channel) const = 0;

    /**
     * \brief  Sets the location of the trigger.
     * \param  triggerSource The source of the trigger, defined in the enum Fed9UTrigSource.
     * \return Self reference.
     */
    virtual Fed9UABC& setTriggerSource(Fed9UTrigSource triggerSource) = 0;    

    /**
     * \brief  Returns the location of the trigger.
     * \return Fed9UTrigSource The source of the trigger, defined in the enum Fed9UTrigSource.
     */
    virtual Fed9UTrigSource getTriggerSource() const = 0;

    /**
     * \brief  Used internal FED tests. No for use in normal operation.
     * \param  testRegister Value to be set to this register.
     * \return Self reference.
     *
     * More details can be found about this register in the BE FPGA documentation.
     */
    virtual Fed9UABC& setTestRegister(u32 testRegister) = 0;

    /**
     * \brief  Used internal FED tests. No for use in normal operation.
     * \return unsigned long Value set in this register.
     *
     * More details can be found about this register in the BE FPGA documentation.
     */
    virtual u32 getTestRegister() const = 0;

    /**
     * \brief  Disables the FE unit readout by the BE FPGA.
     * \param  fedFeUnit FE unit that is to be disabled.
     * \param  fedFeUnitDisable Boolean true disables the FE unit, boolean false enables it.
     * \return Self reference.
     *
     * When a FE unit is disabled their data is removed from the output event and are not stored
     * by the BE FPGA.
     */
    virtual Fed9UABC& setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable) = 0; 

    /**
     * \brief  Disables the FE unit readout by the BE FPGA.
     * \param  fedFeUnit FE unit that is to be disabled.
     * \return bool Boolean true disables the FE unit, boolean false enables it.
     *
     * When a FE unit is disabled their data is removed from the output event and are not stored
     * by the BE FPGA.
     */
    virtual bool getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Disables the FED from recognising and hence outputing events.
     * \param  fedDisable Boolean true will disable the FED, boolean false enable it.
     * \return Self reference.
     *
     * This command prevents the BE FPGA from seeing triggers and frames and hence it
     * will never see an event to form one. All data taken while the FED is disabled
     * is lost.
     */
    virtual Fed9UABC& setFedBeFpgaDisable(bool fedDisable) = 0;

    /**
     * \brief  Disables the FED from recognising and hence outputing events.
     * \return bool Boolean true will disable the FED, boolean false enable it.
     *
     * This command prevents the BE FPGA from seeing triggers and frames and hence it
     * will never see an event to form one. All data taken while the FED is disabled
     * is lost.
     */
    virtual bool getFedBeFpgaDisable() const = 0;

    /**
     * \brief  Something
     * \return Self reference.
     *
     * \todo ask Jonathan about the FED and hardware ID.
     */
    virtual Fed9UABC& setFedId(u16 fedId) = 0;

    /**
     * \todo ask Jonathan about this.
     */
    virtual u16 getFedId() const = 0;

    /**
     * \brief  Something
     * \return Self reference.
     *
     * \todo ask Jonathan about the FED and hardware ID.
     */
    virtual Fed9UABC& setFedHardwareId(u32 fedId) = 0;

    virtual u32 getFedHardwareId() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the FE FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setFeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the FE FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getFeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the BE FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setBeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the BE FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getBeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the VME FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setVmeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the VME FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getVmeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the delay FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setDelayFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the delay FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getDelayFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the EPROM.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setEpromVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the EPROM.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getEpromVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the FED.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     *
     * \todo figure out what the FED version is.
     */
    virtual Fed9UABC& setFedVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the FED.
     * \return unsigned long 32 bit firmware version number.
     *
     * \todo figure out what the FED version is.
     */
    virtual u32 getFedVersion() const = 0;

    /**
     * \brief  Sets data the readout route for the BE FPGA.
     * \param  readRoute Specifies the appropriate route to read the data from,
     *         as defined in Fed9UReadRoute enum.
     * \return Self reference.
     */
    virtual Fed9UABC& setBeFpgaReadRoute(Fed9UReadRoute readRoute) = 0;

    /**
     * \brief  Returns the data readout route for the BE FPGA.
     * \return Fed9UReadRout Specifies the route that the data will be read from as defined in Fed9UReadRoute enum.
     */
    virtual Fed9UReadRoute getBeFpgaReadRoute() const = 0;

    /**
     * \brief  Figure this out
     * \param  fedChannel FED channel that the ADC control bits should be updated on.
     * \param  adcControls Fed9UAdcControls is a data structure containing the four settings for the ADC.
     * \return Self reference.
     *
     * \todo   Figure out what the adc control bits do!
     */
    virtual Fed9UABC& setAdcControls(const Fed9UAddress& fedChannel, const Fed9UAdcControls& adcControls) = 0;

    /**
     * \brief  Figure this out
     * \param  fedChannel FED channel that the ADC control bits should be updated on.
     * \return Fed9UAdcControls A data structure containing the four settings for the ADC.
     *
     * \todo   Figure out what the adc control bits do!
     */
    virtual Fed9UAdcControls getAdcControls(const Fed9UAddress&) const = 0;

    /**
     * \brief  Sets the FED data acquisition mode.
     * \param  fed9UDaqMode The data acquisition mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqMode.
     * \return Self reference.
     *
     * This defines whether to find tick marks or frames and also level of processing the FED will perform on the event.
     */
    virtual Fed9UABC& setDaqMode(Fed9UDaqMode fed9UDaqMode) = 0;

    /**
     * \brief  Returns the FED data acquisition mode.
     * \return Fed9UDaqMode The data acquisition mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqMode.
     *
     * This defines whether to find tick marks or frames and also level of processing the FED will perform on the event.
     */
    virtual Fed9UDaqMode getDaqMode() const = 0;

    /**
     * \brief  Sets the super data acquisition mode.
     * \param  fed9UDaqSuperMode The super mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqSuperMode.
     * \return Self reference.
     *
     * This adds to the existing data acquision mode. Any value of the super mode and normal DAQ
     * mode can be set, however not all combinations will have any effect. There are no incompatible modes.
     * A valid DAQ mode must be set in addition to this.
     */
    virtual Fed9UABC& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode) = 0;

    /**
     * \brief  Returns the super data acquisition mode.
     * \return Fed9UDaqSuperMode The super mode of the FED. Permitted values are defined by the enum Fed9UDaqSuperMode.
     *
     * This adds to the existing data acquision mode. Any value of the super mode and normal DAQ
     * mode can be set, however not all combinations will have any effect. There are no incompatible modes.
     * A valid DAQ mode must be set in addition to this.
     */
    virtual Fed9UDaqSuperMode getDaqSuperMode() const = 0;

    /**
     * \brief  Determines the clock source used on the FED.
     * \param  clockSelect The clock to be used by the FED. Values are defined in the enum Fed9UClockSource.
     * \return Self reference.
     */
    virtual Fed9UABC& setClock(Fed9UClockSource clockSelect) = 0;

    /**
     * \brief  Returns the clock source in use on the FED.
     * \return The clock to be used by the FED. Values are defined in the enum Fed9UClockSource.
     */
    virtual Fed9UClockSource getClock() const = 0; 

    /**
     * \brief  Provides a means to control the LM82 temperature monitoring chips on each FPGA.
     * \param  fpga FPGA that the User wishes to communicate with. Accepts FE, BE and VME FPGA numbers.
     * \param  tempControl The class Fed9UTempControl contains all the parameters that can be set
     *         on the temperature chip. This \link Fed9U::Fed9UTempControl class\endlink should be consulted for further details.
     * \return Self reference.
     *
     * As the class Fed9UTempControl is taken as an argument all parameters for that temperature control
     * chip will be updated at once.
     */
    virtual Fed9UABC& setTempControl(const Fed9UAddress& fpga, const Fed9UTempControl& tempControl) = 0;

    /**
     * \brief  Provides a means to view all the read/write registers on the LM82 temperature monitoring chips on each FPGA.
     * \param  fpga FPGA that the User wishes to communicate with. Accepts FE, BE and VME FPGA numbers.
     * \return Fed9UTempControl The class Fed9UTempControl contains all the parameters that can be on the temperature chip.
     *         The return object contains all the current settings on these register. This \link Fed9U::Fed9UTempControl class\endlink
     *         should be consulted for further details.
     *
     * As this returns a Fed9UTempControl object all read/write parameters on the chip are returned at once.
     */
    virtual Fed9UTempControl getTempControl(const Fed9UAddress& fpga) const = 0;

    /**
     * \brief  Allows the User to complement the raw data that is received from the FED.
     * \param  fedChannel Channel on the FED that needs to be inverted.
     * \param  disable A boolean true will disable the complement and boolean false enable it.
     * \return Self reference.
     *
     * This should only be required if the inverter stage of an APV is turned off.
     */
    virtual Fed9UABC& setComplement(const Fed9UAddress& fedChannel, bool disable) = 0;


    /**
     * \brief  Allows the User to complement the raw data that is received from the FED.
     * \param  fedChannel Channel on the FED that needs to be inverted.
     * \return bool A boolean true will disable the complement and boolean false enable it.
     *
     * This setting does not necessarily mean the data has not been complemented at the APV inverter stage,
     * only that the FED is not complementing the data. By default this should be done and the complement at
     * FED level not required.
     */
    virtual bool getComplement(const Fed9UAddress& channel) const = 0;

    /**
     * \brief  Used by derived classes to return a copy of the Fed9UDescription, that is either a member object or a self reference.
     * \return Self reference.
     *
     * This method returns a reference to the description object itself.
     * The purpose of this is to implement the pure virtual method which maps to the 
     * method in the VME Device, which returns a reference to the local fed settings, 
     * this is so that once can call this method on the base abstract base class. 
     */
    virtual Fed9UABC& getFed9UVMEDeviceDescription() = 0;

    //<JEC date=08/12/2005>
    //Added eventType
    /**
     * \brief  Sets the type of run that is has been performed. This is saved in the event header.
     * \param  eventType They type of event that is being run with.\n
     *         Possible values are:
     *         \li 1	Physics trigger	Final OR (physics algorithms)
     *         \li 2	Calibration trigger Calibration cycle
     *         \li 3	Test trigger        Test cycle
     *         \li 4	Technical trigger   Technical trigger (external trigger)
     *         \li 5	Simulated events    Reserved for DAQ usage
     *         \li 6	Traced events	    Reserved for DAQ usage
     *         \li 7 	Error               Others undefined.
     * \return Self reference.
     * \todo probably want to create another enum for this.
     */
    virtual Fed9UABC& setDaqEventType(u16 eventType) = 0;

    /**
     * \brief  Returns the type of run that is has been performed. This is saved in the event header.
     * \return unsigned short They type of event that is being run with.\n
     *         Possible values are:
     *         \li 1	Physics trigger	Final OR (physics algorithms)
     *         \li 2	Calibration trigger Calibration cycle
     *         \li 3	Test trigger        Test cycle
     *         \li 4	Technical trigger   Technical trigger (external trigger)
     *         \li 5	Simulated events    Reserved for DAQ usage
     *         \li 6	Traced events	    Reserved for DAQ usage
     *         \li 7 	Error               Others undefined.
     * \todo probably want to create another enum for this.
     */
    virtual u16 getDaqEventType(void) const = 0;
    //</JEC>

    //<JEC date=08/12/2005>
    //Added DaqFov
    /**
     * \brief  Set the version format of the event, which is stored in the event header.
     * \param  fov This is the four bit version number of the event. Range: 0-7.
     * \return Self reference.
     * \todo   not certain about exactly what this is.
     */
    virtual Fed9UABC& setDaqFov(u16 fov) = 0;

    /**
     * \brief  Returns the version format of the event, which is also stored in the event header.
     * \return unsigned short This is the four bit version number of the event. Range: 0-7.
     * \todo   not certain about exactly what this is.
     */
    virtual u16 getDaqFov(void) const = 0;
    //</JEC>

    //<JEC date=23/02/2006>
    //Added bunch crossing offsets.
    /**
     * \brief  Applies an offset to the bunch crossing orbit.
     * \param  bxOffset The offset to be applied. Range: 0-4095.
     * \return Self reference.
     *
     * Affects the bunch crossing written in the FED register (see Fed9UVmeDevice::getBunchCount)
     * and also the number written in the FED data buffer.
     */
    virtual Fed9UABC& setBunchCrossingOffset(u16 bxOffset) = 0;

    /**
     * \brief  Returns the offset that has been applied to the bunch crossing orbit.
     * \return unsigned short The value of the bunch crossing orbit.
     *
     * Amount by which the bunch crossing counter is offset in the FED register (see Fed9UVmeDevice::getBunchCount)
     * and also the number written in the FED data buffer.
     */    
    virtual u16 getBunchCrossingOffset(void) const = 0;
    //</JEC>

    //<JEC date=09/01/2006>
    //Added tracker mode to description. 
    /**
     * \brief  Sets the header format type. It determines the format of the FED data buffer header.
     * \param  headerType The possible header types are defined in the enum Fed9UHeaderFormat.
     * \return Self reference.
     *
     * This number is found in both the DAQ and tracker special header.
     */
    virtual Fed9UABC& setHeaderFormatType(Fed9UHeaderFormat headerType) = 0;

    /**
     * \brief  Returns the type of header that will be present in an event.
     * \return Fed9UEventFormat An enum defined in TypeDefs.hh.
     *
     * The value defines what header type is present in an event and the value itself is also included.
     */
    virtual Fed9UHeaderFormat getHeaderFormatType(void) const = 0;
    //</JEC>

    /**
     * \brief  Returns a reference to a Fed9UStrips object. These contain the strip settings.
     * \return Fed9UStrips A class that contains all the strip settings, see Fed9U::Fed9UStrips for more details.
     */
    virtual Fed9UStrips & getFedStrips() = 0;
    //    virtual const Fed9UStrips & getFedStrips() = 0;

    /**
     * \brief  Provides the location of the Fed9U HAL address table, which provides a map of the FED VME FPGA registers.
     * \return string Contains the location of the HAL address table.
     */
    //<JEC date=27/9/2007>
    //    virtual const std::string & getHalAddressTable() const = 0;
    virtual std::string getHalAddressTable() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }
    //</JEC>
    //@}

    /**
     * \name Fed9UVmeDevice specific methods.
     *
     * These methods are required for part of a hardware FED implementation to provide easy to configuration methods. They are not
     * required by a software implementation and hence do not form part of the enforced interface.
     */

    //@{
    /**
     * \brief  Used upload all write settings to the FED from the Fed9UDescription object.
     * \param  bool If boolean true a reset is performed before any settings are updated and the clock value will be set if required.
     * \return Self reference.
     *
     * The strips settings, temperature and voltage monitor chips are not set up. There exist separate initialisation methods for these.
     * All APVs and ADCs are disabled by this method and start() must be called to enable data taking.
     */
    virtual Fed9UABC& init(bool) {return *this;}

    /**
     * \brief  Initialises all the FED strip registers as defined in the class Fed9UStrips. Only required in processed raw and zero suppression data modes.
     * \return Self reference.
     */
    virtual Fed9UABC& initStrips() { return *this;}
    //@}

    /**
     * \name Fed9UDescription specific methods.
     *
     * These methods are required in an implementation of a software based FED, but are not required by a real FED as no registers exist
     * for these settings.
     */

    //@{

    /**
     * \brief This method loads the FED description from a stream that is passed into the method.
     * \param istream& Input stream that the FED description is to be loaded from.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * description to the source of the stream using saveDescription(ostream&).
     */
    virtual void loadDescription(std::istream&) {}

    /**
     * \brief This method saves the FED description to a stream that is passed into the method.
     * \param ostream& Output stream where the description settings are to be saved.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * description passed into loadDescription(istream&).
     */
    virtual void saveDescription(std::ostream&) const {}

    /**
     * \brief This method loads a default set of values into the FED description.
     *
     * This need only be used the first time you construct a FED descrition, all subsequent times you should
     * construct using the loadDescription(istream&) method.
     */
    virtual void loadDefaultDescription(){}

    /**
     * /todo figure out what the name is for.
     */
    virtual Fed9UABC& setName(std::string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this; }
    virtual std::string getName() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }

    /**
     * \brief  FED description objects contain a version to ensure that only an appropriately saved file can be used to load a FED description object.
     * \return string Contains the version in use.
     */
    virtual std::string getVersion() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }

    /**
     * \brief  Required to construct a valid FED object as it contains the address map of the VME FPGA.
     * \param  string Location of the HAL address table.
     * \return Self reference.
     */
    virtual Fed9UABC & setHalAddressTable(const std::string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    /**
     * \brief  Returns the APV disabled settings for all APVs on a FE unit.
     * \return unsigned long The lowest 24 bits contain the APV disable settings. A 1 signifies disabled and a 0 enabled.
     *         The highest of these 24 bits contains FE unit APV 24.
     *
     * \todo check numbering scheme (internal or external).
     */
    virtual u32 getAllApvDisables(const Fed9UAddress&) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0;}

    /**
     * \brief  Returns the maximum buffer size that could be required by the Fed9UVmeDevice::getCompleteEvent method.
     * \return unsigned long Maximum possible required buffer size in number of 32 bit words.
     * 
     * Uses the current description values to esimate what this should be
     */
    virtual u32 getRequiredBufferSize() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }

    /**
     * \brief  Returns the value of the fine delay in use by a FED channel.
     * \param  Fed9UAddress FED channel number that need to be set.
     * \return unsigned short Value of the fine delay setting.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getFineDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }

    /**
     * \brief  Returns the value of the coarse delay in use by a FED channel.
     * \param  Fed9UAddress FED channel number that need to be set.
     * \return unsigned short Value of the coarse delay setting.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getCoarseDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }
    /**
     * \brief  Returns the amount by which to offset the output from the TrimDACs to the ADC inputs.
     * \param  fedChannel FED channel to which the offset is to be applied.
     * \return unsigned short Value of the offset to be set. Range: 0-255.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getTrimDacOffset(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}

    //@}

    /**
     * \name FED location parameters
     *
     * The following methods are required to talk to a FED in a crate.
     * They have no register on a FED and are only required during construction of a
     * FED object. They will however, be required by Fed9UDescription to identify which
     * FED it contains the settings for.
     */
    //@{

    /**
     * \brief Set the base address of a FED in the crate.
     * \return Self reference.
     *
     * The slot number and base address are two different ways of expressing the same number
     * and as such only one should need be set.
     */
    virtual Fed9UABC& setBaseAddress(const u32) { ICUTILS_VERIFY(false).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Returns the base of the FED in the crate.
     * \return u32 Contains the value of the FED base address.
     */
    virtual u32 getBaseAddress() { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}

    /**
     * \brief  Number of the crate that the FED is located in.
     * \return Self reference.
     */
    virtual Fed9UABC& setCrateNumber(u16) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Used to determine the crate number in which the FED is located.
     * \return u16 The number of the crate.
     *
     * A hardware FED does not know its physical location so these parameters must be retrieved from software.
     */
    virtual u16 getCrateNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; } 

    /**
     * \brief  ID of the Vme Controller in the Daisy Chain.
     * \return Self reference.
     */
    virtual Fed9UABC& setVmeControllerDaisyChainId(u16) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Used to determine the Id of the VME Controller in the Daisy Chain
     * \return u16 The number of the crate.
     *
     * A hardware FED does not know its physical location so these parameters must be retrieved from software.
     */
    virtual u16 getVmeControllerDaisyChainId() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; } 

    /**
     * \brief  Slot number in which the FED is located.
     * \return Self reference.
     *
     * The slot number and base address are two different ways of expressing the same number
     * and as such only one should need be set.
     */
    virtual Fed9UABC& setSlotNumber(u8){ ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Returns the slot that the FED is located in.
     * \return u8 Slot that FED is located in.
     */
    virtual u8 getSlotNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }    //@}

  };//class Fed9UABC


}//namespace Fed9U

#endif // H_Fed9UABC
#ifndef H_Fed9UGuiAbcMap
#define H_Fed9UGuiAbcMap


#include <vector>

/////////////////////////////////////////////////////////////////
//                                                             //
//   Fed 9U Abstract Base Class   : Fed9UABC                   //
//                                                             //
//  Author: Matt Noy, Jonathan Fulcher                IC       //
//  date: 14/05/2004                                           //
//  notes:                                                     //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////

namespace Fed9U {
  //using.*std::vector;
  class Fed9UABC;

  /**
   * \brief  Map class to map from an index to an array of abc pointers.
   * \author Matt Noy, Jonathan Fulcher, IC
   * \date   10/05/2004
   *
   * Each vector of Fed9UABC pointers is stored in it own element in the container. Entries can be added to the container
   * or to the vectors stored in the container.
   */
  class Fed9UGuiAbcMap {

  public:

    /**
     * \brief Constructor.
     */
    Fed9UGuiAbcMap();

    /**
     * \brief Destructor.
     *
     * Removes all entries from the array.
     */
    ~Fed9UGuiAbcMap() ;

    /**
     * \brief Adds an empty mapping of Fed9UABC pointers to the end of the container.
     */
    void addEntry() ;

    /**
     * \brief Removes a specific mapping of Fed9UABC pointers from the container.
     * \param index Index of the Fed9UABC pointer vector in the container to be deleted.
     */
    void deleteEntry(u32 index) ;

    /**
     * \brief Adds a pointer to a Fed9UABC class to the end of a mapping stored in the container.
     * \param index Index of the vector to be accessed within the container.
     * \param ptr Pointer to the Fed9UABC class to be added to the vector.
     */
    void addMapping( u32 index, Fed9UABC* ptr ) ;

    /**
     * \brief Deletes the Fed9UABC pointer given from the mapping stored in the container.
     * \param index Index of the vector that is to be accessed from the container.
     * \param ptr Pointer that is to be erased from the mapping.
     */
    void deleteMapping( u32 index, Fed9UABC* ptr ) ;

    /**
     * \brief Removes all data from the container.
     */
    void clear() ;

    /**
     * \brief  Returns a mapping that is stored in the container.
     * \param  index Index of the mapping that is to be returned from the container.
     * \return vector<Fed9UABC*> Reference to the mapping that is being returned from the container.
     */
    std::vector<Fed9UABC*>& getMap(u32 index) ;

    /**
     * \brief Returns the number of mappings that are stored within the container.
     * \return unsigned long.
     */
    u32 size() { return fedAbcMap.size(); }

  private:

    std::vector< std::vector <Fed9UABC*> > fedAbcMap; //!< Container to hold the mapping.

  };
}
#endif // H_Fed9UGuiAbcMap
#ifndef H_Fed9UDescription
#define H_Fed9UDescription

/////////////////////////////////////////////////////////////////
//                                                             //
//   FED 9U Description Class   : Fed9UDescription             //
//                                                             //
//  Author: Matt Noy, Jonathan Fulcher, Emlyn Corrin  IC       //
//  date: 19/06/2003                                           //
//  notes:                                                     //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////

// <NAC date="26/06/2007">
// </NAC>



#include <string>
#include <vector>
#include <iosfwd>
#include <cstring>

// This version number must be incremented whenever
// a change is made to the description strage format:
#define DESCRIPTION_VERSION 22

#define VERSION_TEXT(name) ("Fed9U" #name "Version" STRINGIZE(DESCRIPTION_VERSION))

namespace Fed9U {

  /**
   * \brief  Description of a whole FED.
   * \author Matt Noy, Jonathan Fulcher, Emlyn Corrin  IC
   * \date   19/06/2003
   *
   * The FED Configuration is contained within the class Fed9UDescription. 
   * This class contains all data which is required in order to configure the FED
   * for normal running. Only those registers in the FED who's settings persist during 
   * run time are included here. Registers such as status and counters which 
   * are constantly changing are not included. The basic concept is whatever is 
   * set in the FED description that is then passed to the Fed9UVmeDevice, will mirror
   * what is in the FED after it has been initialized with this object.
   * Therefore during run time, the FED should always contain settings which match a local
   * copy of the FED description. Unless an register has been explicitly changed via some 
   * external interface. Therefore it is recommended that you maintain a local copy of the
   * FED description whenever you instantiate a Fed9UVmeDevice and make sure that it 
   * always mirrors the settings in the FED proper. To do this you should adopt an 
   * approach whereby the Settings that you wish to edit on the FED are first set in the
   * local description, and then transferred to the FED, either by individually writing to
   * the specific register, or by uploading the entire description to the FED.
   */

  class Fed9UDescription  : public Fed9UABC {
  public:
 
    /**
     * \name Constructors
     *
     * Constructors and destructor that are provided by the class.
     */
    //@{

    /**
     * This method replaces the default constructor, but does not perform any initialization
     * of the description. After constructing a FED description, it will be undefined. In
     * order to define settings one must call loadDefaultDescription(), or loadDescription(istream&).
     *
     ********************************************/
    Fed9UDescription();
  
    /**
     * This method replaces the default distructor, but does nothing.
     *
     ********************************************/
    ~Fed9UDescription();
    
    //@}

    /**
     * \name Description settings file setters/getters.
     *
     * Used to load or save the settings contained in a Fed9UDescription to/from a file stream.
     */
    //@{

    /**
     * \brief This method loads the FED description from a stream that is passed into the method.
     * \param std::istream Input stream from which the file is to be loaded.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * description to the source of the stream using saveDescription(std::ostream&).
     */
    void loadDescription(std::istream&);

    /**
     * \brief This method loads the FED settings, which are a subsection of the FED description
     *        from a stream that is passed into the method.
     * \param std::istream Input stream from which the settings are to be loaded.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * settings to the source of the stream using saveSettings(std::ostream&).
     */
    void loadSettings(std::istream&);

    /**
     * \brief This method loads the strips from a stream that is passed into the method.
     * \param std::istream Input stream containing the strip settings.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * strips to the source of the stream using saveStrips(std::ostream&).
     */
    void loadStrips(std::istream&);
    
    /**
     * \brief This method saves the FED description to a stream that is passed into the method.
     * \param Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * description passed into loadDescription(std::istream&).
     */
    void saveDescription(std::ostream&) const;


    /**
     * \brief This method saves the FED Settings to a stream that is passed into the method.
     * \param Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * Settings passed into loadSettings(std::istream&).
     */
    void saveSettings(std::ostream&) const;
    

    /**
     * \brief This method saves the Strips to a stream that is passed into the method.
     * \param std::ostream Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * strips passed into loadStrips(std::istream&).
     */
    void saveStrips(std::ostream&) const;
    
    /** 
     * \brief This method loads a default set of values into the FED description. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultDescription() { loadDefaultSettings(); loadDefaultStrips(); }
    
    /** 
     * \brief This method loads a default settings. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultSettings();
    
    /**
     * \brief This method loads a default set of strips values.
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultStrips();


    /**
     * \brief This method clones an entire description
     * \return a pointer to the description cloned
     * \warning do not forget to delete the result once you have finished with it
     */
    Fed9UDescription* clone ( );
    

    //@}

    /** 
     * Setters for Fed9UDescription
     ********************************************/ 
    
    /**
     * Sets the bus Adaptor Type 
     *
     */
    Fed9UDescription& setBusAdaptorType(Fed9UHalBusAdaptor type);

    /**
     * Sets the version number for fed hardware 
     *
     */
    Fed9UDescription& setFedVersion(u32 version);
 
    /**
     * Sets the version number for eprom data
     *
     */
    Fed9UDescription& setEpromVersion(u32 version);
    
    /**
     * Sets the version number for fe firmware 
     *
     */
    Fed9UDescription& setFeFirmwareVersion(u32 version);

    /**
     * Sets the version number for be firmware 
     *
     */
    Fed9UDescription& setBeFirmwareVersion(u32 version);

    /**
     * Sets the version number for vme firmware 
     *
     */
    Fed9UDescription& setVmeFirmwareVersion(u32 version);
 
    /**
     * Sets the version number for delay firmware 
     *
     */
    Fed9UDescription& setDelayFirmwareVersion(u32 version);
 
    /**
     * gets the version number for fed  hardware
     *
     */
    u32 getEpromVersion() const;

     /**
     * gets the bus adaptor type
     *
     */
    Fed9UHalBusAdaptor getBusAdaptorType() const;
   
    /**
     * gets the version number for fed  hardware
     *
     */
    u32 getFedVersion() const;

    /**
     * gets the version number for fe Firmware 
     *
     */
    u32 getFeFirmwareVersion() const;

    /**
     * gets the version number for Be Firmware 
     *
     */
    u32 getBeFirmwareVersion() const;
    
    /**
     * gets the version number for Vme Firmware 
     *
     */
    u32 getVmeFirmwareVersion() const;

    /**
     * gets the version number for Delay Firmware 
     *
     */
    u32 getDelayFirmwareVersion() const;
  
    /**
     * \copydoc Fed9UABC::setDelay
     */
    Fed9UDescription& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay);
    
    /**
     * \copydoc Fed9UABC::setFineDelay
     */
    Fed9UDescription& setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay);
    
    /**
     * \copydoc Fed9UABC::setCoarseDelay
     */
    Fed9UDescription& setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay);

    /**
     * \copydoc Fed9UABC::setTrimDacOffset
     */
    Fed9UDescription& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset);
    
    
    /**
     * \copydoc Fed9UABC::setOptoRxInputOffset
     */
    Fed9UDescription& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 inputOffset);

    /**
     * \copydoc Fed9UABC::getOptoRxInputOffset
     */
     u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const;

    /**
     * \copydoc Fed9UABC::setOptoRxOutputOffset
     */
    Fed9UDescription& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 outputOffset);
    
    /**
     * \copydoc Fed9UABC::getOptoRxOutputOffset
     */
    u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const;

    /**
     * \copydoc Fed9UABC::setOptoRxCapacitor
     */
    Fed9UDescription& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 capacitor);
    
    /**
     * \copydoc Fed9UABC::getOptoRxCapacitor
     */
    u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const;


    Fed9UDescription& setApvDisable(const Fed9UAddress& fedApv, bool disable);
    
    //<GR date=27/07/2006> Added APV fake event disable
    /**
     * \brief  Removes an APV from a fake event by removing it tick marks.
     * \param  fedApv The FED APV number to be disabled in the fake event.
     * \param  disableFakeApv Boolean true disables the APV, false enables it.
     * \return Self reference
     */
    Fed9UDescription& setApvFakeEventDisable(Fed9UAddress fedApv, bool disable);
    //</GR>


    Fed9UDescription& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, std::vector<u16> medianOverride);
    

    Fed9UDescription& setScopeLength(u16 numberOfWords);
    Fed9UDescription& setFrameThreshold(const Fed9UAddress& channel, u16 channelThreshold);
    Fed9UDescription& setTriggerSource(Fed9UTrigSource triggerSource);    
    Fed9UDescription& setTestRegister(u32 testRegister);
    Fed9UDescription& setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable); 
    Fed9UDescription& setFedBeUnit(u16 beUnit);
    Fed9UDescription& setFedBeFpgaDisable(bool fedDisable);
    Fed9UDescription& setFedId(u16 fedId);
    Fed9UDescription& setFedHardwareId(u32 fedId);
    Fed9UDescription& setBeFpgaReadRoute(Fed9UReadRoute readRoute);
    Fed9UDescription& setAdcControls(const Fed9UAddress& channel, bool dfsen, bool dfsval, bool s1, bool s2);// TODO change from 4 bools to Strut input Fed9UAdcControls
    Fed9UDescription& setAdcControls(const Fed9UAddress& channel, const Fed9UAdcControls& adcControls);

    /**
     * \copydoc Fed9UABC::setDaqMode
     */
    Fed9UDescription& setDaqMode(Fed9UDaqMode fed9UDaqMode);

    /**
     * \copydoc Fed9UABC::setDaqSuperMode
     */
    Fed9UDescription& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode);

    /**
     * \copydoc Fed9UABC::setClock
     */
    Fed9UDescription& setClock(Fed9UClockSource clockSelect);

    /**
     * \copydoc Fed9UABC::setTempControl
     */
    Fed9UDescription& setTempControl(const Fed9UAddress& fedFpga, const Fed9UTempControl& tempControl);
    
    /**
     * \copydoc Fed9UABC::setComplement
     */
    Fed9UDescription& setComplement(const Fed9UAddress& channel, bool disable);
    
    /**
     * Updates the TTCrx description.
     *****************************************************/
    Fed9UDescription& setTtcrx(const Fed9UTtcrxDescription& ttcrxDescription);
    
    /**
     * This holds a copy of the serial EPROM memory.
     *****************************************************/
    Fed9UDescription& setEprom(const Fed9UEpromDescription& epromDescription);

    /**
     * This is used to configure the voltage monitor. It holds all the current read/write settings on the
     * voltage controller.
     *****************************************************/
    Fed9UDescription& setVoltageMonitor(const Fed9UVoltageControl& voltageController);

    /**
     * Getters for Fed9UDescription
     *****************************************/
     u16 getFineDelay(const Fed9UAddress& fedChannel) const;
     u16 getCoarseDelay(const Fed9UAddress& fedChannel) const;
     u16 getTrimDacOffset(const Fed9UAddress& fedChannel) const;
     bool getApvDisable(const Fed9UAddress& fedApv) const;

    //<GR date=27/07/2006> Added APV fake event disable
    /**
     * \brief  Returns the state of the disable APV from fake event setting.
     * \param  fedApv The FED APV number who's state we wish to know.
     * \return bool Boolean true means the APV is disabled, false it is enabled.
     */
    bool getApvFakeEventDisable(Fed9UAddress fedApv) const;
    //</GR>

    u32 getAllApvDisables(const Fed9UAddress& fedFeUnit) const;
    u16 getFakeEventRandomSeed(const Fed9UAddress& fedChannelPair) const;
    u16 getFakeEventRandomMask(const Fed9UAddress& fedChannelPair) const;

    //<JRF date="13/03/2008"> added comments
    /**
     * \brief  Sets the value for the fake event random seed, which is used to generate the random offsets for this channel pair
     * \param  fedChannelPair, the channel pair of which we are setting the random seed.
     * \param u16 value the value of the random seed.
     * \return self reference to the Fed9UDescription Object 
     */
    Fed9UDescription & setFakeEventRandomSeed(Fed9UAddress& fedChannelPair, u16 value);

    Fed9UDescription & setFakeEventRandomMask(Fed9UAddress& fedChannelPair, u16 value);

    /************************************************************
     * This will read the common mode (CM) median override settings on a FE FPGA.
     *
     * The CM median override settings are determined by two settings. These are represented 
     * by the arguments medianDisable and medianValue. If the median override values are 
     * enabled then medianOverrideDisable will return a false, if they are disabled it will 
     * return true. If the median override values are enabled (medianOverrideDisable == false) 
     * then medianValue will return the override values used by the FED, this has a maximum 
     * value of 1023.
     ************************************************************/
    
    std::vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const;

    u16 getScopeLength() const;
    u16 getFrameThreshold(const Fed9UAddress& channel) const;
    Fed9UStrips & getFedStrips() { return _strips; }
    const Fed9UStrips & getFedStrips() const { return _strips; }
    Fed9UTrigSource getTriggerSource() const;
    u32 getTestRegister() const;
    bool getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const;
    u32 getFedBeUnit() const;
    bool getFedBeFpgaDisable() const;
    u16 getFedId() const;
    u32 getFedHardwareId() const;


    Fed9UReadRoute getBeFpgaReadRoute() const;
    Fed9UAdcControls getAdcControls(const Fed9UAddress& channel) const;
    Fed9UDaqMode getDaqMode() const;
    Fed9UDaqSuperMode getDaqSuperMode() const;
    Fed9UClockSource getClock() const; 
    Fed9UTempControl getTempControl(const Fed9UAddress& fedFpga) const;
    bool getComplement(const Fed9UAddress& channel) const;

    /**
     * Returns all the current settings on the TTCrx device.
     */
    Fed9UTtcrxDescription getTtcrx() const;

    /**
     * Returns a copy of the 16KB serial EPROM memory stored on the FED.
     */
    Fed9UEpromDescription getEprom() const;

    /**
     * This returns a Fed9UVoltageMonitorInfo which contains all the current settings
     * on the voltage monitor.
     */
    Fed9UVoltageControl getVoltageMonitor() const;
    
    // <NAC date="25/04/2007>
    /**
     * \brief  Get one of the Fed9UFrontEndDescriptions.
     * \param  addr The address of the FE FPGA to return the description for.
     * \return Fed9UFrontEndDescription Description of the FE components linked
     *         to the specified FE FPGA.
     */
    Fed9UFrontEndDescription getFrontEndDescription(const Fed9UAddress& addr) const;
    /**
     * \brief  Set one of the Fed9UFrontEndDescriptions.
     * \param  addr The address of the FE FPGA to set the description for.
     * \param  theNewFrontEndDescription New description.
     * \return self reference
     */
    Fed9UDescription& setFrontEndDescription(const Fed9UAddress& addr, const Fed9UFrontEndDescription& theNewFrontEndDescription);
    // </NAC>

    /**
     * \name FED location parameters
     *
     * These parameters are required to ensure that the communication with the correct
     * FED is established. A real FED does not know its physical location and as such
     * this must be stored in the Fed9UDescription.
     */
    //@{

    /**
     * \copydoc Fed9UABC::setBaseAddress
     * \param   value Sets the value of the base address.
     */
    Fed9UDescription& setBaseAddress(const u32 value) { _baseAddress = value; return *this; }

    /**
     * \copydoc Fed9UABC::getBaseAddress
     */
    u32 getBaseAddress() const { return _baseAddress; }

    /**
     * \copydoc Fed9ABC::SetCrateNumber
     * \param   crateNumber Used to set the new crate number.
     */
    Fed9UDescription& setCrateNumber(u16 crateNumber);

    /**
     * \copydoc Fed9ABC::SetCrateNumber
     * \param   vmeContrllerDaisychainId Used to set the new daisyChainId.
     */
    Fed9UDescription& setVmeControllerDaisyChainId(u16 daisyChainId);

    /**
     * \copydoc Fed9UABC::getCrateNumber
     */
     u16 getCrateNumber() const;

    /**
     * \copydoc Fed9UABC::getVmeControllerDaisyChainId
     */
     u16 getVmeControllerDaisyChainId() const;

    /**
     * \copydoc Fed9UABC::setSlotNumber
     * \param   slotNumber Parameter that is used to pass the new slot number to be written.
     */
    Fed9UDescription& setSlotNumber(u8 slotNumber);

    /**
     * \copydoc Fed9UABC::getSlotNumber
     */
    u8 getSlotNumber() const;

    //@}



    /**
     *  Functions to be added. 20th October. 
     *
     * Do not use \deprecated Do not use these functions, 
     * they don't do anything, just allows the build to succeed 
     */
    

    
    //enumNamehere represents the register address but it is currently unknown what they are. this is a BE Fpga command only and so does not need a Fed9UAddress
    //void setTTCRXControl(u32 enumNameHere, u32 timingData) {}
    //u32 getTTCRXControl(u32 enumNameHere) {return 0;}
    
    Fed9UDescription& setName(std::string name);

     std::string getName() const { return std::string(_name);}
     std::string getVersion() const { return std::string(VERSION_TEXT(Description)); }
   /** End of new functions */
    
     Fed9UDescription & setHalAddressTable(const std::string val);

    //<JEC date=27/9/2007>
    //    const std::string & getHalAddressTable() const;
    std::string getHalAddressTable() const;
    //</JEC>

    //<JF date=18/03/2004>
    //Add get required buffer size.
    u32 getRequiredBufferSize() const;
    //</JF>      


  private:
    Fed9UDescription& setOptoRxOffset(const Fed9UAddress& fedFeUnit, u16 value);
    u16 getOptoRxOffset(const Fed9UAddress& fedFeUnit) const;
 
  public:
    // these methods should probably be private, but they are being used in the description creator.
    u16 getMedianOverride(const Fed9UAddress& fedApv) const;
    bool getMedianOverrideDisable(const Fed9UAddress& fedFeUnit) const;
    Fed9UDescription& setMedianOverride(const Fed9UAddress& fedApv, u16 medianOverride);
    
    /***********************************************************
     * This is used to either enable the common mode (CM) median override values or to disable them.
     *
     * The CM median override has a maximum value of 1023. Enabling the CM median override with no value
     * will effectivily disable the CM median subtraction. To enable the median override register
     * medianOverrrideDisable must be set to false. To disable them it must be set to true. 
     ***********************************************************/
    Fed9UDescription& setMedianOverrideDisable(const Fed9UAddress& fedFeUnit, bool medianOverrideDisable);


    /*****************************************************
     * This method returns a reference to the description object itself.
     * The purpose of this is to implement the pure virtual method which maps to the 
     * method in the VME Device, which returns a reference to the local fed settings, 
     * this is so that once can call this method on the base abstract base class. 
     *
     ************************************************/
    Fed9UDescription& getFed9UVMEDeviceDescription() { return *this; }

    /**************************************************
     *
     * inline function to obtain the global fine skew 
     * value. thiis is the amount by which all the 
     * delays at the fed front end are shifted. 
     *
     ************************************************/
    inline u16 getGlobalFineSkew()const{return _globalFineSkew;}
    
    /**************************************************
     *
     * setter, see getGlobalFineSkew(). dwisott.
     *
     ************************************************/
    inline Fed9UDescription & setGlobalFineSkew(u16 gs){
      _globalFineSkew=gs;
      return *this;
    }

    /**************************************************
     *
     * inline function to obtain the global coarse skew 
     * value. thiis is the amount by which all the 
     * delays at the fed front end are shifted. 
     *
     ************************************************/
    inline u16 getGlobalCoarseSkew()const{return _globalCoarseSkew;}
    
    /**************************************************
     *
     * setter, see getGlobalCoarseSkew(). dwisott.
     *
     ************************************************/
    inline Fed9UDescription & setGlobalCoarseSkew(u16 gs){
      _globalCoarseSkew=gs;
      return *this;
    }

    /**************************************************
     *
     * inline function to obtain the optorx resistor value should be 62 in V2 Feds
     *
     ************************************************/
    inline u16 getOptoRXResistor()const{return _optoRXResistor;}
    
    /**************************************************
     *
     * setter, see setOptoRxResistor(). sets the value for the RLOAD resistor should be 62 in V2 Feds
     *
     ************************************************/
    inline Fed9UDescription & setOptoRXResistor(u16 value){
      _optoRXResistor=value;
      return *this;
    }
    
    /**************************************************
     *
     * getter, gets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    std::string getFakeEventFile() const {
      return std::string(_fakeEventFile);
    }
    
    /**************************************************
     *
     * setter, sets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    inline Fed9UDescription & setFakeEventFile(std::string value){
      memcpy ( &(_fakeEventFile[0]),value.c_str(),(value.size()>200)?200:value.size());
      //      _fakeEventFile=value.c_str();
      //std::cout << __PRETTY_FUNCTION__ << "fake event file name being set to " << value << " and the array holds" << _fakeEventFile << std::endl;
      return *this;
    }

    u16 getFakeEventTriggerDelay() const {
      return _fakeEventTriggerDelay;
    }

    inline Fed9UDescription & setFakeEventTriggerDelay(u16 value) {
      _fakeEventTriggerDelay =  value;
      return *this;
    }
       
    //<JEC date=08/12/2005>
    //Added eventType to description
    /**
     * \copydoc Fed9UABC::setDaqEventType
     */
    Fed9UDescription& setDaqEventType(u16 eventType);

    /**
     * \copydoc Fed9UABC::getDaqEventType
     */
    u16 getDaqEventType(void) const;
    //</JEC>

    //<JEC date=08/12/2005>
    //Added DaqFov to description
    /**
     * \copydoc Fed9UABC::setDaqFov
     */
    Fed9UDescription& setDaqFov(u16 fov);

    /**
     * \copydoc Fed9UABC::getDaqFov
     */
    u16 getDaqFov(void) const;
    //</JEC>

    //<JEC date=09/01/2006>
    //Added TrackerHeaderMode to description
    /**
     * \copydoc Fed9UABC::getHeaderFormatType
     */
    Fed9UHeaderFormat getHeaderFormatType(void) const;

    /**
     * \copydoc Fed9UABC::setHeaderFormatType
     */
    Fed9UDescription& setHeaderFormatType(Fed9UHeaderFormat headerType);
    //</JEC>


    //<JEC date=23/02/06>
    //Added BunchCrossingOffset to description
    /**
     * \copydoc Fed9UABC::setBunchCrossingOffset
     */
    Fed9UDescription& setBunchCrossingOffset(u16 bxOffset);

    /**
     * \copydoc Fed9UABC::getBunchCrossingOffset
     */
    u16 getBunchCrossingOffset(void) const;
    //</JEC>


  private:
    //these methods are deprocated and will be removed soon
    u16 getAdcOffset(const Fed9UAddress& channel) const;
    Fed9UDescription& setAdcOffset(const Fed9UAddress& channel, u16 offset);
    Fed9UDescription& setChannelBufferOccupancy(const Fed9UAddress& channel, u16 channelOccupancy);
    u16 getChannelBufferOccupancy(const Fed9UAddress& channel) const;

  private:
    //<JEC date=26/9/2007>
    //    static std::string _fed9UAddressTable;
    char _fed9UAddressTable[200];
    //</JEC>
    template <typename T>
    void doLoop(Fed9UDescription & (Fed9UDescription::*setter)(const Fed9UAddress &, T), T value) {      std::string temp;

      Fed9UAddress a;
      for (int i = 0; i < FEUNITS_PER_FED; i++) {
	(this->*setter)(a.setFedFeUnit(i), value);
      }
    }
    template <typename T>
    void doLoop(Fed9UDescription & (Fed9UDescription::*setter)(const Fed9UAddress &, const T &), const T & value) {
      Fed9UAddress a;
      for (int i = 0; i < FEUNITS_PER_FED; i++) {
	(this->*setter)(a.setFedFeUnit(i), value);
      }
    }

    Fed9UHalBusAdaptor _busAdaptorType;
    u32 _feFirmwareVersion,_beFirmwareVersion, _vmeFirmwareVersion, _delayFirmwareVersion;
    u32 _fedVersion, _epromVersion;
    Fed9UStrips _strips; //
    Fed9UFrontEndDescription _feParams[FEUNITS_PER_FED];
    Fed9UTempControl _beTempControl;
    Fed9UTempControl _vmeTempControl;
    Fed9UTtcrxDescription _ttcrxDescription;
    Fed9UEpromDescription _epromDescription;
    Fed9UVoltageControl _voltageController;
    u32 _baseAddress;
    u16 _testRegister;//TOTO u32
    //TODO - u16 _beFpgaMode;
    u16 _beUnit; // TODO - deprocated - should remove this. Check with Gareth
    bool _fedDisable;
    u16 _fedId;
    u32 _fedHardwareId;
    Fed9UTrigSource _triggerSource; 
    Fed9UReadRoute _readRoute;
    Fed9UClockSource _clockMode;
    Fed9UDaqMode _fed9UDaqMode;
    Fed9UDaqSuperMode _fed9UDaqSuperMode;
    u16 _scopeLength;
    char _name[200];
    u16 _crateNumber;
    u16 _vmeControllerDaisyChainId;
    u16 _globalFineSkew;
    u16 _globalCoarseSkew;
    u16 _optoRXResistor;
    char _fakeEventFile[200];
    u16 _fakeEventTriggerDelay;
    u16 _eventType;
    u16 _fov;
    Fed9UHeaderFormat _headerType;
    u16 _bxOffset;
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UDescription& l, const Fed9UDescription& r);
  };
  
  // <NAC date="24/04/2007"> Operator to compare descriptions
  /**
   * \brief  == operator to allow Fed9UDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UDescription& l, const Fed9UDescription& r);
  /**
   * \brief  != operator to allow Fed9UDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UDescription& l, const Fed9UDescription& r) { return !(l==r); }
  // </NAC>
  
}

#endif // H_Fed9UDescription
#ifndef Fed9UEntry_HH
#define Fed9UEntry_HH

#include <vector>
#include <iostream>
#include <iomanip>

#define UNSET_SN 0
#define TBHARDWARE
#ifdef TBHARDWARE
#endif

namespace Fed9U
{

  /**
   * \brief  Fed9UEntry: class to represent and identify a FED in the crate, imple container for the serial number and base address.
   * \author Matt Noy, IC
   * \date:  04/2004
   *
   * 18-05-2004: Fed9UABC pointer added to enable the container to keep the pointers to the Fed9UABC representing real hardware.
   */
  class Fed9UEntry
  {
  public:

    /**
     * \brief Default constructor.
     *
     * Creates an entry with an UNSET serial number, zero base addres, slot number and sets the test me flag to true. If the variable TBHARDWARE
     * is defined then the FED pointer is set to null.
     */
    Fed9UEntry():
      mSerialNo(UNSET_SN),
      mBA(0),
      mSlotNumber(0),
      mTestMe(true)
#ifdef TBHARDWARE
	      ,
      mpFed(NULL)
#endif
    {}

    /**
     * \brief Constructor.
     * \param SN Serial number of the entry.
     * \param BA Base address of the FED entry.
     * \param slot Slot number of the entry.
     * \param f Pointer to a Fed9UABC, which will be used for the entry. Only present if TBHARDWARE is defined.
     *
     * The test me setting is also enabled.
     */
    Fed9UEntry(unsigned long SN, unsigned long BA, unsigned long slot
#ifdef TBHARDWARE 
, Fed9U::Fed9UABC * f=NULL):
#else 
      ):
#endif
      mSerialNo(SN),
      mBA(BA),
      mSlotNumber(slot),
      mTestMe(true)
#ifdef TBHARDWARE
    ,
      mpFed(f)
#endif
    {}

  /**
   * \brief Destructor.
   */
  ~Fed9UEntry()
  {
  }
  
  /**
   * \brief  The base address of a FED object defines the start of it memory address and also its physical location in the crate.
   * \return unsigned long Base address of the FED entry.
   */
  inline unsigned long getBaseAddress() const {return mBA;}

  /**
   * \brief  Each FED has a serial number, which defines it uniquely.
   * \return unsigned long Serial number of the FED entry.
   */
  inline unsigned long getSerialNumber() const {return mSerialNo;}

  /**
   * \brief  The physical location of the FED in a crate is defined by its slot number.
   * \return unsigned long Returns the slot number of the entry.
   */
  inline unsigned long getSlotNumber() const {return mSlotNumber;}    

  /**
   * \todo - find out what this does.
   */
  inline bool getTestFlag() const {return mTestMe;}    

  /**
   * \brief  Each FPGA on the FED has a temperature control monitor, that will cause the power to the FED to be cut in event of the FPGA over heating.
   * \return unsigned long Maximum temperature that the LM82 will allow the FPGA to reach before cutting power to the board.
   */
  //<GR date=14/08/2006>
  //Changed the getMaxLM82Temp to return an unsigned long instead of a bool as that is what it is stored as and set as.
  inline unsigned long getMaxLM82Temp() const {return mMaxLM82Temp;}
  //inline bool getMaxLM82Temp() const {return mMaxLM82Temp;}
  //</GR>

  /**
   * \brief The base address of a FED object defines the start of it memory address and also its physical location in the crate.
   * \param ba Sets the base address of the FED entry.
   */
  inline void setBaseAddress(unsigned long ba){mBA=ba;}

  /**
   * \brief Each FED has a serial number, which defines it uniquely.
   * \param sn Sets the serial number of the FED entry.
   */
  inline void setSerialNumber(unsigned long sn){mSerialNo=sn;}

  /**
   * \todo - find out what this does.
   */
  inline void setTestFlag(bool test){mTestMe=test;}

  /**
   * \brief The physical location of the FED in a crate is defined by its slot number.
   * \param slot Sets the slot number of the entry.
   */
  inline void setSlotNumber(unsigned long slot){mSlotNumber=slot;}    

  /**
   * \brief Each FPGA on the FED has a temperature control monitor, that will cause the power to the FED to be cut in event of the FPGA over heating.
   * \param temp Sets the maximum temperature that the LM82 will allow the FPGA to reach before cutting power to the board.
   */
  inline void setMaxLM82Temp(unsigned long temp){mMaxLM82Temp=temp;}    

#ifdef TBHARDWARE
  /**
   * \brief  The FED entry can be a real FED or software decription that is derived from Fed9UABC if TBHARDWARE is defined.
   * \return Fed9UABC Returns a pointer to the Fed9UABC entry in the class.
   */
  inline Fed9U::Fed9UABC * getFed9UABC(){return mpFed;}

  /**
   * \brief The FED entry can be a real FED or software decription that is derived from Fed9UABC if TBHARDWARE is defined.
   * \param f Sets the pointer to the Fed9UABC class that is used to access the FED object.
   */
  inline void setFed9UABC(Fed9U::Fed9UABC * f){mpFed=f;}

  /**
   * \brief Deletes the Fed9UABC object stored in the Fed9UABC pointer.
   */
  inline void purge() { delete mpFed; }
#endif

  /**
   * \brief  Checks the FED entry to see if a serial number has been set.
   * \return bool Boolean true if the serial number has been set.
   */
  inline bool isSerialNumberSet(){return mSerialNo!=0;} //Assumes a serial number of zero will never exist...

  /**
   * \brief Prints the serial number, base address and test to standard output, cout, along with some explanatory text.
   */
  inline void display() const {
    std::cout<<"Serial No = "<<std::hex<<mSerialNo
	     <<", BA = "<<std::hex<<mBA
	     <<", test = "<<std::dec<<mTestMe
	     <<std::endl;
  }

private:
  unsigned long mSerialNo;   //!< Stores the local copy of the serial number.
  unsigned long mBA;         //!< Stores the local copy of the base address.
  unsigned long mSlotNumber; //!< Stores the local copy of the slot number.
  bool mTestMe;              //!< \todo - find out what this does.
  u32 mMaxLM82Temp;          //!< Stores the local copy of the maximum temperature the LM82 will allow a FPGA or itself to reach.
#ifdef TBHARDWARE
  Fed9UABC * mpFed;          //!< If TBHARDWARE is defined, then this will store a pointer to the FED that is being used.
#endif

  };

}

#endif // Fed9UEntry_HH
#ifndef Fed9UCrateStatus_HH
#define Fed9UCrateStatus_HH

#include <vector>
#include <iostream>
#include <iomanip>


#define UNSET_SN 0


#ifdef TBHARDWARE
#endif

namespace Fed9U
{
//
// 18-05-2004
// public member prependEntry() added, to prepend a Fed9UEntry;
//

  /**
   * \brief  Fed9UCrateStatus: container class for the Fed9UEntry
   * \author Matt Noy
   * \date   04/2004
   *
   * Abstraction of a vector that should provide the ability to detect the crate, and iterate the testing sequence over the crate entries.
   */
  class Fed9UCrateStatus
  {
  public:

    /**
     * \brief Default constructor.
     *
     * Initialises the class to contain an empty set of Fed9UEntry's.
     */
    Fed9UCrateStatus();

    /**
     * \brief Destructor.
     *
     * Purges the FED entries.
     */
    ~Fed9UCrateStatus();
    

    /**
     * \brief  Each FED entry is stored in a vector element.
     * \param  pos Gives the position of the FED entry to be accessed.
     * \return Fed9UEntry Reference to the Fed9UEntry object stored in the given position.
     */
    Fed9UEntry & getFed9UEntry(unsigned long pos);

    /**
     * \brief  Allows a User to get the next Fed9UEntry in the list.
     * \param  f Reference to a Fed9UEntry object that the next entry is to be placed.
     * \return bool Boolean true if there is an entry to return, false if there is not.
     *         The reference to the Fed9UEntry will only be valid if this returns true.
     *
     * Fed9UEntry objects that have their test flag enabled will be skipped. See the
     * Fed9UEntry documentation for details on the test flag.
     */
    bool getNextFed9UEntry(Fed9UEntry & f);
    
    /**
     * \brief  Returns the number of Fed9UEntry objects in the sequence.
     * \return unsigned long Number of entries stored in the class.
     */
    inline unsigned long getNumberOfEntries(){return mFeds.size();}
   
    /**
     * \brief  Used to determine if the class contains any FED entries.
     * \return bool Boolean true if class is empty, false otherwise.
     */
    inline bool isEmpty(){return mFeds.empty();}
    
    /**
     * \brief Places the Fed9UEntry at the end of the sequence.
     * \param f Fed9UEntry to be placed at the end of the sequence.
     *
     * This function resets the internal iterator afterwards.
     */
    void appendFed9UEntry(Fed9UEntry f);

    /**
     * \brief Places the Fed9UEntry at the start of the sequence.
     * \param f Fed9UEntry to be placed at the end of the sequence.
     */
    void prependFed9UEntry(Fed9UEntry f);

    /**
     * \brief Resets the internal iterator to the start of the sequence
     */
    void resetIterator();

    /**
     * \brief Purges the contents of the object
     */
    void purge();

    /**
     * \brief calls Fed9UEntry::display on each element
     *
     * See Fed9UEntry documentation for more details on Fed9UEntry::display.
     */
    void display() const;
   
    /**
     * \brief  checks that each serial number is unique.
     * \return bool Returns true if any of the serial numbers duplicates, false if not.
     *
     * If an entry is unset then this will considered to be unique, even if there are multiple unset serial numbers.
     * areUnsetSerialNumbers should be consulted to check if this is the case.
     */
    bool areDuplicateSerialNumbers();

    /**
     * \brief  Checks if any feds in the crate have the serial number unset.
     * \return bool Boolean true if the serial numbers any serial numbers are unset.
     */
    bool areUnsetSerialNumbers();

   
  protected:

    /**
     * \brief Initialises the slot map.
     *
     * Slot map is defined by this class, so it can be modified if necessary and is used for accessing FED entries.
     */
    void init();

  protected:
    
    std::vector<Fed9UEntry> mFeds;                    //!< Holds each Fed9UEntry.
    std::vector<Fed9UEntry>::const_iterator mFedIter; //!< Used to iterate through the Fed9UEntry's in mFeds.

    static std::vector<unsigned long> mSlotBAMap;     //!< Map of the slot base addresses in a crate.

  }; // class Fed9UCrateStatus

} // namespace Fed9U

#endif // Fed9UCrateStatus_HH
/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 2002 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef _Fed9UStrX_H_
#define _Fed9UStrX_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include<iostream>
//using.*std::ostream;

#include <xercesc/util/XMLString.hpp>
//using xercesc::XMLString;

/**
 * \brief This is a simple class that lets us do easy (though not terribly efficient) trancoding of XMLCh data to local code page for display.
 */
class Fed9UStrX
{
public :

  /** \name Constructors and Destructor */
  //@{

  /**
   * \brief Constructor.
   * \param toTranscode A constant point to constant data of the XMLCh to decode to a normal C string.
   *
   * Takes an XMLCh and converts that to a C char*. The string in this form can be retrieved using localForm().
   */
  Fed9UStrX(const XMLCh* const toTranscode)
  {
    // Call the private transcoding method
    fLocalForm = xercesc::XMLString::transcode(toTranscode);
  }

  /**
   * \brief Destructor.
   *
   * Deletes the C string that the XMLCh was converted to.
   */
  ~Fed9UStrX()
  {
    delete [] fLocalForm;
  }
  //@}

  /** \name Getter methods */
  //@{

  /**
   * \brief  Return the C string of the XMLCh that was passed at construction.
   * \return const char* A read only pointer to the C string.
   */
  const char* localForm() const
    {
        return fLocalForm;
    }

private :

  char*   fLocalForm; //!< This is the local code page form of the string.
};

/**
 * \brief  Overload of the stream operator to write a C string made by the Fed9UStrX class to an output stream.
 * \param  target Output stream the string is to be written to.
 * \param  toDump Fed9UStrX object that is to be written to the stream.
 * \return ostream& Reference to the output stream the data was written to.
 */
inline std::ostream& operator<<(std::ostream& target, const Fed9UStrX& toDump)
{
    target << toDump.localForm();
    return target;
}

#endif

#endif

#endif

/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 2002 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef _Fed9UXStr_H_
#define _Fed9UXStr_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include <xercesc/util/XMLString.hpp>
using xercesc::XMLString;

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class Fed9UXStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    Fed9UXStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~Fed9UXStr()
    {
        delete [] fUnicodeForm;
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) Fed9UXStr(str).unicodeForm()


#endif

#endif

#endif


/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 2002 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef _Fed9UDOMCountErrorHandler_H_
#define _Fed9UDOMCountErrorHandler_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include <xercesc/dom/DOM.hpp>
using xercesc::DOMErrorHandler;
using xercesc::DOMError;

/**
 * \brief Inherited from the DOMErrorHandler this implements the XERCES error handling that is specific
 *        to the Fed9U XML/C++ interface.
 */
class Fed9UDOMCountErrorHandler : public DOMErrorHandler
{
public:
  /**
   * \brief Default constructor. Initialises the class with no errors seen.
   */
  Fed9UDOMCountErrorHandler();

  /**
   * \brief Destructor.
   */
  ~Fed9UDOMCountErrorHandler();

  /**
   * \brief  Used to determine if an error has been seen.
   * \return bool True if an error has been seen.
   */
  bool getSawErrors() const;

  /**
   * \brief  Prints the error message contained within a DOMError.
   * \param  domError DOMError class that is to be printed.
   * \return bool Returns false.
   */
  bool handleError(const DOMError& domError);

  /**
   * \brief Clears the saw errors status.
   */
  void resetErrors();


private :

  /**
   * \name Unimplemented constructors and operators.
   *
   * Copy construction and assignment declared private and unimplemented to override default implementation and
   * to prevent copying or assignment.
   */
  //@{
  Fed9UDOMCountErrorHandler(const Fed9UDOMCountErrorHandler&);
  void operator=(const Fed9UDOMCountErrorHandler&);
  //@}

  bool fSawErrors; ///< This is set if we get any errors, and is queryable via a getter method.
                   ///< It is used by the main code to suppress output if there are errors.
};

#endif

#endif

#endif
/**Matthew Pearson, Dec 2003
   Exception class for Fed9UXMLDescription which inherits from ICException*/

#ifndef _Fed9UXMLDescriptionException_H_
#define _Fed9UXMLDescriptionException_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UXMLDescriptionException class to be used when throwing errors within the Fed9UXMLDescription class.*/

#define FED9U_XMLDESCRIPTION_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_PARSE, "ERROR: An error occured during the XML parsing.") \
  IC_DEF_ERROR(ERROR_UNKNOWN_ATTRIBUTE_VALUE, "ERROR: An illegal attribute value has been found.") \
  IC_DEF_ERROR(ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND, "ERROR: An attribute that was expected has not been found.") \
  IC_DEF_ERROR(ERROR_ILLEGAL_ATTRIBUTE, "ERROR: You have set an attribute to an illegal value.") \
  IC_DEF_ERROR(ERROR_XML, "ERROR: caught XMLException.") \
  IC_DEF_ERROR(ERROR_DOM, "ERROR: caught DOMException.") \
  IC_DEF_ERROR(ERROR_FILE_OPEN, "ERROR: file failed to open.") \
  IC_DEF_ERROR(ERROR_NUM_APV, "ERROR: you have tried to set the medianOverride for a number of APVs which does not equal 24.") \
  IC_DEF_ERROR(ERROR_MEMORY, "ERROR: ran out of memory.") \
  IC_DEF_ERROR(ERROR_CLOB, "ERROR: Clob error from Database") \
  IC_DEF_ERROR(ERROR_NO_DATA, "ERROR: No data in database.") \
  IC_DEF_ERROR(ERROR_INCORRECT_USAGE, "ERROR: you do not have the correct combination of parameters for this method.") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught. This is not an XMLException or DOMException.")
  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UXMLDescriptionException, FED9U_XMLDESCRIPTION_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif

#endif

#endif
/**Matthew Pearson, 2/12/03. */
/**Class which reads in an XML stream and extracts the parameters in order to overide the default 
   Fed9UDescription. The constructor takes as a parameter a Fed9UDescription. The makeNewFed9UDescriptionFromStream
   method takes as a parameter the XML stream. This contains the XML information (the structure of which is defined by
   the Fed9USoftware XML schema file) which is used to set specific parameters in the Fed9UDescription, 
   depending on which XML tags and atttributes are in the stream. The stream does not have to contain a description
   of a whole FED, only a subset of it (for example, all the settings for a specified channel). However, if the user
   wishes, a complete XML description of a FED can be used by this class to completely override every parameter in a Fed9UDescription.
   This class provides the stream interface so that it can be reused by a derived class which will provided an iterface to, 
   for example, a file or a database.*/

#ifndef _Fed9UXMLDescription_H_
#define _Fed9UXMLDescription_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Xerces headers
#include <xercesc/dom/DOM.hpp>

//Fed9U headers

#include <string>

namespace Fed9U {

  //using xercesc::DOMNode;
  //using xercesc::DOMNamedNodeMap;
  //using xercesc::DOMDocument;
  //using xercesc::DOMInputSource;
  //using xercesc::DOMBuilder;

  //using.*std::istream;
  //using.*std::string;

class Fed9UXMLDescription
{

public:
  /**Constructor. This takes as arguments the XML file name and the Fed9UDescription to be modified.*/
  Fed9UXMLDescription(const std::string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Constructor. This takes in the Fed9UDescription to be modified. This is the constructor that should be used by derived classes.*/
  Fed9UXMLDescription(Fed9UDescription &defaultFed9UDescription , std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Destructor. This does some clean up.*/
  virtual ~Fed9UXMLDescription();

  /**Method which performs the Fed9UDescription override. It modifies the object member reference to the Fed9UDescription. 
     This method should only be used if the user has used the constructor which takes in a file name.*/
  std::vector<Fed9UDescription*>* makeNewFed9UDescription(void) throw (Fed9UXMLDescriptionException);

  /**Method which performs the Fed9UDescription overide. 
     It takes in a istream, containing the XML buffer, and modifies the object member reference to the Fed9UDescription.
     This method should be used by derived classes which use the stream interface to this class.*/
  std::vector<Fed9UDescription*>* makeNewFed9UDescriptionFromStream(std::istream &is) throw (Fed9UXMLDescriptionException);

  /**Method to get the value of usingStrips_*/
  bool getUsingStrips();

  /**Method to set the value of usingStrips_*/
  void setUsingStrips(bool usingStrips=true);
  
  /** Method to set the value of debugOutput_. If this is set to true the the buffer will be displayed to standard output for debugging */
  void setDebugOutput(bool value=true);

protected:
  /**Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.*/
  void initializeXerces(void) throw (Fed9UXMLDescriptionException);

  /**Method which will construct and initialize the DOMBuilder.*/
  void makeDOMBuilder(void) throw (Fed9UXMLDescriptionException);

  /**Method to recursively find the child elements under the given Node.*/
  void traverseChildElements(xercesc::DOMNode *n) throw (Fed9UXMLDescriptionException);

  /**Method which reads in a node, finds if it has attributes and sets these in the Fed9UDescription.*/
  void setFed9UDescription(xercesc::DOMNode *n) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the fed9U node.*/
  void setFed9UAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the feFpga node.*/
  void setFeFpgaAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
 
  /**Method which will set the attributes for the channel pair node.*/
  void setChannelPairAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
 
  /**Method which will set the attributes for the channel node.*/
  void setChannelAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
  
  /**Method which will set the attributes for the channel node.*/
  void setApvAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the strip node.*/
  void setStripAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
  
  /**Method which will set the attributes for the strips node.*/
  void setStripsAttribute(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method to get the integer node value from a DOMNamedNodeMap*/
  unsigned long getIntNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the signed integer node value from a DOMNamedNodeMap*/
  int getSignedIntNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap,  std::string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the float node value from a DOMNamedNodeMap*/
  float getFloatNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the string node value from a DOMNamedNodeMap*/
  std::string getStringNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);
  
  
  xercesc::DOMDocument *doc;
  xercesc::DOMLSParser *theDOMBuilder;
  Fed9UDOMCountErrorHandler errorHandler;

  std::string theXMLOverideFile;
  Fed9UAddress theFed9UAddress;
  Fed9UDescription &theFed9UDescription;
  std::vector<Fed9UDescription*> * theFed9UDescriptionList_; // this vector will contain the descriptions if there are more than one.
  u32 numberOfFeds_;
  unsigned short countApvs;
  bool usingStrips_;
  bool debugOutput_;
};


}
#endif
#endif
#endif
/**Matthew Pearson, 20/10/04.*/
/**This class enables the user to convert a XML file into a Fed9UDescription. It inherits functionality from 
   Fed9UXMLDescription. The XML file format must accord to the XML schema for the Fed9USoftware. 
   The XML file is used to set specific parameters in the Fed9UDescription, 
   depending on which XML tags and atttributes are in the file. The file does not have to contain a description
   of a whole FED, only a subset of it (for example, all the settings for a specified channel). However, if the user
   wishes, a complete XML description of a FED can be used by this class to completely override every parameter in a Fed9UDescription.*/

#ifndef _Fed9UXMLDescriptionFromFile_H_
#define _Fed9UXMLDescriptionFromFile_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers

//Std headers
#include<string>

namespace Fed9U {

  //using.*std::string;

  class Fed9UXMLDescriptionFromFile : public Fed9UXMLDescription
  {
  public:
    /**Constructor. This takes as arguments the XML file name and a reference to the Fed9UDescription to be modified.*/
    Fed9UXMLDescriptionFromFile(const std::string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips=true);

    /**Method which modifies the Fed9UDescription. 
     It modifies the object member reference to the Fed9UDescription.*/
    //    vector<Fed9UDescription*>*  makeNewFed9UDescription(void) throw (Fed9U::Fed9UXMLDescriptionException);

  private:
    std::string theXMLFileName;

  };

}

#endif
#endif

#endif //_Fed9UXMLDescriptionFromFile_H_
#ifndef _Fed9UDescriptionToXml_H_
#define _Fed9UDescriptionToXml_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Xerces headers
#include <xercesc/dom/DOM.hpp>

//Fed9U headers

//Standard headers
#include<string>

namespace Fed9U {

//   using xercesc::DOMDocument;
//   using xercesc::DOMWriter;
//   using xercesc::DOMElement;

  //using.*std::string;

  /**
   * \brief  Class which will write a Fed9UDescription to stream in XML format.
   * \author Matthew Pearson, RAL
   * \date   17/02/2004
   *
   * This class uses the Xerces 2.3 library. The input to the constructor is a reference to a Fed9UDescription.
   * This is then converted to an XML format stream using the streamOutDescription method. The XML stream will conform to
   * the XML schema for the Fed9USoftware. This class is intended to be a base class - a derived class can implement a
   * file/database/etc interface to the stream interface provided by this class.
   */
  class Fed9UDescriptionToXml
  {
  public:

    /*\name Constructors and destructors*/
    //@{

    /**
     * \brief Constructor. Writes a single description to a User defined XML file.
     * \param targetXMLFileName Name of the file to be written to.
     * \param Fed9UDescriptionToWrite Reference to the Fed9UDescription object that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This constructor can be used to directly create an XML file.
     */
    Fed9UDescriptionToXml(const std::string &targetXMLFileName,  Fed9UDescription &Fed9UDescriptionToWrite, bool usingStrips = true, bool usingBinaryStrips = true) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Constructor. Takes a Fed9UDescription object to write to the database.
     * \param Fed9UDescriptionToWrite Reference to Fed9UDescription object to be written.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * As no file name is set during construction this cannot be used to make an XML file directly and is for use with the database.
     */
    Fed9UDescriptionToXml(Fed9UDescription &Fed9UDescriptionToWrite , bool usingStrips = true, bool usingBinaryStrips = true) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Constructor. This takes a vector of Fed9UDescription pointers from which to make the XML file.
     * \param targetXMLFileName Name of the XML file to which the XML description is to be written.
     * \param Fed9UDescriptionToWrite Vector of Fed9UDescription pointers, each element contains a pointer to a Fed9UDescription object
     *        that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * All the Fed9UDescription objects pointed to by the elements of the Fed9UDescription vector will be written to the single file specified
     * by targetXMLFileName. Sufficient information is given to this constructor to produce an XML file.
     */
    Fed9UDescriptionToXml(const std::string &targetXMLFileName,  std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite , bool usingStrips = true, bool usingBinaryStrips = true) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Constructor. This takes a vector of Fed9UDescription pointers to write to the database.
     * \param Fed9UDescriptionToWrite Vector of Fed9UDescription pointers, each element contains a pointer to a Fed9UDescription object
     *        that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * Insufficient information is passed to the constructor to create an XML file, no file name is given. It is instead used to writing the  
     * to the database. All objects pointed to by the Fed9UDescription vector will be written to the database.
     */
    Fed9UDescriptionToXml( std::vector<Fed9UDescription*>  Fed9UDescriptionsToWrite , bool usingStrips = true, bool usingBinaryStrips = true) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Destructor.
     */
    virtual ~Fed9UDescriptionToXml();
    //@}


    /**
     * \brief Method which converts the Fed9UDescription to an XML buffer stream.
     * \param os Pointer to the output stream to write the XML data.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This must be called after using the constructor to this class which only takes in a reference to a Fed9UDescription and not an XML file name.
     */
    void streamOutDescription(std::ostream * os) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which converts the Fed9UDescription to an XML file.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This must be called after using the file based constructor to this class.
     */
    void writeXmlFile(void) throw (Fed9UXMLDescriptionException);

    /**
     * \brief  Whether to write the strip data into the  XML or not.
     * \return bool True if writing strip data to xml, false if not.
     */
    bool getUsingStrips();

    /**
     * \brief Whether to write the strip data to the xml.
     * \param usingStrips If set true then strip data is written, false not written. Defaulted to true.
     */
    void setUsingStrips(bool usingStrips=true);

    /**
     * \brief  Whether to write the strip data as a binary data chunk with a single XML tag for all strips or to write each bit of individual strip data
     *         to its own tag.
     * \return bool True if writing binary data, false if human readable.
     */
    bool getUsingBinaryStrips();

    /**
     * \brief Whether to write the strip data as a binary data chunk with a single XML tag for all strips or to write each bit of individual strip data
     *        to its own tag.
     * \param usingStrips If set true then binary data is written, false it is human readable. Defaulted to true.
     */
    void setUsingBinaryStrips(bool usingBinaryStrips=true);

    /**
     * \brief Debug print statements can be optionally sent to the standard cout stream.
     * \param value If true debug statements will be printed, false they will not. Defaulted to true.
     */
    void setDebugOutput(bool value=true);

  protected:

    /**
     * \brief Method which constructs the DOM document.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    virtual void constuctDOMDocument(void) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void initializeXerces(void) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which adds elements to a DOMDocument using the Fed9UDescription to get the attribute values.
     * \param rootElement Pointer to DOM element that will contain the data to be added to the DOM document.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    virtual void addDOMElements(xercesc::DOMElement *rootElement) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which extracts the FED parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains FED parameters.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addFedAttributes(xercesc::DOMElement* childElement) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which extracts the FE unit parameters from the Fed9UDescription and adds them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains FE unit parameters.
     * \param feId ID of the FE unit that is being addressed. Range:0-7, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addFeFpgaAttributes(xercesc::DOMElement* childElement, unsigned short feId=0) throw (Fed9UXMLDescriptionException);


    /**
     * \brief Method which extracts the channel Pair parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains channel pair parameters.
     * \param channelPairId ID of the FE unit channel pair whose parameters are being added. Range:0-5, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addChannelPairAttributes(xercesc::DOMElement* childElement, unsigned short channelPairId) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which extracts the channel parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains channel parameters.
     * \param channelId ID of the FE unit channel whose parameters are being added. Range:0-11, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addChannelAttributes(xercesc::DOMElement* childElement, unsigned short channelId=0) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which extracts the APV parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains APV parameters.
     * \param apvId ID of the channel APV whose parameters are being added. Range: 0-1, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addApvAttributes(xercesc::DOMElement* childElement, unsigned short apvId=0) throw (Fed9UXMLDescriptionException);

    /**
     * \brief Method which extracts the strip parameters from the Fed9UDescription and adds them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains strip parameters.
     * \param stripId ID of the APV strip whose parameters are being added. Range: 0-127, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addStripAttributes(xercesc::DOMElement* childElement, unsigned short stripId=0) throw (Fed9UXMLDescriptionException);
  
    /**
     * \brief Method which extracts all the APV strip parameters from the Fed9UDescription and adds them as a single element attribute.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains strip parameters.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     */
    void addStripsAttribute(xercesc::DOMElement* childElement) throw (Fed9UXMLDescriptionException);

    /**
     * \brief All the strip data on an APV can be encoded as a single chunk of binary data. This is in a non human readable form.
     *        This is used to return it in a human readable form.
     * \param stripsBuf A character point that will be filled with the strip data in a human readable form.
     */
    void getStripDataBuffer(char * stripsBuf) throw (Fed9UXMLDescriptionException);

    xercesc::DOMDocument *doc;                                    //!< DOM document where the XML data is to be stored.
    xercesc::DOMLSSerializer *theDOMWriter;                             //!< DOM writer object to write the data in the XML format.
    xercesc::DOMLSOutput *theDOMOutput;
    Fed9UDOMCountErrorHandler errorHandler;              //!< Fed9U DOM error handler used to detect errors and print the error strings.
    std::string theTargetXMLFileName;                         //!< Name of the XML file to be written to.
    Fed9UDescription &theFed9UDescription;               //!< Fed9UDescription object that is currently being dealt with.
    std::vector <Fed9UDescription*>  theFed9UDescriptionList; //!< Vector of all the pointers Fed9UDescription object to be processed.
    Fed9UAddress theFed9UAddress;                        //!< Fed9UAddress object used to for accessing the various parameters in the Fed9UDescription.
    bool usingStrips_;                                   //!< If true then the strip information for an APV is written as binary, false human readable.
    bool debugOutput_;                                   //!< If true then the debug printout is displayed during member functions calls that contain debug printout.
    bool usingBinaryStrips_;                             //!< If true then the xml buffer created puts the strips data into a binary blob, one for each APV. if false we have an xml tag per strip.
  };

}

#endif

#endif

#endif
#ifndef _Fed9UEventException_H_
#define _Fed9UEventException_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**
 * \brief Fed9UEventException class to be used when throwing errors within the Fed9UEvent class.
 */
#define FED9U_EVENT_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_DAQ_MODE_UNKNOWN, "ERROR: The daq mode set in the event data is unknown/ or unhandled.")  \
  IC_DEF_ERROR(ERROR_GET_SAMPLES_FAILED, "ERROR: The method getSamples() has failed to complete correctly.")  \
  IC_DEF_ERROR(ERROR_FRONT_END_PACKET_CODE_CORRUPT, "ERROR: The packet code on one of the front ends was corrupt.") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")

  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UEventException, FED9U_EVENT_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif

#endif

#endif
#ifndef H_Fed9UEvent
#define H_Fed9UEvent

#include "ICAssert.hh"
//#include "StopWatch.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  //  using namespace::HAL;
  
  class Fed9UEvent;

  /**
   * \brief An enumeration whose values are used to decode the packet code in the event.
   *
   * The packet code represents the data acquision (DAQ) mode that the event was taken in. Possible values are:
   * \li FED9U_PACKET_SCOPE Taken in scope mode.
   * \li FED9U_PACKET_VIRGRAW Taken in virgin raw mode.
   * \li FED9U_PACKET_ZEROSUPP Taken in zero suppressed mode.
   * \li FED9U_PACKET_PROCRAW Taken in processed raw data mode.
   * \li FED9U_PACKET_VIRGRAW_10BIT packed in 10 bit, valid for both ZS and VR only
   * \li FED9U_PACKET_ZEROSUPP_10BIT packed in 10 bit, valid for both ZS and VR only
   * \li FED9U_PACKET_ZEROSUPP_LO Valid for ZS only, here we striped the lowest 2 bits of the 10 bit data to pack into 8 bits
   * \li FED9U_PACKET_ZERO_SUPP_HI_LO Valid for ZS only, here we stripped 1 upper and 1 lower bit to pack 10 into 8 bits. 
   * A. Baty: Added VR bit stripped versions (7/7/2015)
   
   * More details of the different types of DAQ mode can be found in the CMS FED FE FPGA documentation.
   */
  enum { FED9U_PACKET_SCOPE = 0xE1, FED9U_PACKET_VIRGRAW = 0xE6, FED9U_PACKET_VIRGRAW_10BIT = 0x86,
         FED9U_PACKET_ZEROSUPP = 0xEA, FED9U_PACKET_PROCRAW = 0xF2, FED9U_PACKET_ZEROSUPP_10BIT = 0x8A,
	     FED9U_PACKET_ZEROSUPP_LO = 0xCA, FED9U_PACKET_ZEROSUPP_HI_LO = 0xAA,
//Added for bit stripped moded (AAB 8/24/2015) 
             FED9U_PACKET_PROCRAW_10BIT = 0x92,
             FED9U_PACKET_PROCRAW_8BIT_HI_LO = 0xB2,
             FED9U_PACKET_PROCRAW_8BIT_LO = 0xD2,
             FED9U_PACKET_VIRGRAW_8BIT_HI_LO = 0xA6,
             FED9U_PACKET_VIRGRAW_8BIT_LO = 0xC6
  };
  
  /** 
   * \brief Fed9UEventIterator class is designed to be used within the Fed9UEvent Class.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   *
   * This class can be used to iterate through the Fed9UEvent buffer to exam the various bytes in the buffer.
   */
  class Fed9UEventIterator {
  public:

    /**
     * \brief Default Constructor.
     * \param buffer Point to the character buffer that the event is stored in. Defaulted to NULL.
     * \param length Length of the buffer that is given during construction. Defaulted to zero.
     * \param offset An offset into the data buffer to initially place the pointer.
     */
    Fed9UEventIterator(u8 * buffer = 0, u32 length = 0, u32 offset = 0 )
      : _buf(buffer), _len(length), _off(offset) { }

    /**
     * \brief  Used to set the iterator to a new event buffer.
     * \param  buffer Point to the character buffer that the event is stored in. Defaulted to NULL.
     * \param  length Length of the buffer that is given during construction. Defaulted to zero.
     * \param  offset An offset into the data buffer to initially place the pointer.
     * \return Self reference.
     */
    Fed9UEventIterator & set(u8 * buffer = 0, u32 length = 0, u32 offset = 0) {
      _buf = buffer; _len = length; _off = offset; return *this;
    }

    /**
     * \brief  Sets a new size of the event buffer.
     * \param  Length The new length of the buffer.
     * \return Self reference.
     */
    Fed9UEventIterator & resize(u32 length) { (*this) = normalize();  _len = length; return *this; }

    /**
     * \brief Deletes the event buffer.
     */
    void clear() {  if (_buf != NULL ) delete[] _buf; }

    /**
     * \brief  Used to determine the size of the event buffer.
     * \return unsigned long The event size.
     */
    u32 size() const { return _len; }

    /**
     * \brief Moves the buffer pointer along to the next 32-bit word boundary
     *        within the buffer, ensuring that the internal offset is adjusted accordingly
     */
    Fed9UEventIterator normalize() const {
      u32 diff = _off & ~3;
      return Fed9UEventIterator(_buf + diff, _len, _off - diff);
    }
    
    /**
     * \brief  Get a pointer to the byte this is pointing to
     * \return constant unsigned char pointer
     */
    const u8* getPtr() const {
      return &(_buf[0]);
    }

    /**
     * \brief  Used to get a byte from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the byte.
     * \return unsigned char Byte that is retrieved from this offset location.
     */
    u8 getu8(u32 offset) const {
      return (*this)[offset];
    }

    /**
     * \brief  Used to get a 16-bit word of data from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the word.
     * \param  swap If true then the word will be byte swapped, if false it will not. Defaulted to false.
     * \return unsigned short Word that is retrieved from this offset location.
     */
    u16 getu16(u32 offset, bool swap = false) const {
      if (swap) return ((*this)[offset] << 8) |  (*this)[offset+1];
      else      return  (*this)[offset]       | ((*this)[offset+1] << 8);
    }

    /**
     * \brief  Used to get a 32-bit word of data from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the word.
     * \param  swap If true then the word will be byte swapped, if false it will not. Defaulted to false.
     * \return unsigned long Word that is retrieved from this offset location.
     */
    u32 getu32(u32 offset, bool swap = false) const {
      if (swap) return (getu16(offset, swap) << 16) |  getu16(offset+2, swap);
      else      return  getu16(offset, swap)        | (getu16(offset+2, swap) << 16);
    }

    /**
     * \brief  Used to deference the iterator and access the data at its current location.
     * \return unsigned char The byte that is at the current location.
     */
    const u8 & operator * () const { return (*this)[0]; }

    /**
     * \brief  Provides similar functionality to getu8, returning a byte at a given location.
     * \param  offset Offset into the buffer from which to read the byte.
     * \return unsigned char Constant reference to the byte at the offset location.
     */
    const u8 & operator [] (u32 offset) const {
      ICUTILS_VERIFY(offset < _len)(offset)(_len).error();
      return _buf[(_off + offset) ^ 3];
    }

    /**
     * \brief  Subtracts the values of the bytes of two different Fed9UEventIterators at their current location in the buffer.
     * \param  lhs Left hand side Fed9UEventIterator, rhs is subtracted from this objects byte.
     * \param  rhs Right hand side Fed9UEventIterator, this byte is subtracted from the lhs.
     * \return signed long Result of the subtraction.
     */
    friend i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs);

    //Fed9UEventIterator & operator ++ () { ++_off; return *this; }
    //const Fed9UEventIterator & operator ++ const () { ++_off; return *this; }
    //Fed9UEventIterator operator ++ (int) {
    //  Fed9UIterator temp(*this);
    //  _off++;
    //  return temp;
    //}

    /**
     * \brief  Used to increment the iterator onto the next byte.
     * \param  int Not used just part of the operator definition syntacs.
     * \return Fed9UEventIterator A copy of the Fed9UEventIterator object that has been incremented.
     */
    Fed9UEventIterator operator ++ (int) {
      Fed9UEventIterator temp(*this);
      (*this) += 1;
      return temp;
    }

    //Fed9UEventIterator & operator -- () { --_off; return *this; }
    //const Fed9UEventIterator & operator -- const () { --_off; return *this; }
    //Fed9UEventIterator operator -- (int) {
    //  Fed9UEventIterator temp(*this);
    //  _off--;
    //  return temp;
    //}
    //const Fed9UEventIterator operator -- const (int) {
    //  Fed9UEventIterator temp(*this);
    //  _off--;
    //  return temp;
    //}

    /**
     * \brief  Moves the iterator on by given amount.
     * \param  rhs Relative amount by which to increment the iterator offset by.
     * \return Fed9UEventIterator Reference to the original Fed9UEventIterator once it has been incremented.
     */
    Fed9UEventIterator & operator += (u32 rhs) {
      ICUTILS_VERIFY(rhs <= _len)(rhs)(_len).error();
      _off += rhs;
      _len -= rhs;
      return *this;
    }

    /**
     * \brief  Adds a given amount to the current offset poistion of a Fed9UEventIterator.
     * \param  lhs Fed9UEventIterator whose offset into the buffer is to move.
     * \param  rhs Amount that is to be added to the offset.
     */
    friend Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, u32 rhs);

    /**
     * \brief Prints the start address of the buffer, offset and length.
     */
    void debug(std::ostream & os) const {
      os << std::dec << reinterpret_cast<void*>(_buf) << '+' << _off << ':' << _len;
    }

  private:

    u8 * _buf;          //!< Pointer to the start of the buffer.
    u32 _len, _off;     //!< Legnth (_len) of current buffer and offset (_off) into the buffer.
    bool _deleteBuffer; //!< True if the buffer it to be delete. \todo - check.
  };

  inline i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs) {
    return (lhs._buf + lhs._off) - (rhs._buf + rhs._off);
  }
  
  inline Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, int rhs) {
    Fed9UEventIterator temp(lhs); return temp += rhs;
  }

  /**
   * \brief Stores the channel data for the event.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   *
   * Provides access to all the settings that are stored in a given channel in the event.
   */
  class Fed9UEventChannel {
  public:

    /**
     * \brief Constructor.
     * \param data A Fed9UEventIterator pointing to the start of that channel.
     * \param status APV status for that channel. Each channel contains the status of the APVs and whether the ticker was able to lock to
     *        both APVs, if it was out of synch or an error was sent from the APV. getStatus contains more information.
     */
    Fed9UEventChannel(Fed9UEventIterator data, u8 status);

    /*
    Fed9UDaqMode mode() const;
    int operator[] (size_t i) const {
      ICUTILS_VERIFY((i*2+1) < _data.size())(i*2+1)(_data.size())
        .msg("Index out of bounds").error();
      return _data[i*2] | _data[i*2+1] << 8;
    }
    */

    /**
     * \brief  Used to get the status for the channel APVs.
     * \return unsigned char The six status bits, which are as follows:
     *         \li 0: APV0 error - the ticker received an error from the APV1 chips.
     *         \li 1: wrong header APV0 - the header found for this APV is wrong, according to the winner.
     *         \li 2: APV1 error - the ticker received an error from the APV1 chips.
     *         \li 3: wrong header APV1 - the header found for this APV is wrong, according to the winner.
     *         \li 4: out of sync - the ticker has locked for the APVs on this channel, but it is out of sync with the event pulse
     *                from the rest of the APV on a FE unit. 
     *         \li 5: lock - the ticker has locked onto something.
     * The polarities of the bits are set such that 0x3f (all 1's) indicates a fully working channel.
     */
    u8  getStatus() const { return _status; }

    /**
     * \brief Returns the packet code for the event that was recorded.
     *
     * This can be decoded using the anonamous enum declared in this file.
     */
    u8  getPacketCode() const { return _data[2]; }

    /**
     * \brief  Returns the median value recorded during a zero suppressed run for that APV.
     * \param  apv The channel APV that the median should be retrieved for. Range:0-1.
     * \return unsigned short Median value for that APV.
     *
     * Only valid for zero suppressed runs.
     */
    u16 getMedian(unsigned apv) const {
      ICUTILS_VERIFY(apv <= 1)(apv).error();
      ICUTILS_VERIFY(
						getPacketCode() == FED9U_PACKET_ZEROSUPP ||
					    getPacketCode() == FED9U_PACKET_ZEROSUPP_10BIT ||
						getPacketCode() == FED9U_PACKET_ZEROSUPP_LO ||
						getPacketCode() == FED9U_PACKET_ZEROSUPP_HI_LO
						)(getPacketCode()).error();
      return _data.getu16(3 + apv*2);
    }

    /**
     * \brief  Length of the data on that channel.
     * \return unsigned long Data length on that channel, in bytes.
     */
    u32 dataLength() const { return _data.getu16(0); }

    /**
     * \brief  Returns the number of ADC samples recorded for that channel.
     * \return unsigned long Number of samples recorded.
     */
    u16 samples() const;

    /**
     * \brief  Returns the value of a specific sample.
     * \param  i Sample number to be returned.
     * \return unsigned short Number of ADC counts recorded in that sample.
     */
    u16 sample(std::size_t i) const;

    /**
     * \brief Returns all the samples.
     * \param destBuff Pointer to a unsigned short buffer that will contain all the sample values. The buffer mst be as large as the
     *        value from Fed9UEventChannel::samples().
     */
    void getSamples(u16 * destBuff) const;

    /**
     * \brief  Returns all the samples.
     * \return std::vector<unsigned short> A std::vector containing all the ADC counts for each recorded strip. Vector should be as large as
     *         the value from Fed9UEventChannel::samples().
     */
    std::vector<u16> getSamples() const;

    //<RB date=22/07/05>
    //Added method to get the channel iterator.
    /**
     * \brief  Returns channel interator
     * \return Fed9UEventIterator Constant reference to the iterator that points to this channel.
     */
    inline const Fed9UEventIterator& getIterator() const {return _data;}
    //</RB>

  private:
    u8 _status;               //!< APV status bits for that channel.
    Fed9UEventIterator _data; //!< Fed9UEventIterator that is used to iterate through the channel.
  };

  /**
   * \brief  Breaks the event into header and FE unit data sections, allowing access to each of the FE units in the event.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   */
  class Fed9UEventUnit {
  public:
    /**
     * \brief Constructor.
     * \param header Fed9UEventIterator that points to the start of the event header for that FE unit.
     * \param data Fed9UeventIterator that points to the start of the event data for that FE unit.
     * \param headerformat A Fed9UHeaderFormat object that defines the structure of the event header.
     */
    Fed9UEventUnit(Fed9UEventIterator header, Fed9UEventIterator data, Fed9UHeaderFormat headerformat);

    /**
     * \brief  Gives access to one of the channels stored in the event.
     * \param  i FED channel number. Number of channel can be found in Fed9UEventUnit::channels()
     * \return Fed9UEventChannel Constant reference to the requested channel.
     */
    const Fed9UEventChannel & channel(std::size_t i) const ;

    /**
     * \brief  Returns the majoritory pipeline from the ticker voting for that FE unit.
     * \return unsigned char Majoritory pipeline.
     *
     * The majoritory pipeline is the value that after voting on the APV headers was determined. All valid APV signals will
     * will agree with this value. Only valid in full debug mode.
     *
     * \todo DONE! - check that this is correct . <JRF date="27/06/2008"> this was wrong... I have changed the byte which is read, it is now correct.
     */
    u8  getMajorityPipeline() const { 
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("getMajorityPipeline only valid in full debug mode");
      return _header.getu8(14); 
    }

    /**
     * \brief  Total length of all the data recorded is stored within the event data.
     * \return unsigned long Length of the data recorded.
     */
    u32 dataLength() const { 
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("dataLength only valid in full debug mode");
      return (_data.size()+7) & ~7; 
    }

    /**
     * \brief  Returns the number of channels that were found in the event.
     * \return unsigned short Number of channels in the event.
     */
    u16 channels() const { return _channels.size(); }

    /**
     * \brief  Read the middle 32-bit word in the 2nd 64-bit word of the FE header.
     * \return unsigned long Returns the bytes 10-13 inclusive. It can be used to read the DAQ register for example, which is
     *  in this position in the 8th FE header in a full debug mode event.
     */
    u32 getBytes10To13FromFeHeader(void) const {
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("getBytes10To14FromFeHeader only valid in full debug mode");
      return (_header.getu32(10, true) & 0xFFFFFFFF); 
    }
  private:

    Fed9UEventIterator _header, _data;   //!< Fed9UEventIterators to store the header and data iterators.
    Fed9UHeaderFormat _headerFormat;     //!< Data member for store the Fed9UHeaderFormat object, which defines structure of the header.
    std::vector<Fed9UEventChannel> _channels; //!< Vector to hold all of the channels that are recorded in the event.
  };

  /**
   * \class   Fed9UEvent
   * \brief   Class to read in and check the event buffer for errors.
   * \author  Jonathan Fulcher
   * \author  Emlyn Corrin
   * \version 0.0
   * \date    21/08/2003
   *
   * Breaks the event into its component parts providing access to the header, trailer, data (divided into channels) using the
   * supporting classes Fed9UEventUnit, Fed9UEventChannel and Fed9UEventIterator.
   */
  class Fed9UEvent {

  public:

    /**
     * \brief Enables or disables the debug print out for the member functions in this class or Fed9UEventChannel, Unit and Iterator.
     * \param on If true then the debug output is enabled, false disabled.
     */
    static void EnableDebug(bool on = true);

    /**
     * \brief Default constructor.
     *
     * Creates an empty event.
     */
    Fed9UEvent() : _buffer(0), _rubbish(0), _fedDescription(0), _deleteBuffer(false), _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_OLD_VME) {}
    
    /**
     * \brief Constructor.
     * \param is An input stream from which to create the event.
     *
     * Loads an event from an input stream, most typcially one that has been previously saved to file.
     */
    Fed9UEvent(std::istream & is);

    /**
     * \brief Constructor.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in. Defaulted to 0.
     *
     * Creates a Fed9Uevent from a live buffer, stored in a unsigned long array. It requires the current description that the
     * is being run with. The size of the buffer is optional, but desired.
     */
    Fed9UEvent(u32 * buffer, const Fed9UDescription * currentDescription,
               u32 bufsize = 0) :  _buffer(0),_rubbish(0), _fedDescription(0), _deleteBuffer(false), _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_OLD_VME) {
      Init(buffer, currentDescription, bufsize);
    }

    /**
     * \brief Initialises the Fed9UEvent buffer.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in.
     *
     * Splits up the event creating local copies of the header, data which is split into channels, initialising the iterators.
     */
    void Init(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);

    /**
     * \brief Initialises the Fed9UEvent buffer, performing a CRC check on the event data.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in.
     *
     * Provides the same functionality as Init, however it also performs a CRC check as well.
     */
    void DebugInit(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize, std::string& report, u32& readCrc, u32& calcCrc);

    /**
     * \brief Destructor.
     *
     * Deletes the event buffer.
     */
    ~Fed9UEvent();

    /**
     * \brief Checks the event size matches that stated in the event, also performs a CRC check is performed
     */
    void checkEvent() const;
    //void debugCRC() const;

    /**
     * \brief Checks all the status bits (see Fed9UEventChannel::status) on each channel to ensure there was no timing errors
     *        with the APVs.
     * \throw Fed9UEventExpection This exception is thrown, containing a detail error string, if there are any problems with the status bits.
     */
    void checkChannelStatuses() const;

    /**
     * \brief  The data acquisions (DAQ) mode is stored in the event buffer.
     * \return Fed9UDaqMode Returns the DAQ mode saved in the event buffer, in the form of a Fed9UDaqMode enum.
     */
    Fed9UDaqMode getDaqMode() const;
    
    /**
     * \brief  The data acquisions (DAQ) Super mode is stored in the event buffer.
     * \return Fed9UDaqSuperMode Returns the DAQ Super mode saved in the event buffer, in the form of a Fed9UDaqSuperMode enum.
     */
    Fed9UDaqSuperMode getDaqSuperMode() const;
  
    /**
     * \brief  Number of events that have occured since the last reset of the counter.
     * \return unsigned long Event number.
     */
    u32 getEventNumber() const;

    //<JRF date="28/11/2007"> 
    /**
     *
     *the event Type is taken from the 4 bits in the header of the event data the modes are labelled by the follwing decimal values:
     *1 = Scope Mode REAL
     *2 = Virgin Raw Mode REAL
     *3 = Virgin Raw Mode FAKE
     *6 = Processed Raw Mode REAL
     *7 = Processed Raw Mode FAKE
     *8 = Spy Mode REAL
     *10 = Zero Suppressed Mode REAL
     *11 = Zero Suppressed Mode FAKE
     *12 = Zero Suppressed Lite Mode REAL
     *13 = Zero Suppressed Lite Mode FAKE\todo - find out more information about this.
     */
    u8  getEventType() const;

    /**
     * \brief  The bunch crossing number that the event was saved during is recorded in the event.
     * \return unsigned short Returns the bunch crossing number.
     */
    u16 getBunchCrossing() const;

    /**
     * \todo - find out more information about this.
     */
    u16 getSourceId() const;

    /**
     * \todo - find out more information about this.
     */
    u8  getFormatVersion() const;

    /**
     * \brief  Data mode describes the basic type of data you have here, fake, real or legacy data.
     * \return Fed9UEventDataMode An enum that defines the possible types of data that can be processed by the FED.
     *
     * Events can either be generated by the optical inputs, real data, or from the FE units, fake data. When loading fake data
     * it is possible it can either be a real event loaded from file creating a full fake event. It is also possible for a fake
     * event to be created from a simple .apv file, which contains data for only an APV pair. In this case the data will be the
     * same across all FED channels and only a single channel will be unique. There is also support for a legacy data mode, which
     * had a different header structure and as such requires a slightly different decoding. This method is used to determine
     * what type/format of data is being viewed.
     */
    Fed9UEventDataMode getDataMode() const { return _mode ; }

    /**
     * \todo - find out more information about this.
     */
    Fed9UHeaderFormat getTrackerHeaderFormat() const { return _headerFormat; }

    /**
     * \brief  Method to set the data mode to legacy in order to interpret old style data from early beam tests in 2004/5.
     * \return Self reference.
     */
    Fed9UEvent& setLegacyDataMode() { _mode = FED9U_EVENT_MODE_LEGACY; return *this; }

    /**
     * \brief  Returns the CRC number from the event trailers.
     * \return unsigned short The CRC number.
     */
    u16 getCrc() const;

    /**
     * \brief  Calculates the CRC number for the event.
     * \return unsigned short Calculated CRC number.
     */
    u16 calcCrc() const;

    /**
     * \brief  Debug version that calcs CRC without other information guesses position of crc based on length provided.
     * \return unsigned short The calculated CRC.
    */
    u16 calcCrcDirty() const;

    /**
     * \brief  Method to read the DAQ register from the 8th FE header in a full debug mode event.
     * \return unsigned long DAQ register present in the event. Returns zero if there is no full debug mode event.
     */
    u32 getDaqRegister(void) const;


    /**
     * \brief Method to read the DAQ register Two from the 7th FE header in a full debug mode event.                                                                                                
     * \return unsigned log DAQ register two present in the event. Returns zero if there is no full debug mode event.
     */
    u32 getDaqRegisterTwo(void) const;


    /**
     * \brief  Method to read the BE status register from the 1st FE header in a full debug mode event. 
     * \return unsigned long Back end status register bits. It returns zero if the event is not a full debug mode event.
     */
    u32 getBeStatusRegister(void) const;

    /**
     * \name Tracker special header components.
     *
     * Methods to read the tracker special header. 
     * These methods rely on the SPECIAL_OFF private data member to locate the 
     * position of the tracker special header in the FED buffer.
     */
    //@{

    /**
     * \brief  Read first byte of tracker special header (should be 0xED while old and new headers co-exist).
     * \return unsigned char Byte from tracker special header.
     *
     * \todo - This method will have to renamed or replaced when these bits are used for something else.
     */
    u8 getSpecialFirstByte(void) const;

    /**
     * \brief  Read the header format (4 bits) from the tracker special header.
     * \return Fed9UHeaderFormat Returns the value of the header format. See the Fed9UHeaderFormat documentation for more details.
     */
    Fed9UHeaderFormat getSpecialHeaderFormat(void) const;

    /**
     * \brief  Read the tracker event type (4 bits) from the tracker special header.
     * \return unsigned char Event type, whose values are defined below.
     *
     * This is the 'FED mode', the possible values are:
     * \li 1:Scope mode.
     * \li 2:Virgin raw mode.
     * \li 6:Processed raw mode.
     * \li 10:Zero suppressed mode.
     * \li 12:Zero suppressed lite mode.
     * \li 8:Spy mode.
     */
    u8 getSpecialTrackerEventType(void) const;

    /**
     * \brief  Read the APVE Address (8-bits) from the tracker special header.
     * \return unsigned char APVE address.
     *
     * This is the APVE address sent to the FED over the TTC channel B. It should
     * match the real address in the APV headers.
     */
    u8 getSpecialApvEmulatorAddress(void) const;
      
    /**
     * \brief  Read the APV address error (8-bits) from the tracker special header.
     * \return unsigned char APV address error.
     *
     * Each bit returned by this method indicates if the APV address for that FE FPGA
     * is the same as the APVE address. A 1 means they are identical, and a 0 means they are not.
     * Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.
     */
    u8 getSpecialApvAddressError(void) const;

    /**
     * \brief  Read the FE enable register (8-bits) from the tracker special header.
     * \return unsigned char A 1 means the FE unit was enabled, 0 disabled.
     *
     * Each bit returned by this method indicates if the corresponding FE FPGA is
     * enabled (able to accept data). This information is needed to decode the FED
     * buffer when operating in APV error header mode. If set to 0 then there will
     * be no payload data from that particular FE FPGA. Bit 0 corresponds to FE FPGA
     * 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.
     */
    u8 getSpecialFeEnableReg(void) const;

    /**
     * \brief  Read the FE overflow register (8-bits) from the tracker special header.
     * \return unsigned char Value of the overflow register.
     *
     * If a bit is set to 1 then a buffer overflow has occured in the corresponding 
     * FE FPGA, and the data from that FPGA should be ignored.
     * Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.*/
    u8 getSpecialFeOverflowReg(void) const;

    /**
     * \brief  Read the FED status register (16-bits) from the tracker special header.
     * \return unsigned short Value of the status register contained in the special header.
     * Each bit returned by this method indicates the state of a particular part of the FED.
     * \li Bit 0 = Slink full flag.
     * \li Bit 1 = Tracker Header Monitor data ready.
     * \li Bit 2 = QDR Memory full flag.
     * \li Bit 3 = QDR Memory partial full flag.
     * \li Bit 4 = QDR Memory empty flag.
     * \li Bit 5 = L1A FIFO Full flag.
     * \li Bit 6 = L1A FIFO Partial Full flag.
     * \li Bit 7 = L1A FIFO Empty flag.*/
    u16 getSpecialFedStatusRegister(void) const;
    //@}
      
      //Fed9UEventChannel & getChannel(Fed9UAddress);
      //Fed9UEventChannel & getChannel(int fedFeUnit, int feUnitChannel);

    /**
     * \brief Used to write the event data to a stream in a format the scientific display program Igor can understand.
     * \param o Output stream used to write the data to.
     */
    void saveIgorFile(std::ostream & o) const;

    /**
     * \brief Writes the contents of the event buffer to an output stream in a human readable format.
     * \param ostream Reference to the output stream the data is to be written to.
     */
    void dump(std::ostream&) const;

    /**
     * \brief Writes the contents of the event buffer to an output stream in the format as it was received from the FED.
     * \param os Output stream to write the raw data to.
     *
     * The raw data will be arranged into words that are 64-bits per line. 64-bit words is what the FED sends the data as.
     */
    void dumpRawBuffer(std::ostream& os) const;
    const Fed9UEventUnit & feUnit(std::size_t i) const {
      ICUTILS_VERIFY(i < feUnits())(i)(feUnits()).msg("Index out of bounds").error();
      return _feunits[i];
    }

    /**
     * \brief  Gives the Fed9UEventChannel object for a given FE unit channel.
     * \param  unit FE unit to be addressed.
     * \param  channel FE unit channel to be addressed.
     * \return Fed9UEventChannel Constant reference to the specified Fed9UEventChannel object.
     */
    const Fed9UEventChannel & channel(std::size_t unit, std::size_t channel) const;

    /**
     * \brief  Returns the total length of the buffer.
     * \return unsigned long Total length of the buffer.
     */
    u32 getTotalLength() const;

    /**
     * \brief  Returns the number of FE units that are recorded in the event.
     * \return unsigned short Number of FE units.
     */
    u16 feUnits() const { return _feunits.size(); }

    /**
     * \brief  Returns the total number of channels that are recorded in the event.
     * \return unsigned short Number of channels.
     */
    u16 totalChannels() const {
      u16 ret = 0;
      for (int i = 0; i < feUnits(); i++) {
        ret += feUnit(i).channels();
      }
      return ret;
    }

    /**
     * \brief  Returns the appropriate event data for a given FED channel.
     * \param  fedChannel FED channel number to be returned.
     * \return Fed9UEventChannel Constant reference to the event channel that was requested.
     */
    const Fed9UEventChannel & channel(u8 fedchannel) const {
      for (int i = 0; i < feUnits(); i++) {
        if (fedchannel < feUnit(i).channels()) {
	  return feUnit(i).channel(fedchannel);
	}
	fedchannel -= feUnit(i).channels();
      }
      ICUTILS_VERIFY(false)(static_cast<int>(Fed9UAddress(fedchannel).getExternalFedChannel()))(totalChannels()).msg("Bad channel number").error();
      THROW(ICUtils::ICException("Bad channel number"));
    }

    /**
     * \brief  Returns the appropriate event data for a given FED channel.
     * \param  fedChannel The FED channel is given by the Fed9UAddress object and as such does not require any specific type of channel number scheme to be used.
     * \return Fed9UEventChannel Constant reference to the event channel that was requested.
     */
    const Fed9UEventChannel & channel( Fed9UAddress fedChannel ) const {
      return channel(fedChannel.getFedChannel());
    }

    /**
     * \brief  Returns the number of samples that are present on the first channel.
     * \return unsigned short Number of samples in channel.
     *
     * \todo - extend to read all channels and check for consistancy.
     */
    u16 samples() const {
      return channel(0).samples();
      // should really check all channels are consistent
    }

  // <NAC date="21/09/2007"> made getFeBufferLength protected and added getters for data members for use by derrived class
  protected:
    /**
     * \brief  Returns the buffer length in number of bytes of a given FE unit.
     * \param  feUnit FE unit that is to be addressed.
     * \return int Number of bytes in the buffer. 
     */
    int getFeBufferLength(int feUnit) const;
    
    /**
     * \brief  Get the buffer
     * \return const Fed9UEventIterator& buffer
     */
    const Fed9UEventIterator& getBuffer() const { return _buffer; }
    
    /**
     * \brief  Get the packet payload
     * \return const Fed9UEventIterator& payload
     */
    const Fed9UEventIterator& getPayload() const { return _payload; }
    
    /**
     * \brief  Get a pointer to the description used
     * \return const Fed9UDescription* description
     */
    const Fed9UDescription* getDescription() const { return _fedDescription; }
    
    /**
     * \brief  Get the header format
     * \return Fed9UHeaderFormat header format
     */
    Fed9UHeaderFormat getHeaderFormat() const { return _headerFormat; }
    
    /**
     * \brief  Get the event format
     * \return Fed9UEventFormatType event format
     */
    Fed9UEventFormatType getEventFormat() const { return _eventFormat; }
    
    /**
     * \brief  Get the offset of the tracker special header (in bytes)
     * \return u16 offset
     */
    u16 getSpecialOffset() const { return SPECIAL_OFF; }
   // </NAC>

  private:
    /**
     * \name Copy constructor and assignment operator.
     *
     * Declared private and unimplemented to prevent copying of event classes.
     */
    //@{
    Fed9UEvent(const Fed9UEvent &);
    Fed9UEvent & operator = (const Fed9UEvent &);
    //@}

    Fed9UEventIterator _buffer, _trailer, _payload;     //!< Fed9UEventIterators that point to the buffer, trailer and payload respectively.
    std::vector<Fed9UEventUnit> _feunits;     //!< Vector contains an element for each of the FE units recorded in the event.
    u16 _rubbish;                             //!< Used to place data that has been read out but is not required by that specific member function.
    const Fed9UDescription * _fedDescription; //!< Pointer to the Fed9UDescription that contains the settings on the FED that were present when the event was taken.
    static const u16 SPECIAL_OFF = 8;         //!< Offset of tracker special header in bytes.
    bool _deleteBuffer;                       //!< Used to flag if the buffer is to be deleted.
    Fed9UEventDataMode _mode;                 //!< Type of data that was recorded (fake, real or legacy), as defined by Fed9UEventDataMode.
    Fed9UHeaderFormat _headerFormat;          //!< Local copy of the Fed9UHeaderFormat that is used to define the header format used in the event.
    //<JEC date=11/4/07> new internal variable to handle 32-bit swapping in DAQ header and trailer
    Fed9UEventFormatType _eventFormat;        //!< Format of the event record (original VME ordering or new common VME/SLINK ordering.
    //</JEC>

  };

  inline std::ostream& operator<<(std::ostream& os, const Fed9UEvent & ev) { ev.dump(os); return os; }

}

#endif // H_Fed9UEvent
/** Jo Cole September 2006 */

#ifndef H_Fed9UFakeBufferCreator
#define H_Fed9UFakeBufferCreator

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using namespace std;

  /** New fake event buffer construction class.  Used by streamlined event unpacking objects when using fake events.*/

  class Fed9UFakeBufferCreator {
    
  public:
    /** Default constructor. */
    Fed9UFakeBufferCreator() : _mode(FED9U_EVENT_MODE_FAKE_FULL), _size(0) {};
    /** Standard constructor. */
    //added new parameter, see .C file for details (8/18/2015 AAB)
    Fed9UFakeBufferCreator(istream & is, int uniqueChannels = 1);

    /** Default destructor. */
    ~Fed9UFakeBufferCreator();

    /** Returns the size of the created buffer (in bytes).*/
    u32 getBufferSize() const {return _size;}
    /** Returns a point to the created buffer.*/
    u32 * getPointerToBuffer() const {return _buffer;}
    /** Returns the data mode (required by Fed9UEventStreamLine when using fake events).*/
    Fed9UEventDataMode getDataMode() const {return _mode;}

  private:
    u32 * _buffer;
    Fed9UEventDataMode _mode;
    u32 _size; // buffer size in bytes
    
  };
  
}

#endif // H_Fed9UFakeBufferCreator
/** Jo Cole February 2007 */

#ifndef H_Fed9USu8
#define H_Fed9USu8

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  //<JEC date=21/03/07>
  /** Replacement for Fed9UEventIterator class.  Used in Fed9UEventStreamLine and Fed9UEventUnitStreamLine.*/
  
  class Fed9USu8 {
    
  public:
    /** Default constructor. */
    Fed9USu8() : _dataPtr(0), _off(0), _initialOffset(0) {}
    /** Standard constructor. */
    Fed9USu8(u8 * dataPtr) : _dataPtr(dataPtr), _off(0), _initialOffset(0) {}
    Fed9USu8(u8 * dataPtr, u32 offset) : _dataPtr(dataPtr), _off(offset), _initialOffset(offset) {}
    /** Default destructor. */
    ~Fed9USu8() {}

    /** Separate method to set the private data members. */
    void set(u8 * dataPtr = 0, u32 offset = 0) {_dataPtr = dataPtr; _off = offset; _initialOffset = offset;}
    /** Reset the offset to whatever it was either when it was instantiated or when the last set command was called.*/
    void reset() {_off = _initialOffset;}
    /** Delete the pointer.*/
    void clear() {  if (_dataPtr != NULL ) delete[] _dataPtr; }
    /** Return the value from the buffer at a specific offset from where the buffer is pointing to.
     *  Bear in mind that if _off != 0, it will play a role in deciding which value you return.
     */
    u8 getu8(u32 offset) const {return (*this)[offset];}
    /** Determines if the pointer has been set yet or not.*/
    bool comparePtr() const {
      if (_dataPtr == NULL) {return true;}
      return false;
    }

    /** getter for the underlying u8 pointer.*/
    u8 * getPointer() const {return _dataPtr;}
    /** getter for the value of the offset.*/
    u32 getOffset() const {return _off;}
    
    /** overloaded [] operator.  Takes care of the data packing versus data access ordering in the buffer. */
    u8 & operator [] (u32 offset) const {return _dataPtr[(_off + offset) ^ 3];}
    /** overloaded dereference operator.  Returns value at the current location of the pointer. 
     *  NB.  The value of _off matters !*/
    u8 & operator * () const { return (*this)[0]; }

    /** overloaded prefix and postfix increment operators.*/
    Fed9USu8 & operator ++ () {++_off; return *this;}
    Fed9USu8 operator ++ (int) {
      Fed9USu8 temp(*this);
      (*this) += 1;
      return temp;
    }

    /** overloaded increment and assign operator.*/
    const Fed9USu8 & operator += (u32 rhs) {_off += rhs; return *this;}
    const Fed9USu8 & operator += (int rhs) {_off += static_cast<u32>(rhs); return *this;}

    /** overloaded + operator.*/
    const Fed9USu8 & operator + (u32 rhs) {_off += rhs; return *this;}
    
    /** overloaded prefix decrement operator and decrement and assign operator.*/
    Fed9USu8 & operator --() {
      ICUTILS_VERIFY(_off > 0)(_off).error();
      --_off; return *this;
    }
    Fed9USu8 & operator -=(u32 rhs) {
      ICUTILS_VERIFY(_off > rhs)(_off)(rhs).error();
      _off -= rhs; return *this;
    }
    
    /** overloaded assignment operator*/
    const Fed9USu8 & operator =(const Fed9USu8 & rhs) {
      if (this == &rhs) {return *this;}
      _dataPtr = rhs.getPointer();
      _off = rhs.getOffset();
      _initialOffset = rhs.getInitialOffset();
      return *this;
    }
    
  private:
    /** getter for the initial offset value.  This should only be needed within this class, as initialOffset should only
     * be set when the object is instantiated or the set command is called, when it is set equal to the main offset.
     * Needed for the assignment operator.*/
    u32 getInitialOffset() const {return _initialOffset;}
    
    /** The actual pointer for which this class is a wrapper.*/
    u8 * _dataPtr;
    /** _off is an offset which is needed if you want to access anything other than the first byte of the 
     * buffer when you use the overloaded dereference operator.
     *
     * _initialOffset is an extra offset in case you want to reset the value of _off.  This is needed
     * in Fed9UEventUnitStreamLine when returning a Fed9USu8 special pointer.*/
    u32 _off, _initialOffset;
  };
  //</JEC>
}

#endif // H_Fed9USu8
/*   Exception class for Fed9UEventStreamLine which inherits from ICException*/

#ifndef _Fed9UEventStreamLineException_H_
#define _Fed9UEventStreamLineException_H_

//Check if using Xerces version 1
//#if XERCES > 1

//Check Xerces version
//#include "xercesc/util/XercesVersion.hpp"
//#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UEventStreamLineException class to be used when throwing errors within the Fed9UEventStreamLine class.*/

#define FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED9UEVENTSTREAMLINE, "ERROR:  An error occurred in Fed9UEventStreamLine.")  \
  IC_DEF_ERROR(ERROR_FEUNIT_DISABLED, "ERROR:  Attempt to access a disabled FE-unit.")  \
  IC_DEF_ERROR(ERROR_DAQ_MODE_UNKNOWN, "ERROR: The daq mode set in the event data is unknown/ or unhandled.")  \
  IC_DEF_ERROR(ERROR_GET_SAMPLES_FAILED, "ERROR: The method getSamples() has failed to complete correctly.")  \
  IC_DEF_ERROR(ERROR_FRONT_END_PACKET_CODE_CORRUPT, "ERROR: The packet code on one of the front ends was corrupt.") \
  IC_DEF_ERROR(ERROR_FEUNIT_OUT_OF_BOUNDS, "ERROR:  Input FE-FPGA number must run between 1 and 8. ") \
  IC_DEF_ERROR(ERROR_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input channel number must run between 1 and 12. ") \
  IC_DEF_ERROR(ERROR_APV_OUT_OF_BOUNDS, "ERROR:  Input APV number must be either 1 or 2. ") \
  IC_DEF_ERROR(ERROR_BAD_FRONT_END_PACKET_CODE, "ERROR: Information requested which does not exist in this payload format.") \
  IC_DEF_ERROR(ERROR_DATA_BUFFER_CORRUPT, "ERROR: Data buffer is either empty or corrupted.") \
  IC_DEF_ERROR(ERROR_FED_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input FED channel number must run between 1 and 96. ") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")

  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UEventStreamLineException, FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

//#endif

//#endif

#endif
/** Jo Cole June 2006 
 *  Updated March 2007   
 */

#ifndef H_Fed9UEventUnitStreamLine
#define H_Fed9UEventUnitStreamLine

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using std::vector;

  /**  New simplified FE-FPGA class - used by the new streamlined Fed event unpacking class.
       NB.  When a method requires the FE-unit or channel number, you should give the number of the FPGA/channel you want, NOT
       the location in the header/payload.  For example, FE-FPGA number 8 is always first in the header or payload, so give
       Fe-unit number = 8 to access the information associated to it.  A similar approach should be used for the channel numbering,
       bearing in mind that channel numbers are quoted with respect to the associated FE-FPGA, not with respect to the FED, ie.
       channels are numbered 1 - 12, not 1 - 96.
  */
  //<JEC date=21/03/07>  several changes including the use of the u8 pointer wrapper, Fed9USu8.
  
  class Fed9UEventUnitStreamLine {
    
  public:
    /** Default constructor.*/
    Fed9UEventUnitStreamLine() : _headerFormat(FED9U_HEADER_FULLDEBUG), _unit(0) {
      _header.set(NULL);
      _payload.set(NULL);
      _sample.set(NULL);
    };
    /** Standard constructor, which should be used for normal data.
	It should be given a pointer to the start of the FE-Unit-related header information, a pointer to the start of
	the associated payload data, the Header format (FULL DEBUG or APV ERROR) and the unit number.*/
    Fed9UEventUnitStreamLine(u8 * header, u8 * payload, Fed9UHeaderFormat headerFormat, u32 unit);

    /** Returns a pointer to the end of the payload data (including the padding).*/
    u8 * getPointerToEndOfUnitPayload();
    /** Returns either the frame synch out packet (FULL DEBUG mode) or the APV error flags (APV ERROR mode), as stored in the 
	tracker-specific header.  The information is returned in a u8 vector, with the LSB stored in element 0.  
	The vector should be three elements long for the APV error mode and sixteen elements long for FULL DEBUG mode.*/
    vector<u8> getFeUnitHeader() const;
    /** Returns the status of a specified channel, as stored in the header.  If the data is in FULL DEBUG mode, then the
	status code will be 6 bits long, but if it is in APV ERROR mode, then it will be only 2 bits long.*/
    u8 getChannelStatus(u32 channel) const;
    /** Returns the data length for a given FE-FPGA, as stored in the FULL DEBUG mode of the tracker-specific header.
	If the header is in APV ERROR mode, then the length is simply zero.*/
    u16 getUnitDataLengthFromHeader() const;
    /** Returns the majority APV pipeline address, as stored in the FULL DEBUG mode of the tracker-specific header.
        If the header is in APV ERROR mode, then the address is simply zero.*/
    u8 getMajorityPipelineAddress() const;
    /** Returns the packet code, as stored in the payload, which identifies the mode which the data was taken.
	The options are:
	Scope mode                - packet code = xxx0 0101
	Virgin raw data mode      - packet code = xxx0 0110
	Processed raw data mode   - packet code = xxx1 0010
	Zero suppressed mode      - packet code = xxx0 1010
	Zero suppressed lite mode - packet code not stored in payload - method returns zero.
	Only the lower 5 bits are returned.  The upper three bits are reserved.*/
    u8 getPacketCode(u32 channel) const;
    /** Returns the median for a specified APV on a specific channel, as stored in the payload.  
	This information is only stored in Zero Suppressed mode.*/
    u16 getMedian(u32 channel, u32 apv) const;
    /** Returns the data length for a specified channel, as stored in the payload.
	The length is given in bytes and includes the lengths and packet code.*/
    u16 getChannelDataLength(u32 channel) const {return _channelDataLength[12 - channel];}
    /** Returns the total data length for a given FE-FPGA, based on information stored in the payload.*/
    u16 getUnitDataLengthFromPayload() const;
    /** Returns the number of data words for a given channel.  For SCOPE, VIRGIN RAW and PROCESSED RAW data modes, the
     actual number of raw data words is returned.  For the ZERO SUPPRESSED modes the maximum number of clusters is returned.*/
    u16 getSamples(u32 channel) const;
    /** Returns a Fed9USu8 pointer to the data from a specific channel.  To get a pointer to the start of the data,
	simply set dataLoc = 0.  Other values of dataLoc can be used if a pointer to a specific cluster or raw data word is required.*/
    //<JEC date=21/03/07>
    Fed9USu8 & getSampleSpecialPointer(u32 channel, u32 dataLoc);
    //</JEC>
    /** Returns a data sample at a specific location for a specific channel.
	dataLoc = 0 corresponds to the first piece of data in the payload (ie. after length, packet code and median information, where stored).
	Raw data for Scope, Virgin Raw and Processed Raw modes is stored in two bytes.  In ZS and ZS lite, the cluster data is stored in
	bytes, so using this method will return two pieces of cluster data or'ed together.*/
    u16 getSample(u32 channel, u32 dataLoc) const;
    /** Returns a vector containing the data samples for a specified channel.
	For Raw data modes, each vector element contains a two-byte data word, while for the ZS modes each vector element contains one byte.
	Use for time-critical applications.*/
    vector<u16> getSample(u32 channel) const;
    //<JEC date=18/4/07> extra method for use in Fed9UDevice (TrimDAC calibration)
    /** Retrieves only the data (ie. no strip addresses or cluster widths) for a given channel and stores them in a pre-defined buffer to which
	destBuffer is a pointer.*/
    void getSample(u32 channel, u16 * destBuffer);
    //</JEC>
    /** Returns a pointer to the position of the data from a specified FE-FPGA in the payload.*/
    //<JEC date=21/03/07> changed to return a special u8 pointer
    Fed9USu8 getFEUnitInPayloadPtr() const {return _payload;}
    //</JEC>
    /** Performs a consistency check (where possible) of channel data lengths.  Returns true if everything is OK.
	For ZERO SUPPRESSED modes channel data lengths are not expected to be the same, so the return value is always set to true.*/
    bool checkChannelLengths();

  private:

    /** The different possible packet codes for the different data formats.  ZS LITE is not included as the packet code
	is not stored in this data format.*/
    enum { FED9U_PACKET_SCOPE = 5, FED9U_PACKET_VIRGRAW = 6, FED9U_PACKET_ZEROSUPP = 10, FED9U_PACKET_PROCRAW = 18 };
    //AAB updated with more modes AAB (12/8/205)
    enum { FED9U_PACKET_SCOPE_hex = 0xE1, FED9U_PACKET_VIRGRAW_hex = 0xE6, FED9U_PACKET_VIRGRAW_10BIT = 0x86,
         FED9U_PACKET_ZEROSUPP_hex = 0xEA, FED9U_PACKET_PROCRAW_hex = 0xF2, FED9U_PACKET_ZEROSUPP_10BIT = 0x8A,
         FED9U_PACKET_ZEROSUPP_LO = 0xCA, FED9U_PACKET_ZEROSUPP_HI_LO = 0xAA,
         FED9U_PACKET_PROCRAW_10BIT = 0x92,
         FED9U_PACKET_PROCRAW_8BIT_HI_LO = 0xB2,
         FED9U_PACKET_PROCRAW_8BIT_LO = 0xD2,
         FED9U_PACKET_VIRGRAW_8BIT_HI_LO = 0xA6,
         FED9U_PACKET_VIRGRAW_8BIT_LO = 0xC6
     };
        
    /** Private method which returns the location of two consecutive bits of data.  The data is packed into each 64-bit word 
	starting from the MSB and running to the LSB, but the access to the data starts from the middle of the 64-bit word, as
	illustrated:
	63                              0
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |              --> byte-order for data packing
	+---+---+---+---+---+---+---+---+
	| 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 |              --> byte-order for data access
	+---+---+---+---+---+---+---+---+
	
	This routine takes the given channel and location within that channels data in the data packing format and converts it
	into the location of two consecutive words in the data access format.  The location is returned in terms of a number of
	full 64-bit words and then the locations of the two words with respect to the last (incomplete) 64-bit word.*/
    void getLocations(u32 channel, u32 offset, int &nOf64BWords, int &lowerWd, int &upperWd) const;
    int getLocations(u32 channel, u32 offset) const;
    
    /** A pointer to the start of the FE-Unit-specific data in the event header.*/
    Fed9USu8 _header, _payload, _sample;
    /** The header format:  FULL DEBUG or APV ERROR.*/
    Fed9UHeaderFormat _headerFormat;
    /** FE-Unit number.*/
    u32 _unit;
    /** Vector containing the data length for each channel.*/
    vector<u16> _channelDataLength;
  };
  //</JEC>  
}

#endif // H_Fed9UEventUnitStreamLine
/** Jo Cole June 2006 
 *  Updated March 2007
 *  Updated April 2007 to handle 32-bit word swapping
 */

#ifndef H_Fed9UEventStreamLine
#define H_Fed9UEventStreamLine

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {
  
  using namespace std;
  
  class Fed9UEventUnitStreamLine;
  
  /**  New streamlined FED event unpacking class.
       NB.  When a method requires the FE-unit or channel number, you should give the number of the FPGA/channel you want, NOT
       the location in the header/payload.  For example, FE-FPGA number 8 is always first in the header or payload, so give
       Fe-unit number = 8 to access the information associated to it.  A similar approach should be used for the channel numbering,
       bearing in mind that channel numbers are quoted with respect to the associated FE-FPGA, not with respect to the FED, ie.
       channels are numbered 1 - 12, not 1 - 96.
  */
  
  class Fed9UEventStreamLine {
    
  public:
    /** Default constructor.*/
    Fed9UEventStreamLine() : 
      _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_STANDARD), _headerFormat(FED9U_HEADER_FULLDEBUG), 
      _fedDescription(0), _rubbish(0), _bufferLength(0), _deleteBuffer(false) 
    {
      fakeBuffer = NULL;
      _daqHeader = NULL;
      _trackerHeader = NULL;
      _trailer = NULL;
      _feUnitHeader = NULL;
      _payload = NULL;      
    };
    /** Constructor for handling fake events.  Uses Fed9UFakeBufferCreator class to construct the buffer.*/
    //(AAB 8/24/2015) Added support for multiple unique channels loaded in fake events, let uniqueChannels = 1 for other behavior
    Fed9UEventStreamLine(istream & is, int uniqueChannels);
    /** Constructor for handling real data events */
    Fed9UEventStreamLine(u32 * buffer, const Fed9UDescription * currentDescription);
    /** Default destructor. */
    ~Fed9UEventStreamLine();
    
    /** Initialisation method.  Used by all constructors to create a set of pointers to different parts of the header, payload 
	and trailer, regardless of whether the event is real or fake.*/
    void Init(u32 * buffer, const Fed9UDescription * fedDescription);    
    /** Fills a vector with pointers to a set of Fed9UEventUnitStreamLine objects (one for each active FE-FPGA).*/
    void setPointerToUnitDataInPayload();
    
    // methods for extracting information from the DAQ header

    /** Returns the L1 event number returned by the TTC system, as stored in the DAQ header.*/
    //<JEC date=11/4/07> handle 32-bit swapping of DAQ header and trailer
    //    u32 getEventNumber() const {return (*(_daqHeader + 2) << 16 | *(_daqHeader + 1) << 8 | *_daqHeader);}
    u32 getEventNumber() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 2) << 16 | *(_daqHeader + 1) << 8 | *_daqHeader);
      } else {
	return (*(_daqHeader + 6) << 16 | *(_daqHeader + 5) << 8 | *(_daqHeader + 4));
      }
    }
    /** Returns the Event type identifier, as stored in the DAQ header. 
	For details of the meaning of this number, see
	http://cmsdoc.cern.ch/cms/TRIDAS/horizontal/ */
    //    u8 getDaqEventType() const {return (*(_daqHeader + 3) & 0xF);}
    u8 getDaqEventType() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 3) & 0xF);
      } else {
	return (*(_daqHeader + 7) & 0xF);
      }
    }
    /** Returns the bunch crossing number, as stored in the DAQ header.
	The number is reset every orbit.*/
    //    u16 getBunchCrossingNumber() const {return (*(_daqHeader + 7) << 4 | ((*(_daqHeader + 6) >> 4) & 0xF));}
    u16 getBunchCrossingNumber() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 7) << 4 | ((*(_daqHeader + 6) >> 4) & 0xF));
      } else {
	return (*(_daqHeader + 3) << 4 | ((*(_daqHeader + 2) >> 4) & 0xF));
      }
    }
    /** Returns the unique data source identifier, as stored in the DAQ header.
	For the tracker, this number will lie in the range 50 - 489. */
    //    u16 getSourceId() const {return (*(_daqHeader + 5) | (((*(_daqHeader + 6)) & 0xF) << 8));}
    u16 getSourceId() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 5) | (((*(_daqHeader + 6)) & 0xF) << 8));
      } else {
	return (*(_daqHeader + 1) | (((*(_daqHeader + 2)) & 0xF) << 8));
      }
    }
    /** Returns the version identifier of the common FED encapsulation, as stored in the DAQ header.
        The value is set by the DAQ group. */
    //    u8 getDaqFormatVersion() const {return ((*(_daqHeader + 4) >> 4) & 0xF);}
    u8 getDaqFormatVersion() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return ((*(_daqHeader + 4) >> 4) & 0xF);
      } else {
	return ((*(_daqHeader + 0) >> 4) & 0xF);
      }
    }
    //</JEC>

    // methods for extracting information from the tracker-general header

    /** Returns the identifier for the tracker header type, as stored in the tracker-specific header.
	This has two possible values:
	0001 = Full Debug mode,
	0010 = APV Error mode.*/
    Fed9UHeaderFormat getHeaderFormat() const;
    /** Returns the event or payload type identifier, as stored in the tracker-specific header.
	This has several possible values:
	0001 = Scope Mode REAL
	0010 = Virgin Raw Mode REAL
	0011 = Virgin Raw Mode FAKE
	0110 = Processed Raw Mode REAL
	0111 = Processed Raw Mode FAKE
	1010 = Zero Suppressed Mode REAL
	1011 = Zero Suppressed Mode FAKE
	1100 = Zero Suppressed Lite Mode REAL
	1101 = Zero Suppressed Lite Mode FAKE
	
	Note:  The event format for FAKE data is the same as for the corresponding REAL data mode.*/
    u8 getTrackerEventType() const;
    /** Returns the APV pipeline address provided by the APV emulator, as stored in the tracker-specific header. */
    u8 getApvEmulatorAddress() const;
    /** Returns the APV address error, as stored in the tracker-specific header.  The APV address on each FE-FPGA
	is compared with the APVE address.  Each bit of this 8-bit word corresponds to one of the FE-FPGAs.  A 1 indicates
	agreement for a given FE-FPGA, while a zero indicates a disagreement. */
    u8 getApvAddressError() const;
    /** Returns the 16-bit FED status register, as stored in the tracker-specific header.*/
    u16 getStatusRegister() const;
    /** Returns the 8-bit FE overflow register, as stored in the tracker-specific header.  Each bit corresponds to
	a FE-FPGA.  If the bit is set to 1 it indicates than an overflow condition has occurred in the corresponding
	FE-FPGA and that the data from than FPGA should be ignored.*/
    u8 getFeOverflowRegister() const;
    /** Returns the 8-bit FE enable register, as stored in the tracker-specific header.  Each bit corresponds to a
	FE-FPGA.  If the bit is set to 1, it indicates that the FE-FPGA can receive data.  If the bit is set to zero,
	there will be no payload data from that FE-FPGA.*/
    u8 getFeEnableRegister() const;
    /** Returns a boolean indicating whether a given FE-unit is enabled or not.  If the data is old-style (eg. test-beam)
	the return value will always be true.*/
    bool getFeUnitEnable(u32 unit) const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(unit > 0 && unit <=8)(unit).msg("getFeUnitEnable:  wrong FE-FPGA number given as input").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
#endif 
#endif
      if (_mode != FED9U_EVENT_MODE_LEGACY) {return static_cast<bool>((*(_trackerHeader + 7) >> (8 - unit)) & 0x1);}
      return true;
    }
    
    // methods for extracting information specific to the FULL DEBUG version of the tracker header

    /** Returns the 32-bit word stored in the middle of the second 64-bit word associated to a given FE-FPGA in the
	FULL DEBUG version of the tracker-specific header.  For most FE-FPGAs, this word is reserved, but currently
	empty.  The exceptions are:
	unit = 8   gives a copy of the BE status register.
	unit = 2   gives a copy of the second DAQ register.
	unit = 1   gives a copy of the first DAQ register.*/
    u32 getReservedWordFromFullDebugHeader(u32 unit) const;
    
    // methods for extracting information from the DAQ trailer
    
    //<JEC date=11/4/07> handle 32-bit swapping of DAQ header and trailer
    /** Returns the total event fragment length in number of 64-bit words, including the header and the trailer,
	as stored in the DAQ trailer.*/
    //    u32 getTotalDataLength() const {return ((*_trailer) | (*(_trailer + 1) << 8) | (*(_trailer + 2) << 16));}
    u32 getTotalDataLength() const {
      ICUTILS_VERIFY(_trailer != NULL)(_trailer).msg("getTotalDataLength:  trailer pointer is NULL.  Possible data corruption ?").error()
	.code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return ((*_trailer) | (*(_trailer + 1) << 8) | (*(_trailer + 2) << 16));
      } else {
	return (*(_trailer + 4) | (*(_trailer + 5) << 8) | (*(_trailer + 6) << 16));
      }
    }
    /** Returns the CRC value, as stored in the DAQ trailer.*/
    //    u16 getCrc() const {return (*(_trailer + 6) | (*(_trailer + 7) << 8));}
    u16 getCrc() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 6) | (*(_trailer + 7) << 8));
      } else {
	return (*(_trailer + 2) | (*(_trailer + 3) << 8));
      }
    }
    /** Returns the event status, as stored in the DAQ trailer.
	This is a "summary" status of the event, but also incorporates the FED working condition during the event.*/
    //    u8 getEventStatus() const {return (*(_trailer + 5) & 0xF);}
    u8 getEventStatus() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 5) & 0xF);
      } else {
	return (*(_trailer + 1) & 0xF);
      }
    }
    /** Returns the TTS bits, as stored in the DAQ trailer.*/
    //    u8 getcurrentTTSbits() const {return (*(_trailer + 4) >> 4);}
    u8 getcurrentTTSbits() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 4) >> 4);
      } else {
	return (*_trailer >> 4);
      }
    }
    /** Returns the CRC modification bit from the DAQ trailer.  If this returns 1, then the Slink sender card has modified
        the CRC value stored in the DAQ trailer.*/
    //    u8 getCRCmodificationBit() const {return (*(_trailer + 4) & 0x4);}
    u8 getCRCmodificationBit() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 4) & 0x4);
      } else {
	return (*_trailer & 0x4);
      }
    }
    //</JEC>
    
    // methods for accessing pointers
    /** Methods are also available which return pointers to different parts of the event.
	All pointers are unsigned 8-bit pointers, ie. they step through the data in bytes.*/

    /** Returns a pointer to the start of the DAQ header, ie. the very start of the event.*/
    const u8 * getDaqHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_daqHeader != NULL )(_daqHeader).msg("DAQ header pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return  _daqHeader;
    }
    /** Returns a pointer to the start of the tracker-specific header.*/
    const u8 * getTrackerHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_trackerHeader != NULL )(_trackerHeader).msg("Tracker-specific header pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return  _trackerHeader;
    }
    /** Returns a pointer to the start of the FE-FPGA status information in the tracker-specific header.*/
    const u8 * getFEUnitHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_feUnitHeader != NULL )(_feUnitHeader).msg("FE-FPGA status pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _feUnitHeader;
    }
    /** Returns a pointer to the start of the payload.*/
    const u8 * getPayloadPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_payload != NULL )(_payload).msg("payload pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _payload;
    }
    /** Returns a pointer to the start of the DAQ trailer.*/
    const u8 * getTrailerPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_trailer != NULL )(_trailer).msg("DAQ trailer pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _trailer;
    }
    
    // methods for accessing information about enabled channels/FE Units

    /** Returns the number of enabled FE-FPGAs on the FED.*/
    u32 enabledFeUnits() const;
    
    // general event-level methods

    /** Creates the output required for an Igor File, as used in FedDebugSuite.*/
    void saveIgorFile(ostream & o) const;

    /** Returns a pointer to the specified FeUnit.*/
    //<JEC date=20/03/07> moved here to get automatic inline
    Fed9UEventUnitStreamLine & getFeUnit(u32 feUnit) {
      ICUTILS_VERIFY(feUnit > 0 && feUnit <=8)(feUnit).msg("getFeUnit:  wrong FE-FPGA number given as input").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
      ICUTILS_VERIFY(getFeUnitEnable(feUnit))(feUnit)(getFeUnitEnable(feUnit)).msg("getFeUnit:  Requested FE-FPGA is disabled !").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
      return _feUnits[8 - feUnit];
    }
    //</JEC>

    //<JEC date=18/4/07> extra method for converting a FED channel into an Fe-unit & channel number
    Fed9UEventUnitStreamLine & getFeUnit(u32 fedChannel, u32 & feUnitChannel) {
      ICUTILS_VERIFY(fedChannel > 0 && fedChannel <= 96)(fedChannel).msg("getFeUnit:  FED channel number out of bounds !").error()
	.code(Fed9UEventStreamLineException::ERROR_FED_CHANNEL_OUT_OF_BOUNDS);
      u32 feUnit = ((fedChannel - 1)/12) + 1;
      ICUTILS_VERIFY(getFeUnitEnable(feUnit))(feUnit)(getFeUnitEnable(feUnit)).msg("getFeUnit:  Requested FE-FPGA is disabled !").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
      feUnitChannel = fedChannel - ((feUnit - 1)* 12);
      return _feUnits[8 - feUnit];
    }
    //</JEC>

    /** Creates the output required to create a binary dump of the buffer.
	Used by FedDebugSuite and can also be used for fake event input to this class.*/
    void dumpRawBuffer(ostream& os) const;
    /** Provides a prettified text dump of the buffer for debugging purposes.*/
    //Added second and third parameter for FedPatternCheck (AAB 8/24/2015)
    void dumpBuffer(ostream& os, int modFactor = 1, bool lessOutput = false) const;
    /** This method checks the status of every channel on every enabled FE-FPGA on the FED and reports any error found.*/
    void checkChannelStatuses() const;

    /** This method performs some basic checks of the event.  It checks that the length of the buffer matches that that was
     originally created for fake events, it also checks that the CRC value stored in the trailer is as expected.  It also
    checks that the event type is correct.*/
    // TODO event length code only valid for fake data - needs updating for real data.
    void checkEvent() const;

    /** Allows the Event mode to be set externally.  Needed when using Fake Events.*/
    void setEventDataMode(Fed9UEventDataMode mode) {_mode = mode;}
    Fed9UEventDataMode getDataMode() {return _mode;}

    Fed9UEventFormatType getEventFormat() {return _eventFormat;}

    /** Calculates the CRC value for the event, for comparison to that stored in the DAQ trailer.
        Calculation is based on tables.*/
    u16 calcCrc() const;
    
  protected:
    /** Original pointer to the event buffer.*/
    u32 * _buffer;
    
    /** finer granularity pointers for the DAQ header, the tracker-specific header and the DAQ trailer.*/
    u8 * _daqHeader;
    u8 * _trackerHeader;
    u8 * _trailer;
    /** finer granularity pointers for accessing the FE-unit information.*/
    u8 * _feUnitHeader;
    u8 * _payload;
    
    /** _mode is an internal variable which flags if the data is in the standard data format, or in 
	an older data format (used mainly for test-beam) in which the tracker-specific header was one 64-bit
	word shorter than currently.*/
    Fed9UEventDataMode _mode; 
    
    //<JEC date=11/4/07> new internal variable to handle 32-bit swapping in DAQ header and trailer
    Fed9UEventFormatType _eventFormat;
    //</JEC>

    /** An internal variable which records whether the header is in FULL DEBUG mode or APV ERROR mode.*/
    Fed9UHeaderFormat _headerFormat;
    /** FED description.  Required for checks of the channel status.*/
    const Fed9UDescription * _fedDescription;
    
  private:
    /** Pointer to a FakeBufferCreator object - used only for fake data.*/
    Fed9UFakeBufferCreator * fakeBuffer;
    
    /** Vector of pointer to the associated FE-unit objects.  Pointer will be set to NULL for a given
	FE-FPGA if it has been disabled.*/
    //<JEC date=21/03/07>  changed from vector of pointers, to vector of objects.  Should be faster !
    vector<Fed9UEventUnitStreamLine> _feUnits;
    //</JEC>
    
    /** _bufferLength is total event length (minus the trailer) in bytes.
	_rubbish allows for the DAQ or tracker-specific trailer to be of non-zero length.*/
    u32 _rubbish, _bufferLength;
    //<JEC date=06/02/2007> added to deal with differences between buffer from real events and fake events
    bool _deleteBuffer;                       //!< Used to flag if the buffer is to be deleted.
    //</JEC>
    
  };
  
  inline std::ostream& operator<<(std::ostream& os, const Fed9UEventStreamLine & ev) { ev.dumpBuffer(os); return os; }
  
}

#endif // H_Fed9UEventStreamLine
#ifndef H_Fed9UBufferedEvent
#define H_Fed9UBufferedEvent

#ifdef EVENT_STREAMLINE
#else
#endif
#include <iosfwd>
#include <vector>

namespace Fed9U {
  //using.*std::ostream;
  
  /**
   * \brief Designed to extend Fed9UEvent by adding an internal event buffer.
   * \class Fed9UBufferedEvent
   *
   * \author M. Noy
   * \version 0.0
   * \date 04-11-2003
   *
   * \todo - Add support for super DAQ mode.
   */
#ifdef EVENT_STREAMLINE
  class Fed9UBufferedEvent: public Fed9UEventStreamLine
#else
  class Fed9UBufferedEvent:public Fed9UEvent
#endif
  {
  public:

    /**
     * \name Public constructors.
     *
     * Class constructors available to Users.
     */

    //@{
    /**
     * \brief Default constructor.
     *
     * Sets _isOnline to true and _eventBufferLength to zero.
     */
    Fed9UBufferedEvent();

    /**
     * \brief Constructs a Fed9UBufferedEvent from an unsigned int array.
     * \param buffer Pointer to an unsigned long array containing the event buffer.
     * \param currentDescription Current FED settings that used to record the event buffer.
     * \param bufsize Size of the event buffer. Defaulted to 0.
     *
     * A buffered event is constructed from the data pointed to by arugment buffer. It does this by calling loadEvent.
     * _isOnline is set to true and _eventBufferLength to bufsize.
     */
    Fed9UBufferedEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize = 0);

    /**
     * \brief Constructs a Fed9UBufferedEvent from file.
     * \param input Reference to the input stream that contains the event data.
     * \param FedDescriptionAtTheTime FED settings that were used to record the event.
     *
     * Data is constructed from file, which should contain the buffer size and DAQ mode as the first two 32 bits words, then the data for buffer size worth
     * of 32 bits words containing the event data.
     *
     * \todo - check nothing funny with the file format as the described format is based on the code.
     */
    Fed9UBufferedEvent(std::istream & input, Fed9UDescription * FedDescriptionAtTheTime);    
    //@}

    /**
     * \brief Destructor.
     */
    ~Fed9UBufferedEvent();    
     
    
//      u32 getEventNumber() const;
//      u8  getEventType() const;
//      u16 getBunchCrossing() const;
//      u16 getSourceId() const;
//      u8  getFormatVersion() const;
//      u8  getMajorityPipeline(int feUnit) const;
//      u32 getTotalLength() const;
//      u16 getCrc() const;
//      u16 calcCrc() const;
    //void saveIgorFile(std::ostream & o) const;
    //void dump(std::ostream&) const;
  
    /**
     * \brief  Writes the event to file.
     * \param  output Output stream to write the data to.
     * \return unsigned long Returns 0 if the member function completed.
     *
     * The file is written with the event size as the first 32 bit word, DAQ mode as the second and then the subsequent event size worth of words
     * contain the event buffer.
     */
    u32 writeBufferedEventToFile(std::ostream & output);

    /**
     * \brief  Gives a pointer to the first element in the event buffer.
     * \return unsigned long* Pointer to the first element.
     */
    u32 * getEventBufferPointer();

    /**
     * \brief  Private member function to read the events from file.
     * \param  in Input stream containing the event file. First two 32 bit words should be the event length and DAQ mode respectively. Remaining data
     *         is the event buffer.
     * \param  desc Pointer to the description that contains the FED settings when the event was recorded.
     * \return unsigned long Upon completion of the member function a zero is returned.
     *
     * Once the file has been read the internal buffer is initialised with the file data.
     */
    inline u32 getBufferedEventFromFile(std::istream & in, Fed9UDescription * desc){return getEventFromFile(in, desc);}
    
    /**
     * \brief Loads an event buffer into the internal event buffer and initialises the appropriate class member variables.
     * \param buffer Pointer to the beginning of the unsigned long array that contains the event words.
     * \param currentDescription A Fed9UDescription object that contains the FED settings when the event was recorded.
     * \param bufsize Size of the event buffer pointed to by buffer.
     *
     * Should be used to create a new event in the existing Fed9UBufferedEvent object or to add one to a default constructed object.
     */
    void loadEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);

    /**
     * \brief  Returns the data acquisition mode used to record the event.
     * \return Fed9UDaqMode An enumeration that gives the DAQ mode used to record the event. Further details of the allowed values can be
     *         found in the Fed9UDaqMode documentation.
     */
    Fed9UDaqMode getBufferedDaqMode() const {return _daqMode;}

  protected:
    
  private://member functions

    /**
     * \name Private constructors.
     *
     * Constructors used for either internal purposes or to prevent copying of Fed9UBufferedEvent objects.
     */
    //@{

    /**
     * \brief Copy constructor. Unimplemented.
     */
    Fed9UBufferedEvent(const Fed9UBufferedEvent &);

    /**
     * \brief Assignment operator. Unimplemented.
     */
    Fed9UBufferedEvent & operator = (const Fed9UBufferedEvent &);
    //@}

    /**
     * \brief  Private member function to read the events from file.
     * \param  in Input stream containing the event file. First two 32 bit words should be the event length and DAQ mode respectively. Remaining data
     *         is the event buffer.
     * \param  desc Pointer to the description that contains the FED settings when the event was recorded.
     * \return unsigned long Upon completion of the member function a zero is returned.
     *
     * Once the file has been read the internal buffer is initialised with the file data. Has been made private due to an accident with name convension.
     * getBufferedEventFromFile is the public member function that performs this task.
     */
    u32 getEventFromFile(std::istream & in, Fed9UDescription * desc);

    /**
     * \brief Copies an external buffer into the class internal buffer.
     * \param buf Pointer to an unsigned long array containing the event.
     * \param bufLen Size of the buffer begin copied.
     */
    void chargeEventBuffer(u32 * buf, u32 bufLen);   

  private://data members

    std::vector<u32> _eventBuffer; //!< Internal buffer used to store the event.

    u32 _eventBufferLength;        //!< Length of the event buffer.

    bool _isOnline;                //!< True if the buffer is "online" i.e. taken from a buffer that has not been saved for later analysis.
                                   //!< False is the buffer is "offline" i.e. loaded from a file or some other storage mechanism.

    Fed9UDaqMode _daqMode;         //!< Data acquision mode that the event was recorded in.
    

  };
}

#endif //H_Fed9UBufferedEvent
#ifndef H_Fed9UCounters
#define H_Fed9UCounters

#include <iostream>

namespace Fed9U {

  /**
   * \brief A struct to hold the value of all the counters on the FED.
   *
   * The following counters are stored in the struct, in unsigned long format:
   * \li triggerNumber Number of triggers received since last reset.
   * \li qdrFrameCount Number of frames stored in the BE FPGA QDR buffers.
   * \li qdrBufferLevel Fill level of the QDR buffer, number of 64-bit words remaining in the buffer.
   * \li qdrTotalFrameCount Total number of frames received since last reset.
   * \li feLevel Fill level of the FE unit event buffers, number of 64-bit words remaining in the buffer.
   *
   * Two constructors are provided, plus the assigment operator.
   */
  struct Fed9UCounters {

    /**
     * \name Counter members.
     *
     * Values stored in each counter data member are defined in the class documentation.
     */
    //@{
    u32 triggerNumber, qdrFrameCount, qdrBufferLevel, qdrTotalFrameCount, feLevel, bunchCount;
    //@}

    /**
     * \brief Allows each individual counter to be set.
     * \param x0 Trigger number.
     * \param x1 QDR frame count.
     * \param x2 QDR buffer level.
     * \param x3 QDR total frame count.
     * \param x4 FE level.
     */
    Fed9UCounters(u32 x0 = 0, u32 x1 = 0, u32 x2 = 0, u32 x3 = 0, u32 x4 = 0, u32 x5 = 0) :
      triggerNumber(x0), qdrFrameCount(x1), qdrBufferLevel(x2),
      qdrTotalFrameCount(x3), feLevel(x4), bunchCount(x5)
    { }

    /**
     * \brief Takes a vector for BE FPGA counters and single parameter for the FE level.
     * \param x A vector of unsigned ints containing the BE FPGA counters. Contents of each element is as follows:
     *        \li 0: Trigger number.
     *        \li 1: QDR frame count.
     *        \li 2: QDR buffer level.
     *        \li 3: QDR total frame count.
     * \param x4 FE level.
     */
    Fed9UCounters(const std::vector<u32> & x, u32 x4 = 0) :
      triggerNumber(x[0]), qdrFrameCount(x[1]), qdrBufferLevel(x[2]),
      qdrTotalFrameCount(x[3]), feLevel(x4), bunchCount(x[4])
    { }

    //Fed9UCounters & setFeLevel(int i, u32 val) { feLevel[i] = val; }

    /**
     * \brief  Comparison operator. Compares a Fed9UCounter struct to this counter struct comparing the values of the data member.
     * \param  rhs Right side Fed9UCounter object to compare to this object.
     * \return bool True if the two objects have identical values.
     */
    bool operator==(const Fed9UCounters & rhs) const {
      return triggerNumber==rhs.triggerNumber && qdrFrameCount==rhs.qdrFrameCount
	&& qdrBufferLevel==rhs.qdrBufferLevel && qdrTotalFrameCount==rhs.qdrTotalFrameCount
	&& feLevel==rhs.feLevel && bunchCount==rhs.bunchCount;
    }
  };
  
  /**
   * \brief  Output stream operator for Fed9UCounter struct.
   * \param  os Reference to the output stream to send counter values to.
   * \param  fc Fed9UCounters reference to send to the stream.
   * \return ostream Reference to the ostream used to write to.
   *
   * Parameters are sent in the order trigger number, QDR total frame count, QDR frame count and QDR buffer level.
   * FE level are not written to the stream. Explanatory text is also written. 
   */
  inline std::ostream & operator<<(std::ostream & os, const Fed9UCounters & fc) {
    return os << "Triggers=" << fc.triggerNumber
	      << ", QDR: Total events processed=" << fc.qdrTotalFrameCount
	      << " events in memory=" << fc.qdrFrameCount
	      << " data (64 bit words)=" << fc.qdrBufferLevel
	      << ", Bunch Count=" << fc.bunchCount 
      ; //<< ", FE Level=" << fc.feLevel;
  }

}

#endif // H_Fed9UCounters
#ifndef H_Fed9ULockFile
#define H_Fed9ULockFile

#include <string>
#include <sstream>

#include "ICExImpl.hh"

namespace Fed9U {

  /**
   * \brief Provides a base class to open a file and lock it, such that there can only be one instance of that file open.
   *
   * The class is intended for use as a base class and as such its constructor and member functions are declared protected and only accessible by
   * derived classes. Derived classes have the ability to read/write from the file. One public member function is provided to determine if the file is new or not.
   */
  class Fed9ULockFileBase {

  public:

    /**
     * \brief  Returns true if the opened file is new.
     * \return bool
     */
    bool isNewFile() const { return _newfile; }

  protected:

    /**
     * \brief Constructor.
     *
     * \param filename Constant reference to a string that contains the file to be opened.
     *
     * Opens the file specified by filename for read/write access, determining if it is a new file or not.
     */
    Fed9ULockFileBase(const std::string & filename);

    /**
     * \brief Destructor.
     *
     * Closes any open files.
     */
    virtual ~Fed9ULockFileBase();

    /**
     * \brief  Reads the contents of the file to a string.
     * \return string
     */
    std::string readFileString() const;

    /**
     * \brief Writes the contents of a string to the file.
     * \param string Constant reference to the string that is to be written to the file.
     *
     * This will over write any existing data in the file.
     */
    void writeFileString(const std::string &) const;

    int _fd;       //!< Stores the ID of the file that has been opened.

    bool _newfile; //!< Flag to determine if the it is a new file or not.

  };//class Fed9ULockFile


  /**
   * \brief Derived from Fed9ULockFileBase, this templated class allows a file to be synchronised with any type T that has overloaded input/output stream
   *        operators.
   *
   * The type T is loaded from file upon construction of the class, if there is any data from the file to read. Otherwise the contents of type T can be used
   * to update the file. Locks the opened file so only one point of access is allowed to the file, this class.
   */
  template <typename T>
  class Fed9ULockFile : public Fed9ULockFileBase {
  public:

    /**
     * \brief Constructor.
     * \param filename Name of the file to be read/written from.
     * \param def Constant reference to the type that is to be synchronised with the file.
     *
     * Makes a local copy of the object T and opens the file specified. If the file is not new then the contents of the file will be streamed to the type T,
     * otherwise the file is just opened and T left unaffected.
     */
    Fed9ULockFile(const std::string & filename, const T & def = 0)
      : Fed9ULockFileBase(filename), _val(def) {
      if (!_newfile) {
	std::istringstream stream(readFileString());
	if (stream.str().length() > 0) {
	  stream >> _val;
	  if (!stream) THROW(ICUtils::ICException("Error reading from file"));
	} else {
	  THROW(ICUtils::ICException("File is empty"));
	}
      }
    }

    /**
     * \brief Destructor.
     *
     * Synchronises the contents of the type T with the file, overwritting any data that maybe in the file.
     */
    ~Fed9ULockFile() {
      sync();
    }

    /**
     * \brief  Returns a writable reference to the object T that contains the file contents.
     * \return T&
     */
    T & value() { return _val; }

    /**
     * \brief  Returns a read only reference to the object T that is synchronised with the file.
     * \return const T&
     */
    const T & value() const { return _val; }

    /**
     * \brief  Overload of the operator(), which returns a reference to the type T that is synchronised with the file.
     */
    operator T & () { return _val; }

    //operator const T & () const { return value(); }

    /**
     * \brief  Synchronises the type T with the file, by writing the contents of T to the file.
     */
    Fed9ULockFile & sync() {
      std::ostringstream buf;
      buf << _val << "\n";
      writeFileString(buf.str());
      return *this;
    }
  private:
    T _val;
  };

}

#endif // H_Fed9ULockFile
#ifndef H_Fed9ULog
#define H_Fed9ULog

#include <fstream>
#include <string>

namespace Fed9U {

  //using.*std::string;

  /** 
   * \brief A class to log information to a file during a run of the program.
   * \author Emlyn Corrin
   * \version 0
   * \date 30 July 2003
   * 
   * Every time the program is run, instance of this class is automatically created,
   * called Fed9U::log, referring to the file Fed9U.log in the folder in which the
   * program was started. \n
   * On construction, the class looks for an existing log file of the specified name,
   * and if it exists, creates a backup by appending a tilde (~) to the file name,
   * before creating the new log file. The date and time are logged to the file
   * at startup, and at program termination if the program ends normally (e.g. not
   * via a core dump etc). \n
   * The class may be used like a standard ostream to send text, and values of
   * variables to the log file. \n
   * In addition, the log class provides a method stamp(), which inserts a timestamp
   * into the logfile, specifying the number of seconds, to the nearest millisecond
   * since the program started running. Note: this depends on the non-standard gettimeofday()
   * function available in linux, on other platforms this may need to be rewritten. \n
   * The output is unbuffered, so in the event of a crash all output already written
   * should be in the log file. \n
   * Example use:
\verbatim

void somefunction() {
  log.stamp() << "Start of somefunction()" << endl;
  int x = something();
  log << "The value of x is " << x << endl;
}
\endverbatim
   * \n
   * There is a global instantiation of this class called log. It is declared at the end of this file
   * and the definition is in the file Fed9ULogTemplate.cc. Access to this log file is recommended to
   * be done through either a Fed9UMessage<Fed9ULog> or gFed9ULog object.
   */
  class Fed9ULog : public std::ofstream {

  public:

    /**
     * \brief Constructor.
     * \param filename Name of the log file.
     *
     * Opens the log file with the name that can be used for logging the output during the running of a program.
     */
    Fed9ULog(const char* filename);

    /**
     * \brief Destructor.
     *
     * Closes the log file, appending the date the file was closed.
     */
    ~Fed9ULog();

    /**
     * \brief  Writes the time to the file.
     * \return Self reference.
     */
    Fed9ULog& stamp();

    /**
     * \brief Called when an expected error causes the program to terminate.
     *
     * This write a mesage to the log and standard error out to indicate the program terminated unexpectedly.
     */
    static void new_unexpected();

    /**
     * \brief  Returns a string containing the time in seconds.
     * \return string
     */
    static std::string getTimeString();

    /**
     * \brief  Returns the date and time in the form day/month/year hour/minute/second.
     */
    static std::string getDateString();
  };

  //Can be removed as we now no longer require a file to be instantiated as it can be created on the fly.
//   /**
//    * \brief This is a global log file that is linked to the gFed9ULog object, which allows message levels to be set for the stream
//    *        and also will provide atomicity for its writes.
//    *
//    * Must be here in order to ensure that it is constructed before the gFed9ULog object.
//    */
//    extern Fed9ULog log;

}

#endif // H_Fed9ULog
#ifndef _Fed9UCreateDescriptionException_H_
#define _Fed9UCreateDescriptionException_H_
#include "ICExDecl.hh"

namespace Fed9U {

  /**
   * \brief  Defines the error list for use with the class Fed9UCreateDescription.
   * \author Gareth Rogers, RAL
   * \date   05/2004
   *
   * Possible errors defined in the error list are:
   * \li ERROR_CREATE_DESCRIPTION Error occured while creating the description object.
   */
#define FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST \
  IC_DEF_ERROR(ERROR_CREATE_DESCRIPTION, "An error has been thrown from Fed9UCreateDescription.") \

  IC_EXCEPTION_CLASS_BEGIN(Fed9UCreateDescriptionException, FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END
}

#endif
#ifndef _Fed9UCreateDescription_H
#define _Fed9UCreateDescription_H


namespace Fed9U {

  //using.*std::string;

  /**
   * \brief  This class can be used to create a complete description object, with settings other than
   *         the default description settings. It has been designed to be used with the soak test
   *         which requires a range of description settings.
   * \author Gareth Rogers, RAL
   * \date   05/2004
   *
   * The contents of the descripition files created are not controlled by the caller of the class.
   * The description will be created with predefined settings. A single instace of Fed9UCreateDescription
   * will keep track of the number of times it has been called and will create a description with
   * different settings each time. The maximum value of unique descriptions that can be generated is 4294967295.
   */
  class Fed9UCreateDescription {

  public:

    /**
     * \brief Fed9UCreateDescription constructor.
     * \param fed9UDescriptionNumber Each description corresponding to this number is different, by default it is zero
     *        but giving a different value will cause an offset into the created descriptions.
     *
     * A Fed9UCreateDescription will return a different description depending on how many times it
     * has been called.
     */
    Fed9UCreateDescription(u32 fed9UDescriptionNumber = 0);

    /**
     * \brief  Makes a FedUDescription object with predefined settings depending on the description number
     * \return Fed9UDescription A copy of the description object that has been created.
     *
     * The crate number, base address of the FED and the location of the HAL address table must be specified.
     */
    Fed9UDescription makeFed9UDescription(u16 crateNumber, u32 baseAddress, std::string halAddressTable);

  private:

    /**
     * \brief Loads all the FE settings into the description.
     */
    Fed9UCreateDescription& loadFeFpgaSettings();

    /**
     * \brief Loads all the BE settings into the description.
     */
    Fed9UCreateDescription& loadBeFpgaSettings();

    /**
     * \brief Loads all the VME settings into the description.
     */
    Fed9UCreateDescription& loadVmeFpgaSettings();

    /**
     * \brief Loads all the TTCrx settings into the description.
     */
    Fed9UCreateDescription& loadTtcrxDeviceSettings();

    /**
     * \brief Loads all the Voltage Monitor settings into the description.
     */
    Fed9UCreateDescription& loadVoltageMonitorSettings();

    /**
     * \brief Loads the serial EPROM into the description.
     */
    Fed9UCreateDescription& loadEpromSettings();

    /**
     * \brief Loads all the higher level settings into the description.
     */
    Fed9UCreateDescription& loadHigherLevelSettings();

    /**
     * \brief This holds the iteration number. It is used to vary the settings in the returned FED description.
     */
    u32 fed9UDescriptionNumber_;

    /**
     * \brief Holds the settings that are to be returned.
     */
    Fed9UDescription fedDescription;

  };// End of Fed9UCreateDescription class

}// End of Fed9U namespace

#endif// _H_Fed9UCreateDescription
#ifndef FED9UEVENTINFO_H
#define FED9UEVENTINFO_H

namespace Fed9U {
  
  /**
   * \brief  Class which provides access to some useful FED specific numbers for the SiStripDataFormat classes.
   * \author Matthew Pearson, RAL
   * \date   07/2004
   */
  class Fed9UEventInfo {

  public:
    /**
     * \brief Virtual destructor.
     */
    virtual ~Fed9UEventInfo() {}

    /**
     * \brief  Method to get the number of FED front end units.
     * \return unsigned short.
     */
    unsigned short getFeUnitsPerFed(void) const {
      return FEUNITS_PER_FED;
    }

    /**
     * \brief  Method to get the number of channels per FED front end unit.
     * \return unsigned short.
     */
    unsigned short getChannelsPerFeUnit(void) const {
      return CHANNELS_PER_FEUNIT;
    }

    /**
     * \brief  Method to get the number of APVs per FED channel.
     * \return unsigned short.
     */
    unsigned short getApvsPerFedChannel(void) const {
      return APVS_PER_CHANNEL;
    }

    /**
     * \brief  Method to get the number of Si strips per APV in the Si tracker.
     * \return unsigned short.
     */
    unsigned short getStripsPerApv(void) const {
      return STRIPS_PER_APV;
    }

    /**
     * \brief  Method to get the size in bytes of the FED front end header in the FED data buffer.
     * \return unsigned short.
     */
    unsigned short getFeUnitHeaderSize(void) const {
      return TRACKER_FEUNIT_HEADER_SIZE;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U buffer.
     * \return unsigned long.
     */
    unsigned long getBytesInRawFed9UBuffer(void) const {
      return BYTES_IN_RAW_FED9U_BUFFER;
    }

    /**
     * \brief  Method to get the number of bytes to the start of the FE headers.
     * \return unsigned short.
     */
    unsigned short getBytesToStartOfFeHeaders(void) {
      return BYTES_TO_START_OF_FE_HEADERS;
    }

    /**
     * \brief  Method to get the number of bytes to the start of channel data.
     * \return unsigned short.
     */
    unsigned short getBytesToStartOfChannels(void) {
      return BYTES_TO_START_OF_CHANNELS;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U FE data block (including channel headers and packet codes).
     * \return unsigned short.
     */
    unsigned short getBytesInRawFed9UFE(void) {
      return BYTES_IN_RAW_FED9U_FE;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U channel (incluidng header and packet code).
     * \return unsigned short.
     */
    unsigned short getBytesInRawFed9UChannel(void) {
      return BYTES_IN_RAW_FED9U_CHANNEL;
    }

    /**
     * \brief  Method to get the number of bytes in the DAQ trailer.
     * \return unsigned short.
     */
    unsigned short getBytesInDaqTrailer(void) {
      return BYTES_IN_DAQ_TRAILER;
    }

    /**
     * \brief  Method to get the code for FED scope mode.
     * \return unsigned short.
     */
    unsigned short getScopeModeCode(void) const {
      return FED9U_SCOPE;
    }

    /**
     * \brief  Method to get the code for FED frame finding mode.
     * \return unsigned short.
     */
    unsigned short getFrameFindingModeCode(void) const {
      return FED9U_FRAME;
    }

    /**
     * \brief  Method to get the code for FED virgin raw mode.
     * \return unsigned short.
     */
    unsigned short getVirginRawModeCode(void) const {
      return FED9U_VIRGRAW;
    }

    /**
     * \brief  Method to get the code for FED zero suppressed mode.
     * \return unsigned short.
     */
    unsigned short getZeroSuppressedModeCode(void) const {
      return FED9U_ZEROSUPP;
    }

    /**
     * \brief  Method to get the code for FED processed raw mode.
     * \return unsigned short.
     */
    unsigned short getProcessedRawModeCode(void) const {
      return FED9U_PROCRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for scope mode.
     * \return unsigned short.
     */
    unsigned short getScopeDaqModeCode(void) const {
      return FED9U_DAQ_SCOPE;
    }

    /**
     * \brief  Method to get the DAQ mode number for virgin raw mode.
     * \return unsigned short.
     */
    unsigned short getVirginRawDaqModeCode(void) const {
      return FED9U_DAQ_VIRGRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for processed raw mode.
     * \return unsigned short.
     */
    unsigned short getProcessedRawDaqModeCode(void) const {
      return FED9U_DAQ_PROCRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for zero suppressed mode.
     * \return unsigned short.
     */
    unsigned short getZeroSuppressedDaqModeCode(void) const {
      return FED9U_DAQ_ZEROSUPP;
    }

    /**
     * \brief  Method to get the channel status bits size.
     * \return unsigned short.
     */
    unsigned short getChannelStatusSize(void) const {
      return CHANNEL_STATUS_SIZE;
    }

    /**
     * \brief  Methgod to get the size of the frame sync out packet.
     * \return unsigned short.
     */
    unsigned short getSizeOfFrameSyncOutPacket(void) {
      return SIZE_OF_FRAME_SYNC_OUT_PACKET;
    }

    /**
     * \brief  Method to get the tracker special header full debug mode code.
     * \return unsigned short.
     */
    unsigned short getTrackerSpecialHeaderFullDebugMode(void) {
      return TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE;
    }

    /**
     * \brief  Method to get the tracker special header apv error mode code.
     * \return unsigned short.
     */
    unsigned short getTrackerSpecialHeaderApvErrorMode(void) {
      return TRACKER_SPECIAL_HEADER_APV_ERROR_MODE;
    }

  private:
    static const unsigned short FEUNITS_PER_FED = 8;      //!< Number of FE units on a FED.
    static const unsigned short CHANNELS_PER_FEUNIT = 12; //!< Number of channels on a FE unit.
    static const unsigned short APVS_PER_CHANNEL = 2;     //!< Number of APVs per channel.
    static const unsigned short STRIPS_PER_APV = 128;     //!< Number of strips per APV.

    static const unsigned short TRACKER_FEUNIT_HEADER_SIZE = 16;     //!< Number of bytes in the FE unit header.
    static const unsigned long BYTES_IN_RAW_FED9U_BUFFER = 49624;    //!< Number of bytes in a virgin/processed raw event buffer (including inter-FE padding).
    static const unsigned short BYTES_TO_START_OF_FE_HEADERS = 16;   //!< Number of bytes before the start of the FE headers.
    static const unsigned short BYTES_TO_START_OF_CHANNELS = 144;    //!< Number of bytes before the start of the channel data.
    static const unsigned short BYTES_IN_RAW_FED9U_FE = 0x1824;      //!< Number of bytes in the FE units in virgin/processed raw modes.
    static const unsigned short BYTES_IN_RAW_FED9U_CHANNEL = 0x0203; //!< Number of bytes in the channels in virgin/processed raw modes.
    static const unsigned short BYTES_IN_DAQ_TRAILER = 8;            //!< Number of bytes in the DAQ trailer.

    /**
     * \name Packet codes.
     *
     * These are the packet codes (without the 3 MSB).
     */
    //@{
    static const unsigned short FED9U_SCOPE = 1;
    static const unsigned short FED9U_FRAME = 2;
    static const unsigned short FED9U_VIRGRAW = 4;
    static const unsigned short FED9U_ZEROSUPP = 8;
    static const unsigned short FED9U_PROCRAW = 16;
    //@}

    /**
     * \name DAQ event types.
     *
     * These are the DAQ event types.
     */
    //@{
    static const unsigned short FED9U_DAQ_SCOPE = 1;
    static const unsigned short FED9U_DAQ_VIRGRAW = 2;
    static const unsigned short FED9U_DAQ_PROCRAW = 6;
    static const unsigned short FED9U_DAQ_ZEROSUPP = 10;
    //@}

    /**
     * \name Frame sync info.
     *
     * Information about the frame syncronisation out packet.
     */
    //@{
    static const unsigned short CHANNEL_STATUS_SIZE = 6;
    static const unsigned short SIZE_OF_FRAME_SYNC_OUT_PACKET = 81;
    //@}

    /**
     * \name Header formats.
     *
     * Full debug or APV error.
     */
    //@{
    static const unsigned short TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE = 1;
    static const unsigned short TRACKER_SPECIAL_HEADER_APV_ERROR_MODE = 2;
    //@}

  };
 
} //End of Fed9U Namespace

#endif //FED9UEVENTINFO_H
#ifndef FED9USISTRIPREORDERING_H
#define FED9USISTRIPREORDERING_H

#include<vector>

namespace Fed9U {

  //using.*std::vector;

  /**
   * \brief  Class which can perform reordering on silicon strip data.
   * \author Matthew Pearson, RAL
   * \date   07/2004
   *
   * The data output by the tracker FED in raw data mode is dis-ordered (ie. not in strip order). This class 
   * provides a method to re-order a FED channel worth of data. There is also a method
   * to do the reverse, which dis-orders the data to emulate the ordering done in the APV MUX chip.
   * The re-ordering algorithm implemented here is the same as the re-ordering algorithm in the FED 9U.
   */
  class Fed9USiStripReordering {

  public:

    /**
     * \brief Virtual destructor.
     */
    virtual ~Fed9USiStripReordering() {}
    
    /**
     * \brief Method which contains the re-ordering algorithm for the APV data frame.
     * \param apvOutput Contains an element for each strip, with the strips in the APV-MUX order, but for a single APV (i.e. no interleaving). Size:128.
     * \param fedOutput Contains an element for each strip, with the strips in the FED output (physical) ordering for a single APV. Should be an empty
     *        vector as the contents will be deleted.
     *
     * The apvOutput vector will contain the dis-ordered (MUX ordered) strip data that will be placed into the fedOutput vector in the reordered (physical)
     * strip ordering, which is returned by reference.
     */
    void reOrderingAlgorithm(const std::vector<unsigned short> &apvOutput, std::vector<unsigned short> &fedOutput);

    /**
     * \brief Method which contains the dis-ordering algorithm for of APV strips.
     * \param apvInput Contains an element for each strip, with the strips in the physical strip ordering. Size:128.
     * \param apvOutput Contains an element for each strip, with the strips in the APV-MUX order. It should be an empty vector as it will be filled with
     *        strip data.
     *
     * The apvInput vector will contain the re-ordered (physical/FED output ordering) strip data, which will be dis-ordered (APV-MUX ordering) and placed
     * in the apvOutput vector. This is returned by reference.
     */
    void disOrderingAlgorithm(const std::vector<unsigned short> &apvInput, std::vector<unsigned short> &apvOutput);

  private:

    static const unsigned short NUMBER_OF_APV_STRIPS = 128; //!< Number of strips on an APV.
 
  };
  
} //End of Fed9U namespace.


#endif //FED9USISTRIPREORDERING_H
#ifndef FED9UBUFFERDESCRIPTION_H
#define FED9UBUFFERDESCRIPTION_H


#include "ICExDecl.hh"

#include <vector>

namespace Fed9U {

  using ICUtils::ICException;
  //using.*std::vector;
  
  /**Matthew Pearson, RAL, Feb 2005. */

  /**
   * \brief  Container class, which holds the Fed9U buffer parameters.
   * \author Matthew Pearson, RAL.
   * \date   02/2005.
   *
   * It is to be used by the Fed9UBufferCreator derived classes for creating Fed9U buffers for
   * the various event modes. The description contains the data buffer as well as the various header
   * and trailers, which need to be defined in the Fed9UBufferCreator derived classes.
   */
  class Fed9UBufferDescription {

  public:
    /**
     * \brief Constructor.
     *
     * Performs the initialization of all the internal parameters, including the buffer.
     */
    Fed9UBufferDescription(void);

    /**
     * \brief Destructor.
     */
    virtual ~Fed9UBufferDescription() {}

    /**Methods to manipulate the internal Fed9U buffer.*/

    /**
     * \name Fed9U buffer manipulators.
     *
     * The following member functions are all used to manipulate the Fed9U buffer.
     */
    //@{

    /**
     * \brief  Method to provide access to the internal Fed9U buffer.
     * \return unsigned int* Pointer to the beginning of the buffer.
     */
    inline unsigned int * getBuffer(void) {
      return mBuf;
    }

    /**
     * \brief  Method to provide access to the internal Fed9U buffer. No changes can be made to the buffer using this method.
     * \return unsigned int* Pointer the beginning of the constant buffer.
     */
    inline unsigned int * getBufferConst(void) const {
      return const_cast<unsigned int *>(mBuf);
    }

    /**
     * \brief  Method which returns the current size of the buffer (in units of 32-bit words).
     * \return unsigned long Number of 32-bit words in the buffer.
     */
    inline unsigned long getBufferSize(void) const {
      return mIndex;
    }

    /**
     * \brief Increment buffer index.
     *
     * This increments the index parameter for the buffer (it is also the buffer size after the buffer has been made).
     */
    inline void incrementBufferIndex(void) {
      ++mIndex;
    }

    /**
     * \brief Initialize the buffer index to zero.
     *
     * This should be called before generating a new buffer.
     */
    inline void initBufferIndex(void) {
      mIndex = 0;
    }

    /**
     * \brief Initialize the buffer space, called before generating a new buffer.
     */
    void initBuffer(void);
    //@}

    /**
     * \name Setters
     *
     * Methods to set the buffer parameters.
     */
    //@{

    /**
     * \brief  Method to set the bunch crossing number.
     * \param  bunchCrossing Bunch crossing number to be stored in the buffer.
     * \return Self reference
     */
    Fed9UBufferDescription & setBunchCrossing(unsigned long bunchCrossing);

    /**
     * \brief  Method to set the FED data acquision mode.
     * \param  mode DAQ mode to be set.
     * \return Self reference
     *
     * \todo - check this method to see what it is and possible values. It is probably the "Evt_ty", and if so should do something else or be removed.
     */
    Fed9UBufferDescription & setMode(unsigned long mode);

    /**
     * \brief  Method to set the header format in the tracker special header.
     * \param  headerFormat Valid value are:
     *         \li 1: Full debug mode.
     *         \li 2: APV error mode.
     * \return Self reference
     */
    Fed9UBufferDescription & setHeaderFormat(unsigned short headerFormat);

    //<RB>
    //Added tracker event type header.
    /**
     * \brief  Sets the FED readout mode.
     * \param  value "Trkr Evt_Ty" field of the Tracker-specific header, possible values:
     *         \li 1 Scope mode real.
     *         \li 2 Virgin raw real.
     *         \li 3 Virgin raw fake.
     *         \li 6 Processed raw real.
     *         \li 7 Processed raw fake.
     *         \li 10 Zero suppressed real.
     *         \li 11 Zero suppressed fake.
     *         \li 12 Zero suppressed lite real.
     *         \li 13 Zero suppressed lite fake.
     * \return Self reference
     *
     * More details on the running modes can be found in the FED FE FPGA documentation.
     */
    Fed9UBufferDescription& trackerEventType( unsigned short value );
    //</RB>

    /**
     * \brief  Method to set the buffer data vector.
     * \param  bufferData Each element contains the reading from a strip, there must be one element for each strip on a FED. Size:24,576.
     * \return Self reference
     */
    Fed9UBufferDescription & setFed9UBufferData(const std::vector<unsigned short> &bufferData) throw (ICException);

    /**
     * \brief  Method to set the channel length vector.
     * \param  channelLengths Lengths of the data recorded on each channel. Should contain one element for each channel
     *         on the FED. Size:96.
     * \return Self reference
     */
    Fed9UBufferDescription & setChannelLengths(const std::vector<unsigned short> &channelLengths) throw (ICException);

    /**
     * \brief  Method to set the FE length vector.
     * \param  feLengths Each vector element contains the length of the data on the FE units. There must be one element for each
     *         FE unit on the FED. Size:8.
     * \return Self reference
     */
    Fed9UBufferDescription & setFeLengths(const std::vector<unsigned short> &feLengths) throw (ICException);

    /**
     * \brief  Method to set the packet code.
     * \param  packetCode Packet code to be set.
     * \return Self reference
     *
     * \todo - bit more information on this.
     */
    Fed9UBufferDescription & setPacketCode(unsigned short packetCode);

    /**
     * \brief  Method to set the common mode median values.
     * \param  cmMedians Common mode median value recorded by each APV. There must be one element for each APV on the FED. Size:192.
     * \return Self reference
     */
    Fed9UBufferDescription & setCommonModeMedians(const std::vector<unsigned short> &cmMedians) throw (ICException);

   /**
     * \brief  Method to 32-bit swap data into slink format. Should only be performed AFTER buffer generation.
     * \param  None.
     * \return Self reference
     */
    Fed9UBufferDescription & setSlink64();

    //@}

    /**
     * \name Getters.
     *
     * Methods for returning the various buffer parameters.
     */
    //@{

    /**
     * \brief  Method to get the bunch crossing number.
     * \return unsigned long Bunch crossing number for that event.
     */
    unsigned long getBunchCrossing(void) const;

    /**
     * \brief  Method to get the FED DAQ mode.
     * \return FED DAQ mode.
     *
     * \todo - like setMode it is likely depreciated or should do something else.
     */
    unsigned long getMode(void) const;

    /**
     * \brief  Method to get the header format in the tracker special header.
     * \return unsigned short Header format, possible values:
     *         \li 1: Full debug mode.
     *         \li 2: APV error mode.
     */
    unsigned short getHeaderFormat(void) const;

    //<RB>
    //Added tracker event type method.
    /**
     * \brief  Returns the FED readout mode.
     * \return unsigned short The "Trkr Evt_Ty" field of the Tracker-specific header, possible values:
     *         \li 1 Scope mode real.
     *         \li 2 Virgin raw real.
     *         \li 3 Virgin raw fake.
     *         \li 6 Processed raw real.
     *         \li 7 Processed raw fake.
     *         \li 10 Zero suppressed real.
     *         \li 11 Zero suppressed fake.
     *         \li 12 Zero suppressed lite real.
     *         \li 13 Zero suppressed lite fake.
     *
     * More of the event types can be found in the FED FE unit documentation.
     */
    unsigned short trackerEventType(void) const;
    //</RB>
    
    /**
     * \brief  Method to get the buffer data vector.
     * \return vector<unsigned short> A vector containing one element for each strip on a FED, each element contains the value on that strip. Size:24,576.
     */
    std::vector<unsigned short> getFed9UBufferData(void) const;

    /**
     * \brief  Method to get the channel length vector.
     * \return vector<unsigned short> A vector containing one element for each channel on the FED, with each element contains the length of the channel. Size:12.
     */
    std::vector<unsigned short> getChannelLengths(void) const;

    /**
     * \brief  Method to get the FE length vector.
     * \return vector<unsigned short> Returns a vector containing the size of the data for each FE unit. One element per FE unit on a FED. Size:8.
     */
    std::vector<unsigned short> getFeLengths(void) const;

    /**
     * \brief  Method to get the packet code.
     * \return unsigned short The packet code.
     *
     * \todo - figure out a bit more about this.
     */
    unsigned short getPacketCode(void) const;

    /**
     * \brief  Method to get the common mode medians vector.
     * \return vector<unsigned short> Returns a vector which contains the common mode median value calculated for each APV on a FED. Size:192.
     */
    std::vector<unsigned short> getCommonModeMedians(void) const;

    //@}

  private:

    //static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 12406; //!< Max buffer size for raw mode (in units of ints).
    static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 49002; //!< Max buffer size for scope mode, in units of 32 bit words.

    unsigned int mBuf[INTS_IN_RAW_FED9U_BUFFER]; //!< Array to hold the full event data buffer.

    std::vector<unsigned short> mFed9UBufferData; //!< Vector to hold all the FED strip data in the buffer.
    std::vector<unsigned short> mFeLengths;       //!< Vector to hold the 32 bit word length of each FED FE unit.
    std::vector<unsigned short> mChannelLengths;  //!< Vector to hold the 32 bit word length of each FED channel.

    unsigned int mIndex;          //!< Current position of the internal buffer pointer.
    unsigned long mBunchCrossing; //!< Bunch crossing number of the event data.
    unsigned long mMode;          //!< DAQ mode of the event. /todo - Possibily Evt_ty and possibly depreciate or does something else.

    unsigned short mPacketCode;        //!< Packet code. \todo - don't know what this is.
    unsigned short mHeaderFormat;      //!< Header format, see setHeaderFormat or getHeaderFormat for possible values.
    unsigned short trackerEventType_;  //!< Value of "Trkr Evt_Ty" field of Tracker-specific header. See setTrackerHeader or getTrackerHeader for possible values. 
    std::vector<unsigned short> mCMMedians; //!< Vector to hold each of the calculated common mode median values for the 192 APVs on a FED.
    
    Fed9UEventInfo mFed9UEventInfo; //!< See Fed9UEventInfo for more details on this class.
  };

}

#endif //FED9UBUFFERDESCRIPTION_H
#ifndef FED9UBUFFERTRAILER_H
#define FED9UBUFFERTRAILER_H

#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  This class provides methods to construct the trailer part of the Fed9U buffer.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It should be used to compose classes which inherit from Fed9UBufferCreator.
   */
  class Fed9UBufferTrailer {

  public:

    /**
     * \brief Method which attaches the DAQ trailer to the end of the FED buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    void generateFed9UDaqTrailer(Fed9UBufferDescription *theBufDesc) throw (ICException);

  };

}

#endif //FED9UBUFFERTRAILER_H
#ifndef FED9UBUFFERDATA_H
#define FED9UBUFFERDATA_H


#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  Using a Fed9UBufferDescription, which should contain the ADC settings found for each strip on the FED and using the appropriate settings,
   *         for the data acquision modes fills up the data buffer in Fed9UBufferDescription, thus creating a Fed9U event.
   * \author Matthew Pearson, RAL.
   * \date   03/2005.
   *
   * It should be used before constructing the classes that inherit from Fed9UBufferCreator.
   *
   * \todo - add a method for zero suppressed lite mode?
   */
  class Fed9UBufferData {

  public:

    /**
     * \brief Method which puts channel data in the Fed9UBufferDescription's data buffer in both the virgin raw and processed raw DAQ formats.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     * \throw ICException Throws this exception on error conditions and will catch and rethrow all errors from calling methods as this type.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9URawData(Fed9UBufferDescription *bufDesc ) throw (ICException);

    /**
     * \brief Method which puts channel data in the Fed9U data buffer in a zero suppressed DAQ format.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9UZSData(Fed9UBufferDescription *bufDesc) throw (ICException);

    /**
     * \brief Method which puts channel data in the Fed9U data buffer in a scope mode DAQ format.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     * \param scope_mode_length Scope length of the data recorded during the event. It is the number of 32 words that are recorded for each APV input.
     *        It is taken to be the same across all FE units.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9UScopeModeData( Fed9UBufferDescription *bufDesc, unsigned short scope_mode_length ) throw (ICException);

  private:
    /**
     * \brief Method which can pad out the buffer until the start of the next 64-bit word.
     * \param bufDesc Pointer to the Fed9UBufferDescription that the class is processing.
     * \param cBytes Number of bytes used in the buffer for all the clusters in one FE unit. If operating in either virgin or processd raw mode
     *        this is zero.
     *
     * The ends of the FE data blocks in the FED buffer are padded out with zeros until the start of the next 64-bit words.
     */
    void paddToNext64BitWord(Fed9UBufferDescription *bufDesc, unsigned int cBytes) throw (ICException);

    Fed9UEventInfo mFed9UEventInfo; //!< See Fed9UEventInfo for more details on this class.

  };

}

#endif //FED9UBUFFERDATA_H
#ifndef FED9UBUFFERHEADER_H
#define FED9UBUFFERHEADER_H


#include "ICExDecl.hh"

namespace Fed9U {

  //using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  This class provides methods to construct the header part of the Fed9U buffer.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It should be used to compose classes which inherit from Fed9UBufferCreator.
   */
  class Fed9UBufferHeader {
    
  public:

    /**
     * \brief This constructs the Fed9U DAQ header at the start of the Fed9U buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is 8 bytes, which contains the bunch crossing number and FED mode.
     */
    void generateFed9UDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException);

    /**
     * \brief This constructs the Fed9U Tracker Special DAQ header in the 2nd 64-bit word of the Fed9U buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is another 8 bytes which contain FED debug information.
     */
    void generateFed9UTrackerSpecialDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException);

    /**
     * \brief This adds the FE header information to the Fed9U buffer. 
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is 16 bytes per FE (128 bytes in total). The feLength vector inside the Fed9UBufferDescription should contain 
     * the 8 FE lengths in bytes for all channel info for that FE (including channel header and packet code).
     * They do not include the length of the FE header itself.
     */
    void generateFed9UFEHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException);

  private:

    Fed9UEventInfo mFed9UEventInfo; //!< Contains the various event settings. See Fed9UEventInfo class for more information.

  };

}

#endif //FED9UBUFFERHEADER_H
#ifndef FED9UBUFFERCREATOR_H
#define FED9UBUFFERCREATOR_H

namespace Fed9U {
  
  class Fed9UBufferDescription;

  /**
   * \brief  An abstract base class to provide an interface for creating Fed9U buffers.
   * \author Matthew Pearson, RAL.
   * \date   02/2005.
   */
  class Fed9UBufferCreator {
    
  public:
    
    /**
     * \brief A pure virtual function that is used to generate the Fed9U buffer in a given implementation.
     * \param Fed9UBufferDescription* Pointer to a description of the buffer that is defined by the Fed9UBufferDescription.
     *        It is from this that implementations of this method should generate the buffer.
     */
    virtual void generateBuffer(Fed9UBufferDescription *) = 0;
    
    /**
     * \brief Destructor.
     */
    virtual ~Fed9UBufferCreator() {};
    
  protected:
    
    /**
     * \brief Constructor.
     *
     * Declared protected as this is a pABC only dervied classes can instantiate an object.
     */
    Fed9UBufferCreator(void) {};
  
  };
  
}

#endif //FED9UBUFFERCREATOR_H
#ifndef FED9UBUFFERCREATORRAW_H
#define FED9UBUFFERCREATORRAW_H


#include <vector>

namespace Fed9U {

  //using.*std::vector;

  class Fed9UBufferDescription;
  
  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the virgin raw data specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorRaw : public Fed9UBufferCreator {
    
  public:
    
    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U virgin raw data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with virgin raw data specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In virgin raw data mode the data is left untouch and is exactly as it comes from the FED.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);
    
  private:
  
    /**
     * \brief In virgin raw data mode the strip data from the APVs is in the APV-MUX order and this member function
     *        will put strip ordered data into APV-MUX ordered data.
     * \param bufferData Reference to the buffer data to be reordered into MUX ordering. Size: 24,576, number of strips on the FED.
     * \throw ICException  Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * \todo - method very inefficient. Improve!
     */
    void disOrderData(std::vector<unsigned short> &bufferData) const throw (ICException);
      
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.
    
  };
  
}

#endif //FED9UBUFFERCREATORRAW_H
#ifndef FED9UBUFFERCREATORPROCRAW_H
#define FED9UBUFFERCREATORPROCRAW_H


namespace Fed9U {

  class Fed9UBufferDescription;

  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the processed raw data specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorProcRaw : public Fed9UBufferCreator {

  public:

    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U processed raw data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with processed raw data specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In processed raw data mode the FED will perform strip reodering and pedestal subtraction.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);

  private:

    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

  };

}

#endif //FED9UBUFFERCREATORPROCRAW_H
#ifndef FED9UBUFFERCREATORZS_H
#define FED9UBUFFERCREATORZS_H


#include <vector>

namespace Fed9U {

  class Fed9UBufferDescription;

  //using.*std::vector;

  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the zero suppression mode specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorZS : public Fed9UBufferCreator {

  public:

   /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U zero suppression mode data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with the zero suppression mode specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In zero suppression mode full data processing is performed. Strip re-odering, pedestal subtraction, common mode median subtraction and cluster finding.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);

  private:

    /**
     * \brief Method which will calculate the FE lengths and channel lengths (in bytes) needed to construct the buffer.
     * \param bufferData Contains the ADV values for all the strips in a FED. Size:24,576.
     * \param feLengths Lengths of the data in 32 bit words that are stored on each FE unit. Size:8.
     * \param channelLengths Length of the data in  32 bits words that are store on each FED channel. Size:96.
     *
     * It takes in the bufferData vector, which contains ADC values for all the strips in a FED, and searches for
     * non-zero values and clusters of values. The appropriate channel lengths and FE lengths are counted up during the 
     * vector search.
     */
    void findDataLengths(const std::vector<unsigned short> &bufferData, 
			 std::vector<unsigned short> &feLengths, 
			 std::vector<unsigned short> &channelLengths) throw (ICException);
      
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

  };

}

#endif //FED9UBUFFERCREATORZS_H
#ifndef FED9UBUFFERCREATORSCOPEMODE_H
#define FED9UBUFFERCREATORSCOPEMODE_H


namespace Fed9U {
  
  class Fed9UBufferDescription;
  
  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the scope mode specific values.
   * \author Rob Bainbridge, IC
   * \date   09/2005
   */
  class Fed9UBufferCreatorScopeMode : public Fed9UBufferCreator {
    
  public:

    /**
     * \brief Constructor.
     * \param length Length of the data recorded while in scope mode. The scope length.
     */
    Fed9UBufferCreatorScopeMode( unsigned short length = 280 );

    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U scope mode data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with scope mode specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In scope mode the full APV frames are recorded upon receipt of a trigger. A scope lengths worth of 23 bit words of the frame is recorded.
     */
    void generateBuffer( Fed9UBufferDescription *bufDesc ) throw (ICException);
    
  private:
    
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

    unsigned short length_;         //!< Scope length used during the event recording.
    
  };
  
}

#endif // FED9UBUFFERCREATORSCOPEMODE_H
#ifndef FED9UBUFFERGENERATOR_H
#define FED9UBUFFERGENERATOR_H


#include "ICExDecl.hh"

#include<vector>

namespace Fed9U {

  class Fed9UBufferCreator;

  //using ICUtils::ICException;

  /**
   * \brief  User interface class to create Fed9U buffers.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It can be used to create any type of Fed9U buffer, providing it has been given a pointer to an object which inherits from Fed9UBufferCreator.
   * This class gives the user an interface to set various buffer paremeters, such as the data, bunch crossing number, etc. It also provides
   * methods which allow the user to get a copy of the buffer once it has been generated.
   */
  class Fed9UBufferGenerator {

  public:
    /**
     * \brief Constructor. Takes in a pointer to the concrete implementation of Fed9UBufferCreator.
     */
    Fed9UBufferGenerator(Fed9UBufferCreator *theCreator);

    /**
     * \brief Method to generate the Fed9U buffer.
     * \param bufferData Reference to the vector containing the ADC values for each strip on a FED. Size:24,576.
     * \throw ICException Throws ICExceptions for all errors generated by the member function and caught and rethrow by the member function.
     *
     * The first element of the vector corresponds to strip 1, channel 96 in the external numbering or strip 0 or channel 0 internal numbering.
     * The strip order should then follow in physical strip ordering and not APV-MUX order.
     */
    void generateFed9UBuffer(const std::vector<unsigned short> &bufferData) throw (ICUtils::ICException);

    /**
     * \name Buffer getters.
     *
     * The methods to allow the user to get the buffer which is created.
     */
    //@{
    /**
     * \brief Method to copy the Fed9U buffer from internal memory into an area pointed to by the given pointer.
     * \param fedBuffer Area to copy memory to.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    void getBuffer(unsigned int *fedBuffer) const throw (ICUtils::ICException);

    /**
     * \brief  Method to return copy of Fed9U buffer.
     * \return vector<unsigned char> A vector containing a copy of all the data in the internal buffer.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    std::vector<unsigned char> getBuffer() throw (ICUtils::ICException);
    //@}

    /**
     * \name Parameter getters
     *
     * Methods for retrieving the buffer parameters.
     */

    /**
     * \brief  Method to get the buffer size in units of 32-bit words.
     * \return unsigned long Number of 32-bit words in the buffer.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    unsigned long getBufferSize(void) const throw (ICUtils::ICException);

    /**
     * \brief  Method to get the bunch crossing number.
     * \return unsigned long Bunch crossing number for which the event was recorded.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    unsigned long getBunchCrossing(void) const throw (ICUtils::ICException);

    //@}

    /**
     * \name Buffer setters
     *
     * Methods for the setting buffer parameters.
     */
    //@{

    /**
     * \brief  Method to set the bunch crossing number.
     * \param  bunchCrossing Bunch crossing number for which the event was recorded.
     * \return Self reference.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setBunchCrossing(unsigned long bunchCrossing) throw (ICUtils::ICException);

    /**
     * \brief  Method to set the tracker header to full debug mode.
     * \return Self reference
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * The method provides no way to undo this once it has been done. The default mode for an event buffer is APV error mode
     * and no change should be made if this mode is required.
     */
    Fed9UBufferGenerator & setHeaderToFullDebug(void) throw (ICUtils::ICException);

    /**
     * \brief  Method to set the buffer creator pointer. This allows the user to change mode between generating events.
     * \param  theCreator The concrete implementation of the FED buffer that is to be set.
     * \return Self reference.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setFed9UBufferCreator(Fed9UBufferCreator *theCreator);

    /**
     * \brief  Method to set the common mode median values for the buffer when running in zero suppressed full debug mode.
     * \param  cmMedians Reference to a vector who's elements contain the common mode median values calculated for the APVs
     *         in zero suppression mode. There is one element for each APV on the FED and it uses the internal numbering with
     *         a base of zero. Size:192.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setCommonModeMedians(const std::vector<unsigned short> &cmMedians) throw (ICUtils::ICException);

    Fed9UBufferGenerator & setSlink64() throw (ICUtils::ICException);

    //@}
 
 private:

    Fed9UBufferCreator *mFed9UBufferCreator;        //!< Pointer to the Fed9UBufferCreator concrete implementation class, used to create
                                                    //!< the appropriately structure buffer.

    Fed9UBufferDescription mFed9UBufferDescription; //!< Provides a description of the buffer that is to be created. See Fed9UBufferDescription for more details.

    Fed9UEventInfo mFed9UEventInfo;                 //!< Contains the various event settings. See Fed9UEventInfo class for more information.

  };

}

#endif
/** Jo Cole September 2006 */

#ifndef H_Fed9UFakeBufferCreator
#define H_Fed9UFakeBufferCreator

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using namespace std;

  /** New fake event buffer construction class.  Used by streamlined event unpacking objects when using fake events.*/

  class Fed9UFakeBufferCreator {
    
  public:
    /** Default constructor. */
    Fed9UFakeBufferCreator() : _mode(FED9U_EVENT_MODE_FAKE_FULL), _size(0) {};
    /** Standard constructor. */
    //added new parameter, see .C file for details (8/18/2015 AAB)
    Fed9UFakeBufferCreator(istream & is, int uniqueChannels = 1);

    /** Default destructor. */
    ~Fed9UFakeBufferCreator();

    /** Returns the size of the created buffer (in bytes).*/
    u32 getBufferSize() const {return _size;}
    /** Returns a point to the created buffer.*/
    u32 * getPointerToBuffer() const {return _buffer;}
    /** Returns the data mode (required by Fed9UEventStreamLine when using fake events).*/
    Fed9UEventDataMode getDataMode() const {return _mode;}

  private:
    u32 * _buffer;
    Fed9UEventDataMode _mode;
    u32 _size; // buffer size in bytes
    
  };
  
}

#endif // H_Fed9UFakeBufferCreator
/** Jo Cole February 2007 */

#ifndef H_Fed9USu8
#define H_Fed9USu8

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  //<JEC date=21/03/07>
  /** Replacement for Fed9UEventIterator class.  Used in Fed9UEventStreamLine and Fed9UEventUnitStreamLine.*/
  
  class Fed9USu8 {
    
  public:
    /** Default constructor. */
    Fed9USu8() : _dataPtr(0), _off(0), _initialOffset(0) {}
    /** Standard constructor. */
    Fed9USu8(u8 * dataPtr) : _dataPtr(dataPtr), _off(0), _initialOffset(0) {}
    Fed9USu8(u8 * dataPtr, u32 offset) : _dataPtr(dataPtr), _off(offset), _initialOffset(offset) {}
    /** Default destructor. */
    ~Fed9USu8() {}

    /** Separate method to set the private data members. */
    void set(u8 * dataPtr = 0, u32 offset = 0) {_dataPtr = dataPtr; _off = offset; _initialOffset = offset;}
    /** Reset the offset to whatever it was either when it was instantiated or when the last set command was called.*/
    void reset() {_off = _initialOffset;}
    /** Delete the pointer.*/
    void clear() {  if (_dataPtr != NULL ) delete[] _dataPtr; }
    /** Return the value from the buffer at a specific offset from where the buffer is pointing to.
     *  Bear in mind that if _off != 0, it will play a role in deciding which value you return.
     */
    u8 getu8(u32 offset) const {return (*this)[offset];}
    /** Determines if the pointer has been set yet or not.*/
    bool comparePtr() const {
      if (_dataPtr == NULL) {return true;}
      return false;
    }

    /** getter for the underlying u8 pointer.*/
    u8 * getPointer() const {return _dataPtr;}
    /** getter for the value of the offset.*/
    u32 getOffset() const {return _off;}
    
    /** overloaded [] operator.  Takes care of the data packing versus data access ordering in the buffer. */
    u8 & operator [] (u32 offset) const {return _dataPtr[(_off + offset) ^ 3];}
    /** overloaded dereference operator.  Returns value at the current location of the pointer. 
     *  NB.  The value of _off matters !*/
    u8 & operator * () const { return (*this)[0]; }

    /** overloaded prefix and postfix increment operators.*/
    Fed9USu8 & operator ++ () {++_off; return *this;}
    Fed9USu8 operator ++ (int) {
      Fed9USu8 temp(*this);
      (*this) += 1;
      return temp;
    }

    /** overloaded increment and assign operator.*/
    const Fed9USu8 & operator += (u32 rhs) {_off += rhs; return *this;}
    const Fed9USu8 & operator += (int rhs) {_off += static_cast<u32>(rhs); return *this;}

    /** overloaded + operator.*/
    const Fed9USu8 & operator + (u32 rhs) {_off += rhs; return *this;}
    
    /** overloaded prefix decrement operator and decrement and assign operator.*/
    Fed9USu8 & operator --() {
      ICUTILS_VERIFY(_off > 0)(_off).error();
      --_off; return *this;
    }
    Fed9USu8 & operator -=(u32 rhs) {
      ICUTILS_VERIFY(_off > rhs)(_off)(rhs).error();
      _off -= rhs; return *this;
    }
    
    /** overloaded assignment operator*/
    const Fed9USu8 & operator =(const Fed9USu8 & rhs) {
      if (this == &rhs) {return *this;}
      _dataPtr = rhs.getPointer();
      _off = rhs.getOffset();
      _initialOffset = rhs.getInitialOffset();
      return *this;
    }
    
  private:
    /** getter for the initial offset value.  This should only be needed within this class, as initialOffset should only
     * be set when the object is instantiated or the set command is called, when it is set equal to the main offset.
     * Needed for the assignment operator.*/
    u32 getInitialOffset() const {return _initialOffset;}
    
    /** The actual pointer for which this class is a wrapper.*/
    u8 * _dataPtr;
    /** _off is an offset which is needed if you want to access anything other than the first byte of the 
     * buffer when you use the overloaded dereference operator.
     *
     * _initialOffset is an extra offset in case you want to reset the value of _off.  This is needed
     * in Fed9UEventUnitStreamLine when returning a Fed9USu8 special pointer.*/
    u32 _off, _initialOffset;
  };
  //</JEC>
}

#endif // H_Fed9USu8
/*   Exception class for Fed9UEventStreamLine which inherits from ICException*/

#ifndef _Fed9UEventStreamLineException_H_
#define _Fed9UEventStreamLineException_H_

//Check if using Xerces version 1
//#if XERCES > 1

//Check Xerces version
//#include "xercesc/util/XercesVersion.hpp"
//#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UEventStreamLineException class to be used when throwing errors within the Fed9UEventStreamLine class.*/

#define FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED9UEVENTSTREAMLINE, "ERROR:  An error occurred in Fed9UEventStreamLine.")  \
  IC_DEF_ERROR(ERROR_FEUNIT_DISABLED, "ERROR:  Attempt to access a disabled FE-unit.")  \
  IC_DEF_ERROR(ERROR_DAQ_MODE_UNKNOWN, "ERROR: The daq mode set in the event data is unknown/ or unhandled.")  \
  IC_DEF_ERROR(ERROR_GET_SAMPLES_FAILED, "ERROR: The method getSamples() has failed to complete correctly.")  \
  IC_DEF_ERROR(ERROR_FRONT_END_PACKET_CODE_CORRUPT, "ERROR: The packet code on one of the front ends was corrupt.") \
  IC_DEF_ERROR(ERROR_FEUNIT_OUT_OF_BOUNDS, "ERROR:  Input FE-FPGA number must run between 1 and 8. ") \
  IC_DEF_ERROR(ERROR_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input channel number must run between 1 and 12. ") \
  IC_DEF_ERROR(ERROR_APV_OUT_OF_BOUNDS, "ERROR:  Input APV number must be either 1 or 2. ") \
  IC_DEF_ERROR(ERROR_BAD_FRONT_END_PACKET_CODE, "ERROR: Information requested which does not exist in this payload format.") \
  IC_DEF_ERROR(ERROR_DATA_BUFFER_CORRUPT, "ERROR: Data buffer is either empty or corrupted.") \
  IC_DEF_ERROR(ERROR_FED_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input FED channel number must run between 1 and 96. ") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")

  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UEventStreamLineException, FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

//#endif

//#endif

#endif
/** Jo Cole June 2006 
 *  Updated March 2007   
 */

#ifndef H_Fed9UEventUnitStreamLine
#define H_Fed9UEventUnitStreamLine

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using std::vector;

  /**  New simplified FE-FPGA class - used by the new streamlined Fed event unpacking class.
       NB.  When a method requires the FE-unit or channel number, you should give the number of the FPGA/channel you want, NOT
       the location in the header/payload.  For example, FE-FPGA number 8 is always first in the header or payload, so give
       Fe-unit number = 8 to access the information associated to it.  A similar approach should be used for the channel numbering,
       bearing in mind that channel numbers are quoted with respect to the associated FE-FPGA, not with respect to the FED, ie.
       channels are numbered 1 - 12, not 1 - 96.
  */
  //<JEC date=21/03/07>  several changes including the use of the u8 pointer wrapper, Fed9USu8.
  
  class Fed9UEventUnitStreamLine {
    
  public:
    /** Default constructor.*/
    Fed9UEventUnitStreamLine() : _headerFormat(FED9U_HEADER_FULLDEBUG), _unit(0) {
      _header.set(NULL);
      _payload.set(NULL);
      _sample.set(NULL);
    };
    /** Standard constructor, which should be used for normal data.
	It should be given a pointer to the start of the FE-Unit-related header information, a pointer to the start of
	the associated payload data, the Header format (FULL DEBUG or APV ERROR) and the unit number.*/
    Fed9UEventUnitStreamLine(u8 * header, u8 * payload, Fed9UHeaderFormat headerFormat, u32 unit);

    /** Returns a pointer to the end of the payload data (including the padding).*/
    u8 * getPointerToEndOfUnitPayload();
    /** Returns either the frame synch out packet (FULL DEBUG mode) or the APV error flags (APV ERROR mode), as stored in the 
	tracker-specific header.  The information is returned in a u8 vector, with the LSB stored in element 0.  
	The vector should be three elements long for the APV error mode and sixteen elements long for FULL DEBUG mode.*/
    vector<u8> getFeUnitHeader() const;
    /** Returns the status of a specified channel, as stored in the header.  If the data is in FULL DEBUG mode, then the
	status code will be 6 bits long, but if it is in APV ERROR mode, then it will be only 2 bits long.*/
    u8 getChannelStatus(u32 channel) const;
    /** Returns the data length for a given FE-FPGA, as stored in the FULL DEBUG mode of the tracker-specific header.
	If the header is in APV ERROR mode, then the length is simply zero.*/
    u16 getUnitDataLengthFromHeader() const;
    /** Returns the majority APV pipeline address, as stored in the FULL DEBUG mode of the tracker-specific header.
        If the header is in APV ERROR mode, then the address is simply zero.*/
    u8 getMajorityPipelineAddress() const;
    /** Returns the packet code, as stored in the payload, which identifies the mode which the data was taken.
	The options are:
	Scope mode                - packet code = xxx0 0101
	Virgin raw data mode      - packet code = xxx0 0110
	Processed raw data mode   - packet code = xxx1 0010
	Zero suppressed mode      - packet code = xxx0 1010
	Zero suppressed lite mode - packet code not stored in payload - method returns zero.
	Only the lower 5 bits are returned.  The upper three bits are reserved.*/
    u8 getPacketCode(u32 channel) const;
    /** Returns the median for a specified APV on a specific channel, as stored in the payload.  
	This information is only stored in Zero Suppressed mode.*/
    u16 getMedian(u32 channel, u32 apv) const;
    /** Returns the data length for a specified channel, as stored in the payload.
	The length is given in bytes and includes the lengths and packet code.*/
    u16 getChannelDataLength(u32 channel) const {return _channelDataLength[12 - channel];}
    /** Returns the total data length for a given FE-FPGA, based on information stored in the payload.*/
    u16 getUnitDataLengthFromPayload() const;
    /** Returns the number of data words for a given channel.  For SCOPE, VIRGIN RAW and PROCESSED RAW data modes, the
     actual number of raw data words is returned.  For the ZERO SUPPRESSED modes the maximum number of clusters is returned.*/
    u16 getSamples(u32 channel) const;
    /** Returns a Fed9USu8 pointer to the data from a specific channel.  To get a pointer to the start of the data,
	simply set dataLoc = 0.  Other values of dataLoc can be used if a pointer to a specific cluster or raw data word is required.*/
    //<JEC date=21/03/07>
    Fed9USu8 & getSampleSpecialPointer(u32 channel, u32 dataLoc);
    //</JEC>
    /** Returns a data sample at a specific location for a specific channel.
	dataLoc = 0 corresponds to the first piece of data in the payload (ie. after length, packet code and median information, where stored).
	Raw data for Scope, Virgin Raw and Processed Raw modes is stored in two bytes.  In ZS and ZS lite, the cluster data is stored in
	bytes, so using this method will return two pieces of cluster data or'ed together.*/
    u16 getSample(u32 channel, u32 dataLoc) const;
    /** Returns a vector containing the data samples for a specified channel.
	For Raw data modes, each vector element contains a two-byte data word, while for the ZS modes each vector element contains one byte.
	Use for time-critical applications.*/
    vector<u16> getSample(u32 channel) const;
    //<JEC date=18/4/07> extra method for use in Fed9UDevice (TrimDAC calibration)
    /** Retrieves only the data (ie. no strip addresses or cluster widths) for a given channel and stores them in a pre-defined buffer to which
	destBuffer is a pointer.*/
    void getSample(u32 channel, u16 * destBuffer);
    //</JEC>
    /** Returns a pointer to the position of the data from a specified FE-FPGA in the payload.*/
    //<JEC date=21/03/07> changed to return a special u8 pointer
    Fed9USu8 getFEUnitInPayloadPtr() const {return _payload;}
    //</JEC>
    /** Performs a consistency check (where possible) of channel data lengths.  Returns true if everything is OK.
	For ZERO SUPPRESSED modes channel data lengths are not expected to be the same, so the return value is always set to true.*/
    bool checkChannelLengths();

  private:

    /** The different possible packet codes for the different data formats.  ZS LITE is not included as the packet code
	is not stored in this data format.*/
    enum { FED9U_PACKET_SCOPE = 5, FED9U_PACKET_VIRGRAW = 6, FED9U_PACKET_ZEROSUPP = 10, FED9U_PACKET_PROCRAW = 18 };
    //AAB updated with more modes AAB (12/8/205)
    enum { FED9U_PACKET_SCOPE_hex = 0xE1, FED9U_PACKET_VIRGRAW_hex = 0xE6, FED9U_PACKET_VIRGRAW_10BIT = 0x86,
         FED9U_PACKET_ZEROSUPP_hex = 0xEA, FED9U_PACKET_PROCRAW_hex = 0xF2, FED9U_PACKET_ZEROSUPP_10BIT = 0x8A,
         FED9U_PACKET_ZEROSUPP_LO = 0xCA, FED9U_PACKET_ZEROSUPP_HI_LO = 0xAA,
         FED9U_PACKET_PROCRAW_10BIT = 0x92,
         FED9U_PACKET_PROCRAW_8BIT_HI_LO = 0xB2,
         FED9U_PACKET_PROCRAW_8BIT_LO = 0xD2,
         FED9U_PACKET_VIRGRAW_8BIT_HI_LO = 0xA6,
         FED9U_PACKET_VIRGRAW_8BIT_LO = 0xC6
     };
        
    /** Private method which returns the location of two consecutive bits of data.  The data is packed into each 64-bit word 
	starting from the MSB and running to the LSB, but the access to the data starts from the middle of the 64-bit word, as
	illustrated:
	63                              0
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |              --> byte-order for data packing
	+---+---+---+---+---+---+---+---+
	| 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 |              --> byte-order for data access
	+---+---+---+---+---+---+---+---+
	
	This routine takes the given channel and location within that channels data in the data packing format and converts it
	into the location of two consecutive words in the data access format.  The location is returned in terms of a number of
	full 64-bit words and then the locations of the two words with respect to the last (incomplete) 64-bit word.*/
    void getLocations(u32 channel, u32 offset, int &nOf64BWords, int &lowerWd, int &upperWd) const;
    int getLocations(u32 channel, u32 offset) const;
    
    /** A pointer to the start of the FE-Unit-specific data in the event header.*/
    Fed9USu8 _header, _payload, _sample;
    /** The header format:  FULL DEBUG or APV ERROR.*/
    Fed9UHeaderFormat _headerFormat;
    /** FE-Unit number.*/
    u32 _unit;
    /** Vector containing the data length for each channel.*/
    vector<u16> _channelDataLength;
  };
  //</JEC>  
}

#endif // H_Fed9UEventUnitStreamLine
/** Jo Cole June 2006 
 *  Updated March 2007
 *  Updated April 2007 to handle 32-bit word swapping
 */

#ifndef H_Fed9UEventStreamLine
#define H_Fed9UEventStreamLine

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {
  
  using namespace std;
  
  class Fed9UEventUnitStreamLine;
  
  /**  New streamlined FED event unpacking class.
       NB.  When a method requires the FE-unit or channel number, you should give the number of the FPGA/channel you want, NOT
       the location in the header/payload.  For example, FE-FPGA number 8 is always first in the header or payload, so give
       Fe-unit number = 8 to access the information associated to it.  A similar approach should be used for the channel numbering,
       bearing in mind that channel numbers are quoted with respect to the associated FE-FPGA, not with respect to the FED, ie.
       channels are numbered 1 - 12, not 1 - 96.
  */
  
  class Fed9UEventStreamLine {
    
  public:
    /** Default constructor.*/
    Fed9UEventStreamLine() : 
      _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_STANDARD), _headerFormat(FED9U_HEADER_FULLDEBUG), 
      _fedDescription(0), _rubbish(0), _bufferLength(0), _deleteBuffer(false) 
    {
      fakeBuffer = NULL;
      _daqHeader = NULL;
      _trackerHeader = NULL;
      _trailer = NULL;
      _feUnitHeader = NULL;
      _payload = NULL;      
    };
    /** Constructor for handling fake events.  Uses Fed9UFakeBufferCreator class to construct the buffer.*/
    //(AAB 8/24/2015) Added support for multiple unique channels loaded in fake events, let uniqueChannels = 1 for other behavior
    Fed9UEventStreamLine(istream & is, int uniqueChannels);
    /** Constructor for handling real data events */
    Fed9UEventStreamLine(u32 * buffer, const Fed9UDescription * currentDescription);
    /** Default destructor. */
    ~Fed9UEventStreamLine();
    
    /** Initialisation method.  Used by all constructors to create a set of pointers to different parts of the header, payload 
	and trailer, regardless of whether the event is real or fake.*/
    void Init(u32 * buffer, const Fed9UDescription * fedDescription);    
    /** Fills a vector with pointers to a set of Fed9UEventUnitStreamLine objects (one for each active FE-FPGA).*/
    void setPointerToUnitDataInPayload();
    
    // methods for extracting information from the DAQ header

    /** Returns the L1 event number returned by the TTC system, as stored in the DAQ header.*/
    //<JEC date=11/4/07> handle 32-bit swapping of DAQ header and trailer
    //    u32 getEventNumber() const {return (*(_daqHeader + 2) << 16 | *(_daqHeader + 1) << 8 | *_daqHeader);}
    u32 getEventNumber() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 2) << 16 | *(_daqHeader + 1) << 8 | *_daqHeader);
      } else {
	return (*(_daqHeader + 6) << 16 | *(_daqHeader + 5) << 8 | *(_daqHeader + 4));
      }
    }
    /** Returns the Event type identifier, as stored in the DAQ header. 
	For details of the meaning of this number, see
	http://cmsdoc.cern.ch/cms/TRIDAS/horizontal/ */
    //    u8 getDaqEventType() const {return (*(_daqHeader + 3) & 0xF);}
    u8 getDaqEventType() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 3) & 0xF);
      } else {
	return (*(_daqHeader + 7) & 0xF);
      }
    }
    /** Returns the bunch crossing number, as stored in the DAQ header.
	The number is reset every orbit.*/
    //    u16 getBunchCrossingNumber() const {return (*(_daqHeader + 7) << 4 | ((*(_daqHeader + 6) >> 4) & 0xF));}
    u16 getBunchCrossingNumber() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 7) << 4 | ((*(_daqHeader + 6) >> 4) & 0xF));
      } else {
	return (*(_daqHeader + 3) << 4 | ((*(_daqHeader + 2) >> 4) & 0xF));
      }
    }
    /** Returns the unique data source identifier, as stored in the DAQ header.
	For the tracker, this number will lie in the range 50 - 489. */
    //    u16 getSourceId() const {return (*(_daqHeader + 5) | (((*(_daqHeader + 6)) & 0xF) << 8));}
    u16 getSourceId() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_daqHeader + 5) | (((*(_daqHeader + 6)) & 0xF) << 8));
      } else {
	return (*(_daqHeader + 1) | (((*(_daqHeader + 2)) & 0xF) << 8));
      }
    }
    /** Returns the version identifier of the common FED encapsulation, as stored in the DAQ header.
        The value is set by the DAQ group. */
    //    u8 getDaqFormatVersion() const {return ((*(_daqHeader + 4) >> 4) & 0xF);}
    u8 getDaqFormatVersion() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return ((*(_daqHeader + 4) >> 4) & 0xF);
      } else {
	return ((*(_daqHeader + 0) >> 4) & 0xF);
      }
    }
    //</JEC>

    // methods for extracting information from the tracker-general header

    /** Returns the identifier for the tracker header type, as stored in the tracker-specific header.
	This has two possible values:
	0001 = Full Debug mode,
	0010 = APV Error mode.*/
    Fed9UHeaderFormat getHeaderFormat() const;
    /** Returns the event or payload type identifier, as stored in the tracker-specific header.
	This has several possible values:
	0001 = Scope Mode REAL
	0010 = Virgin Raw Mode REAL
	0011 = Virgin Raw Mode FAKE
	0110 = Processed Raw Mode REAL
	0111 = Processed Raw Mode FAKE
	1010 = Zero Suppressed Mode REAL
	1011 = Zero Suppressed Mode FAKE
	1100 = Zero Suppressed Lite Mode REAL
	1101 = Zero Suppressed Lite Mode FAKE
	
	Note:  The event format for FAKE data is the same as for the corresponding REAL data mode.*/
    u8 getTrackerEventType() const;
    /** Returns the APV pipeline address provided by the APV emulator, as stored in the tracker-specific header. */
    u8 getApvEmulatorAddress() const;
    /** Returns the APV address error, as stored in the tracker-specific header.  The APV address on each FE-FPGA
	is compared with the APVE address.  Each bit of this 8-bit word corresponds to one of the FE-FPGAs.  A 1 indicates
	agreement for a given FE-FPGA, while a zero indicates a disagreement. */
    u8 getApvAddressError() const;
    /** Returns the 16-bit FED status register, as stored in the tracker-specific header.*/
    u16 getStatusRegister() const;
    /** Returns the 8-bit FE overflow register, as stored in the tracker-specific header.  Each bit corresponds to
	a FE-FPGA.  If the bit is set to 1 it indicates than an overflow condition has occurred in the corresponding
	FE-FPGA and that the data from than FPGA should be ignored.*/
    u8 getFeOverflowRegister() const;
    /** Returns the 8-bit FE enable register, as stored in the tracker-specific header.  Each bit corresponds to a
	FE-FPGA.  If the bit is set to 1, it indicates that the FE-FPGA can receive data.  If the bit is set to zero,
	there will be no payload data from that FE-FPGA.*/
    u8 getFeEnableRegister() const;
    /** Returns a boolean indicating whether a given FE-unit is enabled or not.  If the data is old-style (eg. test-beam)
	the return value will always be true.*/
    bool getFeUnitEnable(u32 unit) const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(unit > 0 && unit <=8)(unit).msg("getFeUnitEnable:  wrong FE-FPGA number given as input").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
#endif 
#endif
      if (_mode != FED9U_EVENT_MODE_LEGACY) {return static_cast<bool>((*(_trackerHeader + 7) >> (8 - unit)) & 0x1);}
      return true;
    }
    
    // methods for extracting information specific to the FULL DEBUG version of the tracker header

    /** Returns the 32-bit word stored in the middle of the second 64-bit word associated to a given FE-FPGA in the
	FULL DEBUG version of the tracker-specific header.  For most FE-FPGAs, this word is reserved, but currently
	empty.  The exceptions are:
	unit = 8   gives a copy of the BE status register.
	unit = 2   gives a copy of the second DAQ register.
	unit = 1   gives a copy of the first DAQ register.*/
    u32 getReservedWordFromFullDebugHeader(u32 unit) const;
    
    // methods for extracting information from the DAQ trailer
    
    //<JEC date=11/4/07> handle 32-bit swapping of DAQ header and trailer
    /** Returns the total event fragment length in number of 64-bit words, including the header and the trailer,
	as stored in the DAQ trailer.*/
    //    u32 getTotalDataLength() const {return ((*_trailer) | (*(_trailer + 1) << 8) | (*(_trailer + 2) << 16));}
    u32 getTotalDataLength() const {
      ICUTILS_VERIFY(_trailer != NULL)(_trailer).msg("getTotalDataLength:  trailer pointer is NULL.  Possible data corruption ?").error()
	.code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return ((*_trailer) | (*(_trailer + 1) << 8) | (*(_trailer + 2) << 16));
      } else {
	return (*(_trailer + 4) | (*(_trailer + 5) << 8) | (*(_trailer + 6) << 16));
      }
    }
    /** Returns the CRC value, as stored in the DAQ trailer.*/
    //    u16 getCrc() const {return (*(_trailer + 6) | (*(_trailer + 7) << 8));}
    u16 getCrc() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 6) | (*(_trailer + 7) << 8));
      } else {
	return (*(_trailer + 2) | (*(_trailer + 3) << 8));
      }
    }
    /** Returns the event status, as stored in the DAQ trailer.
	This is a "summary" status of the event, but also incorporates the FED working condition during the event.*/
    //    u8 getEventStatus() const {return (*(_trailer + 5) & 0xF);}
    u8 getEventStatus() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 5) & 0xF);
      } else {
	return (*(_trailer + 1) & 0xF);
      }
    }
    /** Returns the TTS bits, as stored in the DAQ trailer.*/
    //    u8 getcurrentTTSbits() const {return (*(_trailer + 4) >> 4);}
    u8 getcurrentTTSbits() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 4) >> 4);
      } else {
	return (*_trailer >> 4);
      }
    }
    /** Returns the CRC modification bit from the DAQ trailer.  If this returns 1, then the Slink sender card has modified
        the CRC value stored in the DAQ trailer.*/
    //    u8 getCRCmodificationBit() const {return (*(_trailer + 4) & 0x4);}
    u8 getCRCmodificationBit() const {
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	return (*(_trailer + 4) & 0x4);
      } else {
	return (*_trailer & 0x4);
      }
    }
    //</JEC>
    
    // methods for accessing pointers
    /** Methods are also available which return pointers to different parts of the event.
	All pointers are unsigned 8-bit pointers, ie. they step through the data in bytes.*/

    /** Returns a pointer to the start of the DAQ header, ie. the very start of the event.*/
    const u8 * getDaqHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_daqHeader != NULL )(_daqHeader).msg("DAQ header pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return  _daqHeader;
    }
    /** Returns a pointer to the start of the tracker-specific header.*/
    const u8 * getTrackerHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_trackerHeader != NULL )(_trackerHeader).msg("Tracker-specific header pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return  _trackerHeader;
    }
    /** Returns a pointer to the start of the FE-FPGA status information in the tracker-specific header.*/
    const u8 * getFEUnitHeaderPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_feUnitHeader != NULL )(_feUnitHeader).msg("FE-FPGA status pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _feUnitHeader;
    }
    /** Returns a pointer to the start of the payload.*/
    const u8 * getPayloadPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_payload != NULL )(_payload).msg("payload pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _payload;
    }
    /** Returns a pointer to the start of the DAQ trailer.*/
    const u8 * getTrailerPtr() const {
#if XERCES > 1
#if _XERCES_VERSION >= 20300
      ICUTILS_VERIFY(_trailer != NULL )(_trailer).msg("DAQ trailer pointer is null !").error().code(Fed9UEventStreamLineException::ERROR_DATA_BUFFER_CORRUPT);
#endif
#endif
      return _trailer;
    }
    
    // methods for accessing information about enabled channels/FE Units

    /** Returns the number of enabled FE-FPGAs on the FED.*/
    u32 enabledFeUnits() const;
    
    // general event-level methods

    /** Creates the output required for an Igor File, as used in FedDebugSuite.*/
    void saveIgorFile(ostream & o) const;

    /** Returns a pointer to the specified FeUnit.*/
    //<JEC date=20/03/07> moved here to get automatic inline
    Fed9UEventUnitStreamLine & getFeUnit(u32 feUnit) {
      ICUTILS_VERIFY(feUnit > 0 && feUnit <=8)(feUnit).msg("getFeUnit:  wrong FE-FPGA number given as input").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
      ICUTILS_VERIFY(getFeUnitEnable(feUnit))(feUnit)(getFeUnitEnable(feUnit)).msg("getFeUnit:  Requested FE-FPGA is disabled !").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
      return _feUnits[8 - feUnit];
    }
    //</JEC>

    //<JEC date=18/4/07> extra method for converting a FED channel into an Fe-unit & channel number
    Fed9UEventUnitStreamLine & getFeUnit(u32 fedChannel, u32 & feUnitChannel) {
      ICUTILS_VERIFY(fedChannel > 0 && fedChannel <= 96)(fedChannel).msg("getFeUnit:  FED channel number out of bounds !").error()
	.code(Fed9UEventStreamLineException::ERROR_FED_CHANNEL_OUT_OF_BOUNDS);
      u32 feUnit = ((fedChannel - 1)/12) + 1;
      ICUTILS_VERIFY(getFeUnitEnable(feUnit))(feUnit)(getFeUnitEnable(feUnit)).msg("getFeUnit:  Requested FE-FPGA is disabled !").error()
	.code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
      feUnitChannel = fedChannel - ((feUnit - 1)* 12);
      return _feUnits[8 - feUnit];
    }
    //</JEC>

    /** Creates the output required to create a binary dump of the buffer.
	Used by FedDebugSuite and can also be used for fake event input to this class.*/
    void dumpRawBuffer(ostream& os) const;
    /** Provides a prettified text dump of the buffer for debugging purposes.*/
    //Added second and third parameter for FedPatternCheck (AAB 8/24/2015)
    void dumpBuffer(ostream& os, int modFactor = 1, bool lessOutput = false) const;
    /** This method checks the status of every channel on every enabled FE-FPGA on the FED and reports any error found.*/
    void checkChannelStatuses() const;

    /** This method performs some basic checks of the event.  It checks that the length of the buffer matches that that was
     originally created for fake events, it also checks that the CRC value stored in the trailer is as expected.  It also
    checks that the event type is correct.*/
    // TODO event length code only valid for fake data - needs updating for real data.
    void checkEvent() const;

    /** Allows the Event mode to be set externally.  Needed when using Fake Events.*/
    void setEventDataMode(Fed9UEventDataMode mode) {_mode = mode;}
    Fed9UEventDataMode getDataMode() {return _mode;}

    Fed9UEventFormatType getEventFormat() {return _eventFormat;}

    /** Calculates the CRC value for the event, for comparison to that stored in the DAQ trailer.
        Calculation is based on tables.*/
    u16 calcCrc() const;
    
  protected:
    /** Original pointer to the event buffer.*/
    u32 * _buffer;
    
    /** finer granularity pointers for the DAQ header, the tracker-specific header and the DAQ trailer.*/
    u8 * _daqHeader;
    u8 * _trackerHeader;
    u8 * _trailer;
    /** finer granularity pointers for accessing the FE-unit information.*/
    u8 * _feUnitHeader;
    u8 * _payload;
    
    /** _mode is an internal variable which flags if the data is in the standard data format, or in 
	an older data format (used mainly for test-beam) in which the tracker-specific header was one 64-bit
	word shorter than currently.*/
    Fed9UEventDataMode _mode; 
    
    //<JEC date=11/4/07> new internal variable to handle 32-bit swapping in DAQ header and trailer
    Fed9UEventFormatType _eventFormat;
    //</JEC>

    /** An internal variable which records whether the header is in FULL DEBUG mode or APV ERROR mode.*/
    Fed9UHeaderFormat _headerFormat;
    /** FED description.  Required for checks of the channel status.*/
    const Fed9UDescription * _fedDescription;
    
  private:
    /** Pointer to a FakeBufferCreator object - used only for fake data.*/
    Fed9UFakeBufferCreator * fakeBuffer;
    
    /** Vector of pointer to the associated FE-unit objects.  Pointer will be set to NULL for a given
	FE-FPGA if it has been disabled.*/
    //<JEC date=21/03/07>  changed from vector of pointers, to vector of objects.  Should be faster !
    vector<Fed9UEventUnitStreamLine> _feUnits;
    //</JEC>
    
    /** _bufferLength is total event length (minus the trailer) in bytes.
	_rubbish allows for the DAQ or tracker-specific trailer to be of non-zero length.*/
    u32 _rubbish, _bufferLength;
    //<JEC date=06/02/2007> added to deal with differences between buffer from real events and fake events
    bool _deleteBuffer;                       //!< Used to flag if the buffer is to be deleted.
    //</JEC>
    
  };
  
  inline std::ostream& operator<<(std::ostream& os, const Fed9UEventStreamLine & ev) { ev.dumpBuffer(os); return os; }
  
}

#endif // H_Fed9UEventStreamLine
#ifndef _Fed9ULogTemplate_H_
#define _Fed9ULogTemplate_H_


#include <pthread.h>

#include <iomanip>
//#include <ostream>
#include <sstream>
#include <typeinfo>

namespace Fed9U {

  template<typename T> class Fed9UMessage;
  class Fed9ULog;

  /**
   * \brief  A template class that once instantiated creates a wrapper for an output stream, making each write to that stream
   *         via the operator<< atomic through mutexes.
   * \author Gareth Rogers, Jonathan Fulcher.
   * \date   2006/09/01
   *
   * The stream handles its own resource locking, thus providing thread safety. It is possible that it will be unable to construct
   * and initialise the mutex it uses for resource locking. In this case the class will still construct, however it will no longer
   * be thread safe and a write or function calls can no longer be guarantied to be atomic. A check to ensure the class is initialised
   * fully is provided though a public member function, it also prints a message to the stream warning of this failure. This should
   * almost never happen.\n\n
   *
   * There are two member functions that are not thread safe, as these are used for reporting errors that occured while attempting
   * to achieve resource locks to the stream, getLockErrorNumber and getUnlockErrorNumber. As these report errors that happen while
   * thread locking or unlocking is in progress it is not possible to provide atomicity for these functions. If errors occur while
   * messages are being written to the stream then the data will be written anyway in a non thread safe way.
   *
   * The stream supports the writing of all data types that can be written to a std::ostream, as well as all the manipulators.
   */
  class Fed9UStream : public std::ostream {

  public:

    /**
     * \brief Constructor.
     * \param os A pointer to the output stream that the messages are to be sent to.
     *
     * The constructor also initialises the mutex that will protect the writes to the underlying buffer and provide
     * thread safety for the buffer writes. If mutex initialisation fails then the class will construct successfully
     * and it should be checked that it is thread safe by checking the appropriate public member function.
     */
    explicit Fed9UStream(std::ostream* const os);

    //<GJR date=26/11/2006>
    /**
     * \brief Constructor.
     * \param filename Name of the file that is to be used to write log messages to, if an empty string is passed std::cout
     *        will be used and no file is created.
     *
     * This will initialise the log stream with a file as its underlying buffer, whose name is provided upon construction.
     * The constructor also initialises the mutex that will protect the writes to the underlying buffer and provide
     * thread safety for the buffer writes. If mutex initialisation fails then the class will construct successfully
     * and it should be checked that it is thread safe by checking the appropriate public member function.
     */
    explicit Fed9UStream(const std::string& fileName);
    //</GJR>

    /**
     * \brief Destructor.
     *
     * Release any new'd memory.
     */
    ~Fed9UStream();

    /**
     * \brief  Returns the state of the class mutex.
     * \return boolean. True if the mutex has been correctly initialised, false if it has not.
     *
     * If a false is return the operator<< is no longer guarantied to be atomic and will behave
     * for example like std::cout in a threaded environment.
     */
    bool getMutexIsInitialised() const;

    /**
     * \brief  Writes the time to the file.
     * \return Self reference.
     */
    Fed9UStream& stamp();

    /**
     * \brief  Allows the std::ostream object that the Fed9UStream writes to be moved to a new stream.
     * \param  os Pointer to the std::ostream whose buffer we wish to use.
     * \return Self reference.
     *
     * This is an atomic operation, provided the mutex has been initialised, however moving a buffer within multiple threads
     * will cause difficult to follow behaviour as the order in which the threads move the buffer can never be gaurantied.
     * Never the less one thread can safely change the output buffer while other threads may be trying to write to it.
     * The User will have to ensure that it is done in a safe order.
     */
    Fed9UStream& setNewOstream(std::ostream* const os);

    /**
     * \brief  Allows the std::ostream object that the Fed9UStream writes to be moved to a new file.
     * \param  filename Name of the file that is to be used for the log, if an empty string is passed then it will be redirected
     *         to std::cout.
     * \return Self reference.
     *
     * This is an atomic operation, provided the mutex has been initialised, however moving a buffer within multiple threads
     * will cause difficult to follow behaviour as the order in which the threads move the buffer can never be gaurantied.
     * Never the less one thread can safely change the output buffer while other threads may be trying to write to it.
     * The User will have to ensure that it is done in a safe order.
     */
    Fed9UStream& setNewOstream(const std::string& filename);

   private:

    /**
     * \brief  Writes the current error returned (can be success) from the most recent lock attempt and returns the error
     *         number itself.
     * \return int Error number that occured. It can be decoded as follows using the following defines by the pthread library:
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct. 
     *         \li EDEADLK A thread attempted to relock an already held mutex, and the mutex is an errorcheck mutex type.
     *
     * The class guards against most of these errors and will prevent most from occuring, even if abused. This is not thread
     * safe.
     */
    int getLockErrorNumber();

    /**
     * \brief  Writes the current error returned (can be success) from the most recent unlock attempt and returns the error
     *         number itself.
     * \return int Error number that occured. It can be decoded as follows using the following defines by the pthread library:
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct.
     *         \li EPERM The mutex is not currently held by the caller.
     *
     * The class guards against most of these errors and will prevent most from occuring, even if abused. This is not thread
     * safe.
     */
    int getUnlockErrorNumber();

    /**
     * \brief  Initialises the class mutex, used to protect write operation within the member functions.
     * \return bool Returns true of the initialisation of the mutex was successful.
     */
    bool initMutex();

    /**
     * \brief  Writes an error number from any mutex operation to the stream. It takes the error number to be written and is
     *         thus re-enterant.
     * \param  errNum Error number that occured.
     * \return Self reference.
     *
     * The following errors will be written to the stream as text, any other will just have their number written.
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct. 
     *         \li EDEADLK A thread attempted to relock an already held mutex, and the mutex is an errorcheck mutex type.
     *         \li EBUSY The mutex is currently owned by another thread.
     *         \li ENOMEM The system cannot allocate the resources required to create the mutex. 
     */
    Fed9UStream& setMutexErrorNumber(int errNum);

  public:

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UStream, such as std::endl;
     * \param  std::ostream& (*pf)(std::ostream&) A pointer to a function that takes a std::ostream& as an arugment and returns a std::ostream&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::ios_base as arguments, any manipulator can be written to the Fed9UStream objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UStream& operator<<(std::ostream& (*pf)(std::ostream&));

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UStream, such as std::endl;
     * \param  std::ios_base& (*pf)(std::ios_base&) A pointer to a function that takes a std::ios_base& as an arugment and returns a std::ios_base&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::iostream as arguments, any manipulator can be written to the Fed9UStream objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UStream& operator<<(std::ios_base& (&pf)(std::ios_base&));

    /**
     * \brief  This overload of the operator<< allows a Fed9UMessage<T> to be set for a Fed9UStream object.
     * \param  level The level that is to be written.
     * \return const Fed9UMessage<T>& A reference to the level that was written.
     *
     * The template parameter T should be an enum that defines the three different types of log that are available for the Fed9U
     * project. Fed9UDebugLevel, Fed9UErrorLevel or Fed9ULogLevel defined in the file TypeDefs.hh are the three that can be used.
     * When the Fed9UMessage<T> is sent to the stream it is not written to the stream, rather it is returned and all subsequent
     * writes will be to the Fed9UDebugLevel object. An overloaded operator is provided which writes the object
     * writes to the appropriate stream will be handled by the Fed9UMessage<T> class and its overloaded operator<<. Only those
     * level that exceed the environmental level will be written. Thus control is provided over what detail of message is provided.
     */
    template<typename T>
    Fed9UMessage<T> operator<<(const Fed9UMessage<T>& level) {
      //std::cout << FED9U_FUNCTION << std::endl;
      return level;
    }//operator<<(Fed9UMessage<T>& level)

    /**
     * \brief  An overload of the operator<< which allows any data type that can be written to an ostream to be written to any instantiation of the
     *         template class Fed9ULogTemplate.
     * \param  data Templated data type that is to be written to the stream. The data type specified by U must have an overload version of the
     *         operator<< for an std::ostream object for it to be written to the log.
     * \return Fed9UStream& Self reference.
     */
    template<typename U>
    Fed9UStream& operator<<(const U& data) {
      //std::cout << FED9U_FUNCTION << std::endl;
      //If have successfully initialised our mutex then we can ask it for a lock to ensure
      //that will behave in a nice thread safe manor, otherwise will just have to send it straight to the ostream.
      if ( mMutexIsInitialised ) {
	//Request a lock on the mutex...
	mErrorLocking = pthread_mutex_lock(&mWriteMutex);
	if (0 == mErrorLocking) {
	  //We were successful in getting the mutex.
	  //This bit of the code actually does the write.
	  //We first cast the template object back to its base class std::ostream and then write the data to the stream.
	  //This allows any data type that has an overloaded operator<< for writing to std::ostreams.
	  *(dynamic_cast<std::ostream*>(this)) << data;

	  //Now we are finished with our writing we can unlock the mutex...
	  mErrorUnlocking = pthread_mutex_unlock(&mWriteMutex);
	  if (0 != mErrorUnlocking) {
	    //...we can't unlock the mutex, the program will probably hang but there is little that can be done
	    //about it. Write the unlock error to the stream and hope we fix the bug that caused it.
	    getUnlockErrorNumber();
	  }
	} else {
	  //The lock failed for some reason. We are now basically screwed and thread safety is out of
	  //the window. Write the error to file and hope that it is not too garbled to read,
	  //also that it was the error number from the calling thread.
	  getLockErrorNumber();
 	}
      } else {
	//Don't have a mutex to use, so will have to work in a none thread safe manor.
	//Hence just write the data straight to the stream.
	*(dynamic_cast<std::ostream*>(this)) << data;
      }

      //...we are now done, so return a reference to the input stream thus allowing stream commands to be chained together.
      return *this;
    }//operator<<(U)


  private:

    Fed9ULog* mpFileLog;  //!< If a file log is required then this will point to the file object.
    pthread_mutexattr_t mWriteMutexAttr; //!< These are the attributes of the mutex used. Controls 
    pthread_mutex_t mWriteMutex;         //!< It is this mutex that must be aquired before writes to the buffer can be performed.
    bool mMutexIsInitialised;            //!< If the mutex fails to get initialised and is in an unusable state, then this data member tells the class about it.
    u32 mErrorLocking;   //!< Stores the last error number to be recorded when a lock was attempted. Should be zero and hence error free.
    u32 mErrorUnlocking; //!< Stores the last error number to be recorded when an unlock was attempted. Should be zero and hence error free.

  };//class Fed9UStream


  /**
   * \brief This is a global Fed9UStream object that is used to write debug messages during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9UOut;

  /**
   * \brief This is a global Fed9UStream object that is used to write error messages during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9UErr;

  /**
   * \brief This is a global Fed9UStream object that is used to log messages to file during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9ULog;


  /**
   * \brief  Used to set the level of a message that is to be written to one of the global streams provided by the Fed9U library.
   * \author Gareth Rogers, Jonathan Fulcher
   * \date   2006/09/07
   *
   * The template parameter T should refer to on of the message level enums (Fed9UDebugLevel, Fed9UErrorLevel or Fed9ULogLevel) and the
   * behaviour of the class is undefined if it is constructed with any other type. The class supports the streaming of any data type
   * that has an overloaded operator<< for use with a std::ostream, plus all the manipulators for use with std::ostreams.
   * The message level written will persit until either a new level is written or a semi colon, ending the line of code, is encountered.
   * The message level can only be changed within the same enum level. You could not for example write one message with a Fed9UDebugLevel
   * and then another at a Fed9UErrorLevel within the same line of code.
   */
  template<typename T>
  class Fed9UMessage {

  public:

    /**
     * \brief Constructor
     * \param level The message level that should be set for any subsequent write to the stream. The level will persit in the stream until
     *        either a new level is written or a semi colon ends the line of code.
     *
     * During construction the class also identifies which stream (gFed9UOut, gFed9UErr or gFed9ULog) and which global message level (Fed9U::Fed9UMessage<Fed9UDebugLevel>::smLevel,
     * Fed9U::Fed9UMessage<Fed9UErrorLevel>::smLevel or Fed9U::Fed9UMessage<Fed9ULogLevel>::smLevel) should be used when deciding whether the message is of sufficient level to be written to the appropriate stream.
     */
    Fed9UMessage(const T& level)
      : mLevel(level), mGlobalLevel(NULL), mGlobalStream(NULL) {

      if ( typeid(T) == typeid(Fed9UDebugLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9UDebugLevel>::smLevel);
	mGlobalStream = &gFed9UOut;
      } else if ( typeid(T) == typeid(Fed9UErrorLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9UErrorLevel>::smLevel);
	mGlobalStream = &gFed9UErr;
      } else if ( typeid(T) == typeid(Fed9ULogLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9ULogLevel>::smLevel);
	mGlobalStream = &gFed9ULog;
        // <NAC date="16/05/2007"> stamp log
        mGlobalStream->stamp();
        // </NAC>
      } else {
	//Do nothing everything remains null.
      }

    }//Fed9UMessage(const T& level)

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UMessage, such as std::endl;
     * \param  std::ostream& (*pf)(std::ostream&) A pointer to a function that takes a std::ostream& as an arugment and returns a std::ostream&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::ios_base as arguments, any manipulator can be written to the Fed9UMessage objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UMessage<T> operator<<(std::ostream& (*pf)(std::ostream&)) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << pf;

      return *this;
    }//operator<<(std::ostream& (*pf)(std::ostream&))

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UMessage, such as std::endl;
     * \param  std::ios_base& (*pf)(std::ios_base&) A pointer to a function that takes a std::ios_base& as an arugment and returns a std::ios_base&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::iostream as arguments, any manipulator can be written to the Fed9UMessage objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UMessage<T> operator<<(std::ios_base& (&pf)(std::ios_base&)) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << pf;

      return *this;
    }//operator<<(std::ios_base& (*pf)(std::ios_base&))

    /**
     * \brief  This allows a new message level to be set for subsequent writes to the stream.
     * \param  newLevel The new message level to be written at.
     * \return Self reference.
     *
     * Message levels can only be changed within the same message level type (i.e. FED9U_DEBUG_LEVEL_DETAILED to FED9U_DEBUG_LEVEL_INTERMEDIATE,
     * but not FED9U_DEBUG_LEVEL_DETAILED to FED9U_ERROR_LEVEL_CRITICAL). The new level persists for the same length of time the old level
     * would have persisted for. Until a new level is written or a semi colon indicates the end of the line of code.
     */
    Fed9UMessage<T> operator<<(const Fed9UMessage<T>& newLevel) {
      //std::cout << FED9U_FUNCTION << std::endl;
      return newLevel;
    }//operator<<(const Fed9UMessage<T>& newLevel)

    /**
     * \brief  This will write any type that can be written to a std::ostream to the appropriate stream, provided the level is greater
     *         than or equal to the environmental level.
     * \param  data Having a templated type U, it can be any object or built in type. Provided it can be written to an std::ostream.
     * \return Self reference.
     *
     * By returning a self reference it ensures that chained stream calls always are written to the message level.
     */
    template<typename U>
    Fed9UMessage<T> operator<<(const U& data) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << data;

      return *this;
    }//operator<<(const U& data)
    
    //template<typename U>
    //friend const Fed9UMessage<Fed9UDebugLevel>& operator<<(const Fed9UMessage<Fed9UDebugLevel>& level, U data);

  /**
   * \brief A static variable to define the level of debug printout that is sent to the debug stream.
   *
   * This is initialised to the FED9U_DEBUG_LEVEL_MINIMAL upon program start up (in Fed9ULogTemplate.cc), which means no messages will be sent to the debug
   * stream. It can be set to any level at run time, which will immediately cause debug messages of that level be sent to the
   * debug stream.
   */
  static T smLevel;

  /**
   * \brief A static variable to define the level of error printout that is sent to the error stream.
   *
   * This is initialised to the FED9U_ERROR_LEVEL_CRITICAL upon program start up (in Fed9ULogTemplate.cc), which means only erorr messages defined as critical
   * will be sent to the debug stream. It can be set to any level at run time, which will immediately cause debug messages of that
   * level be sent to the error stream.
   */
  //static Fed9UErrorLevel sgErrorLevel;

  /**
   * \brief A static variable the defines what level of log messages are to be sent to the Fed9U file log.
   *
   * This is initialised to FED9U_LOG_LEVEL_NONE upon program start up (in Fed9ULogTemplate.cc), which means no log file is created until the
   * log level is set to a higher value and the first message is sent to the log. It can be changed to any level at
   * run time, which will cause the log messages of that level to be immediately written to the stream.
   */
  //static Fed9ULogLevel sgLogLevel;


  private:

    const T& mLevel;
    const T* mGlobalLevel;
    Fed9UStream* mGlobalStream;

  };//class Fed9ULogLevelTemplate

}//namespace Fed9U

#endif//_Fed9ULogTemplate_H_
#ifndef H_Fed9USpyEventException
#define H_Fed9USpyEventException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9USpyEventException class to be used when throwing errors from the Fed9USpyEvent class.
 */

#define FED9U_SPY_EVENT_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_EVENT_BUFFER_FULL, "ERROR: The Buffer is full. No more data can be added to the event object.")  \
  IC_DEF_ERROR(ERROR_DELAY_CHIP_DATA_NOT_FOUND, "ERROR: The data for this delay chip was not found in the event object.")  \
  IC_DEF_ERROR(ERROR_INDEX_OUT_OF_RANGE, "ERROR: The sample index is out of range. ") \
  IC_DEF_ERROR(ERROR_PIPELINE_ADDRESS_MISMATCH, "ERROR: The pipeline addresses for data in event do not match. ") \
  IC_DEF_ERROR(ERROR_NO_TICK_MARK, "ERROR: The channel does not contain a tick mark. ")


  IC_EXCEPTION_CLASS_BEGIN(Fed9USpyEventException, FED9U_SPY_EVENT_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9USpyEventException

#ifndef H_Fed9USpyEvent
#define H_Fed9USpyEvent
#include <vector>
#include <iostream>
#include <fstream>
#include "ICAssert.hh"
namespace Fed9U {
  
  /**
   * \brief  Class to hold and access data from the spy channel.
   */
  class Fed9USpyEvent {
  public:
    /**
     * \brief Construct Fed9USpyEvent by giving a value for thresholds.
     *        The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent with a description to take the thresholds
     *        from. The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent by giving a buffer and a description to 
     *        take the thresholds from. The buffer is not deallocated by 
     *        Fed9USpyEvent.
     * \param bufferPointer Pointer to buffer large enough for data.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(u32* bufferPointer, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from an input stream and a description to
     *        take the thresholds from. The buffer is allocated and 
     *        deallocated by Fed9USpyEvent.
     * \param theStream Pointer to input stream to read from.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(std::istream* theStream, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from the file name of a file written to 
     *        with write and a description to take the thresholds from. The 
     *        buffer is allocated and deallocated by Fed9USpyEvent.
     * \param fileName Name of the file to read from.
     * \param theFedDescription Description containing thresholds.
     */
    Fed9USpyEvent(const char*const filename, const Fed9UDescription& theFedDescription);
    /**
     * \brief Construct Fed9USpyEvent from an input stream. The buffer is 
     *        allocated and deallocated by Fed9USpyEvent.
     * \param theStream Pointer to input stream to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(std::istream* theStream, u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent from the file name of a file written to.
     *        The buffer is allocated and deallocated by Fed9USpyEvent.
     * \param fileName Name of the file to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(const char*const filename, u16 thresholdValue=500);
    /**
     * \brief Construct Fed9USpyEvent from a buffer dumped with writeToBuffer.
     *        The buffer is not allocated or deallocated by Fed9USpyEvent.
     * \param readBuffer Buffer to read from.
     * \param thresholdValue Value to use for all channel thresholds.
     */
    Fed9USpyEvent(u32* readBuffer, u16 thresholdValue=500);
    /**
     * \brief Read SpyEvernt from a binary buffer dumped with writeToBuffer
     * \param readBuffer Buffer to read from.
     */
    void readFromBuffer(u32* inBuffer);
    /**
     * \brief Deallocates the buffer if it is managed by Fed9USpyEvent.
     */
    ~Fed9USpyEvent();
    /**
     * \brief Read SpyEvent from stream from write.
     * \param theStream Input stream to read from.
     */
    void readFromStream(std::istream* theStream);
    /**
     * \brief  Get a new buffer to be passed to Fed9uVmeDevice::fireSpy. 
     * \param  address Address of delay chip which captured this data.
     * \return Pointer to the begining of a buffer large enough for 
     *         one delay chip's spy channel data including offset.
     */
    u32* delayChipBuffer(Fed9UAddress address) const;
    /**
     * \brief  Get the value of a sample.
     * \param  channelAddress The address of the channel from which the sample 
     *         comes.
     * \param  sample The index of the sample.
     * \return unsigned short int The value of the sample.
     */
    u16 getSample(Fed9UAddress channelAddress, u16 sample) const;
    /**
     * \brief  Get the value of a payload sample.
     * \param  channelAddress The address of the channel from which the sample 
     *         comes.
     * \param  sample The index of the sample in the payload.
     * \return unsigned short int The value of the sample.
     */
    u16 getPayloadSample(Fed9UAddress channelAddress, u16 sample) const;
    /**
     * \brief  Returns the number of samples per channel
     * \return unsigned short int Number of samples per channel.
     */
    static u16 samplesPerChannel() { return static_cast<u16>(spyDelayChipBufferSize*32.0/10.0)/CHANNELS_PER_DELAY_CHIP; }
    /**
     * \brief  Returns the size of a full spy event buffer in bytes
     * \return size of full spy event buffer.
     */
    static size_t fullEventSize() { return spyDelayChipBufferSize*4*(CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP); }
    /**
     * \brief  Get the pipeline address of the APV frame in then event.
     * \return unsigned char The pipeline address.
     */
    u8 getPipelineAddress() const;
    /**
     * \brief  Get the pipeline address from a single APV in a single channel.
     * \param  theAPV The Address of the APV.
     * \return unsigned char The APV pipeline address.
     */
    u8 getAPVPipelineAddress(Fed9UAddress apvAddress) const;
    /**
     * \brief  Get the number of samples before the begining of the tick mark.
     * \param  channelAddress The address of the channel.
     * \return unsigned char The offset (in data words) of the begining of the 
     *         tick mark.
     */
    u8 getChannelTickOffset(Fed9UAddress channelAddress) const;
    /**
     * \brief Write SpyEvent to output stream.
     * \param theStream Pointer to output stream to write to.
     */
    void writeToStream(std::ostream* theStream) const;
    /**
     * \brief  Get required size of buffer to write to with writeToBuffer.
     * \return unsigned long int buffer size in bytes
     */
    u32 getDumpBufferSize() const;
    /**
     * \brief Write SpyEvent to a buffer.
     * \param buffer Buffer to write to.
     */
    void writeToBuffer(u32* buffer) const;
    /**
     * \brief Write SpyEvent to file.
     * \param filename Name of file to write to.
     */
    void writeToFile(const char*const filename) const;
    /**
     * \brief Print SpyEvent summary to output stream.
     * \param os Output stream to print to.
     */
    void print(std::ostream& os) const;
    /**
     * \brief  Get the error bit from a single APV in a single channel.
     * \param  theAPV The Address of the APV.
     * \return bool The APV error bit.
     */
    bool getAPVErrorBit(Fed9UAddress apvAddress) const;
    /**
     * \brief Set the thresholds for bits in APV headers.
     * \param theFedDescription Description containing thresholds.
     */
    void setThresholds(const Fed9UDescription& theFedDescription);
    /**
     * \brief Set the thresholds for bits in APV headers for all channels 
     *        at once
     * \param thresholdValue Value for threshold.
     */
    void setAllThresholds(u16 thresholdValue);
    /**
     * \brief  Check if the tick marke could be found.
     * \param  channelAddress The address o0f the channel.
     * \return bool Was it found.
     */
    bool tickFound(Fed9UAddress channelAddress) const;
    void setSample(u8 delayChip, u8 channel, u16 sampleNumber, u16 value);
  private:
    /**
     * \brief  Get the value of a sample.
     * \param  delayChip The delay chip.
     * \param  channel The channel in the delay chip from which the sample comes.
     * \param  sample The index of the sample.
     * \return unsigned short int The value of the sample.
     */
    u16 getSample(u8 delayChip, u8 channel, u16 sample) const;
    /**
     * \brief  Get a pointer to the buffer holding delay chip data.
     * \param  delayChip The delay chip.
     * \return unsigned long int pointer Pointer to the begining of the buffer 
     *         holding a data.
     */
    u32* delayChipBuffer(u8 delayChip) const;
    /**
     * \brief  Get the pipeline address from a single APV in a single channel/
     * \param  delayChip The delay chip buffer number containing the APV's 
     *         channel in the buffer.
     * \param  channel Channel in delay chip. 
     * \param  apv The apv in the channel.
     * \return unsigned char The APV pipeline address.
     */
    u8 getAPVPipelineAddress(u8 delayChip, u8 channel, u8 apv) const;
    /**
     * \brief  Get the error bit from a single APV in a single channel.
     * \param  delayChip The delay chip containing the APV's 
     *         channel in the buffer.
     * \param  channel Channel in delay chip. 
     * \param  apv The apv in the channel.
     * \return bool The APV error bit.
     */
    bool getAPVErrorBit(u8 delayChip, u8 channel, u8 apv) const;
    /**
     * \brief  Get the number of samples before the begining of the tick mark.
     * \param  delayChip The delay chip number.
     * \param  channel The number of the channel in the delay chip.
     * \return unsigned char The offset (in data words) of the begining of the 
     *         tick mark.
     */
    u8 getChannelTickOffset(u8 delayChip, u8 channel) const;
    /**
     * \brief  Check if the tick marke could be found.
     * \param  delayChip The delay chip number.
     * \param  channel The number of the channel in the delay chip.
     * \return bool Was it found.
     */
    bool tickFound(u8 delayChip, u8 channel) const;
    /**
     * \brief Unimplemented constructors to prevent copying.
     */
    Fed9USpyEvent(const Fed9USpyEvent&);
    Fed9USpyEvent& operator = (const Fed9USpyEvent&);
    
    /**
     * \brief  Class used to signal internal errors.
     */
    struct Fed9USpyEventInternalException {};
    
    u32* buffer;		//!< Buffer which holds data.
    bool manageBuffer;		//!< Bool controlling if buffer should be deallocated.
    std::vector<u16> channelThresholds;	//<! Thresholds for header information (position in vector is internal channel number).
  public:  
    static const u16 spyDelayChipBufferSize = 376; //!< Number of u32s in one delay chip's data.
  protected:
    static const u16 offsetOfSpyData = 44; //!< Offset in bits of first bit of spy data in buffer from FED
    
  };
  
  inline std::ostream& operator<<(std::ostream& os, const Fed9USpyEvent & sev) { sev.print(os); return os; }
  
}
#endif // H_Fed9USpyEvent
#ifndef H_Fed9USpyApvDataException
#define H_Fed9USpyApvDataException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9USpyApvDataException class to be used when throwing errors from the Fed9USpyApvData class.
 */

#define FED9U_SPY_APV_DATA_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_OUT_OF_RANGE, "ERROR: The sample is out of range") \
  IC_DEF_ERROR(ERROR_TICK_MARK_NOT_FOUND, "ERROR: The tick mark was not found") \
  IC_DEF_ERROR(ERROR_APV_HEADER, "ERROR: The information could not be extracted from the APV header")


  IC_EXCEPTION_CLASS_BEGIN(Fed9USpyApvDataException, FED9U_SPY_APV_DATA_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9USpyApvDataException

#ifndef H_Fed9USpyApvData
#define H_Fed9USpyApvData

namespace Fed9U {
  
  /**
   * \brief Class to hold the data for one APV from the spy channel 
   *        to be used to transfer between xdaq applications.
   */
  class Fed9USpyApvData {
  public:
    
    /**
     * \brief  Default constructor which sets apvNumber to an invalid number.
     */
    Fed9USpyApvData() {}
    /*
     * \brief  Constructor
     * \param  fedId FedId of the FED from which the spy data will come.
     * \param  fedApvNumber Internal FedApv number of the APV from which the spy 
     *                      data will come.
     */
    Fed9USpyApvData(u16 fedId, u8 fedApvNumber) : fedId_(fedId), apv_(fedApvNumber) {}
    /**
     * \brief  Constructor using address for FedApv number
     * \param  fedId FedId of the FED from which the spy data will come.
     * \param  apvAddress Address of APV from which the spy data will come. 
     */
    Fed9USpyApvData(u16 fedId, Fed9UAddress apvAddress) : fedId_(fedId), apv_(apvAddress.getFedApv()) {}
    
    //geters and seters for member variables
    /**
     * \brief  Get FedId of the FED from which the spy data comes.
     */
    u16 getFedId() const { return fedId_; }
    /**
     * \brief  Set FedId of the FED from which the spy data comes.
     * \param  fedId New FedId.
     * \return Self reference.
     */
    Fed9USpyApvData& setFedId(u16 fedId) { fedId_ = fedId; return *this; }
    /**
     * \brief  Get the internal FedApv number of the APV from which the spy 
     *         data comes.
     */
    u8 getApv() const { return apv_; }
    /**
     * \brief  Set the internal FedApv number of the APV from which the spy data
     *         comes.
     * \param  fedApvNumber New internal FedApv number.
     * \return Self reference.
     */
    Fed9USpyApvData& setApv(u8 fedApvNumber) { apv_ = fedApvNumber; return *this; }
    /**
     * \brief  Set the APV from which the spy data comes.
     * \param  apvAddress Address of the APV on the FED.
     * \return Self reference.
     */
    Fed9USpyApvData& setApv(Fed9UAddress apvAddress) { apv_ = apvAddress.getFedApv(); return *this; }
    /**
     * \brief  Get the address of the APV from which the spy data comes.
     * \return Fed9UAddress the APV's address.
     */
    Fed9UAddress getApvAddress() { return Fed9UAddress().setFedApv(apv_); }
    /**
     * \brief  Get a sample from the spy data.
     * \param  index Index of sample in spy data (not apv data frame).
     * \throw  Fed9USpyApvDataException index is out of range.
     * \return The value of the sample in ADC counts. 
     */
    u16 getSample(u16 index) const throw (Fed9USpyApvDataException);
    /**
     * \brief  Set a sample from the spy data.
     * \param  index Index of sample in spy data (not apv data frame).
     * \throw  Fed9USpyApvDataException index is out of range.
     * \return The value of the sample in ADC counts.
     */
    Fed9USpyApvData& setSample(u16 index, i16 value) throw (Fed9USpyApvDataException);
    
    //member functions to extract values from the APV header
    /**
     * \brief  Get the number of samples before the start of the tick mark.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return Number of samples before the start of the tick mark.
     */
    u8 getTickOffset(i16 threshold) const throw (Fed9USpyApvDataException);
    /*
     * \brief  Get the pipeline address APV header.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return Pipeline address from the APV header.
     */
    u8 getPipelineAddress(i16 threshold) const throw (Fed9USpyApvDataException);
    /**
     * \brief  Get APV header error bit.
     * \param  threshold The threshold for a high value in the APV header.
     * \throw  Fed9USpyApvDataException Tick mark isn't found.
     * \return True if the error bit was high (ie no error) or 
     *         false if it was low (ie there was an error).
     */
    bool getErrorBit(i16 threshold) const throw (Fed9USpyApvDataException);
    
    
    /**
     * \brief  Get a sample from the spy data without range checking. 
     * \param  index Index of sample in spy data (not apv data frame).
     * \return The value of the sample in ADC counts. 
     */
    u16 operator [] (u16 index) const { return samples_[index]; }
    
    static const int SAMPLES_PER_APV = 150;	//!< Number of samples for each APV.
    
  private:

    u16 fedId_;		//!< FedId of the FED from which the spy data comes.
    u8 apv_;	//!< Internal FedApv number of the APV from which the spy data comes.
    i16 samples_[SAMPLES_PER_APV];	//!< Buffer to hold the samples. 
  };
  
} //namespace Fed9U

#endif //ifndef H_Fed9USpyApvData
#ifndef H_Fed9UDescriptionException
#define H_Fed9UDescriptionException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9UDescriptionException class to be used when throwing errors from the Fed9USpyEvent class.
 */

#define FED9U_DESCRIPTION_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_NO_ADDRESS_TABLE, "ERROR: The Address table was not found.")  \


  IC_EXCEPTION_CLASS_BEGIN(Fed9UDescriptionException, FED9U_DESCRIPTION_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9UDescriptionException

#ifndef H_Fed9UCrc
#define H_Fed9UCrc


namespace Fed9U{
  //calculate the CRC for a FED buffer ignoring the CRC bytes. Pass true for the final argument if the 32bit word order is swapped (ie VME order)
  u16 calculateFEDBufferCRC(const u8* buffer, const size_t length, bool wordSwap);
}

#endif //ndef H_Fed9UCrc
