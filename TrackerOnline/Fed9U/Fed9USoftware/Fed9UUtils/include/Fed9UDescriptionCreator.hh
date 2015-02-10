#ifndef _Fed9UDescriptionCreator_H_
#define _Fed9UDescriptionCreator_H_

#include "Fed9UDescription.hh"


//using.*std;

namespace Fed9U {

  /**
   * \brief A class to edit the Fed9UDescription settings and also display all the current settings in the Fed9UDescription.
   */
  class Fed9UDescriptionCreator{

  public:

    /**
     * \brief Constructor. Initialises the pointer to the description to be used.
     * \param ptr_desc Pointer to the description to be editted.
     */
    Fed9UDescriptionCreator(Fed9UDescription* ptr_desc);

    /**
     * \brief The User menu that provides a command line based interaction editting ability for the Fed9UDescription object.
     */
    void menu();
    
  private:
    /**
     * \brief Method to display FE settings.
     * \param feUnit FE unit settings which are to be displayed. Range:0-7, internal numbering.
     */
    void displayFeSettings(u16 feUnit);

    /**
     * \brief Method to display BE settings.
     */
    void displayBeSettings(void);

    /**
     * \brief Method to display channel settings.
     * \param channel FED channel for which the settings are to be displayed. Range:0-95, internal numbering.
     */
    void displayChannelSettings(u16 channel);

    /**
     * \brief Method to display strip settings.
     * \param strip FED strip for which the settings are to be displayed. Range:0-24,576, internal numbering.
     */
    void displayStripSettings(u16 strip);

    /**
     * \brief Method to ask the User to set the APV disable flag. Tells the FED to ignore any APV errors for that APV.
     * \param theFed9UAddress Fed9UAddress object containing the APV that is to have its disable flag set.
     */
    void setApvDisableFlag(const Fed9UAddress &theFed9UAddress);

    /**
     * \brief Method to ask the User to set the APV fake event disable flag. Removes the APV tick marks from the fake event.
     * \param theFed9UAddress Fed9UAddress object containing the APV that is to have its disable flag set.
     */
    void setApvFakeEventDisableFlag(const Fed9UAddress &theFed9UAddress);

    /**
     * \brief Method to ask the user for strip information (ie. a disable flag, pedestal and high and low cluster thresholds).
     * \param stripParameters Strips settings returned by reference in the vector elements. They are in the following order:
     *        \li 0: Disable flag.
     *        \li 1: Pedestal.
     *        \li 2: High threshold.
     *        \li 3: Low threshold.
     *
     * The vector should be passed in empty, if it is not it will be cleared.
     */
    void askForStripParameters(std::vector<u16> &stripParameters);

    /**
     * \brief Method which sets strip parameters in the Fed9UDescription.
     * \param stripParameters Strip settings vector with each element containing a different parameter. The ordering is given in
     *        askForStripParameters.
     * \param theFed9UAddress Fed9UAddress object containing the address of the specific strip that is to be set.
     */
    void setStripParameters(const std::vector<u16> &stripParameters, const Fed9UAddress &theFed9UAddress);

    Fed9UDescription* _ptr_desc; //!< Pointer to the description object that is being modified.

  };

}  

#endif
