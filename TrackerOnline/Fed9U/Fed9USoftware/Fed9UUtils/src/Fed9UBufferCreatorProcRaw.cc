#include "Fed9UBufferCreatorProcRaw.hh"
#include "Fed9UBufferDescription.hh"

namespace Fed9U {

  using std::exception;

  void Fed9UBufferCreatorProcRaw::generateBuffer(Fed9UBufferDescription *bufDesc) throw (ICException) {
    try {      
            
      //Make vectors of the data sizes for each FE and each channel.
      unsigned short channelsPerFeUnit = mFed9UEventInfo.getChannelsPerFeUnit();
      unsigned short feUnitsPerFed = mFed9UEventInfo.getFeUnitsPerFed();
      std::vector<unsigned short> feLengths(feUnitsPerFed, mFed9UEventInfo.getBytesInRawFed9UFE());
      std::vector<unsigned short> channelLengths(feUnitsPerFed*channelsPerFeUnit, mFed9UEventInfo.getBytesInRawFed9UChannel());
      //Put these vectors into the Fed9UBufferDescription
      bufDesc->setFeLengths(feLengths);
      bufDesc->setChannelLengths(channelLengths);

      //Construct the packet code.
      unsigned short packetCode = 0xE0 | mFed9UEventInfo.getProcessedRawModeCode() | mFed9UEventInfo.getFrameFindingModeCode();
      bufDesc->setPacketCode(packetCode);
      bufDesc->setMode( mFed9UEventInfo.getProcessedRawDaqModeCode() );
      bufDesc->trackerEventType( mFed9UEventInfo.getProcessedRawDaqModeCode() ); // added by R.B.

      //Generate the DAQ header
      mBufHeader.generateFed9UDaqHeader(bufDesc);
      //Generate the tracker special header
      mBufHeader.generateFed9UTrackerSpecialDaqHeader(bufDesc);
      //Generate FE headers
      mBufHeader.generateFed9UFEHeader(bufDesc);
      //Generate channel data.
      mBufData.generateFed9URawData(bufDesc);
      //Now add the DAQ trailer
      mBufTrailer.generateFed9UDaqTrailer(bufDesc);
    }
    catch (exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorProcRaw::generateBuffer."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorProcRaw::generateBuffer.."));
    }
  }
  
}
