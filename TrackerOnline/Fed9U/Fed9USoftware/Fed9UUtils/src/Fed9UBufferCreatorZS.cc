#include "Fed9UBufferCreatorZS.hh"
#include "Fed9UBufferDescription.hh"

namespace Fed9U {

  //using std::exception;
    

  void Fed9UBufferCreatorZS::generateBuffer(Fed9UBufferDescription *bufDesc) noexcept(false) {
    try {

      //Init the buffer index
      bufDesc->initBufferIndex();

      //Get the channel lengths and FE lengths vectors. These are calculated 
      //using the bufferData vector inside the Fed9UBufferDescription.
      std::vector<unsigned short> feLengths;
      std::vector<unsigned short> channelLengths;
      findDataLengths(bufDesc->getFed9UBufferData(), feLengths, channelLengths);
      bufDesc->setFeLengths(feLengths);
      bufDesc->setChannelLengths(channelLengths);

      //Construct the packet code.
      unsigned short packetCode = 0xE0 | mFed9UEventInfo.getZeroSuppressedModeCode() | mFed9UEventInfo.getFrameFindingModeCode();
      bufDesc->setPacketCode(packetCode);
      bufDesc->setMode( mFed9UEventInfo.getZeroSuppressedModeCode() );
      bufDesc->trackerEventType( mFed9UEventInfo.getZeroSuppressedDaqModeCode() ); // added by R.B.

      //Generate the DAQ header
      mBufHeader.generateFed9UDaqHeader(bufDesc);
      //Generate the tracker special header
      mBufHeader.generateFed9UTrackerSpecialDaqHeader(bufDesc);
      //Generate FE headers
      mBufHeader.generateFed9UFEHeader(bufDesc);
      //Generate channel data.
      mBufData.generateFed9UZSData(bufDesc);
      //Now add the DAQ trailer
      mBufTrailer.generateFed9UDaqTrailer(bufDesc);
      
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorZS::generateBuffer."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorZS::generateBuffer."));
    }
  }


  void Fed9UBufferCreatorZS::findDataLengths(const std::vector<unsigned short> &bufferData, 
					std::vector<unsigned short> &feLengths, 
					std::vector<unsigned short> &channelLengths) noexcept(false) {
    try {
      feLengths.clear();
      channelLengths.clear();
      unsigned short stripsPerApv = mFed9UEventInfo.getStripsPerApv();
      unsigned short stripsPerChannel = stripsPerApv * mFed9UEventInfo.getApvsPerFedChannel();
      unsigned short channelsPerFeUnit = mFed9UEventInfo.getChannelsPerFeUnit();
      unsigned short stripsPerFe = stripsPerChannel * channelsPerFeUnit;
      unsigned short feUnitsPerFed = mFed9UEventInfo.getFeUnitsPerFed();
      ICUTILS_VERIFY(bufferData.size()==static_cast<unsigned int>(stripsPerFe*feUnitsPerFed))(bufferData.size()).error();
      
      bool foundCluster = false;
      //Loop over front ends
      for (int fe=0; fe<feUnitsPerFed; ++fe) {
	feLengths.push_back(0);
	//Loop over channels
	for (int chan=0; chan<channelsPerFeUnit; ++chan) {
	  channelLengths.push_back(0);
	  //Loop over strips
	  for (int strip=0; strip<stripsPerChannel; ++strip) {
	    //Count the cluster length.
	    if (bufferData[(fe*stripsPerFe)+(chan*stripsPerChannel)+strip]!=0) {
	      foundCluster = true;
	      channelLengths[(fe*channelsPerFeUnit)+chan]+=1;
	    }
	    //Add the cluster length to the total length for this channel. 
	    //Also test if we are at the end of a channel and if there is a hit occupying the last strip.
	    //Also test for a cluster on the boundary of an APV. If there is then force the cluster
	    //to end on the boundary. This is a consequence of FED board operations, 
	    //since the maximum cluster size in practice is 128 strips.
	    if (((foundCluster)&&(bufferData[(fe*stripsPerFe)+(chan*stripsPerChannel)+strip]==0)) || 
		((foundCluster)&&((strip+1)==stripsPerChannel)) ||
		((foundCluster)&&(strip==(stripsPerApv-1)))) {
	      //Add the two bytes required for cluster address and length
	      channelLengths[(fe*channelsPerFeUnit)+chan]+=2;
	      foundCluster = false;
	    }
	  }
	  //Add on 7 bytes for the cluster length, packet code and CMN median values.
	  channelLengths[(fe*channelsPerFeUnit)+chan]+=7;
	  //Add the total channel length to the FE length
	  feLengths[fe]+=channelLengths[(fe*channelsPerFeUnit)+chan];
	}
    }
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorZS::findDataLengths."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorZS::findDataLengths."));
    }
  }
  
}
