#ifndef H_Fed9UEpromDescription
#define H_Fed9UEpromDescription

//Author: Gareth Rogers
//Date  : April 2004

/**
 * 
 */

#include "TypeDefs.hh"

namespace Fed9U {

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  A class that provides the ability to configure the EPROM on the FED.
   * \author Gareth Rogers, RAL
   * \date   04/2004
   *
   * Fed9UEpromDescription contains 2KB of memory that can be formatted as required by the FED. This class also supports an EPROM version number
   * which allows EPROM data to be saved in different formats, according to which version of events is used. This class has no knowledge however
   * of how the write protected areas of the FED are setup. It assumes all is read/writable, however this is not the case on the FED. The class
   * currently supports versions 1 and 2.
   */
  class Fed9UEpromDescription {

  public:

    /**
     * \brief Constructor.
     * \param version EPROM version number. The structure of the saved data in the EPROM varies depending on the version of the EPROM used.
     *
     * It takes a pointer to a 2KB block of memory of which a local copy is made. The data pointer is defaulted to a null pointer and
     * if null a blank 2KB of memory will be created.
     */
    explicit Fed9UEpromDescription(u32 version, const u8* const data = 0);

    /**
     * \brief  This allows you write to a single byte of memory.
     * \param  offset Offset in to EPROM memory where the byte should be written. Range:0x000-0x7ff.
     * \param  data Byte that is to be written to memory. Range:0-0xff.
     * \return Self reference.
     */
    Fed9UEpromDescription& setSingleByte(u32 offset, u8 data);

    /**
     * \brief  This allows you to read a single byte of memory.
     * \param  offset Offset in to EPROM memory where the byte should be read. Range:0x000-0x7ff.
     * \return unsigned char Byte that is read back from that area of the memory.
     */
    u8 getSingleByte(u32 offset) const;

    /**
     * \brief  Each FED has a serial number to uniquely identify it.
     * \param  serialNumber Serial number of the FED.
     * \return Self reference.
     */
    Fed9UEpromDescription& setFedSerialNumber(u32 serialNumber);

    /**
     * \brief  Each FED has a serial number to uniquely identify it.
     * \return unsigned long Serial number for the FED.
     */
    u32 getFedSerialNumber() const;

    /**
     * \brief  An overloaded version of the operator[] which provides the same functionality as set/getEpromSingleByte.
     * \param  offset Offset in to EPROM memory where the byte should be written/read. Range:0x000-0x7ff.
     * \return unsigned char Writeable reference to the byte at the position specified by offset.
     *
     * The reference will give both the value of the byte currently in that location and allow the User to write to it.
     */
    u8& operator[](u32 offset);

    /**
     * \brief  Another overloaded version of the operator[], this however this returns a copy of the byte and as such is read only.
     * \param  offset Offset in to EPROM memory where the byte should be written. Range:0x000-0x7ff.
     * \return unsigned char Copy of the byte at the position specified by offset.
     */
    u8 operator[](u32 offset) const;

    /**
     * \brief  Returns the size of the EPROM memory in bytes.
     * \return unsigned long Size of the EPROM memory.
     */
    u32 getEpromSize() const;
    
    /**
     * \brief  Sets the version of the eprom.
     * \param  version Version of the EPROM that is to be set.
     * \return Self reference.
     */
    Fed9UEpromDescription& setEpromVersion(const u32 version);

    /**
     * \brief  Returns the version of the EPROM.
     * \return unsigned long Version number of the EPROM memory format.
     */
    u32 getEpromVersion() const;

  private:

    /**
     * \brief Size of the EPROM in bytes.
     *
     * The useable EPROM memory is actually 1 byte less than the total size as the last byte of the EPROM is used to set the write
     * protection level and cannot be used for storage.
     */
    static const u32 epromSize_ = 2048;

    u8 epromData_[epromSize_]; //!< This holds the EPROM memory, which is set to be as large as the eprom size.
    u32 epromVersion_;         //!< Stores the version of the EPROM that we are currently using.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r);
    // </NAC>

  };// End of class Fed9UEpromDescription
  
  // <NAC date="24/04/2007"> operator to compare EpromDescriptions
  /**
   * \brief  == operator to allow Fed9UEpromDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r);
  /**
   * \brief  != operator to allow Fed9UEpromDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UEpromDescription& l, const Fed9UEpromDescription& r) { return !(l==r); }
  // </NAC>

  /**
   * \brief  This overload of operator<< can used to write the entire EPROM memory to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data EPROM object that is to be written to the stream.
   * \return ostream Reference to the output stream that the EPROM data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UEpromDescription& data);

  /**
   * \brief  This overload of operator>> can used to read the entire EPROM memory to from an input stream.
   * \param  is Input stream the data is read from.
   * \param  data EPROM object that is filled with the data in the stream.
   * \return istream Reference to the input stream that the EPROM data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UEpromDescription& data);

}// End of Fed9U namespace
#endif// H_Fed9UEpromDescription
