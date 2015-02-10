#include "Fed9UBufferDescription.hh"

namespace Fed9U {


  Fed9UBufferDescription::Fed9UBufferDescription(void) :
    trackerEventType_(0)
  {
    //Initialize default buffer parameters.
    mIndex = 0;
    mBunchCrossing = 0;
    mPacketCode = 0;
    mHeaderFormat = mFed9UEventInfo.getTrackerSpecialHeaderFullDebugMode(); 
    mMode = 0;
    mFed9UBufferData.clear();
    mFeLengths.clear();
    mChannelLengths.clear();
    mCMMedians.clear();
    initBuffer();
  }
  

  void Fed9UBufferDescription::initBuffer(void) {
    for (unsigned int i = 0; i<INTS_IN_RAW_FED9U_BUFFER; ++i) {
      mBuf[i] = 0;
    }
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setBunchCrossing(unsigned long bunchCrossing) {
    mBunchCrossing = bunchCrossing;
    return *this;
  }
  

  Fed9UBufferDescription & Fed9UBufferDescription::setMode(unsigned long mode) {
    mMode = mode;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setHeaderFormat(unsigned short headerFormat) {
    mHeaderFormat = headerFormat;
    return *this;
  }


  Fed9UBufferDescription& Fed9UBufferDescription::trackerEventType( unsigned short value ) {
    trackerEventType_ = value;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setFed9UBufferData(const std::vector<unsigned short> &bufferData) throw (ICException) {
   
//     ICUTILS_VERIFY( bufferData.size() == static_cast<unsigned int>( mFed9UEventInfo.getStripsPerApv() * 
// 							    mFed9UEventInfo.getApvsPerFedChannel() * 
// 							    mFed9UEventInfo.getChannelsPerFeUnit() * 
// 							    mFed9UEventInfo.getFeUnitsPerFed()) )( bufferData.size() ).error();
    mFed9UBufferData = bufferData;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setChannelLengths(const std::vector<unsigned short> &channelLengths) throw (ICException) {
    ICUTILS_VERIFY(channelLengths.size()==static_cast<unsigned int>(mFed9UEventInfo.getChannelsPerFeUnit() * mFed9UEventInfo.getFeUnitsPerFed()))(channelLengths.size()).error();
    mChannelLengths = channelLengths;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setFeLengths(const std::vector<unsigned short> &feLengths) throw (ICException) {
    ICUTILS_VERIFY(feLengths.size()==mFed9UEventInfo.getFeUnitsPerFed())(feLengths.size()).error();
    mFeLengths = feLengths;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setPacketCode(unsigned short packetCode) {
    mPacketCode = packetCode;
    return *this;
  }


  Fed9UBufferDescription & Fed9UBufferDescription::setCommonModeMedians(const std::vector<unsigned short> &cmMedians) throw (ICException) {
    ICUTILS_VERIFY(cmMedians.size()==static_cast<unsigned int>(mFed9UEventInfo.getChannelsPerFeUnit() * mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getApvsPerFedChannel()))(cmMedians.size()).error();
    mCMMedians = cmMedians;
    return *this;
  }

  Fed9UBufferDescription & Fed9UBufferDescription::setSlink64() {

    for (unsigned int i = 0; i < (INTS_IN_RAW_FED9U_BUFFER-1); i+=2) {
      unsigned int temp = mBuf[i];
      mBuf[i]=mBuf[i+1];
      mBuf[i+1]=temp;
    }
    return *this;
  }

  unsigned long Fed9UBufferDescription::getBunchCrossing(void) const {
    return mBunchCrossing;
  }


  unsigned long Fed9UBufferDescription::getMode(void) const {
    return mMode;
  }


  unsigned short Fed9UBufferDescription::getHeaderFormat(void) const {
    return mHeaderFormat;
  }
  

  unsigned short Fed9UBufferDescription::trackerEventType(void) const {
    return trackerEventType_;
  }
  

  std::vector<unsigned short> Fed9UBufferDescription::getFed9UBufferData(void) const {
    return mFed9UBufferData;
  }


  std::vector<unsigned short> Fed9UBufferDescription::getChannelLengths(void) const {
    return mChannelLengths;
  }


  std::vector<unsigned short> Fed9UBufferDescription::getFeLengths(void) const {
    return mFeLengths;
  }


  unsigned short Fed9UBufferDescription::getPacketCode(void) const {
    return mPacketCode;
  }


  std::vector<unsigned short> Fed9UBufferDescription::getCommonModeMedians(void) const {
    return mCMMedians;
  }

}
