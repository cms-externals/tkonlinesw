#ifndef FED9UBUFFERDESCRIPTION_H
#define FED9UBUFFERDESCRIPTION_H

#include "Fed9UEventInfo.hh"

#include "ICExDecl.hh"

#include <vector>

namespace Fed9U {

  using ICUtils::ICException;
  //using.*std::vector;
  
  /**Matthew Pearson, RAL, Feb 2005. */

  /**
   * \brief  Container class, which holds the Fed9U buffer parameters.
   * \author Matthew Pearson, RAL.
   * \date   02/2005.
   *
   * It is to be used by the Fed9UBufferCreator derived classes for creating Fed9U buffers for
   * the various event modes. The description contains the data buffer as well as the various header
   * and trailers, which need to be defined in the Fed9UBufferCreator derived classes.
   */
  class Fed9UBufferDescription {

  public:
    /**
     * \brief Constructor.
     *
     * Performs the initialization of all the internal parameters, including the buffer.
     */
    Fed9UBufferDescription(void);

    /**
     * \brief Destructor.
     */
    virtual ~Fed9UBufferDescription() {}

    /**Methods to manipulate the internal Fed9U buffer.*/

    /**
     * \name Fed9U buffer manipulators.
     *
     * The following member functions are all used to manipulate the Fed9U buffer.
     */
    //@{

    /**
     * \brief  Method to provide access to the internal Fed9U buffer.
     * \return unsigned int* Pointer to the beginning of the buffer.
     */
    inline unsigned int * getBuffer(void) {
      return mBuf;
    }

    /**
     * \brief  Method to provide access to the internal Fed9U buffer. No changes can be made to the buffer using this method.
     * \return unsigned int* Pointer the beginning of the constant buffer.
     */
    inline unsigned int * getBufferConst(void) const {
      return const_cast<unsigned int *>(mBuf);
    }

    /**
     * \brief  Method which returns the current size of the buffer (in units of 32-bit words).
     * \return unsigned long Number of 32-bit words in the buffer.
     */
    inline unsigned long getBufferSize(void) const {
      return mIndex;
    }

    /**
     * \brief Increment buffer index.
     *
     * This increments the index parameter for the buffer (it is also the buffer size after the buffer has been made).
     */
    inline void incrementBufferIndex(void) {
      ++mIndex;
    }

    /**
     * \brief Initialize the buffer index to zero.
     *
     * This should be called before generating a new buffer.
     */
    inline void initBufferIndex(void) {
      mIndex = 0;
    }

    /**
     * \brief Initialize the buffer space, called before generating a new buffer.
     */
    void initBuffer(void);
    //@}

    /**
     * \name Setters
     *
     * Methods to set the buffer parameters.
     */
    //@{

    /**
     * \brief  Method to set the bunch crossing number.
     * \param  bunchCrossing Bunch crossing number to be stored in the buffer.
     * \return Self reference
     */
    Fed9UBufferDescription & setBunchCrossing(unsigned long bunchCrossing);

    /**
     * \brief  Method to set the FED data acquision mode.
     * \param  mode DAQ mode to be set.
     * \return Self reference
     *
     * \todo - check this method to see what it is and possible values. It is probably the "Evt_ty", and if so should do something else or be removed.
     */
    Fed9UBufferDescription & setMode(unsigned long mode);

    /**
     * \brief  Method to set the header format in the tracker special header.
     * \param  headerFormat Valid value are:
     *         \li 1: Full debug mode.
     *         \li 2: APV error mode.
     * \return Self reference
     */
    Fed9UBufferDescription & setHeaderFormat(unsigned short headerFormat);

    //<RB>
    //Added tracker event type header.
    /**
     * \brief  Sets the FED readout mode.
     * \param  value "Trkr Evt_Ty" field of the Tracker-specific header, possible values:
     *         \li 1 Scope mode real.
     *         \li 2 Virgin raw real.
     *         \li 3 Virgin raw fake.
     *         \li 6 Processed raw real.
     *         \li 7 Processed raw fake.
     *         \li 10 Zero suppressed real.
     *         \li 11 Zero suppressed fake.
     *         \li 12 Zero suppressed lite real.
     *         \li 13 Zero suppressed lite fake.
     * \return Self reference
     *
     * More details on the running modes can be found in the FED FE FPGA documentation.
     */
    Fed9UBufferDescription& trackerEventType( unsigned short value );
    //</RB>

    /**
     * \brief  Method to set the buffer data vector.
     * \param  bufferData Each element contains the reading from a strip, there must be one element for each strip on a FED. Size:24,576.
     * \return Self reference
     */
    Fed9UBufferDescription & setFed9UBufferData(const std::vector<unsigned short> &bufferData) noexcept(false);

    /**
     * \brief  Method to set the channel length vector.
     * \param  channelLengths Lengths of the data recorded on each channel. Should contain one element for each channel
     *         on the FED. Size:96.
     * \return Self reference
     */
    Fed9UBufferDescription & setChannelLengths(const std::vector<unsigned short> &channelLengths) noexcept(false);

    /**
     * \brief  Method to set the FE length vector.
     * \param  feLengths Each vector element contains the length of the data on the FE units. There must be one element for each
     *         FE unit on the FED. Size:8.
     * \return Self reference
     */
    Fed9UBufferDescription & setFeLengths(const std::vector<unsigned short> &feLengths) noexcept(false);

    /**
     * \brief  Method to set the packet code.
     * \param  packetCode Packet code to be set.
     * \return Self reference
     *
     * \todo - bit more information on this.
     */
    Fed9UBufferDescription & setPacketCode(unsigned short packetCode);

    /**
     * \brief  Method to set the common mode median values.
     * \param  cmMedians Common mode median value recorded by each APV. There must be one element for each APV on the FED. Size:192.
     * \return Self reference
     */
    Fed9UBufferDescription & setCommonModeMedians(const std::vector<unsigned short> &cmMedians) noexcept(false);

   /**
     * \brief  Method to 32-bit swap data into slink format. Should only be performed AFTER buffer generation.
     * \param  None.
     * \return Self reference
     */
    Fed9UBufferDescription & setSlink64();

    //@}

    /**
     * \name Getters.
     *
     * Methods for returning the various buffer parameters.
     */
    //@{

    /**
     * \brief  Method to get the bunch crossing number.
     * \return unsigned long Bunch crossing number for that event.
     */
    unsigned long getBunchCrossing(void) const;

    /**
     * \brief  Method to get the FED DAQ mode.
     * \return FED DAQ mode.
     *
     * \todo - like setMode it is likely depreciated or should do something else.
     */
    unsigned long getMode(void) const;

    /**
     * \brief  Method to get the header format in the tracker special header.
     * \return unsigned short Header format, possible values:
     *         \li 1: Full debug mode.
     *         \li 2: APV error mode.
     */
    unsigned short getHeaderFormat(void) const;

    //<RB>
    //Added tracker event type method.
    /**
     * \brief  Returns the FED readout mode.
     * \return unsigned short The "Trkr Evt_Ty" field of the Tracker-specific header, possible values:
     *         \li 1 Scope mode real.
     *         \li 2 Virgin raw real.
     *         \li 3 Virgin raw fake.
     *         \li 6 Processed raw real.
     *         \li 7 Processed raw fake.
     *         \li 10 Zero suppressed real.
     *         \li 11 Zero suppressed fake.
     *         \li 12 Zero suppressed lite real.
     *         \li 13 Zero suppressed lite fake.
     *
     * More of the event types can be found in the FED FE unit documentation.
     */
    unsigned short trackerEventType(void) const;
    //</RB>
    
    /**
     * \brief  Method to get the buffer data vector.
     * \return vector<unsigned short> A vector containing one element for each strip on a FED, each element contains the value on that strip. Size:24,576.
     */
    std::vector<unsigned short> getFed9UBufferData(void) const;

    /**
     * \brief  Method to get the channel length vector.
     * \return vector<unsigned short> A vector containing one element for each channel on the FED, with each element contains the length of the channel. Size:12.
     */
    std::vector<unsigned short> getChannelLengths(void) const;

    /**
     * \brief  Method to get the FE length vector.
     * \return vector<unsigned short> Returns a vector containing the size of the data for each FE unit. One element per FE unit on a FED. Size:8.
     */
    std::vector<unsigned short> getFeLengths(void) const;

    /**
     * \brief  Method to get the packet code.
     * \return unsigned short The packet code.
     *
     * \todo - figure out a bit more about this.
     */
    unsigned short getPacketCode(void) const;

    /**
     * \brief  Method to get the common mode medians vector.
     * \return vector<unsigned short> Returns a vector which contains the common mode median value calculated for each APV on a FED. Size:192.
     */
    std::vector<unsigned short> getCommonModeMedians(void) const;

    //@}

  private:

    //static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 12406; //!< Max buffer size for raw mode (in units of ints).
    static const unsigned short INTS_IN_RAW_FED9U_BUFFER = 49002; //!< Max buffer size for scope mode, in units of 32 bit words.

    unsigned int mBuf[INTS_IN_RAW_FED9U_BUFFER]; //!< Array to hold the full event data buffer.

    std::vector<unsigned short> mFed9UBufferData; //!< Vector to hold all the FED strip data in the buffer.
    std::vector<unsigned short> mFeLengths;       //!< Vector to hold the 32 bit word length of each FED FE unit.
    std::vector<unsigned short> mChannelLengths;  //!< Vector to hold the 32 bit word length of each FED channel.

    unsigned int mIndex;          //!< Current position of the internal buffer pointer.
    unsigned long mBunchCrossing; //!< Bunch crossing number of the event data.
    unsigned long mMode;          //!< DAQ mode of the event. /todo - Possibily Evt_ty and possibly depreciate or does something else.

    unsigned short mPacketCode;        //!< Packet code. \todo - don't know what this is.
    unsigned short mHeaderFormat;      //!< Header format, see setHeaderFormat or getHeaderFormat for possible values.
    unsigned short trackerEventType_;  //!< Value of "Trkr Evt_Ty" field of Tracker-specific header. See setTrackerHeader or getTrackerHeader for possible values. 
    std::vector<unsigned short> mCMMedians; //!< Vector to hold each of the calculated common mode median values for the 192 APVs on a FED.
    
    Fed9UEventInfo mFed9UEventInfo; //!< See Fed9UEventInfo for more details on this class.
  };

}

#endif //FED9UBUFFERDESCRIPTION_H
