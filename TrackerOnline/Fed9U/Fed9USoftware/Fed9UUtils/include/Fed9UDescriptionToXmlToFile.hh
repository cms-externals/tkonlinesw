#ifndef _Fed9UDescriptionToXmlToFile_H_
#define _Fed9UDescriptionToXmlToFile_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UDescriptionToXml.hh"
#include "Fed9UDescription.hh"

//Standard headers
#include<string>

namespace Fed9U {

  //using.*std::string;

  /**
   * \brief  Class which will write a Fed9UDescription to file in XML format.
   * \author Matthew Pearson, RAL.
   * \date   22/10/2004.
   *
   * This class uses the Xerces 2.3 library. The XML file format will conform to the XML Schema for the Fed9USoftware. This class inherits from
   * Fed9UDescriptionToXml, which provides the implementation. The constructor takes as parameters the file name to be created and a reference
   * to the Fed9UDescription, from which the XML will be constructed. To perform the construction of the XML file call the writeXmlFile() method.
   */
  class Fed9UDescriptionToXmlToFile : public Fed9UDescriptionToXml {

  public:

    /**
     * \brief Constructor. Write a description to file.
     * \param targetXMLFileName Name of the file to write the Fed9UDescription settings to.
     * \param Fed9UDescriptionToWrite Reference to the Fed9UDescription whose settings are to be written to file.
     */
    Fed9UDescriptionToXmlToFile(const std::string &targetXMLFileName, Fed9UDescription &Fed9UDescriptionToWrite);
    
    /**
     * \brief Constructor. Write a group of description to a single file.
     * \param targetXMLFileName Name of the file to write the Fed9UDescription objects to.
     * \param Fed9UDescriptionToWrite Vector containing pointers to the Fed9UDescription objects whose settings are to be written to the file.
     * \throw Fed9UXMLDescriptionException Throws this error if there are any error during construction and also catches and rethrows any exception
     *        thrown by other functions during the function call.
     */
    Fed9UDescriptionToXmlToFile(const std::string &targetXMLFileName, std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite ) noexcept(false);
    
    /**
     * \brief Method which construct and writes the XML to the file.
     */
    void writeXmlFile(void) noexcept(false);
    
  private:
    std::string theTargetXMLFileName; //!< File to write the XML data to.
  };

}

#endif
#endif
#endif
