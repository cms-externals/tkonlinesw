#include "Fed9UBufferCreatorScopeMode.hh"
#include "Fed9UBufferDescription.hh"

namespace Fed9U {

  using std::exception;

  
  Fed9UBufferCreatorScopeMode::Fed9UBufferCreatorScopeMode( unsigned short length ) :
    length_(length)
  {
    // nothing here
  }
  

  void Fed9UBufferCreatorScopeMode::generateBuffer( Fed9UBufferDescription *bufDesc ) noexcept(false) {
  
    try {      
      
      //Make vectors of the data sizes for each FE and each channel.
      unsigned short channelsPerFeUnit = mFed9UEventInfo.getChannelsPerFeUnit();
      unsigned short feUnitsPerFed = mFed9UEventInfo.getFeUnitsPerFed();

      std::vector<unsigned short> feLengths( feUnitsPerFed, 12*(3+(2*length_)) );
      std::vector<unsigned short> channelLengths( feUnitsPerFed*channelsPerFeUnit, 3+(2*length_) );
      
      //Put these vectors into the Fed9UBufferDescription
      bufDesc->setFeLengths(feLengths);
      bufDesc->setChannelLengths(channelLengths);

      //Construct the packet code.
      unsigned short packetCode = 0xE0 | mFed9UEventInfo.getScopeModeCode();
      bufDesc->setPacketCode(packetCode);
      bufDesc->setMode( mFed9UEventInfo.getScopeModeCode() );
      bufDesc->trackerEventType( mFed9UEventInfo.getScopeDaqModeCode() ); // added by R.B.

      //Generate the DAQ header
      mBufHeader.generateFed9UDaqHeader(bufDesc);
      //Generate the tracker special header
      mBufHeader.generateFed9UTrackerSpecialDaqHeader(bufDesc);
      //Generate FE headers
      mBufHeader.generateFed9UFEHeader(bufDesc);
      //Generate channel data.
      mBufData.generateFed9UScopeModeData( bufDesc, length_ );
      //Now add the DAQ trailer
      mBufTrailer.generateFed9UDaqTrailer(bufDesc);
    }
    catch (exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorScopeMode::generateBuffer."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorScopeMode::generateBuffer.."));
    }
  }
  
}
