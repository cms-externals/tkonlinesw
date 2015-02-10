#include "Fed9UBufferData.hh"
#include "Fed9UBufferDescription.hh"

#include <iostream>

namespace Fed9U {

  //using std::exception;


  void Fed9UBufferData::generateFed9UScopeModeData( Fed9UBufferDescription *bufDesc, 
						    unsigned short sm_length ) throw (ICException) {

    try {

      //Getting the bufferData and the channelLengths vectors
      const std::vector<unsigned short> channelLengths = bufDesc->getChannelLengths();
      const std::vector<unsigned short> bufferData = bufDesc->getFed9UBufferData();
      
      unsigned short channelsPerFed = mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getChannelsPerFeUnit();
      
      //Check that the channelLengths vector has 96 elements
      ICUTILS_VERIFY( channelLengths.size()==channelsPerFed )( channelLengths.size() ).error();

      //Check the size of the bufferData vector
      ICUTILS_VERIFY( bufferData.size() == static_cast<unsigned int>( channelsPerFed * sm_length ) )( bufferData.size() ).error();
      
      //Get the packet code.
      unsigned short packetCode = bufDesc->getPacketCode();
      
      int offset = 0;
      //This dataIndex is to index where we are in the bufferData vector.
      unsigned short dataIndex = 0;

      //Loop over the channels
      for (int chan=0; chan<channelsPerFed; ++chan) {
	
	unsigned short length = channelLengths[chan];

	//Byte swap the length
	length = ((length & 0xFF) << 8) | ((length & 0xFF00) >> 8);

	//Add length and packet code and first byte of data to buffer - byte swap the data (ie. add LSBs first, MSBs last)
	if (offset==0) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0xFFFF) << 16) | ((packetCode & 0xFF) << 8) | (bufferData[dataIndex] & 0xFF);
	}
	else if (offset==1) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((length & 0xFF00) >> 8);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0x00FF) << 24) | ((packetCode & 0xFF) << 16) | ((bufferData[dataIndex] & 0xFF) << 8);
	}
	else if (offset==2) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | (length & 0xFFFF);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((packetCode & 0xFF) << 24) | ((bufferData[dataIndex] & 0xFF) << 16);
	}
	else if (offset==3) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((length & 0xFFFF) << 8) | (packetCode & 0xFF);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((bufferData[dataIndex] & 0xFF) << 24);
	}
	
	bufDesc->incrementBufferIndex();
	++dataIndex;
	
	//Now channel data (apart from overhanging bits).

	//Calculate loop size
	int num32BitWords = static_cast<int>( static_cast<float>( (sm_length-1) * 2.0 ) / 4.0 );

	for (int i=0; i<num32BitWords; ++i) {

	  unsigned int theData = ((((bufferData[dataIndex - 1] & 0xFF00) | (bufferData[dataIndex] & 0xFF)) << 16) | ((bufferData[dataIndex] & 0xFF00) | (bufferData[dataIndex+1] & 0xFF)));
	  dataIndex+=2;
	  if (offset==0) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = theData & 0xFFFFFFFF;
	  }
	  else if (offset==1) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFF000000) >> 24);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x00FFFFFF) << 8);
	  }
	  else if (offset==2) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFF0000) >> 16);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x0000FFFF) << 16);
	  }
	  else if (offset==3) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFFFF00) >> 8);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x000000FF) << 24);
	  }
	  bufDesc->incrementBufferIndex();
	}
	//Last bits (number of bits depends on value of offset)
	unsigned int theData = ((((bufferData[dataIndex - 1] & 0xFF00) | (bufferData[dataIndex] & 0xFF)) << 8) | ((bufferData[dataIndex] & 0xFF00) >> 8));
	++dataIndex;
	if (offset==0) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = theData << 8;
	}
	else if (offset==1) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFF0000) >> 16);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((theData & 0x00FFFF) << 16);
	}
	else if (offset==2) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFF00) >> 8);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((theData & 0x0000FF) << 24);
	}
	else if (offset==3) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | (theData & 0xFFFFFF);
	}
	++offset;
	if (offset<4) {
	  bufDesc->incrementBufferIndex();
	}
	else {
	  offset = 0;
	}
	//If we are at the end of a FE unit data block, padd the buffer until we reach the start of the next 64-bit word.
	if ((chan+1)%12 == 0) {
	  paddToNext64BitWord(bufDesc, 0); //Just pass in zero for any RAW data mode since for a FE we never have non-integer 32-bit words filled up.
	}
      }  
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferData::generateFed9URawData."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferData::generateFed9URawData."));
    }
  }


  void Fed9UBufferData::generateFed9URawData(Fed9UBufferDescription *bufDesc) throw (ICException) {
    try {
      //Getting the bufferData and the channelLengths vectors
      const std::vector<unsigned short> channelLengths = bufDesc->getChannelLengths();
      const std::vector<unsigned short> bufferData = bufDesc->getFed9UBufferData();
      
      unsigned short channelsPerFed = mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getChannelsPerFeUnit();
      
      //Check that the channelLengths vector has 96 elements
      ICUTILS_VERIFY(channelLengths.size()==channelsPerFed)(channelLengths.size()).error();
      //Check the size of the bufferData vector
      ICUTILS_VERIFY(bufferData.size()==static_cast<unsigned int>(channelsPerFed * mFed9UEventInfo.getStripsPerApv() * mFed9UEventInfo.getApvsPerFedChannel()))(bufferData.size()).error();
      
      //Get the packet code.
      unsigned short packetCode = bufDesc->getPacketCode();
      
      int offset = 0;
      //This dataIndex is to index where we are in the bufferData vector.
      unsigned short dataIndex = 0;
      //Loop over the channels
      for (int chan=0; chan<channelsPerFed; ++chan) {
	
	unsigned short length = channelLengths[chan];
	//Byte swap the length
	length = ((length & 0xFF) << 8) | ((length & 0xFF00) >> 8);
	//Add length and packet code and first byte of data to buffer - byte swap the data (ie. add LSBs first, MSBs last)
	if (offset==0) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0xFFFF) << 16) | ((packetCode & 0xFF) << 8) | (bufferData[dataIndex] & 0xFF);
	}
	else if (offset==1) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((length & 0xFF00) >> 8);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0x00FF) << 24) | ((packetCode & 0xFF) << 16) | ((bufferData[dataIndex] & 0xFF) << 8);
	}
	else if (offset==2) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | (length & 0xFFFF);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((packetCode & 0xFF) << 24) | ((bufferData[dataIndex] & 0xFF) << 16);
	}
	else if (offset==3) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((length & 0xFFFF) << 8) | (packetCode & 0xFF);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((bufferData[dataIndex] & 0xFF) << 24);
	}
	bufDesc->incrementBufferIndex();
	++dataIndex;
	
	//Now channel data (apart from overhanging bits).
	//Calculate loop size
	int num32BitWords = static_cast<int>(static_cast<float>(((mFed9UEventInfo.getApvsPerFedChannel() * mFed9UEventInfo.getStripsPerApv()) - 1)*2.0) / 4.0);
	for (int i=0; i<num32BitWords; ++i) {
	  unsigned int theData = ((((bufferData[dataIndex - 1] & 0xFF00) | (bufferData[dataIndex] & 0xFF)) << 16) | ((bufferData[dataIndex] & 0xFF00) | (bufferData[dataIndex+1] & 0xFF)));
	  dataIndex+=2;
	  if (offset==0) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = theData & 0xFFFFFFFF;
	  }
	  else if (offset==1) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFF000000) >> 24);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x00FFFFFF) << 8);
	  }
	  else if (offset==2) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFF0000) >> 16);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x0000FFFF) << 16);
	  }
	  else if (offset==3) {
	    bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFFFF00) >> 8);
	    bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((theData & 0x000000FF) << 24);
	  }
	  bufDesc->incrementBufferIndex();
	}
	//Last bits (number of bits depends on value of offset)
	unsigned int theData = ((((bufferData[dataIndex - 1] & 0xFF00) | (bufferData[dataIndex] & 0xFF)) << 8) | ((bufferData[dataIndex] & 0xFF00) >> 8));
	++dataIndex;
	if (offset==0) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = theData << 8;
	}
	else if (offset==1) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFF0000) >> 16);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((theData & 0x00FFFF) << 16);
	}
	else if (offset==2) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | ((theData & 0xFFFF00) >> 8);
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((theData & 0x0000FF) << 24);
	}
	else if (offset==3) {
	  bufDesc->getBuffer()[bufDesc->getBufferSize()-1] = bufDesc->getBuffer()[bufDesc->getBufferSize()-1] | (theData & 0xFFFFFF);
	}
	++offset;
	if (offset<4) {
	  bufDesc->incrementBufferIndex();
	}
	else {
	  offset = 0;
	}
	//If we are at the end of a FE unit data block, padd the buffer until we reach the start of the next 64-bit word.
	if ((chan+1)%12 == 0) {
	  paddToNext64BitWord(bufDesc, 0); //Just pass in zero for any RAW data mode since for a FE we never have non-integer 32-bit words filled up.
	}
      }  
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferData::generateFed9URawData."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferData::generateFed9URawData."));
    }
  }


  void Fed9UBufferData::generateFed9UZSData(Fed9UBufferDescription *bufDesc) throw (ICException) {
    try {
      //Getting the bufferData and the channelLengths vectors
      const std::vector<unsigned short> channelLengths = bufDesc->getChannelLengths();
      const std::vector<unsigned short> bufferData = bufDesc->getFed9UBufferData();

      //Check that the channelLengths vector has 96 elements
      ICUTILS_VERIFY(channelLengths.size()==static_cast<unsigned int>(mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getChannelsPerFeUnit()))(channelLengths.size()).error();
      
      //Get the packet code.
      unsigned short packetCode = bufDesc->getPacketCode();
       
      // BELOW: ADDED BY RB !!!  
      int n_chans = mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getChannelsPerFeUnit() * mFed9UEventInfo.getApvsPerFedChannel();
      std::vector<unsigned short> cm(n_chans,0);
      bufDesc->setCommonModeMedians( cm );
      // ABOVE: ADDED BY RB !!!

      //@@ BUG HERE: "get" method returns vector that is never filled!!! (temp fix above)
      //Get the vector of common mode median values
      std::vector<unsigned short> theCMMedians = bufDesc->getCommonModeMedians();

      //Loop over the channels (96)
      unsigned int cBytes = 0; //Number of bytes needed for the clusters in a front end.
      unsigned int chanOffset = 0; //This is the channel offset for the bufferData vector.
      int stripsPerFedChannel = mFed9UEventInfo.getApvsPerFedChannel() * mFed9UEventInfo.getStripsPerApv();
      for (int chan=0; chan<(mFed9UEventInfo.getFeUnitsPerFed() * mFed9UEventInfo.getChannelsPerFeUnit()); ++chan) {
	//First add the channel length and packet code and medians
	//The format for one hit per channel is like:
	//Length bytes + packet code + median 1 + median 2 + cluster address + cluster length + data 1
	//The median values are two bytes each, so we have to byte swap them before adding to the buffer.
	int median1 = ((theCMMedians[chan*2] & 0xFF) << 8) | ((theCMMedians[chan*2] & 0xFF00) >> 8);
	int median2 = ((theCMMedians[(chan*2)+1] & 0xFF) << 8) | ((theCMMedians[(chan*2)+1] & 0xFF00) >> 8);
	//Get the length for this channel, and byte swap it.
	int length = static_cast<int>(channelLengths[chan]);
	////std::cout << "chan " << static_cast<int>(Fed9UAddress().setFedChannel(chan).getExternalFedChannel() ) << " length is: " << length << std::endl;
	length = ((length & 0xFF) << 8) | ((length & 0xFF00) >> 8);
	switch (cBytes%4) {
	case 0:
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0xFFFF) << 16) | ((packetCode & 0xFF) << 8) | ((median1 & 0xFF00) >> 8);
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((median1 & 0xFF) << 24) | ((median2 & 0xFF00) << 8) | ((median2 & 0xFF) << 8);
	  break;
	case 1: 
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((length & 0xFFFF) << 8) | (packetCode & 0xFF);
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((median1 & 0xFFFF) << 16) | (median2 & 0xFFFF);
	  bufDesc->incrementBufferIndex();
	  break;
	case 2:
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | (length & 0xFFFF);
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((packetCode & 0xFF) << 24) | ((median1 & 0xFFFF) << 8) | ((median2 & 0xFF00) >> 8); 
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((median2 & 0xFF) << 24);
	  break;
	case 3:
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((length & 0xFF00) >> 8);
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((length & 0x00FF) << 24) | ((packetCode & 0xFF) << 16) | (median1 & 0xFFFF);
	  bufDesc->incrementBufferIndex();
	  bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((median2 & 0xFFFF) << 16);
	  break;
	default:
	  break;
	}
	//Add 7 bytes to the byte counter to take account of the header we have added.
	cBytes+=7;
	
	//Search the data for this channel to find clusters. Then add them to the buffer.
	int clusterLength = 0;
	int clusterAddress = -1;
	for (int strip=0; strip<stripsPerFedChannel; ++strip) {
	  //Find the clusters, store the address and count the length.
	  if (bufferData[(chan*stripsPerFedChannel)+strip]!=0) {
	    //Found a cluster
	    ++clusterLength;
	    if (clusterAddress==-1) {
	      clusterAddress=strip;
	    }
	  }
	  //Test for the end of a cluster and if we have reached the end of a channel (with a hit in the last strip).
	  //Also test for a cluster on the boundary of an APV. If there is then force the cluster
	  //to end on the boundary. This is a consequence of FED board operations, 
	  //since the maximum cluster size in practice is 128 strips.
	  chanOffset = chan*stripsPerFedChannel;
	  if (((bufferData[chanOffset+strip]==0)&&(clusterLength!=0)) || 
	      (((strip+1)==stripsPerFedChannel)&&(clusterLength!=0)) || 
	      ((strip==(mFed9UEventInfo.getStripsPerApv()-1))&&(clusterLength!=0))) { 
	    //Add cluster header to buffer
	    //Format is like: cluster address (1 bytes) + cluster length (1 byte)
	    switch (cBytes%4) {
	    case 0:
	      bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((clusterAddress & 0xFF) << 24) | ((clusterLength & 0xFF) << 16); 
	      break;
	    case 1:
	      bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((clusterAddress & 0xFF) << 16) | ((clusterLength & 0xFF) << 8); 
	      break;
	    case 2:
	      bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((clusterAddress & 0xFF) << 8) | (clusterLength & 0xFF);
	      bufDesc->incrementBufferIndex();
	      break;
	    case 3:
	      bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | (clusterAddress & 0xFF); 
	      bufDesc->incrementBufferIndex();
	      bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((clusterLength & 0xFF) << 24);
	      break;
	    default:
	      break;
	    }
	    //Add 2 bytes to the byte counter to take account of the cluster header we have just added to the buffer.
	    cBytes+=2;
	    
	    //Now add the cluster data
	    for (int clusterStrip=0; clusterStrip<clusterLength; ++clusterStrip) {
	      switch (cBytes%4) {
	      case 0:
		bufDesc->getBuffer()[bufDesc->getBufferSize()] = ((bufferData[chanOffset+clusterAddress+clusterStrip] & 0xFF) << 24);
		break;
	      case 1:
		bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((bufferData[chanOffset+clusterAddress+clusterStrip] & 0xFF) << 16);
		break;
	      case 2:
		bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | ((bufferData[chanOffset+clusterAddress+clusterStrip] & 0xFF) << 8);
		break;
	      case 3:
		bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] | (bufferData[chanOffset+clusterAddress+clusterStrip] & 0xFF);
		bufDesc->incrementBufferIndex();
		break;
	      default:
		break;
	      }
	      //Add a byte to take account of the cluster we have just added to the buffer.
	      ++cBytes;
	    }
	    //Reset bufferLength and bufferAddress for possible next cluster in this channel
	    clusterLength = 0;
	    clusterAddress = -1;
	  }
	}
	//If we are at the end of a FE unit data block, padd the buffer until we reach the start of the next 64-bit word.
	if ((chan+1)%12 == 0) {
	  paddToNext64BitWord(bufDesc, cBytes);
	  cBytes = 0; //Reset cBytes for the next FE.
	}
      }
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferData::generateFed9UZSData."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferData::generateFed9UZSData."));
    }
  }


  void Fed9UBufferData::paddToNext64BitWord(Fed9UBufferDescription *bufDesc, unsigned int cBytes) throw (ICException) {
    try {
      //Padd remaining 32-bit word
      switch(cBytes%4) {
      case 1: 
	bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] & 0xFF000000; 
	bufDesc->incrementBufferIndex();
	break;
      case 2: 
	bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] & 0xFFFF0000;
	bufDesc->incrementBufferIndex();
	break;
      case 3: 
	bufDesc->getBuffer()[bufDesc->getBufferSize()] = bufDesc->getBuffer()[bufDesc->getBufferSize()] & 0xFFFFFF00;
	bufDesc->incrementBufferIndex();
	break;
      default: break;
      }
      
      //Padd to next 64-bit word
      if (bufDesc->getBufferSize()%2) {
	bufDesc->getBuffer()[bufDesc->getBufferSize()] = 0;
	bufDesc->incrementBufferIndex();
      }
    }
    catch (std::exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferData::paddToNext64BitWord."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferData::paddToNext64BitWord."));
    }
  }
  
}
