#ifndef H_Fed9UFrontEndDescription
#define H_Fed9UFrontEndDescription

#include "TypeDefs.hh"
#include "Fed9UTempControl.hh"

#include <iosfwd>

namespace Fed9U {

  struct Fed9UAdcControls {
    //public:
    Fed9UAdcControls(bool dfsen = false, bool dfsval = false, bool s1 = false, bool s2 = false)
      : _dfsen(dfsen), _dfsval(dfsval), _s1(s1), _s2(s2) { }
    /**
     * \brief  Overloaded == operator to allow Fed9UAdcControls to be compared
     * \param  A LHS
     * \param  B RHS
     * \return bool, true or false for == comparison
     */
    // <NAC date="24/04/2007"> added const
    bool operator==(const Fed9UAdcControls& A) const;
    // </NAC>
    bool _dfsen;
    bool _dfsval;
    bool _s1;
    bool _s2;
  };


  /**
   * \brief  Description of a FED front-end module.
   * \author Jonathan Fulcher, IC.
   *
   * It contains all the settings that are associated with a FE module and forms part of the Fed9UDescription,
   * which stores the settings for the whole FED. It also provides streamers to allow the settings to be read/written
   * to/from a stream, typically a file.
   */
  class Fed9UFrontEndDescription {

  public:

    /**
     * \brief Default Constructor.
     *
     * \param fine Sets the fine delay that is applied to the FE unit channels. Range:0-24.
     * \param coarse Sets the coarse delay that is applied to the FE unt channels. Range:0-15.
     * \param trim Sets the trimDAC offset for each FE unit channel. Range:0-255.
     * \param opto Sets the input and output offset for the OptoRxs on the FE unit channels. Input offset range:0xf, output offset range:0-3.
     *             Input offset in first four bits, output in the 5th and 6th bits. Range:0-0x3f.
     * \param cap Sets the capacitor value for the OptoRxs on the FE unit channels. Range:0-3.
     * \param apvDisable Sets the APV disable flags for the FE unit APVs. Boolean true disabled, false enabled.
     * \param apvFakeEventDisable Set the fake event disable flag for the FE unit APVs. Boolean true disabled, false enabled.
     * \param median Sets the common mode median values for APVs on the FE unit APVs. Range:0-1023.
     * \param medianDisable Sets the common mode median disable flags on the FE unit APVs. Boolean true disabled, false enabled.
     * \param channelThreshold Sets the threshold value for the optical input signal on the ADCs. Range:0-992, steps 32.
     * \param channelBufferOccupancy Sets the occupancy level of the buffers storing the event buffers.
     * \param adcControls Sets the ADC control bits, using the Fed9UADCControls struct.
     * \param feUnitDisable Sets the FE unit disable flag. Boolean true disabled, false enabled.
     * \param tempControl Sets the temperature controls for that FE unit, using the Fed9UTempControl class.
     * \param complement Sets the complement incoming data flag. Boolean true disabled, false enabled.
     * \param fakeEventRandomSeed Sets the seed for the fake events random noise.
     * \param fakeEventRandomMask Sets the mask for the fake events.
     *
     * All the settings have been given default values, such that a default constructed object will have settings that a FED can
     * be run with. Further for values that can be unique for channels or APVs are all set to the same constructor passed value,
     * but can be updated through setters in the Fed9UDescription class. This class only provides a container for the settings and not access.
     */
    Fed9UFrontEndDescription(u16 fine = 0, 
			     u16 coarse = 0, 
			     u16 trim = 0, 
			     u16 opto = 0, 
			     u16 cap = 0, 
			     bool apvDisable = false,
			     //<GR date=27/07/2006> Added APV fake event disable
			     bool apvFakeEventDisable = false,
			     //</GR>
			     u16 median = 0, 
			     bool medianDisable = true,
			     u16 channelThreshold = 0,
			     u16 channelBufferOccupancy = 0,
			     const Fed9UAdcControls & adcControls = Fed9UAdcControls(false,false,false,false),
			     bool feUnitDisable = 0,
			     const Fed9UTempControl & tempControl = Fed9UTempControl(),
                             bool complement = true,
			     u16 fakeEventRandomSeed = 0,
			     u16 fakeEventRandomMask = 0
			     );

    /**
     * \brief Destructor.
     */
    ~Fed9UFrontEndDescription() {}

    u16 _fineDelay[CHANNELS_PER_FEUNIT];                //!< Array to contain the fine delay settings for the FE unit channels.
    u16 _coarseDelay[CHANNELS_PER_FEUNIT];              //!< Array to contain the coarse delay settings for the FE unit channels.
    u16 _trimDacOffset[CHANNELS_PER_FEUNIT];            //!< Array to contain the trimDAC offset settings for the FE unit channels.
    u16 _channelThreshold[CHANNELS_PER_FEUNIT];         //!< Array to contain the ADCs threshold settings for the FE unit channels.
    u16 _channelBufferOccupancy[CHANNELS_PER_FEUNIT];   //!< \todo - remove this and update the streamers and description version?
    Fed9UAdcControls _adcControls[CHANNELS_PER_FEUNIT/2]; //!< Array to contain the 6 ADC settings for the FE unit channel pairs. note the pairs are not the same pairs as with random seed and mask settings.
    bool _complement[CHANNELS_PER_FEUNIT];              //!< Array to contain the complement flag settings for the FE unit channels.
    
    bool _apvDisable[APVS_PER_FEUNIT];                  //!< Array to contain the APV disable flag settings for the FE unit APVs.
    //<GR date=27/07/2006> Added APV fake event disable
    bool _apvFakeEventDisable[APVS_PER_FEUNIT];         //!< Array to contain the APV fake event disable flag settings for the FE unit APVs.
    //</GR>
    u16 _optoRxOffset;                                  //!< Array to contain the OptoRx offset settings for the FE unit.
    u16 _optoRxCapacitor;                               //!< Array to contain the OptoRx capacitor settings for the FE unit.
    u16 _medianOverride[APVS_PER_FEUNIT];               //!< Array to contain the common mode median settings for the FE unit APVs.
    bool _medianOverrideDisable;                        //!< Array to contain the common mode median override disable flag settings for the FE unit.
    bool _feUnitDisable;                                //!< Array to contain the FE unit disable flag settings for the FE unit.
    Fed9UTempControl _tempControl;                      //!< Array to contain the temperature monitor control settings for the FE unit.
    u16 _fakeEventRandomSeed[CHANNELS_PER_FEUNIT/2];    //!< Array to contain the fake event random seed settings for the FE unit channel pairs.
    u16 _fakeEventRandomMask[CHANNELS_PER_FEUNIT/2];    //!< Array to contain the fake event random mask settings for the FE unit channel pairs.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r);
    // </NAC>
    
  };

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  Overloaded stream operator to allow Fed9UFrontEndDescriptions to be saved to a stream
   * \param  os Output stream to save the object to.
   * \param  desc Fed9UFrontEndDescription object to save to the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UFrontEndDescription& desc);

  /**
   * \brief  Overloaded stream operator to allow Fed9UFrontEndDescriptions to be read from a stream
   * \param  is Input stream to create an object from.
   * \param  desc Fed9UFrontEndDescription object to load from the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::istream& operator>>(std::istream& is, Fed9UFrontEndDescription& desc);

  /**
   * \brief  Overloaded stream operator to allow Fed9UAdcControls to be saved to a stream
   * \param  os Output stream to save the object to.
   * \param  cont Fed9UAdcControls object to save to the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UAdcControls& cont);

  /**
   * \brief  Overloaded stream operator to allow Fed9UAdcControlss to be read from a stream
   * \param  is Input stream to create an object from.
   * \param  cont Fed9UAdcControls object to load from the stream.
   * \return ostream Reference to the stream the data was written to.
   */
  std::istream& operator>>(std::istream& is, Fed9UAdcControls& cont);
  
  // <NAC date="24/04/2007"> operator to compare front end descriptions
  /**
   * \brief  == operator to allow Fed9UFrontEndDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r);
  /**
   * \brief  != operator to allow Fed9UFrontEndDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r) { return !(l==r); }
  /**
   * \brief  != operator to allow Fed9UAdcControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UAdcControls& l, const Fed9UAdcControls& r) { return !(l==r); }
  // </NAC>
  
}

#endif // H_Fed9UFrontEndDescription
