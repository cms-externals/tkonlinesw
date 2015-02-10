#ifndef H_Fed9UDescription
#define H_Fed9UDescription

/////////////////////////////////////////////////////////////////
//                                                             //
//   FED 9U Description Class   : Fed9UDescription             //
//                                                             //
//  Author: Matt Noy, Jonathan Fulcher, Emlyn Corrin  IC       //
//  date: 19/06/2003                                           //
//  notes:                                                     //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////

// <NAC date="26/06/2007">
#include "Fed9UDescriptionException.hh"
// </NAC>

#include "Fed9UFrontEndDescription.hh"
#include "Fed9UStrips.hh"
#include "Fed9UTtcrxDescription.hh"
#include "Fed9UEpromDescription.hh"
#include "Fed9UVoltageControl.hh"

#include "Fed9UAddress.hh"
#include "TypeDefs.hh"
#include "Fed9UABC.hh"

#include <string>
#include <vector>
#include <iosfwd>
#include <cstring>

// This version number must be incremented whenever
// a change is made to the description strage format:
#define DESCRIPTION_VERSION 22

#define VERSION_TEXT(name) ("Fed9U" #name "Version" STRINGIZE(DESCRIPTION_VERSION))

namespace Fed9U {

  /**
   * \brief  Description of a whole FED.
   * \author Matt Noy, Jonathan Fulcher, Emlyn Corrin  IC
   * \date   19/06/2003
   *
   * The FED Configuration is contained within the class Fed9UDescription. 
   * This class contains all data which is required in order to configure the FED
   * for normal running. Only those registers in the FED who's settings persist during 
   * run time are included here. Registers such as status and counters which 
   * are constantly changing are not included. The basic concept is whatever is 
   * set in the FED description that is then passed to the Fed9UVmeDevice, will mirror
   * what is in the FED after it has been initialized with this object.
   * Therefore during run time, the FED should always contain settings which match a local
   * copy of the FED description. Unless an register has been explicitly changed via some 
   * external interface. Therefore it is recommended that you maintain a local copy of the
   * FED description whenever you instantiate a Fed9UVmeDevice and make sure that it 
   * always mirrors the settings in the FED proper. To do this you should adopt an 
   * approach whereby the Settings that you wish to edit on the FED are first set in the
   * local description, and then transferred to the FED, either by individually writing to
   * the specific register, or by uploading the entire description to the FED.
   */

  class Fed9UDescription  : public Fed9UABC {
  public:
 
    /**
     * \name Constructors
     *
     * Constructors and destructor that are provided by the class.
     */
    //@{

    /**
     * This method replaces the default constructor, but does not perform any initialization
     * of the description. After constructing a FED description, it will be undefined. In
     * order to define settings one must call loadDefaultDescription(), or loadDescription(istream&).
     *
     ********************************************/
    Fed9UDescription();
  
    /**
     * This method replaces the default distructor, but does nothing.
     *
     ********************************************/
    ~Fed9UDescription();
    
    //@}

    /**
     * \name Description settings file setters/getters.
     *
     * Used to load or save the settings contained in a Fed9UDescription to/from a file stream.
     */
    //@{

    /**
     * \brief This method loads the FED description from a stream that is passed into the method.
     * \param std::istream Input stream from which the file is to be loaded.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * description to the source of the stream using saveDescription(std::ostream&).
     */
    void loadDescription(std::istream&);

    /**
     * \brief This method loads the FED settings, which are a subsection of the FED description
     *        from a stream that is passed into the method.
     * \param std::istream Input stream from which the settings are to be loaded.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * settings to the source of the stream using saveSettings(std::ostream&).
     */
    void loadSettings(std::istream&);

    /**
     * \brief This method loads the strips from a stream that is passed into the method.
     * \param std::istream Input stream containing the strip settings.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * strips to the source of the stream using saveStrips(std::ostream&).
     */
    void loadStrips(std::istream&);
    
    /**
     * \brief This method saves the FED description to a stream that is passed into the method.
     * \param Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * description passed into loadDescription(std::istream&).
     */
    void saveDescription(std::ostream&) const;


    /**
     * \brief This method saves the FED Settings to a stream that is passed into the method.
     * \param Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * Settings passed into loadSettings(std::istream&).
     */
    void saveSettings(std::ostream&) const;
    

    /**
     * \brief This method saves the Strips to a stream that is passed into the method.
     * \param std::ostream Output stream to save the settings to.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * strips passed into loadStrips(std::istream&).
     */
    void saveStrips(std::ostream&) const;
    
    /** 
     * \brief This method loads a default set of values into the FED description. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultDescription() { loadDefaultSettings(); loadDefaultStrips(); }
    
    /** 
     * \brief This method loads a default settings. 
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultSettings();
    
    /**
     * \brief This method loads a default set of strips values.
     *
     * This need only be used the first time you construct a fed descrition, all subsequent times you should
     * construct using the loadDescription(std::istream&) method
     */
    void loadDefaultStrips();


    /**
     * \brief This method clones an entire description
     * \return a pointer to the description cloned
     * \warning do not forget to delete the result once you have finished with it
     */
    Fed9UDescription* clone ( );
    

    //@}

    /** 
     * Setters for Fed9UDescription
     ********************************************/ 
    
    /**
     * Sets the bus Adaptor Type 
     *
     */
    Fed9UDescription& setBusAdaptorType(Fed9UHalBusAdaptor type);

    /**
     * Sets the version number for fed hardware 
     *
     */
    Fed9UDescription& setFedVersion(u32 version);
 
    /**
     * Sets the version number for eprom data
     *
     */
    Fed9UDescription& setEpromVersion(u32 version);
    
    /**
     * Sets the version number for fe firmware 
     *
     */
    Fed9UDescription& setFeFirmwareVersion(u32 version);

    /**
     * Sets the version number for be firmware 
     *
     */
    Fed9UDescription& setBeFirmwareVersion(u32 version);

    /**
     * Sets the version number for vme firmware 
     *
     */
    Fed9UDescription& setVmeFirmwareVersion(u32 version);
 
    /**
     * Sets the version number for delay firmware 
     *
     */
    Fed9UDescription& setDelayFirmwareVersion(u32 version);
 
    /**
     * gets the version number for fed  hardware
     *
     */
    u32 getEpromVersion() const;

     /**
     * gets the bus adaptor type
     *
     */
    Fed9UHalBusAdaptor getBusAdaptorType() const;
   
    /**
     * gets the version number for fed  hardware
     *
     */
    u32 getFedVersion() const;

    /**
     * gets the version number for fe Firmware 
     *
     */
    u32 getFeFirmwareVersion() const;

    /**
     * gets the version number for Be Firmware 
     *
     */
    u32 getBeFirmwareVersion() const;
    
    /**
     * gets the version number for Vme Firmware 
     *
     */
    u32 getVmeFirmwareVersion() const;

    /**
     * gets the version number for Delay Firmware 
     *
     */
    u32 getDelayFirmwareVersion() const;
  
    /**
     * \copydoc Fed9UABC::setDelay
     */
    Fed9UDescription& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay);
    
    /**
     * \copydoc Fed9UABC::setFineDelay
     */
    Fed9UDescription& setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay);
    
    /**
     * \copydoc Fed9UABC::setCoarseDelay
     */
    Fed9UDescription& setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay);

    /**
     * \copydoc Fed9UABC::setTrimDacOffset
     */
    Fed9UDescription& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset);
    
    
    /**
     * \copydoc Fed9UABC::setOptoRxInputOffset
     */
    Fed9UDescription& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 inputOffset);

    /**
     * \copydoc Fed9UABC::getOptoRxInputOffset
     */
     u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const;

    /**
     * \copydoc Fed9UABC::setOptoRxOutputOffset
     */
    Fed9UDescription& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 outputOffset);
    
    /**
     * \copydoc Fed9UABC::getOptoRxOutputOffset
     */
    u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const;

    /**
     * \copydoc Fed9UABC::setOptoRxCapacitor
     */
    Fed9UDescription& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 capacitor);
    
    /**
     * \copydoc Fed9UABC::getOptoRxCapacitor
     */
    u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const;


    Fed9UDescription& setApvDisable(const Fed9UAddress& fedApv, bool disable);
    
    //<GR date=27/07/2006> Added APV fake event disable
    /**
     * \brief  Removes an APV from a fake event by removing it tick marks.
     * \param  fedApv The FED APV number to be disabled in the fake event.
     * \param  disableFakeApv Boolean true disables the APV, false enables it.
     * \return Self reference
     */
    Fed9UDescription& setApvFakeEventDisable(Fed9UAddress fedApv, bool disable);
    //</GR>


    Fed9UDescription& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, std::vector<u16> medianOverride);
    

    Fed9UDescription& setScopeLength(u16 numberOfWords);
    Fed9UDescription& setFrameThreshold(const Fed9UAddress& channel, u16 channelThreshold);
    Fed9UDescription& setTriggerSource(Fed9UTrigSource triggerSource);    
    Fed9UDescription& setTestRegister(u32 testRegister);
    Fed9UDescription& setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable); 
    Fed9UDescription& setFedBeUnit(u16 beUnit);
    Fed9UDescription& setFedBeFpgaDisable(bool fedDisable);
    Fed9UDescription& setFedId(u16 fedId);
    Fed9UDescription& setFedHardwareId(u32 fedId);
    Fed9UDescription& setBeFpgaReadRoute(Fed9UReadRoute readRoute);
    Fed9UDescription& setAdcControls(const Fed9UAddress& channel, bool dfsen, bool dfsval, bool s1, bool s2);// TODO change from 4 bools to Strut input Fed9UAdcControls
    Fed9UDescription& setAdcControls(const Fed9UAddress& channel, const Fed9UAdcControls& adcControls);

    /**
     * \copydoc Fed9UABC::setDaqMode
     */
    Fed9UDescription& setDaqMode(Fed9UDaqMode fed9UDaqMode);

    /**
     * \copydoc Fed9UABC::setDaqSuperMode
     */
    Fed9UDescription& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode);

    /**
     * \copydoc Fed9UABC::setClock
     */
    Fed9UDescription& setClock(Fed9UClockSource clockSelect);

    /**
     * \copydoc Fed9UABC::setTempControl
     */
    Fed9UDescription& setTempControl(const Fed9UAddress& fedFpga, const Fed9UTempControl& tempControl);
    
    /**
     * \copydoc Fed9UABC::setComplement
     */
    Fed9UDescription& setComplement(const Fed9UAddress& channel, bool disable);
    
    /**
     * Updates the TTCrx description.
     *****************************************************/
    Fed9UDescription& setTtcrx(const Fed9UTtcrxDescription& ttcrxDescription);
    
    /**
     * This holds a copy of the serial EPROM memory.
     *****************************************************/
    Fed9UDescription& setEprom(const Fed9UEpromDescription& epromDescription);

    /**
     * This is used to configure the voltage monitor. It holds all the current read/write settings on the
     * voltage controller.
     *****************************************************/
    Fed9UDescription& setVoltageMonitor(const Fed9UVoltageControl& voltageController);

    /**
     * Getters for Fed9UDescription
     *****************************************/
     u16 getFineDelay(const Fed9UAddress& fedChannel) const;
     u16 getCoarseDelay(const Fed9UAddress& fedChannel) const;
     u16 getTrimDacOffset(const Fed9UAddress& fedChannel) const;
     bool getApvDisable(const Fed9UAddress& fedApv) const;

    //<GR date=27/07/2006> Added APV fake event disable
    /**
     * \brief  Returns the state of the disable APV from fake event setting.
     * \param  fedApv The FED APV number who's state we wish to know.
     * \return bool Boolean true means the APV is disabled, false it is enabled.
     */
    bool getApvFakeEventDisable(Fed9UAddress fedApv) const;
    //</GR>

    u32 getAllApvDisables(const Fed9UAddress& fedFeUnit) const;
    u16 getFakeEventRandomSeed(const Fed9UAddress& fedChannelPair) const;
    u16 getFakeEventRandomMask(const Fed9UAddress& fedChannelPair) const;

    //<JRF date="13/03/2008"> added comments
    /**
     * \brief  Sets the value for the fake event random seed, which is used to generate the random offsets for this channel pair
     * \param  fedChannelPair, the channel pair of which we are setting the random seed.
     * \param u16 value the value of the random seed.
     * \return self reference to the Fed9UDescription Object 
     */
    Fed9UDescription & setFakeEventRandomSeed(Fed9UAddress& fedChannelPair, u16 value);

    Fed9UDescription & setFakeEventRandomMask(Fed9UAddress& fedChannelPair, u16 value);

    /************************************************************
     * This will read the common mode (CM) median override settings on a FE FPGA.
     *
     * The CM median override settings are determined by two settings. These are represented 
     * by the arguments medianDisable and medianValue. If the median override values are 
     * enabled then medianOverrideDisable will return a false, if they are disabled it will 
     * return true. If the median override values are enabled (medianOverrideDisable == false) 
     * then medianValue will return the override values used by the FED, this has a maximum 
     * value of 1023.
     ************************************************************/
    
    std::vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const;

    u16 getScopeLength() const;
    u16 getFrameThreshold(const Fed9UAddress& channel) const;
    Fed9UStrips & getFedStrips() { return _strips; }
    const Fed9UStrips & getFedStrips() const { return _strips; }
    Fed9UTrigSource getTriggerSource() const;
    u32 getTestRegister() const;
    bool getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const;
    u32 getFedBeUnit() const;
    bool getFedBeFpgaDisable() const;
    u16 getFedId() const;
    u32 getFedHardwareId() const;


    Fed9UReadRoute getBeFpgaReadRoute() const;
    Fed9UAdcControls getAdcControls(const Fed9UAddress& channel) const;
    Fed9UDaqMode getDaqMode() const;
    Fed9UDaqSuperMode getDaqSuperMode() const;
    Fed9UClockSource getClock() const; 
    Fed9UTempControl getTempControl(const Fed9UAddress& fedFpga) const;
    bool getComplement(const Fed9UAddress& channel) const;

    /**
     * Returns all the current settings on the TTCrx device.
     */
    Fed9UTtcrxDescription getTtcrx() const;

    /**
     * Returns a copy of the 16KB serial EPROM memory stored on the FED.
     */
    Fed9UEpromDescription getEprom() const;

    /**
     * This returns a Fed9UVoltageMonitorInfo which contains all the current settings
     * on the voltage monitor.
     */
    Fed9UVoltageControl getVoltageMonitor() const;
    
    // <NAC date="25/04/2007>
    /**
     * \brief  Get one of the Fed9UFrontEndDescriptions.
     * \param  addr The address of the FE FPGA to return the description for.
     * \return Fed9UFrontEndDescription Description of the FE components linked
     *         to the specified FE FPGA.
     */
    Fed9UFrontEndDescription getFrontEndDescription(const Fed9UAddress& addr) const;
    /**
     * \brief  Set one of the Fed9UFrontEndDescriptions.
     * \param  addr The address of the FE FPGA to set the description for.
     * \param  theNewFrontEndDescription New description.
     * \return self reference
     */
    Fed9UDescription& setFrontEndDescription(const Fed9UAddress& addr, const Fed9UFrontEndDescription& theNewFrontEndDescription);
    // </NAC>

    /**
     * \name FED location parameters
     *
     * These parameters are required to ensure that the communication with the correct
     * FED is established. A real FED does not know its physical location and as such
     * this must be stored in the Fed9UDescription.
     */
    //@{

    /**
     * \copydoc Fed9UABC::setBaseAddress
     * \param   value Sets the value of the base address.
     */
    Fed9UDescription& setBaseAddress(const u32 value) { _baseAddress = value; return *this; }

    /**
     * \copydoc Fed9UABC::getBaseAddress
     */
    u32 getBaseAddress() const { return _baseAddress; }

    /**
     * \copydoc Fed9ABC::SetCrateNumber
     * \param   crateNumber Used to set the new crate number.
     */
    Fed9UDescription& setCrateNumber(u16 crateNumber);

    /**
     * \copydoc Fed9ABC::SetCrateNumber
     * \param   vmeContrllerDaisychainId Used to set the new daisyChainId.
     */
    Fed9UDescription& setVmeControllerDaisyChainId(u16 daisyChainId);

    /**
     * \copydoc Fed9UABC::getCrateNumber
     */
     u16 getCrateNumber() const;

    /**
     * \copydoc Fed9UABC::getVmeControllerDaisyChainId
     */
     u16 getVmeControllerDaisyChainId() const;

    /**
     * \copydoc Fed9UABC::setSlotNumber
     * \param   slotNumber Parameter that is used to pass the new slot number to be written.
     */
    Fed9UDescription& setSlotNumber(u8 slotNumber);

    /**
     * \copydoc Fed9UABC::getSlotNumber
     */
    u8 getSlotNumber() const;

    //@}



    /**
     *  Functions to be added. 20th October. 
     *
     * Do not use \deprecated Do not use these functions, 
     * they don't do anything, just allows the build to succeed 
     */
    

    
    //enumNamehere represents the register address but it is currently unknown what they are. this is a BE Fpga command only and so does not need a Fed9UAddress
    //void setTTCRXControl(u32 enumNameHere, u32 timingData) {}
    //u32 getTTCRXControl(u32 enumNameHere) {return 0;}
    
    Fed9UDescription& setName(std::string name);

     std::string getName() const { return std::string(_name);}
     std::string getVersion() const { return std::string(VERSION_TEXT(Description)); }
   /** End of new functions */
    
     Fed9UDescription & setHalAddressTable(const std::string val);

    //<JEC date=27/9/2007>
    //    const std::string & getHalAddressTable() const;
    std::string getHalAddressTable() const;
    //</JEC>

    //<JF date=18/03/2004>
    //Add get required buffer size.
    u32 getRequiredBufferSize() const;
    //</JF>      


  private:
    Fed9UDescription& setOptoRxOffset(const Fed9UAddress& fedFeUnit, u16 value);
    u16 getOptoRxOffset(const Fed9UAddress& fedFeUnit) const;
 
  public:
    // these methods should probably be private, but they are being used in the description creator.
    u16 getMedianOverride(const Fed9UAddress& fedApv) const;
    bool getMedianOverrideDisable(const Fed9UAddress& fedFeUnit) const;
    Fed9UDescription& setMedianOverride(const Fed9UAddress& fedApv, u16 medianOverride);
    
    /***********************************************************
     * This is used to either enable the common mode (CM) median override values or to disable them.
     *
     * The CM median override has a maximum value of 1023. Enabling the CM median override with no value
     * will effectivily disable the CM median subtraction. To enable the median override register
     * medianOverrrideDisable must be set to false. To disable them it must be set to true. 
     ***********************************************************/
    Fed9UDescription& setMedianOverrideDisable(const Fed9UAddress& fedFeUnit, bool medianOverrideDisable);


    /*****************************************************
     * This method returns a reference to the description object itself.
     * The purpose of this is to implement the pure virtual method which maps to the 
     * method in the VME Device, which returns a reference to the local fed settings, 
     * this is so that once can call this method on the base abstract base class. 
     *
     ************************************************/
    Fed9UDescription& getFed9UVMEDeviceDescription() { return *this; }

    /**************************************************
     *
     * inline function to obtain the global fine skew 
     * value. thiis is the amount by which all the 
     * delays at the fed front end are shifted. 
     *
     ************************************************/
    inline u16 getGlobalFineSkew()const{return _globalFineSkew;}
    
    /**************************************************
     *
     * setter, see getGlobalFineSkew(). dwisott.
     *
     ************************************************/
    inline Fed9UDescription & setGlobalFineSkew(u16 gs){
      _globalFineSkew=gs;
      return *this;
    }

    /**************************************************
     *
     * inline function to obtain the global coarse skew 
     * value. thiis is the amount by which all the 
     * delays at the fed front end are shifted. 
     *
     ************************************************/
    inline u16 getGlobalCoarseSkew()const{return _globalCoarseSkew;}
    
    /**************************************************
     *
     * setter, see getGlobalCoarseSkew(). dwisott.
     *
     ************************************************/
    inline Fed9UDescription & setGlobalCoarseSkew(u16 gs){
      _globalCoarseSkew=gs;
      return *this;
    }

    /**************************************************
     *
     * inline function to obtain the optorx resistor value should be 62 in V2 Feds
     *
     ************************************************/
    inline u16 getOptoRXResistor()const{return _optoRXResistor;}
    
    /**************************************************
     *
     * setter, see setOptoRxResistor(). sets the value for the RLOAD resistor should be 62 in V2 Feds
     *
     ************************************************/
    inline Fed9UDescription & setOptoRXResistor(u16 value){
      _optoRXResistor=value;
      return *this;
    }
    
    /**************************************************
     *
     * getter, gets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    std::string getFakeEventFile() const {
      return std::string(_fakeEventFile);
    }
    
    /**************************************************
     *
     * setter, sets the name of the Fake Event File from which the fake event data will be loaded 
     *
     ************************************************/
    inline Fed9UDescription & setFakeEventFile(std::string value){
      memcpy ( &(_fakeEventFile[0]),value.c_str(),(value.size()>200)?200:value.size());
      //      _fakeEventFile=value.c_str();
      //std::cout << __PRETTY_FUNCTION__ << "fake event file name being set to " << value << " and the array holds" << _fakeEventFile << std::endl;
      return *this;
    }

    u16 getFakeEventTriggerDelay() const {
      return _fakeEventTriggerDelay;
    }

    inline Fed9UDescription & setFakeEventTriggerDelay(u16 value) {
      _fakeEventTriggerDelay =  value;
      return *this;
    }
       
    //<JEC date=08/12/2005>
    //Added eventType to description
    /**
     * \copydoc Fed9UABC::setDaqEventType
     */
    Fed9UDescription& setDaqEventType(u16 eventType);

    /**
     * \copydoc Fed9UABC::getDaqEventType
     */
    u16 getDaqEventType(void) const;
    //</JEC>

    //<JEC date=08/12/2005>
    //Added DaqFov to description
    /**
     * \copydoc Fed9UABC::setDaqFov
     */
    Fed9UDescription& setDaqFov(u16 fov);

    /**
     * \copydoc Fed9UABC::getDaqFov
     */
    u16 getDaqFov(void) const;
    //</JEC>

    //<JEC date=09/01/2006>
    //Added TrackerHeaderMode to description
    /**
     * \copydoc Fed9UABC::getHeaderFormatType
     */
    Fed9UHeaderFormat getHeaderFormatType(void) const;

    /**
     * \copydoc Fed9UABC::setHeaderFormatType
     */
    Fed9UDescription& setHeaderFormatType(Fed9UHeaderFormat headerType);
    //</JEC>


    //<JEC date=23/02/06>
    //Added BunchCrossingOffset to description
    /**
     * \copydoc Fed9UABC::setBunchCrossingOffset
     */
    Fed9UDescription& setBunchCrossingOffset(u16 bxOffset);

    /**
     * \copydoc Fed9UABC::getBunchCrossingOffset
     */
    u16 getBunchCrossingOffset(void) const;
    //</JEC>


  private:
    //these methods are deprocated and will be removed soon
    u16 getAdcOffset(const Fed9UAddress& channel) const;
    Fed9UDescription& setAdcOffset(const Fed9UAddress& channel, u16 offset);
    Fed9UDescription& setChannelBufferOccupancy(const Fed9UAddress& channel, u16 channelOccupancy);
    u16 getChannelBufferOccupancy(const Fed9UAddress& channel) const;

  private:
    //<JEC date=26/9/2007>
    //    static std::string _fed9UAddressTable;
    char _fed9UAddressTable[200];
    //</JEC>
    template <typename T>
    void doLoop(Fed9UDescription & (Fed9UDescription::*setter)(const Fed9UAddress &, T), T value) {      std::string temp;

      Fed9UAddress a;
      for (int i = 0; i < FEUNITS_PER_FED; i++) {
	(this->*setter)(a.setFedFeUnit(i), value);
      }
    }
    template <typename T>
    void doLoop(Fed9UDescription & (Fed9UDescription::*setter)(const Fed9UAddress &, const T &), const T & value) {
      Fed9UAddress a;
      for (int i = 0; i < FEUNITS_PER_FED; i++) {
	(this->*setter)(a.setFedFeUnit(i), value);
      }
    }

    Fed9UHalBusAdaptor _busAdaptorType;
    u32 _feFirmwareVersion,_beFirmwareVersion, _vmeFirmwareVersion, _delayFirmwareVersion;
    u32 _fedVersion, _epromVersion;
    Fed9UStrips _strips; //
    Fed9UFrontEndDescription _feParams[FEUNITS_PER_FED];
    Fed9UTempControl _beTempControl;
    Fed9UTempControl _vmeTempControl;
    Fed9UTtcrxDescription _ttcrxDescription;
    Fed9UEpromDescription _epromDescription;
    Fed9UVoltageControl _voltageController;
    u32 _baseAddress;
    u16 _testRegister;//TOTO u32
    //TODO - u16 _beFpgaMode;
    u16 _beUnit; // TODO - deprocated - should remove this. Check with Gareth
    bool _fedDisable;
    u16 _fedId;
    u32 _fedHardwareId;
    Fed9UTrigSource _triggerSource; 
    Fed9UReadRoute _readRoute;
    Fed9UClockSource _clockMode;
    Fed9UDaqMode _fed9UDaqMode;
    Fed9UDaqSuperMode _fed9UDaqSuperMode;
    u16 _scopeLength;
    char _name[200];
    u16 _crateNumber;
    u16 _vmeControllerDaisyChainId;
    u16 _globalFineSkew;
    u16 _globalCoarseSkew;
    u16 _optoRXResistor;
    char _fakeEventFile[200];
    u16 _fakeEventTriggerDelay;
    u16 _eventType;
    u16 _fov;
    Fed9UHeaderFormat _headerType;
    u16 _bxOffset;
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UDescription& l, const Fed9UDescription& r);
  };
  
  // <NAC date="24/04/2007"> Operator to compare descriptions
  /**
   * \brief  == operator to allow Fed9UDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UDescription& l, const Fed9UDescription& r);
  /**
   * \brief  != operator to allow Fed9UDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UDescription& l, const Fed9UDescription& r) { return !(l==r); }
  // </NAC>
  
}

#endif // H_Fed9UDescription
