/** Jo Cole June 2006 
 *  Updated March 2007   
 */

#ifndef H_Fed9UEventUnitStreamLine
#define H_Fed9UEventUnitStreamLine

#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#include "Fed9UEventStreamLine.hh"
#include "Fed9UEventStreamLineException.hh"
#include "Fed9USu8.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using std::vector;

  /**  New simplified FE-FPGA class - used by the new streamlined Fed event unpacking class.
       NB.  When a method requires the FE-unit or channel number, you should give the number of the FPGA/channel you want, NOT
       the location in the header/payload.  For example, FE-FPGA number 8 is always first in the header or payload, so give
       Fe-unit number = 8 to access the information associated to it.  A similar approach should be used for the channel numbering,
       bearing in mind that channel numbers are quoted with respect to the associated FE-FPGA, not with respect to the FED, ie.
       channels are numbered 1 - 12, not 1 - 96.
  */
  //<JEC date=21/03/07>  several changes including the use of the u8 pointer wrapper, Fed9USu8.
  
  class Fed9UEventUnitStreamLine {
    
  public:
    /** Default constructor.*/
    Fed9UEventUnitStreamLine() : _headerFormat(FED9U_HEADER_FULLDEBUG), _unit(0) {
      _header.set(NULL);
      _payload.set(NULL);
      _sample.set(NULL);
    };
    /** Standard constructor, which should be used for normal data.
	It should be given a pointer to the start of the FE-Unit-related header information, a pointer to the start of
	the associated payload data, the Header format (FULL DEBUG or APV ERROR) and the unit number.*/
    Fed9UEventUnitStreamLine(u8 * header, u8 * payload, Fed9UHeaderFormat headerFormat, u32 unit);

    /** Returns a pointer to the end of the payload data (including the padding).*/
    u8 * getPointerToEndOfUnitPayload();
    /** Returns either the frame synch out packet (FULL DEBUG mode) or the APV error flags (APV ERROR mode), as stored in the 
	tracker-specific header.  The information is returned in a u8 vector, with the LSB stored in element 0.  
	The vector should be three elements long for the APV error mode and sixteen elements long for FULL DEBUG mode.*/
    vector<u8> getFeUnitHeader() const;
    /** Returns the status of a specified channel, as stored in the header.  If the data is in FULL DEBUG mode, then the
	status code will be 6 bits long, but if it is in APV ERROR mode, then it will be only 2 bits long.*/
    u8 getChannelStatus(u32 channel) const;
    /** Returns the data length for a given FE-FPGA, as stored in the FULL DEBUG mode of the tracker-specific header.
	If the header is in APV ERROR mode, then the length is simply zero.*/
    u16 getUnitDataLengthFromHeader() const;
    /** Returns the majority APV pipeline address, as stored in the FULL DEBUG mode of the tracker-specific header.
        If the header is in APV ERROR mode, then the address is simply zero.*/
    u8 getMajorityPipelineAddress() const;
    /** Returns the packet code, as stored in the payload, which identifies the mode which the data was taken.
	The options are:
	Scope mode                - packet code = xxx0 0101
	Virgin raw data mode      - packet code = xxx0 0110
	Processed raw data mode   - packet code = xxx1 0010
	Zero suppressed mode      - packet code = xxx0 1010
	Zero suppressed lite mode - packet code not stored in payload - method returns zero.
	Only the lower 5 bits are returned.  The upper three bits are reserved.*/
    u8 getPacketCode(u32 channel) const;
    /** Returns the median for a specified APV on a specific channel, as stored in the payload.  
	This information is only stored in Zero Suppressed mode.*/
    u16 getMedian(u32 channel, u32 apv) const;
    /** Returns the data length for a specified channel, as stored in the payload.
	The length is given in bytes and includes the lengths and packet code.*/
    u16 getChannelDataLength(u32 channel) const {return _channelDataLength[12 - channel];}
    /** Returns the total data length for a given FE-FPGA, based on information stored in the payload.*/
    u16 getUnitDataLengthFromPayload() const;
    /** Returns the number of data words for a given channel.  For SCOPE, VIRGIN RAW and PROCESSED RAW data modes, the
     actual number of raw data words is returned.  For the ZERO SUPPRESSED modes the maximum number of clusters is returned.*/
    u16 getSamples(u32 channel) const;
    /** Returns a Fed9USu8 pointer to the data from a specific channel.  To get a pointer to the start of the data,
	simply set dataLoc = 0.  Other values of dataLoc can be used if a pointer to a specific cluster or raw data word is required.*/
    //<JEC date=21/03/07>
    Fed9USu8 & getSampleSpecialPointer(u32 channel, u32 dataLoc);
    //</JEC>
    /** Returns a data sample at a specific location for a specific channel.
	dataLoc = 0 corresponds to the first piece of data in the payload (ie. after length, packet code and median information, where stored).
	Raw data for Scope, Virgin Raw and Processed Raw modes is stored in two bytes.  In ZS and ZS lite, the cluster data is stored in
	bytes, so using this method will return two pieces of cluster data or'ed together.*/
    u16 getSample(u32 channel, u32 dataLoc) const;
    /** Returns a vector containing the data samples for a specified channel.
	For Raw data modes, each vector element contains a two-byte data word, while for the ZS modes each vector element contains one byte.
	Use for time-critical applications.*/
    vector<u16> getSample(u32 channel) const;
    //<JEC date=18/4/07> extra method for use in Fed9UDevice (TrimDAC calibration)
    /** Retrieves only the data (ie. no strip addresses or cluster widths) for a given channel and stores them in a pre-defined buffer to which
	destBuffer is a pointer.*/
    void getSample(u32 channel, u16 * destBuffer);
    //</JEC>
    /** Returns a pointer to the position of the data from a specified FE-FPGA in the payload.*/
    //<JEC date=21/03/07> changed to return a special u8 pointer
    Fed9USu8 getFEUnitInPayloadPtr() const {return _payload;}
    //</JEC>
    /** Performs a consistency check (where possible) of channel data lengths.  Returns true if everything is OK.
	For ZERO SUPPRESSED modes channel data lengths are not expected to be the same, so the return value is always set to true.*/
    bool checkChannelLengths();

  private:

    /** The different possible packet codes for the different data formats.  ZS LITE is not included as the packet code
	is not stored in this data format.*/
    enum { FED9U_PACKET_SCOPE = 5, FED9U_PACKET_VIRGRAW = 6, FED9U_PACKET_ZEROSUPP = 10, FED9U_PACKET_PROCRAW = 18 };
    //AAB updated with more modes AAB (12/8/205)
    enum { FED9U_PACKET_SCOPE_hex = 0xE1, FED9U_PACKET_VIRGRAW_hex = 0xE6, FED9U_PACKET_VIRGRAW_10BIT = 0x86,
         FED9U_PACKET_ZEROSUPP_hex = 0xEA, FED9U_PACKET_PROCRAW_hex = 0xF2, FED9U_PACKET_ZEROSUPP_10BIT = 0x8A,
         FED9U_PACKET_ZEROSUPP_LO = 0xCA, FED9U_PACKET_ZEROSUPP_HI_LO = 0xAA,
         FED9U_PACKET_PROCRAW_10BIT = 0x92,
         FED9U_PACKET_PROCRAW_8BIT_HI_LO = 0xB2,
         FED9U_PACKET_PROCRAW_8BIT_LO = 0xD2,
         FED9U_PACKET_VIRGRAW_8BIT_HI_LO = 0xA6,
         FED9U_PACKET_VIRGRAW_8BIT_LO = 0xC6
     };
        
    /** Private method which returns the location of two consecutive bits of data.  The data is packed into each 64-bit word 
	starting from the MSB and running to the LSB, but the access to the data starts from the middle of the 64-bit word, as
	illustrated:
	63                              0
	+---+---+---+---+---+---+---+---+
	| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |              --> byte-order for data packing
	+---+---+---+---+---+---+---+---+
	| 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 |              --> byte-order for data access
	+---+---+---+---+---+---+---+---+
	
	This routine takes the given channel and location within that channels data in the data packing format and converts it
	into the location of two consecutive words in the data access format.  The location is returned in terms of a number of
	full 64-bit words and then the locations of the two words with respect to the last (incomplete) 64-bit word.*/
    void getLocations(u32 channel, u32 offset, int &nOf64BWords, int &lowerWd, int &upperWd) const;
    int getLocations(u32 channel, u32 offset) const;
    
    /** A pointer to the start of the FE-Unit-specific data in the event header.*/
    Fed9USu8 _header, _payload, _sample;
    /** The header format:  FULL DEBUG or APV ERROR.*/
    Fed9UHeaderFormat _headerFormat;
    /** FE-Unit number.*/
    u32 _unit;
    /** Vector containing the data length for each channel.*/
    vector<u16> _channelDataLength;
  };
  //</JEC>  
}

#endif // H_Fed9UEventUnitStreamLine
