#ifndef FED9UBUFFERCREATORSCOPEMODE_H
#define FED9UBUFFERCREATORSCOPEMODE_H

#include "Fed9UBufferCreator.hh"
#include "Fed9UEventInfo.hh"
#include "Fed9UBufferHeader.hh"
#include "Fed9UBufferData.hh"
#include "Fed9UBufferTrailer.hh"

namespace Fed9U {
  
  class Fed9UBufferDescription;
  
  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the scope mode specific values.
   * \author Rob Bainbridge, IC
   * \date   09/2005
   */
  class Fed9UBufferCreatorScopeMode : public Fed9UBufferCreator {
    
  public:

    /**
     * \brief Constructor.
     * \param length Length of the data recorded while in scope mode. The scope length.
     */
    Fed9UBufferCreatorScopeMode( unsigned short length = 280 );

    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U scope mode data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with scope mode specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In scope mode the full APV frames are recorded upon receipt of a trigger. A scope lengths worth of 23 bit words of the frame is recorded.
     */
    void generateBuffer( Fed9UBufferDescription *bufDesc ) noexcept(false);
    
  private:
    
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

    unsigned short length_;         //!< Scope length used during the event recording.
    
  };
  
}

#endif // FED9UBUFFERCREATORSCOPEMODE_H
