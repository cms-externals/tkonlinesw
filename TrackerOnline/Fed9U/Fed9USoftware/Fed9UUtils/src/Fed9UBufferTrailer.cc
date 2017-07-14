#include "Fed9UBufferTrailer.hh"
#include "Fed9UBufferDescription.hh"

namespace Fed9U {

  using std::exception;


  void Fed9UBufferTrailer::generateFed9UDaqTrailer(Fed9UBufferDescription *theBufDesc) noexcept(false) {
    try {
      unsigned int totalLength = static_cast<unsigned int>(static_cast<float>(theBufDesc->getBufferSize() + 2) / 2.0); 
      
      //Add to the buffer
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = (0xA0 << 24) | (totalLength & 0xFFFFFF); //The 0xA marks the start of the DAQ trailer.
      theBufDesc->incrementBufferIndex();
      theBufDesc->getBuffer()[theBufDesc->getBufferSize()] = 0x00000000;
      theBufDesc->incrementBufferIndex();
    }
    catch (exception &e) {
      RETHROW(e, ICException("std::exception caught in Fed9UBufferTrailer::generateFed9UDaqTrailer."));
    }
    catch (...) {
      THROW(ICException("Unknown exception caught in Fed9UBufferTrailer::generateFed9UDaqTrailer."));
    }
  }

}
