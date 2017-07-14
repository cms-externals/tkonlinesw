#include "Fed9UUtils.hh"
#include "Fed9UDeviceFactory.hh"
// #####################################################################################
// # Copyright (c) 2004 Imperial College London, Rutherford Appleton Laboratory        #
// # (Jonathan Fulcher, Emlyn Corrin, Matthew Pearson, Gareth Rogers, Matt Noy)        #
// #                                                                                   #
// # Permission is hereby granted, free of charge, to any person obtaining a copy      #
// # of this software and associated documentation files (the "Software"), to use      #
// # and modify for personal use only. No rights are given to deal in the Software,    #
// # publish, distribute, sublicense, and/or sell copies of the Software.              #
// # Support will be available from the authors to anyone using unmodified versions    #
// # of the Software. If the user modifies any part of the Software, no liability      #
// # is accepted on the part of the author and no support will be hitherto available.  #
// #                                                                                   #
// # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
// # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
// # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
// # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, #
// # WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN   #
// # CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
// #####################################################################################
#ifndef _Fed9UVmeDeviceException_H_
#define _Fed9UVmeDeviceException_H_
#include "ICExDecl.hh"

namespace Fed9U {

#define FED9U_VME_DEVICE_EXCEPTION_CODE_LIST \
  IC_DEF_ERROR(ERROR_FED9UVMEDEVICE, "An error has been thrown from Fed9UVmeDevice") \
  IC_DEF_ERROR(ERROR_INIT_FAILED, "The initialisation function has failed") \
  IC_DEF_ERROR(ERROR_START_FAILED, "The FED has failed to start") \
  IC_DEF_ERROR(ERROR_STOP_FAILED, "The FED has failed to stop") \
  IC_DEF_ERROR(ERROR_FED9UTTCRXDEVICE, "An error has been thrown from Fed9UTtcrxDevice") \
  IC_DEF_ERROR(ERROR_FED9UVOLTAGEMONITOR, "An error has been thrown from Fed9UVoltageMonitor") \
  IC_DEF_ERROR(ERROR_FED9USYSTEMACE, "An error has been thrown from Fed9USystemAce") \
  IC_DEF_ERROR(ERROR_CONFIGUREFED9U, "An error has been thrown from Fed9UVmeBase")

/**
 * \brief  Class for error handling within Fed9UVmeDevice.
 * \author Gareth Rogers, RAL
 * \date   2003/07
 *
 * This class provides a list of errors which can occur in the Fed9UVmeDevice and can also be thrown
 * as part of the exception type Fed9UVmeDeviceException.
 */
  IC_EXCEPTION_CLASS_BEGIN(Fed9UVmeDeviceException, FED9U_VME_DEVICE_EXCEPTION_CODE_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END
}

#endif//_Fed9UVmeDeviceException_H_
#ifndef _Fed9UVmeDevice_H_
#define _Fed9UVmeDevice_H_

#include "StopWatch.hh"

#include <memory>
#include <string>
#include <sstream>

//#include <iosfwd>
//#include <list>
#include <vector>
#include <csignal>

namespace Fed9U {

  //using namespace::HAL;
  using namespace std;
  class Fed9UAddress;
  class Fed9UEpromDescription;
#ifdef EVENT_STREAMLINE
  class Fed9UEventStreamLine;
  class Fed9UEventUnitStreamLine;
#else
  class Fed9UEvent;
#endif
  class Fed9UVoltageControl;
  class Fed9UVoltageControlInfo;
  class Fed9UTtcrxDescription;
  class Fed9UTtcrxDescriptionInfo;
  class Fed9UVmeBase;

  /**
   * \brief  Provides parameter initialisation/control, event readout/checking and register setting/getting
   *         for a single FED.
   *
   * \author Jonathan Fulcher, Jo Cole, Mathew Pearson, Gareth Rogers
   *
   * \date   Create June 2003
   *         Last modified July 2006
   *
   * Derived from Fed9UABC it provides an interface to the FED that is common to both
   * Fed9UDescription and this class. Control over individual registers is provided,
   * as well methods to configure all registers and establish the readout chain.
   * A local Fed9UDescription object is kept upto date with all FED run parameters.\n
   */
  class Fed9UVmeDevice : public Fed9UABC {


  public:

    /*******************************************************************************
     * These are the class constructors, destructor and member functions directly  *
     * related to class construction or destruction.                               *
     *******************************************************************************/

    /**
     * \brief Constructs a FED object from a Fed9UDescription object. Does not perform any
     *        FED initialisation, only consitancy checks.
     * \param description Contains all the initial settings that the FED
     *        is to be configured with. A local copy is made that this class will maintain.
     * \throw Fed9UVmeDeviceException. All exceptions from included classes are caught and
     *        converted to the this type, including any relevant information from this
     *        software method.
     * \sa    getFed9UDescription.
     */
    explicit Fed9UVmeDevice(const Fed9UDescription& description, u32 adaptorSlot = 0) noexcept(false);

    /**
     * \brief  Tidies up the FED memory. Deletes the Fed9UDescription object.
     * \sa     getFed9UDescription.
     *
     * The only reference to the Fed9UDescription object used by this FED is deleted.
     */
    ~Fed9UVmeDevice();

  private:

    /**
     * \name   Unimplementted constructors.
     * \brief  Assignment and copy contructors are not implemented.
     */
    //@{
    Fed9UVmeDevice(const Fed9UVmeDevice&);
    Fed9UVmeDevice& operator=(const Fed9UVmeDevice&);
    //@}
 

    /*******************************************************************************
     *           These are the commands for the Front End (FE) FPGAs               *
     *******************************************************************************/

  public:

    // <JRF date=16/02/2010>
    /**
     * \brief  sets the periodic counter for spy arming every N events.
     * \param  spyArmPeriod to set the value of N to arm on every Nth trigger
     */
    Fed9UVmeDevice& setSpyArmPeriod(const u32  spyArmPeriod) noexcept(false);

    u32 getSpyArmPeriod() noexcept(false);
 
    Fed9UVmeDevice& setSpyArmCommand(u8 spyArmCommand_)  noexcept(false);

    // <NAC date=01/03/2007>
    /**
     * \brief  Arms Spy Channel
     * \param  selectedDelayChip The address pointing to the delay chip to pass the 
     * \param  command to or broadcast to all delay chips on one FeUnit or whole FED. 
     */
    void armSpy(const Fed9UAddress& selectedDelayChip) noexcept(false);
    /**
     * \brief  Reads back Spy Channel data
     * \param  selectedDelayChip The address pointing to the delay chip to pass the 
     * \param  command. Must be a single delay chip (no broadcast).
     * \param  destBuffer Pointer to buffer into which data is copied. 
     *         Must have 1504 bytes space.
     */
    void fireSpy(const Fed9UAddress& selectedDelayChip, void* destBuffer) noexcept(false);
 
    // <NAC date="25/06/2009"> Added second spy L1A/QDR register commands
    /**
     * \brief  Reads back Latched QDR count at the time of the last TTC B channel spy arm command (warning: this is L1A count in some firmware versions)
     */
    u32 getSpyTotalEventCount() noexcept(false);

   /**
     * \brief  Reads back Latched L1A count at the time of the last TTC B channel spy arm command (warning: this is L1A count in some firmware versions)
     */
    u32 getSpyL1ACount() noexcept(false);
    // </NAC>

    u32 getTotalEventCount32() noexcept(false);

    u32 getTotalL1ACount32() noexcept(false);

    u32 getTotalEventOutCount() noexcept(false);

    u32 getFeFrameLengthCount() noexcept(false);

    u32 getDummyEventCount() noexcept(false);

    u32 getPayloadDownCount() noexcept(false);

    u32 getPayloadLength() noexcept(false);

    u64 getDebugRegTwo()  noexcept(false);



    /**
     * \brief  Verify hardware ID checks that the hardware ID in the FED matches that in the
     *         description supplied to the constructor of this class.
     * \return A self reference.
     */
    Fed9UVmeDevice& verifyHardwareId() noexcept(false);
    
    /**
     * \brief  Verify versions checks that the versions of hardware and firmware within the
     *         FED matches that in the description supplied to the constructor of this class
     * \return A self reference.
     */
    Fed9UVmeDevice& verifyVersions() noexcept(false);

    /**
     * \brief  Verifies that the firmware versions on the FED match that in the global firmware file.
     *         It will upload the global firmware version if necessary.
     */
    void verifySystemACEFirmwareVersions() noexcept(false);

    /**
     * \copydoc Fed9UABC::setOptoRxInputOffset
     * \throw   Fed9UVmeDeviceException Thrown in the case of an out of bounds error for the
     *          input parameters.
     */
    Fed9UVmeDevice& setOptoRxInputOffset(const Fed9UAddress& feFpga, u16 inputOffset) noexcept(false);

    /**
     * \copydoc Fed9UABC::getOptoRxInputOffset
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getOptoRxInputOffset(const Fed9UAddress& feFpga) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setOptoRxOutputOffset
     * \throw   Fed9UVmeDeviceException Thrown in the case of an out of bounds error for the
     *          input parameters.
     */
    Fed9UVmeDevice& setOptoRxOutputOffset(const Fed9UAddress& feFpga, u16 outputOffset) noexcept(false);

    /**
     * \copydoc Fed9UABC::getOptoRxOutputOffset
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getOptoRxOutputOffset(const Fed9UAddress& feFpga) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setOptoRxCapacitor
     * \throw   Fed9UVmeDeviceException Thrown in the case of an out of bounds error for the
     *          input parameters.
     */
    Fed9UVmeDevice& setOptoRxCapacitor(const Fed9UAddress& feFpga, u16 capacitor) noexcept(false);

    /**
     * \copydoc Fed9UABC::getOptoRxCapacitor
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getOptoRxCapacitor(const Fed9UAddress& feFpga) const noexcept(false);

    /**
     * \brief  Used to set the pedestal values on a given FED APV.
     * \param  fedApv The address of the FED APV for the pedestal values to be set to.
     * \param  pedestals A vector containing 128 strip pedestal values, each with a maximum
     *         value of 1023.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Verifies both the size of the vector and contents of each
     *        element to ensure that they are in bounds.
     * \sa setPedsAndDisabledStrips.
     *
     * Should be used in cases where only the pedestal values wish to be updated. If the disabled
     * strip settings are also to be set then setPedsAndDisabledStrips should be used.
     * The pedestal values are subtracted from the strip data during zero suppression mode.
     */
    Fed9UVmeDevice& setPedestals(const Fed9UAddress& fedApv, const std::vector<u16>& pedestals) noexcept(false);

    /**
     * \brief  Returns a vector containing the pedestal value of each of the 128 strips on an APV.
     * \param  fedApv Address of the FED APV channel that is to be read.
     * \return A vector of unsigned shorts that contain the 128 pedestal values.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     * \sa getPedsAndDisabledStrips.
     *
     * Both the pedestals and disabled strip settings can be returned simulataneously using
     * getPedsAndDisabledStrips. Pedestals are subtracted from the strip reading during zero
     * suppression mode.
     */
    std::vector<u16> getPedestals(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \brief  Used to set the disable values on a given FED APV.
     * \param  fedApv The address of the FED APV for the pedestal values to be set to.
     * \param  disabledStrips A vector containing 128 disabled strip settings, a boolean true
     *         will disable the strip and boolean false will enable the strip.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Verifies both the size of the vector and contents of each
     *        element to ensure that they are in bounds.
     * \sa setPedsAndStripData setNumberValidStrips.
     *
     * Should be used in cases where only the disabled strip values wish to be updated. If the pedestal
     * values are also to be set then setPedsAndDisabledStrips should be used. Disabled the strips
     * removes them from the common mode median calculation and also provides a flag to highlight an
     * invalid strip. It <b> does not </b> remove the strip data from the event.
     */
    Fed9UVmeDevice& setDisabledStrips(const Fed9UAddress& fedApv, const std::vector<bool>& disabledStrips) noexcept(false);

    /**
     * \brief  Returns a vector containing the disbale setting of each of the 128 strips on an APV.
     * \param  fedApv Address of the FED APV channel that is to be read.
     * \return A vector of unsigned shorts that contain the 128 disable settings.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     * \sa getPedsAndDisabledStrips.
     *
     * Both the pedestals and disabled strip settings can be returned simulataneously using
     * getPedsAndDisabledStrips. Used to flag strips that have bad data during a run.
     */
    std::vector<bool> getDisabledStrips(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \brief  Returns the number of valid strips used in the Common Mode Median calculation.
     * \param  fedApv Address of the APV on the FED whose valid strip number is to be set.
     * \return unsigned short Number of valid strips on that APV.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * This number is determined by the number of disabled strips and cannot be set on its own.
     */
    u16 getNumberOfValidStrips(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \brief  Sets the 128 pedestals for an APV and which strips should be excluded from the common mode calculation.
     * \param  fedApv The FED APV that is to have its pedestals and disabled strip settings set.
     * \param  pedestals Vector containing a element for each of the 128 strips on an APV. Each element contains a pedestal
     *         value for that strip. Range:0-1023.
     * \param  disableStrips Vector containing a element for each of the 128 strips on an APV. Each element contains a disable
     *         flag for that strip.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * The CM median calculated by the requires the number of valid strips in order to calculate the median. The number
     * of valid strips is set automatically depending on the number of strips set disabled by this method.
     */
    Fed9UVmeDevice& setPedsAndDisabledStrips(const Fed9UAddress& fedApv, const std::vector<u16>& pedestals, const std::vector<bool>& disabledStrips) noexcept(false);

    /** 
     * \brief  Gets the 128 pedestals and valid strip settings for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
     * \param  pedestals Returns a 128 element vector, with each element containing the pedestal value for that strip.
     * \param  disabledStrips Returns a 128 element vector, with each element containing the disable strip flag for a given strip.
     * \param  numberValidStrips Returns the number of valid strips that the FED uses to calculate the common mode median.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    const Fed9UVmeDevice& getPedsAndDisabledStrips(const Fed9UAddress& fedApv, std::vector<u16>& pedestals, std::vector<bool>& disabledStrips, u16& numberValidStrips) const noexcept(false);

    /**
     * \brief  Sets the 128 high cluster thresholds for an APV.
     * \param  highThresholds A vector containing 128 elements, each element corresponding a strip and containing the high threshold to be set.
     *         Range:0-255.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of cluster threshold can be set. The high thresholds are used to identify single strip clusters
     * in the cluster finding algorithm.
     */
    Fed9UVmeDevice& setHighClusterThresholds(const Fed9UAddress& fedApv, const std::vector<u16>& highThresholds) noexcept(false);

    /**
     * \brief  Returns the 128 high cluster thresholds for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
     * \return A vector containing 128 elements, each element containing the high cluster threshold for the corresponding strip.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of cluster threshold can be set. The high thresholds are used to identify single strip clusters
     * in the cluster finding algorithm.
     */
    std::vector<u16> getHighClusterThresholds(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \brief  Sets the 128 low cluster thresholds for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
     * \param  lowThresholds A vector containing 128 elements, each element corresponding a strip and containing the low threshold to be set.
     *         Range:0-255.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of cluster threshold can be set. The low thresholds are used to identify multiple strip clusters
     * in the cluster finding algorithm.
     */
    Fed9UVmeDevice& setLowClusterThresholds(const Fed9UAddress& fedApv, const std::vector<u16>& lowThresholds) noexcept(false);

    /**
     * \brief  Returns the 128 low cluster thresholds for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
     * \return A vector containing 128 elements, each element containing the low cluster threshold for the corresponding strip.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of cluster threshold can be set. The low thresholds are used to identify multiple strip clusters
     * in the cluster finding algorithm.
     */
    std::vector<u16> getLowClusterThresholds(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \brief  Sets the 128 cluster thresholds for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
     * \param  highThresholds A vector containing 128 elements, each element corresponding a strip and containing the high threshold to be set.
     *         Range:0-255.
     * \param  lowThresholds A vector containing 128 elements, each element corresponding a strip and containing the low threshold to be set.
     *         Range:0-255.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of clusters can be set. The high thresholds and the low thresholds. The high threshold
     * is to be used on single strip hits and low on multiple strip hits. The maximum threshold value is 254
     * for both high and low thresholds. If the threshold is set to 255 it is excluded from the cluster finding.
     */
    Fed9UVmeDevice& setClusterThresholds(const Fed9UAddress& fedApv, const std::vector<u16>& highThresholds, const std::vector<u16>& lowThresholds) noexcept(false);

    /**
     * \brief  Returns the 128 low cluster thresholds for an APV.
     * \param  fedApv Sets the FED APV to be addressed.
      * \param  highThresholds A vector containing 128 elements, each element corresponding a strip and containing the high threshold to be set.
     *         Range:0-255.
     * \param  lowThresholds A vector containing 128 elements, each element corresponding a strip and containing the low threshold to be set.
     *         Range:0-255.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Two types of clusters can be returned. The high thresholds and the low thresholds.
     * The high threshold is to be used on single strip hits and low on multiple
     * strip hits.
     */
    const Fed9UVmeDevice& getClusterThresholds(const Fed9UAddress& fedApv, std::vector<u16>& highThresholds, std::vector<u16>& lowThresholds) const noexcept(false);

    /**
     * \brief  Performs a soft reset on the FPGA.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * Performs a one clock cycle reset on a specified FE FPGA.
     */
    Fed9UVmeDevice& feFpgaSoftReset(const Fed9UAddress& fpga) noexcept(false);

    /**
     * \copydoc Fed9UABC::setAdcControls
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     *
     * As there are six ADCs per FE unit and twelve channels, one ADC must therefore control two channels.
     * Due to this when one FED channel is updated necessarily its partner will be updated too. The channels
     * parings are as follows:
     * li ADC 0 = Ch 0 & 3
     * li ADC 1 = Ch 1 & 2
     * li ADC 2 = Ch 4 & 7
     * li ADC 3 = Ch 5 & 6
     * li ADC 4 = Ch 8 & 11
     * li ADC 5 = Ch 9 & 10
     */
    Fed9UVmeDevice& setAdcControls(const Fed9UAddress& fedChannel, const Fed9UAdcControls& adcControlSettings) noexcept(false);

    /**
     * \copydoc Fed9UABC::getAdcControls
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UAdcControls getAdcControls(const Fed9UAddress& fedChannel) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setFineDelay
     * \throw   Fed9UVmeDeviceException Performs a range check on the fine delay setting and will throw
     *          the error if it is out of bounds.
     */
    Fed9UVmeDevice& setFineDelay(const Fed9UAddress& fedChannel, u16 fineDelay) noexcept(false);

    /**
     * \copydoc Fed9UABC::setCoarseDelay
     * \throw   Fed9UVmeDeviceException Performs a range check on the fine delay setting and will throw
     *          the error if it is out of bounds.
     */
    Fed9UVmeDevice& setCoarseDelay(const Fed9UAddress& fedChannel, u16 coarseDelay) noexcept(false);

    /**
     * \brief  Sets the nano second delay value required for a specific FED channel.
     * \param  fedChannel FED channel required to be delayed.
     * \param  nanoDelay Requested delay in nano seconds. Range: 0-399.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Verifies the nano second delay is with bounds, otherwise this exception
     *         is thrown.
     *
     * The delay in nano seconds is converted to fine and coarse delay values. As a result only whole values of
     * nano seconds are available.
     */
    Fed9UVmeDevice& setDelay(const Fed9UAddress& fedChannel, float nanoDelay) noexcept(false);

    /**
     * \copydoc Fed9UABC::setDelay
     * \throw   Fed9UVmeDeviceException Performs a range check on the fine delay setting and will throw
     *          the error if it is out of bounds.
     */
    Fed9UVmeDevice& setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) noexcept(false);

    /**
     * \copydoc Fed9UABC::setApvDisable
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setApvDisable(const Fed9UAddress& fedApv, bool apvDisable) noexcept(false);

    /**
     * \copydoc Fed9UABC::getApvDisable
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    bool getApvDisable(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setFrameThreshold
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setFrameThreshold(const Fed9UAddress& fedChannel, u16 threshOnChannel) noexcept(false);

    /**
     * \copydoc Fed9UABC::getFrameThreshold
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getFrameThreshold(const Fed9UAddress& fedChannel) const noexcept(false);

    /**
     * \copydoc Fed9UABC::getFedStrips
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UStrips & getFedStrips() noexcept(false);

    /**
     * \copydoc Fed9UABC::setCmMedianOverride
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setCmMedianOverride(Fed9UAddress feFpga, bool medianOverrideDisable, std::vector<u16> medianValue = std::vector<u16>(24,0)) noexcept(false);

    /**
     * \copydoc Fed9UABC::getCmMedianOverride
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    std::vector<u16> getCmMedianOverride(Fed9UAddress feFpga, bool& medianOverrideDisable) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setCmMedianOverrideValue
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setCmMedianOverrideValue(const Fed9UAddress& fedApv, u16 medianValue) noexcept(false);

    /**
     * \copydoc Fed9UABC::getCmMedianOverrideValue
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getCmMedianOverrideValue(const Fed9UAddress& fedApv) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setApvDisable
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setCmMedianOverrideDisable(const Fed9UAddress& feFpga, bool disable) noexcept(false);

    /**
     * \copydoc Fed9UABC::getCmMedianOverrideDisable
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    bool getCmMedianOverrideDisable(const Fed9UAddress& feFpga) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setScopeLength
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setScopeLength(u16 numberOfWords) noexcept(false);

    /**
     * \copydoc Fed9UABC::getScopeLength
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getScopeLength() const noexcept(false);

    /**
     * \copydoc Fed9UABC::setComplement
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& setComplement(const Fed9UAddress& fedChannel, bool disableComplement) noexcept(false);

    /**
     * \copydoc Fed9UABC::getComplement
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    bool getComplement(const Fed9UAddress& fedChannel) const noexcept(false);

    /**
     * \copydoc Fed9UABC::setTrimDacOffset
     * \throw   Fed9UVmeDeviceException Verifies the TrimDAC offset is with bounds, otherwise this exception
     *          is thrown.
     */
    Fed9UVmeDevice& setTrimDacOffset(const Fed9UAddress& fedChannel, u16 offset) noexcept(false);

    /**
     * \brief  Sends a reset to the trimDACs.
     * \return Self reference.
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    Fed9UVmeDevice& sendTrimDacReset(const Fed9UAddress& feFpga) noexcept(false);

    /**
     * \brief  Returns the FE event buffer occupancy on a FED channel.
     * \param  fedChannel Channel on the FED that is being addressed.
     * \return unsigned short.
     * \throw   Fed9UVmeDeviceException Catches all errors from internal method calls and
     *          rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u16 getChannelBufferOccupancy(const Fed9UAddress& fedChannel) const noexcept(false);

    /**
     * \brief  Returns the firmware ID that is in use on all the FE FPGAs.
     * \return u32.
     * \throw  Fed9UVmeDeviceException This exception is thrown if there is a different firmware version
     *         on one of the FE FPGAs compared to the 1st FE FPGA. Catches all exceptions from internal
     *         function calls and rethrows them as this type.
     */
    u32 getFeFirmwareId() const noexcept(false);

    /**
     * \brief  Returns the firmware ID that is in use on all the delay FPGAs.
     * \return u32.
     * \throw  Fed9UVmeDeviceException This exception is thrown if there is a different firmware version
     *         on one of the delay FPGAs compared to the 1st delay FPGA. Catches all exceptions from internal
     *         function calls and rethrows them as this type.
     */
    u32 getDelayFirmwareId() const noexcept(false);

    // <JEC date=30-05-06>
    /**
     * \brief  This command returns the number of triggers in the Fake Event FIFO.
     * \param  feFpga The FE FPGA that is to be addressed.
     * \return u32
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     */
    u32 getFeFakeEventTriggerFifoData(const Fed9UAddress& feFpga) const noexcept(false);
    // </JEC>

    //<JEC date=16/03/07>
    /** 
     * \brief  This method retrieves the prescale applied to the triggers coming in to the FE-FPGA.
     *         It returns the actual prescale factor, rather than the number of events that will be
     *         skipped (what is supplied by the firmware).
     *         Skip event Nth event means a prescale factor of N+1
     * \param  address of the FE-FPGA whose register is being accessed.
     * \todo   make it work !!
    */
    u16 getFEFrameCapturePrescale(const Fed9UAddress& feFpga) const noexcept(false);
    //</JEC>

    // <JEC date=10-04-06>
    /**
     * \brief  This method dumps out the contents of all read/write FE registers, for one or all of
     *	       the FE-FPGAs. Only those registers which are normally set up during initialisation
     *	       are dumped.
     * \param  selectedFpga FE FPGA to be addressed.
     * \param  os Output stream to dump the registers to.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     * \todo   Update to use the Fed9UAddress class.
     */
    void dumpFeRegisters(unsigned short selectedFpga, ostream &os, bool addHtml) noexcept(false);
    // </JEC>

    // <JEC date=10-05-06>
    /**
     * \brief  This method dumps out the contents of the setNumberValid, LoadThresh and LoadMedian FE registers
     *         for one or all of the FE-FPGAs.
     * \param  selectedFpga FE FPGA to be addressed.
     * \param  os Output stream to dump the registers to.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     * \todo   Update to use the Fed9UAddress class.
     */
    void dumpApvData(unsigned short selectedFpga, ostream &os, bool addHtml) noexcept(false);
    // </JEC>

    // <JEC date=10-05-06>
    /**
     * \brief  This method dumps the current strip settings, pedestals, clusters and disable flag to an ostream.
     *         This method is still under development.
     * \param  selectedFpga FE FPGA to be addressed.
     * \param  os Output stream to dump the registers to.
     * \throw  Fed9UVmeDeviceException Catches all errors from internal method calls and
     *         rethrown as a Fed9UVmeDeviceException appending relevant method information.
     * \todo   Update to use the Fed9UAddress class.
     */
    void dumpPedAndThreshData(unsigned short selectedFpga, ostream &os, bool addHtml) noexcept(false);
    // </JEC>

  private:

    /**
     * Used to create the strip data vectors that are required by the setStripData() method.
     *
     * This method will create vectors containing either the pedestals and valid strip settings
     * or the cluster finding threshold settings. If set threshold is true then the thresholds
     * are produced. If false the pedestals and valid strips are produced. The vectors contain
     * 512 strips of data, which corresponds to two channels of data. Each vector element
     * represents a strip. This method uses the FED logic system so a 1 will enable a strip
     * and a 0 will disable it.
     */
    Fed9UVmeDevice& createStripData(const Fed9UAddress& fedApv, const bool setThreshold, const std::vector<u16>& pedsOrHighThresh, const std::vector<u16>& valStripOrLowThresh ) noexcept(false);

    /**
     * Writes the strip data to the FED.
     *
     * This method will set either the pedestal and valid strip values or the threshold values
     * for four APVs. Each element of the vectors represents a different strip. The vectors can
     * contain two different sets of data depending on the value of setThreshold. If setThreshold
     * is true then the cluster threshold data is returned. False and the pedestal and valid
     * channel data is returned.
     */
    Fed9UVmeDevice& setStripData(const Fed9UAddress& fedApv, const bool setThreshold, const std::vector<u16>& pedsOrHighThresh, const std::vector<u16>& valStripOrLowThresh) noexcept(false);

    /**
     * This will return a 128 element vector that contains the pedestals and disable strip
     * settings on an APV.
     *
     * This should only be called from getStripData as it relies on the pedestal address
     * being set before this method is called.
     */
    const Fed9UVmeDevice& getPedsAndDisabledStripData(const Fed9UAddress& fedApv, std::vector<u16>& pedestals, std::vector<u16>& validStrips) const noexcept(false);

    /**
     * This will return a 128 element std::vector that contains the thresholds on an APV.
     *
     * This should only be called from getStripData as it relies on the pedestal address
     * being set before this method is called.
     */
    const Fed9UVmeDevice& getClusterData(const Fed9UAddress& fedApv, std::vector<u16>& highThresholds, std::vector<u16>& lowThresholds) const noexcept(false);

    /**
     * This will take a vector that contains four APVs of strips (512 elements) with
     * each element corresponding to a strip. The vector is the reordered so that the
     * elements are ordered to match the ordering of the strips as they are sent
     * from the APV to the FED.
     *
     * The input ordering is the same as that which is kept by the description and the
     * output ordering is the same as that coming into the FED. This method was written by
     * Mattew Pearson, RAL, 12/2003 and modified by Gareth Rogers, RAL, 01/2004.
     */
    Fed9UVmeDevice& disOrderingAlgorithm(const std::vector<u16>& stripOrderedInput, std::vector<u16>& apvOrderedOutput) noexcept(false);

    /**
     * This will take a vector that contains one APV of strips (128 elements) where
     * the element ordering matches that sent into the FED from an APV. It will
     * reorder the vector so that it is returned with its elements in the order that
     * is output by the FED after it has performed re-ordering.
     *
     * This method was written by Matthew Pearson, RAL, 12/2003.
     */
    const Fed9UVmeDevice& reOrderingAlgorithm(const std::vector<u16>& apvOrderedInput, std::vector<u16>& stripOrderedOutput) const noexcept(false);

    /**
     * This sets the number of strips that should be used in the median calculation
     * for an APV.
     *
     * The vector input contains 128 elements which represent the state of an APV's worth
     * of strips. A 1 represents disabled and a 0 enabled. Each element contains the
     * state of a strip. It must always be the same as the common-mode calculation values.
     */
    Fed9UVmeDevice& setNumberValidStrips(const Fed9UAddress& fedApv, const std::vector<bool>& disabledStrips) noexcept(false);

    /**
     * This gets the number of strips that should be used in the median calculation.
     *
     * The return value is the number strips that are enabled on an APV. It must always
     * be the same as the common-mode calculation values.
     */
    u16 getNumberValidStrips(const Fed9UAddress& fedApv) const noexcept(false);

    //<JEC date=16/03/07>
    /**
     * This sets the prescale for triggers coming into all FE-FPGAs.  It is a private member
     * because it must be used in conjunction with the corresponding BE-FPGA register.
     * All 8 FE-FPGAs are set simultaneously.
     */
    Fed9UVmeDevice & setFEFrameCapturePrescale(u16 prescale) noexcept(false);
    //</JEC>

    /*******************************************************************************
     *          These are the commands for the Back End (BE) FPGA                  *
     *******************************************************************************/

  public:

    /**
     * This allows you to set the trigger source for the BE FPGA.
     *
     * The values of the possible settings are defined in the Fed9UTrigSource enum
     * that can be found in TypeDefs.hh. The possible settings are TTC, backplane
     * or software triggers.
     */
    Fed9UVmeDevice& setTriggerSource(Fed9UTrigSource triggerSource) noexcept(false);

    /**
     * This returns the trigger source for the BE FPGA.
     *
     * The values of the possible settings are defined in the Fed9UTrigSource enum
     * that can be found in TypeDefs.hh. The possible settings are TTC, backplane
     * or software triggers.
     */
    Fed9UTrigSource getTriggerSource() const noexcept(false);

    /**
     * This will load all the read and write setting from the Fed9UTtcrxDescription
     * object to the TTCrx.
     */
    Fed9UVmeDevice& setTtcrx(const Fed9UTtcrxDescription& ttcrxDescription) noexcept(false);

    /**
     * Enable or disable the ttcrx serial b data path explicitly
     */
    Fed9UVmeDevice& setTtcrxSerialBDataEnable(bool value=true) noexcept(false);

    /**
     * Returns all the current read and writable settings on the TTCrx device.
     */
    Fed9UTtcrxDescription getTtcrx() const noexcept(false);

    /**
     * Returns all the current settings on the TTCrx device. Both the read only and read
     * and writable settings.
     */
    Fed9UTtcrxDescriptionInfo getTtcrxInfo() const noexcept(false);

    /**
     * Will reset the TTCrx chip.
     *
     * An internal reset uses the I2C interface to send the reset, an external reset
     * does not. Both perform the same level of reset. You cannot reset the TTCrx while
     * the TTC clock is selected. If internal is true then an internal reset will be
     * sent. If it is false then an external reset will be sent.
     */
    Fed9UVmeDevice& sendTtcrxReset(bool internal, bool doWait=true) noexcept(false);

    /**
     * This command can be used to alter the settings in the test register.
     *
     * The argument passed is just the decimal representation of the bits in the register.
     */
    Fed9UVmeDevice& setTestRegister(u32 testSettings) noexcept(false);

    /**
     * This command will return the decimal equivalent of the number represented
     * by the bits in this register.
     */
    u32 getTestRegister() const noexcept(false);

    /**
     * Disables a FE FPGA.
     *
     * A true will enable a FE FPGA and false will disable it. If a FE FPGA is disabled
     * then a soft reset of the BE FPGA is performed. See BE FPGA soft reset documentation
     * for details of its effect.
     */
    Fed9UVmeDevice& setFedFeUnitDisable(const Fed9UAddress& feFpga, bool feFpgaDisable) noexcept(false);

    /**
     * Returns a true if a FE FPGA is disabled and a false if the FE FPGA is enabled.
     */
    bool getFedFeUnitDisable(const Fed9UAddress& feFpga) const noexcept(false);

    /**
     * This allows the user to select the output data route.
     *
     * The values of the Fed9UReadRoute enum can be found in TypeDefs.hh.
     * The settings for the read route is either the S-LINK or the VME-LINK.
     */
    Fed9UVmeDevice& setBeFpgaReadRoute(Fed9UReadRoute readRoute) noexcept(false);

    /**
     * This returns the output data route.
     *
     * The values of the Fed9UReadRoute enum can be found in TypeDefs.hh.
     * The settings for the read route is either the S-LINK or the VME-LINK.
     */
    Fed9UReadRoute getBeFpgaReadRoute() const noexcept(false);

    /**
     * This will disable the BE FPGA preventing it from seeing both triggers and frames.
     *
     * A true will disable the BE FPGA and a false will enable the BE FPGA.
     */
    Fed9UVmeDevice& setFedBeFpgaDisable(bool fedDisable) noexcept(false);

    /**
     * If a true is returned then the BE FPGA is disabled. If a false is returned then
     * the BE FPGA is enabled.
     *
     * If the BE FPGA is disabled then it will ignore both triggers and frames.
     */
    bool getFedBeFpgaDisable() const noexcept(false);

    /**
     * This allows the user to set the FED ID in the DAQ header.
     *
     * The FED ID is a number unique to a FED and allows the DAQ to identify
     * which FED the data came from. It has a maximum value of 4095.
     */
    Fed9UVmeDevice& setFedId(u16 fedId) noexcept(false);

    /**
     * This allows the user to get the FED ID that will be sent in the DAQ header.
     *
     * The FED ID is a number unique to a FED in a particular setup which is set by software, and allows the DAQ to identify
     * which FED the data came from. do not confuse this software ID with the FedHardwarID, which is unique to every FED and written into 
     * the Eprom during industrial testing
     */
    u16 getFedId() const noexcept(false);

    /**
     * Generates a software trigger.
     */
    Fed9UVmeDevice& sendSoftwareTrigger() noexcept(false);

    /**
     * This performs a soft reset on the BE FPGA.
     *
     * It resets the BE FPGA counters. It does not affect the DCMs and
     * control registers. It does reset the event counters.
     */
    Fed9UVmeDevice& beFpgaSoftReset() noexcept(false);

    /** 
     * The return value holds data from the extra event register
     */
    u32 getBeDebugRegister() const noexcept(false);

              
    /** 
     * The return value holds data from the extra event register
     */
    u32 getBeExtraEventRegister() const noexcept(false);
                 
                 
    /**
     * The return value holds data from various sections
     */
    u32 getBeStatusRegister() const noexcept(false);

    /**
     * Stores the number of triggers that the FED has received since last reset (BE FPGA or
     * FED reset).
     */
    u32 getTriggerNumber() const noexcept(false);

    /**
     * Returns how many bunch crossings have occurred within the FED since the last reset.
     *
     * This counter is reset by both the FED reset and the BE FPGA soft reset.
     */
    u32 getBunchCount() const noexcept(false);

    /**
     * This returns the number of frames that are held in the QDR memory at that
     * instant in time.
     */
    u32 getQdrFrameCount() const noexcept(false);

    /**
     * Returns the status of the TTC chip.
     *
     * 1 ready
     * 0 not ready
     */
    bool getTtcrxStatus() const noexcept(false);

    /**
     * Returns the current version of the firmware that is being used by the BE FPGA.
     */
    u32 getBeFirmwareId() const noexcept(false);

    /**
     * Returns the number of 64 bit words currently in the QDR buffer.
     */
    u32 getQdrBufferLevel() const noexcept(false);

    /**
     * Returns the total number of frames that have been held in the QDR buffer since
     * last reset (BE FPGA or FED reset).
     */
    u32 getQdrTotalFrameCount() const noexcept(false);

    //Matthew Pearson April 2005 - new BE commands for FEDv2.

    /**Returns the DAQ register 32 bit number. 
       This number is written to the FED data buffer header when in full debug mode.*/
    u32 getDaqRegister(void) const noexcept(false);

    /**Sets the DAQ register 32 bit number.                                              
       This number is written to the FED data buffer header when in full debug mode.*/
    Fed9UVmeDevice& setDaqRegister(u32 daqReg) noexcept(false);
    
    /**Returns the DAQ register Two 32 bit number.
       This number is written to the FED data buffer header when in full debug mode.*/
    u32 getDaqRegisterTwo(void) const noexcept(false);
    
    /**Sets the DAQ register Two 32 bit number.
       This number is written to the FED data buffer header when in full debug mode.*/
    Fed9UVmeDevice& setDaqRegisterTwo(u32 daqReg) noexcept(false);
    
    
    /**Returns the status of the requested buffer from the BE status register.
       Uses the enum Fed9UFifoStatus to return a value according to whether the buffer is empty, partially full or full.
       Currently the default is to set it to empty.
       It also uses the enum Fed9UFifoType to identify which buffer status is being requested.*/
    // JEC 23-02-06
    Fed9UFifoStatus getFifoStatus(Fed9UFifoType fifoId) const noexcept(false);

    /**Returns the status of the Tracker Header Monitor data from the BE status register.*/
    // JEC 24-02-06
    bool getTrackerHeaderMonitorStatus(void) const noexcept(false);

    /**Returns an error flag if the internal bunch crossing counter and the value from the TTC system do not match
       at orbit reset.*/
    // JEC 27-02-06
    bool getBunchCrossingAtOrbitReset(void) const noexcept(false);

    /**Returns the S-LINK back pressure flag from the BE status register.*/
    bool getSlinkBackPressure(void) const noexcept(false);

    /**Returns the S-LINK link full flag from the BE status register.*/
    bool getSlinkFull(void) const noexcept(false);

    /**Returns the S-LINK link down flag from the BE status register.*/
    bool getSlinkDown(void) const noexcept(false);

    /**Returns the internal freeze flag. This flag indicated if the FED trigger FIFOs have
       overflowed. The FED will be frozen for debugging if positive.*/
    bool getTriggerFifoOverflow(void) const noexcept(false);

    /**Returns the over temperature interrupt signal flag.*/
    bool getTempInterrupt(void) const noexcept(false);

    //<JEC date=09/01/06>
    //Added tracker header type to Fed9UVmeDevice.
    /**
     * \copydoc Fed9UABC::setHeaderFormatType
     * \throw   Fed9UVmeDeviceException Catches and rethrows all errors thrown during this method call as a this exception type.
     */
    Fed9UVmeDevice& setHeaderFormatType(Fed9UHeaderFormat headerType) noexcept(false);

    /**
     * \copydoc Fed9UABC::getHeaderFormatType
     * \throw   Fed9UVmeDeviceException Catches and rethrows all errors thrown during this method call as this exception type.
     */
    Fed9UHeaderFormat getHeaderFormatType(void) const noexcept(false);
    //</JEC>

    //<JEC date=23/02/2006>
    //Added bunch crossing offset.
    /**
     * \copydoc Fed9UABC::setBunchCrossingOffset
     * \throw   Fed9UVmeDeviceException Throws for an out of bounds error and rethrows all events thrown during member function
     *          execution as this type.
     */
    Fed9UVmeDevice& setBunchCrossingOffset(u16 bxOffset) noexcept(false);

    /**
     * \copydoc Fed9UABC::getBunchCrossingOffset
     * \throw   Fed9UVmeDeviceException Rethrows all errors thrown during member function execution and rethrows as this exception.
     */
    u16 getBunchCrossingOffset(void) const noexcept(false);
    //</JEC>

    //<JEC date=08/12/2005>
    //Added eventType
    /**
     * \copydoc Fed9UABC::setDaqEventType
     * \throw   Fed9UVmeDeviceException Throws this exception if eventType is out of range. Rethrows all errors thrown during member
     *          function execution and rethrows as this exception.
     */
    Fed9UVmeDevice& setDaqEventType(u16 eventType) noexcept(false);

    /**
     * \copdoc Fed9UABC::getDaqEventType
     * \throw  Fed9UVmeDeviceException Rethrows all errors thrown during member function execution and rethrows as this exception.
     */
    u16 getDaqEventType(void) const noexcept(false);
    //</JEC>

    //<JEC date=08/12/2005>
    //Added the FOV
    /**
     * \copydoc Fed9UABC::setDaqFov
     * \throw   Fed9UVmeDeviceException Thrown for an out of bounds error and rethrows all errors thrown
     *          during member function execution.
     */
    Fed9UVmeDevice& setDaqFov(u16 fov) noexcept(false);
    /**
     * \copydoc Fed9UABC::setDaqFov
     * \throw   Fed9UVmeDeviceException Rethrows all errors thrown
     *          during member function execution.
     */
    u16 getDaqFov(void) const noexcept(false);
    //</JEC>

    // <JEC date="1/12/2006">
    Fed9UVmeDevice& setBunchCrossingOrbitTotal(u16 orbitTotal) noexcept(false);
    //</JEC>

    /**Returns the bunch crossing number total for each LHC orbit. This is a read
       only register. It should return the same value each orbit. It is a 12-bit number.*/
    u16 getBunchCrossingOrbitTotal(void) noexcept(false);

    // JEC 8/3/06
    // under development
    Fed9UVmeDevice& setFmmTestRegister(u16 testSettings) noexcept(false);

    /**Returns the contents of the FMM test register.  It is a 4-bit number which is used to test the signals to the FMM
       manually.  This register is enabled via bit 5 of the main test register, but should not be enabled during normal
       operation.*/
    // JEC 6/3/06
    u16 getFmmTestRegister(void) noexcept(false);
    
    //<JEC date=16/03/07>  NB.  setter is private as it must be used in conjunction with corresponding FE command
    /**
     * Retrieves the prescale applied to all triggers received by the BE-FPGA.  The firmware needs the number of events
     * to skip.  This method converts that into the actual prescale factor.  Skip every Nth event means a prescale factor of N+1.
     */
    u16 getBEFrameCapturePrescale(void) const noexcept(false);
    //</JEC>
    
    /**Returns all the tracker header monitor information. The tracker header monitor for
       each FE contains the status words for each fibre in that FE and the majority APV pipeline address.
       The method returns a vector which has 13 elements. Elements 1-12 contain the fibre status words
       for fibres 1-12 respectively. Element 13 contains the majority APV pipeline address for that FE.
       The method expects a u8 input from 1-8 to select the front end unit.
       NOTE: this method internally checks the tracker header monitor status flag which is
       in the DAQ register before reading the tracker header monitor. An exception is thrown
       if the data is not ready to be read.*/
    std::vector<u8> getTrackerHeaderMonitor(u16 feFpga) const noexcept(false);

    /**
     * \brief  Returns the level 1 event number as defined by the TTC system.
     * \return usigned long.
     * \throw  Fed9UVmeDeviceException This exception is thrown if the tracker header monitor is not ready and hence the
     *         level 1 accept from the header would be invalid. All exceptions that are thrown during this member function call
     *         are caught and rethrown as this type.
     */
    u32 getL1AcceptID(void) const noexcept(false);
    
    /**Returns the TTS Read bit.*/
    bool getTTSRead(void) const noexcept(false);

    /**Returns the TTS Warn Overflow bit.*/
    bool getTTSWarnOverflow(void) const noexcept(false);

    /**Returns the TTS Busy bit.*/
    bool getTTSBusy(void) const noexcept(false);

    /**Returns the TTS Out of Sync bit.*/
    bool getTTSOutOfSync(void) const noexcept(false);

    /**Reads the Bx counter value at LHC orbit from the long_read_1 register.
       This should be identical to the value read using the getBunchCrossingOrbitTotal() method.*/
    u16 getBunchCrossingOrbitLongRead1(void) const noexcept(false);

    /** Returns the complete contents of the second BE status register.*/
    // JEC 20-11-06
    vector<u32> getSecondBeStatusRegister(void) const noexcept(false);

    /**Reads the last TTC broadcast frame (short format) decoded from channel B of the TTCrx.*/
    std::vector<u16> getTtcBroadcastCommand(void) const noexcept(false);

    /**Read the last TTC individually addressed command (long format) decoded from channel B of the TTCrx.*/
    std::vector<u16> getTtcIndividuallyAddressedCommand(void) const noexcept(false);

    // JEC 10-04-06
    /** This method dumps the contents of all read/write BE registers.
     *  Only those registers which are set up during initialisation are printed out.
     */
    void dumpBeRegisters(ostream &os, bool addHtml) noexcept(false);
    void dumpBeStatusRegisters(ostream &os, bool addHtml) noexcept(false);
    void dumpTrackerHeaderMonitor(ostream &os, bool addHtml) noexcept(false);

  private:
    // JEC 06-10-06
    void bePrettyPrint(ostream &os, string info, u32 arguments, bool addHtml) {
      if (addHtml) {
	os << "<TR>" << "<TD>" << info << "</TD> <TD> 0x" << std::hex << arguments << std::dec << "</TD>";
      } else {
	info.append(55-info.size(),' ');
	os << info << "0x" << std::hex << arguments << std::dec;
      }
    }

    void bePrettyPrint(ostream &os, string info, vector<u32> arguments, bool addHtml) {
      if (addHtml) {
	os << "<TR>" << "<TD>" << info << "</TD> <TD>";
	for (u32 i = 0; i < arguments.size(); ++i) {
	  os << "0x" << std::hex << arguments[i] << std::dec << " ";
	}
	os << "</TD>";
      } else {
	info.append(55-info.size(),' ');
	os << info;
	for (u32 i = 0; i < arguments.size(); ++i) {
	  os << "0x" << std::hex << arguments[i] << std::dec << " ";
	}
      }
    }

    void ConvertToBinary(ostream &op, vector<u32> numberToConvert, u32 length);

    //<JEC date=16/03/07>  This is a private member because it must be used in conjunction with the corresponding FE command
    /**
     * This sets the value of the prescale applied to all triggers coming into the BE-FPGA.  It is a private member because
     * it must be used in conjunction with the corresponding register for the FE-FPGAs.
    */
    Fed9UVmeDevice& setBEFrameCapturePrescale(u16 prescale) noexcept(false);
    //</JEC>
    
    /*******************************************************************************
     *                     These are the VME FPGA commands                         *
     *******************************************************************************/

  public:

    /**
     * Returns the firmware ID on the VME FPGA.
     */
    u32 getVmeFirmwareId() const noexcept(false);

    /**
     * \copydoc Fed9UABC::setClock
     *
     * If the clock is successfully changed then a FED reset is sent.
     * If the clock is not successful changed then it will return to the
     * previous clock setting. If the clock is lost then it will default
     * to the on board clock. A FED reset is performed in each case.
     * This is not affect by a FED reset. After the rest there will be a
     * software pause to allow the FED to lock to the new clock.
     */
    Fed9UVmeDevice& setClock(Fed9UClockSource clockSelect) noexcept(false);

    /**
     * \copydoc Fed9UABC::setClock
     * \param   doWait If boolean true then there will be a pause after the FED reset
     *          is sent, boolean false will prevent this wait from occuring.
     *
     * This works in the same way as the setClock(clockSelect), however the wait after
     * the reset can be removed. As long as the User guaranties there will be no communication
     * with the FED for at least 0.5secs after the clock change there should be no problem.
     */
    Fed9UVmeDevice& setClock(Fed9UClockSource clockSelect, bool doWait) noexcept(false);

    /**
     * Returns the clock source.
     *
     * The clocks avaliable to be selected are the on board, TTC and back plane clock.
     * The Fed9UClockSource enum is defined in TypeDefs.hh.
     */
    Fed9UClockSource getClock() const noexcept(false);

    /**
     * This method will set the amount of read only memory on the EPROM.
     *
     * The read only level can be set to
     * FED9U_EPROM_NONE   - none of the memory is protected.
     * FED9U_EPROM_QUATER - the upper quater (1536 - 2046) is protected.
     * FED9U_EPROM_HALF   - the upper half (1024 - 2046) is protected.
     * FED9U_EPROM_ALL    - all the memory is write protected.
     * These values are from the Fed9UEpromReadOnlyLevel enum found in the TypeDefs.
     * At present it is not possible to read back this setting.
     */
    Fed9UVmeDevice& setEpromReadOnlyLevel(Fed9UEpromReadOnlyLevel readOnlyLevel) noexcept(false);

    /**
     * This method is used to write to the EPROM memory.
     *
     * It takes an EPROM description, which should contain the new settings to be updated. The offset is the starting address
     * into the EPROM memory that needs to be written to. It is defaulted to 0 and has a maximum value of 2046.
     * There is also the option to specify the number of byte that should be written to. It has a maximum value of 2047
     * and must always be less than the size of the EPROM minus the offset. It is defaulted to 2047. The default
     * value will update the entire EPROM.
     */
    Fed9UVmeDevice& setEprom(const Fed9UEpromDescription& epromDescription, u32 initialOffset = 0, u32 numberOfBytes = 2047) noexcept(false);

    /**
     * This method will return the contents of a section of the EPROM memory.
     *
     * The EPROM descrtiption passed as an argument will be updated with the data from the EPROM starting
     * at the initial offset until it has retrevied the specified number of bytes. The initialOffset is defaulted
     * to 0 and has a maximum value of 2046. The number of bytes has a maximum value of 2047 and is defaulted to 2047.
     * The updated EPROM is returned by reference.
     */
    Fed9UEpromDescription getEprom(u32 initialOffset = 0, u32 numberOfBytes = 2047) const noexcept(false);

    /**
     * This method will set the FED serial number.
     * NOTE: This method will be become depreciated during 2005.
     */
    Fed9UVmeDevice& setFedSerialNumber(u32 serialNumber) noexcept(false);

    /**
     * This method will set the FED hardware id which is also called fed serial number.
     * this method maps to that in the Fed9UABC and the Fed9UDescription
     * NOTE: This method will be become depreciated during 2005.
     */
    Fed9UVmeDevice& setFedHardwareId(u32 serialNumber) noexcept(false);

    /**
     * This method will return the FED serial number.
     * NOTE: This method will be become depreciated during 2005. Use the getFedSerialNumberProtected method instead.
     */
    u32 getFedSerialNumber() const noexcept(false);

    /**
     * This method will read the FED hardware ID (or serial number) from the write protected EPROM memory.
     * It uses a enum defined in TypeDefs.hh which sets the byte at which the write protected area of the
     * EPROM begins.
     */
    u32 getFedSerialNumberProtected() const noexcept(false);
    
    /**
     * This method will return the FED HardwareId which is also called Fed serial number.
     * this method maps to that in the Fed9UABC and the Fed9UDescription
     * NOTE: This method will be become depreciated during 2005. Use the getFedSerialNumberProtected method instead.
     */
    u32 getFedHardwareId() const noexcept(false);

    /**
     * This can be used to place the voltage controller in a stand by mode which does not perform
     * any measurements or conversion and can be used to save power when the voltage controller is
     * not needed.
     *
     * A true will shut the controller down and a false enable it. This will not reset the chip.
     * All register values will remain unaffected.
     */
    Fed9UVmeDevice& setVoltageMonitorStandBy(bool standBy) noexcept(false);

    /**
     * This provides the ability to reset the status registers to their hardware defaults.
     */
    Fed9UVmeDevice& resetVoltageMonitorStatusRegister() noexcept(false);

    /**
     * This is used to configure the voltage monitor.
     *
     * It must be given a Fed9UVoltageController which should contain all the 
     * appropiate settings.
     */
    Fed9UVmeDevice& setVoltageMonitor(const Fed9UVoltageControl& voltageController) noexcept(false);

    /**
     * This return a Fed9UVoltageMonitor which contains all the current read and writable
     * settings on the voltage monitor.
     */
    Fed9UVoltageControl getVoltageMonitor() const noexcept(false);

    /**
     * This returns a Fed9UVoltageMonitorInfo which contains all the current settings
     * on the voltage monitor. Both the read writable settings and the read only ones.
     */
    Fed9UVoltageControlInfo getVoltageMonitorInfo() const noexcept(false);

    /**
     * This will reset all the configuration and status registers on the System ACE controller. It will
     * also perform a soft reset of the Compact Flash (CF) device.
     *
     * The firmware will be reloaded from the current configuration address.
     */
    Fed9UVmeDevice& resetSystemAce() noexcept(false);

    /**
     * \brief  Reload the FPGA firmware from a compact flash card revision.
     * \param  revision The revision number on the CF to reload from. Range: 0-7.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Thrown if the revision number is out of bounds or firmware fails to reload correctly,
     *         also catches errors thrown from called methods and rethrows as this exception type.
     *
     * The compact flash card can contain up to eight different firmware revisons. The revision number will
     * select the revision from which the CF card will load the firmware. getCfFileHeader can be used to view
     * the firmware version stored in each CF revision.
     */
    Fed9UVmeDevice& reloadFpgaFirmwareFromCompactFlash(u32 configAddr) noexcept(false);
    
    /**
     * This is used to set the System ACE data words to 16 bit.
     *
     * When in word mode each System ACE register address refers to a 16 bit
     * register. It is possible to set the System ACE into byte mode where each
     * register will be 8 bits large. This class assumes that the System ACE is
     * in word mode. Each method that uses the System ACE sets itself into word
     * mode and so there should be no need to call this method.
     */
    Fed9UVmeDevice& setSysAceWordMode() noexcept(false);
    
    /**
     * Checks the size of the data used by the Compact Flash (CF).
     *
     * The Compact Flash can either write 8 bit data words or 16 bit data words. This returns the mdoe
     * that it is currently using. Returns a true if the CF is in 16 bit data word mode. False if it is
     * not. All System ACE methods rely on it begin in word mode. Each method always sets itself into
     * word mode before writting to the System ACE.
     */
    bool getSysAceWordMode() const noexcept(false);

    //\todo - these are test functions and need to be removed once everything is working.
    //Depreciated. Do not use. They will be removed in future versions.
    Fed9UVmeDevice& setSystemAceRegister(u32 data, u32 regAddr) noexcept(false);

    u32 getSystemAceRegister(u32 regAddr) const noexcept(false);

    /**
     * \brief  Returns the firmware versions and the revision they are stored in by reading the header file, which was loaded
     *         into the compact flash.
     * \param  cfFileName The name of the file which contains the header to be read.
     * \return String The returned header information is return in a string, matching the file layout.
     * \throw  Fed9UVmeDeviceException If the file fails to open or does not contain a valid header then this exception is thrown.
     *         All exceptions thrown during this method call will be caught and rethrown as this type.
     */
    std::string getCfFileHeader(const std::string& cfFileName) noexcept(false);

    /**
     * \brief  A method to load a new Compact Flash (CF) image on to the FEDs CF card.
     * \param  cfInputImageFilePath A string pointing to the .ace file that contains the CF image, that should be uploaded.
     *         it will open the User defined file.
     *         and no VME access to the FED will be made.
     * \param  testCf If boolean true the loaded image will be written to the file cf_image_test.ace. Default false.
     * \param  setGlobalFirmware If the file name given is the global firmware file then we can skip the file parsing if that has
     *         already been done. Default false.
     * \return string The returned string contains the file header showing the location on the CF card of
     *         all the firmware revisions stored in the image.
     * \throw  Fed9UVmeDeviceException Checks are made on the file that will throw an error of this type
     *         if it does not match the .ace file structure. Any problems in writing to the CF card will
     *         also throw this error if it is unrecoverable. Finally the method will catch all errors thrown
     *         by called methods and rethrow as this type.
     *
     * Once the upload is complete then firmware will not be reloaded and it must be done by a call to
     * reloadFpgaFirmwareFromCompactFlash. This member function is thread safe.
     */
    std::string setCfImageFile(const std::string& cfInputImageFilePath, bool testCf = false, bool setGlobalFirmware = false) noexcept(false);

    /**
     * \brief  Loads a blank file allocation table to the CF card. Performing a quick format of the card.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Thrown if there is a problem loading the file containing the CF data, also if the file
     *         has become corrupt. It will also be thrown if there is an unrecoverable error while trying to load the CF data
     *         to the card.
     *
     * This method has no knowledge of the file structure on the CF card and in order perform a quick format loads a preformatted
     * CF image to the CF device that is included as part of the Fed9U installation.
     */
    Fed9UVmeDevice& setEraseCfImage() noexcept(false) ;

    /**
     * \brief  Reads a portion of the compact flash card and saves it to file. It saves a direct copy of the words on the CF file.
     * \param  mbToReadFromCf The number of operating system defined mega (2^6) bytes to read from the compact flash. Range:0-60.
     * \param  cfOutputImageFile The output image file to write to. It is defaulted to "UserBinary" which instructs the method
     *         to generate a binary encoded name based on the firmware versions present. Providing a string will override
     *         this naming system. All files should be saved using the ".ace" extension.
     * \param  maxRev Sets the maximum number of revisions you wish to check for. This does not affect the revisions that will be downloaded.
     *         Default: 8, max number of revisions on CF.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Throws this error for a number of error conditions within the method and will rethrow errors
     *         caught from its own calls.
     *
     * The CF flash card can handle upto 8 firmware revisions stored in the file. The saved file therefore contains a header documenting
     * the firmware version and the revision number which contains them. The revision number is required when reloading the firmware from CF.
     * Once the CF image has been created the CF card will reload the firmware from the default revision location, which is 0.
     * The file name generated by the member function is in a binary format containing the firmware versions stored on each revision.
     * It can be overriden using a human readable name, however the software will no longer consider this a valid .ace file.
     */
    Fed9UVmeDevice& getCfImageFile(u32 mbToReadFromCf, std::string cfOutputImageFile = "UseBinary", u32 maxRev=8) noexcept(false);


    /**
     * \brief  A method to write data to the compact flash (CF) card in the event the system ace control gets locked in a
     *         state where it is wait for data. It will corrupt the CF card requiring a new CF image to be loaded!
     * \return Self reference
     * \throw  Fed9UVmeDeviceException Catches all exceptions thrown by methods called and rethrow as this error type.
     *
     * This member function should only be called if the system ace status returns true for data buffer ready, mode and CF
     * data request return true. It will write enough data to release the waiting for data lock. New firmware must be loaded
     * after this method is called as it will leave the data on the CF card in an undefined state. Always try to reset the
     * system ace and rewrite the image first this is a last resort method!
     */
    Fed9UVmeDevice& cfDataBufferWriteDump() noexcept(false);

    /**
     * \brief  To be used in the event that reading from the compact flash fails and leave the system ace controller in a
     *         state where it has data waiting to be read. It will read this data out releasing any locks in the controller.
     * \return Self reference
     * \throw  Fed9UVmeDeviceException This member function catches all exceptions thrown by called methods and rethrows as this
     *         exception.
     *
     * This method should only be called in the event that the system ace status returns true for the data buffer ready and
     * CF data request and its false for data buffer mode. It will read the remaining data from the buffer and should unlock
     * the system ace controller.
     */
    Fed9UVmeDevice& cfDataBufferReadDump() noexcept(false);

    /**
     * Returns the version number.
     *
     * The returned string contains the major version, minor version and revision number each separated
     * by an underscore.
     */
    std::string getSystemAceVersionNumber() noexcept(false);

    /**
     * This returns the address that the current firmware is loaded from.
     *
     * The Compact Flash (CF) card can contain multiple firmware versions. This provides the address of the firmware
     * used on the CF card. It should be used if the current firmware version is to be reloaded. It is a 3 bit number.
     */
    u16 getCfFirmwareAddress() noexcept(false);

    /**
     * This returns an 17 element vector which contains all the setting in the status register. A description of
     * all the elements is given below. For a more detailed description the XILINX System ACE Compact Flash (CF)
     * Solution documentation should be consulted.
     *
     * 0 Controller Lock Status - true if the controller has successfully locked to the CF.
     * 1 MPU Lock Status - true if the MPU has successfully locked to the CF.
     * 2 Configuration Controller Error Status - true if a configuration controller error has occured. Consult the
     *                                           error register for more information.
     * 3 CF Controller Error Status - true if a CF error has occured. Consult the error register for more information.
     * 4 CF Detect - true if there is a CF device connected to the ACE controller.
     * 5 Data Buffer Ready - true when the data buffer is ready to be read from or written to.
     * 6 Data Buffer Mode - true when the data buffer is in a write only state and false when in a read only state.
     * 7 Configuration Done - true when teh ACE controller configuration process is complete.
     * 8 Ready For CF Controller Command - true means ready for a command.
     * 9 Configuration Mode - true means that the configuration process will automatically start after the ACE
     *                        controller reset.
     * 10 CF Busy - if true the CF is not ready to accept commands. If false then elements 1-6 are not valid.
     * 11 CF Ready - if true the CF is ready for operation. It reflects the state of the RDY bit in the CF status register.
     * 12 CF Data Write Fault - true if there is a fault in the write process.
     * 13 CF Ready - if true the CF is ready for operation. It reflects the state of the DSC bit in the CF status register.
     * 14 CF Data Request Bit - if true the data is to be transferred to/from the CF.
     * 15 CF Correctable Error - true if a correctable error has occurred. See the error register for more details.
     * 16 CF Error - true if an error has occurred. See the error register for more details.
     */
    std::vector<bool> getSystemAceStatusRegister() noexcept(false);

    /**
     * This returns a five element vector that contains all the error bits for the configuration
     * register.
     *
     * This register is used to configure the Compact Flash (CF) for reads and writes. Each element
     * is described below.
     * 0 Address Error - true if the configuration address used to specify the location of the firmware on
     *                   the CF is invalid.
     * 1 Configuration Failure - true if there is a failure during the configuration of the FPGAs.
     * 2 Read Error - true if an error occured during a read of the CF.
     * 3 Instruction Error - true if an invalid instruction was encountered during configuration.
     * 4 Monitor Error - true if the configuration did not start within 500ms of being told to start.
     */
    std::vector<bool> getSysAceConfigErrorStatus() noexcept(false);

    /**
     * This returns a 10 element vector that contains all the Compact Flash error bits.
     *
     * The means of each element is described below.
     * 0 Reset Error - true if the CF failed to reset properly.
     * 1 CF Ready Error - true if the CF failed to become ready.
     * 2 Read Error - true if the CF read command failed.
     * 3 Write Error - true if a CF write comand failed.
     * 4 Sector Ready Error - true if a sector failed to become ready during a CF read or write.
     * 5 Bad Block Error - true if a bad block has been detected on the CF.
     * 6 Uncorrectable Error - true if an uncorrectable error has been encountered.
     * 7 ID Not Found - true if the requested sector ID is in error or cannot be found.
     * 8 Command Abort - true if a CF command was aborted.
     * 9 General Error - true if a general error has occured.
     */
    std::vector<bool> getCfErrorStatus() noexcept(false);

    /**
     * This returns an 8 element vector which contains the status of the Compact Flash (CF)
     * partition. The CF flash can be formatted as either a FAT12 or FAT16 partition.
     * This returns the status of this partition and which type it is formatted as.
     *
     * The contents of the vector elements is as follows.
     * 0 Master Boot Record (MBR) - true if there is a valid boot record.
     * 1 Partition Boot Record (PBR) - true if there is a vaild partition boot record.
     * 2 MBR FAT12 Flag - true if the FAT12 flag is set in the MBR.
     * 3 PBR FAT12 Flag - true if the FAT12 flag is set in the PBR.
     * 4 MBR FAT16 Flag - true if the FAT16 flag is set in the MBR.
     * 5 PBR FAT16 Flag - true if the FAT16 flag is set in the PBR.
     * 6 Calculate if FAT12 or not based on the cluster count - true if FAT12 format.
     * 7 Calculate if FAT16 or not based on the cluster count - true if FAT16 format.
     */
    std::vector<bool> getCfFatStatus() noexcept(false);

    /**
     * Returns the number of events that have been readout over the VME link.
     */
    u32 vmeEventCounter() const noexcept(false);

    /**
     * \brief  Looks in the event buffer for an event.
     * \return li 1: signifies an event in the buffer,
     *         li 3: signifies the last fragment of an event is in the buffer.
     * \throw  Fed9UVmeDeviceException Catches all exceptions thrown from called methods
     *         and rethrows as this exception with method information appended.
     */
    u32 hasEvent() const noexcept(false);

    /**
     * The event in the buffer is read out to the location of memory specified by the
     * destBuffer pointer.
     *
     * The argument numU32sAddedToBuffer will return the number of 32 bit words that
     * were added to the destBuffer. The freeBufferSpace is the size of the destBuffer
     * in 32 bit words and the event counter will return the number of readouts that
     * were performed over the VME link (will be redudent when using S-Link). The
     * software is capable of performing a block transfer, which is faster than reading
     * out a single 32 bit word at a time. By setting this value to false a block transfer
     * will be performed, true and single reads will be used instead.
     */
    Fed9UVmeDevice& getCompleteEvent(u32* destBuffer, u32 freeBufSpace, u32& numU32sAddedToBuffer, u32& eventCounter, bool blockDisable = true) noexcept(false);

    /**
     * \brief  Reads the full event from memory into a software buffer using a VME block transfer.
     * \param  destBuffer A pointer to the area of memory to save the event.
     * \param  freeBufSpace The size of the buffer to which the event is being saved.
     * \param  numu32sAddedToBuffer The number of 32 bit words that have been written in to the buffer.
     * \param  eventCounter Returns the number of readouts that were performed over the VME link. Redundant if using S-link.
     * \return Self reference
     * \throw  Fed9UVmeDeviceException This error type is thrown in response to a variety of error condition
     *         that may occur while reading the event.
     */
    Fed9UVmeDevice& getCompleteEventTurboStyle(u32* destBuffer, u32 freeBufSpace, u32&  numU32sAddedToBuffer, u32& eventCounter) noexcept(false);

    /**
     * Purge the event buffer.
     *
     * Removes any existing events from the buffer. It returns the number of events that
     * were purged and the number of events that have been readout over the VME link.
     */
    Fed9UVmeDevice& purgeEvents(u32& numberOfPurgedEvents, u32& eventCounter) noexcept(false);


  private:

    /**
     * This is used to write to a byte of memory on the EPROM. It takes a two element
     * vector that contains the byte to be written and the location that byte should be
     * written to.
     *
     * The byte to be written to is given in the first element of the vector.
     * The memory address of the byte to be written is given in the second element of
     * the vector.
     */
    Fed9UVmeDevice& epromWrite(const std::vector<u32>& epromDataAndOffset) noexcept(false);

    /**
     * This is used to read a byte of memory on the EPROM. It takes only one arguement
     * the address of the byte of memory to be read.
     */
    u32 epromRead(u32 offset) const noexcept(false);

    /**
     * Before the Compact Flash (CF) card can be read from the System ACE controller must get a lock.
     * If the method fails to get a lock then a Fed9UVmeDeviceException error will be thrown.
     */
    Fed9UVmeDevice& getCompactFlashLock() noexcept(false);

    /**
     * Once the System ACE has a lock with the Compact Flash (CF) card it must then check to
     * see if the CF device is ready to receive commands.
     */
    Fed9UVmeDevice& checkCfReadyForCommands() noexcept(false);

    /**
     * This sets the starting Logical Block Address (LBA), the number of sectors that are to be written
     * to and whether the Compact Flash (CF) will be written to or read from.
     *
     * A true will enable the CF to be read from and a false to be written to. The sector
     * count has a maximum value of 256 and a minimum of 1. The LBA is a pointer to the first 0.5KB block
     * of memory on the CF card that the read/write should start from. It has a maximum value of 128,000
     * on a 64MB CF card.
     */
    Fed9UVmeDevice& setDataRegionToReadOrWrite(u32 lba, u32 sectorCount, bool read) noexcept(false);

    /**
     * This polls the data buffer to ensure it is ready to be read.
     */
    Fed9UVmeDevice& checkDataBufferReady() noexcept(false);

    /**
     * \brief  Scans the available revisions on the CF cards to look for alternate firmware version
     *         and then records the details to the compact flash image file header.
     * \param  maxRev Sets the maximum number of revisions you wish to check. Default: 8, max number that can be set.
     * \return std::vector<std::string> Returned vector contains two elements. The first is the binary format file name.
     *         The second is the file header.
     * \throw  Fed9UVmeDeviceException Thrown in the event that there are not consistent firmware versions
     *         across the FE or delay FPGAs. Will also catch and throw any additional error thrown from calling
     *         methods as this exception. 
     *
     * This method will detect the firmware versions stored in each revision by sequentially loading each revision stored on the
     * compact flash card and reading back the firmwares from the FED registers. This will disrupt anything else the FED is doing.
     */
    std::vector<std::string> setCfFileHeader(u32 maxRev=8)  noexcept(false);

    /**
     * \brief  Utility method to check the CF image .ace file for consistancy and to remove the file header.
     * \param  cfInputFile The CF .ace file to be processed.
     * \param  endDataMarker A return by reference that points to the end of the data in the .ace file.
     *         Calling method is not required to know this.
     * \param  firmwareInfo A return by reference that contains the .ace file header. The header contains the
     *         firmware versions that are stored in each revision on the CF file. Default -1, invalid.
     * \param  startDataMarker A return by reference that points to the start of the data in the .ace file.
     * \return Self reference.
     * \throw  Fed9UVmeDeviceException Thrown if there are any problems processing the .ace file.
     *
     * This member function guaranties that the file pointer in the cfInputFile will be left at the start of the compact flash data
     * and hence reading from the file can begin from the start of the CF data. This member function is not necessarily thread safe.
     * Arguements are passed by reference and are modified, hence the references must be mutual exclusion must be provided by the User
     * if they are two common data structures.
     */
    Fed9UVmeDevice& removeCfFileHeaderAndMarkers(std::ifstream& cfInputFile, std::ios::pos_type& endDataMarker, std::string& firmwareInfo/*, std::ios::pos_type& startDataMarker=std::ios::off_type(-1)*/) noexcept(false);

    /**
     * \brief  Will return true if the Compact Flash (CF) card reloaded correctly after a reboot from the CF, otherwise false.
     * \return boolean.
     * \throw  Fed9UVmeDeviceException Catches all errors that occur during the member function call and rethrows as this error.
     *
     * This polls the system ACE registers to check for a valid reload in the status register. It will time out if the firmware
     * is found not to reload correctly.
     */
    bool pollCfForFirmwareReload() const noexcept(false);

    /*******************************************************************************
     *                      These are the high level commands                      *
     *******************************************************************************/

  public:
    /**
     * \brief Used to read the firmware version of the FE FPGAs.
     * \returns long unsigned int holding the identification number.
     * \throws Fed9UVmeDeviceException Will be thrown if there is an
     *         inconsistency across the FE FPGA firmware versions.
     * \sa getFeFirmwareId
     *
     * Checks that the firmware versions from each FE FPGA are the same.
     */
    u32 getFeFirmwareVersion() const ;

    /**
     * \brief Used to read the firmware version of the BE FPGAs.
     * \returns long unsigned int holding the identification number.
     * \sa getBeFirmwareId
     */
    u32 getBeFirmwareVersion() const ;

    /**
     * \brief Used to read the firmware version of the VME FPGAs.
     * \returns long unsigned int holding the identification number.
     * \sa getVmeFirmwareId
     */
    u32 getVmeFirmwareVersion() const ;

    /**
     * \brief Used to read the firmware version of the delay FPGAs.
     * \returns long unsigned int holding the identification number.
     * \throws Fed9UVmeDeviceException Will be thrown if there is an
     *         inconsistency across the delay FPGA firmware versions.
     * \sa getDelayFirmwareId
     *
     * Checks that the firmware versions from each delay FPGA are the same.
     */
    u32 getDelayFirmwareVersion() const ;

    /**
     * \brief Used to read the version of the FED in use.
     * \returns long unsigned int holding the identification number.
     *
     * Value is taken from the local description setting, which is update
     * upon contruction of this class.
     */
    u32 getFedVersion() const ;

    /**
     * \brief Used to read the version of the FED EPROM in use.
     * \returns long unsigned int holding the identification number.
     *
     * Value is taken from the local description setting, which is update
     * upon contruction of this class.
     */
    u32 getEpromVersion() const ;

    /**
     * \brief Sets the FE firmware version in the local copy of the description.
     * \param version The version number from that you wish to update to.
     * \returns Self reference.
     * \throw Fed9UVmeDeviceException Version is checked for consistency with the
     *        FED hardware, throwing an exception of this type if there is a mismatch.
     *
     * This is a read only command, hence there can be no setting to a FED register.
     * The value must be consistent across all FE FPGAs hence there is only one
     * variable in the description to store the value.
     */
    Fed9UVmeDevice& setFeFirmwareVersion(u32 version);
    
    /**
     * \brief Sets the BE firmware version in the local copy of the description.
     * \returns Self reference.
     * \throw Fed9UVmeDeviceException Version is checked for consistency with the
     *        FED hardware, throwing an exception of this type if there is a mismatch.
     *
     * This is a read only command, hence there can be no setting to a FED register.
     */
    Fed9UVmeDevice& setBeFirmwareVersion(u32 version);
    
    /**
     * \brief Sets the VME firmware version in the local copy of the description.
     * \returns Self reference.
     * \throw Fed9UVmeDeviceException Version is checked for consistency with the
     *        FED hardware, throwing an exception of this type if there is a mismatch.
     *
     * This is a read only command, hence there can be no setting to a FED register.
     */
    Fed9UVmeDevice& setVmeFirmwareVersion(u32 version);
    
    /**
     * \brief Sets the delay firmware version in the local copy of the description.
     * \param version The version number from that you wish to update to.
     * \returns Self reference.
     * \throw Fed9UVmeDeviceException Version is checked for consistency with the
     *        FED hardware, throwing an exception of this type if there is a mismatch.
     *
     * This is a read only command, hence there can be no setting to a FED register.
     * The value must be consistent across all delay FPGAs hence there is only one
     * variable in the description to store the value.
     */
    Fed9UVmeDevice& setDelayFirmwareVersion(u32 version);
    
    /**
     * \brief Sets the EPROM version in the local copy of the description.
     * \param version The version number from that you wish to update to.
     * \returns Self reference.
     *
     * This is a read only command, hence there can be no setting to a FED register.
     */
    Fed9UVmeDevice& setEpromVersion(u32 version);
    
    /**
     * \brief Sets the EPROM version in the local copy of the description.
     * \param version The version number from that you wish to update to.
     * \returns Self reference.
     *
     * This is a read only command, hence there can be no setting to a FED register. 
     */
    Fed9UVmeDevice& setFedVersion(u32 version);
    
    /**
     * This method sends a complete logic reset to the FE and BE FPGAs.
     *
     * It will not reset the control registers (e.g. the scope length, FED DAQ mode,
     * clock etc) but it will reset all the counters and associated logic. Can be used
     * to clear an event without having to rerun the init() method. Warning this will
     * reset the APV disable settings.
     */
    Fed9UVmeDevice& sendFedCompleteLogicReset() noexcept(false);

    /**
     * This will reset the whole FED.
     *
     * The TTCrx chip, the temperature monitors, pedestals, disabled strips, high and
     * low cluster thresholds and the clock source are unaffected by a FED Reset.
     */
    Fed9UVmeDevice& sendFedReset(bool doWait=true) noexcept(false);

    /**
     * This will reset VME VLink and counters only.
     *
     */
    Fed9UVmeDevice& sendVLinkReset() noexcept(false);

    /**
     * This will reset VME Bus using the Bus controller resetBus Method.
     *
     */
    Fed9UVmeDevice& sendVmeBusReset() noexcept(false);
 
    /**
     * This will reset VME Bus using the Bus controller resetBus Method.
     *
     */
    Fed9UVmeDevice& sendVmeBusResetInhibit(bool value) noexcept(false);

 

   /**
     * \copydoc Fed9UABC::setDaqMode
     */
    Fed9UVmeDevice& setDaqMode(Fed9UDaqMode newFedDaqMode) noexcept(false);

    /**
     * Sets the DAQ super mode for the FED.
     *
     * The possible DAQ Super modes are defined in the Fed9UDaqSuperMode enum, which
     * is declared in TypeDefs.hh. There are two DAQ super modes avaliable
     * FAKE, and NORMAL. FAke mode will enable you to produce fake events from within the front end FPGA, without having anything connected to the FEUnit.
     * NORMAL, is normal running and expects data to come in via the Opto RX.
     */
    /**
     * \copydoc Fed9UDaqSuperMode
     */
    Fed9UVmeDevice& setDaqSuperMode(Fed9UDaqSuperMode newFedDaqMode) noexcept(false);

    
   /**
     * Returns the FED specifier from the VME register.
     *
     * In order to identify FEDs in the crate we read from this register
     * It should hold the value 0x00000FED
     */
    bool getIsFed()  const noexcept(false);


    /**
     * Returns the DAQ mode for the FED.
     *
     * The possible DAQ modes are defined in the Fed9UDaqMode enum, which
     * is declared in TypeDefs.hh. There are four DAQ modes avaliable
     * process raw data mode, zero suppressed data mode, virgin raw dat
     * mode and scope mode.
     */
    Fed9UDaqMode getDaqMode()  const noexcept(false);


    /**
     * Returns the DAQ Super mode for the FED.
     *
     * The possible DAQ modes are defined in the Fed9UDaqSuperMode enum, which
     * is declared in TypeDefs.hh. There are thre DAQ Super modes avaliable
     * fake, zero lite and normal
     */
    Fed9UDaqSuperMode getDaqSuperMode()  const noexcept(false);
  
    /**
     * \copydoc Fed9UABC::setTempControl
     */
    Fed9UVmeDevice& setTempControl(const Fed9UAddress& fpga, const Fed9UTempControl& temperatureSettings) noexcept(false);

    /**
     * Returns a Fed9UTempControl object which contain all the read/write settings on the LM82
     * temperature monitor, but not the read only settings.
     */
    Fed9UTempControl getTempControl(const Fed9UAddress& fpga)  const noexcept(false);

    /**
     * This reads back all the settings on the LM82 temperature monitor. It returns a
     * Fed9UTempControlInfo object which holds the read only and read/write settings.
     *
     * Fed9UTemControlInfo contains the read and writable settings (inherited from Fed9UTempControl), but
     * it also contains the read only register. The status register u32 can be masked with the enum
     * Fed9UTemperatureStatus, found in TypeDefs.hh, to check the individual status bits.
     */
    Fed9UTempControlInfo getTempControlInfo(const Fed9UAddress& fpga)  const noexcept(false);

    /**
     * The returned vector contains the status of the BE FPGA registers that contain
     * the event information.
     *
     * This returns an 4 element vector. The settings contained in the elements are as
     * follows:-
     * 0 Number of triggers received
     * 1 Number of frames stored in QDR memory
     * 2 Number of unread words in QDR
     * 3 Total frame count written to QDR
     */
    std::vector<u32> getBeEventCounterStatus()  const noexcept(false);

    /**
     * In the event that theLocalFedSettings gets totally out of sync with the FED
     * settings this method can be called to retrieve all the current infromation
     * stored on the FED and pass it to the theLocalFedSettings.  The only
     * information it does not get is what is stored in the EPROM.
     */
    Fed9UVmeDevice& updateLocalDescriptionFromFed() noexcept(false);

    /**
     * This method will prepare the FED to run in a DAQ mode specified in theLocalFedSettings
     * and setup the FED so that it will be ready to receive data once the start() method
     * has been called.
     *
     * This calls the stop method, so once initialised the start method must be called before data
     * can be taken. The strip settings (pedestals, disabled strips and cluster thresholds) and
     * the APV disable flag settings and the BE FPGA disable flag are not setup.
     */
    Fed9UVmeDevice& init(bool setClockAndReset=true) noexcept(false);

    /** 
     * This method prepares the FED for running in the fastest possible way.
     * The commands that are called are the following:
     * feFpgaSoftReset
     * beFpgaSoftReset
     * vmeFpgaSoftReset
     * stop() (note that stop also disables all the ADCs in order to reduce power consumption when fed is not running
     */
    Fed9UVmeDevice& fastInit() noexcept(false);

    /**
     * This method will initialise all the strips on a FED so that they are ready to take data.
     *
     * Initialises the pedestals, disabled strips, low and high cluster thresholds from theLocalFedSettings.
     */
    Fed9UVmeDevice& initStrips() noexcept(false);
    
    /**
     * This method will initialise the Fake Event RAMs on a FED so that they are ready to output fake events.
     *
     * Added all parameters after first 2 for use in FedPatternCheck.cc.  Can be left out to run the function as it did previously (AAB 8/15/2015)
     */
    Fed9UVmeDevice& initFakeEvent( bool useEventFile, bool blockUpload = false, std::string * inFEDString = NULL, std::string * inFEDString2 = NULL ,std::string * inFEDString4 = NULL, int uniqueChannels = 1, bool isZS = false ) noexcept(false);

    /**
     * This will initialise the temperature monitors, TTCrx device and the voltage monitor.
     */
    Fed9UVmeDevice& initAuxiliaryChips() noexcept(false);

    /**
     * This enables the FED so that it can start taking data.
     *
     * Sets the APV disable flag settings and the BE FPGA disable flag settings.
     */
    Fed9UVmeDevice& start() noexcept(false);

    /**
     * Stops the FED after it has performed a run
     *
     * This is done by disabling all the APVs and the BE FPGA.
     */
    Fed9UVmeDevice& stop() noexcept(false);

    /**
     * Scans through all the FED delay settings.
     *
     * It can either scan through a nano second range or fine and coarse skew setting range
     * for a given channel.
     */
    Fed9UVmeDevice& scanDelayValues(const Fed9UAddress& fedChannel, bool nanoDelay=true, bool scanCoarse=false) noexcept(false);

    /**
     * \brief  If there is an event in the buffer then an error is thrown.
     * \return Self reference.
     * \throw  Fed9UDeviceException Thrown if an event is found in the buffer.
     * \todo   Check if this is actually a good idea!
     */
    const Fed9UVmeDevice& checkEvent()  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the BE FPGA.
     */
    const Fed9UVmeDevice& backEndCheck()  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the FE FPGA.
     */
    const Fed9UVmeDevice& frontEndCheck(bool CMMedianOverridOnly=false)  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the VME FPGA.
     */
    const Fed9UVmeDevice& vmeCheck()  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the higher level commands.
     */
    const Fed9UVmeDevice& higherLevelCheck()  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the TTCrx device.
     */
    const Fed9UVmeDevice& ttcrxCheck()  const noexcept(false);

    /**
     * This method checks each byte of the EPROM memory to ensure that it is the same
     * as that stored in the Fed9UDescription.
     */
    const Fed9UVmeDevice& epromCheck()  const noexcept(false);

    /**
     * This method can be used to compare the values of the Fed9UDescrition to those
     * on the FED for the whole FED.
     */
    const Fed9UVmeDevice& fedCheck()  const noexcept(false);

    // JEC 10-04-06
    /** This method can be used to dump out all read/write FE & BE register information.
     *  Output is in the form of an ostream object, so can be used either to dump to
     *	the screen or to a file.
     */
    void dumpFedInformation(int dumpChoice, unsigned short Fpga, ostream &os) noexcept(false);

    /**
     * Returns a copy of the Fed9UDescription object.
     */
    Fed9UDescription getFed9UDescription() const { return theLocalFedSettings; }

    /**
     * Loads a new Fed9UDecription object into the Fed9UVmeDevice object, allowing
     * new FED setup to be loaded.
     *
     * The method takes the new Fed9UDescription object to be loaded, of which a
     * local copy is created (as in the constructor). There is also the option to
     * load these new parameters into the FED. This calls init, initStrips,
     * initAuxiliaryChips and setEprom. It is not possible to change FEDs or crates.
     * The base address and crate number should remain the same.
     */
    Fed9UVmeDevice& setFed9UVMEDeviceDescription(const Fed9UDescription& newFed9UDescription, bool initFed) noexcept(false);
    // <NAC date="23/04/2007"> 
    /**
     * Loads a new Fed9UDecription and sets FED registers. 
     *
     * The method takes the new Fed9UDescription object to be loaded, of which a
     * local copy is created (as in the constructor), and loads these new 
     * parameters into the FED. It is not possible to change FEDs or crates.
     * The base address and crate number should remain the same.
     */
    Fed9UVmeDevice& loadDescription(const Fed9UDescription& newDescription) noexcept(false);
    // </NAC>

    /**
     * Returns a const  reference to the local Fed Settings Fed9UDescription object.
     */
    Fed9UDescription& getFed9UVMEDeviceDescription() { return theLocalFedSettings; }

    /**
     * Returns a string containing the name of the halAddressTable that the VME device is using
     */
    //<JEC date=27/9/200>
    //    const std::string & getHalAddressTable() const { return theLocalFedSettings.getHalAddressTable(); }
    std::string getHalAddressTable() const { return theLocalFedSettings.getHalAddressTable(); }
    //</JEC>

    //<JEC date=16/03/07>
    /**
     * Sets the prescale for all triggers coming into the FED.  The method sets the prescale for the 
     * BE and all FE-FPGAs simultaneously, otherwise the FPGAs will get out of synch.  The input argument
     * is the actual prescale factor and is internally converted to the "skip N events" value of N that is
     * required by the firmware.
     */
    Fed9UVmeDevice& setFrameCapturePrescale(u32 prescale) noexcept(false);
    //</JEC>

  private:

    /**
     * This method is used to ensure that either the FE FPGA DAQ mode is the same on all the FE FPGAs
     * and that the BE FPGA is in the same mode as the FE FPGAs.
     *
     * A reference value for FE DAQ mode is passed and all other FE DAQ modes are compared to this value.
     * The BE FPGA is then compared to the FE FPGA mode to ensure they are consistant.
     */
    Fed9UDaqMode daqConsistancyCheck(const std::vector<u32>& feDaqMode, const u32 beDaqMode) const noexcept(false);

    /**
     * This method is used to ensure that either the FE FPGA DAQ mode is the same on all the FE FPGAs
     * and that the BE FPGA is in the same mode as the FE FPGAs.
     *
     * A reference value for FE DAQ mode is passed and all other FE DAQ modes are compared to this value.
     * The BE FPGA is then compared to the FE FPGA mode to ensure they are consistant.
     */
    Fed9UDaqSuperMode daqSuperModeConsistancyCheck(const std::vector<u32>& feDaqMode ) const noexcept(false);

    /**
     * Writes the LM82 temperature monitoring chip setting for a specific register.
     *
     * The FPGA can be 0-7, 9 or 10. The vector contains two elements. The first element
     * contains the new temperature value and the second element the register address.
     * The possible registers are :-
     * 0x09 - Writes to the configuration register. Only required when setting the critical temperature.
     * 0x0b - Writes the LM82 temperature high point.
     * 0x0d - Writes the FPGA temperature high point.
     * 0x5a - Writes the critical temperature set point.
     */
    Fed9UVmeDevice& tempControlWrite(u16 fpga_, const std::vector<u32>& tempDataAndRegAddr) noexcept(false);

    /**
     * Reads the LM82 temperature monitoring chip setting for a specific register.
     *
     * The FPGA can be 0-7, 9 or 10. The vector contains two elements. The first element
     * contains zero and is not used and the second element the register address.
     * The possible register address are :-
     * 0x00 - Reads the LM82 current temperature.
     * 0x01 - Reads the FPGA current temperature.
     * 0x05 - Reads the LM82 temperature high point.
     * 0x07 - Reads the FPGA temperature high point.
     * 0x42 - Reads the critical temperature set point.
     * 0xfe - Reads the LM82 manufacturers ID.
     */
    u32 tempControlRead(u16 fpga_, const std::vector<u32>& regAddress) const noexcept(false);

    /**
     * This is used to write to the configuration register on the temperature monitor chip.
     */
    Fed9UVmeDevice& tempControlConfig(const u16 fpga_, const u32 configSettings) noexcept(false);

    /**
     * Loads the complete set of FE FPGA disbale settings to the FED from the localFedSettings.
     */
    Fed9UVmeDevice& setAllFedFeUnitDisable() noexcept(false);

    /**
     * Loads all the temperature monitor data.
     */
    Fed9UVmeDevice& setAllTempControlData() noexcept(false);

    /**
     * Loads all the frame finding thresholds.
     */
    Fed9UVmeDevice& setAllFrameThresholds() noexcept(false);

    /**
     * Loads all the OptoRx controls.
     */
    Fed9UVmeDevice& setAllOptoRxControls() noexcept(false);

  public:

    /**
     * Loads all the median over-ride values from theLocalFedSettings to the FED.
     */
    Fed9UVmeDevice& setAllCmMedianOverrideValues() noexcept(false);

    /**
     * Loads the complete set of delay FPGA skews.
     */
    Fed9UVmeDevice& setAllDelays() noexcept(false);


    /**
     * Applies the global shift to all the channels in the FED.
     *
     * It has a maximum value of 25.
     */
    Fed9UVmeDevice & setGlobalFineSkew(u16 GlobalSkew) noexcept(false);

    /**
     * Returns the global shift that is applied to all the channels in the FED.
     */
    inline u16 getGlobalFineSkew()const{return theLocalFedSettings.getGlobalFineSkew();}

    /**
     * Loads one Fake Event into the specified APV RAM.
     *
     * 
     */
#ifdef EVENT_STREAMLINE
    Fed9UVmeDevice & setFakeEventFrame(Fed9UAddress & fedChannel, Fed9UEventStreamLine * fakeEvent, bool blockMode = true) noexcept(false);
#else
    Fed9UVmeDevice & setFakeEventFrame(Fed9UAddress & fedChannel, Fed9UEvent * fakeEvent, bool blockMode = true) noexcept(false);
#endif
   
    /**
     * returns a vector containing the fake event frame for a specific channel
     */
    std::vector<u32> getFakeEventFrame(Fed9UAddress & fedChannel) noexcept(false);

    u16 getFakeEventAddress(Fed9UAddress & fedChannel) noexcept(false);
    
    /**
     * return all Fake Event frames in the form of a Fed9UEvent object pointer
     * the memory for this object is managed by the user that receives the returned pointer.
     */
#ifdef EVENT_STREAMLINE
    Fed9UEventStreamLine * getAllFakeEvents() noexcept(false);
#else
    Fed9UEvent * getAllFakeEvents() noexcept(false);
#endif

    /**
     * Set the settings for the Fake Events
     *
     * Settings include tick level, Frame offset DCM low phase, DCM HiPhase, 
     * ( Frame Interval and Frame Repeat not yet implemented )
     */
#ifdef EVENT_STREAMLINE
    Fed9UVmeDevice & setFakeEventSettings(Fed9UAddress & feUnitChannelPair, Fed9UEventStreamLine * fakeEvent) noexcept(false);
#else
    Fed9UVmeDevice & setFakeEventSettings(Fed9UAddress & feUnitChannelPair, Fed9UEvent * fakeEvent) noexcept(false);
#endif
    /**
     * Loads all Fake Events into the FED for all FeUnits
     * for a specific Event passed in by reference
     * 
     */
#ifdef EVENT_STREAMLINE
    Fed9UVmeDevice & setAllFakeEvents(Fed9UEventStreamLine * fakeEvent, bool blockUpload = false) noexcept(false);
#else
    Fed9UVmeDevice & setAllFakeEvents(Fed9UEvent * fakeEvent, bool blockUpload = false) noexcept(false);
#endif
    
    Fed9UVmeDevice & setBlockModeUpload(bool value=true) noexcept(false);

    bool isStarted();
  private:

    /**
     * Loads the complete set of APV complement flags.
     */
    Fed9UVmeDevice& setAllComplements() noexcept(false);

    /**
     * Loads the complete set of Trim DAC offsets.
     */
    Fed9UVmeDevice& setAllTrimDacOffsets() noexcept(false);

    /**
     * Loads the all the strip data for a FED.
     *
     * It loads the pedestal values, the cluster thresholds and the number of valid channels.
     */
    Fed9UVmeDevice& setAllStripData() noexcept(false);

    /**
     * Loads the complete set of ADC settings from the FED.
     */
    Fed9UVmeDevice& setAllAdcControls() noexcept(false);

    /**
     * Loads the complete set of APV disable settings from theLocalFedSettings
     * to the FED.
     */
    Fed9UVmeDevice& setAllApvDisable() noexcept(false);

    /**
     * Disable all APVs.
     */
    Fed9UVmeDevice& disableAllApvs() noexcept(false);

    /**
     * This is used to compare two floating point numbers to see if they are equivalent.
     *
     * It only checks their accuracy to 3 d.p. as this is all that is required for the
     * voltage monitor floating point values.
     */
    bool floatEqual(float floatOne, float floatTwo) const;

    /*******************************************************************************
     *          These are none FPGA specific general private variables             *
     *******************************************************************************/

  protected:

    // <NAC date="23/05/2007"> changed theLocalFedSettings from reference to real object to prevent memory leak when Fed9UVmeDevice constructor throws an exception.
    // No extra copying is involved as the description was coppied anyway in the constructor.
    Fed9UDescription theLocalFedSettings; //!< Parameter settings in the FED.
    // </NAC>

    bool _isStarted;  // !< this variable is set to true when the method start() is called and false when stop() is called
  private:

    /**
     * Fed9UVmeBase has a read/write switch depending on what the method is to do.
     */
    enum { WRITE = false, READ = true };

    const std::auto_ptr<Fed9UVmeBase> theFed; //!< Base class for configuring FED settings.

    HAL::StopWatch timer1;
    HAL::StopWatch timer2;
    u32 timer1Value,timer2Value;
    u32 methodCallCounter;

    bool blockModeUpload_;         //!< Use block mode upload for VME transfers.
    bool mHaveUploadedNewFirmware; //!< Indicates that a new firmware was uploaded during construction.
    
    typedef void (*sighandler_t)(int);
    sighandler_t _oldHandler;


    static const u16 _fineSkewMap[25];     //!< Mapping of integer nanosecond skews to delay chip settings, using 5 bit fine skew values.
    static const u16 _fineSkewMap6Bit[25]; //!< Mapping of integer nanosecond skews to delay chip settings, using 6 bit fine skew values.

    static std::vector<u32> dummyVector;   //!< Created for efficiency, the Fed9UVmeBase requires a vector for the read/write argument
                                           //!< even when a write/read is being performed. Save constructing a local object in each member function.
    static u32 dummyWord;                  //!< Created for efficiency, the Fed9UVmeBase requires an u32 for the read/write argument
                                           //!< even when a write/read is being performed. Save constructing a local object in each member function.

    static bool handleControlC;       //!< If true then the method will be reqiured to handle a control+C signal in order to exit in a safe state. 
    static bool controlCCaught;       //!< If true then a control+C signal has been caught and may optionally be required to be dealt with.
    static void ControlCHandler(int); //!< A static member function that provides the class mechanism for dealing with a control+C signal.

    static pthread_mutex_t fileMutex; //!< This is used to lock a file while a particular thread is creating a local copy of the file.

    static pthread_mutex_t multiFedAccessMutex; //!< This is used to lock the FED access when operating with threads and doing things like resetting feds etc, it's important that no VME accesses occur at certain times when the FEDs could be affecting the bus due to being reset etc..

    static std::ios::pos_type startCfDataMarker; //!< Marks the start of the compact flash image data in the global firmware .ace file.
                                                 //!< Used with fstream seekg. -1 if not set and only valid if cfFileParsed is true.
    static std::ios::pos_type endCfDataMarker;   //!< Marks the end of the compact flash image data in the global firmware .ace file.
                                                 //!< can be compared with fstream tellg. -1 if not set and only valid if cfFileParsed is true.
    static bool cfFileParsed;                    //!< True if the global firmware file has been parsed, hence start/endCfDataMarkers will be valid.

    static u16 firmwareUploadInProcessCount; //!< keep track of how many Fed Objects are currently uploading firmware for use in multi threaded situations.


  };
}

#endif
#ifndef H_Fed9UDeviceException
#define H_Fed9UDeviceException

#include "ICExDecl.hh"
#include <string>

namespace Fed9U {

  /**
   * \class Fed9UDeviceException
   * \brief Exception class to be used when throwing errors within the Fed9UDevice class
   * \author Jonathan Fulcher 
   * \author Emlyn Corrin
   * \version 0.0
   * \date 16 Jul 2003
   * 
   ***********************************************/

#define FED9U_DEVICE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED_READOUT, "Unable to read event from FED") \
  IC_DEF_ERROR(ERROR_FED_TRIM_DAC_CAL, "Unable to complete TrimDAC calibration routine") \
  IC_DEF_ERROR(ERROR_FED_INIT, "FED failed to initialize") \
  IC_DEF_ERROR(ERROR_FED_SOAK_TEST, "Fed9UDevice::soakTest aborted. Unable to handle exception.") \
  IC_DEF_ERROR(ERROR_MEMORY, "Failed to allocate memory.") \
  IC_DEF_ERROR(ERROR_FED_REGISTER_READ, "Error reading FED register.") \
  IC_DEF_ERROR(ERROR_FED_SPY_READOUT, "Failed to read spy channel data.")

  IC_EXCEPTION_CLASS_BEGIN(Fed9UDeviceException, FED9U_DEVICE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif // H_Fed9UDeviceException
#ifndef H_FED9UDevice
#define H_FED9UDevice

#include <csignal>
#include <sstream>
// <NAC date="30/04/2009">
#include <boost/shared_array.hpp>
// </NAC>
namespace Fed9U {
  //using.*std::ostringstream;

  std::string getFed9ULibVersion();
  unsigned    getFed9ULibVersionMajor();
  unsigned    getFed9ULibVersionMinor();

  class Fed9UDevice : public Fed9UVmeDevice {
  public:
    Fed9UDevice(const Fed9UDescription& fed9UDescription, u32 adaptorSlot=0);
    ~Fed9UDevice();
  
    //void init();
    //void start();
    //void stop();
    //u8 hasEvent();
    //void sendSoftwareTrigger();
    //void getCompleteEvent(u32* buffer, u32 bufferSize, u32& numU32sAddedToBuffer, u32& eventCount, bool blockDisable = false);
    void softReadoutLoop();
    Fed9UCounters getCounters(Fed9UAddress feChan = Fed9UAddress(0)) {
      return Fed9UCounters(getBeEventCounterStatus());
      //_fed->getChannelBufferOccupancy(feChan));
    }
    void setChannelDelay(Fed9UAddress channel, u16 value) {
      ICUTILS_VERIFY(value < 32*16)(value);
      setDelay(channel, value / 32, value % 32);
    }
    



    void setAllDelays(u8 value){
      ICUTILS_VERIFY(value < 25)(value);
      Fed9UAddress channel;
      for (u32 j=0 ; j<CHANNELS_PER_FED ; j++ ) {
	channel.setFedChannel(j);
	setDelay(channel,_fed9UDescription->getCoarseDelay(channel),((_fed9UDescription->getFineDelay(channel)+value)%25)&0x1F);
      }
    }

    Fed9UVmeDevice & getFed9UVmeDevice() { return *this; }
    Fed9UDescription & getFed9UVMEDeviceDescription() { return theLocalFedSettings; }
    Fed9UDescription & getFed9UDeviceDescription() { return *_fed9UDescription; }

    void getCompleteBufferedEvent( Fed9UBufferedEvent & bev);

    /**
     * The soak test will write to all read/write registers on the FED and then check
     * that the write was successful.
     *
     * If any errors are found they will be written to the log file. No User action is
     * required. The method takes the number of different description files to be tested.
     * The description files are generated by the class Fed9UCreateDescription. Testing
     * the EPROM doubles the time taken to run the soak test and the User is given the option
     * not to test it. It is defaulted to true, which will test the EPROM. If it is false
     * the EPROM will not be tested.
     */
    Fed9UDevice& soakTest(u32 numberOfTests, bool testEprom = true);

    Fed9UDevice& setBlockModeReadout(bool bmr) {
      _blockModeReadout = bmr;
      return *this;
    }

    bool getBlockModeReadout() {
      return _blockModeReadout;
    }

    /**
     * Fast Hardcore Purge of the event buffer.
     *
     * Removes any existing events from the buffer but does not return the number purged. This is a fast and hard core purge method!
     */
    u32 purgeEvents() noexcept(false);
    
    // <NAC date="30/04/2009"> readout of complete spy events packed with header and trailer
    // <NAC date="15/02/2010"> added 1/N mode
    /**
     * Read a complete spy event from the FED and pack with DAQ and Tk Special header. 
     * Throws if the spy channel is armed during readout (and so a complete event could not be built)
     * set allowMixedEvent to true to allow an incomplete event to be read
     * set pEventCountBeforeRead and pL1ACountBeforeRead to get the eventCount and l1aCount for the packet
     * set oneOverNMode to write the current values of the 1la and qdr total frame count registers to the packet,
     * instead of the latched ones
     */
    // <NAC date="10/02/2010"> added run number to spy data
    boost::shared_array<u8> readCompleteSpyEvent(const std::vector<bool>& delayChipsEnabled, const u32 lvl1ID, 
                                                 const uint32_t runNumber = 0,
                                                 const bool allowMixedEvent = false,
                                                 u32* pEventCountBeforeRead = NULL, u32* pL1ACountBeforeRead = NULL,
                                                 const bool oneOverNMode = false) noexcept(false);
    // </NAC>
    // </NAC>
    // </NAC>

  private:
    Fed9UDevice(const Fed9UDevice &);
    Fed9UDevice & operator = (const Fed9UDevice &);
    // <NAC date="23/05/2007"> changed to auto_ptr to avoid leaks in contructor when an exception is thrown
    std::auto_ptr<Fed9UDescription> _fed9UDescription;
    // </NAC>
    //Fed9UVmeDevice* _fed;
    u32 _eventNumber;
    static volatile bool _isTakingData;
    static void ControlCHandler(int);
    typedef void (*sighandler_t)(int);
    sighandler_t _oldHandler;
    std::ostringstream _errorStr;
    bool _blockModeReadout;

    /**
     *
     * Routines for TrimDAC auto calibration are below here.
     * added by M. Noy, 09-03-2004.
     *
     */
  public:
  

    /**
     * Description:
     *
     * This function will try to set the appropriate TrimDAC setting
     * on each enabled FED channel individually, where the data
     * go depends on the value of mode (see below).
     *
     *
     *
     * Requires: init() to have been called.
     *           inputs/fibres should be dark
     *
     *
     * Arguments:
     *
     *         mode: Argument that decides how the calibrated
     *               values are dealt with after the calibration
     *               finishes.
     *
     *               mode==0: FED is left in calibrated config.
     *                        and the Fed9UDescription in
     *                        Fed9UVmeDevice is updated.
     *
     *               mode==1: Values originally placed in FED are
     *                        restored after calibration and calibration
     *                        results are placed in the Fed9UDescription
     *                        belonging to Fed9UDevice.
     *
     *
     *         UpperThresh: ADC count level that signal must be
     *                      below to be considered calibrated.
     *         LowerThresh: ADC count level that signal must be
     *                      above to be considered calibrated.
     *
     *
     *
     * M. Noy
     * 09-03-2004 Initial
     * 22-03-2004 Default arguments added 
     *
     */

    void trimDACInternalCalibration(u16 mode=0, u16 UpperThresh=60, u16 LowerThresh=30);
  
private:
  
    /**
     * This function will estimate the TrimDAC offset 
     * that should place a dark channel in the centre 
     * of the ADC dynamic range for given OptoRX settings. 
     * It should remain private since it is for 
     * FED internal calibration use.
     *
     * Arguments:
     * u16 Xpre: OptoRX Input Offset Current
     * u16 Xpost: OptoRX Output Offset Current
     * u16 Rload: OptoRX Load Resistor Value (Ohms)
     *
     * return: TrimDAC Offset count.
     *
     * M. Noy 
     * 09-03-2004
     */
    u16 estimateTrimDACStartPoint(u16 Xpre, u16 Xpost, u16 Rload);
    
    /**
     * This function should not exist.
     * 
     * I requested that the pre- and post- OptoRX offsets be obtainable
     * separately for a reason, and it seems that they aren't.
     * 
     *
     * M. Noy 
     * 09-03-2004
     */
    void separateOptoRXOffsets(u16 combined, u16 & Xpre, u16 & Xpost);
    
    /**
     * This function evaulates whether the internal TrimDAC calibration has finished.
     *
     * Arguments:
     *
     * vector<u16> & status: vetor of status bits
     *
     * returns: 
     * u8: with result of evaluation. 
     * 
     * M. Noy 
     * 09-03-2004
     */
    u8 trimDACCalibrationHasFinished(const vector<u16> & status);
    
    
    /**
     * This function fills the reference with an event for 
     * the TrimDAC internal calibration.
     * 
     * Arguments:
     *
     * Fed9UBufferedEvent * ev
     *
     * M. Noy 
     * 10-03-2004
     */
    void getTrimDACCalibrationEvent(Fed9UBufferedEvent & ev);
    
      
    /**
     * This function calculates a u16 casted mean of a vector
     * of scope mode data for the TrimDAC internal calibration
     * 
     * Arguments:
     * vector<u16> & data): vector holding the data.
     * 
     *
     * M. Noy 
     * 10-03-2004
     */
    u16 getTrimDACCalibrationChannelMean(vector<u16> & data);
    

  };
}

#endif // H_FED9UDevice
#ifndef Fed9UCrateStatusProbe_HH
#define Fed9UCrateStatusProbe_HH

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#define UNSET_SN 0

#ifdef TBHARDWARE
#endif

//class std::ostringstream;
  
namespace Fed9U
{

  /** 
   * \brief Fed9UCrateStatus: container class for the Fed9UEntry
   * \author Jonathan Fulcher
   * \author Matt Noy
   * \version 0
   * \date 1 April 2004
   *
   * Fed9UCrateStatus: container class for the Fed9UEntry
   *
   *abstraction of a vector that should
   *provide the ability to detect the crate,
   *and iterate the testing sequence over 
   *the crate entries.
   *
   *
   *
   *18-05-2004
   *public member prependEntry() added, to prepend a Fed9UEntry;
   **/
  class Fed9UCrateStatusProbe : public Fed9UCrateStatus
  {
    
  public:

    /**
     * \brief Default constructor.
     */
    //Fed9UCrateStatusProbe();

    /**
     * \brief Destructor
     */
    ~Fed9UCrateStatusProbe();

    /**
     * \brief Probes the crate for FEDs adding a Fed9UEntry to the sequence for each one found.
     * \param pDesc Pointer to the Fed9UDescription object used to construct Fed9UDevice objects to communicate with the FEDs.
     *        Should have the hardware ID set to 0 or 998 to run the scan. See Fed9UDescription::setHardwareId for more details. Default null.
     * \param start First slot number to be probed. Default 1.
     * \param end Last slot number to be probed. Default 21.
     * \param KeepFeds If boolean true places a valid pointer to the Fed9UDevice into the FedEvent (which then assumes ownership).
     *        If boolean false FED objects are deteleted once crate probe is complete. Default false.
     * \param KeepDescriptions If boolean true the description pointed to by pDesc is left for calling function to delete. If boolean false
     *        it will be deleted at the end of probe crate and the pointer assigned to NULL. Default false
     * \param showTemps If boolean true then the temperatures will be read back from the FED and values displayed if the temperatures are out of range. Default false.
     * \param useThreads Each slot can be probed in its own thread if threading is supported on the system. Default true.
     */
    void probeCrate(Fed9UDescription * pDesc = NULL, unsigned long start=1, unsigned long end=21, bool KeepFeds=false, bool KeepDescriptions=false, bool showTemps=true, bool useThreads=false, bool adcsOff=true );

  private:
    /**
     * \brief  Probes a specific slot in a crate to determine if a FED is present.
     * \param  param A void pointer to the parameter list. The parameters it expects:
     *         \li 0: Pointer to the description being used.
     *         \li 1: Pointer to an area of memory where the FED object will be stored, should be NULL as it will deleted if not.
     *                It will deleted if parameter three is set false, once the member function is complete.
     *         \li 2: Slot number that is to be probed.
     *         \li 3: If boolean true any FED object created is kept and can be accessed through pointer given in parameter 1. 
     *                If boolean false FED objects are deleted once crate probe is complete and pointer given in parameter 1 is set to NULL.
     *         \li 4: If boolean true then the temperatures will be read back from the FED and values displayed if the temperatures are out of range.
     *         \li 5: Counter that is incremented each time a new instance of this static member function is created and decremeneted when it finishes.
     *         \li 6: A pointer to a boolean that if true means that all the threads have finished their initialisation routine and it is safe
     *                to continue with the thread.
     *         \li 7: Pointer to an ostringstream that all the thread output is written to. As cout is not thread safe the main thread must write to it.
     * \return void* Returns null on all returns from the program. It has no means, it is required to be a void* for thread creation.
     *
     * This member function has been written in mind for being spwaned in multiple threads, allow each slot in a crate to be probed almost
     * simulataneously.
     */
    static void* probeSlot(void* param);

    static u32 mRunningProbe; //!< Used to keep track of the number of slots being probed.

 }; // class Fed9UCrateStatus

} // namespace Fed9U

#endif // Fed9UCrateStatusProbe_HH
#ifndef Fed9UMoFO_HH_
#define Fed9UMoFO_HH_



//
// class Fed9UMoFO 
// Author: Matt Noy
// Date: June 2004
// Purpose: Provides a configurable monitoring 
//          handle that enables monitoring of the Fed. 
//  
//


namespace Fed9U
{
  
  class Fed9UDevice;
  
  class Fed9UMoFO
  {
  public:
    
   
    
    //
    // Hardware monitor bit field
    //
    enum MONITOR_FLAGS{COUNTERS=1, TEMPS=2, VOLTAGES=4, TTC=8, CLOCKSRC=16, BE_STATUS=32};


    //
    // constructor for the Fed9UMoFO class. 
    // Counters, Temperature, and Voltage are all 
    // monitored by default. The class doesn't assume
    // ownership of the Fed9UDevice it points to
    //    

//     Fed9UMoFO(Fed9UDevice * pFed, DATA_DESTINATION dest, bool MonCounters=true, 
// 		   bool MonTemp=true, bool MonVolt=true);

    Fed9UMoFO(Fed9UDevice * pFed,
	      u32 Flags=COUNTERS|TEMPS|VOLTAGES|TTC|CLOCKSRC|BE_STATUS);


    ~Fed9UMoFO(){}

    //
    // populates the containers with data from the fed passed to the class
    //
    void Load();

    //
    // 
    //
    void Send(std::ostream * os);

    // <NAC date="08/05/2007"> made all const
    //<JEC date=05-02-07> add get methods for private member variables for monitoring interface to Fed9USupervisor
    inline Fed9UCounters getCounterValue() const{return mFedCounters;}
    inline std::vector<Fed9UTempControlInfo> getTemperatureValues() const{return mTempInfo;}
    inline Fed9UVoltageControlInfo getVoltageValues() const{return mVoltageInfo;}
    inline Fed9UTtcrxDescriptionInfo getTTCRxInfo() const{return mTtcInfo;}
    inline Fed9UClockSource getClockSourceInfo() const{return mClkSource;}
    //</JEC>
    // </NAC>
    // <NAC date="07/05/2007"> added get method to get TempControlInfo for an FPGA by giving its address
    Fed9UTempControlInfo getTemperatureValues(const Fed9UAddress& fpga) const;
    // </NAC>

    //
    // get the monitor state
    //
    inline bool getCountersFlag()const{return mMonitorFlags&COUNTERS;}
    inline bool getTemperatureFlag()const{return mMonitorFlags&TEMPS;}
    inline bool getVoltageFlag()const{return mMonitorFlags&VOLTAGES;}    
    inline bool getTtcFlag()const{return mMonitorFlags&TTC;}
    inline bool getClockSrcFlag()const{return mMonitorFlags&CLOCKSRC;}
    
    //
    // get all the flags at once
    //
    inline u32 getMonitorFlags()const{return mMonitorFlags;}


    //
    // set the monitor state individually
    //
    inline void setCountersFlag(bool c){c?mMonitorFlags|=COUNTERS:mMonitorFlags&=~COUNTERS;}
    inline void setTemperatureFlag(bool t){t?mMonitorFlags|=TEMPS:mMonitorFlags&=~TEMPS;}
    inline void setVoltageFlag(bool v){v?mMonitorFlags|=VOLTAGES:mMonitorFlags&=~VOLTAGES;}
    inline void setTtcFlag(bool ttc){ttc?mMonitorFlags|=TTC:mMonitorFlags&=~TTC;}
    inline void setClockSrcFlag(bool cs){cs?mMonitorFlags|=CLOCKSRC:mMonitorFlags&=~CLOCKSRC;}
    
    //
    // set all flag at once
    //
    inline void setMonitorFlags(u32 Flags){mMonitorFlags=Flags;}    

  private:

    Fed9UMoFO(){}
    Fed9UMoFO(const Fed9UMoFO & m){}
    //operator=();

  private:


    //
    // configuration parameters
    //
    Fed9UDevice * mpFed;
    u32 mMonitorFlags;
    bool mHasNewData;
       

    //
    // data containers
    //
    Fed9UCounters mFedCounters;
    //Vector of Fed9UTempControlInfos. First FEUNITS_PER_FED are for FE units ordered by internal numbering. Next is BE them VME. (NAC 09/05/2007)
    std::vector<Fed9UTempControlInfo> mTempInfo;
    Fed9UVoltageControlInfo mVoltageInfo;
    Fed9UTtcrxDescriptionInfo mTtcInfo;
    Fed9UClockSource mClkSource;
    u32 mBeStatus;


  };

} // namespace Fed9U

#endif // Fed9UMoFO_HH_
