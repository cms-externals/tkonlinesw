#ifndef H_Fed9UVoltageMonitor
#define H_Fed9UVoltageMonitor

//Author: Gareth Rogers.
//Date  : April 2004.

/**
 * This class provides the ability to configure the ADM1025 voltage monitor.
 *
 * The ADM1025 can monitor a 2.5V, 3.3V, 12V, the processor core voltage and its supply
 * voltage as well as it own and an external temperature.  All voltages are accurate to
 * 3 decimal places. Each voltage and temperature measurement are compared to a lower and
 * upper limit. If the measurements are outside this limit then it is flagged in the status register.
 */

#include "Fed9UVoltageControl.hh"
#include "TypeDefs.hh"

#include <vector>

namespace Fed9U {

  //using std::vector;
  class Fed9UVmeBase;

  class Fed9UVoltageMonitor {

    /**
     * The constructor and destructor member functions for creating and destroying a Fed9UVoltageMonitor object.
     */
  public:

    /**
     * The Fed9UVoltageMonitor constructor.
     *
     * The Fed9UVoltageMonitor requires a Fed9UVmeBase to use to read and write to the voltage
     * monitor device on the FED. It also requires a Fed9UVoltageControl object to retrieve the
     * current settings on the voltage monitor. This creates a local copy of the Fed9UVoltageControl.
     */
    Fed9UVoltageMonitor(Fed9UVmeBase& fed, const Fed9UVoltageControl& voltageController);

    /**
     * The Fed9UVoltageMonitor destructor.
     */
    ~Fed9UVoltageMonitor();

    /**
     * These constructors are not implemented and cannot be used with the voltage monitor class.
     */
  private:

    /**
     * The copy constructor and assignment operator have been declared private for this class.
     * This has been done because of the use of references to the Fed9UVmeBase class and
     * Fed9UDescription class. Copying or assignment would cause multiple references to refer
     * to the same object. Therefore the compiler generated methods would be of no use.
     */
    Fed9UVoltageMonitor(const Fed9UVoltageMonitor&);
    Fed9UVoltageMonitor& operator=(const Fed9UVoltageMonitor&);

    /**
     * Public methods used to read and write to the voltage monitor.
     */
  public:

    /**
     * This can be used to place the voltage controller in a stand by mode which does not perform
     * any measurements or conversion and can be used to save power when the voltage controller is
     * not needed.
     *
     * A true will shut the controller down and a false enable it. This will not reset the chip.
     * When the controller enters stand by mode the temperature offset will be reset to the
     * external temperature measure. It will keep its offset value and all other registers will
     * remain unaffected.
     */
    Fed9UVoltageMonitor& setStandBy(bool standBy);

    /**
     * This returns the running state of the voltage controller.
     *
     * If a true is returned then the voltage controller is in a stand by state and no measurements
     * will be made and the limits will not be checked. The register are not reset when the voltage
     * controller leaves this state and so all previous settings are still valid.
     */
    bool getStandBy() const;

    /**
     * This provides the ability to reset the status registers to their hardware defaults.
     */
    Fed9UVoltageMonitor& resetStatusRegister();

    /**
     * This sets the upper limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set2Point5VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    float get2Point5VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set2Point5VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 2.5V reading.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    float get2Point5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set3Point3VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float get3Point3VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set3Point3VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 3.3V reading.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 4.383V and a minimum of 0.
     */
    float get3Point3VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 5V reading.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set5VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 5V reading.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    float get5VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 5V reading.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 6.640V and a minimum of 0.
     */
    Fed9UVoltageMonitor& set5VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 5V reading.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
    * It has a maximum value of 6.640V and a minimum of 0.
    * It has a minimum value of 0V.
     */
    float get5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 12V reading.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    Fed9UVoltageMonitor& set12VoltMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the 12V reading.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    float get12VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 12V reading.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    Fed9UVoltageMonitor& set12VoltMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the 12V reading.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 15.938V and a minimum value of 0V.
     */
    float get12VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    Fed9UVoltageMonitor& setCoreVoltageMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    float getCoreVoltageMax() const;

    /**
     * This sets the lower limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    Fed9UVoltageMonitor& setCoreVoltageMin(float voltage);

    /**
     * This returns the upper limit warning voltage for the core voltage reading.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     * It has a maximum value of 2.988V and a minimum value of 0V.
     */
    float getCoreVoltageMin() const;

    /**
     * This sets the upper limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageMonitor& setSupplyVoltageMax(float voltage);

    /**
     * This returns the upper limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    float getSupplyVoltageMax() const;

    /**
     * This sets the lower limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageMonitor& setSupplyVoltageMin(float voltage);

    /**
     * This returns the lower limit warning voltage for the supply voltage reading.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    float getSupplyVoltageMin() const;

    /**
     * This sets the upper limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageMonitor& setExternalTempMax(i16 temperature);

    /**
     * This returns the upper limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMax() const;

    /**
     * This sets the lower limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageMonitor& setExternalTempMin(i16 temperature);

    /**
     * This returns the lower limit warning temperature for the external temperature reading.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMin() const;

    /**
     * This sets the upper limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageMonitor& setInternalTempMax(i16 temperature);

    /**
     * This returns the upper limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getInternalTempMax() const;

    /**
     * This sets the lower limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    Fed9UVoltageMonitor& setInternalTempMin(i16 temperature);

    /**
     * This returns the lower limit warning temperature for the internal temperature reading.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getInternalTempMin() const;

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to set
     * that offset and to chose which measurement you wish to offset.
     *
     * The offset must be in the range -128oC to 127oC. Passing a boolean true will enable the offset
     * for the internal temperature reading and a false for the external temperature offset.
     * This will be reset to the external temperature measurement if place in or out of stand by mode.
     */
    Fed9UVoltageMonitor& setTempOffset(i16 offset, bool internal);

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to read
     * that offset.
     *
     * The offset is in the range -128oC to 127oC. The offset is given as the return arguement and
     * which reading is offset can be found using the getOffsetTempSelect() member function.
     * This will be reset to the external temperature measurement if place in or out of stand by mode.
     */
    i16 getTempOffset() const;

    /**
     * The temperature measurement can have an offset added to the measured value. The offset can be
     * added to either the external or the internal offset, but not to both. This allows you to find
     * out which measurement is offset.
     *
     * A boolean true will enable the offset for the internal temperature reading and a false for
     * the external temperature offset. A return value of true signifies the internal temperature
     * measurement will be offset and a return value of false the external temperature will be offset.
     * The temperature offset can be found using the getTempOffset member function.
     */
    bool getOffsetTempSelect() const;

    /**
     * This returns the actual voltage on the 2.5V.
     */
    float getActual2Point5Volt() const;

    /**
     * This returns the actual voltage on the 3.3V.
     */
    float getActual3Point3Volt() const;

    /**
     * This returns the actual voltage on the 5V.
     */
    float getActual5Volt() const;

    /**
     * This returns the actual voltage on the 12V.
     */
    float getActual12Volt() const;

    /**
     * This returns the actual voltage on the processor voltage.
     */
    float getActualCoreVoltage() const;

    /**
     * This returns the actual voltage on the 3.3V supply voltage.
     */
    float getActualSupplyVoltage() const;

    /**
     * This returns the current external temperature.
     */
    i16 getActualExternalTemp() const;

    /**
     * This returns the actual internal temperature.
     */
    i16 getActualInternalTemp() const;

    /**
     * This returns the manufactures ID.
     */
    u32 getManufacturesId() const;

    /**
     * This returns the stepping ID.
     */
    u32 getSteppingId() const;

    /**
     * The returned u16 can be masked with the Fed9UVoltageStatus enum, defined in the TypeDefs
     * header, to check which of the voltage limits have been exceeded.
     *
     * Below is a list of the errors that can be checked.
     * 2.5V error. A true signfies that the 2.5V has exceeded its limits.
     * Processor core voltage error. A true signfies that the processor core voltage
     * has exceeded its limits.
     * 3.3V error. A true signfies that the 3.3V has exceeded its limits.
     * 5V error. A true signfies that the 5V has exceeded its limits.
     * Internal temperature error. A true signfies that the internal temperature
     * has exceeded its limits.
     * External temperautre error. A true signfies that the external temperature
     * has exceeded its limits.
     * 12V error. A true signfies that the 12V has exceeded its limits.
     * Supply voltage error. A true signfies that the supply voltage has exceeded its limits.
     * Remote diode fault. A true indicates that the remote device, whos temperature is
     * being measured on the external temperature register is no longer connected.
     */
    u16 getStatusRegister() const;

    /**
     * This returns a copy of the local Fed9UVoltageControl description that is currently in use.
     */
    Fed9UVoltageControl getVoltageControl() {
      return theVoltageController;
    }

    /**
     * Some private member functions that are used to read and write to the voltage monitor.
     */
  private:

    /**
     * This is used to write to the voltage monitor.
     *
     * It takes a two element vector. The first element contains the data that is to be written
     * and the second element contains the register address that it is to be written to.
     */
    Fed9UVoltageMonitor& writeVoltageMonitor(std::vector<u32> dataAndRegAddr);

    /**
     * This is used to read from any register on the voltage monitor.
     *
     * It takes a single arguement that is the address of the register that you wish to write
     * to and it returns the current settings at that address.
     */
    u32 readVoltageMonitor(u32 regAddress) const;

    /**
     * Some private data members used to hold configuration data for the voltage monitor.
     */
  private:

    Fed9UVmeBase& theFed;                             // Base class for configuring FED settings.
    Fed9UVoltageControl& theVoltageController;        // Holds the current voltage controller object.

  };// End of Fed9UVoltageMonitor class.
}// End of Fed9U namespace.

#endif// H_Fed9UVoltageMonitor.
