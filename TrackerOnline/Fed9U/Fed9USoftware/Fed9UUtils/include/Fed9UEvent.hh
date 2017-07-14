#ifndef H_Fed9UEvent
#define H_Fed9UEvent

#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#include "Fed9UAddress.hh"
//#include "StopWatch.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  //  using namespace::HAL;
  
  class Fed9UEvent;

  /**
   * \brief An enumeration whose values are used to decode the packet code in the event.
   *
   * The packet code represents the data acquision (DAQ) mode that the event was taken in. Possible values are:
   * \li FED9U_PACKET_SCOPE Taken in scope mode.
   * \li FED9U_PACKET_VIRGRAW Taken in virgin raw mode.
   * \li FED9U_PACKET_ZEROSUPP Taken in zero suppressed mode.
   * \li FED9U_PACKET_PROCRAW Taken in processed raw data mode.
   * \li FED9U_PACKET_VIRGRAW_10BIT packed in 10 bit, valid for both ZS and VR only
   * \li FED9U_PACKET_ZEROSUPP_10BIT packed in 10 bit, valid for both ZS and VR only
   * \li FED9U_PACKET_ZEROSUPP_LO Valid for ZS only, here we striped the lowest 2 bits of the 10 bit data to pack into 8 bits
   * \li FED9U_PACKET_ZERO_SUPP_HI_LO Valid for ZS only, here we stripped 1 upper and 1 lower bit to pack 10 into 8 bits. 
   * A. Baty: Added VR bit stripped versions (7/7/2015)
   
   * More details of the different types of DAQ mode can be found in the CMS FED FE FPGA documentation.
   */
  enum { FED9U_PACKET_SCOPE = 0xE1, FED9U_PACKET_VIRGRAW = 0xE6, FED9U_PACKET_VIRGRAW_10BIT = 0x86,
         FED9U_PACKET_ZEROSUPP = 0xEA, FED9U_PACKET_PROCRAW = 0xF2, FED9U_PACKET_ZEROSUPP_10BIT = 0x8A,
	     FED9U_PACKET_ZEROSUPP_LO = 0xCA, FED9U_PACKET_ZEROSUPP_HI_LO = 0xAA,
//Added for bit stripped moded (AAB 8/24/2015) 
             FED9U_PACKET_PROCRAW_10BIT = 0x92,
             FED9U_PACKET_PROCRAW_8BIT_HI_LO = 0xB2,
             FED9U_PACKET_PROCRAW_8BIT_LO = 0xD2,
             FED9U_PACKET_VIRGRAW_8BIT_HI_LO = 0xA6,
             FED9U_PACKET_VIRGRAW_8BIT_LO = 0xC6
  };
  
  /** 
   * \brief Fed9UEventIterator class is designed to be used within the Fed9UEvent Class.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   *
   * This class can be used to iterate through the Fed9UEvent buffer to exam the various bytes in the buffer.
   */
  class Fed9UEventIterator {
  public:

    /**
     * \brief Default Constructor.
     * \param buffer Point to the character buffer that the event is stored in. Defaulted to NULL.
     * \param length Length of the buffer that is given during construction. Defaulted to zero.
     * \param offset An offset into the data buffer to initially place the pointer.
     */
    Fed9UEventIterator(u8 * buffer = 0, u32 length = 0, u32 offset = 0 )
      : _buf(buffer), _len(length), _off(offset) { }

    /**
     * \brief  Used to set the iterator to a new event buffer.
     * \param  buffer Point to the character buffer that the event is stored in. Defaulted to NULL.
     * \param  length Length of the buffer that is given during construction. Defaulted to zero.
     * \param  offset An offset into the data buffer to initially place the pointer.
     * \return Self reference.
     */
    Fed9UEventIterator & set(u8 * buffer = 0, u32 length = 0, u32 offset = 0) {
      _buf = buffer; _len = length; _off = offset; return *this;
    }

    /**
     * \brief  Sets a new size of the event buffer.
     * \param  Length The new length of the buffer.
     * \return Self reference.
     */
    Fed9UEventIterator & resize(u32 length) { (*this) = normalize();  _len = length; return *this; }

    /**
     * \brief Deletes the event buffer.
     */
    void clear() {  if (_buf != NULL ) delete[] _buf; }

    /**
     * \brief  Used to determine the size of the event buffer.
     * \return unsigned long The event size.
     */
    u32 size() const { return _len; }

    /**
     * \brief Moves the buffer pointer along to the next 32-bit word boundary
     *        within the buffer, ensuring that the internal offset is adjusted accordingly
     */
    Fed9UEventIterator normalize() const {
      u32 diff = _off & ~3;
      return Fed9UEventIterator(_buf + diff, _len, _off - diff);
    }
    
    /**
     * \brief  Get a pointer to the byte this is pointing to
     * \return constant unsigned char pointer
     */
    const u8* getPtr() const {
      return &(_buf[0]);
    }

    /**
     * \brief  Used to get a byte from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the byte.
     * \return unsigned char Byte that is retrieved from this offset location.
     */
    u8 getu8(u32 offset) const {
      return (*this)[offset];
    }

    /**
     * \brief  Used to get a 16-bit word of data from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the word.
     * \param  swap If true then the word will be byte swapped, if false it will not. Defaulted to false.
     * \return unsigned short Word that is retrieved from this offset location.
     */
    u16 getu16(u32 offset, bool swap = false) const {
      if (swap) return ((*this)[offset] << 8) |  (*this)[offset+1];
      else      return  (*this)[offset]       | ((*this)[offset+1] << 8);
    }

    /**
     * \brief  Used to get a 32-bit word of data from a specific location in the event buffer.
     * \param  offset Offset into the event buffer from which to retrieve the word.
     * \param  swap If true then the word will be byte swapped, if false it will not. Defaulted to false.
     * \return unsigned long Word that is retrieved from this offset location.
     */
    u32 getu32(u32 offset, bool swap = false) const {
      if (swap) return (getu16(offset, swap) << 16) |  getu16(offset+2, swap);
      else      return  getu16(offset, swap)        | (getu16(offset+2, swap) << 16);
    }

    /**
     * \brief  Used to deference the iterator and access the data at its current location.
     * \return unsigned char The byte that is at the current location.
     */
    const u8 & operator * () const { return (*this)[0]; }

    /**
     * \brief  Provides similar functionality to getu8, returning a byte at a given location.
     * \param  offset Offset into the buffer from which to read the byte.
     * \return unsigned char Constant reference to the byte at the offset location.
     */
    const u8 & operator [] (u32 offset) const {
      ICUTILS_VERIFY(offset < _len)(offset)(_len).error();
      return _buf[(_off + offset) ^ 3];
    }

    /**
     * \brief  Subtracts the values of the bytes of two different Fed9UEventIterators at their current location in the buffer.
     * \param  lhs Left hand side Fed9UEventIterator, rhs is subtracted from this objects byte.
     * \param  rhs Right hand side Fed9UEventIterator, this byte is subtracted from the lhs.
     * \return signed long Result of the subtraction.
     */
    friend i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs);

    //Fed9UEventIterator & operator ++ () { ++_off; return *this; }
    //const Fed9UEventIterator & operator ++ const () { ++_off; return *this; }
    //Fed9UEventIterator operator ++ (int) {
    //  Fed9UIterator temp(*this);
    //  _off++;
    //  return temp;
    //}

    /**
     * \brief  Used to increment the iterator onto the next byte.
     * \param  int Not used just part of the operator definition syntacs.
     * \return Fed9UEventIterator A copy of the Fed9UEventIterator object that has been incremented.
     */
    Fed9UEventIterator operator ++ (int) {
      Fed9UEventIterator temp(*this);
      (*this) += 1;
      return temp;
    }

    //Fed9UEventIterator & operator -- () { --_off; return *this; }
    //const Fed9UEventIterator & operator -- const () { --_off; return *this; }
    //Fed9UEventIterator operator -- (int) {
    //  Fed9UEventIterator temp(*this);
    //  _off--;
    //  return temp;
    //}
    //const Fed9UEventIterator operator -- const (int) {
    //  Fed9UEventIterator temp(*this);
    //  _off--;
    //  return temp;
    //}

    /**
     * \brief  Moves the iterator on by given amount.
     * \param  rhs Relative amount by which to increment the iterator offset by.
     * \return Fed9UEventIterator Reference to the original Fed9UEventIterator once it has been incremented.
     */
    Fed9UEventIterator & operator += (u32 rhs) {
      ICUTILS_VERIFY(rhs <= _len)(rhs)(_len).error();
      _off += rhs;
      _len -= rhs;
      return *this;
    }

    /**
     * \brief  Adds a given amount to the current offset poistion of a Fed9UEventIterator.
     * \param  lhs Fed9UEventIterator whose offset into the buffer is to move.
     * \param  rhs Amount that is to be added to the offset.
     */
    friend Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, u32 rhs);

    /**
     * \brief Prints the start address of the buffer, offset and length.
     */
    void debug(std::ostream & os) const {
      os << std::dec << reinterpret_cast<void*>(_buf) << '+' << _off << ':' << _len;
    }

  private:

    u8 * _buf;          //!< Pointer to the start of the buffer.
    u32 _len, _off;     //!< Legnth (_len) of current buffer and offset (_off) into the buffer.
    bool _deleteBuffer; //!< True if the buffer it to be delete. \todo - check.
  };

  inline i32 operator - (const Fed9UEventIterator & lhs, const Fed9UEventIterator & rhs) {
    return (lhs._buf + lhs._off) - (rhs._buf + rhs._off);
  }
  
  inline Fed9UEventIterator operator + (const Fed9UEventIterator & lhs, int rhs) {
    Fed9UEventIterator temp(lhs); return temp += rhs;
  }

  /**
   * \brief Stores the channel data for the event.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   *
   * Provides access to all the settings that are stored in a given channel in the event.
   */
  class Fed9UEventChannel {
  public:

    /**
     * \brief Constructor.
     * \param data A Fed9UEventIterator pointing to the start of that channel.
     * \param status APV status for that channel. Each channel contains the status of the APVs and whether the ticker was able to lock to
     *        both APVs, if it was out of synch or an error was sent from the APV. getStatus contains more information.
     */
    Fed9UEventChannel(Fed9UEventIterator data, u8 status);

    /*
    Fed9UDaqMode mode() const;
    int operator[] (size_t i) const {
      ICUTILS_VERIFY((i*2+1) < _data.size())(i*2+1)(_data.size())
        .msg("Index out of bounds").error();
      return _data[i*2] | _data[i*2+1] << 8;
    }
    */

    /**
     * \brief  Used to get the status for the channel APVs.
     * \return unsigned char The six status bits, which are as follows:
     *         \li 0: APV0 error - the ticker received an error from the APV1 chips.
     *         \li 1: wrong header APV0 - the header found for this APV is wrong, according to the winner.
     *         \li 2: APV1 error - the ticker received an error from the APV1 chips.
     *         \li 3: wrong header APV1 - the header found for this APV is wrong, according to the winner.
     *         \li 4: out of sync - the ticker has locked for the APVs on this channel, but it is out of sync with the event pulse
     *                from the rest of the APV on a FE unit. 
     *         \li 5: lock - the ticker has locked onto something.
     * The polarities of the bits are set such that 0x3f (all 1's) indicates a fully working channel.
     */
    u8  getStatus() const { return _status; }

    /**
     * \brief Returns the packet code for the event that was recorded.
     *
     * This can be decoded using the anonamous enum declared in this file.
     */
    u8  getPacketCode() const { return _data[2]; }

    /**
     * \brief  Returns the median value recorded during a zero suppressed run for that APV.
     * \param  apv The channel APV that the median should be retrieved for. Range:0-1.
     * \return unsigned short Median value for that APV.
     *
     * Only valid for zero suppressed runs.
     */
    u16 getMedian(unsigned apv) const {
      ICUTILS_VERIFY(apv <= 1)(apv).error();
      ICUTILS_VERIFY(
						getPacketCode() == FED9U_PACKET_ZEROSUPP ||
					    getPacketCode() == FED9U_PACKET_ZEROSUPP_10BIT ||
						getPacketCode() == FED9U_PACKET_ZEROSUPP_LO ||
						getPacketCode() == FED9U_PACKET_ZEROSUPP_HI_LO
						)(getPacketCode()).error();
      return _data.getu16(3 + apv*2);
    }

    /**
     * \brief  Length of the data on that channel.
     * \return unsigned long Data length on that channel, in bytes.
     */
    u32 dataLength() const { return _data.getu16(0); }

    /**
     * \brief  Returns the number of ADC samples recorded for that channel.
     * \return unsigned long Number of samples recorded.
     */
    u16 samples() const;

    /**
     * \brief  Returns the value of a specific sample.
     * \param  i Sample number to be returned.
     * \return unsigned short Number of ADC counts recorded in that sample.
     */
    u16 sample(std::size_t i) const;

    /**
     * \brief Returns all the samples.
     * \param destBuff Pointer to a unsigned short buffer that will contain all the sample values. The buffer mst be as large as the
     *        value from Fed9UEventChannel::samples().
     */
    void getSamples(u16 * destBuff) const;

    /**
     * \brief  Returns all the samples.
     * \return std::vector<unsigned short> A std::vector containing all the ADC counts for each recorded strip. Vector should be as large as
     *         the value from Fed9UEventChannel::samples().
     */
    std::vector<u16> getSamples() const;

    //<RB date=22/07/05>
    //Added method to get the channel iterator.
    /**
     * \brief  Returns channel interator
     * \return Fed9UEventIterator Constant reference to the iterator that points to this channel.
     */
    inline const Fed9UEventIterator& getIterator() const {return _data;}
    //</RB>

  private:
    u8 _status;               //!< APV status bits for that channel.
    Fed9UEventIterator _data; //!< Fed9UEventIterator that is used to iterate through the channel.
  };

  /**
   * \brief  Breaks the event into header and FE unit data sections, allowing access to each of the FE units in the event.
   * \author Jonathan Fulcher
   * \author Emlyn Corrin
   * \author Matthew Pearson
   * \version 1
   * \date 15 March 2005
   */
  class Fed9UEventUnit {
  public:
    /**
     * \brief Constructor.
     * \param header Fed9UEventIterator that points to the start of the event header for that FE unit.
     * \param data Fed9UeventIterator that points to the start of the event data for that FE unit.
     * \param headerformat A Fed9UHeaderFormat object that defines the structure of the event header.
     */
    Fed9UEventUnit(Fed9UEventIterator header, Fed9UEventIterator data, Fed9UHeaderFormat headerformat);

    /**
     * \brief  Gives access to one of the channels stored in the event.
     * \param  i FED channel number. Number of channel can be found in Fed9UEventUnit::channels()
     * \return Fed9UEventChannel Constant reference to the requested channel.
     */
    const Fed9UEventChannel & channel(std::size_t i) const ;

    /**
     * \brief  Returns the majoritory pipeline from the ticker voting for that FE unit.
     * \return unsigned char Majoritory pipeline.
     *
     * The majoritory pipeline is the value that after voting on the APV headers was determined. All valid APV signals will
     * will agree with this value. Only valid in full debug mode.
     *
     * \todo DONE! - check that this is correct . <JRF date="27/06/2008"> this was wrong... I have changed the byte which is read, it is now correct.
     */
    u8  getMajorityPipeline() const { 
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("getMajorityPipeline only valid in full debug mode");
      return _header.getu8(14); 
    }

    /**
     * \brief  Total length of all the data recorded is stored within the event data.
     * \return unsigned long Length of the data recorded.
     */
    u32 dataLength() const { 
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("dataLength only valid in full debug mode");
      return (_data.size()+7) & ~7; 
    }

    /**
     * \brief  Returns the number of channels that were found in the event.
     * \return unsigned short Number of channels in the event.
     */
    u16 channels() const { return _channels.size(); }

    /**
     * \brief  Read the middle 32-bit word in the 2nd 64-bit word of the FE header.
     * \return unsigned long Returns the bytes 10-13 inclusive. It can be used to read the DAQ register for example, which is
     *  in this position in the 8th FE header in a full debug mode event.
     */
    u32 getBytes10To13FromFeHeader(void) const {
      ICUTILS_VERIFY(_headerFormat == FED9U_HEADER_FULLDEBUG)(_headerFormat).error().msg("getBytes10To14FromFeHeader only valid in full debug mode");
      return (_header.getu32(10, true) & 0xFFFFFFFF); 
    }
  private:

    Fed9UEventIterator _header, _data;   //!< Fed9UEventIterators to store the header and data iterators.
    Fed9UHeaderFormat _headerFormat;     //!< Data member for store the Fed9UHeaderFormat object, which defines structure of the header.
    std::vector<Fed9UEventChannel> _channels; //!< Vector to hold all of the channels that are recorded in the event.
  };

  /**
   * \class   Fed9UEvent
   * \brief   Class to read in and check the event buffer for errors.
   * \author  Jonathan Fulcher
   * \author  Emlyn Corrin
   * \version 0.0
   * \date    21/08/2003
   *
   * Breaks the event into its component parts providing access to the header, trailer, data (divided into channels) using the
   * supporting classes Fed9UEventUnit, Fed9UEventChannel and Fed9UEventIterator.
   */
  class Fed9UEvent {

  public:

    /**
     * \brief Enables or disables the debug print out for the member functions in this class or Fed9UEventChannel, Unit and Iterator.
     * \param on If true then the debug output is enabled, false disabled.
     */
    static void EnableDebug(bool on = true);

    /**
     * \brief Default constructor.
     *
     * Creates an empty event.
     */
    Fed9UEvent() : _buffer(0), _rubbish(0), _fedDescription(0), _deleteBuffer(false), _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_OLD_VME) {}
    
    /**
     * \brief Constructor.
     * \param is An input stream from which to create the event.
     *
     * Loads an event from an input stream, most typcially one that has been previously saved to file.
     */
    Fed9UEvent(std::istream & is);

    /**
     * \brief Constructor.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in. Defaulted to 0.
     *
     * Creates a Fed9Uevent from a live buffer, stored in a unsigned long array. It requires the current description that the
     * is being run with. The size of the buffer is optional, but desired.
     */
    Fed9UEvent(u32 * buffer, const Fed9UDescription * currentDescription,
               u32 bufsize = 0) :  _buffer(0),_rubbish(0), _fedDescription(0), _deleteBuffer(false), _mode(FED9U_EVENT_MODE_REAL), _eventFormat(FED9U_EVENT_FORMAT_OLD_VME) {
      Init(buffer, currentDescription, bufsize);
    }

    /**
     * \brief Initialises the Fed9UEvent buffer.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in.
     *
     * Splits up the event creating local copies of the header, data which is split into channels, initialising the iterators.
     */
    void Init(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);

    /**
     * \brief Initialises the Fed9UEvent buffer, performing a CRC check on the event data.
     * \param buffer Pointer to the buffer where the event is stored in a unsigned long array.
     * \param currentDescription Fed9UDescription object that contains the settings the event was ran with.
     * \param bufsize Size of the buffer that is being read in.
     *
     * Provides the same functionality as Init, however it also performs a CRC check as well.
     */
    void DebugInit(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize, std::string& report, u32& readCrc, u32& calcCrc);

    /**
     * \brief Destructor.
     *
     * Deletes the event buffer.
     */
    ~Fed9UEvent();

    /**
     * \brief Checks the event size matches that stated in the event, also performs a CRC check is performed
     */
    void checkEvent() const;
    //void debugCRC() const;

    /**
     * \brief Checks all the status bits (see Fed9UEventChannel::status) on each channel to ensure there was no timing errors
     *        with the APVs.
     * \throw Fed9UEventExpection This exception is thrown, containing a detail error string, if there are any problems with the status bits.
     */
    void checkChannelStatuses() const;

    /**
     * \brief  The data acquisions (DAQ) mode is stored in the event buffer.
     * \return Fed9UDaqMode Returns the DAQ mode saved in the event buffer, in the form of a Fed9UDaqMode enum.
     */
    Fed9UDaqMode getDaqMode() const;
    
    /**
     * \brief  The data acquisions (DAQ) Super mode is stored in the event buffer.
     * \return Fed9UDaqSuperMode Returns the DAQ Super mode saved in the event buffer, in the form of a Fed9UDaqSuperMode enum.
     */
    Fed9UDaqSuperMode getDaqSuperMode() const;
  
    /**
     * \brief  Number of events that have occured since the last reset of the counter.
     * \return unsigned long Event number.
     */
    u32 getEventNumber() const;

    //<JRF date="28/11/2007"> 
    /**
     *
     *the event Type is taken from the 4 bits in the header of the event data the modes are labelled by the follwing decimal values:
     *1 = Scope Mode REAL
     *2 = Virgin Raw Mode REAL
     *3 = Virgin Raw Mode FAKE
     *6 = Processed Raw Mode REAL
     *7 = Processed Raw Mode FAKE
     *8 = Spy Mode REAL
     *10 = Zero Suppressed Mode REAL
     *11 = Zero Suppressed Mode FAKE
     *12 = Zero Suppressed Lite Mode REAL
     *13 = Zero Suppressed Lite Mode FAKE\todo - find out more information about this.
     */
    u8  getEventType() const;

    /**
     * \brief  The bunch crossing number that the event was saved during is recorded in the event.
     * \return unsigned short Returns the bunch crossing number.
     */
    u16 getBunchCrossing() const;

    /**
     * \todo - find out more information about this.
     */
    u16 getSourceId() const;

    /**
     * \todo - find out more information about this.
     */
    u8  getFormatVersion() const;

    /**
     * \brief  Data mode describes the basic type of data you have here, fake, real or legacy data.
     * \return Fed9UEventDataMode An enum that defines the possible types of data that can be processed by the FED.
     *
     * Events can either be generated by the optical inputs, real data, or from the FE units, fake data. When loading fake data
     * it is possible it can either be a real event loaded from file creating a full fake event. It is also possible for a fake
     * event to be created from a simple .apv file, which contains data for only an APV pair. In this case the data will be the
     * same across all FED channels and only a single channel will be unique. There is also support for a legacy data mode, which
     * had a different header structure and as such requires a slightly different decoding. This method is used to determine
     * what type/format of data is being viewed.
     */
    Fed9UEventDataMode getDataMode() const { return _mode ; }

    /**
     * \todo - find out more information about this.
     */
    Fed9UHeaderFormat getTrackerHeaderFormat() const { return _headerFormat; }

    /**
     * \brief  Method to set the data mode to legacy in order to interpret old style data from early beam tests in 2004/5.
     * \return Self reference.
     */
    Fed9UEvent& setLegacyDataMode() { _mode = FED9U_EVENT_MODE_LEGACY; return *this; }

    /**
     * \brief  Returns the CRC number from the event trailers.
     * \return unsigned short The CRC number.
     */
    u16 getCrc() const;

    /**
     * \brief  Calculates the CRC number for the event.
     * \return unsigned short Calculated CRC number.
     */
    u16 calcCrc() const;

    /**
     * \brief  Debug version that calcs CRC without other information guesses position of crc based on length provided.
     * \return unsigned short The calculated CRC.
    */
    u16 calcCrcDirty() const;

    /**
     * \brief  Method to read the DAQ register from the 8th FE header in a full debug mode event.
     * \return unsigned long DAQ register present in the event. Returns zero if there is no full debug mode event.
     */
    u32 getDaqRegister(void) const;


    /**
     * \brief Method to read the DAQ register Two from the 7th FE header in a full debug mode event.                                                                                                
     * \return unsigned log DAQ register two present in the event. Returns zero if there is no full debug mode event.
     */
    u32 getDaqRegisterTwo(void) const;


    /**
     * \brief  Method to read the BE status register from the 1st FE header in a full debug mode event. 
     * \return unsigned long Back end status register bits. It returns zero if the event is not a full debug mode event.
     */
    u32 getBeStatusRegister(void) const;

    /**
     * \name Tracker special header components.
     *
     * Methods to read the tracker special header. 
     * These methods rely on the SPECIAL_OFF private data member to locate the 
     * position of the tracker special header in the FED buffer.
     */
    //@{

    /**
     * \brief  Read first byte of tracker special header (should be 0xED while old and new headers co-exist).
     * \return unsigned char Byte from tracker special header.
     *
     * \todo - This method will have to renamed or replaced when these bits are used for something else.
     */
    u8 getSpecialFirstByte(void) const;

    /**
     * \brief  Read the header format (4 bits) from the tracker special header.
     * \return Fed9UHeaderFormat Returns the value of the header format. See the Fed9UHeaderFormat documentation for more details.
     */
    Fed9UHeaderFormat getSpecialHeaderFormat(void) const;

    /**
     * \brief  Read the tracker event type (4 bits) from the tracker special header.
     * \return unsigned char Event type, whose values are defined below.
     *
     * This is the 'FED mode', the possible values are:
     * \li 1:Scope mode.
     * \li 2:Virgin raw mode.
     * \li 6:Processed raw mode.
     * \li 10:Zero suppressed mode.
     * \li 12:Zero suppressed lite mode.
     * \li 8:Spy mode.
     */
    u8 getSpecialTrackerEventType(void) const;

    /**
     * \brief  Read the APVE Address (8-bits) from the tracker special header.
     * \return unsigned char APVE address.
     *
     * This is the APVE address sent to the FED over the TTC channel B. It should
     * match the real address in the APV headers.
     */
    u8 getSpecialApvEmulatorAddress(void) const;
      
    /**
     * \brief  Read the APV address error (8-bits) from the tracker special header.
     * \return unsigned char APV address error.
     *
     * Each bit returned by this method indicates if the APV address for that FE FPGA
     * is the same as the APVE address. A 1 means they are identical, and a 0 means they are not.
     * Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.
     */
    u8 getSpecialApvAddressError(void) const;

    /**
     * \brief  Read the FE enable register (8-bits) from the tracker special header.
     * \return unsigned char A 1 means the FE unit was enabled, 0 disabled.
     *
     * Each bit returned by this method indicates if the corresponding FE FPGA is
     * enabled (able to accept data). This information is needed to decode the FED
     * buffer when operating in APV error header mode. If set to 0 then there will
     * be no payload data from that particular FE FPGA. Bit 0 corresponds to FE FPGA
     * 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.
     */
    u8 getSpecialFeEnableReg(void) const;

    /**
     * \brief  Read the FE overflow register (8-bits) from the tracker special header.
     * \return unsigned char Value of the overflow register.
     *
     * If a bit is set to 1 then a buffer overflow has occured in the corresponding 
     * FE FPGA, and the data from that FPGA should be ignored.
     * Bit 0 corresponds to FE FPGA 1 (external numbering), bit 1 corresponds to FE FPGA 2, etc.*/
    u8 getSpecialFeOverflowReg(void) const;

    /**
     * \brief  Read the FED status register (16-bits) from the tracker special header.
     * \return unsigned short Value of the status register contained in the special header.
     * Each bit returned by this method indicates the state of a particular part of the FED.
     * \li Bit 0 = Slink full flag.
     * \li Bit 1 = Tracker Header Monitor data ready.
     * \li Bit 2 = QDR Memory full flag.
     * \li Bit 3 = QDR Memory partial full flag.
     * \li Bit 4 = QDR Memory empty flag.
     * \li Bit 5 = L1A FIFO Full flag.
     * \li Bit 6 = L1A FIFO Partial Full flag.
     * \li Bit 7 = L1A FIFO Empty flag.*/
    u16 getSpecialFedStatusRegister(void) const;
    //@}
      
      //Fed9UEventChannel & getChannel(Fed9UAddress);
      //Fed9UEventChannel & getChannel(int fedFeUnit, int feUnitChannel);

    /**
     * \brief Used to write the event data to a stream in a format the scientific display program Igor can understand.
     * \param o Output stream used to write the data to.
     */
    void saveIgorFile(std::ostream & o) const;

    /**
     * \brief Writes the contents of the event buffer to an output stream in a human readable format.
     * \param ostream Reference to the output stream the data is to be written to.
     */
    void dump(std::ostream&) const;

    /**
     * \brief Writes the contents of the event buffer to an output stream in the format as it was received from the FED.
     * \param os Output stream to write the raw data to.
     *
     * The raw data will be arranged into words that are 64-bits per line. 64-bit words is what the FED sends the data as.
     */
    void dumpRawBuffer(std::ostream& os) const;
    const Fed9UEventUnit & feUnit(std::size_t i) const {
      ICUTILS_VERIFY(i < feUnits())(i)(feUnits()).msg("Index out of bounds").error();
      return _feunits[i];
    }

    /**
     * \brief  Gives the Fed9UEventChannel object for a given FE unit channel.
     * \param  unit FE unit to be addressed.
     * \param  channel FE unit channel to be addressed.
     * \return Fed9UEventChannel Constant reference to the specified Fed9UEventChannel object.
     */
    const Fed9UEventChannel & channel(std::size_t unit, std::size_t channel) const;

    /**
     * \brief  Returns the total length of the buffer.
     * \return unsigned long Total length of the buffer.
     */
    u32 getTotalLength() const;

    /**
     * \brief  Returns the number of FE units that are recorded in the event.
     * \return unsigned short Number of FE units.
     */
    u16 feUnits() const { return _feunits.size(); }

    /**
     * \brief  Returns the total number of channels that are recorded in the event.
     * \return unsigned short Number of channels.
     */
    u16 totalChannels() const {
      u16 ret = 0;
      for (int i = 0; i < feUnits(); i++) {
        ret += feUnit(i).channels();
      }
      return ret;
    }

    /**
     * \brief  Returns the appropriate event data for a given FED channel.
     * \param  fedChannel FED channel number to be returned.
     * \return Fed9UEventChannel Constant reference to the event channel that was requested.
     */
    const Fed9UEventChannel & channel(u8 fedchannel) const {
      for (int i = 0; i < feUnits(); i++) {
        if (fedchannel < feUnit(i).channels()) {
	  return feUnit(i).channel(fedchannel);
	}
	fedchannel -= feUnit(i).channels();
      }
      ICUTILS_VERIFY(false)(static_cast<int>(Fed9UAddress(fedchannel).getExternalFedChannel()))(totalChannels()).msg("Bad channel number").error();
      THROW(ICUtils::ICException("Bad channel number"));
    }

    /**
     * \brief  Returns the appropriate event data for a given FED channel.
     * \param  fedChannel The FED channel is given by the Fed9UAddress object and as such does not require any specific type of channel number scheme to be used.
     * \return Fed9UEventChannel Constant reference to the event channel that was requested.
     */
    const Fed9UEventChannel & channel( Fed9UAddress fedChannel ) const {
      return channel(fedChannel.getFedChannel());
    }

    /**
     * \brief  Returns the number of samples that are present on the first channel.
     * \return unsigned short Number of samples in channel.
     *
     * \todo - extend to read all channels and check for consistancy.
     */
    u16 samples() const {
      return channel(0).samples();
      // should really check all channels are consistent
    }

  // <NAC date="21/09/2007"> made getFeBufferLength protected and added getters for data members for use by derrived class
  protected:
    /**
     * \brief  Returns the buffer length in number of bytes of a given FE unit.
     * \param  feUnit FE unit that is to be addressed.
     * \return int Number of bytes in the buffer. 
     */
    int getFeBufferLength(int feUnit) const;
    
    /**
     * \brief  Get the buffer
     * \return const Fed9UEventIterator& buffer
     */
    const Fed9UEventIterator& getBuffer() const { return _buffer; }
    
    /**
     * \brief  Get the packet payload
     * \return const Fed9UEventIterator& payload
     */
    const Fed9UEventIterator& getPayload() const { return _payload; }
    
    /**
     * \brief  Get a pointer to the description used
     * \return const Fed9UDescription* description
     */
    const Fed9UDescription* getDescription() const { return _fedDescription; }
    
    /**
     * \brief  Get the header format
     * \return Fed9UHeaderFormat header format
     */
    Fed9UHeaderFormat getHeaderFormat() const { return _headerFormat; }
    
    /**
     * \brief  Get the event format
     * \return Fed9UEventFormatType event format
     */
    Fed9UEventFormatType getEventFormat() const { return _eventFormat; }
    
    /**
     * \brief  Get the offset of the tracker special header (in bytes)
     * \return u16 offset
     */
    u16 getSpecialOffset() const { return SPECIAL_OFF; }
   // </NAC>

  private:
    /**
     * \name Copy constructor and assignment operator.
     *
     * Declared private and unimplemented to prevent copying of event classes.
     */
    //@{
    Fed9UEvent(const Fed9UEvent &);
    Fed9UEvent & operator = (const Fed9UEvent &);
    //@}

    Fed9UEventIterator _buffer, _trailer, _payload;     //!< Fed9UEventIterators that point to the buffer, trailer and payload respectively.
    std::vector<Fed9UEventUnit> _feunits;     //!< Vector contains an element for each of the FE units recorded in the event.
    u16 _rubbish;                             //!< Used to place data that has been read out but is not required by that specific member function.
    const Fed9UDescription * _fedDescription; //!< Pointer to the Fed9UDescription that contains the settings on the FED that were present when the event was taken.
    static const u16 SPECIAL_OFF = 8;         //!< Offset of tracker special header in bytes.
    bool _deleteBuffer;                       //!< Used to flag if the buffer is to be deleted.
    Fed9UEventDataMode _mode;                 //!< Type of data that was recorded (fake, real or legacy), as defined by Fed9UEventDataMode.
    Fed9UHeaderFormat _headerFormat;          //!< Local copy of the Fed9UHeaderFormat that is used to define the header format used in the event.
    //<JEC date=11/4/07> new internal variable to handle 32-bit swapping in DAQ header and trailer
    Fed9UEventFormatType _eventFormat;        //!< Format of the event record (original VME ordering or new common VME/SLINK ordering.
    //</JEC>

  };

  inline std::ostream& operator<<(std::ostream& os, const Fed9UEvent & ev) { ev.dump(os); return os; }

}

#endif // H_Fed9UEvent
