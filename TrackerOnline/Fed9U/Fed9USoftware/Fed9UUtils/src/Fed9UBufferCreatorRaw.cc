#include "Fed9UBufferCreatorRaw.hh"
#include "Fed9UBufferDescription.hh"
#include "Fed9USiStripReordering.hh"

namespace Fed9U {

  //using std::exception;

  void Fed9UBufferCreatorRaw::generateBuffer(Fed9UBufferDescription *bufDesc) noexcept(false) {
    try {

      //Make vectors of the data sizes for each FE and each channel.
      unsigned short channelsPerFeUnit = mFed9UEventInfo.getChannelsPerFeUnit();
      unsigned short feUnitsPerFed = mFed9UEventInfo.getFeUnitsPerFed();
      std::vector<unsigned short> feLengths(feUnitsPerFed, mFed9UEventInfo.getBytesInRawFed9UFE());
      std::vector<unsigned short> channelLengths(feUnitsPerFed*channelsPerFeUnit, mFed9UEventInfo.getBytesInRawFed9UChannel());
      //Put these vectors into the Fed9UBufferDescription
      bufDesc->setFeLengths(feLengths);
      bufDesc->setChannelLengths(channelLengths);

      //Construct the RAW data packet code
      unsigned short packetCode = 0xE0 | mFed9UEventInfo.getVirginRawModeCode() | mFed9UEventInfo.getFrameFindingModeCode();
      bufDesc->setPacketCode(packetCode);
      bufDesc->setMode( mFed9UEventInfo.getVirginRawModeCode() );
      bufDesc->trackerEventType( mFed9UEventInfo.getVirginRawDaqModeCode() ); // added by R.B.

      //Generate the DAQ header
      mBufHeader.generateFed9UDaqHeader(bufDesc);
      //Generate the tracker special header
      mBufHeader.generateFed9UTrackerSpecialDaqHeader(bufDesc);
      //Generate FE headers
      mBufHeader.generateFed9UFEHeader(bufDesc);
      //Before adding the data to the buffer, we need to dis-order the data to match APV-MUX order
      std::vector<unsigned short> disorderedFed9UBufferData = bufDesc->getFed9UBufferData();
      disOrderData(disorderedFed9UBufferData);
      bufDesc->setFed9UBufferData(disorderedFed9UBufferData);
      //Generate channel data.
      mBufData.generateFed9URawData(bufDesc);
      //Now add the DAQ trailer
      mBufTrailer.generateFed9UDaqTrailer(bufDesc);
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorRaw::generateBuffer."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorRaw::generateBuffer.."));
    }
  }
  
  void Fed9UBufferCreatorRaw::disOrderData(std::vector<unsigned short> &bufferData) const noexcept(false) {
    try {
      unsigned short stripsPerApv = mFed9UEventInfo.getStripsPerApv();
      unsigned short stripsPerChannel = stripsPerApv * mFed9UEventInfo.getApvsPerFedChannel();
      unsigned short chansPerFed = mFed9UEventInfo.getChannelsPerFeUnit() * mFed9UEventInfo.getFeUnitsPerFed();
      unsigned short stripsPerFed = stripsPerChannel * chansPerFed;
      ICUTILS_VERIFY(bufferData.size()==stripsPerFed)(bufferData.size())(stripsPerFed).error();
      //First make two vectors which contain the 1st and 2nd APV. We also have to take into account the interleaving here
      //(ie. the data from the APV is interleaved, as well as being disordered).
      std::vector<unsigned short> apv1;
      std::vector<unsigned short> apv2;
      apv1.reserve(stripsPerApv);
      apv2.reserve(stripsPerApv);
      //Make new large vector to hold dis-ordered, interleaved data.
      std::vector<unsigned short> newFed9UBufferData;
      //Ordering object
      Fed9U::Fed9USiStripReordering theFed9USiStripReordering;
      //Copy the data from each channel into the two vectors, dis-order them, and interleave them.
      for (int chan=0; chan<chansPerFed; ++chan) {
	for (int i = 0; i<stripsPerChannel; ++i) {
	  if (i<stripsPerApv) { //Get first APV data
	    apv1.push_back(bufferData[i+(chan*stripsPerChannel)]);
	  }
	  else { //Get second APV data
	    apv2.push_back(bufferData[i+(chan*stripsPerChannel)]);
	  }
	}
	//Now dis-order
	std::vector<unsigned short> disOrderedData1;
	std::vector<unsigned short> disOrderedData2;
	theFed9USiStripReordering.disOrderingAlgorithm(apv1, disOrderedData1);
	theFed9USiStripReordering.disOrderingAlgorithm(apv2, disOrderedData2);
	apv1.clear();
	apv2.clear();
	//Interleave data into new large vector
	for (int i = 0; i<stripsPerChannel; ++i) {
	  if ((i%2)==0) {
	    bufferData[i+(chan*stripsPerChannel)] = disOrderedData1[i/2];
	  }
	  else {
	    bufferData[i+(chan*stripsPerChannel)] = disOrderedData2[(i-1)/2];
	  }
	}
      }
    } 
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferCreatorRaw::disOrderData."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferCreatorRaw::disOrderData."));
    }
  }
    
}
  
