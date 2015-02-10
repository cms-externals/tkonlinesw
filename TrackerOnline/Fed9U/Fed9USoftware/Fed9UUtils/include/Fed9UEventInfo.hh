#ifndef FED9UEVENTINFO_H
#define FED9UEVENTINFO_H

namespace Fed9U {
  
  /**
   * \brief  Class which provides access to some useful FED specific numbers for the SiStripDataFormat classes.
   * \author Matthew Pearson, RAL
   * \date   07/2004
   */
  class Fed9UEventInfo {

  public:
    /**
     * \brief Virtual destructor.
     */
    virtual ~Fed9UEventInfo() {}

    /**
     * \brief  Method to get the number of FED front end units.
     * \return unsigned short.
     */
    unsigned short getFeUnitsPerFed(void) const {
      return FEUNITS_PER_FED;
    }

    /**
     * \brief  Method to get the number of channels per FED front end unit.
     * \return unsigned short.
     */
    unsigned short getChannelsPerFeUnit(void) const {
      return CHANNELS_PER_FEUNIT;
    }

    /**
     * \brief  Method to get the number of APVs per FED channel.
     * \return unsigned short.
     */
    unsigned short getApvsPerFedChannel(void) const {
      return APVS_PER_CHANNEL;
    }

    /**
     * \brief  Method to get the number of Si strips per APV in the Si tracker.
     * \return unsigned short.
     */
    unsigned short getStripsPerApv(void) const {
      return STRIPS_PER_APV;
    }

    /**
     * \brief  Method to get the size in bytes of the FED front end header in the FED data buffer.
     * \return unsigned short.
     */
    unsigned short getFeUnitHeaderSize(void) const {
      return TRACKER_FEUNIT_HEADER_SIZE;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U buffer.
     * \return unsigned long.
     */
    unsigned long getBytesInRawFed9UBuffer(void) const {
      return BYTES_IN_RAW_FED9U_BUFFER;
    }

    /**
     * \brief  Method to get the number of bytes to the start of the FE headers.
     * \return unsigned short.
     */
    unsigned short getBytesToStartOfFeHeaders(void) {
      return BYTES_TO_START_OF_FE_HEADERS;
    }

    /**
     * \brief  Method to get the number of bytes to the start of channel data.
     * \return unsigned short.
     */
    unsigned short getBytesToStartOfChannels(void) {
      return BYTES_TO_START_OF_CHANNELS;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U FE data block (including channel headers and packet codes).
     * \return unsigned short.
     */
    unsigned short getBytesInRawFed9UFE(void) {
      return BYTES_IN_RAW_FED9U_FE;
    }

    /**
     * \brief  Method to get the number of bytes in a raw Fed9U channel (incluidng header and packet code).
     * \return unsigned short.
     */
    unsigned short getBytesInRawFed9UChannel(void) {
      return BYTES_IN_RAW_FED9U_CHANNEL;
    }

    /**
     * \brief  Method to get the number of bytes in the DAQ trailer.
     * \return unsigned short.
     */
    unsigned short getBytesInDaqTrailer(void) {
      return BYTES_IN_DAQ_TRAILER;
    }

    /**
     * \brief  Method to get the code for FED scope mode.
     * \return unsigned short.
     */
    unsigned short getScopeModeCode(void) const {
      return FED9U_SCOPE;
    }

    /**
     * \brief  Method to get the code for FED frame finding mode.
     * \return unsigned short.
     */
    unsigned short getFrameFindingModeCode(void) const {
      return FED9U_FRAME;
    }

    /**
     * \brief  Method to get the code for FED virgin raw mode.
     * \return unsigned short.
     */
    unsigned short getVirginRawModeCode(void) const {
      return FED9U_VIRGRAW;
    }

    /**
     * \brief  Method to get the code for FED zero suppressed mode.
     * \return unsigned short.
     */
    unsigned short getZeroSuppressedModeCode(void) const {
      return FED9U_ZEROSUPP;
    }

    /**
     * \brief  Method to get the code for FED processed raw mode.
     * \return unsigned short.
     */
    unsigned short getProcessedRawModeCode(void) const {
      return FED9U_PROCRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for scope mode.
     * \return unsigned short.
     */
    unsigned short getScopeDaqModeCode(void) const {
      return FED9U_DAQ_SCOPE;
    }

    /**
     * \brief  Method to get the DAQ mode number for virgin raw mode.
     * \return unsigned short.
     */
    unsigned short getVirginRawDaqModeCode(void) const {
      return FED9U_DAQ_VIRGRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for processed raw mode.
     * \return unsigned short.
     */
    unsigned short getProcessedRawDaqModeCode(void) const {
      return FED9U_DAQ_PROCRAW;
    }

    /**
     * \brief  Method to get the DAQ mode number for zero suppressed mode.
     * \return unsigned short.
     */
    unsigned short getZeroSuppressedDaqModeCode(void) const {
      return FED9U_DAQ_ZEROSUPP;
    }

    /**
     * \brief  Method to get the channel status bits size.
     * \return unsigned short.
     */
    unsigned short getChannelStatusSize(void) const {
      return CHANNEL_STATUS_SIZE;
    }

    /**
     * \brief  Methgod to get the size of the frame sync out packet.
     * \return unsigned short.
     */
    unsigned short getSizeOfFrameSyncOutPacket(void) {
      return SIZE_OF_FRAME_SYNC_OUT_PACKET;
    }

    /**
     * \brief  Method to get the tracker special header full debug mode code.
     * \return unsigned short.
     */
    unsigned short getTrackerSpecialHeaderFullDebugMode(void) {
      return TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE;
    }

    /**
     * \brief  Method to get the tracker special header apv error mode code.
     * \return unsigned short.
     */
    unsigned short getTrackerSpecialHeaderApvErrorMode(void) {
      return TRACKER_SPECIAL_HEADER_APV_ERROR_MODE;
    }

  private:
    static const unsigned short FEUNITS_PER_FED = 8;      //!< Number of FE units on a FED.
    static const unsigned short CHANNELS_PER_FEUNIT = 12; //!< Number of channels on a FE unit.
    static const unsigned short APVS_PER_CHANNEL = 2;     //!< Number of APVs per channel.
    static const unsigned short STRIPS_PER_APV = 128;     //!< Number of strips per APV.

    static const unsigned short TRACKER_FEUNIT_HEADER_SIZE = 16;     //!< Number of bytes in the FE unit header.
    static const unsigned long BYTES_IN_RAW_FED9U_BUFFER = 49624;    //!< Number of bytes in a virgin/processed raw event buffer (including inter-FE padding).
    static const unsigned short BYTES_TO_START_OF_FE_HEADERS = 16;   //!< Number of bytes before the start of the FE headers.
    static const unsigned short BYTES_TO_START_OF_CHANNELS = 144;    //!< Number of bytes before the start of the channel data.
    static const unsigned short BYTES_IN_RAW_FED9U_FE = 0x1824;      //!< Number of bytes in the FE units in virgin/processed raw modes.
    static const unsigned short BYTES_IN_RAW_FED9U_CHANNEL = 0x0203; //!< Number of bytes in the channels in virgin/processed raw modes.
    static const unsigned short BYTES_IN_DAQ_TRAILER = 8;            //!< Number of bytes in the DAQ trailer.

    /**
     * \name Packet codes.
     *
     * These are the packet codes (without the 3 MSB).
     */
    //@{
    static const unsigned short FED9U_SCOPE = 1;
    static const unsigned short FED9U_FRAME = 2;
    static const unsigned short FED9U_VIRGRAW = 4;
    static const unsigned short FED9U_ZEROSUPP = 8;
    static const unsigned short FED9U_PROCRAW = 16;
    //@}

    /**
     * \name DAQ event types.
     *
     * These are the DAQ event types.
     */
    //@{
    static const unsigned short FED9U_DAQ_SCOPE = 1;
    static const unsigned short FED9U_DAQ_VIRGRAW = 2;
    static const unsigned short FED9U_DAQ_PROCRAW = 6;
    static const unsigned short FED9U_DAQ_ZEROSUPP = 10;
    //@}

    /**
     * \name Frame sync info.
     *
     * Information about the frame syncronisation out packet.
     */
    //@{
    static const unsigned short CHANNEL_STATUS_SIZE = 6;
    static const unsigned short SIZE_OF_FRAME_SYNC_OUT_PACKET = 81;
    //@}

    /**
     * \name Header formats.
     *
     * Full debug or APV error.
     */
    //@{
    static const unsigned short TRACKER_SPECIAL_HEADER_FULL_DEBUG_MODE = 1;
    static const unsigned short TRACKER_SPECIAL_HEADER_APV_ERROR_MODE = 2;
    //@}

  };
 
} //End of Fed9U Namespace

#endif //FED9UEVENTINFO_H
