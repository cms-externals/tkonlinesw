#ifndef FED9UBUFFERHEADER_H
#define FED9UBUFFERHEADER_H

#include "Fed9UEventInfo.hh"

#include "ICExDecl.hh"

namespace Fed9U {

  //using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  This class provides methods to construct the header part of the Fed9U buffer.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It should be used to compose classes which inherit from Fed9UBufferCreator.
   */
  class Fed9UBufferHeader {
    
  public:

    /**
     * \brief This constructs the Fed9U DAQ header at the start of the Fed9U buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is 8 bytes, which contains the bunch crossing number and FED mode.
     */
    void generateFed9UDaqHeader(Fed9UBufferDescription *theBufDesc) noexcept(false);

    /**
     * \brief This constructs the Fed9U Tracker Special DAQ header in the 2nd 64-bit word of the Fed9U buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is another 8 bytes which contain FED debug information.
     */
    void generateFed9UTrackerSpecialDaqHeader(Fed9UBufferDescription *theBufDesc) noexcept(false);

    /**
     * \brief This adds the FE header information to the Fed9U buffer. 
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * This is 16 bytes per FE (128 bytes in total). The feLength vector inside the Fed9UBufferDescription should contain 
     * the 8 FE lengths in bytes for all channel info for that FE (including channel header and packet code).
     * They do not include the length of the FE header itself.
     */
    void generateFed9UFEHeader(Fed9UBufferDescription *theBufDesc) noexcept(false);

  private:

    Fed9UEventInfo mFed9UEventInfo; //!< Contains the various event settings. See Fed9UEventInfo class for more information.

  };

}

#endif //FED9UBUFFERHEADER_H
