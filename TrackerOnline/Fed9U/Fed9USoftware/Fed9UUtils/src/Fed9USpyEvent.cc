#include "Fed9USpyEvent.hh"
#include <iomanip>

namespace Fed9U {
  
  Fed9USpyEvent::Fed9USpyEvent(u16 thresholdValue)
    : buffer(new u32[spyDelayChipBufferSize*CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP]),
      manageBuffer(true) { setAllThresholds(thresholdValue); }
  Fed9USpyEvent::Fed9USpyEvent(const Fed9UDescription& theFedDescription) 
    : buffer(new u32[spyDelayChipBufferSize*CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP]),
      manageBuffer(true) { setThresholds(theFedDescription); }
  Fed9USpyEvent::Fed9USpyEvent(u32* bufferPointer, const Fed9UDescription& theFedDescription)
    : buffer(bufferPointer),
      manageBuffer(false) { setThresholds(theFedDescription); }
  Fed9USpyEvent::Fed9USpyEvent(std::istream* theStream, const Fed9UDescription& theFedDescription)
    : buffer(NULL),
      manageBuffer(true)
  {
    readFromStream(theStream);
    setThresholds(theFedDescription);
  }
  Fed9USpyEvent::Fed9USpyEvent(const char*const filename, const Fed9UDescription& theFedDescription)
    : buffer(NULL),
      manageBuffer(true)
  {
    std::ifstream* theFile = new std::ifstream(filename);
    readFromStream(theFile);
    delete theFile;
    setThresholds(theFedDescription);
  }
  Fed9USpyEvent::Fed9USpyEvent(std::istream* theStream, u16 thresholdValue)
    : buffer(NULL),
      manageBuffer(true)
  {
    readFromStream(theStream);
    setAllThresholds(thresholdValue);
  }
  Fed9USpyEvent::Fed9USpyEvent(const char*const filename, u16 thresholdValue)
    : buffer(NULL),
      manageBuffer(true)
  {
    std::ifstream* theFile = new std::ifstream(filename);
    readFromStream(theFile);
    delete theFile;
    setAllThresholds(thresholdValue);
  }
  void Fed9USpyEvent::readFromStream(std::istream* theStream) {
    //create buffer if it is managed
    if (manageBuffer) {
      if (buffer) delete[] buffer;
      buffer = new u32[spyDelayChipBufferSize*CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP];
    }
    //loop over delay chip buffers reading them into the buffer
    for (u8 i=0; i<CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP; i++) {
      u32* theDelayBuffer = delayChipBuffer(i);
      for (u16 j=0; j<spyDelayChipBufferSize; j++) {
        (*theStream) >> theDelayBuffer[j];
      }
    }
  }
  Fed9USpyEvent::~Fed9USpyEvent() {
    if (manageBuffer) delete[] buffer;
  }
  u32 Fed9USpyEvent::getDumpBufferSize() const {
    u16 sizeIn32BitWords = 0;
    //length
    sizeIn32BitWords++;
    //delay chip buffers (each is spyDelayChipBufferSize 32 bit words)
    sizeIn32BitWords += CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP*spyDelayChipBufferSize;
    return sizeIn32BitWords*4;
  }
  void Fed9USpyEvent::writeToBuffer(u32* writeBuffer) const {
    //length info
    (*writeBuffer++) = getDumpBufferSize();
    //data
    for (u16 i=0; i<CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP*spyDelayChipBufferSize; i++) {
      (*writeBuffer++) = buffer[i];
    }
  }
  void Fed9USpyEvent::setThresholds(const Fed9UDescription& theFedDescription) {
    //clear the saved thresholds
    channelThresholds.clear();
    channelThresholds.reserve(CHANNELS_PER_FED);
    //loop over channels
    Fed9UAddress addr;
    for (int c=0; c<CHANNELS_PER_FED; c++) {
      addr.setFedChannel(c);
      channelThresholds.push_back( theFedDescription.getFrameThreshold(addr) );
    }
  }
  void Fed9USpyEvent::setAllThresholds(u16 thresholdValue) {
    //clear the saved thresholds
    channelThresholds.clear();
    channelThresholds.reserve(CHANNELS_PER_FED);
    //loop over channels
    for (int c=0; c<CHANNELS_PER_FED; c++) {
      channelThresholds.push_back(thresholdValue);
    }
  }
  u32* Fed9USpyEvent::delayChipBuffer(Fed9UAddress address) const {
    //return a pointer to the begining of the new delay chip buffer
    return delayChipBuffer(address.getFedDelayChip());
  }
  u32* Fed9USpyEvent::delayChipBuffer(u8 delayChipNumber) const {
    return buffer+spyDelayChipBufferSize*delayChipNumber;
  }
  u16 Fed9USpyEvent::getSample(u8 delayChip, u8 channel, u16 sample) const {
    const u32* delayChipData = buffer + (delayChip*spyDelayChipBufferSize);
    //range check
    ICUTILS_VERIFY(sample < samplesPerChannel())(sample).msg("Sample number out of range.").error().code(Fed9USpyEventException::ERROR_INDEX_OUT_OF_RANGE);
    //find start of word
    const u16 startBitNumber = (sample*4+channel)*10+offsetOfSpyData;
    const u16 startU32Number = static_cast<int>(static_cast<float>(startBitNumber)/32.0);
    const u8 startOffset = startBitNumber%32;
    //if word is wholly in a single u32 then shift the 32bit word so the lowest order bit is the first bit of the sample and & with 10bit mask
    if (startOffset < 23) {
      return ( (delayChipData[startU32Number]>>(32-10-startOffset)) & 0x3FF );
    }
    //if not then shift the first 32 bit word so that correct bits are selected and & with mask
    //shift mask so that correct bits are selected from second 32bit word and then shift result so that they match the bit possition in the 10 bit sample
    //| the two results together to get 10 bit word
    else {
      return ( ((delayChipData[startU32Number]<<(10-32+startOffset))&0x3FF) | ((delayChipData[startU32Number+1]&(0xFFC00000<<(32-startOffset)))>>(64-10-startOffset)) );
    }
  }
  void Fed9USpyEvent::setSample(u8 delayChip, u8 channel, u16 sampleNumber, u16 value) {
    //only use lowest order 10 bits
    value = value & 0x03FF;
    //find data to modify
    u32* delayChipData = buffer + (delayChip*spyDelayChipBufferSize);
    //range check
    ICUTILS_VERIFY(sampleNumber < samplesPerChannel())(sampleNumber).msg("Sample number out of range.").error().code(Fed9USpyEventException::ERROR_INDEX_OUT_OF_RANGE);
    //find start of word
    const u16 startBitNumber = (sampleNumber*4+channel)*10+offsetOfSpyData;
    const u16 startU32Number = static_cast<int>(static_cast<float>(startBitNumber)/32.0);
    const u8 startOffset = startBitNumber%32;
    //modify value in buffer
    if (startOffset < 23) {
      uint16_t mask = (~0x03FF) << startOffset;
      u32 shiftedValue = value << startOffset;
      delayChipData[startU32Number] = (delayChipData[startU32Number] & mask) | shiftedValue;
    }
    else {
      uint16_t mask1 = (~0x03FF) << startOffset;
      u32 shiftedValue1 = value << startOffset;
      delayChipData[startU32Number] = (delayChipData[startU32Number] & mask1) | shiftedValue1;
      uint16_t mask2 = (~0x03FF) >> (32-startOffset);
      u32 shiftedValue2 = value >> (32-startOffset);
      delayChipData[startU32Number+1] = (delayChipData[startU32Number+1] & mask2) | shiftedValue2;
    }
  }
  u16 Fed9USpyEvent::getSample(Fed9UAddress channelAddress, u16 sample) const {
    const u8 delayChip = channelAddress.getFedDelayChip();
    const u8 channelOnDelayChip = channelAddress.getFeUnitChannel()%4;
    return getSample(delayChip,channelOnDelayChip,sample);
  }
  u16 Fed9USpyEvent::getPayloadSample(Fed9UAddress channelAddress, u16 sample) const {
    //add on length of APV header and samples before start of frame
    u16 sampleInChannelData = sample+getChannelTickOffset(channelAddress)+12;
    //check that sample is less than the number of samples in channel data excluding the data before the tick mark and the APV header
    ICUTILS_VERIFY((samplesPerChannel()-sampleInChannelData) > 0)(sample).msg("Sample number out of range.").error().code(Fed9USpyEventException::ERROR_INDEX_OUT_OF_RANGE);
    return getSample(channelAddress,sampleInChannelData);
  }
  u8 Fed9USpyEvent::getPipelineAddress() const {
    u8 theAddress = 0;
    //loop over delay chips
    for (int i=0; i<CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP; i++) {
      //loop over channels
      for (int j=0; j<4; j++) {
        //u8 to hold address we are calculating
        u8 result = getAPVPipelineAddress(i,j,0);
        //if this is the first channel then store
        if (i==0 && j==0) { theAddress=result; }
        //if not then check we got the same address
        else { ICUTILS_VERIFY(result==theAddress)(result).msg("Address for APV does not match previous address.").error().code(Fed9USpyEventException::ERROR_PIPELINE_ADDRESS_MISMATCH); }
        //now do other APV
        result = getAPVPipelineAddress(i,j,1);
        //check we got the same address
        ICUTILS_VERIFY(result==theAddress)(result).msg("Address for APV does not match previous address.").error().code(Fed9USpyEventException::ERROR_PIPELINE_ADDRESS_MISMATCH);
      }
    }
    return theAddress;
  }
  u8 Fed9USpyEvent::getAPVPipelineAddress(Fed9UAddress apvAddress) const {
    const u8 delayChip = apvAddress.getFedDelayChip();
    const u8 channel = apvAddress.getFeUnitChannel()%4;
    const u8 apv = apvAddress.getChannelApv();
    return getAPVPipelineAddress(delayChip,channel,apv);
  }
  u8 Fed9USpyEvent::getAPVPipelineAddress(u8 delayChip, u8 channel, u8 apv) const {
    //get the threshold to use
    u16 bitHighThreshold = channelThresholds[ delayChip*4 + channel ];
    u8 result = 0x0;
    //find the tick mark
    u8 tickOffset = getChannelTickOffset(delayChip,channel);
    //skip tick mark
    tickOffset+=6;
    //loop over bits in header
    for (int k=0; k<8; k++) {
      bool high=false;	//bool to store if this bit is high
      high = getSample(delayChip,channel,2*k+tickOffset+apv)>bitHighThreshold;
      if (high) result |= (0x80>>k);
    }
    return result;
  }
  u8 Fed9USpyEvent::getChannelTickOffset(Fed9UAddress channelAddress) const {
    const u8 delayChip = channelAddress.getFedDelayChip();
    const u8 channelOnDelayChip = channelAddress.getFeUnitChannel()%4;
    return getChannelTickOffset(delayChip,channelOnDelayChip);
  }
  u8 Fed9USpyEvent::getChannelTickOffset(u8 delayChip, u8 channel) const {
    //get the threshold to use
    u16 bitHighThreshold = channelThresholds[ delayChip*4 + channel ];
    u8 tickOffset=0;
    //skip low bits at the begining
    while ( getSample(delayChip,channel,tickOffset)<bitHighThreshold && tickOffset<100 ) tickOffset++;
    //if tick mark wasn't in the first 100 samples then throw
    ICUTILS_VERIFY(tickOffset < 100)(delayChip*4+channel).msg("Tickmark not found in range.").error().code(Fed9USpyEventException::ERROR_NO_TICK_MARK);
    ICUTILS_VERIFY((getSample(delayChip,channel,tickOffset)>bitHighThreshold) && (getSample(delayChip,channel,tickOffset+1)>bitHighThreshold) && (getSample(delayChip,channel,tickOffset+2)>bitHighThreshold)).msg("Tickmark too short").error().code(Fed9USpyEventException::ERROR_NO_TICK_MARK);
    return tickOffset;
  }
  bool Fed9USpyEvent::tickFound(Fed9UAddress channelAddress) const {
    const u8 delayChip = channelAddress.getFedDelayChip();
    const u8 channelOnDelayChip = channelAddress.getFeUnitChannel()%4;
    return tickFound(delayChip,channelOnDelayChip);
  }
  bool Fed9USpyEvent::tickFound(u8 delayChip, u8 channel) const {
    //get the threshold to use
    u16 bitHighThreshold = channelThresholds[ delayChip*4 + channel ];
    u8 tickOffset=0;
    //skip low bits at the begining
    while ( getSample(delayChip,channel,tickOffset)<bitHighThreshold && tickOffset<100 ) tickOffset++;
    bool inRange = tickOffset<100;
    bool complete = (getSample(delayChip,channel,tickOffset)>bitHighThreshold) && 
                    (getSample(delayChip,channel,tickOffset+1)>bitHighThreshold) && 
                    (getSample(delayChip,channel,tickOffset+2)>bitHighThreshold);
    return inRange && complete;
  }
  void Fed9USpyEvent::writeToStream(std::ostream* theStream) const {
    //loop over delay chips and write them out
    for (u8 i=0; i<CHANNELS_PER_FED/CHANNELS_PER_DELAY_CHIP; i++) {
      u32* delayChipData = delayChipBuffer(i);
      for (u16 j=0; j<spyDelayChipBufferSize; j++) {
        (*theStream) << delayChipData[j] << std::endl;
      }
    }
  }
  void Fed9USpyEvent::writeToFile(const char*const filename) const {
    std::ofstream* theFile = new std::ofstream(filename,std::ios::trunc);
    writeToStream(theFile);
    delete theFile;
  }
  void Fed9USpyEvent::print(std::ostream& os) const {
    //loop over FE units
    for (u8 f=0; f<FEUNITS_PER_FED; f++) {
      Fed9UAddress addr;
      addr.setFedFeUnit(f);
      os << "FeUnit: " << (u16)addr.getExternalFedFeUnit() << std::endl;
      //loop over channels in FeUnit
      for (u8 c=0; c<CHANNELS_PER_FEUNIT; c++) {
        addr.setFeUnitChannel(c);
        //find the tick mark
        u8 tickOffset = 0;
        bool tickMarkFound = false;
        try {
          tickOffset = getChannelTickOffset(addr);
          tickMarkFound = true;
        }
        catch (...) {
          tickMarkFound = false;
        }
        if (tickMarkFound) os << "Channel: " << (u16)addr.getExternalFeUnitChannel() << '\t' << "Tick mark found " << (u16)tickOffset << " words into channel data" << std::endl;
        else os << "Channel: " << (u16)addr.getExternalFeUnitChannel() << '\t' << "Tick mark not found " << std::endl;
        //loop over apvs
        for (u8 a=0; a<APVS_PER_CHANNEL; a++) {
          addr.setChannelApv(a);
          os << '\t' << "APV: " << (u16)a;
          if (tickMarkFound) os << " Pipeline address: " << (u16)getAPVPipelineAddress(addr) << " Error: " << (!getAPVErrorBit(addr)?"true":"false");
          os << std::endl;
          os << '\t';
          //loop over first 25 samples in each apv
          for (int s=0; s<25; s++) {
            os << std::setw(4) << getSample(addr,2*s+a) << ' ';
          }
          os << std::endl;
        }
      }
    }
  }
  bool Fed9USpyEvent::getAPVErrorBit(Fed9UAddress apvAddress) const {
    const u8 delayChip = apvAddress.getFedDelayChip();
    const u8 channel = apvAddress.getFeUnitChannel()%4;
    const u8 apv = apvAddress.getChannelApv();
    return getAPVErrorBit(delayChip,channel,apv);
  }
  bool Fed9USpyEvent::getAPVErrorBit(u8 delayChip, u8 channel, u8 apv) const {
    //get the threshold to use
    u16 bitHighThreshold = channelThresholds[ delayChip*4 + channel ];
    //find the error bit
    u8 errorBitOffset = getChannelTickOffset(delayChip,channel)+22;
    return getSample(delayChip,channel,errorBitOffset+apv)>bitHighThreshold;
  }

}

