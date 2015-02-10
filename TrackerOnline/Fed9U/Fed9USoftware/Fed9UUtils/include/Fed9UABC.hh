#ifndef H_Fed9UABC
#define H_Fed9UABC

#include "TypeDefs.hh"
#include "Fed9UStrips.hh"
#include "Fed9UAddress.hh"
#include "ICExImpl.hh"
#include "Fed9UFrontEndDescription.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace Fed9U {

//using.*std::string;

  /**
   * \brief Abstract Base Class to a generic FED.
   * \author Jonathan Fulcher Matt Noy
   * \date Created 14/05/2004
   *
   * This class defines an interface to a generic FED device. It is used to ensure consistancy
   * between interfaces for real FEDs and software replicas.
   */
  class Fed9UABC {

  public:
    /**
     * \brief Class destructor.
     */
    virtual ~Fed9UABC() {}

    /**
     * \name Pure virtual methods.
     *
     * These methods are required to be implemented by any derived class, such that it provides
     * the minimum number of methods that will provide a complete interface to the FED.
     */
    //@{
  public:

    /**
     * \brief  Used to set the FED coarse and fine delay settings for a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  coarseDelay Number of fine steps to delay by. Range: 0-15.
     * \param  fineDelay Number of fine steps to delay by. Range: 0-24.
     * \return Self reference.
     *
     * A FED channel can be skewed by upto 400 nano seconds in nano second steps. The coarse delay
     * represents delays by whole bunch crossings (25ns) of the incoming channel data and the fine delay
     * integer values between coarse delay. Each fine delay value corresponds to a nano second, however
     * as it corresponds to the number of fine steps past a coarse setting it is limited to a
     * maximum of 24.
     */
    virtual Fed9UABC& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) = 0;

    /**
     * \brief  Used to set the fine delay on a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  fineDelay Number of fine steps to delay by. Range: 0-24.
     * \return Self reference.
     *
     * Each fine delay value corresponds to a nano second delay of the incoming channel data,
     * however as it provides the number of fine steps to delay between coarse delay settings
     * it is limit to a maximum of 24.
     */
    virtual Fed9UABC& setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay) = 0;

    /**
     * \brief  Used to set the coarse delay on a given FED channel.
     * \param  fedChannel Specifies the FED channel that the delay should be set for.
     * \param  coarseDelay Number of coarse steps to delay by. Range: 0-15.
     * \return Self reference.
     *
     * Each coarse delay value will delay the incoming channel data by whole bunch crossing (25ns)
     * steps.
     */
    virtual Fed9UABC& setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay) = 0;

    /**
     * \brief  Sets the amount by which to offset the output from the TrimDACs to the ADC inputs.
     * \param  fedChannel FED channel to which the offset is to be applied.
     * \param  offset Value of the offset to be set. Range: 0-255.
     * \return Self reference.
     */
    virtual Fed9UABC& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset) = 0;

    /**
     * \brief  Set the OptoRx input offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  inputOffset A value that specifies the offset of the input signal to the OptoRx.
     *         Range:0-15
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 inputOffset) = 0;

    /**
     * \brief  Returns the OptoRx input offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the input signal to the OptoRx.
     *         Range:0-15
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const = 0;


    /**
     * \brief  Set the OptoRx output offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  outputOffset A value that specifies the offset of the input signal to the OptoRx.
     *         Range: 0-3.
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 outputOffset) = 0;

    /**
     * \brief  Returns the OptoRx output offset.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the input signal to the OptoRx.
     *         Range: 0-3.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Set the OptoRx capacitor setting.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \param  capacitor A value that specifies the offset of the capacitor setting to the OptoRx.
     *         Range: 0-3.
     * \return Self reference.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual Fed9UABC& setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 capacitor) = 0;

    /**
     * \brief  Returns the OptoRx capacitor setting.
     * \param  feFpga Fed9UAddress object specifying the FE FPGA number to be set.
     * \return unsigned short A value that specifies the offset of the capacitor setting to the OptoRx.
     *         Range: 0-3.
     *
     * \todo   Figure out what these parameters mean.
     */
    virtual u16 getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Disables an APV from header dection and also removes its data from the output.
     * \param  fedApv The address of FED APV that should be enabled/disabled.
     * \param  disable States whether to disable, boolean true, or enable, boolean false, an APV.
     * \return Self reference.
     *
     * \todo check if anyone figured out if this worked.
     */
    virtual Fed9UABC& setApvDisable(const Fed9UAddress& fedApv, bool disable) = 0;

    /**
     * \brief  Disables an APV from header dection and also removes its data from the output.
     * \param  fedApv The address of FED APV that should be enabled/disabled.
     * \return bool States whether it is disable, boolean true, or enable, boolean false, an APV.
     */
    virtual bool getApvDisable(const Fed9UAddress& fedApv) const = 0;

    /**
     * \brief  Used to disable FED common mode median calculation and over ride with User defined value.
     * \param  fedFeUnit FE unit whose settings are to be modified.
     * \param  medianOverrideDisable Boolean true if the values are to be disabled, boolean false if enabled.
     * \param  medianOverride A vector containing 24 elements to set the common mode median values to on each
     *         APV. Range: 0-1023.
     * \return Self reference.
     */
    virtual Fed9UABC& setCmMedianOverride(Fed9UAddress fedFeUnit, bool medianOverrideDisable, std::vector<u16> medianOverride) = 0;

    /**
     * \brief  Used to disable FED common mode median calculation and over ride with User defined value.
     * \param  fedFeUnit FE unit whose settings are to be modified.
     * \param  medianOverrideDisable Boolean true if the values are to be disabled, boolean false if enabled. Value returned by reference.
     * \return std::vector<u16> A vector containing 24 elements containing the common mode median values to on each APV. Range: 0-1023.
     */
    virtual std::vector<u16> getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const = 0;

    /**
     * \brief  Sets the number of words of raw data that are written when running in scope mode.
     * \param  numberOfWords The number of 32 bit words to be written.
     * \return Self reference.
     */
    virtual Fed9UABC& setScopeLength(u16 numberOfWords) = 0;

    /**
     * \brief  Returns the number of words of raw data that are written when running in scope mode.
     * \return unsigned short The number of 32 bit words to be written.
     */
    virtual u16 getScopeLength() const = 0;

    /**
     * \brief  Sets the threshold values for the APV tick marks as seen by the tickers.
     * \param  fedChannel Channel on the FED that the threshold should address.
     * \param  channelThreshold Value of the threshold to be set. Range: 0-992, steps 32.
     * \return Self reference.
     *
     * The threshold value can only be set in steps of 32. The value given refers to the value
     * of the threshold from the 10bit (1023) ADC input. As it can only be set in steps of 32
     * any value that is not a multiple of 32 will be rounded to the nearest whole multiple of 32.
     */
    virtual Fed9UABC& setFrameThreshold(const Fed9UAddress& fedChannel, u16 channelThreshold) = 0;

    /**
     * \brief  Returns the threshold values for the APV tick marks as seen by the tickers.
     * \param  fedChannel Channel on the FED that the threshold should address.
     * \return unsigned short Value of the threshold to be set. Range: 0-992, steps 32.
     *
     * The threshold value can only be set in steps of 32. The value returned refers to the value
     * of the threshold from the 10bit (1023) ADC input. As it can only be set in steps of 32
     * any value that is not a multiple of 32 will be rounded to the nearest whole multiple of 32.
     */
    virtual u16 getFrameThreshold(const Fed9UAddress& channel) const = 0;

    /**
     * \brief  Sets the location of the trigger.
     * \param  triggerSource The source of the trigger, defined in the enum Fed9UTrigSource.
     * \return Self reference.
     */
    virtual Fed9UABC& setTriggerSource(Fed9UTrigSource triggerSource) = 0;    

    /**
     * \brief  Returns the location of the trigger.
     * \return Fed9UTrigSource The source of the trigger, defined in the enum Fed9UTrigSource.
     */
    virtual Fed9UTrigSource getTriggerSource() const = 0;

    /**
     * \brief  Used internal FED tests. No for use in normal operation.
     * \param  testRegister Value to be set to this register.
     * \return Self reference.
     *
     * More details can be found about this register in the BE FPGA documentation.
     */
    virtual Fed9UABC& setTestRegister(u32 testRegister) = 0;

    /**
     * \brief  Used internal FED tests. No for use in normal operation.
     * \return unsigned long Value set in this register.
     *
     * More details can be found about this register in the BE FPGA documentation.
     */
    virtual u32 getTestRegister() const = 0;

    /**
     * \brief  Disables the FE unit readout by the BE FPGA.
     * \param  fedFeUnit FE unit that is to be disabled.
     * \param  fedFeUnitDisable Boolean true disables the FE unit, boolean false enables it.
     * \return Self reference.
     *
     * When a FE unit is disabled their data is removed from the output event and are not stored
     * by the BE FPGA.
     */
    virtual Fed9UABC& setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable) = 0; 

    /**
     * \brief  Disables the FE unit readout by the BE FPGA.
     * \param  fedFeUnit FE unit that is to be disabled.
     * \return bool Boolean true disables the FE unit, boolean false enables it.
     *
     * When a FE unit is disabled their data is removed from the output event and are not stored
     * by the BE FPGA.
     */
    virtual bool getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const = 0;

    /**
     * \brief  Disables the FED from recognising and hence outputing events.
     * \param  fedDisable Boolean true will disable the FED, boolean false enable it.
     * \return Self reference.
     *
     * This command prevents the BE FPGA from seeing triggers and frames and hence it
     * will never see an event to form one. All data taken while the FED is disabled
     * is lost.
     */
    virtual Fed9UABC& setFedBeFpgaDisable(bool fedDisable) = 0;

    /**
     * \brief  Disables the FED from recognising and hence outputing events.
     * \return bool Boolean true will disable the FED, boolean false enable it.
     *
     * This command prevents the BE FPGA from seeing triggers and frames and hence it
     * will never see an event to form one. All data taken while the FED is disabled
     * is lost.
     */
    virtual bool getFedBeFpgaDisable() const = 0;

    /**
     * \brief  Something
     * \return Self reference.
     *
     * \todo ask Jonathan about the FED and hardware ID.
     */
    virtual Fed9UABC& setFedId(u16 fedId) = 0;

    /**
     * \todo ask Jonathan about this.
     */
    virtual u16 getFedId() const = 0;

    /**
     * \brief  Something
     * \return Self reference.
     *
     * \todo ask Jonathan about the FED and hardware ID.
     */
    virtual Fed9UABC& setFedHardwareId(u32 fedId) = 0;

    virtual u32 getFedHardwareId() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the FE FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setFeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the FE FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getFeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the BE FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setBeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the BE FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getBeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the VME FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setVmeFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the VME FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getVmeFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the delay FPGAs.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setDelayFirmwareVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the delay FPGAs.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getDelayFirmwareVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the EPROM.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     */
    virtual Fed9UABC& setEpromVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the EPROM.
     * \return unsigned long 32 bit firmware version number.
     */
    virtual u32 getEpromVersion() const = 0;

    /**
     * \brief  Used to set the firmware version in use by the FED.
     * \param  version 32 bit firmware version number.
     * \return Self reference.
     *
     * \todo figure out what the FED version is.
     */
    virtual Fed9UABC& setFedVersion(u32 version) = 0;

    /**
     * \brief  Returns the firmware version curreently in use by the FED.
     * \return unsigned long 32 bit firmware version number.
     *
     * \todo figure out what the FED version is.
     */
    virtual u32 getFedVersion() const = 0;

    /**
     * \brief  Sets data the readout route for the BE FPGA.
     * \param  readRoute Specifies the appropriate route to read the data from,
     *         as defined in Fed9UReadRoute enum.
     * \return Self reference.
     */
    virtual Fed9UABC& setBeFpgaReadRoute(Fed9UReadRoute readRoute) = 0;

    /**
     * \brief  Returns the data readout route for the BE FPGA.
     * \return Fed9UReadRout Specifies the route that the data will be read from as defined in Fed9UReadRoute enum.
     */
    virtual Fed9UReadRoute getBeFpgaReadRoute() const = 0;

    /**
     * \brief  Figure this out
     * \param  fedChannel FED channel that the ADC control bits should be updated on.
     * \param  adcControls Fed9UAdcControls is a data structure containing the four settings for the ADC.
     * \return Self reference.
     *
     * \todo   Figure out what the adc control bits do!
     */
    virtual Fed9UABC& setAdcControls(const Fed9UAddress& fedChannel, const Fed9UAdcControls& adcControls) = 0;

    /**
     * \brief  Figure this out
     * \param  fedChannel FED channel that the ADC control bits should be updated on.
     * \return Fed9UAdcControls A data structure containing the four settings for the ADC.
     *
     * \todo   Figure out what the adc control bits do!
     */
    virtual Fed9UAdcControls getAdcControls(const Fed9UAddress&) const = 0;

    /**
     * \brief  Sets the FED data acquisition mode.
     * \param  fed9UDaqMode The data acquisition mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqMode.
     * \return Self reference.
     *
     * This defines whether to find tick marks or frames and also level of processing the FED will perform on the event.
     */
    virtual Fed9UABC& setDaqMode(Fed9UDaqMode fed9UDaqMode) = 0;

    /**
     * \brief  Returns the FED data acquisition mode.
     * \return Fed9UDaqMode The data acquisition mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqMode.
     *
     * This defines whether to find tick marks or frames and also level of processing the FED will perform on the event.
     */
    virtual Fed9UDaqMode getDaqMode() const = 0;

    /**
     * \brief  Sets the super data acquisition mode.
     * \param  fed9UDaqSuperMode The super mode the FED should be set to. Permitted values are defined by the enum Fed9UDaqSuperMode.
     * \return Self reference.
     *
     * This adds to the existing data acquision mode. Any value of the super mode and normal DAQ
     * mode can be set, however not all combinations will have any effect. There are no incompatible modes.
     * A valid DAQ mode must be set in addition to this.
     */
    virtual Fed9UABC& setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode) = 0;

    /**
     * \brief  Returns the super data acquisition mode.
     * \return Fed9UDaqSuperMode The super mode of the FED. Permitted values are defined by the enum Fed9UDaqSuperMode.
     *
     * This adds to the existing data acquision mode. Any value of the super mode and normal DAQ
     * mode can be set, however not all combinations will have any effect. There are no incompatible modes.
     * A valid DAQ mode must be set in addition to this.
     */
    virtual Fed9UDaqSuperMode getDaqSuperMode() const = 0;

    /**
     * \brief  Determines the clock source used on the FED.
     * \param  clockSelect The clock to be used by the FED. Values are defined in the enum Fed9UClockSource.
     * \return Self reference.
     */
    virtual Fed9UABC& setClock(Fed9UClockSource clockSelect) = 0;

    /**
     * \brief  Returns the clock source in use on the FED.
     * \return The clock to be used by the FED. Values are defined in the enum Fed9UClockSource.
     */
    virtual Fed9UClockSource getClock() const = 0; 

    /**
     * \brief  Provides a means to control the LM82 temperature monitoring chips on each FPGA.
     * \param  fpga FPGA that the User wishes to communicate with. Accepts FE, BE and VME FPGA numbers.
     * \param  tempControl The class Fed9UTempControl contains all the parameters that can be set
     *         on the temperature chip. This \link Fed9U::Fed9UTempControl class\endlink should be consulted for further details.
     * \return Self reference.
     *
     * As the class Fed9UTempControl is taken as an argument all parameters for that temperature control
     * chip will be updated at once.
     */
    virtual Fed9UABC& setTempControl(const Fed9UAddress& fpga, const Fed9UTempControl& tempControl) = 0;

    /**
     * \brief  Provides a means to view all the read/write registers on the LM82 temperature monitoring chips on each FPGA.
     * \param  fpga FPGA that the User wishes to communicate with. Accepts FE, BE and VME FPGA numbers.
     * \return Fed9UTempControl The class Fed9UTempControl contains all the parameters that can be on the temperature chip.
     *         The return object contains all the current settings on these register. This \link Fed9U::Fed9UTempControl class\endlink
     *         should be consulted for further details.
     *
     * As this returns a Fed9UTempControl object all read/write parameters on the chip are returned at once.
     */
    virtual Fed9UTempControl getTempControl(const Fed9UAddress& fpga) const = 0;

    /**
     * \brief  Allows the User to complement the raw data that is received from the FED.
     * \param  fedChannel Channel on the FED that needs to be inverted.
     * \param  disable A boolean true will disable the complement and boolean false enable it.
     * \return Self reference.
     *
     * This should only be required if the inverter stage of an APV is turned off.
     */
    virtual Fed9UABC& setComplement(const Fed9UAddress& fedChannel, bool disable) = 0;


    /**
     * \brief  Allows the User to complement the raw data that is received from the FED.
     * \param  fedChannel Channel on the FED that needs to be inverted.
     * \return bool A boolean true will disable the complement and boolean false enable it.
     *
     * This setting does not necessarily mean the data has not been complemented at the APV inverter stage,
     * only that the FED is not complementing the data. By default this should be done and the complement at
     * FED level not required.
     */
    virtual bool getComplement(const Fed9UAddress& channel) const = 0;

    /**
     * \brief  Used by derived classes to return a copy of the Fed9UDescription, that is either a member object or a self reference.
     * \return Self reference.
     *
     * This method returns a reference to the description object itself.
     * The purpose of this is to implement the pure virtual method which maps to the 
     * method in the VME Device, which returns a reference to the local fed settings, 
     * this is so that once can call this method on the base abstract base class. 
     */
    virtual Fed9UABC& getFed9UVMEDeviceDescription() = 0;

    //<JEC date=08/12/2005>
    //Added eventType
    /**
     * \brief  Sets the type of run that is has been performed. This is saved in the event header.
     * \param  eventType They type of event that is being run with.\n
     *         Possible values are:
     *         \li 1	Physics trigger	Final OR (physics algorithms)
     *         \li 2	Calibration trigger Calibration cycle
     *         \li 3	Test trigger        Test cycle
     *         \li 4	Technical trigger   Technical trigger (external trigger)
     *         \li 5	Simulated events    Reserved for DAQ usage
     *         \li 6	Traced events	    Reserved for DAQ usage
     *         \li 7 	Error               Others undefined.
     * \return Self reference.
     * \todo probably want to create another enum for this.
     */
    virtual Fed9UABC& setDaqEventType(u16 eventType) = 0;

    /**
     * \brief  Returns the type of run that is has been performed. This is saved in the event header.
     * \return unsigned short They type of event that is being run with.\n
     *         Possible values are:
     *         \li 1	Physics trigger	Final OR (physics algorithms)
     *         \li 2	Calibration trigger Calibration cycle
     *         \li 3	Test trigger        Test cycle
     *         \li 4	Technical trigger   Technical trigger (external trigger)
     *         \li 5	Simulated events    Reserved for DAQ usage
     *         \li 6	Traced events	    Reserved for DAQ usage
     *         \li 7 	Error               Others undefined.
     * \todo probably want to create another enum for this.
     */
    virtual u16 getDaqEventType(void) const = 0;
    //</JEC>

    //<JEC date=08/12/2005>
    //Added DaqFov
    /**
     * \brief  Set the version format of the event, which is stored in the event header.
     * \param  fov This is the four bit version number of the event. Range: 0-7.
     * \return Self reference.
     * \todo   not certain about exactly what this is.
     */
    virtual Fed9UABC& setDaqFov(u16 fov) = 0;

    /**
     * \brief  Returns the version format of the event, which is also stored in the event header.
     * \return unsigned short This is the four bit version number of the event. Range: 0-7.
     * \todo   not certain about exactly what this is.
     */
    virtual u16 getDaqFov(void) const = 0;
    //</JEC>

    //<JEC date=23/02/2006>
    //Added bunch crossing offsets.
    /**
     * \brief  Applies an offset to the bunch crossing orbit.
     * \param  bxOffset The offset to be applied. Range: 0-4095.
     * \return Self reference.
     *
     * Affects the bunch crossing written in the FED register (see Fed9UVmeDevice::getBunchCount)
     * and also the number written in the FED data buffer.
     */
    virtual Fed9UABC& setBunchCrossingOffset(u16 bxOffset) = 0;

    /**
     * \brief  Returns the offset that has been applied to the bunch crossing orbit.
     * \return unsigned short The value of the bunch crossing orbit.
     *
     * Amount by which the bunch crossing counter is offset in the FED register (see Fed9UVmeDevice::getBunchCount)
     * and also the number written in the FED data buffer.
     */    
    virtual u16 getBunchCrossingOffset(void) const = 0;
    //</JEC>

    //<JEC date=09/01/2006>
    //Added tracker mode to description. 
    /**
     * \brief  Sets the header format type. It determines the format of the FED data buffer header.
     * \param  headerType The possible header types are defined in the enum Fed9UHeaderFormat.
     * \return Self reference.
     *
     * This number is found in both the DAQ and tracker special header.
     */
    virtual Fed9UABC& setHeaderFormatType(Fed9UHeaderFormat headerType) = 0;

    /**
     * \brief  Returns the type of header that will be present in an event.
     * \return Fed9UEventFormat An enum defined in TypeDefs.hh.
     *
     * The value defines what header type is present in an event and the value itself is also included.
     */
    virtual Fed9UHeaderFormat getHeaderFormatType(void) const = 0;
    //</JEC>

    /**
     * \brief  Returns a reference to a Fed9UStrips object. These contain the strip settings.
     * \return Fed9UStrips A class that contains all the strip settings, see Fed9U::Fed9UStrips for more details.
     */
    virtual Fed9UStrips & getFedStrips() = 0;
    //    virtual const Fed9UStrips & getFedStrips() = 0;

    /**
     * \brief  Provides the location of the Fed9U HAL address table, which provides a map of the FED VME FPGA registers.
     * \return string Contains the location of the HAL address table.
     */
    //<JEC date=27/9/2007>
    //    virtual const std::string & getHalAddressTable() const = 0;
    virtual std::string getHalAddressTable() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }
    //</JEC>
    //@}

    /**
     * \name Fed9UVmeDevice specific methods.
     *
     * These methods are required for part of a hardware FED implementation to provide easy to configuration methods. They are not
     * required by a software implementation and hence do not form part of the enforced interface.
     */

    //@{
    /**
     * \brief  Used upload all write settings to the FED from the Fed9UDescription object.
     * \param  bool If boolean true a reset is performed before any settings are updated and the clock value will be set if required.
     * \return Self reference.
     *
     * The strips settings, temperature and voltage monitor chips are not set up. There exist separate initialisation methods for these.
     * All APVs and ADCs are disabled by this method and start() must be called to enable data taking.
     */
    virtual Fed9UABC& init(bool) {return *this;}

    /**
     * \brief  Initialises all the FED strip registers as defined in the class Fed9UStrips. Only required in processed raw and zero suppression data modes.
     * \return Self reference.
     */
    virtual Fed9UABC& initStrips() { return *this;}
    //@}

    /**
     * \name Fed9UDescription specific methods.
     *
     * These methods are required in an implementation of a software based FED, but are not required by a real FED as no registers exist
     * for these settings.
     */

    //@{

    /**
     * \brief This method loads the FED description from a stream that is passed into the method.
     * \param istream& Input stream that the FED description is to be loaded from.
     *
     * The stream can be any stream, but it must have been created by previously saving the 
     * description to the source of the stream using saveDescription(ostream&).
     */
    virtual void loadDescription(std::istream&) {}

    /**
     * \brief This method saves the FED description to a stream that is passed into the method.
     * \param ostream& Output stream where the description settings are to be saved.
     *
     * The stream can be any stream and should have the same stream destination which is used to load the 
     * description passed into loadDescription(istream&).
     */
    virtual void saveDescription(std::ostream&) const {}

    /**
     * \brief This method loads a default set of values into the FED description.
     *
     * This need only be used the first time you construct a FED descrition, all subsequent times you should
     * construct using the loadDescription(istream&) method.
     */
    virtual void loadDefaultDescription(){}

    /**
     * /todo figure out what the name is for.
     */
    virtual Fed9UABC& setName(std::string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this; }
    virtual std::string getName() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }

    /**
     * \brief  FED description objects contain a version to ensure that only an appropriately saved file can be used to load a FED description object.
     * \return string Contains the version in use.
     */
    virtual std::string getVersion() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return std::string(" "); }

    /**
     * \brief  Required to construct a valid FED object as it contains the address map of the VME FPGA.
     * \param  string Location of the HAL address table.
     * \return Self reference.
     */
    virtual Fed9UABC & setHalAddressTable(const std::string) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}
    /**
     * \brief  Returns the APV disabled settings for all APVs on a FE unit.
     * \return unsigned long The lowest 24 bits contain the APV disable settings. A 1 signifies disabled and a 0 enabled.
     *         The highest of these 24 bits contains FE unit APV 24.
     *
     * \todo check numbering scheme (internal or external).
     */
    virtual u32 getAllApvDisables(const Fed9UAddress&) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0;}

    /**
     * \brief  Returns the maximum buffer size that could be required by the Fed9UVmeDevice::getCompleteEvent method.
     * \return unsigned long Maximum possible required buffer size in number of 32 bit words.
     * 
     * Uses the current description values to esimate what this should be
     */
    virtual u32 getRequiredBufferSize() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }

    /**
     * \brief  Returns the value of the fine delay in use by a FED channel.
     * \param  Fed9UAddress FED channel number that need to be set.
     * \return unsigned short Value of the fine delay setting.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getFineDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }

    /**
     * \brief  Returns the value of the coarse delay in use by a FED channel.
     * \param  Fed9UAddress FED channel number that need to be set.
     * \return unsigned short Value of the coarse delay setting.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getCoarseDelay(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }
    /**
     * \brief  Returns the amount by which to offset the output from the TrimDACs to the ADC inputs.
     * \param  fedChannel FED channel to which the offset is to be applied.
     * \return unsigned short Value of the offset to be set. Range: 0-255.
     *
     * This is a write only register on the FED.
     */
    virtual u16 getTrimDacOffset(const Fed9UAddress&) const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}

    //@}

    /**
     * \name FED location parameters
     *
     * The following methods are required to talk to a FED in a crate.
     * They have no register on a FED and are only required during construction of a
     * FED object. They will however, be required by Fed9UDescription to identify which
     * FED it contains the settings for.
     */
    //@{

    /**
     * \brief Set the base address of a FED in the crate.
     * \return Self reference.
     *
     * The slot number and base address are two different ways of expressing the same number
     * and as such only one should need be set.
     */
    virtual Fed9UABC& setBaseAddress(const u32) { ICUTILS_VERIFY(false).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Returns the base of the FED in the crate.
     * \return u32 Contains the value of the FED base address.
     */
    virtual u32 getBaseAddress() { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error();  return 0;}

    /**
     * \brief  Number of the crate that the FED is located in.
     * \return Self reference.
     */
    virtual Fed9UABC& setCrateNumber(u16) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Used to determine the crate number in which the FED is located.
     * \return u16 The number of the crate.
     *
     * A hardware FED does not know its physical location so these parameters must be retrieved from software.
     */
    virtual u16 getCrateNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; } 

    /**
     * \brief  ID of the Vme Controller in the Daisy Chain.
     * \return Self reference.
     */
    virtual Fed9UABC& setVmeControllerDaisyChainId(u16) { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Used to determine the Id of the VME Controller in the Daisy Chain
     * \return u16 The number of the crate.
     *
     * A hardware FED does not know its physical location so these parameters must be retrieved from software.
     */
    virtual u16 getVmeControllerDaisyChainId() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; } 

    /**
     * \brief  Slot number in which the FED is located.
     * \return Self reference.
     *
     * The slot number and base address are two different ways of expressing the same number
     * and as such only one should need be set.
     */
    virtual Fed9UABC& setSlotNumber(u8){ ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return *this;}

    /**
     * \brief  Returns the slot that the FED is located in.
     * \return u8 Slot that FED is located in.
     */
    virtual u8 getSlotNumber() const { ICUTILS_VERIFY(0).msg("Method unimplemented in base class. Implement in derived class").error(); return 0; }    //@}

  };//class Fed9UABC


}//namespace Fed9U

#endif // H_Fed9UABC
