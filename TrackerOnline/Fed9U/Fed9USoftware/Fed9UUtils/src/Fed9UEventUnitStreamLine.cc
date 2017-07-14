#include "Fed9UEventUnitStreamLine.hh"
#include "Fed9USu8.hh"
#include "Fed9UEventStreamLineException.hh"
#include "Fed9UBufferGenerator.hh"
#include "Fed9UBufferCreator.hh"
#include "Fed9UBufferCreatorRaw.hh"
#include "Fed9UBufferCreatorScopeMode.hh"
#include "ICAssert.hh"
#include "Fed9UDescription.hh"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <climits>
#include <bitset>
#include <cstring>
using namespace std;

namespace Fed9U {

  // Constructor for a FE-Unit
  //<JEC date=27/02/07> construct Fed9UEventUnitStreamLine just with u8 pointers, not constant u8 pointers
  Fed9UEventUnitStreamLine::Fed9UEventUnitStreamLine(u8 * header, u8 * payload, Fed9UHeaderFormat headerFormat, u32 unit) 
    //</JEC>
    : _headerFormat(headerFormat), _unit(unit) {
    
    _header.set(header);
    _payload.set(payload);
    _sample.set(payload);
    _channelDataLength.clear();
    _channelDataLength.resize(CHANNELS_PER_FEUNIT,0);
    
    u32 inc = 0;
    for (int i = 0; i < CHANNELS_PER_FEUNIT; ++i) {
      try {
	if (i > 0) {inc += _channelDataLength[i-1];}
	_channelDataLength[i] = (_payload.getu8(inc+1) & 0xF) << 8 | _payload.getu8(inc);
      } catch (const exception & e) {
	stringstream temp;
	temp << "Fed9UEventUnitStreamLine constructor: Error detected whilst storing data length for channel " << i << " maybe this channel is missing from the data ?";
	RETHROW(e, ICUtils::ICException(temp.str()));
      }
    } // end of loop over channels
    
    /*
      u32 inc = 0;
      u32 remainingBytes = 0, numberOf64bitWords = 0;
      for (int i = 0; i < CHANNELS_PER_FEUNIT; ++i) {
      try {
      inc += (numberOf64bitWords*8);
      int lowerWd = 3 - static_cast<int>(remainingBytes);
      if (lowerWd < 0) {lowerWd += 8;}
      int upperWd = 3 - static_cast<int>(remainingBytes) - 1;
      if (upperWd < 0) {upperWd += 8;}
      // In this case upperWd ends up on the next line, so need to account for it !
      if (remainingBytes == 7) {upperWd += 8;}
      // determine the data length for the fibre, as stored in the payload and store it.
      _channelDataLength[i] = (*(_payload + inc + upperWd) & 0xF) << 8 | *(_payload + inc + lowerWd);
      numberOf64bitWords = (static_cast<u32>(_channelDataLength[i]) - (8 - remainingBytes))/8;
      remainingBytes = (static_cast<u32>(_channelDataLength[i]) - (8 - remainingBytes)) % 8;
      inc += 8;
      } catch (const exception & e) {
      stringstream temp;
      temp << "Fed9UEventUnitStreamLine constructor: Error detected whilst storing data length for channel " << i << " maybe this channel is missing from the data ?";
      RETHROW(e, ICUtils::ICException(temp.str()));
      }
      } // end of loop over channels
    */
  }
  
  u8 * Fed9UEventUnitStreamLine::getPointerToEndOfUnitPayload() {
    // calculate the total length of the unit data
    u16 unitLength = getUnitDataLengthFromPayload();
    // now add the padding
    if ((unitLength % 8) > 0) {unitLength += (8 - (unitLength % 8));}
    //    return (_payload + unitLength);
    return (_payload.getPointer() + unitLength);
  }

  // Frame synch out packet or APV error flags are returned
  // in vector with LSB stored in element[0] of the vector
  vector<u8> Fed9UEventUnitStreamLine::getFeUnitHeader() const {
    vector<u8> feHeader;
    if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
      feHeader.reserve(10);
      //<JEC date=23/02/07> change header pointer to special u8
      /*
	u32 inc = 4;
	for (int i = 0; i < 10; ++i) {
	// move inc along each time we get to the middle or the end of a 64-bit word
	if (inc == 8) {inc -= 8;}
	if ((i!= 0) && (inc == 4)) {inc += 8;}
	feHeader.push_back(*(_header + inc + (8 - _unit)*16));
	inc++;
	}
      */
      int inc = 7;
      for (int i = 0; i < 10; ++i) {
	if (inc < 0) {inc += 16;}
	feHeader.push_back(_header.getu8(inc + (8 - _unit)*16));
	inc--;
      }
      //</JEC>
    } else if (_headerFormat == FED9U_HEADER_APVERROR) {
      feHeader.reserve(3);
      //<JEC date=23/02/07> change header u8* to special u8*
	u32 numberOf8ByteWords = ((8 - _unit)*3)/8;
	u32 remainingBytes = ((8 - _unit)*3) % 8;

	int inc = 7 - static_cast<int>(remainingBytes);
	for (int i = 0; i < 3; ++i) {
	  if (inc < 0) {
	    inc += 8;
	    numberOf8ByteWords++;
	  }
	  feHeader.push_back(_header.getu8(inc + static_cast<int>(numberOf8ByteWords)*8));
	  inc--;
	}

	/*
	// extract the three bytes which correspond to the APV error flag information for 
	// the specified FE unit, bearing in mind that they may be in different 64-bit words.
	int firstByte = 0;
	if (remainingBytes <= 3) {
	firstByte += (remainingBytes + 4);
	} else {
	firstByte += (remainingBytes - 4);
	}
	int secondByte = firstByte + 1;
	int thirdByte = firstByte + 2;
	
	if (firstByte >= 4 && firstByte <= 7) {
	if (secondByte > 7) {secondByte -= 8;}
	if (thirdByte  > 7) {thirdByte -= 8;}
	} else if (firstByte >= 0 && firstByte <= 3) {
	if (secondByte > 3) {secondByte += 8;}
	if (thirdByte > 3) {thirdByte += 8;}
	}
	
	firstByte += (static_cast<int>(numberOf8ByteWords)*8);
	secondByte += (static_cast<int>(numberOf8ByteWords)*8);
	thirdByte += (static_cast<int>(numberOf8ByteWords)*8);
	
	feHeader.push_back(*(_header + firstByte));
	feHeader.push_back(*(_header + secondByte));
	feHeader.push_back(*(_header + thirdByte));
      */
      //</JEC>
    }
    return feHeader;
  }

  u8 Fed9UEventUnitStreamLine::getChannelStatus(u32 channel) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getChannelStatus:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);
    vector<u8> status = getFeUnitHeader();
    if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
      // extract the status information from the vector returned by getFeUnitHeader,
      // bearing in mind that it may be spread over more than one vector element
      u32 startPoint = (channel - 1)*6;
      if ((startPoint/8) == ((startPoint + 5)/8)) {                     // status information is contained within one element
	return (status[startPoint/8] >> (startPoint % 8)) & 0x3F;
      } else {                                                          // status information is spread over two elements
	return (status[startPoint/8] >> (startPoint % 8)) | ((status[(startPoint + 5)/8] << (8 - (startPoint % 8))) & 0x3F);
      }
    } else if (_headerFormat == FED9U_HEADER_APVERROR) {
      // as 8-bits is divisible by 2-bits, the status information in this mode
      // will never be spread over more than one vector element
      u32 startPoint = (channel - 1)*2;
      return (status[startPoint/8] >> (startPoint % 8)) & 0x3;
    }
    return 0;
  }

  u8 Fed9UEventUnitStreamLine::getMajorityPipelineAddress() const {
    if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
      vector<u8> status = getFeUnitHeader();
      return status[9];
    }
    return 0;
  }

  u16 Fed9UEventUnitStreamLine::getUnitDataLengthFromHeader() const {
    u16 dataLength = 0;
    if (_headerFormat == FED9U_HEADER_FULLDEBUG) {
      //<JEC date=23/02/07> change header u8* to special u8*
      u32 location = 8 + (8 - _unit)*16;
      dataLength = (_header.getu8(location) << 8 | _header.getu8(location+1));
      //      dataLength = ((*(_header + 11 + (8 - _unit)*16))) << 8 | (*(_header + 10 + (8 - _unit)*16));
      //</JEC>
    }
    return dataLength;
  }

  // NB.  Assume three MSBs are 111 (will need to be modified if this is changed)
  u8 Fed9UEventUnitStreamLine::getPacketCode(u32 channel) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getPacketCode:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);
    
    //<JEC date=23/02/07> payload pointer changed to special u8 pointer
    /*
      int numberOf64BitWords = 0, lowerWd = 0, upperWd = 0;
      getLocations(channel,2,numberOf64BitWords,lowerWd,upperWd);
      u8 pCode = *(_payload + (numberOf64BitWords*8) + lowerWd);
    */
    int location = getLocations(channel,2);
    u8 pCode = _payload.getu8(location);
    //</JEC>

    //AAB Added more packet codes to next line and changed output format to not strip some bits (returns more information)
    //Changed Fed9UEventStreamline to use the new return value and strip bits there if needed
    //if ((pCode == 0xE5) || (pCode == 0xE6) || (pCode == 0xF2) || (pCode == 0xEA)) {return (pCode & 0x1F);}
    if ((pCode == 0xE5) || (pCode == 0xE6) || (pCode == 0xF2) || (pCode == 0xEA) || (pCode == 0xE1) || (pCode == 0x8A) || (pCode == 0x86) || (pCode == 0xaa) || (pCode == 0xca) || (pCode == 0xA6) || (pCode == 0xC6) || (pCode == 0x92) || (pCode == 0xD2) || (pCode == 0xB2)) {return pCode;}
    else {return 0;}
  }

  u16 Fed9UEventUnitStreamLine::getMedian(u32 channel, u32 apv) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getMedian:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);
    ICUTILS_VERIFY(apv > 0 && apv <=2)(apv).msg("getMedian:  wrong APV number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_APV_OUT_OF_BOUNDS);
    ICUTILS_VERIFY(getPacketCode(channel)==0xA)(channel).msg("getMedian:  no median information available!").error()
      .code(Fed9UEventStreamLineException::ERROR_BAD_FRONT_END_PACKET_CODE);
    
    //    int numberOf64BitWords = 0, lowerWd = 0, upperWd = 0;
    // location of median information is always in the same place with respect to the start of 
    // the data for a given fibre.
    u32 offset = 3;
    // Need an extra offset depending on which APV you want to get the information about
    if ((2 - apv) == 0) {offset += 2;}
    //<JEC date=23/02/07> payload pointer changed to special u8 pointer
    /*
      getLocations(channel,offset,numberOf64BitWords,lowerWd,upperWd);
      return (*(_payload + (numberOf64BitWords*8) + lowerWd)) | ((*(_payload + (numberOf64BitWords*8) + upperWd) & 0x3) << 8);
    */
    int location = getLocations(channel,offset);
    return (_payload.getu8(location) | ((_payload.getu8(location+1) & 0x3) << 8));
    //</JEC>
  }

  u16 Fed9UEventUnitStreamLine::getUnitDataLengthFromPayload() const {
    u16 unitDataLength = 0;
    for (u32 i = 0; i < _channelDataLength.size(); ++i) {
      unitDataLength += _channelDataLength[i];
    }
    return unitDataLength;
  }

  u16 Fed9UEventUnitStreamLine::getSamples(u32 channel) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getSamples:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);
    
    u8 pCode = getPacketCode(channel);
    //(AAB 8/26/2015) changed the names to name_hex to be compatible with the slightly changed return of getPacketCode (returns more information now than previously)
    if ((pCode ==  FED9U_PACKET_SCOPE_hex) || (pCode == FED9U_PACKET_VIRGRAW_hex) || (pCode == FED9U_PACKET_PROCRAW_hex)) {
      // This approach only works for the above packet codes, as for these data formats, the length will be
      // the same for every channel
      return (_channelDataLength[12 - channel] - 3)/2;
    }
    //AAB 8/26/2015 added 10 bit modes
    else if((pCode == FED9U_PACKET_VIRGRAW_10BIT) || (pCode == FED9U_PACKET_PROCRAW_10BIT)) {
      return (_channelDataLength[12 - channel] - 3)*4/5;
    }
    //AAB 8/26/2015 added 8 bit modes
    else if((pCode == FED9U_PACKET_VIRGRAW_8BIT_LO) || (pCode == FED9U_PACKET_PROCRAW_8BIT_LO) || (pCode == FED9U_PACKET_VIRGRAW_8BIT_HI_LO) || (pCode == FED9U_PACKET_PROCRAW_8BIT_HI_LO)) {
      return (_channelDataLength[12 - channel] - 3);
    }
    // In zero suppressed mode, the number of clusters will vary from fibre to fibre, so simply return the maximum
    // number of entries a channel can possibly have
    return STRIPS_PER_APV * APVS_PER_CHANNEL;
  }

  Fed9USu8 & Fed9UEventUnitStreamLine::getSampleSpecialPointer(u32 channel, u32 dataLoc) {
    
    //(AAB 8/26/2015) added & 0x1F to be compatible with the slightly changed return of getPacketCode (returns more information now than previously)
    u8 pCode = (getPacketCode(channel) & 0x1F);
    u32 offset = 0;
    if ((pCode ==  FED9U_PACKET_SCOPE) || (pCode == FED9U_PACKET_VIRGRAW) || (pCode == FED9U_PACKET_PROCRAW)) {
      offset = 3 + dataLoc*2;
    } else if (pCode == FED9U_PACKET_ZEROSUPP) {
      offset = 7 + dataLoc;
    } else {
      offset = 2 + dataLoc;
    }
    
    // make sure that _sample is pointing to the start of the payload
    _sample = _payload;
    int location = getLocations(channel,offset);
    _sample += location;
    return _sample;
  }

  void Fed9UEventUnitStreamLine::getSample(u32 channel, u16 * destBuffer) {
    try {
      //(AAB 8/26/2015) added & 0x1F to be compatible with the slightly changed return of getPacketCode (returns more information now than previously)
      u8 pCode = (getPacketCode(channel) & 0x1F);
      // reset sample special pointer to the start of the payload
      _sample = _payload;

      if ((pCode ==  FED9U_PACKET_SCOPE) || (pCode == FED9U_PACKET_VIRGRAW) || (pCode == FED9U_PACKET_PROCRAW)) {
	// move _sample pointer to the start of the actual data for the relevant channel
	_sample += getLocations(channel,3);
	u32 length = static_cast<u32>(getChannelDataLength(channel)) - 3;
	for (u32 i = 0; i < (length/2); i++) {
	  u8 lower = *_sample++;
	  u8 upper = *_sample++;
	  destBuffer[i] = lower | (upper & 0x3) << 8;
	}
      } else {  // ZS or ZS lite
	u32 offset = 0;
	if (pCode == FED9U_PACKET_ZEROSUPP) {offset = 7;}
	else {offset = 2;}
	memset(static_cast<void*>(destBuffer), 0, STRIPS_PER_APV * APVS_PER_CHANNEL * sizeof (*destBuffer));
	// move _sample pointer to the start of the actual data for the relevant channel
	_sample += getLocations(channel,offset);
	u32 length = static_cast<u32>(getChannelDataLength(channel)) - offset;
	while (length > 0) {
	  u8 strip = *_sample++;
	  length--;
	  u8 width = *_sample++;
	  length--;
	  for (u8 istr = 0; istr < width; istr++) {
	    destBuffer[strip + istr] = *_sample++;
	    length--;
	  }
	}
      }
      
    } catch (exception & exc) {
      RETHROW(exc,Fed9UEventStreamLineException(Fed9UEventStreamLineException::ERROR_GET_SAMPLES_FAILED,"An error ocurred in method getSamples()"));
    } catch (...) {
      THROW(Fed9UEventStreamLineException(Fed9UEventStreamLineException::ERROR_UNKNOWN,"An unknown error occurred in method getSamples()"));
    }
  }


  u16 Fed9UEventUnitStreamLine::getSample(u32 channel, u32 dataLoc) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getSample:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);
    vector<u16> sample = getSample(channel);
    return sample[dataLoc];
  }

  vector<u16> Fed9UEventUnitStreamLine::getSample(u32 channel) const {
    ICUTILS_VERIFY(channel > 0 && channel <=12)(channel).msg("getSample:  wrong channel number given as input").error()
      .code(Fed9UEventStreamLineException::ERROR_CHANNEL_OUT_OF_BOUNDS);

    vector<u16> sample;
    try {
    //(AAB 8/26/2015) added & 0x1F and _hex mode to be compatible with the slightly changed return of getPacketCode (returns more information now than previously)
      u8 pCode_hex = (getPacketCode(channel));
      u8 pCode = (getPacketCode(channel) & 0x1F);
      u32 offset = 0;
      if ((pCode ==  FED9U_PACKET_SCOPE) || (pCode == FED9U_PACKET_VIRGRAW) || (pCode == FED9U_PACKET_PROCRAW)) {offset = 3;}
      else if (pCode == FED9U_PACKET_ZEROSUPP) {offset = 7;}
      else {offset = 2;}
      
      // find the start of the channel payload
      //<JEC date=23/02/07> payload pointer change
      int numberOfUsedBytes = getLocations(channel,offset);
      /*
	u32 numberOfUsedBytes = 0;
	u32 numberOfChannels = 12;
	while(numberOfChannels > channel) {
	numberOfUsedBytes += static_cast<u32>(getChannelDataLength(numberOfChannels));
	numberOfChannels--;
	}
	numberOfUsedBytes += offset;
      */
      //</JEC>

      //AAB using hex versions now (more info) 8/26/2015
      if ((pCode_hex ==  FED9U_PACKET_SCOPE_hex) || (pCode_hex == FED9U_PACKET_VIRGRAW_hex) || (pCode_hex == FED9U_PACKET_PROCRAW_hex)) {
	sample.resize(((static_cast<u32>(_channelDataLength[12 - channel]) - offset)/2),0);
	for (u32 len = 0; len < (static_cast<u32>(_channelDataLength[12 - channel]) - offset)/2; ++len) {
	  //<JEC date=23/02/07> changed payload pointer to special u8 pointer
	  /*
	    u32 numberOf64BitWords = numberOfUsedBytes/8;
	    u32 startPoint = numberOfUsedBytes % 8;
	    int lowerWd = 3 - static_cast<int>(startPoint);
	    int upperWd = lowerWd - 1;
	    if (lowerWd < 0) {lowerWd += 8;}
	    if (upperWd < 0) {upperWd += 8;}
	    if (startPoint == 7) {upperWd += 8;}
	    sample[len] = *(_payload + (numberOf64BitWords << 3) + lowerWd) | (*(_payload + (numberOf64BitWords << 3) + upperWd) << 8);
	  */
	  sample[len] = (_payload.getu8(numberOfUsedBytes) | (_payload.getu8(numberOfUsedBytes+1) << 8));
	  numberOfUsedBytes += 2;
	}
        }
      //AAB 8/26/2015 added 8 bit LO unpacker
      else if ((pCode_hex == FED9U_PACKET_VIRGRAW_8BIT_LO) || (pCode_hex == FED9U_PACKET_PROCRAW_8BIT_LO)) {
	sample.resize(((static_cast<u32>(_channelDataLength[12 - channel]) - offset)),0);
	for (u32 len = 0; len < (static_cast<u32>(_channelDataLength[12 - channel]) - offset); ++len) {
	  
          sample[len] = (_payload.getu8(numberOfUsedBytes) << 2);
	  numberOfUsedBytes += 1;
	}
        }
      //AAB 8/26/2015 added 8 bit HI_LO unpacker
      else if ((pCode_hex == FED9U_PACKET_VIRGRAW_8BIT_HI_LO) || (pCode_hex == FED9U_PACKET_PROCRAW_8BIT_HI_LO)) {
	sample.resize(((static_cast<u32>(_channelDataLength[12 - channel]) - offset)),0);
	for (u32 len = 0; len < (static_cast<u32>(_channelDataLength[12 - channel]) - offset); ++len) {
	  
          sample[len] = (_payload.getu8(numberOfUsedBytes) << 1);
	  numberOfUsedBytes += 1;
	}
        }
      //AAD 8/26/2015 added 10 bit unpacker
      else if ((pCode_hex == FED9U_PACKET_VIRGRAW_10BIT) || (pCode_hex == FED9U_PACKET_PROCRAW_10BIT)) {
	sample.resize(((static_cast<u32>(_channelDataLength[12 - channel]) - offset)*4/5),0);
        
        //bitOverlap keeps track of the number of bits 'bleeding' into the next byte because we are storing  10 bit word in 8bit bytes, has a periodic structure of 2,4,6,8,2.... as we read out the data 
        int bitOverlap = 2;
	for (u32 len = 0; len < (static_cast<u32>(_channelDataLength[12 - channel]) - offset)*4/5; ++len) {
	  
          sample[len] = (_payload.getu8(numberOfUsedBytes));
          sample[len] = sample[len] << bitOverlap;
	  numberOfUsedBytes++;
          sample[len] = sample[len] | (_payload.getu8(numberOfUsedBytes) >> (8-bitOverlap));
          sample[len] = sample[len] & 1023;
          if(bitOverlap == 8) numberOfUsedBytes++;
          bitOverlap = bitOverlap%8+2; 
	}
      } else {
	sample.resize(STRIPS_PER_APV * APVS_PER_CHANNEL,0);     
	u32 count = 0;
	//	int lowerWd = 0;
	while (count < (static_cast<u32>(_channelDataLength[12 - channel]) - offset)) {
	  //<JEC date=23/02/07> payload change
	  /*
	    lowerWd = 3 - static_cast<int>(numberOfUsedBytes % 8);
	    if (lowerWd < 0) {lowerWd += 8;}
	    u8 addr = *(_payload + ((numberOfUsedBytes/8) << 3) + lowerWd);
	    ++numberOfUsedBytes;
	    ++count;
	    lowerWd = 3 - static_cast<int>(numberOfUsedBytes % 8);
	    if (lowerWd < 0) {lowerWd += 8;}
	    u8 len = *(_payload + ((numberOfUsedBytes/8) << 3) + lowerWd);
	    ++numberOfUsedBytes;
	    ++count;
	  */
	  u8 addr = _payload.getu8(numberOfUsedBytes);
	  u8 len  = _payload.getu8(numberOfUsedBytes+1);
	  numberOfUsedBytes += 2;
	  count += 2;
	  //</JEC>
	  
	  for (u32 j = 0; j < static_cast<u32>(len); ++j) {
	    //<JEC date=23/02/07> payload change
	    /*
	      lowerWd = 3 - static_cast<int>(numberOfUsedBytes % 8);
	      if (lowerWd < 0) {lowerWd += 8;}
	      sample[static_cast<u32>(addr)+j] = *(_payload + ((numberOfUsedBytes/8) << 3) + lowerWd);
	    */
	    sample[static_cast<u32>(addr)+j] = _payload.getu8(numberOfUsedBytes);
	    //</JEC>
	    ++numberOfUsedBytes;
	    ++count;
	  }
	}
      }
    } catch (exception & exc) {
      RETHROW(exc,Fed9UEventStreamLineException(Fed9UEventStreamLineException::ERROR_GET_SAMPLES_FAILED,"Fed9UEventUnitStreamLine::getSamples:  data is corrupted ?"));
    } catch (...) {
      THROW(Fed9UEventStreamLineException(Fed9UEventStreamLineException::ERROR_UNKNOWN,"An unknown error occurred in method getSamples()"));
    }
    return sample;
  }

  bool Fed9UEventUnitStreamLine::checkChannelLengths() {
    u32 channel = 12;
    u8 pCode = getPacketCode(channel);
    bool matchLength = true;
    //AAB 8/26/2015 added & 0x1F to be compatible w/ new getPacketCode output
    if ((pCode & 0x1F) == FED9U_PACKET_SCOPE) {
      for (u32 c1 = 0; c1 < _channelDataLength.size(); ++c1) {
	for (u32 c2 = 0; c2 < _channelDataLength.size(); ++c2) {
	  if (c1 != c2) {
	    if (_channelDataLength[c1] != _channelDataLength[c2]) {
	      matchLength = false;
	      cout << "Mismatch in length for channels " << c1 << " and " << c2 << endl;
	    }
	  }
	}
      }
    //AAB 8/26/2015 added _hex to be compatible w/ the getPacketOutput() return information change
    } else if ((pCode == FED9U_PACKET_VIRGRAW_hex)  || (pCode == FED9U_PACKET_PROCRAW_hex)) {
      for (u32 c = 0; c < _channelDataLength.size(); ++c) {
	if (_channelDataLength[c] != 515) {
	  matchLength = false;
	  cout << "Mismatch in length for channel " << c << endl;
	}
      }
    //AAB 8/26/2015 added below 10 bit and 8 bit length checking versions
    } else if ((pCode == FED9U_PACKET_VIRGRAW_10BIT)  || (pCode == FED9U_PACKET_PROCRAW_10BIT)) {
      for (u32 c = 0; c < _channelDataLength.size(); ++c) {
	if (_channelDataLength[c] != 323) {
	  matchLength = false;
	  cout << "Mismatch in length for channel " << c << endl;
	}
      }
    } else if ((pCode == FED9U_PACKET_VIRGRAW_8BIT_HI_LO)  || (pCode == FED9U_PACKET_PROCRAW_8BIT_HI_LO) || (pCode == FED9U_PACKET_VIRGRAW_8BIT_LO)  || (pCode == FED9U_PACKET_PROCRAW_8BIT_LO)) {
      for (u32 c = 0; c < _channelDataLength.size(); ++c) {
	if (_channelDataLength[c] != 255) {
	  matchLength = false;
	  cout << "Mismatch in length for channel " << c << endl;
	}
      }
    } else {
      cout << "We are in ZERO SUPPRESSED (LITE) mode - channel lengths will not necessarily be the same" << endl;
    }
    return matchLength;
  }

  // PRIVATE methods
  
  // returns the locations of two consecutive bytes, based on the input FE unit and channel numbers, as well as an offset
  // according to how far from the start of the data you are.  It takes into account the fact that the data is packed
  // assuming that bit 0 is on the right and bit 63 is on the left, while the pointer starts at bit 32, moves up to bit 63,
  // then down to bit 0, ending at bit 31.  The method converts between the two different counting schemes, returning the locations 
  // in the centre-based counting scheme of consecutive bytes in the right-left counting scheme.
  void Fed9UEventUnitStreamLine::getLocations(u32 channel, u32 offset, int &nOf64BWords, int &lowerWd, int &upperWd) const {
    u16 numberOfUsedBytes = 0;
    u32 numberOfChannels = 12;
    while (numberOfChannels > channel) {
      numberOfUsedBytes += getChannelDataLength(numberOfChannels);
      numberOfChannels--;
    }
    nOf64BWords = static_cast<int>(numberOfUsedBytes)/8;
    u32 remainingBytes = (static_cast<u32>(numberOfUsedBytes) % 8) + offset;
    while (remainingBytes > 7) {
      nOf64BWords++;
      remainingBytes -= 8;
    }
    
    lowerWd = 3 - static_cast<int>(remainingBytes);
    if (lowerWd < 0) {lowerWd += 8;}
    upperWd = 3 - static_cast<int>(remainingBytes) - 1;
    if (upperWd < 0) {upperWd += 8;}
    // In this case upperWd ends up on the next line, so need to account for it !
    if (remainingBytes == 7) {upperWd += 8;}
  }

  // overloaded version of getLocations for use with special u8 pointer
  int Fed9UEventUnitStreamLine::getLocations(u32 channel, u32 offset) const {
    u16 numberOfUsedBytes = 0;
    u32 numberOfChannels = 12;
    while (numberOfChannels > channel) {
      numberOfUsedBytes += getChannelDataLength(numberOfChannels);
      numberOfChannels--;
    }
    return (static_cast<u32>(numberOfUsedBytes) + offset);
  }
  
}

