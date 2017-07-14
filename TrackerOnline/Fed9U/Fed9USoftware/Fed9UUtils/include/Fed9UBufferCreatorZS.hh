#ifndef FED9UBUFFERCREATORZS_H
#define FED9UBUFFERCREATORZS_H

#include "Fed9UBufferCreator.hh"
#include "Fed9UEventInfo.hh"
#include "Fed9UBufferHeader.hh"
#include "Fed9UBufferData.hh"
#include "Fed9UBufferTrailer.hh"

#include <vector>

namespace Fed9U {

  class Fed9UBufferDescription;

  //using.*std::vector;

  /**
   * \brief  Derived from the pABC Fed9UBufferCreator it generates a Fed9U buffer, which contain the zero suppression mode specific values.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   */
  class Fed9UBufferCreatorZS : public Fed9UBufferCreator {

  public:

   /**
     * \brief Part of the interface inherited from Fed9UBufferCreator it generates a Fed9U zero suppression mode data buffer.
     * \param bufDesc Pointer to a Fed9UBufferDescription, which provides a description of the buffer who's values are to
     *        be filled with the zero suppression mode specfic data.
     * \throw ICException Throws this for any error conditions that occur and will rethrow all thrown from other functions as this exception type.
     *
     * In zero suppression mode full data processing is performed. Strip re-odering, pedestal subtraction, common mode median subtraction and cluster finding.
     */
    void generateBuffer(Fed9UBufferDescription *bufDesc) noexcept(false);

  private:

    /**
     * \brief Method which will calculate the FE lengths and channel lengths (in bytes) needed to construct the buffer.
     * \param bufferData Contains the ADV values for all the strips in a FED. Size:24,576.
     * \param feLengths Lengths of the data in 32 bit words that are stored on each FE unit. Size:8.
     * \param channelLengths Length of the data in  32 bits words that are store on each FED channel. Size:96.
     *
     * It takes in the bufferData vector, which contains ADC values for all the strips in a FED, and searches for
     * non-zero values and clusters of values. The appropriate channel lengths and FE lengths are counted up during the 
     * vector search.
     */
    void findDataLengths(const std::vector<unsigned short> &bufferData, 
			 std::vector<unsigned short> &feLengths, 
			 std::vector<unsigned short> &channelLengths) noexcept(false);
      
    Fed9UEventInfo mFed9UEventInfo; //!< See the Fed9UEventInfo documentation for more information about this class.

    Fed9UBufferHeader mBufHeader;   //!< See the Fed9UBufferHeader documentation for more information about this class.
    Fed9UBufferData mBufData;       //!< See the Fed9UBufferData documentation for more information about this class.
    Fed9UBufferTrailer mBufTrailer; //!< See the Fed9UBufferTrailer documentation for more information about this class.

  };

}

#endif //FED9UBUFFERCREATORZS_H
