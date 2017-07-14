#ifndef FED9UBUFFERDATA_H
#define FED9UBUFFERDATA_H

#include "Fed9UEventInfo.hh"

#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  Using a Fed9UBufferDescription, which should contain the ADC settings found for each strip on the FED and using the appropriate settings,
   *         for the data acquision modes fills up the data buffer in Fed9UBufferDescription, thus creating a Fed9U event.
   * \author Matthew Pearson, RAL.
   * \date   03/2005.
   *
   * It should be used before constructing the classes that inherit from Fed9UBufferCreator.
   *
   * \todo - add a method for zero suppressed lite mode?
   */
  class Fed9UBufferData {

  public:

    /**
     * \brief Method which puts channel data in the Fed9UBufferDescription's data buffer in both the virgin raw and processed raw DAQ formats.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     * \throw ICException Throws this exception on error conditions and will catch and rethrow all errors from calling methods as this type.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9URawData(Fed9UBufferDescription *bufDesc ) noexcept(false);

    /**
     * \brief Method which puts channel data in the Fed9U data buffer in a zero suppressed DAQ format.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9UZSData(Fed9UBufferDescription *bufDesc) noexcept(false);

    /**
     * \brief Method which puts channel data in the Fed9U data buffer in a scope mode DAQ format.
     * \param bufDesc A pointer to the Fed9UBufferDescription, which is to be processed.
     * \param scope_mode_length Scope length of the data recorded during the event. It is the number of 32 words that are recorded for each APV input.
     *        It is taken to be the same across all FE units.
     *
     * Inside the Fed9UBufferDescription must be the vector of data, which contains the ADC values for each strip (in channel and strip order order). 
     * Fed9UBufferDescriptions internal buffer will be updated to contain the whole event.
     */
    void generateFed9UScopeModeData( Fed9UBufferDescription *bufDesc, unsigned short scope_mode_length ) noexcept(false);

  private:
    /**
     * \brief Method which can pad out the buffer until the start of the next 64-bit word.
     * \param bufDesc Pointer to the Fed9UBufferDescription that the class is processing.
     * \param cBytes Number of bytes used in the buffer for all the clusters in one FE unit. If operating in either virgin or processd raw mode
     *        this is zero.
     *
     * The ends of the FE data blocks in the FED buffer are padded out with zeros until the start of the next 64-bit words.
     */
    void paddToNext64BitWord(Fed9UBufferDescription *bufDesc, unsigned int cBytes) noexcept(false);

    Fed9UEventInfo mFed9UEventInfo; //!< See Fed9UEventInfo for more details on this class.

  };

}

#endif //FED9UBUFFERDATA_H
