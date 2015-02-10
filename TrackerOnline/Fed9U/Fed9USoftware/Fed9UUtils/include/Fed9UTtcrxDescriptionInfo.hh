#ifndef H_Fed9UTtcrxDescriptionInfo
#define H_Fed9UTtcrxDescriptionInfo

#include "Fed9UTtcrxDescription.hh"
#include "TypeDefs.hh"

#include <vector>

namespace Fed9U {

/**
 * \brief  A class used to provide a software representation of the TTCrx chip.
 * \author Gareth Rogers.
 * \date   2004/03
 *
 * This is a derived class from the Fed9UTtcrxDescription. It contains in addition
 * to the read and writable settings the read only settings. The purpose of this
 * class is to read back settings from the TTCrx. It can be used to configure the
 * TTCrx as it is a derived class of Fed9UTtcrxDescription, however the methods
 * provided in this class can only hold read only values and any information set by
 * the user will not be written to the TTCrx.
 */

  class Fed9UTtcrxDescriptionInfo : public Fed9UTtcrxDescription {

  public:

    /**
     * \brief Default constructor.
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
     * \param bunchCount Number of bunch that have occured since the last reset. Range:0-4095, default 0.
     * \param eventCount Number of event that have been triggered since the last reset. Range:0-16777215, default 0.
     * \param singleBitErrorCount Number of errors that have occurred to single bits in the transmission since the last reset.
     *        Range:0-65535, default 0.
     * \param doubleBitErrorCount Number of errors that have occurred to two consequtive bits in the transmission since the last reset.
     *        Range:0-255, default 0.
     * \param seuErrorCount Number of single event upsets that the TTCrx corrected for since the last reset. Range:0-255, default 0.
     * \param masterModeA Determines the mode of running of channel A. Range:0-2, default 0.
     * \param masterModeB Determines the mode of running of channel B. Range:0-2, default 0.
     * \param testOutputs Value of the TTCrx test register used for debugging the chip. Range:0-7, default 5.
     * \param numbClockCycles Determines the number of clock cycles after frequency phase detection will stop. Range:0-7, default 7.
     * \param configSettings Configuration register that contains some of the debug settings for the TTCrx. Set using Fed9UTtcrxConfig enum.
     *        Range:0-255, default 66.
     * \param statusRegister Status register that contains the settings on the TTCrx. Set using Fed9UTtcrxStatus enum. Range:0-15, default 0.
     *
     * Allows a either completely initialised object to be created. Alternatively a the default values can be used
     * and a blank object created and used.
     */
    Fed9UTtcrxDescriptionInfo(Fed9UTtcrxCounterReset counterReset = Fed9UTtcrxCounterReset(false, false, false, false),
			      u16 clockDesOneCoarseDelay = 0,
			      u16 clockDesOneFineDelay   = 0,
			      u16 clockDesTwoCoarseDelay = 0,
			      u16 clockDesTwoFineDelay   = 0,
			      u16 dllPumpCurrent = 2,
			      u16 pllPumpCurrent = 3,
			      u16 iacId = 0,
			      u16 i2cId = 0,
			      u16 counterOperation = FED9U_TTCRX_ENABLE_BUNCH_CTR | FED9U_TTCRX_ENABLE_EVENT_CTR,
			      bool disableHammingChecking  = false,
			      bool selectClockTwo  = false,
			      bool disableClockTwo = true,
			      bool disbaleClockL1Accept    = false,
			      bool disableParrallelBus = true,
			      bool disableSerialB      = true,
			      bool disableNonDeskewedClock = false,
			      u16 bunchCount = 0,
			      u32 eventCount = 0,
			      u16 singleBitErrorCount = 0,
			      u16 doubleBitErrorCount = 0,
			      u16 seuErrorCount = 0,
			      u16 masterModeA = 0,
			      u16 masterModeB = 0,
			      u16 testOutputs = 5,
			      u16 numbClockCycles = 7,
			      u16 configSettings  = 66,
			      u16 statusRegister  = 0);

    /**
     * \brief  This sets the number of bunch crossing that have occured since the last reset.
     * \param  bunchCount Number of bunch crossing to be set. Range:0-4095.
     * \return Self reference.
     *
     * It counts the number of clock signals that are received by the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setBunchCounter(u16 bunchCount);

    /**
     * \brief  Returns the number of bunch crossing that have occured since the last reset.
     * \return unsigned short.
     *
     * It counts the number of clock signals that are received by the TTCrx.
     */
    u16 getBunchCounter() const;

    /**
     * \brief  This sets the number of events that have been received since the last reset.
     * \param  eventCount Number of events to be set.
     * \return Self reference.
     *
     * It counts the number of trigger signals that are received by the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setEventCounter(u32 eventCount);

    /**
     * \brief  Returns the number of events that have been received since the last reset.
     * \return unsigned long.
     *
     * It counts the number of trigger signals that are received by the TTCrx.
     */
    u32 getEventCounter() const;

    /**
     * \brief  This stores the number of single bit errors that are recignised and corrected by the
     *         receiver's Hamming decoder.
     * \param  singleBitErrorCount Error count to be set. Range:0-65355.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setSingleBitErrorCount(u16 singleBitErrorCount);

    /**
     * \brief  Returns the number of single bit errors that are recignised and corrected by the
     *         receiver's Hamming decoder.
     * \return unsigned short.
     */
    u16 getSingleBitErrorCount() const;

    /**
     * \brief  Stores the number of double bit Hamming and frame errors that the TTCrx recognises.
     * \param  doubleBitErrorCount Number of double bit errors to be set. Range:0-255.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setDoubleBitErrorCount(u16 doubleBitErrorCount);

    /**
     * \brief  Returns the number of double bit Hamming and frame errors.
     * \return unsigned short.
     */
    u16 getDoubleBitErrorCount() const;

    /**
     * \brief  Stores the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * \param  seuErrorCount Number of SEU to be set. Range:0-255.
     * \return Self reference.
     */
    Fed9UTtcrxDescriptionInfo& setSeuErrorCount(u16 seuErrorCount);

    /**
     * \brief  Returns the number of Single Event Upsets (SEU) errors that occur and the TTCrx corrects for.
     * \return unsigned short.
     */
    u16 getSeuErrorCount() const;

    /**
     * \brief  This determines the overall operating mode of the TTCrx.
     * \param  masterMode Master mode of channel A to be set. Range:0-2.
     * \return Self reference.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * A. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeA(u16 masterMode);

    /**
     * \brief  Returns the overall operating mode of the TTCrx.
     * \return unsigned short.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * A. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeA() const;

    /**
     * \brief  This determines the overall operating mode of the TTCrx.
     * \param  masterMode Master mode of channel B to be set. Range:0-2.
     * \return Self reference.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    Fed9UTtcrxDescriptionInfo& setMasterModeB(u16 masterMode);

    /**
     * \brief  Returns the overall operating mode of the TTCrx.
     * \return unsigned short.
     *
     * If the first bit returned is 1 then a serial/parrallel conversion will be performed on channel
     * B. If the second bit is 1 then the test/debugging mode is turned on. A zero should be returned
     * when running a standard setup.
     */
    u16 getMasterModeB() const;

    /**
     * \brief  This is a read only register, which selects the test outputs.
     * \param  testOutputs Value of the test outputs to set. Range:0-7.
     * \return Self reference.
     *
     * This register is used for testing and debugging the TTCrx.
     */
    Fed9UTtcrxDescriptionInfo& setTestOutputs(u16 testOutputs);

    /**
     * \brief  Returns the settings for the test outputs.
     * \return unsigned short.
     */
    u16 getTestOutputs() const;

    /**
     * \brief  This is a read only register, which selects the number of clock
     *         cycles after which the frequency phase detection will stop.
     * \param  numbClockCycles Sets the number of clock cycles to stop frequency phase detection.
     * \return Self reference.
     *
     * It is a testing and debugging register.
     */
    Fed9UTtcrxDescription& setFreqDetectCount(u16 numbClockCycles);

    /**
     * \brief  Returns the number of clock cycles after which the frequency phase detection will stop.
     * \return unsigned short.
     *
     * It is a testing and debugging register.
     */
    u16 getFreqDetectCount() const;

    /**
     * \brief  This is a read only configuration register. It contains some testing and debugging settings.
     * \param  configSettings Sets the configuration register settings.
     * \return Self reference.
     *
     * The enum Fed9UTtcrxConfig can be used to set these bits. As these are read only on the TTCrx,
     * what is set should correspond to what is on the FED and the defauls given in the enum documentation.
     * Any setting not set will be cleared.
     */
    Fed9UTtcrxDescription& setConfigSettings(u16 configSettings);

    /**
     * \brief  Returns the settings on the configuration register. They can be decoded using the Fed9UTtcrxConfig enum.
     * \return unsigned short.
     */
    u16 getConfigSettings() const;

    /**
     * \brief  The current status of the internal monitoring signals.
     * \param  statusRegister Value of the status register set using the Fed9UTtcrxStatus enum.
     * \return Self reference.
     *
     * The flags are set using the Fed9UTtcrxStatus enum. Multiple flags can be use by using a
     * a bitwise OR. Any flag not set will be cleared.
     */
    Fed9UTtcrxDescriptionInfo& setStatusRegister(u16 statusRegister);

    /**
     * \brief  Returns the status of of internal monitoring signals.
     * \return unsigned short.
     *
     * The returned unsigned short can be masked with the Fed9UTtcrxStatus enum to check for set flags
     * and the meaning of each flag is given with the Fed9UTtcrxStatus enum documentation.
     */
    u16 getStatusRegister() const;

    /**
     * Some private data members to define the state of the TTCrx.
     */
  private:
    u16 _bunchCount;
    u32 _eventCount;
    u16 _singleBitErrorCount, _doubleBitErrorCount, _seuErrorCount, _masterModeA, _masterModeB;
    u16 _testOutputs, _numbClockCycles, _configSettings, _statusRegister;
  };//End of class Fed9UTtcrxDescriptionInfo declaration.

  /**
   * \brief  This overload of operator<< will allow the Fed9UTtcrxDescriptionInfo to be written to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UTtcrxDescriptionInfo class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UTtcrxDescriptionInfo& data);

  /**
   * \brief  This overload of operator>> will allow the Fed9UTtcrxDescription to be created from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UTtcrxDescriptionInfo class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UTtcrxDescriptionInfo& data);

}      //End of Fed9U namespace
#endif //H_Fed9UTtcrxDescriptionInfo
