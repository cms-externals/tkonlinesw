#ifndef FED9UBUFFERGENERATOR_H
#define FED9UBUFFERGENERATOR_H

#include "Fed9UBufferDescription.hh"
#include "Fed9UEventInfo.hh"

#include "ICExDecl.hh"

#include<vector>

namespace Fed9U {

  class Fed9UBufferCreator;

  //using ICUtils::ICException;

  /**
   * \brief  User interface class to create Fed9U buffers.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It can be used to create any type of Fed9U buffer, providing it has been given a pointer to an object which inherits from Fed9UBufferCreator.
   * This class gives the user an interface to set various buffer paremeters, such as the data, bunch crossing number, etc. It also provides
   * methods which allow the user to get a copy of the buffer once it has been generated.
   */
  class Fed9UBufferGenerator {

  public:
    /**
     * \brief Constructor. Takes in a pointer to the concrete implementation of Fed9UBufferCreator.
     */
    Fed9UBufferGenerator(Fed9UBufferCreator *theCreator);

    /**
     * \brief Method to generate the Fed9U buffer.
     * \param bufferData Reference to the vector containing the ADC values for each strip on a FED. Size:24,576.
     * \throw ICException Throws ICExceptions for all errors generated by the member function and caught and rethrow by the member function.
     *
     * The first element of the vector corresponds to strip 1, channel 96 in the external numbering or strip 0 or channel 0 internal numbering.
     * The strip order should then follow in physical strip ordering and not APV-MUX order.
     */
    void generateFed9UBuffer(const std::vector<unsigned short> &bufferData) noexcept(false);

    /**
     * \name Buffer getters.
     *
     * The methods to allow the user to get the buffer which is created.
     */
    //@{
    /**
     * \brief Method to copy the Fed9U buffer from internal memory into an area pointed to by the given pointer.
     * \param fedBuffer Area to copy memory to.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    void getBuffer(unsigned int *fedBuffer) const noexcept(false);

    /**
     * \brief  Method to return copy of Fed9U buffer.
     * \return vector<unsigned char> A vector containing a copy of all the data in the internal buffer.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    std::vector<unsigned char> getBuffer() noexcept(false);
    //@}

    /**
     * \name Parameter getters
     *
     * Methods for retrieving the buffer parameters.
     */

    /**
     * \brief  Method to get the buffer size in units of 32-bit words.
     * \return unsigned long Number of 32-bit words in the buffer.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    unsigned long getBufferSize(void) const noexcept(false);

    /**
     * \brief  Method to get the bunch crossing number.
     * \return unsigned long Bunch crossing number for which the event was recorded.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    unsigned long getBunchCrossing(void) const noexcept(false);

    //@}

    /**
     * \name Buffer setters
     *
     * Methods for the setting buffer parameters.
     */
    //@{

    /**
     * \brief  Method to set the bunch crossing number.
     * \param  bunchCrossing Bunch crossing number for which the event was recorded.
     * \return Self reference.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setBunchCrossing(unsigned long bunchCrossing) noexcept(false);

    /**
     * \brief  Method to set the tracker header to full debug mode.
     * \return Self reference
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     *
     * The method provides no way to undo this once it has been done. The default mode for an event buffer is APV error mode
     * and no change should be made if this mode is required.
     */
    Fed9UBufferGenerator & setHeaderToFullDebug(void) noexcept(false);

    /**
     * \brief  Method to set the buffer creator pointer. This allows the user to change mode between generating events.
     * \param  theCreator The concrete implementation of the FED buffer that is to be set.
     * \return Self reference.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setFed9UBufferCreator(Fed9UBufferCreator *theCreator);

    /**
     * \brief  Method to set the common mode median values for the buffer when running in zero suppressed full debug mode.
     * \param  cmMedians Reference to a vector who's elements contain the common mode median values calculated for the APVs
     *         in zero suppression mode. There is one element for each APV on the FED and it uses the internal numbering with
     *         a base of zero. Size:192.
     * \throw  ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    Fed9UBufferGenerator & setCommonModeMedians(const std::vector<unsigned short> &cmMedians) noexcept(false);

    Fed9UBufferGenerator & setSlink64() noexcept(false);

    //@}
 
 private:

    Fed9UBufferCreator *mFed9UBufferCreator;        //!< Pointer to the Fed9UBufferCreator concrete implementation class, used to create
                                                    //!< the appropriately structure buffer.

    Fed9UBufferDescription mFed9UBufferDescription; //!< Provides a description of the buffer that is to be created. See Fed9UBufferDescription for more details.

    Fed9UEventInfo mFed9UEventInfo;                 //!< Contains the various event settings. See Fed9UEventInfo class for more information.

  };

}

#endif
