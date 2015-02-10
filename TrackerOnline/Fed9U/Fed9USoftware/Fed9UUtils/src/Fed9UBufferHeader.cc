#include "Fed9UBufferHeader.hh"
#include "Fed9UBufferDescription.hh"

namespace Fed9U {

  //using std::exception;


  void Fed9UBufferHeader::generateFed9UDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException) {
    try {
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = ((0x5000 | ((theBufDesc->getMode() & 0xf) << 8)) << 16) + 0x0000; //The 0x5 is the start of the buffer.
      theBufDesc->incrementBufferIndex();
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = ((((theBufDesc->getBunchCrossing() & 0xFFF) << 4) | 0x0) << 16) + 0x1218; //bx is in 12 MSB //The 0x1218 is copied from an event dump.
      theBufDesc->incrementBufferIndex();
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferHeader::generateFed9UDaqHeader."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferHeader::generateFed9UDaqHeader."));
    }
  }


  void Fed9UBufferHeader::generateFed9UTrackerSpecialDaqHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException) {
    try {
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = 
	(0xED << 24) | // Temporary label to indicate that new tracker-specific header is being used
	((theBufDesc->getHeaderFormat() & 0xF) << 20) | // header format (APV error, full debug mode)
	((theBufDesc->trackerEventType() & 0xF) << 16) | // FED readout mode
	0x00000; // To be expanded...
      theBufDesc->incrementBufferIndex();
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = 
	(0xFF << 24) |
	0x00000; // To be expanded...
      theBufDesc->incrementBufferIndex();
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferHeader::generateFed9UTrackerSpecialDaqHeader."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferHeader::generateFed9UTrackerSpecialDaqHeader."));
    }
  }


  void Fed9UBufferHeader::generateFed9UFEHeader(Fed9UBufferDescription *theBufDesc) throw (ICUtils::ICException) {
    try {
      //Check that the feLengths vector has 8 elements
      const std::vector<unsigned short> feLengths = theBufDesc->getFeLengths();
      ICUTILS_VERIFY(feLengths.size()==mFed9UEventInfo.getFeUnitsPerFed())(feLengths.size()).error();
      
      //Loop over the 8 FEs
      for (int fe=0; fe<mFed9UEventInfo.getFeUnitsPerFed(); ++fe) {
	//Set all frame sync bits to 1
	theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = 0xFFFFFFFF; 
	theBufDesc->incrementBufferIndex();
	theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = 0xFFFFFFFF; 
	theBufDesc->incrementBufferIndex();
	//Set the 16-bit length and 16 zeros
	theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = ((feLengths[fe] & 0xFFFF) << 16) | (0x0000); 
	theBufDesc->incrementBufferIndex();
	//Now we have another 16 zeros and then 16 sync bits (set these to 1's)
	theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = (0x0000 << 16) | (0xFFFF);
	theBufDesc->incrementBufferIndex();
      }
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferHeader::generateFed9UFEHeader."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught Fed9UBufferHeader::generateFed9UFEHeader."));
    }
  }
  
  
}
