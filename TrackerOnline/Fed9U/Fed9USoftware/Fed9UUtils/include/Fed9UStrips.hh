#ifndef H_Fed9UStrips
#define H_Fed9UStrips

#include <iosfwd>
#include <vector>

#include "Fed9UStripDescription.hh"
#include "Fed9UAddress.hh"

namespace Fed9U {

  /**
   * \brief  Description of all the strips in a FED.
   * \author Jonathan Fulcher
   *
   * Stores a Fed9UStripDescription for each APV strip in a FED.
   */

  class Fed9UStrips {
  public:

    /**
     * \Default constructor.
     *
     * Initialises each strip on the FED to have its default values.
     */
    Fed9UStrips();

    /**
     * \brief Destructor.
     */
    ~Fed9UStrips();

    /**
     * \brief  Returns a modifiable reference specific strip on a FED.
     * \param  fedStrip The FED strip that is to be retrieved.
     * \return Fed9UStripDescription&
     */
    Fed9UStripDescription& getStrip(Fed9UAddress fedStrip);

    /**
     * \brief  Returns a constant reference to a specific strip on a FED.
     * \param  fedStrip The FED strip that is to be retrieved.
     * \return Fed9UStripDescription&
     */
    const Fed9UStripDescription& getStrip(Fed9UAddress fedStrip) const;

    /**
     * \brief  Returns a vector containing a copy of all the strip descriptions on a given FED APV.
     * \param  fedApv Fed9UAddress class containing the address of the specific APV to be referenced.
     * \return vector<Fed9UStripDescription>
     */
    std::vector<Fed9UStripDescription> getApvStrips(Fed9UAddress fedApv) const;

    /**
     * \brief Sets the settings for a given strip on the FED.
     * \param fedStrip Address of the FED strip to be updated.
     * \param value Constant reference to the Fed9UStripDescription settings that are to be written to that strip.
     */
    void setStrip(Fed9UAddress fedStrip, const Fed9UStripDescription& value);

    /**
     * \brief  Sets all the strip descriptions on a given FED APV.
     * \param  fedApv Fed9UAddress class containing the address of the specific APV to be referenced.
     * \param  values Constant reference to a vector contain the settings for all the values on a given FED APV.
     */
    void setApvStrips(Fed9UAddress fedApv, const std::vector<Fed9UStripDescription> & values);

    /**
     * \brief Loads all the strip settings on a FED from an input stream.
     * \param is Reference to the input stream the settings are to be loaded from.
     */
    void loadStrips(std::istream& is);

    /**
     * \brief Saves all the strip settings on a FED to an input stream.
     * \param os Reference to the output stream where the settings are to be written to.
     */
    void saveStrips(std::ostream& os) const;

    /**
     * \brief Loads each strip one the FED with a default constructed Fed9UDescription object.
     */
    void loadDefaultStrips();

  private:
    //std::vector< std::vector< std::vector< std::vector<Fed9UStripDescription> > > > _strips;
    Fed9UStripDescription _strips[STRIPS_PER_FED];//!< Vector used to store a Fed9UStripDescription for each object on a FED.
    
  };
  
  // <NAC date="24/04/2007"> operator to compare strips
  /**
   * \brief  == operator to allow Fed9UStrips to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for == comparison
   */
  bool operator == (const Fed9UStrips& l, const Fed9UStrips& r);
  /**
   * \brief  != operator to allow Fed9UStrips to be compared
   * \param  l LHS
   * \param  r RHS
   * \return bool, true or false for != comparison
   */
  inline bool operator != (const Fed9UStrips& l, const Fed9UStrips& r) { return !(l==r); }
  // </NAC>
  
  /**
   * \brief  Overload of the operator>> which is used to read the strip settings from an input stream.
   * \param  is Input stream to read the settings from.
   * \param  fs Fed9UStrips that are to be read from the stream.
   * \return std::istream Reference to the input stream the setting were read from.
   */
  std::istream& operator>>(std::istream& is, Fed9UStrips& fs);
  
  /**
   * \brief  Overload of the operator<< which is used to write the strip settings to an output stream.
   * \param  os Output stream to write the settings to.
   * \param  fs Fed9UStrips that are to be written to the stream.
   * \return std::ostream Reference to the output stream the setting were written to.
   */
  std::ostream& operator<<(std::ostream&, const Fed9UStrips&);

}

#endif // H_Fed9UStrips
