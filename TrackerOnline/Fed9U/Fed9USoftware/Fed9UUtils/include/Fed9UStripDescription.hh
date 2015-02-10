#ifndef H_Fed9UStripDescription
#define H_Fed9UStripDescription

#include "TypeDefs.hh"
#include "ICAssert.hh"

#include <iosfwd>

namespace Fed9U {

  /**
   * \brief  Describes a strip.
   * \author Jonanthan Fulcher, IC.
   *
   * Contains the pedestal, high and low cluster threshold, and the disable setting for a given strip. These values are what they FED requires
   * to perform its signal processing when in processed or zero suppressed (lite) mode. This class provides an abstraction from these base
   * values and instead uses a base lines noise setting to determine by what factor the threshold should be above this noise level.
   * It provides high and low threshold factors to store this information. It has two constructors, however the default constructor
   * should be used (with or without arguments) in preference as this contains the noise and threshold factors, rather than the threshold
   * values. The second constructor converted the high and low thresholds into noise and uses the default values FED_STRIP_HIT_THRESHOLD_HIGH
   * and FED_STRIP_HIT_THRESHOLD_LOW as defined in TypeDefs.hh.
   */
  class Fed9UStripDescription {
  public:

    /**\name Constructors and destructor.*/
    //@{

    /**
     * \brief Constructor.
     *
     * \param pedestal Pedestal value for that strip. Only valid during processed raw, zero suppressed and zero suppressed lite modes.
     * \param highThreshold High threshold value for that strip. Defines the signal hight of single strip clusters in the zero suppression mode.
     * \param lowThreshold Low threshold value for that strip. Defines the signal hight of multi-strip clusters in zero suppression mode.
     * \param disable True if the strip is disabled, false if enabled.
     *
     * Initialises the settings for strip given in the constructor parameter list and sets the noise to the high threshold divided by
     * FED_STRIP_HIT_THRESHOLD_HIGH as defined in TypeDefs.hh. It is recommended not to use this constructor and to use the default
     * one provided instead.
     */
    Fed9UStripDescription(i16 pedestal,
			  i16 highThreshold,
			  i16 lowThreshold,
			  bool disable
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(FED_STRIP_HIT_THRESHOLD_HIGH),
      _lowThresholdFactor(FED_STRIP_HIT_THRESHOLD_LOW),
      _noise( (static_cast<float>(highThreshold)/static_cast<float>(FED_STRIP_HIT_THRESHOLD_HIGH)<=51.1)?(static_cast<float>(highThreshold)/static_cast<float>(FED_STRIP_HIT_THRESHOLD_HIGH)):51.1),
      _disable(disable) {}
  
    /**
     * \brief Default constructor.
     *
     * \param pedestal Pedestal value for that strip. Default 0.
     * \param highThresholdFactor High threshold factor value for that strip. Defines the signal hight of single strip clusters in the zero suppression mode,
     *        relative to the noise level. Default FED_STRIP_HIT_THRESHOLD_HIGH, as defined in TypeDefs.hh.
     * \param lowThresholdFactor Low threshold factor value for that strip. Defines the signal hight of multi-strip clusters in zero suppression mode,
     *        relative to the noise level. Default FED_STRIP_HIT_THRESHOLD_LOW, as defined in TypeDefs.hh.
     * \param noise Default 0.
     * \param disable True if the strip is disabled, false if enabled. Default false.
     *
     * Can either be used as a default constructor as all parameters have sensible default values or used to construct all the strip settings.
     * The threshold factors are used to define the high and low thresholds required by the FEDs by setting them to a value of the factor times
     * the noise level.
     */
    Fed9UStripDescription(i16 pedestal = 0,
			  float highThresholdFactor = FED_STRIP_HIT_THRESHOLD_HIGH,
			  float lowThresholdFactor = FED_STRIP_HIT_THRESHOLD_LOW,
			  float noise = 0.0,
			  bool disable = false
			  ) :
      _pedestal(pedestal),
      _highThresholdFactor(highThresholdFactor),
      _lowThresholdFactor(lowThresholdFactor),
      _noise(noise<=51.1?noise:51.1),
      _disable(disable) {}

    /**
     * \brief Destructor.
     */   
    ~Fed9UStripDescription() {}
    //@}

    /**\name Getters */
    //@{

    /**
     * \brief  Returns the pedestal value for that strip.
     * \return signed long.
     */
    i16 getPedestal() const { return _pedestal; }

    /**
     * \brief  Returns the high threshold value for that strip.
     * \return signed short.
     */
    i16 getHighThreshold() const { return _noise==0?0xFF:(static_cast<i16>(static_cast<float>(_highThresholdFactor)*_noise+0.5 ) >0xFF?0xFF:static_cast<i16>(static_cast<float>(_highThresholdFactor)*_noise+0.5 )); }

    /**
     * \brief  Returns the low threshold value for that strip.
     * \return signed short.
     */
    i16 getLowThreshold() const { return _noise==0?0xFF:(static_cast<i16>(static_cast<float>(_lowThresholdFactor)*_noise+0.5) >0xFF?0xFF:static_cast<i16>(static_cast<float>(_lowThresholdFactor)*_noise+0.5)); }

    /**
     * \brief  Returns the high threshold factor for that strip.
     * \return float.
     */
    float getHighThresholdFactor() const { return _highThresholdFactor; }

    /**
     * \brief  Returns the low threshold factor for that strip.
     * \return float.
     */
    float getLowThresholdFactor() const { return _lowThresholdFactor; }

    /**
     * \brief  Returns the noise value for that strip.
     * \return float.
     */
    float getNoise() const { return _noise; }

    /**
     * \brief  Returns the disable settings for that strip.
     * \return boolean.
     */
    bool getDisable() const { return _disable; }
    //@}

    /** \name Setters */
    //@{
    /**
     * \brief Sets the pedestal value for that strip.
     * \param value Pedestal value to be set.
     */
    void setPedestal(i16 value) { _pedestal = value; }

    /**
     * \brief Sets the high threshold factor value for that strip.
     * \param value High threshold factor to be set.
     */
    void setHighThresholdFactor(float value) { _highThresholdFactor = value; }

    /**
     * \brief Sets the low threshold factor value for that strip.
     * \param value Low threshold factor for that strip.
     */
    void setLowThresholdFactor(float value) { _lowThresholdFactor = value; }

    /**
     * \brief Sets the high threshold value for that strip.
     * \param value High threshold to be set.
     */
    void setHighThreshold(i16 value) { 
      if( _noise > 0 )
	_highThresholdFactor = static_cast<float>(value)/_noise; 
      else 
	_highThresholdFactor = 0;
    }

    /**
     * \brief Sets the low threshold value for that strip.
     * \param value Low threshold for that strip.
     */
    void setLowThreshold(i16 value) {    
      if ( _noise > 0 )
	_lowThresholdFactor = static_cast<float>(value)/_noise; 
      else
	_lowThresholdFactor = 0;
    }
    
    /**
     * \brief Sets the noise level for that strip.
     * \param value Noise value to be set.
     */
    void setNoise(float value) { value <= 51.1? _noise = value: _noise=51.1; }

    /**
     * \brief Set whether this strip is disabled or not.
     * \param value True disables the strip, false enables it.
     */
    void setDisable(bool value) { _disable = value; }
    //@}

    /**
     * \brief Loads the strip settings from an input stream.
     * \param is Input stream to load the parameters from.
     */
    void load(std::istream& is);

    /**
     * \brief Writes the strip settings to an output stream.
     * \param os The output stream to write the parameters to.
     */
    void save(std::ostream& os) const;

  private:
    i16	_pedestal;              //!< Holds the value of the pedestal.
    float _highThresholdFactor; //!< Holds the value of the low threshold factor.
    float _lowThresholdFactor;  //!< Holds the value of the high threshold factor.
    float _noise;               //!< Holds the value of the noise.
    bool _disable;              //!< Holds the disable setting.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UStripDescription& l, const Fed9UStripDescription& r);
    // </NAC>
  };
  
  // <NAC date="24/04/2007"> operator to compare stripDescriptions
  /**
   * \brief  == operator to allow Fed9UStripDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UStripDescription& l, const Fed9UStripDescription& r);
  /**
   * \brief  != operator to allow Fed9UStripDescriptions to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UStripDescription& l, const Fed9UStripDescription& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief  Overload of the operator<< which is used to write the strip settings to an output stream.
   * \param  os Output stream to write the settings to.
   * \param  sd Fed9UStripDescription settings that are to be written to the stream.
   * \return std::ostream Reference to the output stream the setting were written to.
   */
  std::ostream& operator<<(std::ostream& os, const Fed9UStripDescription& sd);

  /**
   * \brief  Overload of the operator>> which is used to read the strip settings from an input stream.
   * \param  is Input stream to read the settings from.
   * \param  sd Fed9UStripDescription settings that are to be written to the stream.
   * \return std::istream Reference to the input stream the setting were read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UStripDescription& sd);

}

#endif // H_Fed9UStripDescription
