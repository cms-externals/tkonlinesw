/** Jo Cole June 2006 
 *  Updated March 2007
 *  Updated April 2007 to handle 32-bit word swapping
 */

#ifndef H_Fed9UEventStreamLine
#define H_Fed9UEventStreamLine

#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#include "Fed9UFakeBufferCreator.hh"
#include "Fed9UEventUnitStreamLine.hh"
#include "Fed9UEventStreamLineException.hh"
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
