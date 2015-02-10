#ifndef H_Fed9UVoltageControl
#define H_Fed9UVoltageControl

#include "TypeDefs.hh"

#include <vector>

namespace Fed9U {

  /**
   * \brief  A class to provide a software representation of the ADM1025 voltage monitor.
   * \author Gareth Rogers, RAL.
   * \date   2004/04.
   *
   * This class can be used to configure the read and write registers on the ADM1025
   * voltage montior. It can monitor a 2.5V, the processor core voltage, 3.3V, 12V and its supply voltage as well
   * as it own and an external temperature.  All voltages are accurate to 3 decimal places.
   */
  class Fed9UVoltageControl {

  public:

    /**
     * \brief Default constructor.
     *
     * \param standby Set the voltage monitor into a stand by mode where no values are read or monitored. Default false.
     * \param resetStatusRegister A true will reset the voltage monitor when the settings are loaded. Default false.
     * \param max2Point5Volt Maximum value the 2.5V line can reach before it is flagged. Range:0-3.320, default 3.320.
     * \param min2Point5Volt Minium value the 2.5V line can drop to before it is flagged. Range:0-3.320, default 0.
     * \param max3Point3Volt Maximum value the 3.3V line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param min3Point3Volt Minium value the 3.3V line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param max5Volt Maximum value the 5V line can reach before it is flagged. Range:0-6.640, default 6.640.
     * \param min5Volt Minium value the 5V line can drop to before it is flagged. Range:0-6.640, default 0.
     * \param max12Volt Maximum value the 12V line can reach before it is flagged. Range:0-15.938, default 15.938.
     * \param min12Volt Minium value the 12V line can drop to before it is flagged. Range:0-15.938, default 0.
     * \param maxCoreVoltage Maximum value the core voltage line can reach before it is flagged. Range:0-2.988, default 2.988.
     * \param minCoreVoltage Minium value the core voltage line can drop to before it is flagged. Range:0-2.988, default 0.
     * \param maxSupplyVoltage Maximum value the supply voltage line can reach before it is flagged. Range:0-4.383, default 4.383.
     * \param minSupplyVoltage Minium value the supply voltage line can drop to before it is flagged. Range:0-4.383, default 0.
     * \param maxExternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minExternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default -128.
     * \param maxInternalTemp Maximum value the external temperature can reach before being flagged. Range:-128 to 127, default 127.
     * \param minInternalTemp Minimum value the internal temperature can drop to before being flagged. Range:-128 to 127, default 0.
     * \param tempOffset Offset that is to be applied to the internal temperature readings., default 0.
     * \param offsetInternalTemp Whether the internal or external temperature offset value is to be added to the temperature readings. Default true.
     *
     * A Fed9UVoltageControl object can be default constructed. If this is the case then all limits
     * will be set to their maximum values and no resets will be flagged.
     */
    Fed9UVoltageControl(bool standBy = false,
			bool resetStatusRegister    = false,
			float max2Point5Volt  = 3.320,
			float min2Point5Volt  = 0,
			float max3Point3Volt  = 4.383,
			float min3Point3Volt  = 0,
			float max5Volt        = 6.640,
			float min5Volt        = 0,
			float max12Volt       = 15.938,
			float min12Volt       = 0,
			float maxCoreVoltage  = 2.988,
			float minCoreVoltage  = 0,
			float maxSupplyVoltage = 4.383,
			float minSupplyVoltage = 0,
			i16 maxExternalTemp    = 127,
			i16 minExternalTemp    = -128,
			i16 maxInternalTemp    = 127,
			i16 minInternalTemp    = 0,
			i16 tempOffset = 0,
			bool offsetInternalTemp = true);

    /**
     * \brief  This can be used to place the voltage controller in a stand by mode which does not perform
     *         any measurements or conversion and can be used to save power when the voltage controller is
     *         not needed.
     * \param  standBy True puts the voltage monitor into standby, false takes it out.
     * \return Self reference.
     *
     * This will not reset the chip, all register values will be kept.
     */
    Fed9UVoltageControl& setStandBy(bool standBy);

    /**
     * \brief  Returns the running state of the voltage controller, in standby or monitoring.
     * \return Boolean
     *
     * This will not reset the chip, all register values will be kept.
     */
    bool getStandBy() const;

    /**
     * \brief  This provides the ability to reset the status registers to their hardware defaults.
     * \param  reset Passing a true will flag the status register for a reset, a false will not perform a reset.
     * \return Self reference.
     */
    Fed9UVoltageControl& setResetStatusRegister(bool reset);

    /**
     * \brief  Returns the state of the reset status register flag.
     * \return Boolean.
     */
    bool getResetStatusRegister() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 2.5V reading.
     * \param  voltage Upper limit to be set. Range:0-3.320V.
     * \return Self reference.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     * It has a maximum value of 3.320V and a minimum of 0.
     */
    Fed9UVoltageControl& set2Point5VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 2.5V reading.
     * \return float.
     *
     * If the 2.5V level is greater than this value it will be flagged in the status register.
     */
    float get2Point5VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 2.5V reading.
     * \param  voltage Limit to be set. Range:0-3.320V.
     * \return Self reference.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set2Point5VoltMin(float voltage);

    /**
     * Returns the lower limit warning voltage for the 2.5V reading.
     * \return float.
     *
     * If the 2.5V level is less than or equal to this value it will be flagged in the status register.
     */
    float get2Point5VoltMin() const;

    /**
     * This sets the upper limit warning voltage for the 3.3V reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set3Point3VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 3.3V reading.
     * \return float.
     *
     * If the 3.3V level is greater than this value it will be flagged in the status register.
     */
    float get3Point3VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 3.3V reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set3Point3VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 3.3V reading.
     * \return float.
     *
     * If the 3.3V level is less than or equal to this value it will be flagged in the status register.
     */
    float get3Point3VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 5V reading.
     * \param  voltage Limit to be set. Range:0-6.640V.
     * \return Self reference.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set5VoltMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the 5V reading.
     * \return float.
     *
     * If the 5V level is greater than this value it will be flagged in the status register.
     */
    float get5VoltMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the 5V reading.
     * \param  voltage Limit to be set. Range:0-6.640V.
     * \return Self reference.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set5VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 5V reading.
     * \return float.
     *
     * If the 5V level is less than or equal to this value it will be flagged in the status register.
     */
    float get5VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the 12V reading.
     * \param  voltage Limit to be set. Range:0-15.938V.
     * \return Self reference.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set12VoltMax(float voltage);

    /**
     * Returns the upper limit warning voltage for the 12V reading.
     * \return float.
     *
     * If the 12V level is greater than this value it will be flagged in the status register.
     */
    float get12VoltMax() const;

    /**
     * This sets the lower limit warning voltage for the 12V reading.
     * \param  voltage Limit to be set. Range:0-15.938V.
     * \return Self reference.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& set12VoltMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the 12V reading.
     * \return float.
     *
     * If the 12V level is less than or equal to this value it will be flagged in the status register.
     */
    float get12VoltMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the core voltage reading.
     * \param  voltage Limit to be set. Range:0-2.988V.
     * \return Self reference.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setCoreVoltageMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the core voltage reading.
     * \return float.
     *
     * If the core voltage level is greater than this value it will be flagged in the status register.
     */
    float getCoreVoltageMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the core voltage reading.
     * \param  voltage Limit to be set. Range:0-2.988V.
     * \return Self reference.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setCoreVoltageMin(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the core voltage reading.
     * \return float.
     *
     * If the core voltage level is than or equal to this value it will be flagged in the status register.
     */
    float getCoreVoltageMin() const;

    /**
     * \brief  This sets the upper limit warning voltage for the supply voltage reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setSupplyVoltageMax(float voltage);

    /**
     * \brief  Returns the upper limit warning voltage for the supply voltage reading.
     * \return float.
     *
     * If the supply voltage level is greater than this value it will be flagged in the status register.
     */
    float getSupplyVoltageMax() const;

    /**
     * \brief  This sets the lower limit warning voltage for the supply voltage reading.
     * \param  voltage Limit to be set. Range:0-4.383V.
     * \return Self reference.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setSupplyVoltageMin(float voltage);

    /**
     * \brief  Returns the lower limit warning voltage for the supply voltage reading.
     * \return float.
     *
     * If the supply voltage level is less than or equal to this value it will be flagged in the status register.
     */
    float getSupplyVoltageMin() const;

    /**
     * \brief  This sets the upper limit warning temperature for the external temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127oC.
     * \return Self reference.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setExternalTempMax(i16 temperature);

    /**
     * \brief  Returns the upper limit warning temperature for the external temperature reading.
     * \return signed long.
     *
     * If the external temperature level is greater than this value it will be flagged in the status register.
     * This has a maximum value of 127oC and a minimum value of -128oC.
     */
    i16 getExternalTempMax() const;

    /**
     * \brief  This sets the lower limit warning temperature for the external temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127oC.
     * \return Self reference.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setExternalTempMin(i16 temperature);

    /**
     * \brief  Returns the lower limit warning temperature for the external temperature reading.
     * \return signed long.
     *
     * If the external temperature level is less than or equal to this value it will be flagged in the status register.
     */
    i16 getExternalTempMin() const;

    /**
     * \brief  This sets the upper limit warning temperature for the internal temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127.
     * \return Self reference.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setInternalTempMax(i16 temperature);

    /**
     * \brief  Returns the upper limit warning temperature for the internal temperature reading.
     * \return signed long.
     *
     * If the internal temperature level is greater than this value it will be flagged in the status register.
     */
    i16 getInternalTempMax() const;

    /**
     * \brief  This sets the lower limit warning temperature for the internal temperature reading.
     * \param  temperature Limit to be set. Range:-128 to 127.
     * \return Self reference.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     */
    Fed9UVoltageControl& setInternalTempMin(i16 temperature);

    /**
     * \brief  Returns the lower limit warning temperature for the internal temperature reading.
     * \return signed long.
     *
     * If the internal temperature level is less than or equal to this value it will be flagged in the status register.
     */
    i16 getInternalTempMin() const;

    /**
     * \brief  The temperature measurement can have an offset added to the measured value. The offset can be
     *         added to either the external or the internal offset, but not to both.
     * \param  temperature Offset to be set. Range:-128 to 127.
     * \param  internal True offset is added to the internal measurement, false the external.
     * \return Self reference.
     */
    Fed9UVoltageControl& setTempOffset(i16 offset, bool internal);

    /**
     * \brief  Returns the offset that is added to either the internal or external temperature measurements.
     * \return signed long.
     */
    i16 getTempOffset() const;

    /**
     * \brief  Returns true if the offset is added to either the internal or false if added to the external temperature measurements.
     * \return Boolean.
     */
    bool getOffsetTempSelect() const;

    /**
     * Some private data members used to store the state of a Fed9UVoltageControl object.
     */
  private:
    bool standBy_, resetStatusRegister_;
    float max2Point5Volt_, min2Point5Volt_, max3Point3Volt_, min3Point3Volt_;
    float max5Volt_, min5Volt_, max12Volt_, min12Volt_, maxCoreVoltage_, minCoreVoltage_;
    float maxSupplyVoltage_, minSupplyVoltage_;
    i16 maxExternalTemp_, minExternalTemp_, maxInternalTemp_, minInternalTemp_, offset_;
    bool internal_;
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r);
    // </NAC>

  };// End of class Fed9UVoltageControl
  
  // <NAC date="24/04/2007"> operator to compare Volatge Monitors
  /**
   * \brief  == operator to allow Fed9UVoltageControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r);
  /**
   * \brief  != operator to allow Fed9UVoltageControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UVoltageControl& l, const Fed9UVoltageControl& r) { return !(l==r); }
  // </NAC>

  /**
   * \brief  This overload of operator<< can used to write a Fed9UVoltageControl to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UVoltageControl class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UVoltageControl& data);

  /**
   * \brief  This overload of operator>> can used to read a Fed9UVoltageControl from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UVoltageControl class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream & is, Fed9UVoltageControl& data);
  
}// End of Fed9U namespace
#endif// H_Fed9UVoltageControl
