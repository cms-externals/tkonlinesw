#include "Fed9UEventStreamLine.hh"
#include "Fed9UEventStreamLineException.hh"
#include "Fed9UEventUnitStreamLine.hh"
#include "Fed9USu8.hh"
#include "Fed9UFakeBufferCreator.hh"
#include "ICAssert.hh"
#include "Fed9UDescription.hh"
// <NAC date="04/05/2009"> refactored CRC into different file
#include "Fed9UCrc.hh"
// </NAC>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <climits>
#include <bitset>
#include <cstring>
using namespace std;

namespace Fed9U {

  // constructor for real data
  Fed9UEventStreamLine::Fed9UEventStreamLine(u32 * buffer, const Fed9UDescription * currentDescription) : _rubbish(0), _bufferLength(0), _deleteBuffer(false) {
    fakeBuffer = NULL;
    _daqHeader = NULL;
    _trackerHeader = NULL;
    _trailer = NULL;
    _feUnitHeader = NULL;
    _payload = NULL;
    _fedDescription = NULL;

    _mode = FED9U_EVENT_MODE_REAL;
    _eventFormat = FED9U_EVENT_FORMAT_STANDARD;
    _headerFormat = FED9U_HEADER_FULLDEBUG;
    Init(buffer,currentDescription);
  }

  // Constructor for fake data
  // Added support for multiple channels if wanted(8/26/2015 AAB)
  Fed9UEventStreamLine::Fed9UEventStreamLine(istream & is, int uniqueChannels = 1) : _fedDescription(0), _rubbish(0), _bufferLength(0), _deleteBuffer(true) {
    fakeBuffer = NULL;
    _daqHeader = NULL;
    _trackerHeader = NULL;
    _trailer = NULL;
    _feUnitHeader = NULL;
    _payload = NULL;
    
    _eventFormat = FED9U_EVENT_FORMAT_STANDARD;
    _headerFormat = FED9U_HEADER_FULLDEBUG;
    fakeBuffer = new Fed9UFakeBufferCreator(is, uniqueChannels);
    setEventDataMode(fakeBuffer->getDataMode());
    Init(fakeBuffer->getPointerToBuffer(),NULL);
  }

  Fed9UEventStreamLine::~Fed9UEventStreamLine() {
    if (_deleteBuffer) {delete fakeBuffer;}
    //<JEC date=21/03/07>  no longer needed as actual objects are stored, not pointers
    //    for (u32 i = 0; i < _feUnits.size(); ++i) {delete _feUnits[i];}
    //</JEC>
  }
  
  // Initialisation method which applies for both real and fake data
  void Fed9UEventStreamLine::Init(u32 * buffer, const Fed9UDescription * fedDescription) {
    try {
      _feUnits.clear();
      _fedDescription = fedDescription;
      _buffer = buffer;
      
      // point the daqHeader pointer to the start of the DAQ header
      _daqHeader = reinterpret_cast<u8*>(buffer);
      // pointer starts at the beginning of the DAQ header.
      // move to the start of the tracker header (remember that we move in multiples of 32 bits !)
      buffer += 2;
      _bufferLength += 8;
      _trackerHeader = reinterpret_cast<u8*>(buffer);
      // use trackerHeader pointer to decide start mode decision making process
      // reserved bits will only equal 0xED in final data format, not earlier data eg. test beam
      if (*(_trackerHeader + 3) == 0xED) {
	_headerFormat = getHeaderFormat();
	_eventFormat = FED9U_EVENT_FORMAT_OLD_VME;
	// buffer currently points to general header information, so move it to the start of the FE_FPGA information
	buffer += 2;
	_bufferLength += 8;
      } else if (*(_trackerHeader + 3) == 0xC5) {
	_headerFormat = getHeaderFormat();
	_eventFormat = FED9U_EVENT_FORMAT_STANDARD;
	// buffer currently points to general header information, so move it to the start of the FE_FPGA information
	buffer += 2;
	_bufferLength += 8;
      } else {
	_mode = FED9U_EVENT_MODE_LEGACY;
	// In legacy format, events are always in full_debug mode ...
	_headerFormat = FED9U_HEADER_FULLDEBUG;
	_eventFormat = FED9U_EVENT_FORMAT_OLD_VME;
      }
      
      // Set up pointer which is used to access FE unit header information
      _feUnitHeader = reinterpret_cast<u8*>(buffer);
      
      // Set up pointer to the payload
      if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
	buffer += (FEUNITS_PER_FED * 4);
	_bufferLength += (FEUNITS_PER_FED * 16);
      } else if (_headerFormat == FED9U_HEADER_APVERROR) {
	buffer += (FEUNITS_PER_FED * 3)/4;
	_bufferLength += (FEUNITS_PER_FED * 3);
      }
      _payload = reinterpret_cast<u8*>(buffer);

      // now that payload pointer is set up, create the vector of FE-FPGA pointers to the payload
      try {
	setPointerToUnitDataInPayload();
      } catch (const exception & e) {
        RETHROW(e, ICUtils::ICException("Fed9UEventStreamLine::init(): Unable to access all FE-Units in the event record!"));
      }

      // calculate the total length of the payload
      for (u32 i = 0; i < FEUNITS_PER_FED; ++i) {
	//	if (_feUnits[i] != 0) {
	if (getFeUnitEnable(8 - i)) {
	  _bufferLength += _feUnits[i].getUnitDataLengthFromPayload();
	  if ((_bufferLength % 8) > 0) {_bufferLength += (8 - (_bufferLength % 8));}
	} 
      }
      
      // finally set up the pointer to the trailer
      int lastPtr = 7;
      //      while (_feUnits[lastPtr] == 0 && lastPtr >= 0) {lastPtr--;}
      while (!getFeUnitEnable(8 - lastPtr) && lastPtr >= 0) {lastPtr--;}
      if (lastPtr >= 0) {
	_trailer = _feUnits[lastPtr].getPointerToEndOfUnitPayload();
      } 
      if (_trailer != NULL) {
	bool contBit = false;
	if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {contBit = static_cast<bool>(*(_trailer + 4) & 0x8);}
	else {contBit = static_cast<bool>(*_trailer & 0x8);}
	
	while (contBit) {
	  _rubbish += 8;
	  _trailer += 8;
	  if (_eventFormat == FED9U_EVENT_FORMAT_OLD_VME) {contBit = static_cast<bool>(*(_trailer + 4) & 0x8);}
	  else {contBit = static_cast<bool>(*_trailer & 0x8);}
	}
      }
    } catch (const exception & e) {
      RETHROW(e, ICUtils::ICException("Fed9UEventStreamLine::init():  Failure to initialise correctly.  Maybe event data is corrupted ?"));
    }
  }
  
  void Fed9UEventStreamLine::setPointerToUnitDataInPayload() {
    _feUnits.resize(FEUNITS_PER_FED);
    // inc is the variable which allows us to step through the payload, finding the start of the data from each FE unit
    u32 inc = 0;
    //    u16 channelLength = 0;
    for (u32 f = 0; f < FEUNITS_PER_FED; ++f) {
      //      channelLength = 0;
      // If this FE-unit is enabled
      try {
	if ((getFeEnableRegister() != 0) && (getFeUnitEnable(8 - f))) {
	  //<JEC date=13/03/07> change to using actual objects not pointers
	  //	  _feUnits[f] = new Fed9UEventUnitStreamLine(_feUnitHeader,(_payload + inc),_headerFormat,(8-f));
	  _feUnits[f] = Fed9UEventUnitStreamLine(_feUnitHeader,(_payload + inc),_headerFormat,(8-f));
	  //</JEC>

	  //<JEC date=27/02/07> no longer need to access all channels to set up the pointers - channel data lengths accessed in Fed9UEventUnitStreamLine anyway
	  inc += static_cast<u32>(_feUnits[f].getUnitDataLengthFromPayload());
	  // unit length does not include padding bytes
	  u32 paddingBytes = inc - ((inc/8)*8);
	  if (paddingBytes != 0) {inc += (8 - paddingBytes);}
	  /*
	    u32 remainingBytes = 0, numberOf64bitWords = 0;
	    for (int i = 0; i < CHANNELS_PER_FEUNIT; ++i) {
	    inc += (numberOf64bitWords*8);
	    
	    int lowerWd = 3 - static_cast<int>(remainingBytes);
	    if (lowerWd < 0) {lowerWd += 8;}
	    int upperWd = 3 - static_cast<int>(remainingBytes) - 1;
	    if (upperWd < 0) {upperWd += 8;}
	    // In this case upperWd ends up on the next line, so need to account for it !
	    if (remainingBytes == 7) {upperWd += 8;}
	    
	    // determine the data length for the fibre, as stored in the payload and store it.
	    channelLength = ((*(_payload + inc + upperWd) & 0xF) << 8) | (*(_payload + inc + lowerWd));
	    
	    numberOf64bitWords = (static_cast<u32>(channelLength) - (8 - remainingBytes))/8;
	    remainingBytes = (static_cast<u32>(channelLength) - (8 - remainingBytes)) % 8;
	    inc += 8;
	    } // end of loop over channels
	    
	    // move inc on to the start of the next FE unit, based on the channel data length
	    inc += (numberOf64bitWords*8);
	    if (remainingBytes != 0) {
	    inc += 8;
	    }
	  */
	  //</JEC>
	  
	} else {
	  cout << "FE-FPGA " << 8 - f << " not enabled in setPointerToUnitDataInPayload !" << endl;
	  Fed9UEventUnitStreamLine tempUnit;
	  _feUnits[f] = tempUnit;
	}
      } catch (const exception & e) {
	stringstream temp;
	temp << " Fed9UEventStreamLine::setPointerToUnitDataInPayload():  Error detected while constructing FE-FPGA " << f;
	RETHROW(e, ICUtils::ICException(temp.str()));
      }
    } // end of loop over FE-units
  }
  
  Fed9UHeaderFormat Fed9UEventStreamLine::getHeaderFormat() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return static_cast<Fed9UHeaderFormat>((*(_trackerHeader + 2) >> 4) & 0xF);} 
    return FED9U_HEADER_FULLDEBUG;
  }
  
  u8 Fed9UEventStreamLine::getTrackerEventType() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return (*(_trackerHeader + 2) & 0xF);}
    return 0;
  }
  
  u8 Fed9UEventStreamLine::getApvEmulatorAddress() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return *(_trackerHeader + 1);}
    return 0;
  }
  
  u8 Fed9UEventStreamLine::getApvAddressError() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return *(_trackerHeader);}
    return 0;
  }
  
  u16 Fed9UEventStreamLine::getStatusRegister() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return *(_trackerHeader + 4);}
    return 0;
  }
  
  u8 Fed9UEventStreamLine::getFeOverflowRegister() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return *(_trackerHeader + 6);}
    return 0;
  }

  u8 Fed9UEventStreamLine::getFeEnableRegister() const {
    if (_mode != FED9U_EVENT_MODE_LEGACY) {return *(_trackerHeader + 7);}
    return 0;
  }

  u32 Fed9UEventStreamLine::getReservedWordFromFullDebugHeader(u32 unit) const {
    ICUTILS_VERIFY(unit > 0 && unit <=8)(unit).msg("getReservedWordFromFullDebugHeader:  wrong FE-FPGA number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
    if (_headerFormat ==  FED9U_HEADER_FULLDEBUG) {
      u32 offset = (8 - unit)*16;
      return (*(_feUnitHeader + offset + 14) | (*(_feUnitHeader + offset + 15) << 8) | (*(_feUnitHeader + offset + 8) << 16) | (*(_feUnitHeader + offset + 9) << 24));
    }
    return 0;
  }

  // JEC 24/07/06:  Copied from the original Fed9UEvent, but modified to deal with both FULL DEBUG and APV ERROR modes
  void Fed9UEventStreamLine::checkChannelStatuses() const {
    ICUTILS_VERIFY(_fedDescription != NULL )(_fedDescription).msg("you must supply a valid description if you want to check channel statuses.").error();
    Fed9UAddress addr;
    stringstream statusErrorString;
    
    if ( _fedDescription->getDaqMode() != FED9U_MODE_SCOPE ) {
      for (u32 f = 0; f < FEUNITS_PER_FED; f++) {
	if (! _fedDescription->getFedFeUnitDisable(addr.setFedFeUnit(f))) {
	  for (u32 c = 0; c < CHANNELS_PER_FEUNIT; c++) {
	    if ( _fedDescription->getApvDisable(addr.setFeUnitChannel(c).setChannelApv(true)) || 
		 _fedDescription->getApvDisable(addr.setChannelApv(false)) ) {
	      continue;
	    }
	    ICUTILS_VERIFY(getFeUnitEnable(8 - f))(getFeUnitEnable(8 - f))(f).msg("FE-FPGA is disabled !").error().code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
	    //	    ICUTILS_VERIFY(_feUnits[f] != 0 )(_feUnits[f])(f).msg("FE-FPGA is disabled !").error().code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
	    u8 status = _feUnits[f].getChannelStatus(c+1);
	    if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
	      if (status != 0x3F) {statusErrorString << "\nFeUnit " << f+1 << ", Channel " << c+1 << " Has Errors: ";}
	      if ((status & 0x1) != 0x1) {statusErrorString << "\nAPV0:  APV Error B";}
	      if ((status & 0x2) != 0x2) {statusErrorString << "\nAPV0:  wrong header B";}
	      if ((status & 0x4) != 0x4) {statusErrorString << "\nAPV1:  APV Error B";}
	      if ((status & 0x8) != 0x8) {statusErrorString << "\nAPV1:  wrong header B";}
	      if ((status & 0x10) != 0x10) {statusErrorString << "\nOut of Synch B";}
	      if ((status & 0x20) != 0x20) {statusErrorString << "\nLock failed";}
	      if (status != 0x3F) {statusErrorString << endl;}
	    } else if (_headerFormat == FED9U_HEADER_APVERROR) {
	      if (status != 0x3) {statusErrorString << "\nFeUnit " << f+1 << ", Channel " << c+1 << " Has Errors: ";}
	      if ((status & 0x1) != 0x1) {statusErrorString << "\nAPV0:  lock failed or out of synch B or APV error B";}
	      if ((status & 0x2) != 0x2) {statusErrorString << "\nAPV1:  lock failed or out of synch B or APV error B";}
	      if (status != 0x3) {statusErrorString << endl;}
	    } else {
	      cout << "header format not recognised " << endl;
	    }
	  }
	}
      }
    } else {
      statusErrorString << "FPGAs are in scope mode - frame header information not available !" << endl;
    }
    ICUTILS_VERIFY(statusErrorString.str().size() == 0).msg(statusErrorString.str().c_str()).error();
  }

  void Fed9UEventStreamLine::checkEvent() const {
    u32 totalLength = _bufferLength + _rubbish + 8;
    ICUTILS_VERIFY(8*getTotalDataLength()+_rubbish == totalLength)(8*getTotalDataLength())(_rubbish)(totalLength)
      .error().msg("DAQ Length does not match actual length");
    ICUTILS_VERIFY(getCrc() == calcCrc())(getCrc())(calcCrc())
      .error().msg("Calculated CRC does not match CRC in DAQ Trailer");

    switch (getTrackerEventType()) {
    case 1: 
      cout << "This is REAL data" << endl;
      cout << "Data Format is:  SCOPE MODE" << endl;
      break;
    case 2:
      cout << "This is REAL data" << endl;
      cout << "Data Format is:  VIRGIN RAW MODE" << endl;
      break;
    case 3:
      cout << "This is FAKE data" << endl;
      cout << "Data Format is:  VIRGIN RAW MODE" << endl;
      break;
    case 6:
      cout << "This is REAL data" << endl;
      cout << "Data Format is:  PROCESSED RAW MODE" << endl;
      break;
    case 7:
      cout << "This is FAKE data" << endl;
      cout << "Data Format is:  PROCESSED RAW MODE" << endl;
      break;
    case 10:
      cout << "This is REAL data" << endl;
      cout << "Data Format is:  ZERO SUPPRESSED MODE" << endl;
      break;
    case 11:
      cout << "This is FAKE data" << endl;
      cout << "Data Format is:  ZERO SUPPRESSED MODE" << endl;
      break;
    case 12:
      cout << "This is REAL data" << endl;
      cout << "Data Format is:  ZERO SUPPRESSED LITE MODE" << endl;
      break;
    case 13:
      cout << "This is FAKE data" << endl;
      cout << "Data Format is:  ZERO SUPPRESSED LITE MODE" << endl;
      break;
    default:
      ICUTILS_VERIFYX(false, Fed9UEventStreamLineException)(getTrackerEventType()).error().code(Fed9UEventStreamLineException::ERROR_DAQ_MODE_UNKNOWN)
	.msg("Unrecognised event type"); 
    }
  }

  // JEC 10/08/06 - return the number of FE-units
  u32 Fed9UEventStreamLine::enabledFeUnits() const {
    u32 numberOfEnabledFeUnits = 0;
    for (unsigned int loop = 0; loop < FEUNITS_PER_FED; loop++) {
      // If this FE-unit is enabled
      if (getFeUnitEnable(8 - loop)) {numberOfEnabledFeUnits++;}
    }
    return numberOfEnabledFeUnits;
  }
  
  // JEC 21/8/06 - stolen from Fed9UEvent and modified
  void Fed9UEventStreamLine::dumpRawBuffer(ostream& os) const {
    u32 bufferLength = _bufferLength + 8 + _rubbish;
    std::cout << "Total length in bytes is " << std::hex << bufferLength << std::dec << std::endl;
    char temp = 0;
    os.write(&temp,1); // for binary event data we set the first byte to zero (for text apv frames we set it to 1)
    std::cout << "written first byte to file!!!" << std::endl;
    
    // fill the first 4 bytes with the buffer length 
    os.write(reinterpret_cast<char*>(&bufferLength),4);
    std::cout << "written buffer length to file to file!!!" << std::endl;
    os.write(reinterpret_cast<const char*>(&_buffer[0]),static_cast<int>(bufferLength));
    std::cout << "written data to file!!!" << std::endl;
  }

  // JEC 21/8/06 - stolen from Fed9UEvent and modified
  void Fed9UEventStreamLine::saveIgorFile(ostream & o) const {
    u32 channel = 12;
    o << "IGOR\n" << dec
      << "WAVES/D/N=(" << _feUnits[0].getSamples(channel) << ", " << enabledFeUnits()*12 << ") wave\n"
      << "BEGIN" << endl;
    
    // loop over enabled FE-Units
    for (u32 s = 0; s < _feUnits[0].getSamples(channel); s++) {
      for (u8 f = 0; f < FEUNITS_PER_FED; f++) {
	if (getFeUnitEnable(8 - f)) {
	  for (u8 c = 0; c < CHANNELS_PER_FEUNIT; c++) {
	    channel = 12 - static_cast<u32>(c);
	    o << _feUnits[f].getSample(channel,s) << '\t';
	  }
	}
      }
      o << endl;
    }
    o << "END" << endl;
  }

  //<JEC date=20/03/07>  made automatic inline by move to header file
  /*
  Fed9UEventUnitStreamLine & Fed9UEventStreamLine::getFeUnit(u32 feUnit) {
    //<JEC date=14/02/07> throw an exception if null pointer is returned
    ICUTILS_VERIFY(feUnit > 0 && feUnit <=8)(feUnit).msg("getFeUnit:  wrong FE-FPGA number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_FEUNIT_OUT_OF_BOUNDS);
    //    ICUTILS_VERIFY(_feUnits[8 - feUnit] != 0 )(_feUnits[8 - feUnit])(feUnit).msg("FE-FPGA is disabled !").error()
    //    ICUTILS_VERIFY(getFeUnitEnable(8 - feUnit))(getFeUnitEnable(8 - feUnit))(feUnit).msg("FE-FPGA is disabled !").error()
    //      .code(Fed9UEventStreamLineException::ERROR_FEUNIT_DISABLED);
    //</JEC>
    return _feUnits[8 - feUnit];
  }
  */
  //</JEC>

  //<JEC date=02/04/07> stolen from Fed9UEvent
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
  //</JEC>
  
  
  // JEC 15/8/06 - dump the raw buffer in a 64-bit hex format
  // <JEC date=02/04/07> modified to make it more like the old version in Fed9UEvent
  // AAB 8/26/2015  Changed output to decimal and also added 'lessOutput' option for FedPatternCheck.cc
  void Fed9UEventStreamLine::dumpBuffer(ostream& os, int modFactor, bool lessOutput) const {
    if(!lessOutput) 
    {
      printValue(os, "DAQ Event Number", getEventNumber(), 24);
      printValue(os, "DAQ Bunch Crossing", getBunchCrossingNumber(), 12);
      printValue(os, "DAQ Event Type", getDaqEventType(), 4);
      printValue(os, "DAQ Source Id", getSourceId(), 12);
      printValue(os, "DAQ Format Version", getDaqFormatVersion(), 4);
      printValue(os, "DAQ Total Length", getTotalDataLength(), 24);
      printValue(os, "DAQ CRC", getCrc());
      printValue(os, "Calculated CRC", calcCrc());
    }
    
    for (u32 f = 0; f < 8; f++) {
      if (getFeUnitEnable(8 - f)) {
	os << "\nFeUnit " << static_cast<int>(Fed9UAddress().setFedFeUnit(f).getExternalFedFeUnit()) << flush;
        if(!lessOutput)	os << ", Pipeline " << static_cast<int>(_feUnits[f].getMajorityPipelineAddress()) << flush;
        if(!lessOutput)	os << ", Length " << _feUnits[f].getUnitDataLengthFromPayload() << endl;
	os << "Ch  PC St  Len  Num  ..." << endl;
	for (u32 c = 0; c < 12; c++) {
	  u32 channel = 12 - c;
	  os << setw(2) << setfill(' ') << static_cast<int>(Fed9UAddress().setFedFeUnit(f).setFeUnitChannel(c).getExternalFeUnitChannel()) << ": ";
	if(!lessOutput)     os << setw(2) << hex << static_cast<int>(_feUnits[f].getPacketCode(channel));
	     os << ' ' << setw(2) << static_cast<int>(_feUnits[f].getChannelStatus(channel));
	if(!lessOutput)     os << ' ' << setw(4) << dec << _feUnits[f].getChannelDataLength(channel) << ' ';
	     os << setw(4) << _feUnits[f].getSamples(channel) << endl;
	  os << "data ..." << endl;
	  vector<u16> samples = _feUnits[f].getSample(channel);
	  for (u32 d = 0; d < samples.size(); d++) {
            //AAB 8/26/2015 old default commented below
	    //os << hex << "0x" << std::setfill('0') << std::setw(3) << static_cast<u32>(samples[d]) << dec << " ";
          
          //Added some if statements here for FedPatternCheck.  'Normal' running w/ FedDebugSuite will skip to the last else statement ('normal' output)
          if(modFactor==2 &&  (static_cast<u32>(samples[d]))>=508 && (static_cast<u32>(samples[d]))<1023)
          {
            //for modes dropping 1 high bit
            os << dec << std::setw(4) << 508 << dec << " "; 
          }
          else if(modFactor==2 && (static_cast<u32>(samples[d]))==1023)
          {
             //for modes dropping 1 high bit w/ saturated signal
             os << dec << std::setw(4) << 510 << dec << " ";
          }
          else if(modFactor==-1)
          {
             //for ZS modes, which reorganize the data, have 8 bits total, and display saturated signals as 255-median
             int samp = samples[2*(4*((static_cast<int>((static_cast<float>(d%128)/8.0)))%4) + static_cast<int>(static_cast<float>(d%128)/32.0) + 16*((d%128)%8))+(d>127?1:0)]-1;
             if(samp<0) os << dec << std::setw(4) << 0 << dec << " ";
             else if(samp>254) os << dec << std::setw(4) << 254 << dec << " ";
             else os << dec << std::setw(4) << samp << dec << " ";
          }
          else  //modes going from 0 to 1023 fully
          {
            os << dec << std::setw(4) << (static_cast<u32>(samples[d]))-(static_cast<u32>(samples[d]))%modFactor << dec << " ";
          }
	    //Added more numbers per line (AAB 8/26/2015)
	    if (((d+1) % 32) == 0) {os << endl;}
	  }
	  os << endl;
	}
      }
    }
  }
  //</JEC>
  
  // <NAC date="04/05/2009"> refactored into different file
  u16 Fed9UEventStreamLine::calcCrc() const {
    return calculateFEDBufferCRC(_daqHeader,_bufferLength+8,_eventFormat == FED9U_EVENT_FORMAT_OLD_VME);
  }
  // </NAC>
  
}

