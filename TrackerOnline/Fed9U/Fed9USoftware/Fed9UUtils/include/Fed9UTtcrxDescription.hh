#ifndef H_Fed9UTtcrxDescription
#define H_Fed9UTtcrxDescription

#include "TypeDefs.hh"

#include <vector>

namespace Fed9U {

  /**
   * This structure holds a bool value for each type of reset that can be performed on the TTCrx.
   * Setting that value true will cause that reset to be performed, false and it will not.
   *
   * The errorCounters, bunchCount, eventCount and status variables will reset the error counter,
   * bunch counter, event counter and status registers respectivily.
   */
  struct Fed9UTtcrxCounterReset {
    Fed9UTtcrxCounterReset(bool errorCounters = false, bool bunchCount = false, bool eventCount = false, bool status = false) :
      _errorCounters(errorCounters), _bunchCount(bunchCount), _eventCount(eventCount), _status(status) {}
    bool _errorCounters;
    bool _bunchCount;
    bool _eventCount;
    bool _status;
  };// End of Fed9UTtcrxReset.

  /**
   * \brief  A class used to provide a software representation of the TTCrx chip.
   * \author Gareth Rogers, RAL.
   * \date   2004/03.
   *
   * The class provides a representation of the all the read and writable registers
   * on the TTCrx chip. More information about the TTCrx chip can be found in the
   * TTCrx Reference Manual: A Timing, Trigger and Control Receiver ASIC for LHC Detectors. 
   */
  class Fed9UTtcrxDescription {

    /**
     * The Fed9UTtcrxDescription public interface.
     */
  public:

    /**
     * \biref This constructor will create a Fed9UTtcrxDescription either with User defined values
     *        or the default values.
     *
     * \param counterReset Sets which counters are to be reset when the object is loaded into the FED TTCrx chip registers.
     * \param l1AcceptOneCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param brcstStrTwoCoarseDelay Sets a delay to be set to the level 1 accept signal, in steps of 25ns. Range:0-15, default 0.
     * \param clockDesOneFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param clockDesTwoFineDelay Sets a delay to the deskewed clock 1 line, in steps of 104ps. Range:0-239, default 0.
     * \param dllPumpCurrent Sets the value of the pump current to be used in the delay lock loop chips. Range:1-7, default 2.
     * \param pllPumpCurrent Sets the value of the pupm current to be used in the delay lock loop chips. Range:1-7, default 3.
     * \param iacId Sets the individually addressed command ID. Range:0-16383, default 0.
     * \param i2cId Sets the I2C ID. Range:0-63, default 0.
     * \param counterOperation Set which counters are enabled. Default FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
     * \param disableHammingChecking Used to disable the hamming error checking. Default false.
     * \param selectClockTwo Used to select the second clock. Default false.
     * \param disableClockTwo Used to disable the second clock. Default true.
     * \param disableClockL1Accept Used to disable the level 1 accept signal. Default false.
     * \param disableParrallelBus Used to disable the parrallel bus. Default true.
     * \param disableSerialB Used to disable the serial B line. Default true.
     * \param disableNonDeskewedClock Disables the none deskewed clock. Default false.
     *
     * All the defaults are set to their hardware defaults, except the fine delay settings.
     */
    Fed9UTtcrxDescription(Fed9UTtcrxCounterReset counterReset = Fed9UTtcrxCounterReset(false,false,false,false),
			  u16 l1AcceptOneCoarseDelay = 0,
			  u16 brcstStrTwoCoarseDelay = 0,
			  u16 clockDesOneFineDelay = 0,
			  u16 clockDesTwoFineDelay = 0,
			  u16 dllPumpCurrent = 2,
			  u16 pllPumpCurrent = 3,
			  u16 iacId = 0,
			  u16 i2cId = 0,
			  u16 counterOperation = FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
			  bool disableHammingChecking  = false,
			  bool selectClockTwo  = false,
			  bool disableClockTwo = true,
			  bool disableClockL1Accept    = false,
			  bool disableParrallelBus     = true,
			  bool disableSerialB  = true,
			  bool disableNonDeskewedClock = false);

    /**
     * \brief  This is used to hold the information about which reset is required on the TTCrx.
     * \param  counterReset A Fed9UTtcrxCounterReset struct that determines which counters will be reset.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setCounterReset(const Fed9UTtcrxCounterReset& counterReset);

    /**
     * \brief  Returns a TTCrx reset structure which contains the settings for which resets
     *         are require on the TTCrx.
     * \return Fed9UTtcrxCounterReset. 
     */
    Fed9UTtcrxCounterReset getCounterReset() const;

    /**
     * \brief  This set the total amount of delay skew for the L1Accept, broadcast strobe 1
     *         signal, the bunch and event counter reset signals and the system broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * It sets the fine delay for either the 40MHz deskewed clock one or two depending on which the signals are
     * synchronised to. This is chosen by the either selecting or deselecting the deskew clock two.
     */
    Fed9UTtcrxDescription& setL1AcceptDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * \brief  This will set the total amount of delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * It sets the fine delay for either the 40MHz deskewed clock one or two depending on which
     * the signals are synchronised to, which can be selected using setDeskewedClockTwoSelected.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay);

    /**
     * \brief  This will set the coarse delay skew for the L1Accept, broadcast strobe 1 signal,
     *         bunch and event counter reset signals and the system broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setL1AcceptCoarseDelay(u16 coarseDelay);

    /**
     * \brief This will return the coarse delay skew for the deskewed 40MHz clock one.
     *        This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     *        event counter reset signals and the system broadcast command.
     * \return unsigned short.
     */
    u16 getL1AcceptCoarseDelay() const;

    /**
     * \brief  This will set the coarse delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \param  coarseDelay Delay to be applied in 25ns steps. Range:0-15 (400ns).
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setBrcstStrTwoCoarseDelay(u16 coarseDelay);

    /**
     * \brief  This will set the coarse delay skew for the broadcast strobe 2 signal
     *         and the user broadcast command.
     * \return unsigned short.
     */
    u16 getBrcstStrTwoCoarseDelay() const;

    /**
     * \brief This will set the fine delay skew for the deskewed 40MHz clock one.
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if the second clock is deselected.
     */
    Fed9UTtcrxDescription& setClockDesOneFineDelay(u16 fineDelay);

   /**
     * \brief  This will return the fine delay skew for the deskewed 40MHz clock one.
     * \return unsigned short.
     *
     * This is the skew for the L1Accept, broadcast strobe 1 signal, bunch and
     * event counter reset signals and the system broadcast command. It will
     * also effect the broadcast strobe two and the user broadcast command
     * if the second deskewed clock is deselected.
     */
    u16 getClockDesOneFineDelay() const;

    /**
     * \brief  This will set the fine delay skew for the deskewed 40MHz clock two.
     * \param  fineDelay Delay to be applied in 104ps steps. Range:0-239 (24.856ns).
     * \return Self reference.
     *
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled by passing a true to the
     * setDeskewedClock2Selected method.
     */
    Fed9UTtcrxDescription& setClockDesTwoFineDelay(u16 fineDelay);

    /**
     * \brief  This will return the fine delay skew for the deskewed 40MHz clock two.
     * \return unsigned short.
     *
     * This will affect the broadcast strobe 2 signal and the user broadcast
     * command only if it is enabled. This can be checked using the method
     * getDeskewedClock2Selected. If a true is returned it is enabled.
     */
    u16 getClockDesTwoFineDelay() const;

    /**
     * \brief  Sets the charge pump current used in the delay-lock loops of the high
     *         resolution phase shifters.
     * \param  pumpCurrent Current to be set. Range:1-7.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setDllPumpCurrent(u16 pumpCurrent);

    /**
     * \brief  Returns the charge pump current used in the delay-lock loops of the high
     *         resolution phase shifters.
     * \return unsigned short.
     */
    u16 getDllPumpCurrent() const;

    /**
     * \brief  Sets the charge pump current used in the phase-lock loops which provide
     *         the clock recovery.
     * \param  pumpCurrent Pump current to use. Range:0-7.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setPllPumpCurrent(u16 pumpCurrent);

    /**
     * \brief  Returns the charge pump current used in the phase-lock loops which provide
     *         the clock recovery.
     * \return unsigned short.
     */
    u16 getPllPumpCurrent() const;

    /**
     * \brief  This sets the 14 bit individually addressed command (IAC) ID.
     * \param  id ID number the IAC is to be set to. Range:0-16383.
     * \return Self reference.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link.
     */
    Fed9UTtcrxDescription& setIacId(u16 id);

    /**
     * \brief  This returns the 14 bit individually addressed command (IAC) ID.
     * \return unsigned short.
     *
     * It is used to identifying the individually addressed commands (IAC) that are sent
     * over the optical link.
     */
    u16 getIacId() const;

    /**
     * \brief  This sets the 6 bit I2C ID.
     * \param  id I2C ID to be set. Range:0-63.
     * \return Self reference.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     */
    Fed9UTtcrxDescription& setI2cId(u16 id);

    /**
     * \brief  This returns the 6 bit I2C ID.
     * \return unsigned short.
     *
     * It is used as the base address for accessing the TTCrx using the I2C interface.
     */
    u16 getI2cId() const;

    /**
     * \brief  This is used to control enable or disable the bunch and event counter operations.
     * \param  counterOpeation Counters that are to be enabled or disabled.
     * \return Self reference.
     *
     * The individual settings can be enabled or disabled using the enum
     * Fed9UTtcrxCtrOperation found in TypeDefs.hh. Both flags can be set using a bitwise OR
     * and any flag not set will be disabled. The bunch and event counter settings are used in
     * conjuction to specify the mode of the level 1 trigger sequence. For more information see
     * the TTCrx Reference Manual chapter 6.
     */
    Fed9UTtcrxDescription& setCounterOperation(u16 counterOperation);

    /**
     * \brief  Returns the enable or disable flags for the bunch and event counter operations.
     * \return unsigned short.
     *
     * The individual settings can be checked by using the masks provided in Fed9UTtcrxCtrOperation.
     * The bunch and event counter settings are used in conjuction to specify the mode
     * of the level 1 trigger sequency. For more information see the TTCrx Reference
     * Manual chapter 6. Disabling these can be used to save power.
     */
    u16 getCounterOperation() const;

    /**
     * \brief  This can be used to enable or disable the hamming error checking.
     * \param  disableHammingChecking True disables the checking, false will enable it.
     * \return Self reference.
     */
    Fed9UTtcrxDescription& setHammingCheckingDisable(bool disableHammingChecking);

    /**
     * \brief  Returns the enable state of the hamming error checking.
     * \return Boolean.
     */
    bool getHammingCheckingDisable() const;

    /**
     * \brief  This selects the 40MHz deskewed clock two. If selected it is used to sychronise
     *         the broadcast strobe 2 signal and the User broadcast command.
     * \param  selectClockTwo True selects the clock, false deselects it.
     * \return Self reference.
     *
     * This relates to which signals will have a fine skew applied to them, depending on whether they are
     * synchronised with clock two.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Selected(bool selectClockTwo);

    /**
     * \brief  Returns the selected state of the deskewed clock two.
     * \return boolean.
     *
     * If a true is returned then the broadcast strobe 2 signal and the user broadcast
     * command are synchronised to the 40MHz deskewed clock two.
     */
    bool getDeskewedClock2Selected() const;

    /**
     * \brief  Used to disable the second deskewed clock.
     * \param disableClockTwo If a true is passed it will be disabled, false enabled.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setDeskewedClock2Disable(bool disableClockTwo);

    /**
     * \brief  Returns the disabled state of the deskewed clock two.
     * \return Boolean.
     */
    bool getDeskewedClock2Disable() const;

    /**
     * \brief  Disables the L1Accept clock output.
     * \param  l1AcceptDisable True will disable the output, false enable it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setClockL1AcceptDisable(bool l1AcceptDisable);

    /**
     * \brief  Returns the setting on the L1Accept clock disable flag.
     * \return Self reference.
     */
    bool getClockL1AcceptDisable() const;

    /**
     * \brief  Used to disable the non-deskewed 40MHz clock output.
     * \param  disable True will disable the clock, false enable it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setNonDeskewedClockDisable(bool disable);

    /**
     * \brief  Returns the disable state of the non-deskewed 40MHz clock.
     * \return Boolean.
     */
    bool getNonDeskewedClockDisable() const;

    /**
     * \brief  Used to disable the parrallel bus output.
     * \param  disable True disables the output, false enables it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setParrallelOutputDisable(bool disable);

    /**
     * \brief  Returns the disable state of the parrallel bus output.
     * \return Boolean.
     */
    bool getParrallelOutputDisable() const;

    /**
     * \brief  Sets the disable flag for the serial B output.
     * \param  disable True disables the output, false enables it.
     * \return Self reference.
     *
     * This can be disabled to save power.
     */
    Fed9UTtcrxDescription& setSerialBDisable(bool disable);

    /**
     * \brief  Returns the disable status of the serial B output.
     * \return Boolean.
     */
    bool getSerialBDisable() const;

    /**
     * Some private data members to define the state of the TTCrx.
     */
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    static const unsigned short CONTROL_REGISTER_SIZE = 8;
    // </NAC>
  private:
    Fed9UTtcrxCounterReset _counterReset;
    u16 _l1AcceptCoarseDelay, _brcstStrTwoCoarseDelay, _clockDesOneFineDelay, _clockDesTwoFineDelay;
    u16 _dllPumpCurrent, _pllPumpCurrent, _iacId, _i2cId;
    u16 _counterOperation;
    bool _disableHammingChecking, _selectClockTwo, _disableClockTwo, _disableClockL1Accept;
    bool _disableParrallelBus, _disableSerialB, _disableNonDeskewedClock;
    // <NAC date="16/11/2007"> changed vector to array to make class serializable
    //std::vector<bool> _controlRegister;
    bool _controlRegister[CONTROL_REGISTER_SIZE];
    // </NAC>
    
    
    // <NAC date="24/04/2007">
    friend  bool operator == (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r);
    // </NAC>

  }; //End of class Fed9UTtcrxDescription declaration.

  /**
   * \brief  This overload of operator<< will allow the Fed9UTtcrxDescription to be written to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UTtcrxDescription class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UTtcrxDescription& data);

  /**
   * \brief  This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UTtcrxDescription class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescription& data);
  
  // <NAC date="24/04/2007"> operator to compare Ttcrx descriptions
  /**
   * \brief  == operator to allow Fed9UTtcrxDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r);
  /**
   * \brief  != operator to allow Fed9UTtcrxDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTtcrxDescription& l, const Fed9UTtcrxDescription& r) { return !(l==r); }
    /**
   * \brief  == operator to allow Fed9UTtcrxCounterResets to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UTtcrxCounterReset& l, const Fed9UTtcrxCounterReset& r);
  /**
   * \brief  != operator to allow Fed9UTtcrxCounterResets to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTtcrxCounterReset& l, const Fed9UTtcrxCounterReset& r) { return !(l==r); }
  // </NAC>

}      //End of Fed9U namespace.
#endif //H_Fed9UTtcrxDescription
