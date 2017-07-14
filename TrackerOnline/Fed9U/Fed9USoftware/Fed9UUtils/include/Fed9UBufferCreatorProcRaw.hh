#ifndef FED9UBUFFERCREATORPROCRAW_H
#define FED9UBUFFERCREATORPROCRAW_H

#include "Fed9UBufferCreator.hh"
#include "Fed9UEventInfo.hh"
#include "Fed9UBufferHeader.hh"
#include "Fed9UBufferData.hh"
#include "Fed9UBufferTrailer.hh"

namespace Fed9U {

  class Fed9UBufferDescription;

  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the processed raw data specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorProcRaw : public Fed9UBufferCreator {

  public:

    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U processed raw data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with processed raw data specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In processed raw data mode the FED will perform strip reodering and pedestal subtraction.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) noexcept(false);

  private:

    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

  };

}

#endif //FED9UBUFFERCREATORPROCRAW_H
