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
