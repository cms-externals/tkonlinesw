#include "Fed9UBufferGenerator.hh"
#include "Fed9UBufferCreator.hh"
#include <cstring>

namespace Fed9U {

  //using std::exception;


  Fed9UBufferGenerator::Fed9UBufferGenerator(Fed9UBufferCreator *theCreator) : mFed9UBufferCreator(theCreator) {
  }


  void Fed9UBufferGenerator::generateFed9UBuffer(const std::vector<unsigned short> &bufferData) noexcept(false) {
    try {
      //First, initialize the buffer index inside the Fed9UBufferDescription
      mFed9UBufferDescription.initBufferIndex();
      //Also init the buffer space
      mFed9UBufferDescription.initBuffer();
      //Put the bufferData into the Fed9UBufferDescription
      mFed9UBufferDescription.setFed9UBufferData(bufferData);
      //Generate the buffer.
      mFed9UBufferCreator->generateBuffer(&mFed9UBufferDescription);
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::generateFed9UBuffer."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::generateFed9UBuffer."));
    }
  }


  Fed9UBufferGenerator & Fed9UBufferGenerator::setBunchCrossing(const unsigned long bunchCrossing) noexcept(false) {
    try {
      mFed9UBufferDescription.setBunchCrossing(bunchCrossing);
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::setBunchCrossing."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::setBunchCrossing."));
    }
    return *this;
  }


  Fed9UBufferGenerator & Fed9UBufferGenerator::setHeaderToFullDebug(void) noexcept(false) {
    try {
      mFed9UBufferDescription.setHeaderFormat(mFed9UEventInfo.getTrackerSpecialHeaderFullDebugMode());
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::setHeaderToFullDebug."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::setHeaderToFullDebug."));
    }
    return *this;
  }


  Fed9UBufferGenerator & Fed9UBufferGenerator::setFed9UBufferCreator(Fed9UBufferCreator *theCreator) {
    if (theCreator) {
      mFed9UBufferCreator = theCreator;
    }
    return *this;
  }


  Fed9UBufferGenerator & Fed9UBufferGenerator::setCommonModeMedians(const std::vector<unsigned short> &cmMedians) noexcept(false) {
    try {
      mFed9UBufferDescription.setCommonModeMedians(cmMedians);
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::setCommonModeMedians."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::setCommonModeMedians."));
    }
    return *this;
  }

  Fed9UBufferGenerator & Fed9UBufferGenerator::setSlink64() noexcept(false) {

    try {
      mFed9UBufferDescription.setSlink64();
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::setSlink64."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::setSlink64."));
    }
    return *this;
  }
    
  void Fed9UBufferGenerator::getBuffer(unsigned int *fedBuffer) const noexcept(false) {
    try {
      if ( (mFed9UBufferDescription.getBufferConst()) && 
	   (mFed9UBufferDescription.getBufferSize()!=0) ) {
	memcpy( reinterpret_cast<char*>(fedBuffer), 
		reinterpret_cast<char*>(mFed9UBufferDescription.getBufferConst()), 
		(static_cast<unsigned int>(static_cast<float>(mFed9UBufferDescription.getBufferSize())*4.0)) );
      }
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::getBuffer."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::getBuffer."));
    }
  }


  std::vector<unsigned char> Fed9UBufferGenerator::getBuffer() noexcept(false) {
    std::vector<unsigned char> buffer; // returned object
    try {
      if ( mFed9UBufferDescription.getBufferConst() && 
	   mFed9UBufferDescription.getBufferSize() ) {
	unsigned char* chars = reinterpret_cast<unsigned char*>( mFed9UBufferDescription.getBuffer() );
 	unsigned long nbytes = mFed9UBufferDescription.getBufferSize() * 4;
	for ( unsigned long ii = 0; ii < nbytes; ii++ ) { buffer.push_back( chars[ii] ); }
      }
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::getBuffer."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::getBuffer."));
    }
    return buffer;
  }
    

  unsigned long Fed9UBufferGenerator::getBufferSize(void) const noexcept(false) {
    try {
      return mFed9UBufferDescription.getBufferSize();
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::getBufferSize."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::getBufferSize."));
    }
  }


  unsigned long Fed9UBufferGenerator::getBunchCrossing(void) const noexcept(false) {
    try {
      return mFed9UBufferDescription.getBunchCrossing();
    }
    catch (std::exception &e) {
      RETHROW(e, ICUtils::ICException("std::exception caught in Fed9UBufferGenerator::getBunchCrossing."));
    }
    catch (...) {
      THROW(ICUtils::ICException("Unknown exception caught in Fed9UBufferGenerator::getBunchCrossing."));
    }
  }

}
