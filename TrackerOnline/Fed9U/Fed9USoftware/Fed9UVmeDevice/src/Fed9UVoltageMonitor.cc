#include <inttypes.h>
#include <stdint.h>
#include "Fed9UVoltageMonitor.hh"
#include "Fed9UVmeDeviceException.hh"
#include "Fed9UVmeBase.hh"
#include "Fed9UWait.hh"

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

namespace Fed9U {

  using namespace std;
  /**
   * The Fed9UVoltageMonitor constructor.
   *
   * The Fed9UVoltageMonitor requires a Fed9UVmeBase to use to read and write to the voltage
   * monitor device on the FED. It also requires a Fed9UVoltageControl object to retrieve the
   * current settings on the voltage monitor.
   */
  Fed9UVoltageMonitor::Fed9UVoltageMonitor(Fed9UVmeBase& fed, const Fed9UVoltageControl& voltageController) :
    theFed(fed), theVoltageController( *new Fed9UVoltageControl(voltageController) ) {
  }

  /**
   * The Fed9UVoltageMonitor destructor.
   */
  Fed9UVoltageMonitor::~Fed9UVoltageMonitor() {
    delete &theVoltageController;
  }

  /**
   * This can be used to place the voltage controller in a stand by mode which does not perform
   * any measurements or conversion and can be used to save power when the voltage controller is
   * not needed.
   *
   * A true will shut the controller down and a false enable it. This will not reset the chip.
   * All registers will remain unaffected.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setStandBy(bool standBy) {
    try {
      //This vector holds the data (in the first element) and the address of the register that will be written to (in the second element).
      vector<u32> dataAndRegAddr(2,0);
      //This will disable the internal ADC and prevent further measurements from being made. It is possible to further disable the voltage
      //monitor, however this requires settings the smae bits as for the temperature offset and it will cause great difficulty if either
      //are to be configured correctly.
      //Bit 0 in register 0x40 must be set low to disable the monitor and high to enable it.
      //First the current setting of that regsiter must be found, then the appropiate bit updated and then the value can be written back
      //to the voltage monitor.
      dataAndRegAddr[1] = 0x40;
      //Retrieve the existing settings.
      dataAndRegAddr[0] = readVoltageMonitor(dataAndRegAddr[1]);
      //Update bit 0. First a mask is created that will set bit 0 low and leave the other bits unchanged.
      u32 mask = ~0x01;
      dataAndRegAddr[0] &= mask;
      //Now bit 0 will take on the inverse value of stand by.
      dataAndRegAddr[0] |= static_cast<u32>(!standBy);
      //The new value is now written to the voltage monitor.
      writeVoltageMonitor(dataAndRegAddr);

      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setStandBy."));
    }
  }

  /**
   * This returns the running state of the voltage controller.
   *
   * If a true is returned then the voltage controller is in a stand by state and no measurements
   * will be made and the limits will not be checked. The register are not reset when the voltage
   * controller leaves this state and so all previous settings are still valid.
   */
  bool Fed9UVoltageMonitor::getStandBy() const {
    try {
      //The voltage monitor will be in stand by mode when bit 0 from register 0x40 is set high.
      //Otherwise the voltage monitor must be active.
      //Holds the address of the register that we wish to read.
      u32 regAddr = 0x40;
      //This holds the state of the first relavent bit. Bit 0 in register 0x40.
      //The return value is masked so that we only use the value of the relavent bit.
      u32 bitOne = readVoltageMonitor(regAddr) & 0x01;
      bool standBy;
      if (0x00 == bitOne) {
	standBy = true;
      } else {
	standBy = false;
      }
      return standBy;

     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getStandBy."));
    }
  }

  /**
   * This provides the ability to reset the status registers to their hardware defaults.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::resetStatusRegister() {
    try {
      //This will also clear the Configuration register to its power up default, but the user has no access to this.
      //It is this register that needs to be written to perform the reset. As it is reset there is no need to remember its current values.
      vector<u32> dataAndRegAddr(2,0);
      //For some reason if the values set to register 0x40 are just 0x80 then the voltage monitor enters an error state.
      //Therefore you have to write the previous settings with bit 7 set high to reset this register and the status registers.
      dataAndRegAddr[1] = 0x40;
      dataAndRegAddr[0] = readVoltageMonitor(dataAndRegAddr[0]);
      //This will set bit 7 to 1, leaving the other bits at their current values.
      dataAndRegAddr[0] |= 0x80;
      writeVoltageMonitor(dataAndRegAddr);
      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::resetStatusRegister."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the 2.5V reading.
   *
   * If the 2.5V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 3.320V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set2Point5VoltMax(float voltage) {
    try {
      float max = 3.320;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 2.5V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 2.5 + 0.5);
      dataAndRegAddr[1] = 0x2b;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set2Point5VoltMax(voltage);
      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set2Point5VoltMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the 2.5V reading.
   *
   * If the 2.5V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 3.320V and a minimum of 0.
   */
  float Fed9UVoltageMonitor::get2Point5VoltMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x2b;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 2.5 / 192.0;
      return voltageLimit;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get2Point5VoltMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the 2.5V reading.
   *
   * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 3.320V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set2Point5VoltMin(float voltage) {
    try {
      float max = 3.320;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 2.5V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 2.5 + 0.5);
      dataAndRegAddr[1] = 0x2c;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set2Point5VoltMin(voltage);
      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set2Point5VoltMin."));
    }
  }

  /**
   * This returns the lower limit warning voltage for the 2.5V reading.
   *
   * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 3.320V and a minimum of 0.
   */
  float Fed9UVoltageMonitor::get2Point5VoltMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x2c;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 2.5 / 192.0;
      return voltageLimit;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get2Point5VoltMin."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the 3.3V reading.
   *
   * If the 3.3V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set3Point3VoltMax(float voltage) {
    try {
      float max = 4.383;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 3.3V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 3.3);
      dataAndRegAddr[1] = 0x2f;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set3Point3VoltMax(voltage);
      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set3Point3VoltMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the 3.3V reading.
   *
   * If the 3.3V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum of 0.
   */
  float Fed9UVoltageMonitor::get3Point3VoltMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x2f;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 3.3 / 192.0;
      return voltageLimit;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get3Point3VoltMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the 3.3V reading.
   *
   * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set3Point3VoltMin(float voltage) {
    try {
      float max = 4.383;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 3.3V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 3.3 + 0.5);
      dataAndRegAddr[1] = 0x30;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set3Point3VoltMin(voltage);
      return *this;
     } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set3Point3VoltMin."));
    }
  }

  /**
   * This returns the lower limit warning voltage for the 3.3V reading.
   *
   * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum of 0.
   */
  float Fed9UVoltageMonitor::get3Point3VoltMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x30;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 3.3 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get3Point3VoltMin."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the 5V reading.
   *
   * If the 5V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 6.640V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set5VoltMax(float voltage) {
    try {
      float max = 6.640;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 5V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 5.0 + 0.5);
      dataAndRegAddr[1] = 0x31;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set5VoltMax(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set5VoltMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the 5V reading.
   *
   * If the 5V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 6.640V and a minimum of 0.
   */
  float Fed9UVoltageMonitor::get5VoltMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x31;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 5.0 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get5VoltMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the 5V reading.
   *
   * If the 5V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 6.640V and a minimum of 0.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set5VoltMin(float voltage) {
    try {
      float max = 6.640;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 5V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 5.0 + 0.5);
      dataAndRegAddr[1] = 0x32;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set5VoltMin(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set5VoltMin."));
    }
  }

  /**
   * This returns the lower limit warning voltage for the 5V reading.
   *
   * If the 5V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 6.640V and a minimum of 0.
   * It has a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::get5VoltMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x32;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 5.0 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get5VoltMin."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the 12V reading.
   *
   * If the 12V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 15.938V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set12VoltMax(float voltage) {
    try {
      float max = 15.938;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the 12V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 12.0 + 0.5);
      dataAndRegAddr[1] = 0x33;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set12VoltMax(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set12VoltMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the 12V reading.
   *
   * If the 12V level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 15.938V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::get12VoltMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x33;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 12.0 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get12VoltMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the 12V reading.
   *
   * If the 12V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 15.938V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::set12VoltMin(float voltage) {
    try {
      float max = 15.938;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the 12V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 12.0 + 0.5);
      dataAndRegAddr[1] = 0x34;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.set12VoltMin(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::set12VoltMin."));
    }
  }

  /**
   * This returns the lower limit warning voltage for the 12V reading.
   *
   * If the 12V level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 15.938V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::get12VoltMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x34;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 12.0 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::get12VoltMin."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the core voltage reading.
   *
   * If the core voltage level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 2.988V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setCoreVoltageMax(float voltage) {
    try {
      float max = 2.988;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the processor core voltage is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 2.249 + 0.5);
      dataAndRegAddr[1] = 0x2d;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setCoreVoltageMax(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setCoreVoltageMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the core voltage reading.
   *
   * If the core voltage level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 2.988V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::getCoreVoltageMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x2d;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 2.249 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getCoreVoltageMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the core voltage reading.
   *
   * If the core voltage level is than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 2.988V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setCoreVoltageMin(float voltage) {
    try {
      float max = 2.988;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the processor core voltage is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 2.249 + 0.5);
      dataAndRegAddr[1] = 0x2e;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setCoreVoltageMin(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setCoreVoltageMin."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the core voltage reading.
   *
   * If the core voltage level is than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 2.988V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::getCoreVoltageMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x2e;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 2.249 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getCoreVoltageMin."));
    }
  }

  /**
   * This sets the upper limit warning voltage for the supply voltage reading.
   *
   * If the supply voltage level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setSupplyVoltageMax(float voltage) {
    try {
      float max = 4.383;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The upper limit on the supply 3.3V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 3.3 + 0.5);
      dataAndRegAddr[1] = 0x35;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setSupplyVoltageMax(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setSupplyVoltageMax."));
    }
  }

  /**
   * This returns the upper limit warning voltage for the supply voltage reading.
   *
   * If the supply voltage level is greater than this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::getSupplyVoltageMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x35;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 3.3 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getSupplyVoltageMax."));
    }
  }

  /**
   * This sets the lower limit warning voltage for the supply voltage reading.
   *
   * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum value of 0V.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setSupplyVoltageMin(float voltage) {
    try {
      float max = 4.383;
      float min = 0;
      ICUTILS_VERIFY( (voltage >= min) && (voltage <= max) )(voltage)(max)(min).error().msg("The lower limit on the supply 3.3V is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The voltage is set as an 8 bit number so the float must be converted to a u32. Need to round up to set the correct value.
      dataAndRegAddr[0] = static_cast<u32>(voltage * 192.0 / 3.3 + 0.5);
      dataAndRegAddr[1] = 0x36;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setSupplyVoltageMin(voltage);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setSupplyVoltageMin."));
    }
  }

  /**
   * This returns the lower limit warning voltage for the supply voltage reading.
   *
   * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
   * It has a maximum value of 4.383V and a minimum value of 0V.
   */
  float Fed9UVoltageMonitor::getSupplyVoltageMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x36;
      //The read back data is a u32 with a maximum value of 255.
      u32 data = readVoltageMonitor(regAddr);
      //This corresponds to a voltage dependent on which range is being measured.
      float voltageLimit = data * 3.3 / 192.0;
      return voltageLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getSupplyVoltageMin."));
    }
  }

  /**
   * This sets the upper limit warning temperature for the external temperature reading.
   *
   * If the external temperature level is greater than this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setExternalTempMax(i16 temperature) {
    try {
      i16 max = 127;
      i16 min = -128;
      ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The upper limit on the external temperature is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The temperature is passed as a signed number, but must be converted to an unsigned number to be written as to HAL.
      //The voltage monitor will convert it back again.
      dataAndRegAddr[0] = static_cast<u32>(temperature);
      dataAndRegAddr[1] = 0x37;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setExternalTempMax(temperature);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setExternalTempMax."));
    }
  }

  /**
   * This returns the upper limit warning temperature for the external temperature reading.
   *
   * If the external temperature level is greater than this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  i16 Fed9UVoltageMonitor::getExternalTempMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x37;
      //The read back data is a 32 bit unsigned number.
      u32 data = readVoltageMonitor(regAddr);
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      i16 tempLimit = static_cast<i8>(data);
      return tempLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getExternalTempMax."));
    }
  }

  /**
   * This sets the lower limit warning temperature for the external temperature reading.
   *
   * If the external temperature level is less than or equal to this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setExternalTempMin(i16 temperature) {
    try {
      i16 max = 127;
      i16 min = -128;
      ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The lower limit on the external temperature is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The temperature is passed as a signed number, but must be converted to an unsigned number to be written as to HAL.
      //The voltage monitor will convert it back again.
      dataAndRegAddr[0] = static_cast<u32>(temperature);
      dataAndRegAddr[1] = 0x38;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setExternalTempMin(temperature);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setExternalTempMin."));
    }
  }

  /**
   * This returns the lower limit warning temperature for the external temperature reading.
   *
   * If the external temperature level is less than or equal to this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  i16 Fed9UVoltageMonitor::getExternalTempMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x38;
      //The read back data is a 32 bit unsigned number.
      u32 data = readVoltageMonitor(regAddr);
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      i16 tempLimit = static_cast<i8>(data);
      return tempLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getExternalTempMin."));
    }
  }

  /**
   * This sets the upper limit warning temperature for the internal temperature reading.
   *
   * If the internal temperature level is greater than this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setInternalTempMax(i16 temperature) {
    try {
      i16 max = 127;
      i16 min = -128;
      ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The upper limit on the internal temperature is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The temperature is passed as a signed number, but must be converted to an unsigned number to be written as to HAL.
      //The voltage monitor will convert it back again.
      dataAndRegAddr[0] = static_cast<u32>(temperature);
      dataAndRegAddr[1] = 0x39;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setInternalTempMax(temperature);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setInternalTempMax."));
    }
  }

  /**
   * This returns the upper limit warning temperature for the internal temperature reading.
   *
   * If the internal temperature level is greater than this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  i16 Fed9UVoltageMonitor::getInternalTempMax() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x39;
      //The read back data is a 32 bit unsigned number.
      u32 data = readVoltageMonitor(regAddr);
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      i16 tempLimit = static_cast<i8>(data);
      return tempLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getInternalTempMax."));
    }
  }

  /**
   * This sets the lower limit warning temperature for the internal temperature reading.
   *
   * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setInternalTempMin(i16 temperature) {
    try {
      i16 max = 127;
      i16 min = -128;
      ICUTILS_VERIFY( (temperature >= min) && (temperature <= max) )(temperature)(max)(min).error().msg("The lower limit on the internal temperature is out of range.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //The temperature is passed as a signed number, but must be converted to an unsigned number to be written as to HAL.
      //The voltage monitor will convert it back again.
      dataAndRegAddr[0] = static_cast<u32>(temperature);
      dataAndRegAddr[1] = 0x3a;
      writeVoltageMonitor(dataAndRegAddr);
      theVoltageController.setInternalTempMin(temperature);
      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setInternalTempMin."));
    }
  }

  /**
   * This returns the lower limit warning temperature for the internal temperature reading.
   *
   * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
   * This has a maximum value of 127oC and a minimum value of -128oC.
   */
  i16 Fed9UVoltageMonitor::getInternalTempMin() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x3a;
      //The read back data is a 32 bit unsigned number.
      u32 data = readVoltageMonitor(regAddr);
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      i16 tempLimit = static_cast<i8>(data);
      return tempLimit;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getInternalTempMin."));
    }
  }

  /**
   * The temperature measurement can have an offset added to the measured value. The offset can be
   * added to either the external or the internal offset, but not to both. This allows you to set
   * that offset and to chose which measurement you wish to offset.
   *
   * The offset must be in the range -128oC to 127oC. Passing a boolean true will enable the offset
   * for the internal temperature reading and a false for the external temperature offset.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::setTempOffset(i16 offset, bool internal) {
    try {
      i16 max = 127;
      i16 min = -128;
      ICUTILS_VERIFY( (offset >= min) && (offset <= max) )(offset)(min)(max).error().msg("The temperature offset is out of bounds.");
      //The first element of this vector holds the data that we wish to write and the second the register we wish to write to.
      vector<u32> dataAndRegAddr(2,0);
      //First select which temperature measurement the offset will apply to.
      //This is done by setting 3 bits in 2 registers.
      //First the current setting of that regsiter must be found, then the appropiate bit updated and then the value can be written back
      //to the voltage monitor. First, register 0x15, bit 0 is set.
      dataAndRegAddr[1] = 0x15;
      //Retrieve the existing settings.
      dataAndRegAddr[0] = readVoltageMonitor(dataAndRegAddr[1]);
      //Update bit 0. First a mask is created that will set bit 0 low and leave the other bits unchanged.
      u32 mask = ~0x01;
      dataAndRegAddr[0] &= mask;
      //Now bit 0 will take on the value of internal.
      dataAndRegAddr[0] |= internal;
      writeVoltageMonitor(dataAndRegAddr);

      //Next bit 6 in register 0x47 must be set to internal and bit 7 in that register set to !internal.
      dataAndRegAddr[1] = 0x47;
      //REad back the current settings.
      dataAndRegAddr[0] = readVoltageMonitor(dataAndRegAddr[1]);
      //Update bit 6 and 7. First a mask is created that will set bit 6 and 7 low, leaving the other bits unchanged.
      mask = ~0xc0;
      dataAndRegAddr[0] &= mask;
      //Now bit 6 will take on the value of internal.
      dataAndRegAddr[0] |= internal << 6;
      //Bit 7 !internal.
      dataAndRegAddr[0] |= !internal << 7;
      //Write back to register 0x47.
      writeVoltageMonitor(dataAndRegAddr);

      //Now write the offset register 0x1f.
      dataAndRegAddr[0] = offset;
      dataAndRegAddr[1] = 0x1f;
      writeVoltageMonitor(dataAndRegAddr);

      return *this;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::setTempOffset."));
    }
  }

  /**
   * The temperature measurement can have an offset added to the measured value. The offset can be
   * added to either the external or the internal offset, but not to both. This allows you to read
   * that offset.
   *
   * The offset is in the range -128oC to 127oC. The offset is given as the return arguement and
   * which reading is offset can be found using the getOffsetTempSelect() member function.
   */
  i16 Fed9UVoltageMonitor::getTempOffset() const {
    try {
      //This holds the register address we wish to write to.
      u32 regAddr = 0x1f;
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      return static_cast<i8>(readVoltageMonitor(regAddr));
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getTempOffset."));
    }
  }

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
  bool Fed9UVoltageMonitor::getOffsetTempSelect() const {
    try {
      //There are 3 bits in 2 registers that will determine which temperature measurement the offset will affect.
      //The first bit is bit 0 in register 0x15.
      u32 regAddr = 0x15;
      u32 bitOne  = readVoltageMonitor(regAddr) & 0x01;
      //The second and third bits are in register 0x47
      regAddr = 0x47;
      //The second bit is bit 6 of this register and the third is bit 7.
      u32 bitTwoAndThree = readVoltageMonitor(regAddr) & 0xc0;
      //If bit one is set high and both bit two and three are set low then the internal (true) offset is selected, otherwise it is the external (false).
      bool offsetSelection = false;
      if (0x01 == bitOne && 0x40 == bitTwoAndThree) {
	offsetSelection = true;
      }
      return offsetSelection;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getOffsetTempSelect."));
    }
  }

  /**
   * This returns the actual voltage on the 2.5V.
   */
  float Fed9UVoltageMonitor::getActual2Point5Volt() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x20;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 2.5 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActual2Point5Volt."));
    }
  }

  /**
   * This returns the actual voltage on the 3.3V.
   */
  float Fed9UVoltageMonitor::getActual3Point3Volt() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x22;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 3.3 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActual3Point3Volt."));
    }
  }

  /**
   * This returns the actual voltage on the 5V.
   */
  float Fed9UVoltageMonitor::getActual5Volt() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x23;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 5.0 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActual5Volt."));
    }
  }

  /**
   * This returns the actual voltage on the 12V.
   */
  float Fed9UVoltageMonitor::getActual12Volt() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x24;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 12.0 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActual12Volt."));
    }
  }

  /**
   * This returns the actual voltage on the processor voltage.
   */
  float Fed9UVoltageMonitor::getActualCoreVoltage() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x21;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 2.249 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActualCoreVoltage."));
    }
  }

  /**
   * This returns the actual voltage on the 3.3V supply voltage.
   */
  float Fed9UVoltageMonitor::getActualSupplyVoltage() const {
    try {
      //If the supply voltage is change to 5V then the reading will need to be taken from the 5V register, 0x23.
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x25;
      //The data returned is an 8 bit unsigned integer and it needs to be converted to a float.
      u32 data = readVoltageMonitor(regAddr);
      return data * 3.3 / 192.0;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActualSupplyVoltage."));
    }
  }

  /**
   * This returns the current external temperature.
   */
  i16 Fed9UVoltageMonitor::getActualExternalTemp() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x26;
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      return static_cast<i8>( readVoltageMonitor(regAddr) );
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActualExternalTemp."));
    }
  }

  /**
   * This returns the actual internal temperature.
   */
  i16 Fed9UVoltageMonitor::getActualInternalTemp() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x27;
      //The returned value must first be cast to a i8 (signed char) as that is how it is stored on the FED.
      //Then to make the read out easier it is cast to a i16 so that it is read as a number.
      //However if it is not first converted from u32 to i8 and goes straight to an i16 then
      //the compilier will misinterpret the number. e.g. the FED returns the bit pattern 0x80 as u32.
      //This FED considers this to be -128. However a cast from u32 -> i16 will cause it to be interpreted
      //as 128 due to the extra zeros in bit 31:8. By first casting to an i8 and then to an i16
      //it will be correctly interpreted.
      return static_cast<i8>( readVoltageMonitor(regAddr) );
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getActualInternalTemp."));
    }
  }

  /**
   * This returns the manufactures ID.
   */
  u32 Fed9UVoltageMonitor::getManufacturesId() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x3e;
      return readVoltageMonitor(regAddr);
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getManufacturesId."));
    }
  }

  /**
   * This returns the stepping ID.
   */
  u32 Fed9UVoltageMonitor::getSteppingId() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x3f;
      return readVoltageMonitor(regAddr);
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getSteppingId."));
    }
  }

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
  u16 Fed9UVoltageMonitor::getStatusRegister() const {
    try {
      //This holds the address of the register we wish to read from.
      u32 regAddr = 0x41;
      //The first 6 status bits are contained in register 0x41.
      u16 data = readVoltageMonitor(regAddr);
      //They need to be reordered to match the Fed9UVoltageStatus enum masks.
      u16 statusRegister = 0;
      statusRegister |=  data       & FED9U_VOLT_STATUS_2POINT5;
      statusRegister |= (data >> 1) & FED9U_VOLT_STATUS_3POINT3;
      statusRegister |= (data >> 1) & FED9U_VOLT_STATUS_5;
      statusRegister |= (data << 3) & FED9U_VOLT_STATUS_CORE;
      statusRegister |= (data << 2) & FED9U_VOLT_STATUS_INTERNAL_TEMP;
      statusRegister |= (data << 2) & FED9U_VOLT_STATUS_EXTERNAL_TEMP;
      //The remaining 3 status bits are in a different register.
      //The out of limit flags are not grouped in the register and need to be masked and bit shift to
      //place them appropiately in the returned word.
      regAddr  = 0x42;
      data = readVoltageMonitor(regAddr);
      statusRegister  |= (data << 3) & FED9U_VOLT_STATUS_12;
      statusRegister  |= (data << 4) & FED9U_VOLT_STATUS_SUPPLY;
      statusRegister  |= (data << 2) & FED9U_VOLT_STATUS_NO_EXTERNAL_TEMP;

      return statusRegister;
    } catch (const ICUtils::ICException& e) {
      RETHROW(e, Fed9UVmeDeviceException(Fed9UVmeDeviceException::ERROR_FED9UVOLTAGEMONITOR, "error in Fed9UVoltageMonitor::getStatusRegister."));
    }
  }

  /**
   * This is used to write to the voltage monitor.
   *
   * It takes a two element vector. The first element contains the data that is to be written
   * and the second element contains the register address that it is to be written to.
   */
  Fed9UVoltageMonitor& Fed9UVoltageMonitor::writeVoltageMonitor(vector<u32> dataAndRegAddr) {
    ICUTILS_VERIFY(dataAndRegAddr.size() == 2)(dataAndRegAddr.size()).error().msg("The input argument vector is of an invalid size.");
    ICUTILS_VERIFY(dataAndRegAddr[1] < 0x74)(dataAndRegAddr[1]).error().msg("The register address is invalid.");
    //If the write fails then either a busy or an error signel can be returned by the voltage monitor.
    bool busy  = false;
    bool error = false;
    //Used to limit the number of times that it attempts to write if a busy or error signal is present.
    u32 loopCount      = 0;
    const u32 maxLoops = 100;
    //This tells theFed to write.
    bool write = false;
    //This is where theFed would return the read arguments and so it is not required when writing.
    u32 dummyWord = 0;
    do {
      //Write to the voltage monitor.
      theFed.vmeCommandVoltageMonitorControl(write, dataAndRegAddr, dummyWord, busy, error);
      //If the write still fails after a set number of attempts then
      //the software gives up and throws an error.
      ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then voltage monitor is busy. If error = 1 then the write failed");
      //If the write fails there is a pause before it tries again.
      if (busy || error) {
	fed9Uwait(100000);
      }
      ++loopCount;
      //Will try to write again if a busy or error signal is present.
    } while (busy || error);

    return *this;
  }

  /**
   * This is used to read from any register on the voltage monitor.
   *
   * It takes a single arguement that is the address of the register that you wish to write
   * to and it returns the current settings at that address.
   */
  u32 Fed9UVoltageMonitor::readVoltageMonitor(u32 regAddress) const {
    ICUTILS_VERIFY(regAddress < 0x74)(regAddress).error().msg("The register address is invalid.");
    //If the write fails then either a busy or an error signle can be returned by the EPROM.
    bool busy  = false;
    bool error = false;
    //Used to limit the number of times that it attempts to write if a busy or error signal is present.
    u32 loopCount      = 0;
    const u32 maxLoops = 100;
    //Used to hold the value that is read back.
    u32 voltageMonitorData;
    //theFed requires a vector with the second element for the register address that is to be written to.
    vector<u32> regAddr(2,0);
    regAddr[1] = regAddress;
    //This is require for theFed to know it is reading.
    bool read = true;
    do {
      //Read from the voltage monitor.
      theFed.vmeCommandVoltageMonitorControl(read, regAddr, voltageMonitorData, busy, error);
      //If the write still fails after a set number of attempts then
      //the software gives up and throws an error.
      ICUTILS_VERIFY(loopCount < maxLoops)(busy)(error).error().msg("If busy = 1, then voltage monitor is busy. If error = 1 then the write failed");
      //If the write fails there is a pause before it tries again.
      if (busy || error) {
	fed9Uwait(1000000);
      }
      ++loopCount;
    } while (busy || error);

    return voltageMonitorData;
  }


}// End of Fed9U namespace.
