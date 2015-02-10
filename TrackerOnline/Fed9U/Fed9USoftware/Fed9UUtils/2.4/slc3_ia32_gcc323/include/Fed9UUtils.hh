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

#define FED9U_FUNCTION __PRETTY_FUNCTION__
#define FED9U_DEBUG_BREAK __asm("int3")

#ifdef DAQ_VERSION_2
#define FED9U_INFO(x)     LOG4CPLUS_INFO(logger_, (x) )
#define FED9U_ERROR(x,y)  LOG4CPLUS_ERROR(logger_,(x + y))
#else
#define FED9U_INFO(x)  XDAQ_NOTE((x))
#define FED9U_ERROR(x,y) ims_->createErrorMessage(TFP_NONEXT_ERROR,                             \
						  time(0),                                      \
						  SOFTWAREERRORCODE,                            \
						  FEDSUPERVISORSYSTEM,                          \
						  TRACKERFEDPMCSYSTEM,                          \
						  x,                                            \
						  y);                                           \
                         ims_->emit();                                                          \
                         XDAQ_RAISE(xdaqException,y)                                    
#endif

#include <iosfwd>
#include <iostream>
#include <string>



#define FED9U_EPROM_VERSION_1 1 //the first version of eprom data had only the fed serial number at offset 516
#define FED9U_EPROM_VERSION_2 2 // second version of eprom data has test results and serial number at offset 1536
#define FED9U_EPROM_INDUSTRY_RESULTS_OFFSET 1536
#define FED9U_EPROM_V1_SERIAL_NUMBER_OFFSET 516



// This software project is only compatible with firmware versions equal to or above the following
#define FED9U_FE_FIRMWARE_MIN_VER 0X10000000
#define FED9U_BE_FIRMWARE_MIN_VER 0X10000000
#define FED9U_VME_FIRMWARE_MIN_VER 0X10000000
#define FED9U_DELAY_FIRMWARE_MIN_VER 0X10000000


namespace Fed9U {





  std::string getFed9UUtilsVersion();
  unsigned    getFed9UUtilsVersionMajor();
  unsigned    getFed9UUtilsVersionMinor();
  
  enum {
    FED_STRIP_HIT_THRESHOLD_HIGH = 5,
    FED_STRIP_HIT_THRESHOLD_LOW = 2,

    FEUNITS_PER_FED = 8,
    CHANNELS_PER_FEUNIT = 12,
    APVS_PER_CHANNEL = 2,
    STRIPS_PER_APV = 128,

    CHANNELS_PER_DELAY_CHIP = 4,

    CHANNELS_PER_FED = CHANNELS_PER_FEUNIT * FEUNITS_PER_FED,
    APVS_PER_FEUNIT = APVS_PER_CHANNEL * CHANNELS_PER_FEUNIT,
    APVS_PER_FED = APVS_PER_FEUNIT * FEUNITS_PER_FED,
    STRIPS_PER_CHANNEL = STRIPS_PER_APV * APVS_PER_CHANNEL,
    STRIPS_PER_FED = STRIPS_PER_CHANNEL * CHANNELS_PER_FED
  };

  enum Fed9UEventDataMode { FED9U_EVENT_MODE_REAL, FED9U_EVENT_MODE_FAKE_FULL, FED9U_EVENT_MODE_FAKE_SINGLE };

  enum Fed9UHalBusAdaptor     { FED9U_HAL_BUS_ADAPTOR_SBS, FED9U_HAL_BUS_ADAPTOR_CAEN_PCI,
				FED9U_HAL_BUS_ADAPTOR_CAEN_USB, FED9U_HAL_BUS_ADAPTOR_VXI, FED9U_HAL_BUS_ADAPTOR_DUMMY};

  enum Fed9UDaqMode     { FED9U_MODE_PROCESSED_RAW, FED9U_MODE_ZERO_SUPPRESSED,
		          FED9U_MODE_VIRGIN_RAW,    FED9U_MODE_SCOPE };

  enum Fed9UDaqSuperMode     { FED9U_SUPER_MODE_FAKE, FED9U_SUPER_MODE_ZERO_LITE, FED9U_SUPER_MODE_FAKE_ZERO_LITE, FED9U_SUPER_MODE_NORMAL };

  enum Fed9UTrigSource  { FED9U_TRIG_NONE          = 0, FED9U_TRIG_TTC             = 1,
			  FED9U_TRIG_BACKPLANE     = 2, FED9U_TRIG_SOFTWARE        = 4 };

  enum Fed9UClockSource { FED9U_CLOCK_INTERNAL     = 1, FED9U_CLOCK_TTC            = 2,
			  FED9U_CLOCK_BACKPLANE    = 4 };

  enum Fed9UReadRoute   { FED9U_ROUTE_NONE         = 0, FED9U_ROUTE_SLINK64        = 1,
			  FED9U_ROUTE_VME          = 2 };

  enum Fed9UHeaderFormat {FED9U_HEADER_FULLDEBUG = 1, FED9U_HEADER_APVERROR = 2};
  /**
   * This enum is used to define how much of the EPROM memory is read only.
   *
   * FED9U_EPROM_NONE   - none of the memory is protected.
   * FED9U_EPROM_QUATER - the upper quater (1536 - 2046) is protected.
   * FED9U_EPROM_HALF   - the upper half (1024 - 2046) is protected.
   * FED9U_EPROM_ALL    - all the memory is write protected.
   */
  enum Fed9UEpromReadOnlyLevel { FED9U_EPROM_NONE, FED9U_EPROM_QUATER, FED9U_EPROM_HALF, FED9U_EPROM_ALL };
  //This number sets the byte at which the level of the EPROM write protected area begins.
  enum Fed9UEpromArea {EPROM_WRITE_PROTECTED = 1536};
  

  /**
   * This enum is used to flag the set bits in the status register. It is also a mask to be
   * used to read back the set bits
   */
  enum Fed9UVoltageStatus { FED9U_VOLT_STATUS_2POINT5 = 0x001, FED9U_VOLT_STATUS_3POINT3 = 0x002,
			    FED9U_VOLT_STATUS_5       = 0x004, FED9U_VOLT_STATUS_12      = 0x008,
			    FED9U_VOLT_STATUS_CORE    = 0x010, FED9U_VOLT_STATUS_SUPPLY  = 0x020,
			    FED9U_VOLT_STATUS_INTERNAL_TEMP    = 0x040, FED9U_VOLT_STATUS_EXTERNAL_TEMP = 0x080,
			    FED9U_VOLT_STATUS_NO_EXTERNAL_TEMP = 0x100 };

  /**
   * This enum is used to make the returned u32 from the status register in the Fed9UTempControlInfo
   * class in order to check the status bits on the temperature limits.
   */
  enum Fed9UTemperatureStatus { FED9U_TEMP_STATUS_LM82_HIGH = 0x01, FED9U_TEMP_STATUS_FPGA_HIGH = 0x02,
				FED9U_TEMP_STATUS_LM82_CRIT = 0x04, FED9U_TEMP_STATUS_FPGA_CRIT = 0x08,
				FED9U_TEMP_STATUS_FPGA_DISCONNETED = 0x10 };

  /**
   * These enum masks values can be used to enable the bunch and event counters.
   */
  enum Fed9UTtcrxCntrOperation { FED9U_TTCRX_ENABLE_BUNCH_CTR = 0x01, FED9U_TTCRX_ENABLE_EVENT_CTR = 0x02 };

  /**
   * This is a read only configuration register. The settings here control the behaviour of the TTCrx.
   */
  enum Fed9UTtcrxConfig { FED9U_TTCRX_CONFIG_SEL_TEST_PD = 0x01, FED9U_TTCRX_CONFIG_SEL_INPUT_A = 0x02,
			  FED9U_TTCRX_CONFIG_PLL_RESET   = 0x04, FED9U_TTCRX_CONFIG_DLL_RESET   = 0x08,
			  FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR = 0x10,  FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG = 0x20,
			  FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION = 0x40, FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS = 0x80 };

  /**
   * This enum is used to check the bits of the TTCrx status register.
   */
  enum Fed9UTtcrxStatus { FED9U_TTCRX_STATUS_AUTO_RESET = 0x1, FED9U_TTCRX_STATUS_FRAME_SYNCH = 0x2,
			  FED9U_TTCRX_STATUS_DLL_READY  = 0x4, FED9U_TTCRX_STATUS_PLL_READY   = 0x8 };

//   enum Fed9UTempMonitor  { FED9U_TEMP_LM82_TEMP, FED9U_TEMP_FPGA_TEMP, FED9U_TEMP_LM82_HIGH,
//                            FED9U_TEMP_FPGA_HIGH, FED9U_TEMP_CRITICAL,  FED9U_TEMP_READ_ID };
//   /* This enum has been replaced with the one above Jan20 2004
//   enum Fed9UTempMonitor  { FED9U_TEMP_WRITE_LM82_HIGH = 0x0b, FED9U_TEMP_WRITE_FPGA_HIGH = 0x0d,
//                            FED9U_TEMP_WRITE_CRIT_TEMP = 0x5a,
//                            FED9U_TEMP_READ_LM82_TEMP  = 0x00, FED9U_TEMP_READ_FPGA_TEMP  = 0x01,
// 			   FED9U_TEMP_READ_LM82_HIGH  = 0x05, FED9U_TEMP_READ_FPGA_HIGH  = 0x07,
//                            FED9U_TEMP_READ_CRIT_TEMP  = 0x42, FED9U_TEMP_READ_ID         = 0xfe };

//    This enum has been replace with the one above on 29th october 2003
//      enum Fed9UTempMonitor  { FED9U_TEMP_WRITE_CONFIG  = 0x09, FED9U_TEMP_WRITE_LHIGH   = 0x0b,
//                            FED9U_TEMP_WRITE_RHIGH   = 0x0d, FED9U_TEMP_WRITE_CRIT    = 0x5a,
//                            FED9U_TEMP_READ_LTEMP    = 0x00, FED9U_TEMP_READ_RTEMP    = 0x01,
// 			   FED9U_TEMP_READ_STATUS   = 0x02, FED9U_TEMP_READ_CONFIG   = 0x03,
// 			   FED9U_TEMP_READ_LHIGH    = 0x05, FED9U_TEMP_READ_RHIGH    = 0x07,
//                            FED9U_TEMP_READ_CRIT     = 0x42, FED9U_TEMP_READ_ID       = 0xfe };
//   */

// enum Fed9UTtcControl { FED9U_TTC_ERROR, FED9U_TTC_STATUS, FED9U_TTC_BUNCH, FED9U_TTC_EVENT };

#define MAKE_ENUM_STREAMABLE(name) \
inline std::ostream& operator<<(std::ostream& os, name   value) { return os << static_cast<long>(value); } \
inline std::istream& operator>>(std::istream& is, name & value) { long t; is >> t; value = static_cast<name>(t); return is; }
  MAKE_ENUM_STREAMABLE(Fed9UHalBusAdaptor);
  MAKE_ENUM_STREAMABLE(Fed9UDaqMode);
  MAKE_ENUM_STREAMABLE(Fed9UDaqSuperMode);
  MAKE_ENUM_STREAMABLE(Fed9UTrigSource);
  MAKE_ENUM_STREAMABLE(Fed9UClockSource);
  MAKE_ENUM_STREAMABLE(Fed9UReadRoute);
  MAKE_ENUM_STREAMABLE(Fed9UEpromReadOnlyLevel);
  MAKE_ENUM_STREAMABLE(Fed9UVoltageStatus);
  MAKE_ENUM_STREAMABLE(Fed9UTemperatureStatus);
  MAKE_ENUM_STREAMABLE(Fed9UTtcrxCntrOperation);
  MAKE_ENUM_STREAMABLE(Fed9UTtcrxStatus);
  MAKE_ENUM_STREAMABLE(Fed9UHeaderFormat);
//   MAKE_ENUM_STREAMABLE(Fed9UTempMonitor)

  typedef signed char  i8;  ///< Signed 8-bit integer.
  typedef signed short i16; ///< Signed 16-bit integer.
  typedef signed long  i32; ///< Signed 32-bit integer.

  typedef unsigned char  u8;  ///< Unsigned 8-bit integer.
  typedef unsigned short u16; ///< Unsigned 16-bit integer.
  typedef unsigned long  u32; ///< Unsigned 32-bit integer.

#define STRINGIZE_2(x) #x
#define STRINGIZE(x) STRINGIZE_2(x)

#define NO_DELAY_SKEWS 32

}

#endif // H_TypeDefs
#ifndef H_Fed9UFileHelpers
#define H_Fed9UFileHelpers

#include <string>

namespace Fed9U {

  int Fed9UFile_open(const std::string & filename, bool & newfile);
  void Fed9UFile_close(int fd);
  std::string Fed9UFile_read(int fd);
  void Fed9UFile_overwrite(int fd, const std::string & data);

}

#endif // H_Fed9UFileHelpers
#ifndef H_Fed9UWait
#define H_Fed9UWait

namespace Fed9U {

  void fed9Uwait(unsigned long microsec = 0);
  void fed9Uwait(unsigned long seconds, unsigned long microsec);
  
}

#endif // H_Fed9UWait
#ifndef H_Fed9UChannelInterface
#define H_Fed9UChannelInterface


#define FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT false
#define FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT 1

namespace Fed9U {

  class Fed9UAddress;
class Fed9UEvent;

  /**
   * FED Channel Interface class.
   *
   * This class simply provides an abstracted interface for the user to set the desired channel.
   * It enables the user to count in 0 or 1 based and from top to bottom or bottom to top, however they choose.
   * 
   ******************************************/

  class Fed9UChannelInterface {
  public:

    Fed9UChannelInterface();
    Fed9UChannelInterface(u8 fedChannel, u8 countingBase = FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT, bool fromTop = FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT);

    u8 getChannel() const;
    u8 getFeUnit() const;
    u8 getFeUnitChannel() const;
    u8 getBase();
    bool getCountFromTop();

    Fed9UChannelInterface & setChannel(u8 channel);
    Fed9UChannelInterface & setFeUnit(u8 feUnit);
    Fed9UChannelInterface & setFeUnitChannel(u8 feUnitChannel);
    Fed9UChannelInterface & setBase(u8 base);
    Fed9UChannelInterface & setCountFromTop(bool fromTop);


    friend class Fed9UAddress;
    friend class Fed9UEvent;
  

  private:
    u8  getChannelInternal() const;
    Fed9UChannelInterface & setChannelInternal(u8 channel);

    u8 _channel;
    u8 _base;
    bool _fromTop;
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
    enum { UNSPECIFIED = 0xFF, FEBROADCAST = 14, BACKEND = 9, VME = 10 };
    /** \name Constructors */
    //@{
    Fed9UAddress();
    Fed9UAddress(u8 fedChannel, u8 ChannelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);
    Fed9UAddress(bool, u16);

    //@}
    /** \name Getters */
    //@{
    u8  getFedFpga() const;

    u8  getFedFeUnit() const;
    u8  getFeUnitChannel() const;
    u8  getChannelApv() const;
    u8  getApvStrip() const;

    u8  getFedChannel() const;
    u8  getFedApv() const;
    u16 getFedStrip() const;
    u8  getFeUnitApv() const;


    u8  getExternalFedFeUnit() const;
    u8  getExternalFeUnitChannel() const;
    u8  getExternalChannelApv() const;
    u8  getExternalApvStrip() const;

    u8  getExternalFedChannel() const; 
    u16 getExternalFedApv() const;
    u16 getExternalFedStrip() const;
    u8  getExternalFeUnitApv() const;

    u16 getCountBase() const;
    bool getCountFromTop() const;

    //@}
    /** \name Setters */
    //@{
    Fed9UAddress& setAddress(u8 fedFeUnit, u8 feUnitChannel = UNSPECIFIED,
			  u8 channelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);

    Fed9UAddress& setFedFpga(u8 fedFpga);

    Fed9UAddress& setFedFeUnit(u8 value);
    Fed9UAddress& setFeUnitChannel(u8 value);
    Fed9UAddress& setChannelApv(u8 value);
    Fed9UAddress& setApvStrip(u8 value);

    Fed9UAddress& setFedChannel(u8 fedChannel);
    Fed9UAddress& setFedApv(u8 fedApv);
    Fed9UAddress& setFedStrip(u16 fedStrip);
    Fed9UAddress& setFeUnitApv(u8 feUnitApv);

    Fed9UAddress& setExternalFedFeUnit(u8 value);
    Fed9UAddress& setExternalFeUnitChannel(u8 value);
    Fed9UAddress& setExternalChannelApv(u8 value);
    Fed9UAddress& setExternalApvStrip(u8 value);

    Fed9UAddress& setExternalFedChannel(u8 value); 
    Fed9UAddress& setExternalFedApv(u16 value); 
    Fed9UAddress& setExternalFedStrip(u16 value); 
    Fed9UAddress& setExternalFeUnitApv(u8 feUnitApv);
    
    
    /** This method allows you to set the feUnit number bassed upon the feFirmware Address (which happens to run from 1 throuh 8 and 15)
     *  rather than from 0 through 7 as in software internal or 8 through 1 in external numbering
     *  This is used inside the Fed9UVmeBase and below. It should
     *  be used when passing fpga numbers to Fed9UVmeBase.
     ************************************/
    Fed9UAddress& setFirmwareFedFeUnit(const u8 feFpga);
    
    /** This method allows you to get the feUnit number bassed upon the feFirmware Address (which happens to run from 1 throuh 8 and 15)
     *  rather than from 0 through 7 as in software internal or 8 through 1 in external numbering
     *  This is used inside the Fed9UVmeBase and below. It should
     *  be used when passing fpga numbers to Fed9UVmeBase.
     ************************************/
    u8 getFirmwareFedFeUnit() const;
      
    private:
    u8 _femod, _fechan, _apv, _strip;
    bool _countFromTop;
    u16 _countBase;
  };

}

#endif // H_Fed9UChannelAddress
#ifndef H_Fed9UBeStatus
#define H_Fed9UBeStatus

//*******************************************************************
//*								    *
//*   Class Fed9UBeStatus.hh  by Jonathan Fulcher IC	            *
//*								    *
//*******************************************************************


#include <iostream>
#include <sstream>
#include <string>


namespace Fed9U {

  using std::istream;
  using std::ostream;
  /**
   * Describes the contents of the Be Status Register.
   *
   *
   ********************************************************/

  class Fed9UBeStatus {
  public:
    Fed9UBeStatus( u32 status): _status(status){}
    
    ~Fed9UBeStatus() {}
    

    /*\method getStatusErrorsString() returns a string which describes any errors which are set within the be status register
     *
     *****************************/
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

    // ********* getters ********* //    
    bool getInternalFreeze() const {
      return (_status >> 1) & 0x1;
    }
    bool getSLinkDown() const {
      return (_status >> 2) & 0x1;
    }
    bool getSLinkFull() const {
      return (_status >> 3) & 0x1;
    }
    bool getBackPressure() const {
      return (_status >> 4) & 0x1;
    }
    bool getBxErrorAtOrbitReset() const {
      return (_status >> 5) & 0x1;
    }
    bool getTtcReady() const {
      return (_status >> 6) & 0x1;
    }
    bool getTrackerHeaderMonitorDataReady() const {
      return (_status >> 7) & 0x1;
    }
    bool getQdrFull()  const {
      return (_status >> 8) & 0x1;
    }
    bool getFrameAddressFifoFull() const  {
      return (_status >> 9) & 0x1;
    }
    bool getTotalLengthFifoFull() const {
      return (_status >> 10) & 0x1;
    }
    bool getTrackerHeaderFifoFull() const {
      return (_status >> 11) & 0x1;
    }
    bool getL1ABxFifoFull() const {
      return (_status >> 12) & 0x1;
    }
    bool getFeEventLengthFifoFull() const {
      return (_status >> 13) & 0x1;
    }
    bool getFeFpgaFull() const {
      return (_status >> 14) & 0x1;
    }
    bool getQdrPartialFull() const {
      return (_status >> 16) & 0x1;
    }
    bool getFrameAddressFifoPartialFull() const {
      return (_status >> 17) & 0x1;
    }
    bool getTotalLengthFifoPartialFull() const {
      return (_status >> 18) & 0x1;
    }
    bool getTrackerHeaderFifoPartialFull() const {
      return (_status >> 19) & 0x1;
    }
    bool getL1ABxFifoPartialFull() const {
      return (_status >> 20) & 0x1;
    }
    bool getFeEventLengthFifoPartialFull() const {
      return (_status >> 21) & 0x1;
    }
    bool getFeFpgaPartialFull() const {
      return (_status >> 22) & 0x1;
    }
    bool getQdrEmpty() const {
      return (_status >> 24) & 0x1;
    }
    bool getFrameAddressFifoEmpty() const {
      return (_status >> 25) & 0x1;
    }
    bool getTotalLengthFifoEmpty() const {
      return (_status >> 26) & 0x1;
    }
    bool getTrackerHeaderFifoEmpty() const {
      return (_status >> 27) & 0x1;
    }
    bool getL1ABxFifoEmpty() const {
      return (_status >> 28) & 0x1;
    }
    bool getFeEventLengthFifoEmpty() const {
      return (_status >> 29) & 0x1;
    }
    bool getFeFpgaEmpty() const {
      return (_status >> 30) & 0x1;
    }

  private:
    u32 _status;
  };

  using std::istream;
  using std::ostream;

  ostream& operator<<(ostream& os, const Fed9UBeStatus& sd);
  
}

#endif // H_Fed9UBeStatus
#ifndef H_Fed9UStripDescription
#define H_Fed9UStripDescription

//*******************************************************************
//*								    *
//*   Class Fed9UStripDescription.hh  by Jonathan Fulcher IC	    *
//*								    *
//*******************************************************************


#include <iosfwd>

namespace Fed9U {

  using std::istream;
  using std::ostream;
  /**
   * Describes a strip.
   *
   * Contains the pedestal, both threshold factors, the noise, and the disable bit
   * associated with each FED strip.
   *
   ********************************************************/

  class Fed9UStripDescription {
  public:
    Fed9UStripDescription(i16 pedestal,
			  i16 highThreshold,
			  i16 lowThreshold,
			  bool disable
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(FED_STRIP_HIT_THRESHOLD_HIGH),
      _lowThresholdFactor(FED_STRIP_HIT_THRESHOLD_LOW),
      _noise(static_cast<float>(highThreshold)/static_cast<float>(FED_STRIP_HIT_THRESHOLD_HIGH)),
      _disable(disable) {}
  
    Fed9UStripDescription(i16 pedestal = 0,
			  float highThresholdFactor = FED_STRIP_HIT_THRESHOLD_HIGH,
			  float lowThresholdFactor = FED_STRIP_HIT_THRESHOLD_LOW,
			  float noise = 0.0,
			  bool disable = false
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(highThresholdFactor),
      _lowThresholdFactor(lowThresholdFactor),
      _noise(noise),
      _disable(disable) {}
   
    ~Fed9UStripDescription() {}
    
    // ********* getters ********* //
    i16 getPedestal() const { return _pedestal; }
    i16 getHighThreshold() const { return static_cast<i16>(_highThresholdFactor*_noise); }
    i16 getLowThreshold() const { return static_cast<i16>(_lowThresholdFactor*_noise); }
    float getHighThresholdFactor() const { return _highThresholdFactor; }
    float getLowThresholdFactor() const { return _lowThresholdFactor; }
    float getNoise() const { return _noise; }
    bool getDisable() const { return _disable; }

    // ********* setters ********* //
    void setPedestal(i16 value) { _pedestal = value; }
    void setHighThresholdFactor(float value) { _highThresholdFactor = value; }
    void setLowThresholdFactor(float value) { _lowThresholdFactor = value; }
    void setHighThreshold(i16 value) { _noise = static_cast<float>(value)/_highThresholdFactor; }
    void setLowThreshold(i16 value) { _noise = static_cast<float>(value)/_lowThresholdFactor; }
    void setNoise(float value) { _noise = value; }
    void setDisable(bool value) { _disable = value; }

    void load(istream&);
    void save(ostream&) const;
  private:
    i16	_pedestal;
    float _highThresholdFactor;
    float _lowThresholdFactor;
    float _noise;
    bool _disable;
  };

  using std::istream;
  using std::ostream;

  ostream& operator<<(ostream& os, const Fed9UStripDescription& sd);
  istream& operator>>(istream& is, Fed9UStripDescription& sd);

}

#endif // H_Fed9UStripDescription
#ifndef H_Fed9UStrips
#define H_Fed9UStrips

//*******************************************************************
//*								    *
//*   Class Fed9UStrips.hh  by Jonathan Fulcher IC		    *
//*								    *
//*******************************************************************

#include <iosfwd>
#include <vector>


namespace Fed9U {

  using std::vector;

  /**
   * Description of all the strips in a FED.
   *
   * Stores a Fed9UStripDescription for each APV strip in a FED.
   *
   *******************************************************************/  

  class Fed9UStrips {
  public:
    Fed9UStrips();
    ~Fed9UStrips();

    Fed9UStripDescription& getStrip(Fed9UAddress fedStrip);
    const Fed9UStripDescription& getStrip(Fed9UAddress fedStrip) const;
    vector<Fed9UStripDescription> getApvStrips(Fed9UAddress fedApv) const;

    void setStrip(Fed9UAddress fedStrip, const Fed9UStripDescription& value);
    void setApvStrips(Fed9UAddress fedApv, const vector<Fed9UStripDescription> & values);

    void loadStrips(istream&);
    void saveStrips(ostream&) const;
    void loadDefaultStrips();
  private:
    //vector< vector< vector< vector<Fed9UStripDescription> > > > _strips;
    Fed9UStripDescription _strips[STRIPS_PER_FED];
  };
  
  using std::istream;
  using std::ostream;

  ostream& operator<<(ostream&, const Fed9UStrips&);
  istream& operator>>(istream&, Fed9UStrips&);
  
}

#endif // H_Fed9UStrips
#ifndef H_Fed9UTempControl
#define H_Fed9UTempControl

#include <iostream>

namespace Fed9U {
  using std::istream;
  using std::ostream;

  class Fed9UTempControl {
  public:
    
    Fed9UTempControl(const Fed9UTempControl& temp) : 
      _lm82High(temp.getLm82High()), _fpgaHigh(temp.getFpgaHigh()), _critical(temp.getCritical())
    { }
    Fed9UTempControl(u32 lm82High = 127, u32 fpgaHigh = 127, u32 critical = 127) :
      _lm82High(lm82High), _fpgaHigh(fpgaHigh), _critical(critical)
    { }
    u32 getLm82High() const { return _lm82High; }
    u32 getFpgaHigh() const { return _fpgaHigh; }
    u32 getCritical() const { return _critical; }
    Fed9UTempControl & setLm82High(u32 lm82High) { _lm82High = lm82High; return *this; }
    Fed9UTempControl & setFpgaHigh(u32 fpgaHigh) { _fpgaHigh = fpgaHigh; return *this; }
    Fed9UTempControl & setCritical(u32 critical) { _critical = critical; return *this; }
  private:
    u32 _lm82High, _fpgaHigh, _critical;
  };
  
  class Fed9UTempControlInfo : public Fed9UTempControl {
  public:
    Fed9UTempControlInfo():Fed9UTempControl(),_lm82Actual(0), _fpgaActual(0), _id(0), _status(0)
    {}
    Fed9UTempControlInfo(u32 lm82High, u32 fpgaHigh, u32 critical,
                         u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(lm82High, fpgaHigh, critical),
      _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }
    Fed9UTempControlInfo( const Fed9UTempControl& temp, u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(temp), _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }
    u32 getLm82Actual() const { return _lm82Actual; }
    u32 getFpgaActual() const { return _fpgaActual; }
    u32 getId()         const { return _id; }
    u32 getStatus()     const { return _status; }
    Fed9UTempControlInfo & setLm82Actual(u32 lm82Actual) { _lm82Actual = lm82Actual; return *this; }
    Fed9UTempControlInfo & setFpgaActual(u32 fpgaActual) { _fpgaActual = fpgaActual; return *this; }
    Fed9UTempControlInfo & setId(u32 id) { _id = id; return *this; }
    Fed9UTempControlInfo & setStatus(u32 status) { _status = status; return *this; }
    
  private:
    u32 _lm82Actual, _fpgaActual, _id, _status;
  };
  
  inline ostream& operator << (ostream & os, const Fed9UTempControl& data) {
    return os << data.getLm82High() 
              << '\t' 
              << data.getFpgaHigh() 
              << '\t' << data.getCritical() 
              << '\n';
  }
  
  inline ostream& operator << (ostream & os, const Fed9UTempControlInfo& data) {
    return os << reinterpret_cast<const Fed9UTempControl&>(data) 
              << data.getLm82Actual() 
              << '\t' 
              << data.getFpgaActual()
              << '\t' 
              << data.getId() 
              << '\t' 
              << data.getStatus()
              << '\n';
  }
  
  inline istream& operator >> (istream & is, Fed9UTempControl& data) {
    u32 temp;
    is >> temp;
    data.setLm82High(temp);
    is >> temp;
    data.setFpgaHigh(temp);
    is >> temp;
    data.setCritical(temp);
    return is;
  }
  
  inline istream& operator >> (istream & is, Fed9UTempControlInfo& data) {
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

//*******************************************************************
//*								    *
//*   Class Fed9UFrontEndDescription.hh  by Matt Noy, J Fulcher	    *
//*								    *
//*******************************************************************

namespace Fed9U {

  /**
   * Description of a FED front-end module.
   *
   * Contains the 1 Opto-RX offset, 12 Trim-DAC offsets, and 12 fine
   * and coarse phase settings associated with each front-end module.
   ********************************************************************/

  struct Fed9UAdcControls {
    Fed9UAdcControls(bool dfsen = false, bool dfsval = false, bool s1 = false, bool s2 = false)
      : _dfsen(dfsen), _dfsval(dfsval), _s1(s1), _s2(s2) { }
    bool _dfsen;
    bool _dfsval;
    bool _s1;
    bool _s2;
  };

  class Fed9UFrontEndDescription {
  public:
    Fed9UFrontEndDescription(u16 fine = 0, 
			     u16 coarse = 0, 
			     u16 trim = 0, 
			     u16 opto = 0, 
			     u16 cap = 0, 
			     bool apvDisable = false, 
			     u16 median = 0, 
			     bool medianDisable = true,
			     u16 channelThreshold = 0,
			     u16 channelBufferOccupancy = 0,
			     const Fed9UAdcControls & adcControls = Fed9UAdcControls(false,false,false,false),
			     bool feUnitDisable = 0,
			     const Fed9UTempControl & tempControl = Fed9UTempControl(),
                             bool complement = true
			     );
    ~Fed9UFrontEndDescription() {}

    //private:
    u16 _fineDelay[CHANNELS_PER_FEUNIT];
    u16 _coarseDelay[CHANNELS_PER_FEUNIT];
    u16 _trimDacOffset[CHANNELS_PER_FEUNIT];
    u16 _channelThreshold[CHANNELS_PER_FEUNIT];
    u16 _channelBufferOccupancy[CHANNELS_PER_FEUNIT]; // TODO remove this and update the streamers and description version?
    Fed9UAdcControls _adcControls[CHANNELS_PER_FEUNIT];
    bool _complement[CHANNELS_PER_FEUNIT];

    bool _apvDisable[APVS_PER_FEUNIT];
    u16 _optoRxOffset;
    u16 _optoRxCapacitor;
    u16 _medianOverride[APVS_PER_FEUNIT];
    bool _medianOverrideDisable;
    bool _feUnitDisable;
    Fed9UTempControl _tempControl;
  };

  using std::istream;
  using std::ostream;

  ostream& operator<<(ostream& os, const Fed9UFrontEndDescription& desc);
  istream& operator>>(istream& is, Fed9UFrontEndDescription& desc);
  ostream& operator<<(ostream& os, const Fed9UAdcControls& cont);
  istream& operator>>(istream& is, Fed9UAdcControls& cont);

}

#endif // H_Fed9UFrontEndDescription
#ifndef H_Fed9UTtcrxDescription
#define H_Fed9UTtcrxDescription

//Author: Gareth Rogers.
//Date  : March 2004.

  /**
   * A class used to provide a software representation of the TTCrx chip.
   *
   * The class provides a representation of the all the read and writable registers
   * on the TTCrx chip. More information about the TTCrx chip can be found in the
   * TTCrx Reference Manual: A Timing, Trigger and Control Receiver ASIC for LHC Detectors. 
   */


#include <vector>

namespace Fed9U {

  using std::istream;
  using std::ostream;
  using std::vector;
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

  class Fed9UTtcrxDescription {

    /**
     * The Fed9UTtcrxDescription public interface.
     */
  public:

    /**
     * This constructor will create a Fed9UTtcrxDescription either with User defined values
     * or the default values. The defaults are all set to the hardware defaults, except
     * the fine delay settings.
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
			  bool disableParrallelBus = true,
			  bool disableSerialB      = true,
			  bool disableNonDeskewedClock = false);

    /**
     * This is used to hold the information about which reset is required on the TTCrx.
     */
    Fed9UTtcrxDescription& setCounterReset(const Fed9UTtcrxCounterReset& counterReset);

    /**
     * Returns a TTCrx reset structure which contains the settings for which resets
     * are require on the TTCrx.
     */
    Fed9UTtcrxCounterReset getCounterReset() const;

    /**
     * This set the total amount of delay skew for the L1Accept, broadcast strobe 1
     * signal, the bunch and event counter reset signals and the system broadcast
     * command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns. The fine
     * delay can be set from 0 to 239, with each step representing approximately 104ps.
     */
    Fed9UTtcrxDescription& setL1AcceptDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * This will set the total amount of delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * This sets the coarse delay for those signals, where the coarse delay can be
     * set from 0 to 15. Each step represents 25ns. It sets the fine delay for either
     * the 40MHz deskewed clock one or two depending on which the signals are
     * synchronised to. This can be set in element two fo the control register vector.
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * This will set the coarse delay skew for the L1Accept, broadcast strobe 1 signal,
     * bunch and event counter reset signals and the system broadcast command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns.
     */
    Fed9UTtcrxDescription& setL1AcceptCoarseDelay(u16 coarseDelay);

    /**
     * This will return the coarse delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command.
     *
     * The coarse delay can be 0 to 15. Each step represents 25ns.
     */
    u16 getL1AcceptCoarseDelay() const;

    /**
     * This will set the coarse delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoCoarseDelay(u16 coarseDelay);

    /**
     * This will set the coarse delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * The coarse delay can be 0 to 15. Each step represents 25ns.
     */
    u16 getBrcstStrTwoCoarseDelay() const;

    /**
     * This will set the fine delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if element 2 in the control register is disabled.
     *
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDescription& setClockDesOneFineDelay(u16 fineDelay);

   /**
     * This will return the fine delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if element 2 in the control register is disabled.
     *
     * The fine delay can be 0 to 239, with each step representing
     * approximately 104ps.
     */
    u16 getClockDesOneFineDelay() const;

    /**
     * This will set the fine delay skew for the deskewed 40MHz clock two.
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled by passing a true to the
     * setDeskewedClock2Selected method.
     *
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDescription& setClockDesTwoFineDelay(u16 fineDelay);

    /**
     * This will return the fine delay skew for the deskewed 40MHz clock two.
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled. This can be checked using the method
     * getDeskewedClock2Selected. If a true is returned it is enabled.
     *
     * The fine delay can be 0 to 239, with each step representing
     * approximately 104ps.
     */
    u16 getClockDesTwoFineDelay() const;

    /**
     * Sets the charge pump current used in the delay-lock loops of the high
     * resolution phase shifters.
     *
     * It has a maximum value of 7 and a minimum value of 1.
     */
    Fed9UTtcrxDescription& setDllPumpCurrent(u16 pumpCurrent);

    /**
     * Returns the charge pump current used in the delay-lock loops of the high
     * resolution phase shifters.
     */
    u16 getDllPumpCurrent() const;

    /**
     * Sets the charge pump current used in the phase-lock loops which provide
     * the clock recovery.
     *
     * It has a maximum value of 7 and a minimum value of 1.
     */
    Fed9UTtcrxDescription& setPllPumpCurrent(u16 pumpCurrent);

    /**
     * Returns the charge pump current used in the phase-lock loops which provide
     * the clock recovery.
     */
    u16 getPllPumpCurrent() const;

    /**
     * This sets the 14 bit individually addressed command (IAC) ID.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link. It has a maximum value of 16383.
     */
    Fed9UTtcrxDescription& setIacId(u16 id);

    /**
     * This returns the 14 bit individually addressed command (IAC) ID.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link. It has a maximum value of 16383.
     */
    u16 getIacId() const;

    /**
     * This sets the 6 bit I2C ID.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     * It has a maximum value of 64.
     */
    Fed9UTtcrxDescription& setI2cId(u16 id);

    /**
     * This returns the 6 bit I2C ID.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     * It has a maximum value of 64.
     */
    u16 getI2cId() const;

    /**
     * This is used to control enable or disable the bunch and event counter operations.
     * The individual settings can be enabled or disabled using the enum
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh. Both flags can be set using a bitwise
     * OR and any flag not set will be disabled.
     *
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6.
     */
    Fed9UTtcrxDescription& setCounterOperation(u16 counterOperation);

    /**
     * This is used to the bunch and event counter operations enable or disable flags.
     * The individual settings can be checked by using the masks provided in
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh.
     *
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6. Disabling these can be used to save power.
     */
    u16 getCounterOperation() const;

    /**
     * This can be used to enable or disable the hamming checking.
     *
     * A true will disable the hamming checking and a false will enable hamming checking.
     */
    Fed9UTtcrxDescription& setHammingCheckingDisable(bool disableHammingChecking);

    /**
     * This can be used to read the status of the hamming checking.
     *
     * A true means that hamming checking is disabled and a false enabled.
     */
    bool getHammingCheckingDisable() const;

    /**
     * This selectes the 40MHz deskewed clock two. If selected it is used to sychronise
     * the broadcast strobe 2 signal and the user broadcast command. A true will select
     * this clock.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Selected(bool selectClockTwo);

    /**
     * If a true is returned then the broadcast strobe 2 signal and the user broadcast
     * command are synchronised to the 40MHz deskewed clock two.
     */
    bool getDeskewedClock2Selected() const;

    /**
     * If a true is passed this will disable the 40MHz deskewed clock two.
     * If false the clock will be disabled.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Disable(bool disableClockTwo);

    /**
     * If a true is returned then the deskewed clock two is disabled. If
     * false it is enabled.
     */
    bool getDeskewedClock2Disable() const;

    /**
     * Disable the L1Accept clock output.
     *
     * A true will disable the output and a false enable it.
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setClockL1AcceptDisable(bool l1AcceptDisable);

    /**
     * Returns the setting on the L1Accept clock disable flag.
     *
     * A true means it is disabled and a false enabled.
     */
    bool getClockL1AcceptDisable() const;

    /**
     * This will disable the non-deskewed 40MHz clock output if a
     * true is passed to the function. A false will enable it.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setNonDeskewedClockDisable(bool disable);

    /**
     * If true then the non-deskewed 40MHz clock is disabled.
     * False and it is enabled.
     */
    bool getNonDeskewedClockDisable() const;

    /**
     * Disable the parrallel bus output if true.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setParrallelOutputDisable(bool disable);

    /**
     * Return true if the parrallel bus output is disabled.
     */
    bool getParrallelOutputDisable() const;

    /**
     * Disable the serial B output if true.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setSerialBDisable(bool disable);

    /**
     * Return true if the serial B output is disabled.
     */
    bool getSerialBDisable() const;

//     //todo - These are depricated and will be removed in later versions.
//     /**
//      * This is used to control the operational mode of the TTCrx chip. It can also be
//      * used to reduce power consumption by disabling some of the chips features.
//      *
//      * The control register is set using an eight element vector. A description of the elements
//      * is given below.
//      * 0 Enable bunch counter operation - default true.
//      * 1 Enable event counter operation - default true.
//      * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//      * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//      * explanation.
//      * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//      *                        synchronise the Broadcast Strobe 2 - default false.
//      * 3 Enable Deskewed clock 2 output   - default false.
//      * 4 Enable L1Accept clock output - default true.
//      * 5 Enable Parallel output bus  - default false.
//      * 6 Enable Serial B output       - default false.
//      * 7 Enable (non-deskewed) 40MHz clock output - default true.
//      * In all cases a true is used to enable the setting and false to disable.
//      */
//     Fed9UTtcrxDescription& setControlRegister(vector<bool> controlRegister);

//     //todo - These are depricated and will be removed in later versions.
//     /**
//      * This is used to read back the operational mode of the TTCrx chip.
//      *
//      * The settings are returned in an eight element vector. A description of the elements
//      * is given below.
//      * 0 Enable bunch counter operation - default true.
//      * 1 Enable event counter operation - default true.
//      * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//      * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//      * explanation.
//      * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//      *                        synchronise the Broadcast Strobe 2 - default false.
//      * 3 Enable Deskewed clock 2 output   - default false.
//      * 4 Enable L1Accept clock output - default true.
//      * 5 Enable Parallel output bus  - default false.
//      * 6 Enable Serial B output       - default false.
//      * 7 Enable (non-deskewed) 40MHz clock output - default true.
//      * In all cases a true is used to enable the setting and false to disable.
//      */
//     vector<bool> getControlRegister() const;

    /**
     * Some private data members to define the state of the TTCrx.
     */
  private:
    Fed9UTtcrxCounterReset _counterReset;
    u16 _l1AcceptCoarseDelay, _brcstStrTwoCoarseDelay, _clockDesOneFineDelay, _clockDesTwoFineDelay;
    u16 _dllPumpCurrent, _pllPumpCurrent, _iacId, _i2cId;
    u16 _counterOperation;
    bool _disableHammingChecking, _selectClockTwo, _disableClockTwo, _disableClockL1Accept;
    bool _disableParrallelBus, _disableSerialB, _disableNonDeskewedClock;
    vector<bool> _controlRegister;

  }; //End of class Fed9UTtcrxDescription declaration.

  /**
   * This overload of operator<< will allow the Fed9UTtcrxDescription to be written to an ostream.
   */
  ostream& operator<<(ostream& os, const Fed9UTtcrxDescription& data);

  /**
   * This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   */
  istream& operator>>(istream& is, Fed9UTtcrxDescription& data);

}      //End of Fed9U namespace.
#endif //H_Fed9UTtcrxDescription
#ifndef H_Fed9UTtcrxDescriptionInfo
#define H_Fed9UTtcrxDescriptionInfo

//Author: Gareth Rogers.
//Date  : March 2004.

/**
 * A class used to provide a software representation of the TTCrx chip.
 *
 * This is a derived class from the Fed9UTtcrxDescription. It contains in addition
 * to the read and writable settings the read only settings. The purpose of this
 * class is to read back settings from the TTCrx. It can be used to configure the
 * TTCrx as it is a derived class of Fed9UTtcrxDescription, however the methods
 * provided in this class can only hold read only values and any information set by
 * the user will not be written to the TTCrx.
 */


#include <vector>

namespace Fed9U {

  using std::istream;
  using std::ostream;

  class Fed9UTtcrxDescriptionInfo : public Fed9UTtcrxDescription {

    /**
     * The Fed9UTtcrxDescriptionInfo public interface.
     */
  public:

    /**
     * The Fed9UTtcrxDescriptionInfo constructor.
     *
     * Allows a either completely initialised object to be created.
     * Alternatively a the default values can be used and a blank object
     * created and used.
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
			      u16 masterModeB = 0,
			      u16 masterModeA = 0,
			      u16 testOutputs = 5,
			      u16 numbClockCycles = 7,
			      u16 configSettings  = 66,
			      u16 statusRegister  = 0);

    /**
     * This sets the number of bunch crossing that have occured since the last reset.
     *
     * It counts the number of clock signals that are received by the TTCrx. It is a 12
     * bit register and so has a maximum value of 4095.
     */
    Fed9UTtcrxDescriptionInfo& setBunchCounter(u16 bunchCount);

    /**
     * This returns the number of bunch crossing that have occured since the last reset.
     *
     * It counts the number of clock signals that are received by the TTCrx. It is a 12
     * bit register and so has a maximum value of 4095.
     */
    u16 getBunchCounter() const;

    /**
     * This sets the number of events that have been received since the last reset.
     *
     * It counts the number of trigger signals that are received by the TTCrx. It is a 24
     * bit register and so has a maximum value of 16777215.
     */
    Fed9UTtcrxDescriptionInfo& setEventCounter(u32 eventCount);

    /**
     * This returns the number of events that have been received since the last reset.
     *
     * It counts the number of trigger signals that are received by the TTCrx. It is a 24
     * bit register and so has a maximum value of 16777215.
     */
    u32 getEventCounter() const;

    /**
     * This stores the number of single bit errors that are recignised and corrected by the
     * receiver's Hamming decoder. It has a maximum value  of 65535.
     */
    Fed9UTtcrxDescriptionInfo& setSingleBitErrorCount(u16 singleBitErrorCount);

    /**
     * This return the number of single bit errors that are recignised and corrected by the
     * receiver's Hamming decoder.
     */
    u16 getSingleBitErrorCount() const;

    /**
     * Stores the number of double bit Hamming and frame errors that the TTCrx recognises.
     * It has a maximum value of 255.
     */
    Fed9UTtcrxDescriptionInfo& setDoubleBitErrorCount(u16 doubleBitErrorCount);

    /**
     * Returns the number of double bit Hamming and frame errors. It has a maximum value of 255.
     */
    u16 getDoubleBitErrorCount() const;

    /**
     * Stores the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * It has a maximum value of 255.
     */
    Fed9UTtcrxDescriptionInfo& setSeuErrorCount(u16 seuErrorCount);

    /**
     * Returns the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * It has a maximum value of 255.
     */
    u16 getSeuErrorCount() const;

    /**
     * This determines the overall operating mode of the TTCrx.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeA(u16 masterMode);

    /**
     * This determines the overall operating mode of the TTCrx.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeA() const;

    /**
     * This determines the overall operating mode of the TTCrx.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeB(u16 masterMode);

    /**
     * This determines the overall operating mode of the TTCrx.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeB() const;

    /**
     * This is a read only register, which selects the test outputs.
     *
     * This register is used for testing and debugging the TTCrx. It
     * has a maximum value of 7.
     */
    Fed9UTtcrxDescriptionInfo& setTestOutputs(u16 testOutputs);

    /**
     * This is a read only register, which selects the test outputs.
     *
     * This register is used for testing and debugging the TTCrx. It
     * has a maximum value of 7.
     */
    u16 getTestOutputs() const;

    /**
     * This is a read only register, which selects the number of clock
     * cycles after which the frequency phase detection will stop.
     *
     * It is a testing and debugging register. It has a maximum value
     * of 7 and a hardware default value of 7.
     */
    Fed9UTtcrxDescription& setFreqDetectCount(u16 numbClockCycles);

    /**
     * This is a read only register, which selects the number of clock
     * cycles after which the frequency phase detection will stop.
     *
     * It is a testing and debugging register. It has a maximum value
     * of 7 and a hardware default value of 7.
     */
    u16 getFreqDetectCount() const;

    /**
     * This is a read only configuration register. It contains some testing
     * and debugging settings. The enum Fed9UTtcrxConfig can be used to set
     * these bits. Any setting not set will be cleared.
     *
     * A description of the bits is given below. As these are read only on the
     * TTCrx, what is read back should correspond to the defaults given below.
     * These defaults are also the settings used by the TTCrx.
     * FED9U_TTCRX_CONFIG_SEL_TEST_PD - Selects the external test signal for enabling
     *                                  the PPL phase detector. Default is 0
     * FED9U_TTCRX_CONFIG_SEL_INPUT_A - When 1 selects the inputs from option link,
     *                                  otherwise test_in. Default is 1.
     * FED9U_TTCRX_CONFIG_PLL_RESET   - Assert PLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DLL_RESET   - Assert DLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR - If 1 disables the automatic frequency increase
     *                                        after a PLL reset. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG  - If 1 disbale watchdog circuit. Default 0.
     * FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION - Enable hamming error correction on
     *                                                incoming data stream. Default 1.
     * FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS - Enables test input/outputs. Default 0.
     */
    Fed9UTtcrxDescription& setConfigSettings(u16 configSettings);

    /**
     * This is a read only configuration register. It contains some testing
     * and debugging settings. The enum Fed9UTtcrxConfig can be used to set
     * these bits. Any setting not set will be cleared.
     *
     * A description of the bits is given below. As these are read only on the
     * TTCrx, what is read back should correspond to the defaults given below.
     * These defaults are also the settings used by the TTCrx.
     * FED9U_TTCRX_CONFIG_SEL_TEST_PD - Selects the external test signal for enabling
     *                                  the PPL phase detector. Default is 0
     * FED9U_TTCRX_CONFIG_SEL_INPUT_A - When 1 selects the inputs from option link,
     *                                  otherwise test_in. Default is 1.
     * FED9U_TTCRX_CONFIG_PLL_RESET   - Assert PLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DLL_RESET   - Assert DLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR - If 1 disables the automatic frequency increase
     *                                        after a PLL reset. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG  - If 1 disbale watchdog circuit. Default 0.
     * FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION - Enable hamming error correction on
     *                                                incoming data stream. Default 1.
     * FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS - Enables test input/outputs. Default 0.
     */
    u16 getConfigSettings() const;

  //\todo - depricated. To be delete.
//     /**
//      * This is a configuration registers that holds the TTCrx read only configuration settings.
//      *
//      * The vector that is passed to the method should contain 10 elements and the contents of
//      * each element are as follows:-
//      * 0 Which test outputs have been selected.
//      * 1 Whether an external test signal is enabled for the phase-lock loop phase detector.
//      * 2 When 1 selects input from the optical link.
//      * 3 Assert phase-lock loop test reset line.
//      * 4 Assert delay-lock loop test reset line.
//      * 5 Frequency check setting.
//      * 6 If 1 then the automatic frequency increase after a phase-lock loop reset will be disabled.
//      * 7 If 1 then the watchdog circuit is disabled.
//      * 8 Whether or not the Hamming error detection/correction on incoming data stream is enabled.
//      * 9 Whether the test input/outputs are enabled or disabled.
//      */
//     Fed9UTtcrxDescriptionInfo& setConfigurationSettings(vector<u16> configSettings);

//     /**
//      * This is a read only configuration register. The settings here control the
//      * behaviour of the TTCrx.
//      *
//      * The returned vector has 10 elements. The contents of the elements is:-
//      * 0 Shows which test outputs have been selected.
//      * 1 Whether an external test signal is enabled for the phase-lock loop phase detector.
//      * 2 When 1 selects input from the optical link.
//      * 3 Assert phase-lock loop test reset line.
//      * 4 Assert delay-lock loop test reset line.
//      * 5 Frequency check setting.
//      * 6 If 1 then the automatic frequency increase after a phase-lock loop reset will be disabled.
//      * 7 If 1 then the watchdog circuit is disabled.
//      * 8 Whether or not the Hamming error detection/correction on incoming data stream is enabled.
//      * 9 Whether the test input/outputs are enabled or disabled.
//      */
//     vector<u16> getConfigurationSettings() const;

    /**
     * The current status of the internal monitoring signals. The flags are set using the
     * Fed9UTtcrxStatus enum defined in TypeDefs.hh. Multiple flags can be use by using a
     * a bitwise OR. Any flag not set will be cleared.
     *
     * The flags that can be set are as follows :-
     * Auto Reset Flag - a 1 indicates an automatic reset has occurred due to a
     *                   timeout condition in the watchdog circuit.
     * Frame Synch     - a 1 indicates that channel B is synchronized to the
     *                   data stream.
     * DLL Ready       - a 1 indicates the High-Resolution phase shiftes are
     *                   working properly.
     * PLL ready       - a 1 indicates that the clock and data recovery circuit
     *                   is locked on the incoming data stream.
     * Elements 1 to 3 should be 1 under normal operating conditions.
     */
    Fed9UTtcrxDescriptionInfo& setStatusRegister(u16 statusRegister);

    /**
     * Returns the status of of internal monitoring signals. The returned u16 can be masked
     * with the Fed9UTtcrxStatus enum defined in TypeDefs.hh to check for set flags.
     *
     * The flags that can be set are as follows :-
     * Auto Reset Flag - a 1 indicates an automatic reset has occurred due to a
     *                   timeout condition in the watchdog circuit.
     * Frame Synch     - a 1 indicates that channel B is synchronized to the
     *                   data stream.
     * DLL Ready       - a 1 indicates the High-Resolution phase shiftes are
     *                   working properly.
     * PLL ready       - a 1 indicates that the clock and data recovery circuit
     *                   is locked on the incoming data stream.
     * Elements 1 to 3 should read a 1 under normal operating conditions.
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
   * This overload of operator<< will allow the Fed9UTtcrxDescriptionInfo to be written to an ostream.
   */
  ostream& operator<<(ostream & os, const Fed9UTtcrxDescriptionInfo& data);

  /**
   * This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   */
  istream& operator>>(istream& is, Fed9UTtcrxDescriptionInfo& data);

}      //End of Fed9U namespace
#endif //H_Fed9UTtcrxDescriptionInfo
#ifndef H_Fed9UVoltageControl
#define H_Fed9UVoltageControl

//Author: Gareth Rogers.
//Date  : April 2004.

  /**
   * A class to provide a software representation of the ADM1025 voltage monitor.
   *
   * This class can be used to configure the read and write registers on the ADM1025
   * voltage montior. It can monitor a 2.5V, the processor core voltage, 3.3V, 12V and its supply voltage as well
   * as it own and an external temperature.  All voltages are accurate to 3 decimal places.
   */


#include <vector>

namespace Fed9U {

  using std::istream;
  using std::ostream;

  class Fed9UVoltageControl {

    /**
     * Some public member functions for creating and maintaining a Fed9UVoltageControl object.
     */
  public:

    /**
     * The Fed9UVoltageControl constructor.
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
     * This can be used to place the voltage controller in a stand by mode which does not perform
     * any measurements or conversion and can be used to save power when the voltage controller is
     * not needed.
     *
     * A true will shut the controller down and a false enable it. This will not reset the chip.
     * All register values will be kept.
     */
    Fed9UVoltageControl& setStandBy(bool standBy);

    /**
     * This returns the running state of the voltage controller.
     *
     * A true will shut the controller down and a false enable it. This will not reset the chip.
     * All register values will be kept.
     */
    bool getStandBy() const;

    /**
     * This provides the ability to reset the status registers to their hardware defaults.
     *
     * Passing a true will flag the status register for a reset, a false will not perform a reset.
     */
    Fed9UVoltageControl& setResetStatusRegister(bool reset);

    /**
     * Returns the state of the reset status register flag.
     *
     * If the return value is true then the status register will be reset. If it is false then is won't.
     */
    bool getResetStatusRegister() const;

    /**
     * This sets the upper limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageControl& set2Point5VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    float get2Point5VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageControl& set2Point5VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    float get2Point5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageControl& set3Point3VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float get3Point3VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageControl& set3Point3VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float get3Point3VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 5V reading.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    Fed9UVoltageControl& set5VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 5V reading.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    float get5VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 5V reading.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    Fed9UVoltageControl& set5VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 5V reading.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
    * It has a maximum value of 6.640V and a minimum of 0.
    * It has a minimum value of 0V.
     */
    float get5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 12V reading.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    Fed9UVoltageControl& set12VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 12V reading.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    float get12VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 12V reading.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    Fed9UVoltageControl& set12VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 12V reading.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    float get12VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    Fed9UVoltageControl& setCoreVoltageMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    float getCoreVoltageMax() const;

    /**
     * This sets the lower limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    Fed9UVoltageControl& setCoreVoltageMin(float voltage);

    /**
     * This returns the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    float getCoreVoltageMin() const;

    /**
     * This sets the upper limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageControl& setSupplyVoltageMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float getSupplyVoltageMax() const;

    /**
     * This sets the lower limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageControl& setSupplyVoltageMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float getSupplyVoltageMin() const;

    /**
     * This sets the upper limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageControl& setExternalTempMax(i16 temperature);

    /**
     * This returns the upper limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMax() const;

    /**
     * This sets the lower limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageControl& setExternalTempMin(i16 temperature);

    /**
     * This returns the lower limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMin() const;

    /**
     * This sets the upper limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageControl& setInternalTempMax(i16 temperature);

    /**
     * This returns the upper limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getInternalTempMax() const;

    /**
     * This sets the lower limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageControl& setInternalTempMin(i16 temperature);

    /**
     * This returns the lower limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getInternalTempMin() const;

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to set
     * that offset and to chose which measurement you wish to offset.
     *
     * The offset must be in the range -128oC to 127oC. Passing a boolean true will enable the offset
     * for the internal temperature reading and a false for the external temperature offset.
     */
    Fed9UVoltageControl& setTempOffset(i16 offset, bool internal);

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to read
     * that offset.
     *
     * The offset is in the range -128oC to 127oC. The offset is given as the return arguement and
     * which reading is offset can be found using the getOffsetTempSelect() member function.
     */
    i16 getTempOffset() const;

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to find
     * out which measurement is offset.
     *
     * A boolean true will enable the offset for the internal temperature reading and a false for
     * the external temperature offset. A return value of true signifies the internal temperature
     * measurement will be offset and a return value of false the external temperature will be offset.
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

  };// End of class Fed9UVoltageControl

  /**
   * This overload of operator<< can used to write a Fed9UVoltageControl to an ostream.
   */
  ostream& operator<<(ostream & os, const Fed9UVoltageControl& data);

  /**
   * This overload of operator>> can used to read a Fed9UVoltageControl from an istream.
   */
  istream& operator>>(istream & is, Fed9UVoltageControl& data);
  
}// End of Fed9U namespace
#endif// H_Fed9UVoltageControl
#ifndef H_Fed9UVoltageControlInfo
#define H_Fed9UVoltageControlInfo

//Author: Gareth Rogers.
//Date  : April 2004.

  /**
   * A class to provide a software representation of the ADM1025 voltage monitor.
   *
   * This class is designed to read back all the read only settings on the voltage
   * monitor. It inherits from Fed9UVoltageControl, so it can also read the read
   * write settings on the ADM1025 voltage controller. All voltages are accurate
   * to 3 decimal places.
   */


#include <vector>

namespace Fed9U {

  using std::istream;
  using std::ostream;

  class Fed9UVoltageControlInfo : public Fed9UVoltageControl {

    /**
     * Some public member functions for creating and maintaining a Fed9UVoltageControl object.
     */
  public:

    /**
     * The Fed9UVoltageControlInfo constructor.
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
			    bool offsetTempSelect   = false,
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
     * This sets the actual voltage on the 2.5V.
     */
    Fed9UVoltageControlInfo& setActual2Point5Volt(float voltage);

    /**
     * This returns the actual voltage on the 2.5V.
     */
    float getActual2Point5Volt() const;

    /**
     * This sets the actual voltage on the 3.3V.
     */
    Fed9UVoltageControlInfo& setActual3Point3Volt(float voltage);

    /**
     * This returns the actual voltage on the 3.3V.
     */
    float getActual3Point3Volt() const;

    /**
     * This sets the actual voltage on the 5V.
     */
    Fed9UVoltageControlInfo& setActual5Volt(float voltage);

    /**
     * This returns the actual voltage on the 5V.
     */
    float getActual5Volt() const;

    /**
     * This sets the actual voltage on the 12V.
     */
    Fed9UVoltageControlInfo& setActual12Volt(float voltage);

    /**
     * This returns the actual voltage on the 12V.
     */
    float getActual12Volt() const;

    /**
     * This sets the actual voltage on the processore core voltage.
     */
    Fed9UVoltageControlInfo& setActualCoreVoltage(float voltage);

    /**
     * This returns the actual voltage on the processore voltage.
     */
    float getActualCoreVoltage() const;

    /**
     * This sets the actual voltage on the 3.3V supply voltage.
     */
    Fed9UVoltageControlInfo& setActualSupplyVoltage(float voltage);

    /**
     * This returns the actual voltage on the 3.3V supply voltage.
     */
    float getActualSupplyVoltage() const;

    /**
     * This sets the current external temperature.
     */
    Fed9UVoltageControlInfo& setActualExternalTemp(i16 temp);

    /**
     * This returns the current external temperature.
     */
    i16 getActualExternalTemp() const;

    /**
     * This sets the actual internal temperature.
     */
    Fed9UVoltageControlInfo& setActualInternalTemp(i16 temp);

    /**
     * This returns the actual internal temperature.
     */
    i16 getActualInternalTemp() const;

    /**
     * This sets the manufactures ID.
     */
    Fed9UVoltageControlInfo& setManufacturesId(u32 manufacturesId);

    /**
     * This returns the manufactures ID.
     */
    u32 getManufacturesId() const;

    /**
     * This sets the stepping ID.
     */
    Fed9UVoltageControlInfo& setSteppingId(u32 steppingId);

    /**
     * This returns the stepping ID.
     */
    u32 getSteppingId() const;

    /**
     * This sets all the settings in the status register. The enum Fed9UVoltageStatus can be
     * used to set the flags for the voltage readings.
     *
     * A bitwise OR can be used to set multiple flag. Below are a list of the flags that can
     * be set and their meaning.
     * 2.5V error. A true signfies that the 2.5V has exceeded its limits.
     * Processor core voltage error. A true signfies that the processor core voltage
     * has exceeded its limits.
     * 3.3V error. A true signfies that the 3.3V has exceeded its limits.
     * 5V error. A true signfies that the 5V has exceeded its limits.
     * Internal temperature error. A true signfies that the internal temperature
     * has exceeded its limits.
     * External temperautre error. A true signfies that the external temperature
     * has exceeded its limits.
     * 12V error. A true signfies that the 12V has exceeded its limits.
     * Supply voltage error. A true signfies that the supply voltage has exceeded its limits.
     * Remote diode fault. A true indicates that the remote device, whos temperature is
     * being measured on the external temperature register is no longer connected.
     */
    Fed9UVoltageControlInfo& setStatusRegister(u16 statusRegister);

    /**
     * The returned u16 can be masked with the Fed9UVoltageStatus enum to check which of the
     * voltage limits have been exceeded.
     *
     * Below is a list of the errors that can be checked.
     * 2.5V error. A true signfies that the 2.5V has exceeded its limits.
     * Processor core voltage error. A true signfies that the processor core voltage
     * has exceeded its limits.
     * 3.3V error. A true signfies that the 3.3V has exceeded its limits.
     * 5V error. A true signfies that the 5V has exceeded its limits.
     * Internal temperature error. A true signfies that the internal temperature
     * has exceeded its limits.
     * External temperautre error. A true signfies that the external temperature
     * has exceeded its limits.
     * 12V error. A true signfies that the 12V has exceeded its limits.
     * Supply voltage error. A true signfies that the supply voltage has exceeded its limits.
     * Remote diode fault. A true indicates that the remote device, whos temperature is
     * being measured on the external temperature register is no longer connected.
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
   * This overload of operator<< can used to write a Fed9UVoltageControlInfo to an ostream.
   */
  ostream& operator<<(ostream & os, const Fed9UVoltageControlInfo& data);

  /**
   * This overload of operator>> can used to read a Fed9UVoltageControlInfo from an istream.
   */
  istream& operator>>(istream & is, Fed9UVoltageControlInfo& data);
  
}// End of Fed9U namespace.
#endif// H_Fed9UVoltageControlInfo.
#ifndef H_Fed9UEpromDescription
#define H_Fed9UEpromDescription

//Author: Gareth Rogers
//Date  : April 2004

/**
 * A class that provides the ability to configure the EPROM on the FED.
 *
 * Fed9UEpromDescription contains 2KB of memory that can be formatted as required
 * by the FED. This class has no knowledge however of how the write protected areas
 * of the FED are setup. It assumes all is read/writable, however this is not the
 * case on the FED.
 */


namespace Fed9U {

  using std::istream;
  using std::ostream;

  class Fed9UEpromDescription {

    /**
     * Some public member functions that provide access to the ability to create an new EPROM
     * object (the constructor) and to allow that data to be accessed and written to.
     */
  public:

    /**
     * The Fed9UEpromDescription constructor. It takes a pointer to a 2KB block of memory of
     * which a local copy is made. The data pointer is defaulted to a null pointer. A blank 2KB
     * of memory will be created.
     */
    explicit Fed9UEpromDescription(u32 version, const u8* const data = 0);

    /**
     * This allows you write to a single byte of memory.
     *
     * The offset is a number between 0 and 0x7ff and represents a byte of memory in the EPROM.
     */
    Fed9UEpromDescription& setSingleByte(u32 offset, u8 data);

    /**
     * This method will set the FED serial number.
     */
    Fed9UEpromDescription& setFedSerialNumber(u32 serialNumber);

    /**
     * This method will return the FED serial number.
     */
    u32 getFedSerialNumber() const;

    /**
     * This allows you to read a single byte of memory.
     *
     * The offset is a number between 0 and 0x7fe and represents a byte of memory in the EPROM.
     */
    u8 getSingleByte(u32 offset) const;

    /**
     * An overloaded version of the operator[] which provides the same functionality
     * as the setEpromSingleByte.
     *
     * The offset is a number between 0 and 0x7fe and represents a byte of memory in the EPROM.
     */
    u8& operator[](u32 offset);

    /**
     * Another overloaded version of the operator[], this however is a read only version.
     *
     * The offset is a number between 0 and 0x7fe and represents a byte of memory in the EPROM.
     */
    u8 operator[](u32 offset) const;

    /**
     * Returns the size of the EPROM memory in bytes.
     */
    u32 getEpromSize() const;
    
    /**
     * Returns the version of the eprom.
     */
    u32 getEpromVersion() const;

     /**
     * sets the version of the eprom.
     */
    Fed9UEpromDescription& setEpromVersion(const u32 version);

     
    /**
     * Some private data members. Used to set the size of the EPROM and to provide an area
     * of memory to store the EPRM data and to define which areas are read only.
     */
  private:
    /**
     * This sets the size of the EPROM in bytes. The useable EPROM memory is actually 1 byte
     * less than the total size as the last byte of the EPROM is used to set the write
     * protection level and cannot be used for storage.
     */
    static const u32 epromSize_ = 2048;
    //This holds the EPROM memory, which is set to be as large as the eprom size.
    u8 epromData_[epromSize_];
    u32 epromVersion_;
  };// End of class Fed9UEpromDescription

  /**
   * This overload of operator<< can used to write a Fed9UEpromDescription to an ostream.
   */
  ostream& operator<<(ostream& os, const Fed9UEpromDescription& data);

  /**
   * This overload of operator>> can used to read a Fed9UEpromDescription from an istream.
   */
  istream& operator>>(istream& is, Fed9UEpromDescription& data);

}// End of Fed9U namespace
#endif// H_Fed9UEpromDescription
#ifndef H_Fed9UABC
#define H_Fed9UABC

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

#include "ICExImpl.hh"

#include <string>
#include <vector>
#include <iosfwd>



namespace Fed9U {

using std::string;

  /**
   * Abstract Base Class to a generic FED.
   *
   * This base class sets the API for both the Fed9UDescription and the Fed9UVmeDevice
   * 
   *********************************************/

  class Fed9UABC {

    /**
     * This first set of methods are required by both Fed9UDescription and Fed9UVmeDevice
     *
     **********************************************/
  public:
    virtual ~Fed9UABC() {}
    virtual Fed9UABC& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) = 0;
    virtual Fed9UABC& setFineDelay(const Fed9UAddress& fedChannel, u16 value) = 0;
    virtual Fed9UABC& setCoarseDelay(const Fed9UAddress& fedChannel, u16 value) = 0;
    virtual Fed9UABC& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 value) = 0;
    virtual Fed9UABC& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 value) = 0;
    virtual Fed9UABC& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 value) = 0;
    virtual Fed9UABC& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 value) = 0;
    virtual Fed9UABC& setBaseAddress(const u32) { ICUTILS_VERIFY(false).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    virtual Fed9UABC& setApvDisable(const Fed9UAddress& fedApv, bool disable) = 0;
    virtual Fed9UABC& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, vector<u16> medianOverride) = 0;
    virtual Fed9UABC& setScopeLength(u16 numberOfWords) = 0;
    virtual Fed9UABC& setFrameThreshold(const Fed9UAddress& channel, u16 channelThreshold) = 0;
    virtual Fed9UABC& setTriggerSource(Fed9UTrigSource triggerSource) = 0;    
    virtual Fed9UABC& setTestRegister(u32 testRegister) = 0;
    virtual Fed9UABC& setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable) = 0; 
    virtual Fed9UABC& setFedBeFpgaDisable(bool fedDisable) = 0;
    virtual Fed9UABC& setFedId(u16 fedId) = 0;
    virtual Fed9UABC& setFedHardwareId(u32 fedId) = 0;
    virtual Fed9UABC& setFeFirmwareVersion(u32 version) = 0;
    virtual Fed9UABC& setBeFirmwareVersion(u32 version) = 0;
    virtual Fed9UABC& setVmeFirmwareVersion(u32 version) = 0;
    virtual Fed9UABC& setDelayFirmwareVersion(u32 version) = 0;
    virtual Fed9UABC& setEpromVersion(u32 version) = 0;
    virtual Fed9UABC& setFedVersion(u32 version) = 0;
    virtual Fed9UABC& setBeFpgaReadRoute(Fed9UReadRoute readRoute) = 0;
    virtual Fed9UABC& setAdcControls(const Fed9UAddress& channel, const Fed9UAdcControls& adcControls) = 0;
    virtual Fed9UABC& setDaqMode(Fed9UDaqMode fed9UDaqMode) = 0;
    virtual Fed9UABC& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode) = 0;
    virtual Fed9UABC& setClock(Fed9UClockSource clockSelect) = 0;
    virtual Fed9UABC& setTempControl(const Fed9UAddress& fpga, const Fed9UTempControl& tempControl) = 0;
    virtual Fed9UABC& setComplement(const Fed9UAddress& channel, bool disable) = 0;
    virtual Fed9UABC& setCrateNumber(u16) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    virtual Fed9UABC& setSlotNumber(u8){ ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    virtual u16 getFineDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }
    virtual u16 getCoarseDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }
    virtual u16 getTrimDacOffset(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}
    virtual u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const = 0;
    virtual u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const = 0;
    virtual u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const = 0;
    virtual u32 getBaseAddress() { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}
    virtual bool getApvDisable(const Fed9UAddress& fedApv) const = 0;
    virtual u32 getAllApvDisables(const Fed9UAddress&) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0;}
    virtual vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const = 0;
    virtual u16 getScopeLength() const = 0;
    virtual u16 getFrameThreshold(const Fed9UAddress& channel) const = 0;
    virtual Fed9UStrips & getFedStrips() = 0;
    //    virtual const Fed9UStrips & getFedStrips() = 0;
    virtual Fed9UTrigSource getTriggerSource() const = 0;
    virtual u32 getTestRegister() const = 0;
    virtual bool getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const = 0;
    virtual bool getFedBeFpgaDisable() const = 0;
    virtual u16 getFedId() const = 0;
    virtual u32 getFedHardwareId() const = 0;
    virtual u32 getFeFirmwareVersion() const = 0;
    virtual u32 getBeFirmwareVersion() const = 0;
    virtual u32 getVmeFirmwareVersion() const = 0;
    virtual u32 getDelayFirmwareVersion() const = 0;
    virtual u32 getFedVersion() const = 0;
    virtual u32 getEpromVersion() const = 0;

    virtual Fed9UReadRoute getBeFpgaReadRoute() const = 0;
    virtual Fed9UAdcControls getAdcControls(const Fed9UAddress&) const = 0;
    virtual Fed9UDaqMode getDaqMode() const = 0;
    virtual Fed9UDaqSuperMode getDaqSuperMode() const = 0;
     virtual Fed9UClockSource getClock() const = 0; 
    virtual Fed9UTempControl getTempControl(const Fed9UAddress& fpga) const = 0;
    virtual bool getComplement(const Fed9UAddress& channel) const = 0;
    virtual u16 getCrateNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; } 
    virtual u8 getSlotNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }
     
    virtual Fed9UABC& setName(string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this; }

    virtual string getName() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return string(" "); }
    virtual string getVersion() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return string(" "); }
    virtual Fed9UABC & setHalAddressTable(const string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    virtual const string & getHalAddressTable() const = 0;
    virtual u32 getRequiredBufferSize() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }

    virtual Fed9UABC& getFed9UVMEDeviceDescription() = 0;


    //Methods specific to the VMEDevice
    
    virtual Fed9UABC& init() {return *this;}
    virtual Fed9UABC& initStrips() { return *this;}


    //Methods specific to the Description
    virtual void loadDescription(istream&) {}
    virtual void saveDescription(ostream&) const {}
    virtual void loadDefaultDescription(){}

  };
  
  
}

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
  using std::vector;
  class Fed9UABC;

  /**
   * Map class to map from an index to an array of abc pointers.
   *
   * 
   *********************************************/

  class Fed9UGuiAbcMap {
  public:
    Fed9UGuiAbcMap();
    ~Fed9UGuiAbcMap() ;

    void addEntry() ;
    void deleteEntry(u32 index) ;
    void addMapping( u32 index, Fed9UABC* ptr ) ;
    void deleteMapping( u32 index, Fed9UABC* ptr ) ;
    void clear() ;
    vector<Fed9UABC*>& getMap(u32 index) ;
    u32 size() { return fedAbcMap.size(); }
  private:
    vector< vector <Fed9UABC*> > fedAbcMap;
  };
}
#endif // H_Fed9UGuiAbcMap
#ifndef H_Fed9UDescription
#define H_Fed9UDescription

/////////////////////////////////////////////////////////////////
//                                                             //
//   Fed 9U Description Class   : Fed9UDescription             //
//                                                             //
//  Author: Matt Noy, Jonathan Fulcher, Emlyn Corrin  IC       //
//  date: 19/06/2003                                           //
//  notes:                                                     //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////



#include <string>
#include <vector>
#include <iosfwd>

// This version number must be incremented whenever
// a change is made to the description strage format:
#define DESCRIPTION_VERSION 14

#define VERSION_TEXT(name) ("Fed9U" #name "Version" STRINGIZE(DESCRIPTION_VERSION))

namespace Fed9U {

  using std::vector;
  using std::string;
  using std::istream;
  using std::ostream;

  /**
   * Description of a whole FED.
   *
   * The Fed Configuration is contained within the class Fed9UDescription. 
   * This class contains all data which is required in order to configure the FED
   * for normal running. Only those registers in the FED who's settings persist during 
   * run time are included here. Registers such as Status and counters which 
   * are constantly changing are not included. The basic concept is that whatever is 
   * set in the FedDescription that is then passed to the Fed9UVmeDevice, will mirror
   * what is in the FED after it has been initialized with this object.
   * Therefore during run time, the FED should always contain settings which match a local
   * copy of the FED Description. Unless an register has been explicitly changed via some 
   * external interface.  therefore It is recommended that you maintain a local copy of the
   * FED description whenever you instantiate a FED9UVMEDevice. And make sure that it 
   * always mirrors the settings in the FED proper. To do this you should adopt an 
   * approach whereby the Settings that you wish to edit on the fed are first set in the
   * local description, and then transferred to the FED, either by individually writing to
   * the specific register, or by uploading the entire description to the FED.
   *
   * 
   *********************************************/

  class Fed9UDescription  : public Fed9UABC {
  public:
    
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
    
    
    /**
     * This method loads the Fed description from a stream that is passed into the method.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * description to the source of the stream using saveDescription(ostream&).
     ********************************************/
    void loadDescription(istream&);

    /**
     * This method loads the Fed settings, which are a subsection of the fed description
     * from a stream that is passed into the method.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * settings to the source of the stream using saveSettings(ostream&).
     ********************************************/
    void loadSettings(istream&);

    /**
     * This method loads the strips from a stream that is passed into the method.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * strips to the source of the stream using saveStrips(ostream&).
     ********************************************/
    void loadStrips(istream&);
    
    /**
     * This method saves the Fed description to a stream that is passed into the method.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * description passed into loadDescription(istream&).
     ********************************************/
    void saveDescription(ostream&) const;


    /**
     * This method saves the Fed Settings to a stream that is passed into the method.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * Settings passed into loadSettings(istream&).
     ********************************************/
    void saveSettings(ostream&) const;
    

    /**
     * This method saves the Strips to a stream that is passed into the method.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * strips passed into loadStrips(istream&).
     ********************************************/
    void saveStrips(ostream&) const;
    
    /** 
     * This method loads a default set of values into the fed description. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(istream&) method
     ********************************************/
    void loadDefaultDescription() { loadDefaultSettings(); loadDefaultStrips(); }
    
    /** 
     * This method loads a default settings. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(istream&) method
     ********************************************/
    void loadDefaultSettings();
    
    /** 
     * This method loads a default set of strips values.
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(istream&) method
     ********************************************/
    void loadDefaultStrips();
    
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
     * Sets the fine delay for each FED channel.
     *
     * The fine delay has a maximum value of 32. A fine delay of 1 corresponds
     * approximately to a 0.78ns delay.
     */
    Fed9UDescription& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay);
    
    /**
     * This method sets the fine delay of the sampling clock relative to the FED system clock on a
     * per-channel basis in units of 1/32 of a clock cycle
     ********************************************/    
    Fed9UDescription& setFineDelay(const Fed9UAddress& fedChannel, u16 value);
    
    /**
     * This method sets the coarse delay of the sampling clock relative to the FED trigger on a
     * per-channel basis, in units of whole clock cycles
     ********************************************/    
    Fed9UDescription& setCoarseDelay(const Fed9UAddress& fedChannel, u16 value);
    
    
    /**
     * This method sets the offset generated by the TrimDAC which shifts the input to the ADC on a
     * per-channel basis.
     *
     * The maximum value the offset can take is 255.
     */
    Fed9UDescription& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 value);
    
    
    /**
     * Set the OptoRx input offset.
     *
     * This has a maximum value of 15.
     */
    Fed9UDescription& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 value);
    
    Fed9UDescription& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 value);
    
    Fed9UDescription& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 value);
    
    Fed9UDescription& setBaseAddress(const u32 value) { _baseAddress = value; return *this; }
    
    Fed9UDescription& setApvDisable(const Fed9UAddress& fedApv, bool disable);
    
    Fed9UDescription& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, vector<u16> medianOverride);
    
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
    Fed9UDescription& setDaqMode(Fed9UDaqMode fed9UDaqMode);
    Fed9UDescription& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode);
    Fed9UDescription& setClock(Fed9UClockSource clockSelect);
    Fed9UDescription& setTempControl(const Fed9UAddress& fedFpga, const Fed9UTempControl& tempControl);
    
        //this is a write only command and so description is the only way to see what the current setting is
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
     *This method sets the Crate number in the description which represents the number of the crate in which the FED is inserted.
     *****************************************************/
    Fed9UDescription& setCrateNumber(u16 crateNumber);

    /**
     * This method sets the slot number in which the fed should be located, based upon what the Base address is set to.
     *************************************/
     Fed9UDescription& setSlotNumber(u8 slotNumber);


    /**
    * Getters for Fed9UDescription
    *****************************************/
     u16 getFineDelay(const Fed9UAddress& fedChannel) const;
     u16 getCoarseDelay(const Fed9UAddress& fedChannel) const;
     u16 getTrimDacOffset(const Fed9UAddress& fedChannel) const;
     u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const;
     u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const;
     u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const;
     u32 getBaseAddress() const { return _baseAddress; }
     bool getApvDisable(const Fed9UAddress& fedApv) const;
     u32 getAllApvDisables(const Fed9UAddress& fedFeUnit) const;

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
    
    vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const;

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

    /**
     * This method gets the crate number from the description, which represents the number of the crate in which the fed is inserted.
     *************************************/
     u16 getCrateNumber() const; 
    
    /**
     * This method gets the slot number in which the fed should be located, based upon what the Base address is set to.
     *************************************/
     u8 getSlotNumber() const;

    /**
     *  Functions to be added. 20th October. 
     *
     * Do not use \deprecated Do not use these functions, 
     * they don't do anything, just allows the build to succeed 
     */
    

    
    //enumNamehere represents the register address but it is currently unknown what they are. this is a BE Fpga command only and so does not need a Fed9UAddress
    //void setTTCRXControl(u32 enumNameHere, u32 timingData) {}
    //u32 getTTCRXControl(u32 enumNameHere) {return 0;}
    
    Fed9UDescription& setName(string name);

     string getName() const { return _name;}
     string getVersion() const { return string(VERSION_TEXT(Description)); }
   /** End of new functions */
    
     Fed9UDescription & setHalAddressTable(const string val);

     const string & getHalAddressTable() const;

    /**
     * 
     * Returns maximum possible event buffer size required for Fed9UDevice::getCompleteEvent()
     * 
     * Uses the current description values to esimate what this should be
     *
     * J. Fulcher, M. Noy
     * 18-03-2004
     */
  u32 getRequiredBufferSize() const;
      


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
     * setter, see setOptoRxResistor(). sets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    string getFakeEventFile() const {
      return _fakeEventFile;
    }
    
    /**************************************************
     *
     * setter, see setOptoRxResistor(). sets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    inline Fed9UDescription & setFakeEventFile(string value){
      _fakeEventFile=value;
      return *this;
    }
  private:
    //these methods are deprocated and will be removed soon
    u16 getAdcOffset(const Fed9UAddress& channel) const;
    Fed9UDescription& setAdcOffset(const Fed9UAddress& channel, u16 offset);
    Fed9UDescription& setChannelBufferOccupancy(const Fed9UAddress& channel, u16 channelOccupancy);
    u16 getChannelBufferOccupancy(const Fed9UAddress& channel) const;

  private:
    static string _fed9UAddressTable;
    template <typename T>
    void doLoop(Fed9UDescription & (Fed9UDescription::*setter)(const Fed9UAddress &, T), T value) {      string temp;

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
    string _name;
    u16 _crateNumber;
    u16 _globalFineSkew;
    u16 _globalCoarseSkew;
    u16 _optoRXResistor;
    string _fakeEventFile;
  };
  
  
}

#endif // H_Fed9UDescription
#ifndef Fed9UEntry_HH
#define Fed9UEntry_HH

#include <vector>
#include <iostream>
#include <iomanip>

#define UNSET_SN 0
#ifdef TBHARDWARE
#endif

namespace Fed9U
{

  //
  // Fed9UEntry: class to represent and identify a fed in the crate
  // simple container for the serial number and base address.
  //
  
  //
  // Author: Matt Noy
  // Date: April 2004
  //
  
  //
  // 18-05-2004
  // Fed9UABC pointer added to enable the container to keep
  // the pointers to the Fed9UABC representing real hardware
  //

 
  
  
  class Fed9UEntry
  {
  public:
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
  
  ~Fed9UEntry()
  {
  }
  
    inline unsigned long getBaseAddress() const {return mBA;}
    inline unsigned long getSerialNumber() const {return mSerialNo;}
    inline unsigned long getSlotNumber() const {return mSlotNumber;}    
    inline bool getTestFlag() const {return mTestMe;}    
    inline bool getMaxLM82Temp() const {return mMaxLM82Temp;}    

    inline void setBaseAddress(unsigned long ba){mBA=ba;}
    inline void setSerialNumber(unsigned long sn){mSerialNo=sn;}
    inline void setTestFlag(bool test){mTestMe=test;}
    inline void setSlotNumber(unsigned long slot){mSlotNumber=slot;}    
    inline void setMaxLM82Temp(unsigned long temp){mMaxLM82Temp=temp;}    

#ifdef TBHARDWARE
  inline Fed9U::Fed9UABC * getFed9UABC(){return mpFed;}
  inline void setFed9UABC(Fed9U::Fed9UABC * f){mpFed=f;}
  inline void purge() { delete mpFed; }
#endif

    //
    // assumes a zero serial number will never exist...
    //
    inline bool isSerialNumberSet(){return mSerialNo!=0;}

    inline void display() const {
      std::cout<<"Serial No = "<<std::hex<<mSerialNo
	       <<", BA = "<<std::hex<<mBA
	       <<", test = "<<std::dec<<mTestMe
	       <<std::endl;
    }
private:
  unsigned long mSerialNo;
  unsigned long mBA;
  unsigned long mSlotNumber;
  bool mTestMe;
  u32 mMaxLM82Temp; 
#ifdef TBHARDWARE
    Fed9UABC * mpFed;
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
// Fed9UCrateStatus: container class for the Fed9UEntry
// 
// abstraction of a vector that should
// provide the ability to detect the crate,
// and iterate the testing sequence over 
// the crate entries.
//  

//
// Author: Matt Noy
// Date: April 2004
//

//
// 18-05-2004
// public member prependEntry() added, to prepend a Fed9UEntry;
//


  class Fed9UCrateStatus
  {
  public:
    Fed9UCrateStatus();
    ~Fed9UCrateStatus();
    

    //
    // returns a reeference to the Fed9UEntry at 
    // position pos
    //
    Fed9UEntry & getFed9UEntry(unsigned long pos);

    //
    // returns true if there is a next entry, and 
    // false if not. if so, f is a valid reference 
    // to that entry, otherwise it isn't.
    //
    // skips Fed9UEntry objects that have the 
    //
    bool getNextFed9UEntry(Fed9UEntry & f);
    
    //
    // returns the number of Fed9UEntry objects in the sequence.
    //
    inline unsigned long getNumberOfEntries(){return mFeds.size();}
   
    //
    // returns true if class is empty, false otherwise.
    // 
    inline bool isEmpty(){return mFeds.empty();}
    
    //
    // places the Fed9UEntry at the end of the sequence.
    // note: this function resets the internal iterator afterwards.
    //
    void appendFed9UEntry(Fed9UEntry f);

    //
    // places Fed9UEntry (f) at the start of the sequence
    //
    void prependFed9UEntry(Fed9UEntry f);

    //
    // resets the internal iterator to the start of the sequence
    //
    void resetIterator();

    //
    // purges the contents of the object
    //
    void purge();

    //
    // calls Fed9UEntry::display on each element
    //
    void display() const;
   
    //
    // checks that each serial number is unique, unless unset.
    //
    bool areDuplicateSerialNumbers();

    //
    // checks if any feds in the crate have the sn unset
    //
    bool areUnsetSerialNumbers();

   
  protected:
    void init();

  protected:
    
    std::vector<Fed9UEntry> mFeds;
    std::vector<Fed9UEntry>::const_iterator mFedIter;

    static std::vector<unsigned long> mSlotBAMap;
  }; // class Fed9UCrateStatus

} // namespace Fed9UTB

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
using std::ostream;

#include <xercesc/util/XMLString.hpp>
using xercesc::XMLString;

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class Fed9UStrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    Fed9UStrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~Fed9UStrX()
    {
        delete [] fLocalForm;
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline ostream& operator<<(ostream& target, const Fed9UStrX& toDump)
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

class Fed9UDOMCountErrorHandler : public DOMErrorHandler
{
public:
    Fed9UDOMCountErrorHandler();
    ~Fed9UDOMCountErrorHandler();

    bool getSawErrors() const;

    bool handleError(const DOMError& domError);
    void resetErrors();


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    Fed9UDOMCountErrorHandler(const Fed9UDOMCountErrorHandler&);
    void operator=(const Fed9UDOMCountErrorHandler&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
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

  using xercesc::DOMNode;
  using xercesc::DOMNamedNodeMap;
  using xercesc::DOMDocument;
  using xercesc::DOMInputSource;
  using xercesc::DOMBuilder;

  using std::istream;
  using std::string;

class Fed9UXMLDescription
{

public:
  /**Constructor. This takes as arguments the XML file name and the Fed9UDescription to be modified.*/
  Fed9UXMLDescription(const string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Constructor. This takes in the Fed9UDescription to be modified. This is the constructor that should be used by derived classes.*/
  Fed9UXMLDescription(Fed9UDescription &defaultFed9UDescription , vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Destructor. This does some clean up.*/
  virtual ~Fed9UXMLDescription();

  /**Method which performs the Fed9UDescription override. It modifies the object member reference to the Fed9UDescription. 
     This method should only be used if the user has used the constructor which takes in a file name.*/
  vector<Fed9UDescription*>* makeNewFed9UDescription(void) throw (Fed9UXMLDescriptionException);

  /**Method which performs the Fed9UDescription overide. 
     It takes in a istream, containing the XML buffer, and modifies the object member reference to the Fed9UDescription.
     This method should be used by derived classes which use the stream interface to this class.*/
  vector<Fed9UDescription*>* makeNewFed9UDescriptionFromStream(istream &is) throw (Fed9UXMLDescriptionException);

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
  void traverseChildElements(DOMNode *n) throw (Fed9UXMLDescriptionException);

  /**Method which reads in a node, finds if it has attributes and sets these in the Fed9UDescription.*/
  void setFed9UDescription(DOMNode *n) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the fed9U node.*/
  void setFed9UAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the feFpga node.*/
  void setFeFpgaAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the channel node.*/
  void setChannelAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
  
  /**Method which will set the attributes for the channel node.*/
  void setApvAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method which will set the attributes for the strip node.*/
  void setStripAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);
  
  /**Method which will set the attributes for the strips node.*/
  void setStripsAttribute(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException);

  /**Method to get the integer node value from a DOMNamedNodeMap*/
  unsigned long getIntNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the signed integer node value from a DOMNamedNodeMap*/
  int getSignedIntNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap,  string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the float node value from a DOMNamedNodeMap*/
  float getFloatNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);

  /**Method to get the string node value from a DOMNamedNodeMap*/
  string getStringNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException);
  
  
  DOMDocument *doc;
  DOMBuilder *theDOMBuilder;
  Fed9UDOMCountErrorHandler errorHandler;

  string theXMLOverideFile;
  Fed9UAddress theFed9UAddress;
  Fed9UDescription &theFed9UDescription;
  vector<Fed9UDescription*> * theFed9UDescriptionList_; // this vector will contain the descriptions if there are more than one.
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

  using std::string;

  class Fed9UXMLDescriptionFromFile : public Fed9UXMLDescription
  {
  public:
    /**Constructor. This takes as arguments the XML file name and a reference to the Fed9UDescription to be modified.*/
    Fed9UXMLDescriptionFromFile(const string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, vector<Fed9UDescription*>* desclist = NULL, bool usingStrips=true);

    /**Method which modifies the Fed9UDescription. 
     It modifies the object member reference to the Fed9UDescription.*/
    //    vector<Fed9UDescription*>*  makeNewFed9UDescription(void) throw (Fed9U::Fed9UXMLDescriptionException);

  private:
    string theXMLFileName;

  };

}

#endif
#endif

#endif //_Fed9UXMLDescriptionFromFile_H_
/**Matthew Pearson, 17 Feb 2004.
Class which will write a Fed9UDescription to stream in XML format.
This class uses the Xerces 2.3 library. The input to the constructor is 
a reference to a Fed9UDescription. This is then converted to an XML format
stream using the streamOutDescription method. The XML stream will conform to
the XML schema for the Fed9USoftware. This class is intended to be a base class - a derived 
class can implement a file/database/etc interface to the stream interface provided by this class.*/

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

  using xercesc::DOMDocument;
  using xercesc::DOMWriter;
  using xercesc::DOMElement;

  using std::string;

class Fed9UDescriptionToXml
{
public:
  /**Constructor. This takes the file name of the XML file to be written, and a reference to the 
     Fed9UDescription from which to make the XML file. This constructor can be used to directly create
     an XML file.*/
  Fed9UDescriptionToXml(const string &targetXMLFileName, const Fed9UDescription &Fed9UDescriptionToWrite, bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Constructor. This takes in a referecne to the Fed9UDescription from which to make the XML file.*/
  Fed9UDescriptionToXml(const Fed9UDescription &Fed9UDescriptionToWrite , bool usingStrips = true) throw (Fed9UXMLDescriptionException);

  /**Destructor.*/
  virtual ~Fed9UDescriptionToXml();

  /**Method which converts the Fed9UDescription to an XML buffer stream. This must be called after using the constructor to this class which only takes in a reference to a Fed9UDescription.*/
  void streamOutDescription(ostream * os) throw (Fed9UXMLDescriptionException);

  /**Method which converts the Fed9UDescription to an XML file. This must be called after using the file based constructor to this class.*/
  void writeXmlFile(void) throw (Fed9UXMLDescriptionException);

  /**Method to get the value of usingStrips_*/
  bool getUsingStrips();

  /**Method to set the value of usingStrips_*/
  void setUsingStrips(bool usingStrips=true);

  /**Method to set the value of debugOutput_*/
  void setDebugOutput(bool value=true);

protected:
  /**Method which constructs the DOM document.*/
  virtual void constuctDOMDocument(void) throw (Fed9UXMLDescriptionException);

  /**Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.*/
  void initializeXerces(void) throw (Fed9UXMLDescriptionException);

  /**Method which adds elements to a DOMDocument using the Fed9UDescription to get the attribute values.*/
  virtual void addDOMElements(DOMElement *rootElement) throw (Fed9UXMLDescriptionException);

  /**Method which extracts the fed parameters from the Fed9UDescription and add them as element attributes.*/
  void addFedAttributes(DOMElement* childElement) throw (Fed9UXMLDescriptionException);

  /**Method which extracts the feFpga parameters from the Fed9UDescription and add them as element attributes.
     The second parameter is the FE id number (0-7)*/
  void addFeFpgaAttributes(DOMElement* childElement, unsigned short feId=0) throw (Fed9UXMLDescriptionException);

  /**Method which extracts the channel parameters from the Fed9UDescription and add them as element attributes.
     The second parameter is the channel ID number (0-11)*/
  void addChannelAttributes(DOMElement* childElement, unsigned short channelId=0) throw (Fed9UXMLDescriptionException);

  /**Method which extracts the APV parameters from the Fed9UDescription and add them as element attributes.
     The second parameter is the APV ID number (0-1).*/
  void addApvAttributes(DOMElement* childElement, unsigned short apvId=0) throw (Fed9UXMLDescriptionException);

  /**Method which extracts the strip parameters from the Fed9UDescription and adds them as element attributes.
     The second parameter is the strip ID number (0-127).*/
  void addStripAttributes(DOMElement* childElement, unsigned short stripId=0) throw (Fed9UXMLDescriptionException);
  
  //********************************************************************************************************
  //Method which extracts the strip parameters from the Fed9UDescription and adds them as a single element attribute.
  // this attribute called "data" contains a string representing a hex number which is 1024 characters long allowing 32 bits of info per strip
  void addStripsAttribute(DOMElement* childElement) throw (Fed9UXMLDescriptionException);


  DOMDocument *doc;
  DOMWriter *theDOMWriter;
  Fed9UDOMCountErrorHandler errorHandler; 
  string theTargetXMLFileName;
  const Fed9UDescription &theFed9UDescription;
  Fed9UAddress theFed9UAddress;
  bool usingStrips_;
  bool debugOutput_;
};

}

#endif

#endif

#endif
/*   Exception class for Fed9UEvent which inherits from ICException*/

#ifndef _Fed9UEventException_H_
#define _Fed9UEventException_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UEventException class to be used when throwing errors within the Fed9UEvent class.*/

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
/** Emlyn Corrin, Jonathan Fulcher, Matthew Pearson - March 2005 */


#ifndef H_Fed9UEvent
#define H_Fed9UEvent

#include "ICAssert.hh"
#include <iosfwd>
#include <vector>

namespace Fed9U {

  using std::ostream;
  using std::dec;
  using std::vector;
  using std::size_t;
  
  class Fed9UEvent;

  enum { FED9U_PACKET_SCOPE = 1, FED9U_PACKET_FRAME = 2, FED9U_PACKET_VIRGRAW = 4,
         FED9U_PACKET_ZEROSUPP = 8, FED9U_PACKET_PROCRAW = 16
  };

/** 
 * \brief Fed9UEventIterator class is designed to be used within the Fed9UEvent Class.
 * \author Jonathan Fulcher
 * \author Emlyn Corrin
 * \version 1
 * \date 15 March 2005
 */
  class Fed9UEventIterator {
  public:
    Fed9UEventIterator(u8 * buffer = 0, u32 length = 0, u32 offset = 0 )
      : _buf(buffer), _len(length), _off(offset) { }
    Fed9UEventIterator & set(u8 * buffer = 0, u32 length = 0, u32 offset = 0) {
      _buf = buffer; _len = length; _off = offset; return *this;
    }
    Fed9UEventIterator & resize(u32 length) { (*this) = normalize();  _len = length; return *this; }
    void clear() { if (_buf != NULL ) delete[] _buf; }
    u32 size() const { return _len; }
    Fed9UEventIterator normalize() const {
      u32 diff = _off & ~3;
      return Fed9UEventIterator(_buf + diff, _len, _off - diff);
    }
    u8 getu8(u32 offset) const {
      return (*this)[offset];
    }
    u16 getu16(u32 offset, bool swap = false) const {
      if (swap) return ((*this)[offset] << 8) |  (*this)[offset+1];
      else      return  (*this)[offset]       | ((*this)[offset+1] << 8);
    }
    u32 getu32(u32 offset, bool swap = false) const {
      if (swap) return (getu16(offset, swap) << 16) |  getu16(offset+2, swap);
      else      return  getu16(offset, swap)        | (getu16(offset+2, swap) << 16);
    }
    const u8 & operator * () const { return (*this)[0]; }
    const u8 & operator [] (u32 offset) const {
      ICUTILS_VERIFY(offset < _len)(offset)(_len).error();
      return _buf[(_off + offset) ^ 3];
    }
    friend i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs);
    //Fed9UEventIterator & operator ++ () { ++_off; return *this; }
    //const Fed9UEventIterator & operator ++ const () { ++_off; return *this; }
    //Fed9UEventIterator operator ++ (int) {
    //  Fed9UIterator temp(*this);
    //  _off++;
    //  return temp;
    //}
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
    Fed9UEventIterator & operator += (u32 rhs) {
      ICUTILS_VERIFY(rhs <= _len)(rhs)(_len).error();
      _off += rhs;
      _len -= rhs;
      return *this;
    }
    friend Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, u32 rhs);
    void debug(ostream & os) const {
      os << dec << reinterpret_cast<void*>(_buf) << '+' << _off << ':' << _len;
    }
  private:
    u8 * _buf;
    u32 _len, _off;
    bool _deleteBuffer;
  };

  inline i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs) {
    return (lhs._buf + lhs._off) - (rhs._buf + rhs._off);
  }
  
  inline Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, int rhs) {
    Fed9UEventIterator temp(lhs); return temp += rhs;
  }

  class Fed9UEventChannel {
  public:
    Fed9UEventChannel(Fed9UEventIterator data, u8 status);
    /*
    Fed9UDaqMode mode() const;
    int operator[] (size_t i) const {
      ICUTILS_VERIFY((i*2+1) < _data.size())(i*2+1)(_data.size())
        .msg("Index out of bounds").error();
      return _data[i*2] | _data[i*2+1] << 8;
    }
    */
    u8  getStatus() const { return _status; }
    u8  getPacketCode() const { return _data[2]; }
    u16 getMedian(unsigned apv) const {
      ICUTILS_VERIFY(apv <= 1)(apv).error();
      ICUTILS_VERIFY(getPacketCode() & FED9U_PACKET_ZEROSUPP)(getPacketCode()).error();
      return _data.getu16(3 + apv*2);
    }
    u32 dataLength() const { return _data.getu16(0); }
    u16 samples() const;
    u16 sample(size_t i) const;
    void getSamples(u16 * destBuff) const;
    vector<u16> getSamples() const;
    /** returns channel interator */ 
    inline const Fed9UEventIterator& getIterator() const {return _data;} // added by RB, 22/07/05
  private:
    u8 _status;
    Fed9UEventIterator _data;
  };

  class Fed9UEventUnit {
  public:
    Fed9UEventUnit(Fed9UEventIterator header, Fed9UEventIterator data);
    const Fed9UEventChannel & channel(size_t i) const {
      ICUTILS_VERIFY(i < _channels.size())(i)(_channels.size()).msg("Index out of bounds").error();
      return _channels[i];
    }
    u8  getMajorityPipeline() const { return _header.getu16(4); }
    u32 dataLength() const { return (_data.size()+7) & ~7; }
    u16 channels() const { return _channels.size(); }
    /**Read the middle 32-bit word in the 2nd 64-bit word of the FE header. This method returns
       the bytes 10-14 inclusive. It can be used to read the DAQ register for example, which is
       in this position in the 8th FE header in a full debug mode event.*/
    u32 getBytes10To14FromFeHeader(void) const {return (_header.getu32(10, true) & 0xFFFFFFFF); }
  private:
    Fed9UEventIterator _header, _data;
    vector<Fed9UEventChannel> _channels;
  };

  /**
   * \class Fed9UEvent
   * \brief ###BRIEF###DESCRIPTION###
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \version 0.0
   * \date 21 Aug 2003
   *
   * ###LONG###DESCRIPTION###
   ***********************************************/

  class Fed9UEvent {
  public:
    static void EnableDebug(bool on = true);
    Fed9UEvent() : _rubbish(0), _fedDescription(0), _deleteBuffer(false), _mode(FED9U_EVENT_MODE_REAL) {}
    
    Fed9UEvent(istream & is);
    
    Fed9UEvent(u32 * buffer, const Fed9UDescription * currentDescription,
               u32 bufsize = 0) : _deleteBuffer(false){
      Init(buffer, currentDescription, bufsize);
      _mode =  FED9U_EVENT_MODE_REAL;
    }
    void Init(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);
    void DebugInit(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize, string& report, u32& readCrc, u32& calcCrc);
    ~Fed9UEvent();
    void checkEvent() const;
    //void debugCRC() const;
    void checkChannelStatuses() const;
    Fed9UDaqMode getDaqMode() const;
    u32 getEventNumber() const;
    u8  getEventType() const;
    u16 getBunchCrossing() const;
    u16 getSourceId() const;
    u8  getFormatVersion() const;
    Fed9UEventDataMode getDataMode() const { return _mode ; }
    u16 getCrc() const;
    u16 calcCrc() const;
    u16 calcCrcDirty() const;

    /**Method to read the DAQ register from the 8th FE header in a full debug mode event.                                                
       It returns zero if there is no full debug mode event.*/
    u32 getDaqRegister(void) const;

    /**Method to read the BE status register from the 1st FE header in a full debug mode event. 
       It returns zero if the event is not a full debug mode event.*/
    u32 getBeStatusRegister(void) const;

    /**Methods to read the tracker special header. 
       These methods rely on the SPECIAL_OFF private data member to locate the 
       position of the tracker special header in the FED buffer.*/

    /**Read first 8 bytes of tracker special header (should be 0xED while old and new headers co-exist).
       This method will have to renamed or replaced when these bits are used for something else.*/
    u8 getSpecialFirstByte(void) const;

    /**Read the header format (4 bits) from the tracker special header.
       1=Full debug mode
       2=APV error mode*/
    u8 getSpecialHeaderFormat(void) const;

    /**Read the tracker event type (4 bits) from the tracker special header.
       This is the 'FED mode'. 
       1=Scope mode
       2=Virgin raw mode
       6=Processed raw mode
       10=Zero suppressed mode*/
    u8 getSpecialTrackerEventType(void) const;

    /**Read the APVE Address (8-bits) from the tracker special header.
       This is the APVE address sent to the FED over the TTC channel B. It should
       match the real address in the APV headers.*/
    u8 getSpecialApvEmulatorAddress(void) const;
      
    /**Read the APV address error (8-bits) from the tracker special header.
       Each bit returned by this method indicates if the APV address for that FE FPGA
       is the same as the APVE address. A 1 means they are identical, and a 0 means they are not.
       Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.*/
    u8 getSpecialApvAddressError(void) const;

    /**Read the FE enable register (8-bits) from the tracker special header.
       Each bit returned by this method indicates if the corresponding FE FPGA is
       enabled (able to accept data). A 1 means enabled and a 0 means disabled.
       This information is needed to decode the FED buffer when operating in APV error header mode.
       If set to 0 then there will be no payload data from that particular FE FPGA.
       Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.*/
    u8 getSpecialFeEnableReg(void) const;

    /**Read the FE overflow register (8-bits) from the tracker special header.
       If a bit is set to 1 then a buffer overflow has occured in the corresponding 
       FE FPGA, and the data from that FPGA should be ignored.
       Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.*/
    u8 getSpecialFeOverflowReg(void) const;

    /**Read the FED status register (16-bits) from the tracker special header.
       Each bit returned by this method indicates the state of a particular part of the FED. 
       Bit 0 = Slink full flag.
       Bit 1 = Tracker Header Monitor data ready.
       Bit 2 = QDR Memory full flag.
       Bit 3 = QDR Memory partial full flag.
       Bit 4 = QDR Memory empty flag.
       Bit 5 = L1A FIFO Full flag.
       Bit 6 = L1A FIFO Partial Full flag.
       Bit 7 = L1A FIFO Empty flag.*/
    u16 getSpecialFedStatusRegister(void) const;
      
      //Fed9UEventChannel & getChannel(Fed9UAddress);
      //Fed9UEventChannel & getChannel(int fedFeUnit, int feUnitChannel);
    void saveIgorFile(ostream & o) const;
    void dump(ostream&) const;
    void dumpRawBuffer(ostream& os) const;
    const Fed9UEventUnit & feUnit(size_t i) const {
      ICUTILS_VERIFY(i < feUnits())(i)(feUnits()).msg("Index out of bounds").error();
      return _feunits[i];
    }
    const Fed9UEventChannel & channel(size_t unit, size_t channel) const {
      return feUnit(unit).channel(channel);
    }
    u32 getTotalLength() const;
    u16 feUnits() const { return _feunits.size(); }
    u16 totalChannels() const {
      u16 ret = 0;
      for (int i = 0; i < feUnits(); i++) {
        ret += feUnit(i).channels();
      }
      return ret;
    }
    const Fed9UEventChannel & channel(u8 fedchannel) const {
      for (int i = 0; i < feUnits(); i++) {
        if (fedchannel < feUnit(i).channels()) return feUnit(i).channel(fedchannel);
        fedchannel -= feUnit(i).channels();
      }
      ICUTILS_VERIFY(false)(static_cast<int>(Fed9UAddress(fedchannel).getExternalFedChannel()))(totalChannels()).msg("Bad channel number").error();
      THROW(ICUtils::ICException("Bad channel number"));
    }

    const Fed9UEventChannel & channel( Fed9UAddress fedChannel ) const {
      return channel(fedChannel.getFedChannel());
    }

    u16 samples() const {
      return channel(0).samples();
      // should really check all channels are consistent
    }
  private:
    Fed9UEvent(const Fed9UEvent &);
    Fed9UEvent & operator = (const Fed9UEvent &);
    int getFeBufferLength(int feUnit) const;
    Fed9UEventIterator _buffer, _trailer;
    vector<Fed9UEventUnit> _feunits;
    u16 _rubbish;
    const Fed9UDescription * _fedDescription;
    static const u16 SPECIAL_OFF = 8; //Offset of tracker special header in bytes.
    bool _deleteBuffer;
    Fed9UEventDataMode _mode;
  };

  inline ostream& operator<<(ostream& os, const Fed9UEvent & ev) { ev.dump(os); return os; }

}

#endif // H_Fed9UEvent
#ifndef H_Fed9UBufferedEvent
#define H_Fed9UBufferedEvent

#include <iosfwd>
#include <vector>

namespace Fed9U {

  using std::ostream;
  
  /**
   * \class Fed9UBufferedEvent
   *
   * \author M. Noy
   * \version 0.0
   * \date 04-11-2003
   *
   * ###LONG###DESCRIPTION###
   *
   * designed to extend Fed9UEvent by adding 
   * an internal event buffer. 
   ***********************************************/

  class Fed9UBufferedEvent:public Fed9UEvent
  {
  public:
    Fed9UBufferedEvent();
    Fed9UBufferedEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize = 0);
    Fed9UBufferedEvent(istream & input, Fed9UDescription * FedDescriptionAtTheTime);    

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
    //void saveIgorFile(ostream & o) const;
    //void dump(ostream&) const;
  
    
    u32 writeBufferedEventToFile(ostream & output);
    u32 * getEventBufferPointer();
  private:
    u32 getEventFromFile(istream & in, Fed9UDescription * desc);
  public:
    //interface due to accident with name convention
    inline u32 getBufferedEventFromFile(istream & in, Fed9UDescription * desc){return getEventFromFile(in, desc);}
    
    void loadEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);

    Fed9UDaqMode getBufferedDaqMode() const {return _daqMode;}

  protected:
    
  private://member functions
    Fed9UBufferedEvent(const Fed9UBufferedEvent &);
    Fed9UBufferedEvent & operator = (const Fed9UBufferedEvent &);
    void chargeEventBuffer(u32 * buf, u32 bufLen);   

  private://data members
    vector <u32> _eventBuffer; 
    u32 _eventBufferLength;
    bool _isOnline;
    Fed9UDaqMode _daqMode;
    

  };
}

#endif //H_Fed9UBufferedEvent
#ifndef H_Fed9UCounters
#define H_Fed9UCounters

#include <iostream>

namespace Fed9U {

  struct Fed9UCounters {
    u32 triggerNumber, qdrFrameCount, qdrBufferLevel, qdrTotalFrameCount, feLevel;
    Fed9UCounters(u32 x0 = 0, u32 x1 = 0, u32 x2 = 0, u32 x3 = 0, u32 x4 = 0) :
      triggerNumber(x0), qdrFrameCount(x1), qdrBufferLevel(x2),
      qdrTotalFrameCount(x3), feLevel(x4)
    { }
    Fed9UCounters(const vector<u32> & x, u32 x4 = 0) :
      triggerNumber(x[0]), qdrFrameCount(x[1]), qdrBufferLevel(x[2]),
      qdrTotalFrameCount(x[3]), feLevel(x4)
    { }
    //Fed9UCounters & setFeLevel(int i, u32 val) { feLevel[i] = val; }
    bool operator==(const Fed9UCounters & rhs) const {
      return triggerNumber==rhs.triggerNumber && qdrFrameCount==rhs.qdrFrameCount
	&& qdrBufferLevel==rhs.qdrBufferLevel && qdrTotalFrameCount==rhs.qdrTotalFrameCount
	&& feLevel==rhs.feLevel;
    }
  };
  
  inline std::ostream & operator<<(std::ostream & os, const Fed9UCounters & fc) {
    return os << "Triggers=" << fc.triggerNumber
	      << ", QDR: Total events processed=" << fc.qdrTotalFrameCount
	      << " events in memory=" << fc.qdrFrameCount
	      << " data (64 bit words)=" << fc.qdrBufferLevel
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

  class Fed9ULockFileBase {
  public:
    bool isNewFile() const { return _newfile; }
  protected:
    Fed9ULockFileBase(const std::string & filename);
    virtual ~Fed9ULockFileBase();
    std::string readFileString() const;
    void writeFileString(const std::string &) const;
    int _fd;
    bool _newfile;
  };

  template <typename T>
  class Fed9ULockFile : public Fed9ULockFileBase {
  public:
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

    ~Fed9ULockFile() {
      sync();
    }

    T & value() { return _val; }
    const T & value() const { return _val; }
    operator T & () { return _val; }
    //operator const T & () const { return value(); }

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

  using std::string;

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
   */
  class Fed9ULog : public std::ofstream {
  public:
    Fed9ULog(const char* filename);
    ~Fed9ULog();
    Fed9ULog& stamp();
    static void new_unexpected();
  private:
    static string getTimeString();
    static string getDateString();
  };

  extern Fed9ULog log;

}

#endif // H_Fed9ULog
//Author: Gareth Rogers, RAL
//Date: May 2004

/**
 * Class for error handling within the Fed9UCreateDescription.
 * This class provides a list of errors which can occur
 * in Fed9UCreateDescription.
 */

#ifndef _Fed9UCreateDescriptionException_H_
#define _Fed9UCreateDescriptionException_H_
#include "ICExDecl.hh"

namespace Fed9U {

#define FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST \
  IC_DEF_ERROR(ERROR_CREATE_DESCRIPTION, "An error has been thrown from Fed9UCreateDescription.") \

  IC_EXCEPTION_CLASS_BEGIN(Fed9UCreateDescriptionException, FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END
}

#endif
#ifndef _Fed9UCreateDescription_H
#define _Fed9UCreateDescription_H

//Author - Gareth Rogers, RAL
//Date   - May 2004

/**
 * This class can be used to create a complete description object, with settings other than
 * the default description settings. It has been designed to be used with the soak test
 * which requires a range of description settings.
 *
 * The contents of the descripition files create are not controlled by the caller of the class.
 * The description will be created with predefined settings. An object of the Fed9UCreateDescription
 * will keep track of the number of times it has been called and will create a description with
 * different settings each time it is asked to create a new description.  The maximum value of
 * fed9UDescription number is 4294967295.
 */


namespace Fed9U {

  using std::string;

  class Fed9UCreateDescription {

  public:

    /**
     * Fed9UCreateDescription constructor.
     *
     * A Fed9UCreateDescription will return a different description depending on how many times it
     * has been called. By default it starts at description 0, however by altering the Fed9UDescription
     * number it will return the description defined by that number.
     */
    Fed9UCreateDescription(u32 fed9UDescriptionNumber = 0);

    /**
     * This returns the Fed9UDescription object. It contains a set of predefined values that depend on the
     * number of times this method has been called. The returned Fed9UDescription contains the settings.
     * The crate number, base address of the FED and the location of the HAL address table must be specified.
     */
    Fed9UDescription makeFed9UDescription(u16 crateNumber, u32 baseAddress, std::string halAddressTable);

  private:

    /**
     * This will load all the FE settings into the description.
     */
    Fed9UCreateDescription& loadFeFpgaSettings();

    /**
     * This will load all the BE settings into the description.
     */
    Fed9UCreateDescription& loadBeFpgaSettings();

    /**
     * This will load all the VME settings into the description.
     */
    Fed9UCreateDescription& loadVmeFpgaSettings();

    /**
     * This will load all the TTCrx settings into the description.
     */
    Fed9UCreateDescription& loadTtcrxDeviceSettings();

    /**
     * This will load all the Voltage Monitor settings into the description.
     */
    Fed9UCreateDescription& loadVoltageMonitorSettings();

    /**
     * This will load the serial EPROM into the description.
     */
    Fed9UCreateDescription& loadEpromSettings();

    /**
     * This will load all the higher level settings into the description.
     */
    Fed9UCreateDescription& loadHigherLevelSettings();

    /**
     * This holds the iteration number. It is used to vary the settings in the returned FED description.
     */
    u32 fed9UDescriptionNumber_;

    /**
     * Holds the settings that are to be returned.
     */
    Fed9UDescription fedDescription;

  };// End of Fed9UCreateDescription class

}// End of Fed9U namespace

#endif// _H_Fed9UCreateDescription
/**Matthew Pearson - RAL, July 2004.
   Class which provides access to some useful FED specific numbers for the SiStripDataFormat classes.*/

#ifndef FED9UEVENTINFO_H
#define FED9UEVENTINFO_H

namespace Fed9U {
  
  class Fed9UEventInfo {

  public:
    /**Virtual destructor.*/
    virtual ~Fed9UEventInfo() {}

    /**Method to get the number of FED front end units.*/
    unsigned short getFeUnitsPerFed(void) const {
      return FEUNITS_PER_FED;
    }

    /**Method to get the number of channels per FED front end unit.*/
    unsigned short getChannelsPerFeUnit(void) const {
      return CHANNELS_PER_FEUNIT;
    }

    /**Method to get the number of APVs per FED channel.*/
    unsigned short getApvsPerFedChannel(void) const {
      return APVS_PER_CHANNEL;
    }

    /**Method to get the number of Si strips per APV in the Si tracker.*/
    unsigned short getStripsPerApv(void) const {
      return STRIPS_PER_APV;
    }

    /**Method to get the size in bytes of the FED front end header in the FED data buffer.*/
    unsigned short getFeUnitHeaderSize(void) const {
      return TRACKER_FEUNIT_HEADER_SIZE;
    }

    /**Method to get the number of bytes in a raw Fed9U buffer.*/
    unsigned long getBytesInRawFed9UBuffer(void) const {
      return BYTES_IN_RAW_FED9U_BUFFER;
    }

    /**Method to get the number of bytes to the start of the FE headers.*/
    unsigned short getBytesToStartOfFeHeaders(void) {
      return BYTES_TO_START_OF_FE_HEADERS;
    }

    /**Method to get the number of bytes to the start of channel data.*/
    unsigned short getBytesToStartOfChannels(void) {
      return BYTES_TO_START_OF_CHANNELS;
    }

    /**Method to get the number of bytes in a raw Fed9U FE data block (including channel headers and packet codes).*/
    unsigned short getBytesInRawFed9UFE(void) {
      return BYTES_IN_RAW_FED9U_FE;
    }

    /**Method to get the number of bytes in a raw Fed9U channel (incluidng header and packet code).*/
    unsigned short getBytesInRawFed9UChannel(void) {
      return BYTES_IN_RAW_FED9U_CHANNEL;
    }

    /**Method to get the number of bytes in the DAQ trailer.*/
    unsigned short getBytesInDaqTrailer(void) {
      return BYTES_IN_DAQ_TRAILER;
    }

    /**Method to get the code for FED scope mode.*/
    unsigned short getScopeModeCode(void) const {
      return FED9U_SCOPE;
    }

    /**Method to get the code for FED frame finding mode.*/
    unsigned short getFrameFindingModeCode(void) const {
      return FED9U_FRAME;
    }

    /**Method to get the code for FED virgin raw mode.*/
    unsigned short getVirginRawModeCode(void) const {
      return FED9U_VIRGRAW;
    }

    /**Method to get the code for FED zero suppressed mode.*/
    unsigned short getZeroSuppressedModeCode(void) const {
      return FED9U_ZEROSUPP;
    }

    /**Method to get the code for FED processed raw mode.*/
    unsigned short getProcessedRawModeCode(void) const {
      return FED9U_PROCRAW;
    }

    /**Method to get the DAQ mode number for scope mode.*/
    unsigned short getScopeDaqModeCode(void) const {
      return FED9U_DAQ_SCOPE;
    }

    /**Method to get the DAQ mode number for virgin raw mode.*/
    unsigned short getVirginRawDaqModeCode(void) const {
      return FED9U_DAQ_VIRGRAW;
    }

     /**Method to get the DAQ mode number for processed raw mode.*/
    unsigned short getProcessedRawDaqModeCode(void) const {
      return FED9U_DAQ_PROCRAW;
    }

     /**Method to get the DAQ mode number for zero suppressed mode.*/
    unsigned short getZeroSuppressedDaqModeCode(void) const {
      return FED9U_DAQ_ZEROSUPP;
    }

    /**Method to get the channel status bits size.*/
    unsigned short getChannelStatusSize(void) const {
      return CHANNEL_STATUS_SIZE;
    }

    /**Methgod to get the size of the frame sync out packet.*/
    unsigned short getSizeOfFrameSyncOutPacket(void) {
      return SIZE_OF_FRAME_SYNC_OUT_PACKET;
    }

    /**Method to get the tracker special header full debug mode code.*/
    unsigned short getTrackerSpecialHeaderFullDebugMode(void) {
      return TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE;
    }

    /**Method to get the tracker special header apv error mode code.*/
    unsigned short getTrackerSpecialHeaderApvErrorMode(void) {
      return TRACKER_SPECIAL_HEADER_APV_ERROR_MODE;
    }

  private:
    static const unsigned short FEUNITS_PER_FED = 8;
    static const unsigned short CHANNELS_PER_FEUNIT = 12;
    static const unsigned short APVS_PER_CHANNEL = 2;
    static const unsigned short STRIPS_PER_APV = 128;

    static const unsigned short TRACKER_FEUNIT_HEADER_SIZE = 16; //Bytes
    static const unsigned long BYTES_IN_RAW_FED9U_BUFFER = 49624; //(including inter-FE padding)
    static const unsigned short BYTES_TO_START_OF_FE_HEADERS = 16;
    static const unsigned short BYTES_TO_START_OF_CHANNELS = 144;
    static const unsigned short BYTES_IN_RAW_FED9U_FE = 0x1824;
    static const unsigned short BYTES_IN_RAW_FED9U_CHANNEL = 0x0203;
    static const unsigned short BYTES_IN_DAQ_TRAILER = 8;

    /**These are the packet codes (without the 3 MSB).*/
    static const unsigned short FED9U_SCOPE = 1;
    static const unsigned short FED9U_FRAME = 2;
    static const unsigned short FED9U_VIRGRAW = 4;
    static const unsigned short FED9U_ZEROSUPP = 8;
    static const unsigned short FED9U_PROCRAW = 16;

    /**These are the DAQ event types.*/
    static const unsigned short FED9U_DAQ_SCOPE = 1;
    static const unsigned short FED9U_DAQ_VIRGRAW = 2;
    static const unsigned short FED9U_DAQ_PROCRAW = 6;
    static const unsigned short FED9U_DAQ_ZEROSUPP = 10;

    /**Info about the frame sync out packet.*/
    static const unsigned short CHANNEL_STATUS_SIZE = 6;
    static const unsigned short SIZE_OF_FRAME_SYNC_OUT_PACKET = 81;

    /**Header format - full debug or APV error.*/
    static const unsigned short TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE = 1;
    static const unsigned short TRACKER_SPECIAL_HEADER_APV_ERROR_MODE = 2;

  };
 
} //End of Fed9U Namespace

#endif //FED9UEVENTINFO_H
/**Matthew Pearson - RAL, July 2004
   Class which can perform reordering on silicon strip data. The data output by
   the tracker FED in raw data mode is dis-ordered (ie. not in strip order). This class 
   provides a method to re-order a FED channel worth of data. There is also a method
   to do the reverse, which dis-orders the data to emulate the ordering done in the APV MUX chip.
   The re-ordering algorithm implemented here is the same as the re-ordering algorithm in the FED 9U.*/

#ifndef FED9USISTRIPREORDERING_H
#define FED9USISTRIPREORDERING_H

#include<vector>

namespace Fed9U {

  using std::vector;

  class Fed9USiStripReordering {

  public:

    /**Virtual destructor.*/
    virtual ~Fed9USiStripReordering() {}
    
    /**Method which contains the re-ordering algorithm for the APV
       data frame. The input vector, apvOutput, will contain the the data
       which is like the output from an APV (ie. not in strip order).
       The re-ordered data will be put into fedOutput.*/
    void reOrderingAlgorithm(const vector<unsigned short> &apvOutput, vector<unsigned short> &fedOutput);

    /**Method which contains the dis-ordering algorithm for of APV strips.
       The input vector, apvInput, will contain the APV signal in strip
       order, and this method will dis-order it so it emulates
       the APV frame.
       The dis-ordered data will be put into apvOutput.*/
    void disOrderingAlgorithm(const vector<unsigned short> &apvInput, vector<unsigned short> &apvOutput);

  private:
    static const unsigned short NUMBER_OF_APV_STRIPS = 128;
 
  };
  
} //End of Fed9U namespace.


#endif //FED9USISTRIPREORDERING_H
/**Matthew Pearson, RAL, Feb 2005. */

/**Container class, which contains Fed9U buffer parameters. This
   is used by classes inheriting from the abstract base class Fed9UBufferCreator,
   and also by the user interface class Fed9UBufferGenerator.
   This class also contains the actual Fed9U buffer that will be created by
   the Fed9UBufferCreator implementations. Various methods are provided to 
   enable buffer manipulation, and getting/setting of buffer parameters.*/

#ifndef FED9UBUFFERDESCRIPTION_H
#define FED9UBUFFERDESCRIPTION_H


#include "ICExDecl.hh"

#include <vector>

namespace Fed9U {

  using ICUtils::ICException;
  using std::vector;

  class Fed9UBufferDescription {

  public:
    /**Constructor. This does the initialization of all the internal parameters, incluidng the buffer.*/
    Fed9UBufferDescription(void);

    virtual ~Fed9UBufferDescription() {}

    /**Methods to manipulate the internal Fed9U buffer.*/

    /**Method to provide access to the internal Fed9U buffer.*/
    inline unsigned int * getBuffer(void) {
      return mBuf;
    }

    /**Method to provide const access to the internal Fed9U buffer.*/
    inline unsigned int * getBufferConst(void) const {
      return const_cast<unsigned int *>(mBuf);
    }

    /**Method which returns the current size of the buffer (in units of 32-bit words).*/
    inline unsigned long getBufferSize(void) const {
      return mIndex;
    }

    /**Increment buffer index. This increments the index parameter for the buffer (it is also the
       buffer size after the buffer has been made.)*/
    inline void incrementBufferIndex(void) {
      ++mIndex;
    }

    /**Initialize the buffer index to zero. This should be called before generating a new buffer.*/
    inline void initBufferIndex(void) {
      mIndex = 0;
    }

    /**Initialize the buffer space. This should be called before generating a new buffer.*/
    void initBuffer(void);

    /**Methods to set the buffer parameters.*/

    /**Method to set the bunch crossing number. */
    Fed9UBufferDescription & setBunchCrossing(unsigned long bunchCrossing);

    /**Method to set the fed mode.*/
    Fed9UBufferDescription & setMode(unsigned long mode);

    /**Method to set the header format in the tracker special header (1=full debug mode, 2=APV error mode).*/
    Fed9UBufferDescription & setHeaderFormat(unsigned short headerFormat);

    /** Sets the FED readout mode ("Trkr Evt_Ty" field of the Tracker-specific header). */
    Fed9UBufferDescription& trackerEventType( unsigned short value ); // added by R.B.

    /**Method to set the buffer data vector. This must have as many elements as the number of strips a FED can read in.*/
    Fed9UBufferDescription & setFed9UBufferData(const vector<unsigned short> &bufferData) throw (ICException);

    /**Method to set the channel length vector. This must have as many elements as there are channels in a FED.*/
    Fed9UBufferDescription & setChannelLengths(const vector<unsigned short> &channelLengths) throw (ICException);

    /**Method to set the Fe length vector. This must have as many elements as there are FE units in a FED.*/
    Fed9UBufferDescription & setFeLengths(const vector<unsigned short> &feLengths) throw (ICException);

    /**Method to set the packet code.*/
    Fed9UBufferDescription & setPacketCode(unsigned short packetCode);

    /**Method to set the common mode median values. This must have as many elements as there APV read by a FED.*/
    Fed9UBufferDescription & setCommonModeMedians(const vector<unsigned short> &cmMedians) throw (ICException);

    /**Get methods for the buffer parameters.*/

    /**Method to get the bunch crossing number. */
    unsigned long getBunchCrossing(void) const;

    /**Method to get the fed mode.*/
    unsigned long getMode(void) const;

    /**Method to get the header format in the tracker special header (1=full debug mode, 2=APV error mode).*/
    unsigned short getHeaderFormat(void) const;

    /** Returns the FED readout mode ("Trkr Evt_Ty" field of the Tracker-specific header). */
    unsigned short trackerEventType(void) const; // added by R.B.
    
    /**Method to get the buffer data vector.*/
    vector<unsigned short> getFed9UBufferData(void) const;

    /**Method to get the channel length vector.*/
    vector<unsigned short> getChannelLengths(void) const;

    /**Method to get the FE length vector.*/
    vector<unsigned short> getFeLengths(void) const;

    /**Method to get the packet code.*/
    unsigned short getPacketCode(void) const;

    /**Method to get the common mode medians vector.*/
    vector<unsigned short> getCommonModeMedians(void) const;

  private:

    //static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 12406; // Max buffer size for raw mode (in units of ints).
    static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 49002; // Max buffer size for scope mode (in units of ints).

    unsigned int mBuf[INTS_IN_RAW_FED9U_BUFFER];

    vector<unsigned short> mFed9UBufferData;
    vector<unsigned short> mFeLengths;
    vector<unsigned short> mChannelLengths;

    unsigned int mIndex;
    unsigned long mBunchCrossing;
    unsigned long mMode;

    unsigned short mPacketCode;
    unsigned short mHeaderFormat;
    /** Value of "Trkr Evt_Ty" field of Tracker-specific header. */ 
    unsigned short trackerEventType_;
    vector<unsigned short> mCMMedians; 
    
    Fed9UEventInfo mFed9UEventInfo;
  };

}

#endif //FED9UBUFFERDESCRIPTION_H
/** Matthew Pearson - RAL, Feb 2005. */

/**This class provides methods to construct the trailer part of the Fed9U buffer.
   It should be used to compose classes which inherit from Fed9UBufferCreator.*/

#ifndef FED9UBUFFERTRAILER_H
#define FED9UBUFFERTRAILER_H

#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  class Fed9UBufferTrailer {

  public:
    /**Method which attaches the DAQ trailer to the end of the FED buffer.*/
    void generateFed9UDaqTrailer(Fed9UBufferDescription *theBufDesc) throw (ICException);

  };

}

#endif //FED9UBUFFERTRAILER_H
/** Matthew Pearson - RAL, March 2005. */

/**This class provided methods to construct the data payload part of the Fed9U buffer.
   It should be used to compose classes which inherit from Fed9UBufferCreator.*/

#ifndef FED9UBUFFERDATA_H
#define FED9UBUFFERDATA_H


#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  class Fed9UBufferData {

  public:

    /**Method which puts channel data in the Fed9U RAW data buffer. Inside the Fed9UBufferDescription must be the vector 
       of data, which contains the ADC values for each strip (in channel and strip order order). 
       The length of the buffer for each channel is also passed in inside the channelLengths vector inside
       the Fed9UBufferDescription. This method can be used for both the virgin raw data mode and processed raw data mode.*/
    void generateFed9URawData(Fed9UBufferDescription *bufDesc ) throw (ICException);

    /**Method which puts channel data into the Fed9U zero suppressed buffer.*/
    void generateFed9UZSData(Fed9UBufferDescription *bufDesc) throw (ICException);

    /**Method which puts channel data in the Fed9U scope mode data
       buffer. Inside the Fed9UBufferDescription must be the vector of
       data, which contains the ADC values for each strip (in channel
       and strip order order).  The length of the buffer for each
       channel is also passed in inside the channelLengths vector
       inside the Fed9UBufferDescription.*/
    void generateFed9UScopeModeData( Fed9UBufferDescription *bufDesc, unsigned short scope_mode_length ) throw (ICException);

  private:
    /*Method which can pad out the buffer until the start of the next 64-bit word.
      The ends of the FE data blocks in the FED buffer are padded out with zeros until 
      the start of the next 64-bit words. The input needed is the number of bytes used 
      in the buffer for all the clusters in one front end. If operating in RAW data mode,
      one just needs to input zero for the cBytes.*/
    void paddToNext64BitWord(Fed9UBufferDescription *bufDesc, unsigned int cBytes) throw (ICException);

    Fed9UEventInfo mFed9UEventInfo;

  };

}

#endif //FED9UBUFFERDATA_H
/** Matthew Pearson - RAL, Feb 2005. */

/**This class provides methods to construct the header part of the Fed9U buffer. 
   It should be used to compose classes which inherit from Fed9UBufferCreator.*/

#ifndef FED9UBUFFERHEADER_H
#define FED9UBUFFERHEADER_H


#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  class Fed9UBufferHeader {
    
  public:
    /**This constructs the Fed9U DAQ header at the start of the Fed9U buffer. This is 8 bytes, which contains
       the bunch crossing number and FED mode.*/
    void generateFed9UDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICException);

    /**This constructs the Fed9U Tracker Special DAQ header in the 2nd 64-bit word of the Fed9U buffer. This
       is another 8 bytes which contain FED debug information.*/
    void generateFed9UTrackerSpecialDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICException);

    /**This adds the FE header information to the Fed9U buffer. 
       This is 16 bytes per FE (128 bytes in total). The feLength vector inside the Fed9UBufferDescription should contain 
       the 8 FE lengths in bytes for all channel info for that FE (including channel header and packet code).
       They do not include the length of the FE header itself.*/
    void generateFed9UFEHeader(Fed9UBufferDescription *theBufDesc) throw (ICException);

  private:
    Fed9UEventInfo mFed9UEventInfo;

  };

}

#endif //FED9UBUFFERHEADER_H
/** Matthew Pearson - RAL, Feb 2005 */

/** This class is an abstract base class to define an interface 
    for generating Fed9U buffers. A concrete implementation of
    this class should inherit from this class and re-implement the
    appropriate methods.*/

#ifndef FED9UBUFFERCREATOR_H
#define FED9UBUFFERCREATOR_H

namespace Fed9U {
  
  class Fed9UBufferDescription;
  
  class Fed9UBufferCreator {
    
  public:
    
    /**Method to be re-implemented. Concrete implementations of this should generate the Fed9U buffer.
       The Fed9UBufferDescription object contains information relevant for making a Fed9U buffer.*/
    virtual void generateBuffer(Fed9UBufferDescription *) = 0;
    
    virtual ~Fed9UBufferCreator() {};
    
  protected:
    
    Fed9UBufferCreator(void) {};
  
  };
  
}

#endif //FED9UBUFFERCREATOR_H
/** Matthew Pearson - RAL, Feb 2005. */

/**This is a concrete implementation of the Fed9UBufferCreator class for the Fed9U RAW data format.
   It reimplements the generateBuffer method to provide a consistent user interface. */

#ifndef FED9UBUFFERCREATORRAW_H
#define FED9UBUFFERCREATORRAW_H


#include <vector>

namespace Fed9U {

  using std::vector;

  class Fed9UBufferDescription;
  
  class Fed9UBufferCreatorRaw : public Fed9UBufferCreator {
    
  public:
    
    /**Method to generate a Fed9U RAW data buffer. It takes in a pointer to a Fed9UBufferDescription.*/
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);
    
  private:
  
    /**Method to dis-order the data for a whole FED, so that data for each channel 
       (ie. each APV pair) is in APV-MUX order. The input vector has to be a size equal
       to the number of strips a FED caters for. The vector will be modified to contain 
       the dis-ordered data.*/
    //NOT VERY EFFICIENT. IT WILL BE IMPROVED AT SOME POINT.
    void disOrderData(vector<unsigned short> &bufferData) const throw (ICException);
      
    Fed9UEventInfo mFed9UEventInfo;

    Fed9UBufferHeader mBufHeader;
    Fed9UBufferData mBufData;
    Fed9UBufferTrailer mBufTrailer;
    
  };
  
}

#endif //FED9UBUFFERCREATORRAW_H
/** Matthew Pearson - RAL, Feb 2005. */

/**This is a concrete implementation of the Fed9UBufferCreator class for the Fed9U Processed RAW data format.
   It reimplements the generateBuffer method to provide a consistent user interface. */

#ifndef FED9UBUFFERCREATORPROCRAW_H
#define FED9UBUFFERCREATORPROCRAW_H


namespace Fed9U {

  class Fed9UBufferDescription;

  class Fed9UBufferCreatorProcRaw : public Fed9UBufferCreator {

  public:

    /**Method to generate a Fed9U RAW data buffer.  It takes in a pointer to a Fed9UBufferDescription.*/
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);

  private:

    Fed9UEventInfo mFed9UEventInfo;

    Fed9UBufferHeader mBufHeader;
    Fed9UBufferData mBufData;
    Fed9UBufferTrailer mBufTrailer;

  };

}

#endif //FED9UBUFFERCREATORPROCRAW_H
/** Matthew Pearson - RAL, Feb 2005. */

/**This is a concrete implementation of the Fed9UBufferCreator class for the Fed9U zero suppressed data format.
   It reimplements the generateBuffer method to provide a consistent user interface. */

#ifndef FED9UBUFFERCREATORZS_H
#define FED9UBUFFERCREATORZS_H


#include <vector>

namespace Fed9U {

  class Fed9UBufferDescription;

  using std::vector;

  class Fed9UBufferCreatorZS : public Fed9UBufferCreator {

  public:

    /**Method to generate a Fed9U ZS data buffer.  It takes in a pointer to a Fed9UBufferDescription.*/
    void generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException);

  private:

    /**Method which will calculate the FE lengths and channel lengths (in bytes) needed to construct the buffer.
       It takes in the bufferData vector, which contains ADC values for all the strips in a FED, and searches for
       non-zero values and clusters of values. The appropriate channel lengths and FE lengths are counted up during the 
       vector search.*/
    void findDataLengths(const vector<unsigned short> &bufferData, 
			 vector<unsigned short> &feLengths, 
			 vector<unsigned short> &channelLengths) throw (ICException);
      
    Fed9UEventInfo mFed9UEventInfo;

    Fed9UBufferHeader mBufHeader;
    Fed9UBufferData mBufData;
    Fed9UBufferTrailer mBufTrailer;

  };

}

#endif //FED9UBUFFERCREATORZS_H
/** R.Bainbridge, Sept 2005. */

/** This is a concrete implementation of the Fed9UBufferCreator class
    for the Fed9U Scope Mode data format. It reimplements the
    generateBuffer method to provide a consistent user interface. */

#ifndef FED9UBUFFERCREATORSCOPEMODE_H
#define FED9UBUFFERCREATORSCOPEMODE_H


namespace Fed9U {
  
  class Fed9UBufferDescription;
  
  class Fed9UBufferCreatorScopeMode : public Fed9UBufferCreator {
    
  public:

    /** Constructor that takes scope mode length as an argument. */
    Fed9UBufferCreatorScopeMode( unsigned short length = 280 );
    
    /** Method to generate a Fed9U RAW data buffer. It takes in a
	pointer to a Fed9UBufferDescription. */
    void generateBuffer( Fed9UBufferDescription *bufDesc ) throw (ICException);
    
  private:
    
    Fed9UEventInfo mFed9UEventInfo;
    
    Fed9UBufferHeader mBufHeader;
    Fed9UBufferData mBufData;
    Fed9UBufferTrailer mBufTrailer;

    unsigned short length_;
    
  };
  
}

#endif // FED9UBUFFERCREATORSCOPEMODE_H
/**Matthew Pearson - RAL, Feb 2005. */

/**The user interface class to create Fed9U buffers. It can be used to create any type of
   Fed9U buffer, providing it has been given a pointer to an object which inherits from Fed9UBufferCreator.
   This class gives the user an interface to set various buffer paremeters, such as the data, bunch crossing
   number, etc. It also provides methods which allow the user to get a copy of the buffer once it
   has been generated.*/

#ifndef FED9UBUFFERGENERATOR_H
#define FED9UBUFFERGENERATOR_H


#include "ICExDecl.hh"

#include<vector>

namespace Fed9U {

  class Fed9UBufferCreator;

  using ICUtils::ICException;

  class Fed9UBufferGenerator {

  public:
    /**Constructor. This takes in a pointer to the concrete implementation of Fed9UBufferCreator.*/
    Fed9UBufferGenerator(Fed9UBufferCreator *theCreator);

    /**Method to generate the Fed9U buffer. This takes in a vector (equal in size to the number
       of strips for a Fed9U) which contains the ADC values for each strip in the buffer (in strip and channel
       order - eg. element 0 is the first strip in the first channel in the first FE unit). The data have to 
       be in 'strip' order (not in APV-MUX order).*/
    void generateFed9UBuffer(const std::vector<unsigned short> &bufferData) throw (ICException);

    /**The methods to allow the user to get the buffer which is created.*/

    /**Method to copy the Fed9U buffer from internal memory into an area pointed to by the given pointer.*/
    void getBuffer(unsigned int *fedBuffer) const throw (ICException);
    /** Method to return copy of Fed9U buffer. */
    vector<unsigned char> getBuffer() throw (ICException);
    /**Method to get the buffer size in units of 32-bits.*/
    unsigned long getBufferSize(void) const throw (ICException);

    /**Set methods for the buffer parameters.*/

    /**Method to set the bunch crossing number. */
    Fed9UBufferGenerator & setBunchCrossing(unsigned long bunchCrossing) throw (ICException);
    /**Method to set the tracker header to full debug mode.*/
    Fed9UBufferGenerator & setHeaderToFullDebug(void) throw (ICException);
    /**Method to set the buffer creator pointer. This allows the user to change mode between generating events.*/
    Fed9UBufferGenerator & setFed9UBufferCreator(Fed9UBufferCreator *theCreator);
    /**Method to set the common mode median values for the buffer when running in zero suppressed full debug mode.
       The input vector must have as many elements as there APVs read by a FED.*/
    Fed9UBufferGenerator & setCommonModeMedians(const vector<unsigned short> &cmMedians) throw (ICException);

    /**Get methods for the buffer parameters.*/

    /**Method to get the bunch crossing number. */
    unsigned long getBunchCrossing(void) const throw (ICException);
    
  private:

    Fed9UBufferCreator *mFed9UBufferCreator;

    /**This is the only copy of the Fed9UBufferDescription. 
       The user can change this using the interface provided with this class.*/
    Fed9UBufferDescription mFed9UBufferDescription;

    Fed9UEventInfo mFed9UEventInfo;
    
  };

}

#endif
