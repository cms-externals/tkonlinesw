#include "Fed9UEvent.hh"
#include "Fed9UEventException.hh"
#include "Fed9UBufferGenerator.hh"
#include "Fed9UBufferCreator.hh"
#include "Fed9UBufferCreatorRaw.hh"
#include "ICAssert.hh"
#include "Fed9UDescription.hh"
// <NAC date="04/05/2009"> Refactored CRC code into separate header
#include "Fed9UCrc.hh"
// </NAC>
//#include "crcmodel.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <climits>
#include <bitset>
#include <cstring>
using namespace std;

namespace Fed9U {

  const u16 Fed9UEvent::SPECIAL_OFF; //Offset of tracker special header in bytes.

#ifndef FED9U_NDEBUG_EVENT
# define DBG(x) if (Fed9UEventDebug) cerr << "DEBUG: " << x << endl
  static bool Fed9UEventDebug = false;
#else
# define DBG(x) ((void)0)
#endif


  void Fed9UEvent::EnableDebug(bool on) { cout << "called Fed9UEvent::EnableDebug(" << (on?"true":"false")<< ")"<< endl; Fed9UEventDebug = on; }


  void Fed9UEvent::DebugInit(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize, std::string & report, u32& readCrc, u32& calcCrc) {
    std::stringstream tempReport;
    try{
      _buffer.set(reinterpret_cast<u8*>(buffer), bufsize * 4);  
      //      cout << hex << 
      readCrc = _buffer.getu16(_buffer.size()-4,true);
      calcCrc = calcCrcDirty();
      ICUTILS_VERIFY(readCrc == calcCrc)(readCrc)(calcCrc).error();
    }catch (exception & exc) {
      tempReport << "Failed to get CRC with following error: " << exc.what() << endl;      
    }catch (...) {
      tempReport << "Failed to get CRC with unknown error " << endl;      
    }
    try {
      Init( buffer, currentDescription, bufsize);
    } catch ( exception & exc) {
      tempReport << "Failed to construct Event with following error: " << exc.what() << endl;
      RETHROW(exc, ICUtils::ICException("Error whilst constructing event"));
    }
    report = tempReport.str();
  }

  
  void Fed9UEvent::Init(u32 * buffer, const Fed9UDescription * fedDescription, u32 bufsize) {
    try {
      _buffer.set(reinterpret_cast<u8*>(buffer), bufsize * 4);
      _rubbish = 0;
      _fedDescription = fedDescription;
      _feunits.clear();
      _feunits.reserve(FEUNITS_PER_FED);
      Fed9UEventIterator headerptr;
      Fed9UEventIterator dataptr;
      try {
	//EnableDebug(true);
	//Matthew Pearson Jan 05 - read the new tracker special header to determin the type of header.
	//Increment the headerptr accordingly.
	headerptr = _buffer + 8;
	if (headerptr.getu8(0) == 0xED) {
	  _eventFormat = FED9U_EVENT_FORMAT_OLD_VME;
	  DBG("increment header pointer to miss first 64 bits!!!" << endl);
	  headerptr = headerptr + 8;
	  // make sure that the mode has not already been set !
	  if ((_mode != FED9U_EVENT_MODE_FAKE_FULL) && (_mode != FED9U_EVENT_MODE_FAKE_SINGLE)) {
	    _mode = FED9U_EVENT_MODE_REAL;
	  }
	  _headerFormat = getSpecialHeaderFormat();
	  //<JEC date=18/4/07> modified to handle 32-bit word swapping
	} else if (headerptr.getu8(0) == 0xC5) {
	  _eventFormat = FED9U_EVENT_FORMAT_STANDARD;
	  DBG("increment header pointer to miss first 64 bits!!!" << endl);
	  headerptr = headerptr + 8;
	  // make sure that the mode has not already been set !
	  if ((_mode != FED9U_EVENT_MODE_FAKE_FULL) && (_mode != FED9U_EVENT_MODE_FAKE_SINGLE)) {
	    _mode = FED9U_EVENT_MODE_REAL;
	  }
	  _headerFormat = getSpecialHeaderFormat();
	  //</JEC>
	} else {
	  _mode = FED9U_EVENT_MODE_LEGACY;
	  _eventFormat = FED9U_EVENT_FORMAT_OLD_VME;
	  // In legacy format, events are always in full_debug mode ...
	  _headerFormat = FED9U_HEADER_FULLDEBUG;
	}
	dataptr = headerptr + FEUNITS_PER_FED * 16;
	headerptr.resize(FEUNITS_PER_FED * 16);
      } catch (const exception & e) {
        RETHROW(e, ICUtils::ICException("Event buffer too small (header)"));
      }
      DBG("Buffer.size(): " << _buffer.size());
      DBG("dataptr.size(): " << dataptr.size());
      DBG("headerptr.size(): " << headerptr.size());
      _payload = dataptr;
      
      // JEC 16-06-06: added check to make sure that this is not used for anything other than full debug mode
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("Fed9UEvent is only valid for the Full Debug Header Format.");
      
      if (getDaqMode() != FED9U_MODE_SPY) {
        for (int i = 0; i < FEUNITS_PER_FED; i++) {
          DBG("FE Unit " << i);
          try {
            _feunits.push_back(Fed9UEventUnit(headerptr, dataptr, _headerFormat));
          } catch (const exception & e) {
            stringstream temp;
            temp << "Error detected while constructing module " << i;
            RETHROW(e, ICUtils::ICException(temp.str()));
          }
          DBG("Length = " << _feunits.back().dataLength());
          try {
            dataptr += _feunits.back().dataLength();
          } catch (const exception & e) {
            stringstream temp;
            temp << "Error detected after constructing module " << i;
            RETHROW(e, ICUtils::ICException(temp.str()));
          }
          headerptr += 16;
        }
      } else {
        headerptr += FEUNITS_PER_FED*16;
        //read the event length from the buffer
        u32 length = dataptr.getu32(0);
        //increment dataptr so that it points to the trailer
        dataptr += length;
      }
      try {
	// set trailer pointer to the start of the trailer (ie. move from dataptr to the end of padding bytes for the last FE-unit)
        _trailer = dataptr.normalize();
        //std::cout << "removing rubbish" << endl;
        for (int i = 0; i < 3; i++) {
	  //<JEC date=18/4/07> modified to handle 32-bit word swapping
	  if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
	    if (_trailer.getu32(0) != 0) {break;}
	  } else {
	    if (_trailer.getu32(4) != 0) {break;}
	  }
	  //</JEC>
          //std::cout << "  found a word" << endl;
          _rubbish += 4;
          _trailer += 4;
          _trailer = _trailer.normalize();
        }
        ICUTILS_VERIFY(_trailer.size() >= 8)(_trailer.size()).error()
          .msg("Missing trailer");
      } catch (const exception & e) {
        RETHROW(e, ICUtils::ICException("Event buffer too small (trailer)"));
      }
    } catch (const exception & e) {
      RETHROW(e, ICUtils::ICException("Event data is corrupted"));
    }
    //cerr << "Event constructed successfully" << endl;
  }


  std::ostream & operator<< (std::ostream & os, const Fed9UEventIterator & it) {
    it.debug(os);
    return os;
  }


  Fed9UEvent::Fed9UEvent(istream & is) : _buffer(0), _rubbish(0), _fedDescription(0), _deleteBuffer(true), _mode(FED9U_EVENT_MODE_REAL),  _eventFormat(FED9U_EVENT_FORMAT_OLD_VME) {
    u32 size; // buffer size in bytes
    u8 fileType; // fileType can be 0, 1 or 2.  0 means binary full raw event buffer, 1 means single APV frame file in text, 2 means full FED data in text read back from Fake Event registers.
    // first read in the file type ( the first byte must be 1 for text apv frames mode and zero for binary, 2 for full FED fake event readback data. )
    is.read(reinterpret_cast<char*>(&fileType),1);
    //cout << "file type = " << static_cast<u16>(fileType) << endl;
    u32 *  buffer=NULL;
    if (fileType == 0 ) { // if we are in binary file mode we can read the binary file in directly
      // First read in the size of the FED buffer, this is stored in the first 4 bytes
      is.read(reinterpret_cast<char*>(&size),4);
      //create a buffer that will be managed by the fed9UEvent class in this instance (we have set _deleteBuffer to true 
      buffer = new u32[size/4]; //the size stored in the buffer file is number of bytes.
      //cout << "Fed9UEvent:: self managed buffer address = " << hex << buffer << dec << "Hit a key to continue" << endl;
      //u32 temp;
      //cin >> temp;
      // read in the buffer of length size bytes.
      is.read(reinterpret_cast<char*>(buffer),static_cast<int>(size));
      _mode =  FED9U_EVENT_MODE_FAKE_FULL;

    } 
    else if(fileType == 49) { // if we are in text APV frame file mode we must read in the text and interpret them into a FED buffer
      // text fed buffer is simply one sample per line, for both apvs in the channel... thus 280 samples for the complete frame.
      const unsigned short strips_per_fed = 96 * 256;
      std::vector<unsigned short> data_buffer;
      data_buffer.reserve(strips_per_fed);
      Fed9U::Fed9UBufferCreator* creator = new Fed9U::Fed9UBufferCreatorRaw();
      
      // generate FED buffer
      Fed9U::Fed9UBufferGenerator generator( creator );

      //     u32 bunchCrossingNumber, mode;
      //u16 headerFormat;

      //generator.setBunchCrossing();
      //generator.setMode();
      //generator.setHeaderFormat();
      //generator.setChannelLengths();
      //generator.setFeLengths();
      //generator.setPacketCode(unsigned short packetCode);
      std::vector<u16> samples;
      u16 sample;
      for (u32 i = 0 ; i < 256 ; i ++ ) {
	is >> sample;
	samples.push_back(sample);
	//cout << "sample " << i << " = " << sample << endl;
      }
      
      for ( u32 j = 0 ; j < 96 ; j ++ ) {
	for (u32 i = 0 ; i < 256 ; i ++ ) {
	  data_buffer.push_back(samples[i]);
	}
      }
      
      //cout << " size of my buffer = " << data_buffer.size() << endl;

      generator.generateFed9UBuffer( data_buffer );
      //setCommonModeMedians(const std::vector<unsigned short> &cmMedians);

      // calculate size of FED buffer in units of bytes (unsigned char)
      size = generator.getBufferSize() * 4;
      
      // resize (public) "data_" member of struct FEDRawData
      //fedrawdata.resize( nbytes );
      buffer = new u32[size/4];
      //cout << "Fed9UEvent:: self managed buffer address = " << hex << buffer << dec << " Hit a key to continue" << endl;
      //u32 temp;
      //cin >> temp;
      
      // copy FED buffer to buffer using Fed9UBufferGenerator
      unsigned int* ints = reinterpret_cast<unsigned int*>( buffer );
      generator.getBuffer( ints );
     
      _mode =  FED9U_EVENT_MODE_FAKE_SINGLE;
      
    }
    else if(fileType == 50) { // if we are in text APV frame file mode we must read in the text and interpret them into a FED buffer
            // text fed buffer is simply one sample per line, for both apvs in the channel... thus 280 samples for the complete frame.
      const unsigned short strips_per_fed = 96 * 256;
      std::vector<unsigned short> data_buffer;
      data_buffer.reserve(strips_per_fed);
      Fed9U::Fed9UBufferCreator* creator = new Fed9U::Fed9UBufferCreatorRaw();
      
      // generate FED buffer
      Fed9U::Fed9UBufferGenerator generator( creator );

      //      u32 bunchCrossingNumber, mode;
      //u16 headerFormat;

      //generator.setBunchCrossing();
      //generator.setMode();
      //generator.setHeaderFormat();
      //generator.setChannelLengths();
      //generator.setFeLengths();
      //generator.setPacketCode(unsigned short packetCode);
      std::vector<u16> samples;
      u16 sample;
      for (u32 i = 0 ; i < (256 * 96) ; i ++ ) {
	is >> sample;
	data_buffer.push_back(sample);
	//cout << "sample " << i << " = " << sample << endl;
      }
      
      //cout << " size of my buffer = " << data_buffer.size() << endl;

      generator.generateFed9UBuffer( data_buffer );
      //setCommonModeMedians(const std::vector<unsigned short> &cmMedians);

      // calculate size of FED buffer in units of bytes (unsigned char)
      size = generator.getBufferSize() * 4;
      
      // resize (public) "data_" member of struct FEDRawData
      //fedrawdata.resize( nbytes );
      buffer = new u32[size/4];
      //cout << "Fed9UEvent:: self managed buffer address = " << hex << buffer << dec << " Hit a key to continue" << endl;
      //u32 temp;
      //cin >> temp;
      
      // copy FED buffer to buffer using Fed9UBufferGenerator
      unsigned int* ints = reinterpret_cast<unsigned int*>( buffer );
      generator.getBuffer( ints );
     
      _mode =  FED9U_EVENT_MODE_FAKE_FULL;
      

    }
    else {
      _mode = FED9U_EVENT_MODE_REAL; // added to try to remove a warning.
      ICUTILS_VERIFY(1==2).error().msg("Fed9UEvent input stream constructor does not recognise this input format");

    }
    Init(buffer, NULL, size/4);
  }
  
  Fed9UEvent::~Fed9UEvent() {
    //_buffer.debug(std::cout);
    if (_deleteBuffer)
      _buffer.clear();
  }


  const Fed9UEventChannel & Fed9UEvent::channel(size_t unit, size_t channel) const {
    //std::cout << "feUnit = " << unit << " channel = " << channel << std::endl;
    //  Fed9UEventUnit temp = feUnit(unit);
    //std::cout << "got unit" << std::endl;
    //Fed9UEventChannel test = temp.channel(channel);
    //std::cout << "got channel" << std::endl;
    return feUnit(unit).channel(channel);
  }

  
  const Fed9UEventChannel & Fed9UEventUnit::channel(size_t i) const {
    //std::cout << "calling channel() on unit" << std::endl;
    ICUTILS_VERIFY(i < _channels.size())(i)(_channels.size()).msg("Index out of bounds").error();
    //std::cout << "checked size" << std::endl;
    return _channels[i];
  }


  void Fed9UEvent::checkEvent() const {
    u32 bufferLength = _trailer - _buffer + _rubbish + _trailer.size();
    ICUTILS_VERIFY(8*getTotalLength()+_rubbish == bufferLength)(8*getTotalLength())(_rubbish)(bufferLength).error()
      .error().msg("DAQ Length does not match actual length");
    getDaqMode(); // don't need return value, just want to check if it will throw
    ICUTILS_VERIFY(getCrc() == calcCrc())(getCrc())(calcCrc()).error()
      .error().msg("Calculated CRC does not match CRC in DAQ Trailer");
    // if we are in zero suppressed mode we need to check that all the channel data are in the right place...
    if (getDaqMode() == FED9U_MODE_ZERO_SUPPRESSED ) {
      for (int i=0 ; i<96 ; i++) {
	ICUTILS_VERIFY((channel(i).getIterator().getu8(2)) == 0xEA)(string("channel = "))(i).msg("packet code wrong on a channel").error();
      }
    }
  }


  void Fed9UEvent::checkChannelStatuses() const {
    /*
     * Now we check the Fiber Statuses to see if the channels are all timed in and have sufficient amplitudes.
     */    
    //cout << "check channel statuses!" << std::hex << static_cast<u16>(this->channel(0,11).getStatus()) << std::dec << endl;
    ICUTILS_VERIFY(_fedDescription != NULL )(_fedDescription).msg("you must supply a valid description if you want to check channel statuses.").error();
    stringstream statusErrorString;
    Fed9UAddress addr;
    if ( _fedDescription->getDaqMode() != FED9U_MODE_SCOPE ) {
      for (u16 j = 0; j < FEUNITS_PER_FED; j++){
	//cout << "FEUNIT NUMBER " << static_cast<int>(Fed9UAddress().setFedFeUnit(j).getExternalFedFeUnit() ) << " is " << ( _fedDescription->getFedFeUnitDisable(addr.setFedFeUnit(j))?"disabled":"enabled") <<  endl;
	if ( _fedDescription->getFedFeUnitDisable(addr.setFedFeUnit(j)) ) {
	  //cout << "fe unit disabled!!!  j = " << j << endl;
	  continue;
	}
	for (u16 i = 0; i < CHANNELS_PER_FEUNIT; ++i){
	  if ( _fedDescription->getApvDisable(addr.setFeUnitChannel(i).setChannelApv(true)) || 
	       _fedDescription->getApvDisable(addr.setChannelApv(false)) ) {
	    //cout << "channel disabled!!! i = " << i << ", j = " << j << endl;
	    continue;
	  }
	  if ( (this->channel(j,i).getStatus() & 0x3f) != 0x3f) statusErrorString << "\nFeUnit " << static_cast<u16>(addr.getExternalFedFeUnit()) << ", Channel " << static_cast<u16>(addr.getExternalFeUnitChannel()) << " Has Errors: ";
	  if ( (this->channel(j,i).getStatus() & 0x1) != 0x1) statusErrorString << "APV ErrorB<APV2>, ";
	  if ( (this->channel(j,i).getStatus() & 0x2) != 0x2) statusErrorString << "Wrong Header<APV2>, ";
	  if ( (this->channel(j,i).getStatus() & 0x4) != 0x4) statusErrorString << "APV ErrorB<APV1>, ";
	  if ( (this->channel(j,i).getStatus() & 0x8) != 0x8) statusErrorString << "Wrong Header<APV1>, ";
	  if ( (this->channel(j,i).getStatus() & 0x10) != 0x10) statusErrorString << "Out Of SyncB, ";
	  if ( (this->channel(j,i).getStatus() & 0x20) != 0x20) statusErrorString << "Not Locked, ";
	  if ( (this->channel(j,i).getStatus() & 0x3f) != 0x3f) statusErrorString << endl;
	} 
      }
      ICUTILS_VERIFY(statusErrorString.str().size() == 0).msg(statusErrorString.str().c_str()).error();
    }
  }

  
  Fed9UDaqMode Fed9UEvent::getDaqMode() const {
    //<JRF date="28/11/2007"> the event Type is taken from the 4 bits in the header of the event data the modes are labelled by the follwing decimal values:
    //1 = Scope Mode REAL
    //2 = Virgin Raw Mode REAL
    //3 = Virgin Raw Mode FAKE
    //6 = Processed Raw Mode REAL
    //7 = Processed Raw Mode FAKE
    //10 = Zero Suppressed Mode REAL
    //11 = Zero Suppressed Mode FAKE
    //12 = Zero Suppressed Lite Mode REAL
    //13 = Zero Suppressed Lite Mode FAKE
    //14 = Spy Mode REAL
    
    switch (getEventType()) {
    case 1: return FED9U_MODE_SCOPE;
    case 2: return FED9U_MODE_VIRGIN_RAW;
    case 3: return FED9U_MODE_VIRGIN_RAW;
    case 6: return FED9U_MODE_PROCESSED_RAW;
    case 7: return FED9U_MODE_PROCESSED_RAW;
    case 10: return FED9U_MODE_ZERO_SUPPRESSED;
    case 11: return FED9U_MODE_ZERO_SUPPRESSED;
    case 12: return FED9U_MODE_ZERO_SUPPRESSED;
    case 13: return FED9U_MODE_ZERO_SUPPRESSED;
    case 14: return FED9U_MODE_SPY;

    default: ICUTILS_VERIFYX(false, Fed9UEventException)(getEventType()).error().code(Fed9UEventException::ERROR_DAQ_MODE_UNKNOWN).msg("Unrecognised event type");
    }
    return FED9U_MODE_SCOPE; // never get here, just to silence warning
  }

   Fed9UDaqSuperMode Fed9UEvent::getDaqSuperMode() const {
    //<JRF date="28/11/2007"> the event Type is taken from the 4 bits in the header of the event data the modes are labelled by the follwing decimal values:
    //1 = Scope Mode REAL
    //2 = Virgin Raw Mode REAL
    //3 = Virgin Raw Mode FAKE
    //6 = Processed Raw Mode REAL
    //7 = Processed Raw Mode FAKE
    //10 = Zero Suppressed Mode REAL
    //11 = Zero Suppressed Mode FAKE
    //12 = Zero Suppressed Lite Mode REAL
    //13 = Zero Suppressed Lite Mode FAKE
    //14 = Spy Mode REAL
    
    switch (getEventType()) {
    case 1: return FED9U_SUPER_MODE_NORMAL;
    case 2: return FED9U_SUPER_MODE_NORMAL;
    case 3: return FED9U_SUPER_MODE_FAKE;
    case 6: return FED9U_SUPER_MODE_NORMAL;
    case 7: return FED9U_SUPER_MODE_FAKE;
    case 8: return FED9U_SUPER_MODE_NORMAL;
    case 10: return FED9U_SUPER_MODE_NORMAL;
    case 11: return FED9U_SUPER_MODE_FAKE;
    case 12: return FED9U_SUPER_MODE_ZERO_LITE;
    case 13: return FED9U_SUPER_MODE_FAKE_ZERO_LITE;
    case 14: return FED9U_SUPER_MODE_NORMAL;
    default: ICUTILS_VERIFYX(false, Fed9UEventException)(getEventType()).error().code(Fed9UEventException::ERROR_DAQ_MODE_UNKNOWN).msg("Unrecognised event type");
    }
    return FED9U_SUPER_MODE_NORMAL; // never get here, just to silence warning
  }


  int Fed9UEvent::getFeBufferLength(int feUnit) const {return (_buffer[2 + feUnit*4 + 2] >> 16) & 0xffff;}


  //<JEC date=18/4/07> modified to handle 32-bit word swapping
  u32 Fed9UEvent::getEventNumber()   const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {
      return _buffer.getu32(0, true) & 0xffffff; 
    } else {
      return _buffer.getu32(4, true) & 0xffffff; 
    }
  }
  //</JEC>

  //<JEC date=19/4/07> modified to handle 32-bit word swapping
  u16 Fed9UEvent::getBunchCrossing() const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _buffer.getu16(4, true) >> 4;}
    else {return _buffer.getu16(0, true) >> 4;}
  }
  //<JEC date=27/04/06>  replace getEventType to make sure the new data format works OK
  //  u8  Fed9UEvent::getEventType()     const { return _buffer.getu8(0) & 0xf;       }
  //</JEC>


  
  //<JRF date="28/11/2007"> the event Type is taken from the 4 bits in the header of the event data the modes are labelled by the follwing decimal values:
  //1 = Scope Mode REAL
  //2 = Virgin Raw Mode REAL
  //3 = Virgin Raw Mode FAKE
  //6 = Processed Raw Mode REAL
  //7 = Processed Raw Mode FAKE
  //10 = Zero Suppressed Mode REAL
  //11 = Zero Suppressed Mode FAKE
  //12 = Zero Suppressed Lite Mode REAL
  //13 = Zero Suppressed Lite Mode FAKE
  //14 = Spy Mode REAL
  u8  Fed9UEvent::getEventType()     const {
    if (_mode == FED9U_EVENT_MODE_LEGACY) { 
      if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _buffer.getu8(0) & 0xf;}
      else {return _buffer.getu8(4) & 0xf;}
    }
    else { return getSpecialTrackerEventType(); }
  }


  u16 Fed9UEvent::getSourceId()      const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _buffer.getu16(5, true) & 0xfff;}
    else {return _buffer.getu16(1, true) & 0xfff;}
  }

  u8  Fed9UEvent::getFormatVersion() const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _buffer.getu8(7) >>  4;}
    else {return _buffer.getu8(3) >>  4;}
  }

  u32 Fed9UEvent::getTotalLength()   const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _trailer.getu32(0, true) & 0xffffff;}
    else {return _trailer.getu32(4, true) & 0xffffff;}
  }

  u16 Fed9UEvent::getCrc()           const { 
    if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {return _trailer.getu16(4, true);}
    else {return _trailer.getu16(0, true);}
  }
  //</JEC>


  u32 Fed9UEvent::getDaqRegister(void) const {
    //Check we have a full debug header, if not then return zero.
    if (getSpecialHeaderFormat() == FED9U_HEADER_FULLDEBUG) { 
      return feUnit(7).getBytes10To13FromFeHeader();
    }
    else {
      return 0;
    }
  }
  /**Method to read the DAQ register Two from the 7th FE header in a full debug mode event. 
     It returns zero if there is no full debug mode event.*/
  u32 Fed9UEvent::getDaqRegisterTwo(void) const {
    //Check we have a full debug header, if not then return zero.
    if (getSpecialHeaderFormat() == FED9U_HEADER_FULLDEBUG) { 
      return feUnit(6).getBytes10To13FromFeHeader();
    }
    else {
      return 0;
    }
  }


  u32 Fed9UEvent::getBeStatusRegister(void) const {
    //Check we have a full debug header, if not then return zero.                                                                   
    if (getSpecialHeaderFormat() == FED9U_HEADER_FULLDEBUG) {
      return feUnit(0).getBytes10To13FromFeHeader();
    }
    else {
      return 0;
    }
  }


  u8 Fed9UEvent::getSpecialFirstByte(void) const {
    return (_buffer.getu8(SPECIAL_OFF) & 0xFF);  
  }


  Fed9UHeaderFormat Fed9UEvent::getSpecialHeaderFormat(void) const {
    return static_cast<Fed9UHeaderFormat>((_buffer.getu8(SPECIAL_OFF+1) >> 4) & 0xF);
  }


  u8 Fed9UEvent::getSpecialTrackerEventType(void) const {
    return (_buffer.getu8(SPECIAL_OFF+1) & 0xF);
  }


  u8 Fed9UEvent::getSpecialApvEmulatorAddress(void) const {
    return (_buffer.getu8(SPECIAL_OFF+2) & 0xFF);
  }
  

  u8 Fed9UEvent::getSpecialApvAddressError(void) const {
    return (_buffer.getu8(SPECIAL_OFF+3) & 0xFF);
  }


  u8 Fed9UEvent::getSpecialFeEnableReg(void) const {
    return (_buffer.getu8(SPECIAL_OFF+4) & 0xFF);
  }


  u8 Fed9UEvent::getSpecialFeOverflowReg(void) const {
    return (_buffer.getu8(SPECIAL_OFF+5) & 0xFF);
  }


  u16 Fed9UEvent::getSpecialFedStatusRegister(void) const {
    return (_buffer.getu16(SPECIAL_OFF+6, true) & 0xFFFF);
  }


  /**
   * \brief Templated method to print a value of any type in hex to an output stream.
   * \param os Output stream to write the data to.
   * \param name Name of the data that is to be written, will preceed the data in the output stream.
   * \param val The value of type T that is to be written to the stream as a hex value.
   * \param bits The number of bits that is contained in the data type. Defaulted to the size of T in bytes times the number of bits in a byte.
   *
   * The template argument, class T, is the type of object that is to be written to the stream.
   */  
  template<class T>
  static void printValue(std::ostream& os, string name, T val, int bits = CHAR_BIT*sizeof(T)) {
    const int nibbles = (bits+3) / 4;
    const unsigned namelen = 20;
    string space;
    if (name.length() < namelen) space = string(namelen - name.length(), ' ');
    os.width(0);
    os.setf(ios::uppercase);
    os << name << space
	 << " = 0x" << hex << setw(nibbles) << setfill('0')
         << static_cast<u32>(val) << string(8 - nibbles, ' ')
         << " = " << dec << static_cast<u32>(val) << endl;
  }
 

  /* 
     #define PRINTVALUES(os, name, func)\
     os << name << endl;              \
     for (int i = 0; i < 8; i++) {    \
     stringstream s;                \
     s << "        " << i;          \
     printValue(os, s.str(), func(i)); \
     }
  */


  void Fed9UEvent::dump(std::ostream& os) const {
    printValue(os, "DAQ Event Number", getEventNumber(), 24);
    printValue(os, "DAQ Bunch Crossing", getBunchCrossing(), 12);
    printValue(os, "DAQ Event Type", getEventType(), 4);
    printValue(os, "DAQ Source Id", getSourceId(), 12);
    printValue(os, "DAQ Format Version", getFormatVersion(), 4);
    printValue(os, "DAQ Total Length", getTotalLength(), 24);
    printValue(os, "DAQ CRC", getCrc());
    printValue(os, "Calculated CRC", calcCrc());
 
    for (u16 i = 0; i < feUnits(); i++) {
      const Fed9UEventUnit & u = feUnit(i);
      os << "\nFeUnit " << static_cast<int>(Fed9UAddress().setFedFeUnit(i).getExternalFedFeUnit()) << flush;
      os << ", Pipeline " << static_cast<int>(u.getMajorityPipeline()) << flush;
      os << ", Length " << u.dataLength() << endl;

      if (u.channels() > 0) os << "Ch  PC St Len  Num    S1   S2  ..." << endl;
      for (u16 j = 0; j < u.channels(); j++) {
        const Fed9UEventChannel & c = u.channel(j);
        os << setw(2) << setfill(' ') << static_cast<int>(Fed9UAddress().setFedFeUnit(i).setFeUnitChannel(j).getExternalFeUnitChannel()) << ": "
           << setw(2) << hex << static_cast<int>(c.getPacketCode())
           << ' ' << setw(2) << static_cast<int>(c.getStatus())
           << ' ' << setw(4) << dec << c.dataLength() << ' '
           << setw(4) << c.samples();
        for (u16 k = 0; k < c.samples(); k++) {
          os << ' ' << setw(4)  << c.sample(k);
          if (k > 32) break;
        }
        os << endl;
      }
      //os << "debug: end of unit" << endl;
    }
  }


  void Fed9UEvent::dumpRawBuffer(std::ostream& os) const {
    u32 bufferLength = _buffer.size();
    std::cout << "getTotalLength() * 8  = " \
	      << std::hex 
	      << bufferLength 
	      << "buffer size = " << _buffer.size() << std::endl;
    char temp=0;
    os.write(&temp,1); // for binary event data we set the first byte to zero (for text apv frames we set it to 1)
    std::cout << "written first byte to file!!!" << std::endl;
    
    // fill the first 4 bytes with the buffer length 
    os.write(reinterpret_cast<char*>(&bufferLength),4);
    std::cout << "written buffer length to file to file!!!" << std::endl;
    // TODO what should this line actually do ???
    os.write(reinterpret_cast<const char*>(&_buffer[0]-3),static_cast<int>(bufferLength));
    std::cout << "written data to file!!!" << std::endl;
  }

  template <typename T>
  static void push_bits(std::vector<bool> & b, const T & t) {
    enum { size = sizeof t * CHAR_BIT };
    for (int i = 0; i < size; i++) {
      b.push_back((t & (1UL << i)) != 0);
    }
  }


  Fed9UEventUnit::Fed9UEventUnit(Fed9UEventIterator header, Fed9UEventIterator data, Fed9UHeaderFormat headerformat)
    : _header(header), _data(data), _headerFormat(headerformat)
  {
    enum { STATUS_BITS = 6, STATUS_TOTAL = STATUS_BITS * CHANNELS_PER_FEUNIT, STATUS_REDUCED =  APVS_PER_CHANNEL * CHANNELS_PER_FEUNIT };
    _channels.reserve(CHANNELS_PER_FEUNIT);
    _header.resize(16);

    ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("Fed9UEventUnit does not support APV Error mode!");

    u16 len = 0;
    len = _header.getu16(8, true);
	  
	// Packet Codes:
	// Are stored in the firmware as follows: "1" & not mode_a & not mode_b & ProcessedRaw & ZeroSuppressed & VirginRaw & FrameFinding & Scope
	// note that mode_a and mode_b, which are bits 3 and 2 of the super mode, are inverted for backward compatibility:d mode_a = mode_b = 0 implies old style data 
	// 0xF2 Processed Raw
	// 0xEA Zero Suppressed 8 bit Top 2 bits stripped
	// 0xE6 Virgin Raw 16 Bit
	// 0xE5 SCOPE Mode
	// 0x8A Zero Suppressed 10 bit readout
	// 0x86 Virgin Raw 10 Bit
	// 0xCA Zero Suppressed 8 bit bottom two bits stripped
	// 0xAA Zero Suppressed 8 bit one top one bottom bits stripped
	  
    if (len > 0) ICUTILS_VERIFYX( // verify that the packet code is one of the known ones. 
									(data.getu8(2) == 0xf2 ) ||
									(data.getu8(2) == 0xea ) ||
									(data.getu8(2) == 0xe6 ) ||
									(data.getu8(2) == 0xe5 ) ||
									(data.getu8(2) == 0x8A ) ||
									(data.getu8(2) == 0x86 ) ||
									(data.getu8(2) == 0xaa ) ||
									(data.getu8(2) == 0xca ) ||
                                                                        (data.getu8(2) == 0xA6 ) ||
           //added VR bit stripped codes (A Baty, 7/7/2015)
                                                                        (data.getu8(2) == 0xC6 ) ||
                                                                        (data.getu8(2) == 0x92 ) ||
                                                                        (data.getu8(2) == 0xD2 ) ||
                                                                        (data.getu8(2) == 0xB2 ) 
									, Fed9UEventException  )(static_cast<u16>(data.getu8(2))).msg("Front End Unit Packet Code is corrupt!").code(Fed9UEventException::ERROR_FRONT_END_PACKET_CODE_CORRUPT).error();
    DBG("length = " << len);
    ICUTILS_VERIFY(len <= _data.size())(len)(_data.size()).error()
      .msg("Length in tracker header is longer than length available in buffer");
    _data.resize(len);
    DBG(" -> " << dataLength());

    if (len) {
      // strips out of the status word for each channel and then creates the channel
      std::vector<bool> b;
      b.reserve(STATUS_TOTAL);
      push_bits(b, header.getu32(4, true));
      push_bits(b, header.getu32(0, true));
      push_bits(b, header[15]);
      for (int f = 0; f < CHANNELS_PER_FEUNIT; f++) {
	DBG("Channel " << dec << f);
	int status = 0;
	for (int i = 0; i < STATUS_BITS; i++) {
	  status <<= 1;
	  status |= b.back();
	  b.pop_back();
	}
        DBG("Status " << status << ", Iterator " << data);
        try {
	  // Fed9UEventChannel does not need to know about the header format
          _channels.push_back(Fed9UEventChannel(data, status));
        } catch (const exception & e) {
          stringstream temp;
          temp << "Error detected while constructing channel " << f;
          RETHROW(e, ICUtils::ICException(temp.str()));
        }
        DBG("Length = " << _channels.back().dataLength());
        try {
          data += _channels.back().dataLength();
        } catch (const exception & e) {
          stringstream temp;
          temp << "Error detected after constructing channel " << f;
          RETHROW(e, ICUtils::ICException(temp.str()));          
        }
      } // end of loop over FE-unit channels
    } // length check
  }


  Fed9UEventChannel::Fed9UEventChannel(Fed9UEventIterator data, u8 status) :
    _status(status), _data(data)
  {
    //try {
      _data.resize(dataLength());
      //} catch (const exception & e) {
      //RETHROW(e, ICUtils::ICException("Corrupt event detected"));
      //}
  }


  u16 Fed9UEventChannel::samples() const {
    // JF 11/11/2005 swapped this with line below    if (getPacketCode() & FED9U_PACKET_SCOPE) {
	// JF 2/5/2015 ten years on we add a bunch of new data packing modes
	// AAB 8/24/2015 Added support for Bit Stripped modes (HI,LO,10BIT,etc)
    if (  getPacketCode() == FED9U_PACKET_VIRGRAW ||
		  getPacketCode() == FED9U_PACKET_SCOPE ||
		  getPacketCode() == FED9U_PACKET_PROCRAW ) {
      return (dataLength() - 3) / 2;
    } else if (getPacketCode() == FED9U_PACKET_ZEROSUPP ||
			   getPacketCode() == FED9U_PACKET_ZEROSUPP_HI_LO ||
			   getPacketCode() == FED9U_PACKET_ZEROSUPP_LO) {
		return 0;// no way to calculate
	} else if ( getPacketCode() == FED9U_PACKET_ZEROSUPP_10BIT ) { // 10 bit 
		return 0;// no way to calculate
	} else if ( getPacketCode() == FED9U_PACKET_VIRGRAW_10BIT ||
                    getPacketCode() == FED9U_PACKET_PROCRAW_10BIT ) { // 10 bit
                return (dataLength()-3)*4/5;
        } else if ( getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_LO || 
                    getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_LO ) { // 8 bit
                return (dataLength()-3);
        } else if ( getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_HI_LO ||
                    getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_HI_LO) { // 8 bit
                return (dataLength()-3);
	} else // must be Zero supp lite
		return STRIPS_PER_APV * APVS_PER_CHANNEL;
  }



  u16 Fed9UEventChannel::sample(size_t i) const {
    /*Commented block out for the addition below (6/30/2015 A. Baty)
    if (getPacketCode() & FED9U_PACKET_ZEROSUPP) {
      // could be more efficient, but should work for now...
      DBG("Debug: Got sample " << i << " val = " << temp[i] << endl);
      //std::vector<u16> temp = getSamples();
      //return temp[i];
    }*/

    //Added for 10 bit unpacker (6/30/2015 A. Baty)
    if(getPacketCode() == FED9U_PACKET_VIRGRAW_10BIT ||
                getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_LO ||
                getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_HI_LO ||
                getPacketCode() == FED9U_PACKET_VIRGRAW_10BIT ||
                getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_LO ||
                getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_HI_LO ||
                getPacketCode() == FED9U_PACKET_PROCRAW_10BIT ||
       		getPacketCode() == FED9U_PACKET_ZEROSUPP) {
      std::vector<u16> temp = getSamples(); 
      return temp[i];
    }
    return _data.getu16(i*2+3);
  }


  void Fed9UEventChannel::getSamples(u16 * destBuf) const {
    //    std::vector<u16> ret;
    try {
      if (getPacketCode() == FED9U_PACKET_ZEROSUPP) {
	//DBG("DEBUG: Zero suppressed mode");
	memset(static_cast<void*>(destBuf), 0, STRIPS_PER_APV * APVS_PER_CHANNEL * sizeof (*destBuf));
	for (Fed9UEventIterator i = _data + 7; i.size() > 0; /**/) {
          // needs error checking...
          u8 add = *i++;
          u8 len = *i++;
          for (u16 j = 0; j < len; j++) {
            destBuf[add+j] = *i++;
          }
	  //DBG("DEBUG: Cluster at " << dec << (int)add << " size " << (int)len << " data " << destBuf[add] << endl);
        }
      } else if (getPacketCode() == FED9U_PACKET_ZEROSUPP_LO) {
		  ;
	  } else if (getPacketCode() == FED9U_PACKET_ZEROSUPP_HI_LO) {
		  ;
	  } else if(getPacketCode() == FED9U_PACKET_ZEROSUPP_10BIT) {
		  ;
	  } else if(getPacketCode() == FED9U_PACKET_VIRGRAW_10BIT ||
                    getPacketCode() == FED9U_PACKET_PROCRAW_10BIT) {
                  //Added 6/30/2015 by A Baty. for 10 bit unpacking
                  //currently not sure if stable when channels are turned off...
                  memset(static_cast<void*>(destBuf), 0, STRIPS_PER_APV * APVS_PER_CHANNEL * sizeof (*destBuf));
                  Fed9UEventIterator i = _data+3; 

	          //bitOverlap keeps track of the number of bits 'bleeding' into the next byte because we are storing  10 bit word in 8bit bytes, has a periodic structure of 2,4,6,8,2.... as we read out the data
                  int bitOverlap = 2; 
                  for (int j = 0; i.size() > 0; j++) {
                    //some clever manipulation of bits in order to extra the 10 bit words out of an 8 bit buffer into a 16 bit buffer...
                    destBuf[j] = *i;//get the first byte
                    //std::cout << j << " " << destBuf[j] << " : " << flush;
                    destBuf[j] = destBuf[j] << bitOverlap;//make enough room for the overlapping bits in the 2nd byte
                    //std::cout << destBuf[j] << " : " << flush;
                    i++;// point to next byte
                    destBuf[j] = destBuf[j] | (*i >> (8-bitOverlap));//get remaining bits needed
                    //std::cout << destBuf[j] << " : " << flush;
                    destBuf[j] = destBuf[j] & 1023; //zeroes for safety the top 6 bits of our 16 bit structure (leaving the 10 bit word)
                    //std::cout << destBuf[j] << " : " << bitOverlap<<std::endl;
                    if(bitOverlap == 8) i++;//increment again if the 2nd byte was fully used
                    bitOverlap = bitOverlap%8+2;//update overlap*/
                  }
	  } else if(getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_LO ||
                    getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_LO ) {
                   for (u16 i = 0; i < samples(); i++) {
                     destBuf[i] = _data.getu8(i+3);
                     destBuf[i] = destBuf[i] << 2;
                   }
          } else if(getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_HI_LO ||
                    getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_HI_LO ) {
                   for (u16 i = 0; i < samples(); i++) {
                     destBuf[i] = _data.getu8(i+3);
                     destBuf[i] = destBuf[i] << 1;
                   }
          } else { // Proc raw, VR, SCOPE, etc. use samples to return number of samples
        //ret.resize(samples());
        for (u16 i = 0; i < samples(); i++) {
          std::cout << i << " " << destBuf[i] << std::endl;
          destBuf[i] = _data.getu16(i*2+3);
        }
      }    
    } catch (exception & exc) {
      RETHROW(exc,Fed9UEventException(Fed9UEventException::ERROR_GET_SAMPLES_FAILED,"An error of type exception ocurred in method getSamples()"));
    } catch (...) {
      THROW(Fed9UEventException(Fed9UEventException::ERROR_UNKNOWN,"An unknown error occurred in method getSamples()"));
    }
    
    
    //return ret;
  }


  std::vector<u16> Fed9UEventChannel::getSamples() const {
    std::vector<u16> ret;
    //if (getPacketCode() & FED9U_PACKET_ZEROSUPP) {
    //Commented out for if statement below 6/30/2015 A. Baty)
    if ( getPacketCode()== FED9U_PACKET_ZEROSUPP ||
        	getPacketCode() == FED9U_PACKET_VIRGRAW_10BIT ||
                getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_LO ||
                getPacketCode() == FED9U_PACKET_VIRGRAW_8BIT_HI_LO ||
        	getPacketCode() == FED9U_PACKET_PROCRAW_10BIT ||
                getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_LO ||
                getPacketCode() == FED9U_PACKET_PROCRAW_8BIT_HI_LO) {
      ret.resize(STRIPS_PER_APV * APVS_PER_CHANNEL);
    } else {
      ret.resize(samples());
    }
    getSamples(&ret[0]);
    /*DBG("VECTOR CONTAINS! (");
    for (int i = 0 ; i < 256 ; i++) {
      cout << dec << ret[i] << ",";
    }
    cout << " ) " << endl;
    */
    return ret;
  }


  void Fed9UEvent::saveIgorFile(std::ostream & o) const {
    o << "IGOR\n" << dec
      << "WAVES/D/N=(" << samples() << ", " << totalChannels() << ") wave\n"
      << "BEGIN" << endl;
    for (int i = 0; i < samples(); i++) {
      for (int j = 0; j < totalChannels(); j++) {
        o << channel(j).sample(i) << '\t';
      }
      o << endl;
    }
    o << "END" << endl;
  }

  // <NAC date="04/05/2009"> Refactored CRC code into separate header
  u16 Fed9UEvent::calcCrc() const {
    return calculateFEDBufferCRC(_buffer.getPtr(),_buffer.size()-_rubbish,_eventFormat == FED9U_EVENT_FORMAT_OLD_VME);
  }

 
  u16 Fed9UEvent::calcCrcDirty() const {
    return calculateFEDBufferCRC(_buffer.getPtr(),_buffer.size(),_eventFormat == FED9U_EVENT_FORMAT_OLD_VME);
  }
  //</NAC>
}

