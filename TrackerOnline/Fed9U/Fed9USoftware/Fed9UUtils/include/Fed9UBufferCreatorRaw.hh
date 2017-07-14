#ifndef FED9UBUFFERCREATORRAW_H
#define FED9UBUFFERCREATORRAW_H

#include "Fed9UBufferCreator.hh"
#include "Fed9UEventInfo.hh"
#include "Fed9UBufferHeader.hh"
#include "Fed9UBufferTrailer.hh"
#include "Fed9UBufferData.hh"

#include <vector>

namespace Fed9U {

  //using.*std::vector;

  class Fed9UBufferDescription;
  
  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the virgin raw data specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorRaw : public Fed9UBufferCreator {
    
  public:
    
    /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U virgin raw data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with virgin raw data specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In virgin raw data mode the data is left untouch and is exactly as it comes from the FED.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) noexcept(false);
    
  private:
  
    /**
     * \brief In virgin raw data mode the strip data from the APVs is in the APV-MUX order and this member function
     *        will put strip ordered data into APV-MUX ordered data.
     * \param bufferData Reference to the buffer data to be reordered into MUX ordering. Size: 24,576, number of strips on the FED.
     * \throw ICException  Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * \todo - method very inefficient. Improve!
     */
    void disOrderData(std::vector<unsigned short> &bufferData) const noexcept(false);
      
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.
    
  };
  
}

#endif //FED9UBUFFERCREATORRAW_H
