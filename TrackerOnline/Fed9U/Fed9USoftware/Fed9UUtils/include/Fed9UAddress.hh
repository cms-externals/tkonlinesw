#ifndef H_Fed9UChannelAddress
#define H_Fed9UChannelAddress

#include "TypeDefs.hh"
#include "Fed9UChannelInterface.hh"

namespace Fed9U {

  /**
   * FED front-end address conversion class.
   *
   * Front-end address class, can point to any area of the FED front-end, from a front-end unit,
   * through any fibre channel, an individual APV, or any strip within an APV. \n
   * The different granularities are achieved by setting a value of UNSPECIFIED in all
   * unrequired levels. \n
   * It can also point to the BE and VME FPGAs, which do not containing any subunits.
   * Example: \n
   * To get the FED channel number of channel 11 in unit 3: \n
\verbatim
u8 fedChannelNumber = Fed9UAddress(3, 11).getFedChannel();
\endverbatim
   * It is also possible to create an Fed9UAddress object from the FED channel number: \n
\verbatim
Fed9UAddress fedChannel = Fed9UAddress(123, 0);
u8 fedFeUnit = fedChannel.getFedFeUnit();
u8 moduleChannel = fedChannel.getFeUnitChannel();
\endverbatim
   * The setters return a reference to the original object, so that multiple sets can be chained together.
   *
   * The second part to this class is an interface to an external numbering system. If the external numbering 
   * system differs from the internal. In the example of the FED the external numbering for channels runs from 
   * 1 to 96 from bottom to top but the internal numbering runs from top to bottom 0 to 95. Also the numbering of the 
   * Front End Units (FEUnits) runs from 1 to 8 bottom to top externally, and 0 to 7 top to bottom internally.
   * Users must be aware of this difference and therefore make sure they know when they are using external methods.
   * all external methods are marked with External Keyword. So getFEUnitChannel() will get you the internal representation
   * and getExternalFEUnitChannel() will get you the user defined external numbering for that channel.
   * if the channel in question is internally numbered as 5 then the external FedChannel will be 91 for final CMS Tracker FED 
   * numbering.
   * WARNING, if you change the base to anything other than 0 or 1 then there will be conflicts between BACKEND fpga number and FeUnit number since this should be 9 for both. This software is not designed to work for any other bases than 0 and 1.
   ******************************************/

#define FED9U_ADDRESS_COUNT_FROM_TOP false
#define FED9U_ADDRESS_COUNT_BASE 1

  class Fed9UAddress {
  public:
    /**
     * Defines the values for a few specific addresses on the FED.
     *
     * \li UNSPECIFIED Used internally if an address is not set, most commonly when that level of granularity is not required.
     * \li FEBROADCAST Tells the FED that the command should be issued to all FE units.
     * \li BACKEND The BE FPGA address, used for commands that can be issued to all FPGAs.
     * \li VME The VME FPGA address, used for commands that can be issued to all FPGAs.
     */
    enum { UNSPECIFIED = 0xFF, FEBROADCAST = 14, BACKEND = 9, VME = 10 };

    /** \name Constructors */
    //@{

    /**
     * \brief Default constructor. Sets all values to unspecified and counts from bottom, with a base of one by default.
     *
     * Used to create an empty address, that can be updated using the setter member functions.
     */
    Fed9UAddress();

    /**
     * \brief A constructor provided to allow a FED channel address and further granularities to be set upon construction.
     * \param fedChannel FED channel that is to be used, will also set the FE unit and FE unit channel.
     * \param channelApv APV on the channel to be set. Defaulted to UNSPECIFIED, not required at all granularities.
     * \param apvStrip Strip on a given APV, as defined by channelApv. Defaulted to UNSPECIFIED, only required at lowest granularities.
     */
    Fed9UAddress(u8 fedChannel, u8 channelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);

    /**
     * \brief A constructor provided to allow the counting direction to be specfied and the base to be set.
     * \param bool Boolean true will start the counting from the top of the FED, internal numbering. A boolean
     *        false will start the numbering from the bottom, external number.
     * \param u16 Sets the counting base, 0 or 1 are the only two valid bases. External numbering uses 1 as the first bottom FE unit
     *        and internal numbering uses 0 for the top FE unit.
     *
     * The counting base also affects the channel, APV and strip numbering as well as the FE unit number.
     */
    Fed9UAddress(bool, u16);

    //@}
    /** \name Getters */
    //@{

    /**
     * \brief  Returns which of the ten FED FPGA is being addressed.
     * \return unsigned char FED FGPA number, internal numbering system.
     *
     * FED FPGAs include FE, BE, VME and broadcast settings. These latter three are defined in the class enumeration.
     */
    u8  getFedFpga() const;

    /**
     * \brief  Returns which of the eight FED FE units is being addressed.
     * \return unsigned char FED FE unit number, internal numbering system.
     */
    u8  getFedFeUnit() const;

    /**
     * \brief  Returns which of the 12 FE channels is being addressed.
     * \return unsigned char FED channel number, internal numbering system.
     */
    u8  getFeUnitChannel() const;
    
    // <NAC date=28/03/2007> DelayChip
    /**
     * \brief  Returns which of the 3 delay chips is being addressed.
     * \return unsigned char FED FE unit delay chip number (0-3, top to bottom).
     */
    u8  getFeUnitDelayChip() const;
    
    /**
     * \brief  Returns which of the 24 delay chips is being addressed.
     * \return unsigned char FED delay chip number.
     */
    u8 getFedDelayChip() const;
    // </NAC>

    /**
     * \brief  Returns which of the two channel APVs are being addressed.
     * \return unsigned char Channel APV number, internal numbering system.
     */
    u8  getChannelApv() const;

    /**
     * \brief  Returns which of the 128 strips on an APV is being addressed.
     * \return unsigned char APV strip unit number, internal numbering system.
     */
    u8  getApvStrip() const;

    /**
     * \brief  Returns which of the 96 FED channel is being addressed.
     * \return unsigned char FED channel number, internal numbering system.
     */
    u8  getFedChannel() const;

    /**
     * \brief  Returns which of the 192 FED APVs is being addressed.
     * \return unsigned char FED APV number, internal numbering system.
     */
    u8  getFedApv() const;

    /**
     * \brief  Returns which of the FEDs 24,576 strips are being addressed.
     * \return unsigned char FED strip number, internal numbering system.
     */
    u16 getFedStrip() const;
    /**
     * \brief  Returns which of the 24 FE unit APVs is being addressed.
     * \return unsigned char FE unit APV number, internal numbering system.
     */
    u8  getFeUnitApv() const;

    /**
     * \brief  Returns which of the FEDs 48 channel pairs is being addressed.
     * \return unsigned char FED channel pair number, internal numbering system.
     */
    u8  getFedChannelPair() const;

    /**
     * \brief  Returns which of theis six FE unit channel pairs is being addressed.
     * \return unsigned char FE unit channel pair number, internal numbering system.
     */
    u8  getFeUnitChannelPair() const;

    /**
     * \brief  Returns which of the channels from the channel pair is being addressed.
     * \return unsigned char Channel from the channel pair number, internal number system.
     */
    u8  getChannelPairChannel() const;

    /**
     * \brief  Returns which of the channels in the pair is being addressed in the external numbering scheme.
     * \return unsigned char External channel pair number.
     */
    u8  getExternalChannelPairChannel() const;

    /**
     * \brief  Returns which of the 48 FED channel pairs is being addressed in the external numbering scheme.
     * \return unsigned char External FED channel pair number.
     */
    u8  getExternalFedChannelPair() const;

    /**
     * \brief  Returns which of the six FE unit channel pairs is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel pair number.
     */
    u8  getExternalFeUnitChannelPair() const;

    /**
     * \brief  Returns which of the eight FE unit is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel pair number.
     */
    u8  getExternalFedFeUnit() const;

    /**
     * \brief  Returns which of the 12 FE unit channels is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit channel number.
     */
    u8  getExternalFeUnitChannel() const;

    /**
     * \brief  Returns which of the two channel APVs is being addressed in the external numbering scheme.
     * \return unsigned char External channel APV number.
     */
    u8  getExternalChannelApv() const;

    /**
     * \brief  Returns which of the 128 APV strips is being addressed in the external numbering scheme.
     * \return unsigned char External APV strip number.
     */
    u8  getExternalApvStrip() const;

    /**
     * \brief  Returns which of the 96 FED channels is being addressed in the external numbering scheme.
     * \return unsigned char External FED channel number.
     */
    u8  getExternalFedChannel() const; 

    /**
     * \brief  Returns which of the 192 FED APVs is being addressed in the external numbering scheme.
     * \return unsigned char External FED APV number.
     */
    u16 getExternalFedApv() const;

    /**
     * \brief  Returns which of the 24,576 FED strips is being addressed in the external numbering scheme.
     * \return unsigned char External FED strip number.
     */
    u16 getExternalFedStrip() const;

    /**
     * \brief  Returns which of the 48 FE unit APVs is being addressed in the external numbering scheme.
     * \return unsigned char External FE unit APV number.
     */
    u8  getExternalFeUnitApv() const;

    /**
     * \brief  Returns the counting base used by the class.
     * \return unsigned short Counting base.
     */
    u16 getCountBase() const;

    /**
     * \brief  Whether the class is counting from the top or bottom FE unit.
     * \return bool Boolean true it is counting from the top, internal numbering; boolean flase from the bottom, external numbering.
     */
    bool getCountFromTop() const;

    /**
     * \brief  This method allows you to get the feUnit number based upon the feFirmware Address.
     * \return unsigned char FE firmware FE unit number.
     *
     * The FED firmware numbering runs from 1 throuh 8 and 15, than from 0 through 7 as in software internal
     * or 8 through 1 in external numbering is used inside the Fed9UVmeBase and below. It should be used when
     * passing fpga numbers to Fed9UVmeBase.
     */
    u8 getFirmwareFedFeUnit() const;
      
    //@}
    /** \name Setters */
    //@{

    /**
     * \brief  Allows the setting of up to the full range of address granularity from FE unit through FE unit channel, channel APV and finally APV strip.
     * \param  fedFeUnit FE unit address that is to be set. Range: 0-7.
     * \param  feUnitChannel FE unit channel that is to be addressed. Range: 0-11. Default UNSPECIFIED.
     * \param  channelApv APV on the channel that is to be addressed. Range: 0-1. Default UNSPECIFIED.
     * \param  apvStrip Strip number on the given APV to be addressed. Range:0-127. Default UNSPECIFIED.
     * \return Self reference
     *
     * Allow the address to be set at each granularity stage, but it is only required to be set to the FED FE unit level with the rest being left UNSPECIFIED.
     */
    Fed9UAddress& setAddress(u8 fedFeUnit, u8 feUnitChannel = UNSPECIFIED,
			  u8 channelApv = UNSPECIFIED, u8 apvStrip = UNSPECIFIED);

    /**
     * \brief  Sets the FED FPGA to be addressed in the internal numbering scheme.
     * \param  fedFpga FED FPGA number. Range:0-7, BACKEND, VME and BROADCAST.
     * \return Self reference.
     */
    Fed9UAddress& setFedFpga(u8 fedFpga);

    /**
     * \brief  Sets the FED FE unit to be addressed in the internal numbering scheme.
     * \param  value FED FE unit number. Range:0-7 and BROADCAST.
     * \return Self reference.
     */
    Fed9UAddress& setFedFeUnit(u8 value);

    /**
     * \brief  Sets the FED FE unit channel to be addressed in the internal numbering scheme.
     * \param  value FED FE unit channel number. Range:0-11.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitChannel(u8 value);
    
    // <NAC date=28/03/2007>  DelayChip
    /**
     * \brief  Sets the FED FE unit delay chip to be addressed.
     * \param  value FED FE unit delay chip number. Range:0-3.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitDelayChip(u8 value);
    
    /**
     * \brief  Sets the FED delay chip to be addressed.
     * \param  value FED delay chip number. Range:0-23.
     * \return Self reference.
     */
    Fed9UAddress& setFedDelayChip(u8 value);
    // </NAC>

    /**
     * \brief  Sets the channel APV to be addressed in the internal numbering scheme.
     * \param  value Channel APV number. Range:0-1.
     * \return Self reference.
     */
    Fed9UAddress& setChannelApv(u8 value);

    /**
     * \brief  Sets the APV strip to be addressed in the internal numbering scheme.
     * \param  value APV strip number. Range:0-127.
     * \return Self reference.
     */
    Fed9UAddress& setApvStrip(u8 value);

    /**
     * \brief  Sets the FED channel to be addressed in the internal numbering scheme.
     * \param  fedChannel FED channel number. Range:0-95.
     * \return Self reference.
     */
    Fed9UAddress& setFedChannel(u8 fedChannel);

    /**
     * \brief  Sets the FED APV to be addressed in the internal numbering scheme.
     * \param  fedApv FED APV number. Range:0-191.
     * \return Self reference.
     */
    Fed9UAddress& setFedApv(u8 fedApv);

    /**
     * \brief  Sets the FED strip to be addressed in the internal numbering scheme.
     * \param  fedStrip FED strip number. Range:0-24,576.
     * \return Self reference.
     */
    Fed9UAddress& setFedStrip(u16 fedStrip);

    /**
     * \brief  Sets the FE unit APV to be addressed in the internal numbering scheme.
     * \param  feUnitApv FE unit APV number. Range:0-47.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitApv(u8 feUnitApv);

    /**
     * \brief  Sets the FED channel pair to be addressed in the internal numbering scheme.
     * \param  fedChannelPair FED channel pair number. Range:0-47.
     * \return Self reference.
     */
    Fed9UAddress& setFedChannelPair(u8 fedChannelPair);
    /**
     * \brief  Sets the FE unit channel pair to be addressed in the internal numbering scheme.
     * \param  feUnitChannelPair FE unit channel pair number. Range:0-5.
     * \return Self reference.
     */
    Fed9UAddress& setFeUnitChannelPair(u8 feUnitChannelPair);

    /**
     * \brief  Sets the channel in the channel pair to be addressed in the internal numbering scheme.
     * \param  channel Channel in the channel pair number. Range:0-1.
     * \return Self reference.
     */
    Fed9UAddress& setChannelPairChannel(u8 channel);

    /**
     * \brief  Sets the channel from the channel pair to be addressed in the external numbering scheme.
     * \param  channel External channel in the channel pair. Range:1-2.
     * \return Self reference.
     */
    Fed9UAddress& setExternalChannelPairChannel(u8 channel);

    /**
     * \brief  Sets the FED channel pair to be addressed in the external numbering scheme.
     * \param  fedChannelPair FED channel pair number. Range:1-48.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedChannelPair(u8 fedChannelPair);

    /**
     * \brief  Sets the FE unit channel pair to be addressed in the external numbering scheme.
     * \param  feUnitChannelPair FE unit channel pair number. Range:1-6.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitChannelPair(u8 feUnitChannelPair);

    /**
     * \brief  Sets the FED FE unit to be addressed in the external numbering scheme.
     * \param  value FED FE unit number. Range:1-8.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedFeUnit(u8 value);

    /**
     * \brief  Sets the FE unit channel to be addressed in the external numbering scheme.
     * \param  value FE unit channel number. Range:1-12.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitChannel(u8 value);

    /**
     * \brief  Sets the channel APV to be addressed in the external numbering scheme.
     * \param  value Channel APV number. Range:1-2.
     * \return Self reference.
     */
    Fed9UAddress& setExternalChannelApv(u8 value);

    /**
     * \brief  Sets the APV strip to be addressed in the external numbering scheme.
     * \param  value APV strip number. Range:1-128.
     * \return Self reference.
     */
    Fed9UAddress& setExternalApvStrip(u8 value);

    /**
     * \brief  Sets the FED channel to be addressed in the external numbering scheme.
     * \param  value FED channel number. Range:1-92.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedChannel(u8 value); 

    /**
     * \brief  Sets the FED APV to be addressed in the external numbering scheme.
     * \param  value FED APV number. Range:1-192.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedApv(u16 value); 

    /**
     * \brief  Sets the FED strip to be addressed in the external numbering scheme.
     * \param  value FED strip number. Range:1-24,576.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFedStrip(u16 value); 

    /**
     * \brief  Sets the FE unit APV to be addressed in the external numbering scheme.
     * \param  feUnitApv FE unit APV number. Range:1-24.
     * \return Self reference.
     */
    Fed9UAddress& setExternalFeUnitApv(u8 feUnitApv);
    
    
    /** 
     * \brief  This method allows you to set the feUnit number based upon the feFirmware Address.
     * \param  feFpga FE FPGA number. Range:1-8.
     * \return Self reference.
     *
     * The FED firmware numbering runs from 1 throuh 8 and 15, rather than from 0 through 7 as in software internal
     * or 8 through 1 in external numbering. This is used inside the Fed9UVmeBase and below. It should be used when
     * passing FPGA numbers to Fed9UVmeBase.
     */
    Fed9UAddress& setFirmwareFedFeUnit(const u8 feFpga);

    //@}

  private:
    u8 _femod;          //!< Stores the FE unit number.
    u8 _fechan;         //!< Stores the FE channel number.
    u8 _apv;            //!< Stores the channel APV number.
    u8 _strip;          //!< Stores the APV strip number.
    bool _countFromTop; //!< Whether the first FE unit is counted from the top.
    u16 _countBase;     //!< 0 or 1 depending on the counting base.
    // <NAC date=28/03/2007>
    bool _broadcastDelayChip;	//!< Controls whether delay chip is calculate from channel number or set to 3 (broadcast to delaychips on one FeUnit)
    // <NAC>
  };

}

#endif // H_Fed9UChannelAddress
