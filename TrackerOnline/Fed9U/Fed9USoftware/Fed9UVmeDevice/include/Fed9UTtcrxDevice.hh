#ifndef _Fed9UTtcrxDevice_H_
#define _Fed9UTtcrxDevice_H_

//Author: Gareth Rogers RAL
//Date: December 2003

/**
 * Class for TTCrx configuration.
 * This class contains methods that can be used to configure
 * and read the status of the TTCrx chip.
 *
 * Every effort has been made to make this class fully documented. However for those
 * methods that are unclear or if not enough detail has been provided more information
 * can be found in the TTCrx Reference Manual - A Timing, Trigger and Control Reference
 * ASIC for LHC Detectors. The types of exceptions that can be expected to be thrown are
 * Fed9UVmeDeviceExceptions.
 */

#include "Fed9UTtcrxDescription.hh"
#include "TypeDefs.hh"

//#include <vector>

namespace Fed9U {

  class Fed9UVmeBase;

  class Fed9UTtcrxDevice {

    /**
     * The public constructor and destructor that is provided with the TTCrx device class.
     */
  public:

    /**
     * The Fed9UTtcrxDevice constructor.
     *
     * The Fed9UTtcrxDevice object passed as an arguement is used internally to retrieve
     * settings when writing to the TTCrx. The Fed9UVmeBase is required to give the TTCrx device
     * access to the TTCrx on the FED.
     */
    Fed9UTtcrxDevice(Fed9UVmeBase& fed, const Fed9UTtcrxDescription& ttcrx);

    /**
     * The Fed9UTtcrxDevice destructor.
     */
    ~Fed9UTtcrxDevice();

    /**
     * These constructors are not implemented and cannot be used with the TTCrx device class.
     */
  private:

    /**
     * The copy constructor and assignment operator have been declared private for this class.
     * This has been done because of the use of references to the Fed9UVmeBase class and
     * Fed9UDescription class. Copying or assignment would cause multiple references to refer
     * to the same object. Therefore the compiler generated methods would be of no use.
     */
    Fed9UTtcrxDevice(const Fed9UTtcrxDevice&);
    Fed9UTtcrxDevice& operator=(const Fed9UTtcrxDevice&);

    /**
     * Public methods used to read and write to the TTCrx chip.
     */
  public:

    /**
     * Will reset the TTCrx chip.
     *
     * An internal reset use the I2C interface to send the reset, an external reset
     * does not. Both perform the same level of reset. You cannot reset the TTCrx
     * while the TTC clock is selected. If internal is true then an internal reset
     * will be sent. If it is false then an external reset will be sent.
     */
    Fed9UTtcrxDevice& sendTtcrxReset(bool internal);

    /**
     * This method will perform a reset on the error counters, bunch counter, event counter and the status
     * register. The register to be reset will be determined by the Fed9UTtcrxCounterReset struct.
     *
     * If the struct contains a true for a specific reset then that reset will be performed. A false and
     * it will not be performed.
     */
    Fed9UTtcrxDevice& setCounterReset(const Fed9UTtcrxCounterReset& counterReset);

    /**
     * This set the total amount of delay skew for the L1Accept, broadcast strobe 1
     * signal, the bunch and event counter reset signals and the system broadcast
     * command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns. The fine
     * delay can be set from 0 to 239, with each step representing approximately 104ps.
     */
    Fed9UTtcrxDevice& setL1AcceptDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * This will set the total amount of delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * This sets the coarse delay for those signals, where the coarse delay can be
     * set from 0 to 15. Each step represents 25ns. It sets the fine delay for either
     * the 40MHz deskewed clock one or two depending on which the signals are
     * synchronised to. This can be set in element two fo the control register vector.
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDevice& setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * This will set the coarse delay skew for the L1Accept, broadcast strobe 1 signal,
     * bunch and event counter reset signals and the system broadcast command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns.
     */
    Fed9UTtcrxDevice& setL1AcceptCoarseDelay(u16 coarseDelay);

    /**
     * This will return the coarse delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command.
     *
     * The coarse delay can be 0 to 15. Each step represents 25ns.
     */
    u16 getL1AcceptCoarseDelay() const;

    /**
     * This will set the coarse delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * The coarse delay can be set from 0 to 15. Each step represents 25ns.
     */
    Fed9UTtcrxDevice& setBrcstStrTwoCoarseDelay(u16 coarseDelay);

    /**
     * This will set the coarse delay skew for the broadcast strobe 2 signal
     * and the user broadcast command.
     *
     * The coarse delay can be 0 to 15. Each step represents 25ns.
     */
    u16 getBrcstStrTwoCoarseDelay() const;

    /**
     * This will set the fine delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if element 2 in the control register is disabled.
     *
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDevice& setClockDesOneFineDelay(u16 fineDelay);

    /**
     * This will return the fine delay skew for the deskewed 40MHz clock one.
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if element 2 in the control register is disabled.
     *
     * The fine delay can be 0 to 239, with each step representing
     * approximately 104ps.
     */
    u16 getClockDesOneFineDelay() const;

    /**
     * This will set the fine delay skew for the deskewed 40MHz clock two.
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled by setting element 2 in the control register vector.
     *
     * The fine delay can be set from 0 to 239, with each step representing
     * approximately 104ps.
     */
    Fed9UTtcrxDevice& setClockDesTwoFineDelay(u16 fineDelay);

    /**
     * This will return the fine delay skew for the deskewed 40MHz clock two.
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled by setting element 2 in the control register vector.
     *
     * The fine delay can be 0 to 239, with each step representing
     * approximately 104ps.
     */
    u16 getClockDesTwoFineDelay() const;

    /**
     * Sets the charge pump current used in the delay-lock loops of the high
     * resolution phase shifters.
     *
     * It has a maximum value of 7 and a minimum value of 1.
     */
    Fed9UTtcrxDevice& setDllPumpCurrent(u16 pumpCurrent);

    /**
     * Returns the charge pump current used in the delay-lock loops of the high
     * resolution phase shifters.
     */
    u16 getDllPumpCurrent() const;

    /**
     * Sets the charge pump current used in the phase-lock loops which provide
     * the clock recovery.
     *
     * It has a maximum value of 7 and a minimum value of 1.
     */
    Fed9UTtcrxDevice& setPllPumpCurrent(u16 pumpCurrent);

    /**
     * Returns the charge pump current used in the phase-lock loops which provide
     * the clock recovery.
     */
    u16 getPllPumpCurrent() const;

    /**
     * This sets the 14 bit individually addressed command (IAC) ID.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link. It has a maximum value of 16383.
     */
    Fed9UTtcrxDevice& setIacId(u16 id);

    /**
     * This returns the 14 bit individually addressed command (IAC) ID.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link. It has a maximum value of 16383.
     */
    u16 getIacId() const;

    /**
     * This sets the 6 bit I2C ID.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     * It has a maximum value of 64.
     */
    Fed9UTtcrxDevice& setI2cId(u16 id);

    /**
     * This returns the 6 bit I2C ID.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     * It has a maximum value of 64.
     */
    u16 getI2cId() const;

    /**
     * This is used to control enable or disable the bunch and event counter operations.
     * The individual settings can be enabled or disabled using the enum
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh. Both flags can be set using a bitwise
     * OR and any flag not set will be disabled.
     *
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6.
     */
    Fed9UTtcrxDevice& setCounterOperation(u16 counterOperation);

    /**
     * This is used to the bunch and event counter operations enable or disable flags.
     * The individual settings can be checked by using the masks provided in
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh.
     *
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6. Disabling these can be used to save power.
     */
    u16 getCounterOperation() const;

    /**
     * This can be used to enable or disable the hamming checking.
     *
     * A true will disable the hamming checking and a false will enable hamming checking.
     */
    Fed9UTtcrxDevice& setHammingCheckingDisable(bool disableHammingChecking);

    /**
     * This can be used to read the status of the hamming checking.
     *
     * A true means that hamming checking is disabled and a false enabled.
     */
    bool getHammingCheckingDisable() const;

    /**
     * This selectes the 40MHz deskewed clock two. If selected it is used to sychronise
     * the broadcast strobe 2 signal and the user broadcast command. A true will select
     * this clock.
     */
    Fed9UTtcrxDevice& setDeskewedClock2Selected(bool selectClockTwo);

    /**
     * If a true is returned then the broadcast strobe 2 signal and the user broadcast
     * command are synchronised to the 40MHz deskewed clock two.
     */
    bool getDeskewedClock2Selected() const;

    /**
     * If a true is passed this will disable the 40MHz deskewed clock two.
     * If false the clock will be disabled.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDevice& setDeskewedClock2Disable(bool disableClockTwo);

    /**
     * If a true is returned then the deskewed clock two is disabled. If
     * false it is enabled.
     */
    bool getDeskewedClock2Disable() const;

    /**
     * Disable the L1Accept clock output.
     *
     * A true will disable the output and a false enable it.
     * This can be disabled to save power.
     */
    Fed9UTtcrxDevice& setClockL1AcceptDisable(bool l1AcceptDisable);

    /**
     * Returns the setting on the L1Accept clock disable flag.
     *
     * A true means it is disabled and a false enabled.
     */
    bool getClockL1AcceptDisable() const;

    /**
     * This will disable the non-deskewed 40MHz clock output if a
     * true is passed to the function. A false will enable it.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDevice& setNonDeskewedClockDisable(bool disable);

    /**
     * If true then the non-deskewed 40MHz clock is disabled.
     * False and it is enabled.
     */
    bool getNonDeskewedClockDisable() const;

    /**
     * Disable the parrallel bus output if true.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDevice& setParrallelOutputDisable(bool disable);

    /**
     * Return true if the parrallel bus output is disabled.
     */
    bool getParrallelOutputDisable() const;

    /**
     * Disable the serial B output if true.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDevice& setSerialBDisable(bool disable);

    /**
     * Return true if the serial B output is disabled.
     */
    bool getSerialBDisable() const;

    //Depreciated. \todo - to be removed in later versions.
//     /**
//      * This is used to control the operational mode of the TTCrx chip. It can also be
//      * used to reduce power consumption by disabling some of the chips features.
//      *
//      * The control register is set using an eight element vector. A description of the elements
//      * is given below.
//      * 0 Enable bunch counter operation - default true.
//      * 1 Enable event counter operation - default true.
//      * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//      * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//      * explanation.
//      * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//      *                        synchronise the Broadcast Strobe 2 - default false.
//      * 3 Enable Deskewed clock 2 output   - default false.
//      * 4 Enable L1Accept clock output - default true.
//      * 5 Enable Parallel output bus  - default false.
//      * 6 Enable Serial B output       - default false.
//      * 7 Enable (non-deskewed) 40MHz clock output - default true.
//      * In all cases a true is used to enable the setting and false to disable.
//      */
//     Fed9UTtcrxDevice& setControlRegister(std::vector<bool> controlRegister);

//     //Depreciated. \todo - to be removed in later versions.
//     /**
//      * This is used to read back the operational mode of the TTCrx chip.
//      *
//      * The settings are returned in an eight element vector. A description of the elements
//      * is given below.
//      * 0 Enable bunch counter operation - default true.
//      * 1 Enable event counter operation - default true.
//      * The settings in elements 0 and 1 are used in conjunction to specify the mode of the
//      * level 1 trigger sequence. Refer to the TTCrx Reference Manual chapter 6 for a detailed
//      * explanation.
//      * 2 Select Deskewed clock 2 - Determines whether the Deskewed clock 1 or Deskewed clock 2 is used to
//      *                        synchronise the Broadcast Strobe 2 - default false.
//      * 3 Enable Deskewed clock 2 output   - default false.
//      * 4 Enable L1Accept clock output - default true.
//      * 5 Enable Parallel output bus  - default false.
//      * 6 Enable Serial B output       - default false.
//      * 7 Enable (non-deskewed) 40MHz clock output - default true.
//      * In all cases a true is used to enable the setting and false to disable.
//      */
//     std::vector<bool> getControlRegister() const;

    /**
     * This returns the number of bunch crossing that have occured since the last reset.
     *
     * It counts the number of clock signals that are received by the TTCrx. It is a 12
     * bit register and so has a maximum value of 4095.
     */
    u16 getBunchCounter() const;

    /**
     * This returns the number of events that have been received since the last reset.
     *
     * It counts the number of trigger signals that are received by the TTCrx. It is a 24
     * bit register and so has a maximum value of 16777215.
     */
    u32 getEventCounter() const;

    /**
     * This return the number of single bit errors that are recignised and corrected by the
     * receiver's Hamming decoder.
     */
    u16 getSingleBitErrorCount() const;

    /**
     * Returns the number of double bit Hamming and frame errors.
     *
     * Once it reaches its maximum value of 255 the register will need to be reset.
     */
    u16 getDoubleBitErrorCount() const;

    /**
     * Returns the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * It has a maximum value of 255.
     */
    u16 getSeuErrorCount() const;

    /**
     * This is a read only value that determines the over all operating mode of the circuit.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeA() const;

    /**
     * This is a read only value that determines the over all operating mode of the circuit.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeB() const;

    /**
     * This is a read only register, which selects the test outputs.
     *
     * This register is used for testing and debugging the TTCrx. It
     * has a maximum value of 7.
     */
    u16 getTestOutputs() const;

    /**
     * This is a read only register, which selects the number of clock
     * cycles after which the frequency phase detection will stop.
     *
     * It is a testing and debugging register. It has a maximum value
     * of 7 and a hardware default value of 7.
     */
    u16 getFreqDetectCount() const;

    /**
     * This is a read only configuration register. It contains some testing
     * and debugging settings. The enum Fed9UTtcrxConfig can be used to set
     * these bits. Any setting not set will be cleared.
     *
     * A description of the bits is given below. As these are read only on the
     * TTCrx, what is read back should correspond to the defaults given below.
     * These defaults are also the settings used by the TTCrx.
     * FED9U_TTCRX_CONFIG_SEL_TEST_PD - Selects the external test signal for enabling
     *                                  the PPL phase detector. Default is 0
     * FED9U_TTCRX_CONFIG_SEL_INPUT_A - When 1 selects the inputs from option link,
     *                                  otherwise test_in. Default is 1.
     * FED9U_TTCRX_CONFIG_PLL_RESET   - Assert PLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DLL_RESET   - Assert DLL test reset line. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_FREQ_INCR - If 1 disables the automatic frequency increase
     *                                        after a PLL reset. Default 0.
     * FED9U_TTCRX_CONFIG_DISABLE_WATCHDOG  - If 1 disbale watchdog circuit. Default 0.
     * FED9U_TTCRX_CONFIG_ENABLE_HAMMING_CORRECTION - Enable hamming error correction on
     *                                                incoming data stream. Default 1.
     * FED9U_TTCRX_CONFIG_ENABLE_TEST_INPUTS - Enables test input/outputs. Default 0.
     */
    u16 getConfigSettings() const;

//     //\todo - depricated, delete.
//     /**
//      * This is a read only configuration register. The settings here control the
//      * behaviour of the TTCrx.
//      *
//      * The returned vector has 10 elements. The contents of the elements is:-
//      * 0 Shows which test outputs have been selected.
//      * 1 Whether an external test signal is enabled for the phase-lock loop phase detector.
//      * 2 When 1 selects input from the optical link.
//      * 3 Assert phase-lock loop test reset line.
//      * 4 Assert delay-lock loop test reset line.
//      * 5 Frequency check setting.
//      * 6 If 1 then the automatic frequency increase after a phase-lock loop reset will be disabled.
//      * 7 If 1 then the watchdog circuit is disabled.
//      * 8 Whether or not the Hamming error detection/correction on incoming data stream is enabled.
//      * 9 Whether the test input/outputs are enabled or disabled.
//      */
//     std::vector<u16> getConfigurationSettings() const;

    /**
     * Returns the status of of internal monitoring signals. The returned u16 can be masked
     * with the Fed9UTtcrxStatus enum defined in TypeDefs.hh to check for set flags.
     *
     * The flags that can be set are as follows :-
     * Auto Reset Flag - a 1 indicates an automatic reset has occurred due to a
     *                   timeout condition in the watchdog circuit.
     * Frame Synch     - a 1 indicates that channel B is synchronized to the
     *                   data stream.
     * DLL Ready       - a 1 indicates the High-Resolution phase shiftes are
     *                   working properly.
     * PLL ready       - a 1 indicates that the clock and data recovery circuit
     *                   is locked on the incoming data stream.
     * Elements 1 to 3 should read a 1 under normal operating conditions.
     */
    u16 getStatusRegister() const;

    /**
     * Performs the writes to the registers on the TTCrx chip.
     */
    Fed9UTtcrxDevice& setTtcrxControls(const u32 regAddr, const u32 timingData);

    /**
     * Performs the reads to the registers on the TTCrx chip.
     */
    u32 getTtcrxControls(const u32 regAddr) const;

    /**
     * Returns a copy of the TTCrx description.
     */
    Fed9UTtcrxDescription getTtcrxDescription() { return theTtcrxDescription; }

    /**
     * Some private data members.
     */
    private:

    Fed9UVmeBase& theFed;                             // Base class for configuring FED settings.
    Fed9UTtcrxDescription& theTtcrxDescription;       // Holds the current TTCrx object.

  }; // End of Fed9UTtcrxDevice class.
}    // End of Fed9U namespace.

#endif
