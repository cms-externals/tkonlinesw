#ifndef H_Fed9UTempControl
#define H_Fed9UTempControl

#include "TypeDefs.hh"
#include <iostream>

namespace Fed9U {

  /**
   * \brief A class that stores all the read write settings on an LM82 temperature monitor that monitor each FPGA on the FED.
   */
  class Fed9UTempControl {
  public:

    /**
     * \brief Copy constructor.
     * \param temp Fed9UTempControl object to be copied.
     *
     * Creates a new Fed9UTempControl object by copying the settings in an existing object.
     */
    Fed9UTempControl(const Fed9UTempControl& temp) : 
      _lm82High(temp.getLm82High()), _fpgaHigh(temp.getFpgaHigh()), _critical(temp.getCritical())
    { }

    /**
     * \brief Default constructor.
     * \param lm82High The is the temperature at which the LM82 will flag its own temperature as being too high. Default 127oC.
     * \param fpgaHigh The is the temperature at which the LM82 will flag the FPGA it is monitoring as its temperature as being too high. Default 127oC.
     * \param critical The temperature at which the LM82 will cause the FED to shut down to prevent either itself or an FPGA from over heating. Default 127oC.
     */
    Fed9UTempControl(u32 lm82High = 127, u32 fpgaHigh = 127, u32 critical = 127) :
      _lm82High(lm82High), _fpgaHigh(fpgaHigh), _critical(critical)
    { }

    /**
     * \brief  Returns the high temperature for the LM82 chip.
     * \return unsigned long.
     */
    u32 getLm82High() const { return _lm82High; }

    /**
     * \brief  Returns the high temperature for the FPGA chip that is being monitored.
     * \return unsigned long.
     */
    u32 getFpgaHigh() const { return _fpgaHigh; }

    /**
     * \brief  Returns the critical temperature for either the LM82 or its FPGA.
     * \return unsigned long.
     */
    u32 getCritical() const { return _critical; }

    /**
     * \brief  Sets the high point of the LM82 chip.
     * \param  lm82High High point of the LM82 to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setLm82High(u32 lm82High) { _lm82High = lm82High; return *this; }

    /**
     * \brief  Sets the high point of the FPGA being monitored.
     * \param  fpgaHigh High point of the FPGA being monitored to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setFpgaHigh(u32 fpgaHigh) { _fpgaHigh = fpgaHigh; return *this; }

    /**
     * \brief  Sets the critical temperature of the LM82 and FPGA.
     * \param  critical Critical temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControl & setCritical(u32 critical) { _critical = critical; return *this; }

  private:
    u32 _lm82High, _fpgaHigh, _critical; //!< Private data members to store the class settings.
    
    // <NAC date="24/04/2007">
    friend bool operator == (const Fed9UTempControl& l, const Fed9UTempControl& r);
    // </NAC>
  };
  
  // <NAC date="24/04/2007"> operator to compare TempControls
  /**
   * \brief  == operator to allow Fed9UTempControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  inline bool operator == (const Fed9UTempControl& l, const Fed9UTempControl& r)
  {
    if (l._lm82High != r._lm82High) return false;
    if (l._fpgaHigh != r._fpgaHigh) return false;
    if (l._critical != r._critical) return false;
    return true;
  }
  /**
   * \brief  != operator to allow Fed9UTempControls to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UTempControl& l, const Fed9UTempControl& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief A class that contains all the read/write and read only settings on an LM82 temperature monitor.
   *
   * Derived from the class Fed9UTempControl, it extends the interface to include the read only settings on the LM82 chip as well.
   */
  class Fed9UTempControlInfo : public Fed9UTempControl {
  public:

    /**
     * \brief Defuault constructor.
     *
     * Initialises the Fed9UTempControl settings to their default values and the LM82 and FPGA actual temperatures to zero, chip ID to zero
     * and its status to zero.
     */
    Fed9UTempControlInfo():Fed9UTempControl(),_lm82Actual(0), _fpgaActual(0), _id(0), _status(0)
    {}

    /**
     * \brief Constructor.
     * \param lm82High The is the temperature at which the LM82 will flag its own temperature as being too high.
     * \param fpgaHigh The is the temperature at which the LM82 will flag the FPGA it is monitoring as its temperature as being too high.
     * \param critical The temperature at which the LM82 will cause the FED to shut down to prevent either itself or an FPGA from over heating.
     * \param lm82Actual The actual temperature of the LM82 object.
     * \param fpgaActual The actual temperature of the monitored FPGA.
     * \param id ID of the LM82 temperature monitor.
     * \param status Status of the LM82 temperature monitor.
     *
     * This constructor initialises all the LM82 temperature monitor parameters explicitly.
     */
    Fed9UTempControlInfo(u32 lm82High, u32 fpgaHigh, u32 critical,
                         u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(lm82High, fpgaHigh, critical),
      _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }

    /**
     * \brief Constructor.
     * \param temp Fed9UTempControl object to initialise the base class through the copy constructor.
     * \param lm82Actual The actual temperature of the LM82 object.
     * \param fpgaActual The actual temperature of the monitored FPGA.
     * \param id ID of the LM82 temperature monitor.
     * \param status Status of the LM82 temperature monitor.
     */
    Fed9UTempControlInfo( const Fed9UTempControl& temp, u32 lm82Actual, u32 fpgaActual, u32 id, u32 status) :
      Fed9UTempControl(temp), _lm82Actual(lm82Actual), _fpgaActual(fpgaActual), _id(id), _status(status)
    { }

    /**
     * \brief  Returns the actual temperature of the LM82 chip.
     * \return unsigned long.
     */
    u32 getLm82Actual() const { return _lm82Actual; }

    /**
     * \brief  Returns the actual temperature of the FPGA being monitored.
     * \return unsigned long.
     */
    u32 getFpgaActual() const { return _fpgaActual; }

    /**
     * \brief  Returns the ID of the LM82 chip.
     * \return unsigned long.
     */
    u32 getId() const { return _id; }

    /**
     * \brief  Returns the status of the LM82 chip. Can be decoded with the enum Fed9UTempMonitorStatus.
     * \return unsigned long.
     */
    u32 getStatus() const { return _status; }

    /**
     * \brief  Sets the actual temperature of the LM82.
     * \param  lm82Actual Temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setLm82Actual(u32 lm82Actual) { _lm82Actual = lm82Actual; return *this; }

    /**
     * \brief  Sets the actual temperature of the monitored FPGA.
     * \param  fpgaActual Temperature to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setFpgaActual(u32 fpgaActual) { _fpgaActual = fpgaActual; return *this; }

    /**
     * \brief  Sets the ID of the LM82 the object refers to.
     * \param  id ID to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setId(u32 id) { _id = id; return *this; }

    /**
     * \brief  Sets the status of the LM82 chip.
     * \param  status Status value to be set.
     * \return Self reference.
     */
    Fed9UTempControlInfo & setStatus(u32 status) { _status = status; return *this; }

  private:
    u32 _lm82Actual, _fpgaActual, _id, _status; //!< Private data members to store the class settings.
    
  };

  /**
   * \brief  Overload of the operator<< which allows a Fed9UTempControl object to be written to an output stream.
   * \param  os The output stream to be written to.
   * \param  data Fed9UTempControl object to be written to file.
   * \return std::ostream Reference to the output stream that was written to.
   */
  inline std::ostream& operator << (std::ostream & os, const Fed9UTempControl& data) {
    return os << data.getLm82High() 
              << '\t' 
              << data.getFpgaHigh() 
              << '\t' << data.getCritical() 
              << '\n';
  }
  
  /**
   * \brief  Overload of the operator<< which allows a Fed9UTempControlInfo object to be written to an output stream.
   * \param  os The output stream to be written to.
   * \param  data Fed9UTempControlInfo object to be written to file.
   * \return std::ostream Reference to the output stream that was written to.
   */
  inline std::ostream& operator << (std::ostream & os, const Fed9UTempControlInfo& data) {
    return os //<< reinterpret_cast<const Fed9UTempControl&>(data) //removed this cause we don't really need to see the cut of temps and high temps. we only want to see the actual temps.
              << data.getLm82Actual() 
              << '\t' 
              << data.getFpgaActual()
              << '\t' 
              << data.getId() 
              << '\t' 
              << data.getStatus()
              << '\n';
  }
  
  /**
   * \brief  Overload of the operator>> which allows a Fed9UTempControl object to be read from an input stream.
   * \param  is The input stream to be read from.
   * \param  data Fed9UTempControl object to be written to file.
   * \return std::istream Reference to the output stream that was written to.
   */
  inline std::istream& operator >> (std::istream & is, Fed9UTempControl& data) {
    u32 temp;
    is >> temp;
    data.setLm82High(temp);
    is >> temp;
    data.setFpgaHigh(temp);
    is >> temp;
    data.setCritical(temp);
    return is;
  }
  
  /**
   * \brief  Overload of the operator>> which allows a Fed9UTempControlInfo object to be read from an input stream.
   * \param  is The input stream to be read from.
   * \param  data Fed9UTempControlInfo object to be written to file.
   * \return std::istream Reference to the output stream that was written to.
   */
  inline std::istream& operator >> (std::istream & is, Fed9UTempControlInfo& data) {
    u32 temp;
    is >> reinterpret_cast<Fed9UTempControl&>(data);
    is >> temp;
    data.setLm82Actual(temp);
    is >> temp;
    data.setFpgaActual(temp);
    is >> temp;
    data.setId(temp);
    is >> temp;
    data.setStatus(temp);
    return is;
  }
  
}
#endif // H_Fed9UTempControl
