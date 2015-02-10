#ifndef H_Fed9UVoltageControlInfo
#define H_Fed9UVoltageControlInfo

#include "Fed9UVoltageControl.hh"
#include "TypeDefs.hh"

#include <vector>

namespace Fed9U {

  /**
   * \brief  A class to provide a software representation of the ADM1025 voltage monitor.
   * \author Gareth Rogers, RAL.
   * \date   2004/04.
   *
   * This class is designed to read back all the read only settings on the voltage
   * monitor. It inherits from Fed9UVoltageControl, so it can also read the read
   * write settings on the ADM1025 voltage controller. All voltages are accurate
   * to 3 decimal places.
   */
  class Fed9UVoltageControlInfo : public Fed9UVoltageControl {

  public:

    /**
     * \brief Constructor.
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
     * \param actual2Point5Volt Sets the actual 2.5V reading taken by the by the monitor. Default 0.
     * \param actual3Point3Volt Sets the actual 3.3V reading taken by the monitor. Default 0.
     * \param actual5Volt  Sets the actual 5V reading taken by the monitor. Default 0.
     * \param actual12Volt Sets the actual 12V reading taken by the monitor. Default 0.
     * \param actualCoreVoltage Sets the actual core voltage reading taken by the monitor. Default 0.
     * \param actualSupplyVoltage Sets the actual supply voltage reading taken by the monitor. Default 0.
     * \param actualExternalTempSets the actual external temperature measurement taken by the monitor. Default 0.
     * \param actualInternalTemp Sets the actual internal temperature measurement taken by the monitor. Default 0.
     * \param manufacturesId Sets the manufactures ID of the voltage monitor. Default 0.
     * \param steppingId Sets the stepping ID of the voltage monitor. Default 0.
     * \param statusRegister Sets the status register bits of the voltage monitor. Default 0.
     *
     * A Fed9UVoltageControlInfo object can be default constructed. If this is the case then all limits
     * will be set to their maximum values and no resets will be flagged.
     */
    Fed9UVoltageControlInfo(bool standBy = false,
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
			    i16 minExternalTemp    = 0,
			    i16 maxInternalTemp    = 127,
			    i16 minInternalTemp    = 0,
			    i16 tempOffset = 0,
			    bool offsetInternalTemp   = false,
			    float actual2Point5Volt   = 0,
			    float actual3Point3Volt   = 0,
			    float actual5Volt  = 0,
			    float actual12Volt = 0,
			    float actualCoreVoltage   = 0,
			    float actualSupplyVoltage = 0,
			    i16 actualExternalTemp  = 0,
			    i16 actualInternalTemp  = 0,
			    u32 manufacturesId = 0,
			    u32 steppingId     = 0,
			    u16 statusRegister = 0);

    /**
     * \brief  This sets the actual voltage on the 2.5V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual2Point5Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 2.5V.
     * \return float.
     */
    float getActual2Point5Volt() const;

    /**
     * \brief  This sets the actual voltage on the 3.3V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual3Point3Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 3.3V.
     * \return float.
     */
    float getActual3Point3Volt() const;

    /**
     * \brief  This sets the actual voltage on the 5V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual5Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 5V.
     * \return float.
     */
    float getActual5Volt() const;

    /**
     * \brief  This sets the actual voltage on the 12V.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActual12Volt(float voltage);

    /**
     * \brief  Returns the actual voltage on the 12V.
     * \return float.
     */
    float getActual12Volt() const;

    /**
     * \brief  This sets the actual voltage on the processore core voltage.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualCoreVoltage(float voltage);

    /**
     * \brief  Returns the actual voltage on the processore voltage.
     * \return float.
     */
    float getActualCoreVoltage() const;

    /**
     * \brief  This sets the actual voltage on the 3.3V supply voltage.
     * \param  voltage Voltage reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualSupplyVoltage(float voltage);

    /**
     * \brief  Returns the actual voltage on the 3.3V supply voltage.
     * \return float.
     */
    float getActualSupplyVoltage() const;

    /**
     * \brief  This sets the current external temperature.
     * \param  temp Temperature reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualExternalTemp(i16 temp);

    /**
     * \brief  Returns the current external temperature.
     * \return signed short.
     */
    i16 getActualExternalTemp() const;

    /**
     * \brief  This sets the actual internal temperature.
     * \param  temp Temperature reading to be set.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setActualInternalTemp(i16 temp);

    /**
     * \brief  Returns the actual internal temperature.
     * \return signed short.
     */
    i16 getActualInternalTemp() const;

    /**
     * \brief  This sets the manufactures ID.
     * \param  manufacturesId Sets the voltage monitor ID as read from the chip.
     * \return Self reference.
     */
    Fed9UVoltageControlInfo& setManufacturesId(u32 manufacturesId);

    /**
     * \brief  Returns the manufactures ID.
     * \return unsigned long.
     */
    u32 getManufacturesId() const;

    /**
     * \brief  This sets the stepping ID.
     * \param  steppingId Set the voltage monitor ID as read from the chip.
     */
    Fed9UVoltageControlInfo& setSteppingId(u32 steppingId);

    /**
     * \brief  Returns the stepping ID.
     * \return unsigned long.
     */
    u32 getSteppingId() const;

    /**
     * \brief  This sets all the settings in the status register. The enum Fed9UVoltageStatus can be
     *         used to set the flags for the voltage readings.
     * \param  statusRegister Contains the status bits that are to be set.
     * \return Self reference.
     *
     * A bitwise OR can be used to set multiple flag. Any flag not set will be cleared. Further details
     * about the meaning of the bits can be found in the enum Fed9UVoltageStatus documentation.
     */
    Fed9UVoltageControlInfo& setStatusRegister(u16 statusRegister);

    /**
     * \brief  Returns an unsigned short that contains the status bits for the voltage monitor chip.
     * \return unsigned short.
     *
     * The status bits can be decoded with the Fed9UVoltageStatus enum and an explanation of what each
     * bit means can be found in the Fed9UVoltageStatus enum documentation.
     */
    u16 getStatusRegister() const;

    /**
     * Some private data members used to store the state of a Fed9UVoltageControl object.
     */
  private:

    float actual2Point5Volt_, actual3Point3Volt_, actual5Volt_, actual12Volt_;
    float actualCoreVoltage_, actualSupplyVoltage_;
    i16 actualExternalTemp_, actualInternalTemp_;
    u32 manufacturesId_, steppingId_;
    u16 statusRegister_;

  };// End of class Fed9UVoltageControlInfo.

  /**
   * \brief  This overload of operator<< can used to write a Fed9UVoltageControlInfo to an ostream.
   * \param  os Output stream the data is to be written to.
   * \param  data Fed9UVoltageControlInfo class the data is to be written to the stream.
   * \return std::ostream Reference to the output stream the data was written to.
   */
  std::ostream& operator<<(std::ostream & os, const Fed9UVoltageControlInfo& data);

  /**
   * \brief  This overload of operator>> can used to read a Fed9UVoltageControlInfo from an istream.
   * \param  is Input stream the data is to be read from.
   * \param  data Fed9UVoltageControlInfo class the data is to be loaded from the stream.
   * \return std::istream Reference to the input stream the data was read from.
   */
  std::istream& operator>>(std::istream & is, Fed9UVoltageControlInfo& data);

}// End of Fed9U namespace.
#endif// H_Fed9UVoltageControlInfo.
