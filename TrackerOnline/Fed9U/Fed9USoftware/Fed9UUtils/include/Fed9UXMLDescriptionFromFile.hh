/**Matthew Pearson, 20/10/04.*/
/**This class enables the user to convert a XML file into a Fed9UDescription. It inherits functionality from 
   Fed9UXMLDescription. The XML file format must accord to the XML schema for the Fed9USoftware. 
   The XML file is used to set specific parameters in the Fed9UDescription, 
   depending on which XML tags and atttributes are in the file. The file does not have to contain a description
   of a whole FED, only a subset of it (for example, all the settings for a specified channel). However, if the user
   wishes, a complete XML description of a FED can be used by this class to completely override every parameter in a Fed9UDescription.*/

#ifndef _Fed9UXMLDescriptionFromFile_H_
#define _Fed9UXMLDescriptionFromFile_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UDescription.hh"
#include "Fed9UXMLDescription.hh"

//Std headers
#include<string>

namespace Fed9U {

  //using.*std::string;

  class Fed9UXMLDescriptionFromFile : public Fed9UXMLDescription
  {
  public:
    /**Constructor. This takes as arguments the XML file name and a reference to the Fed9UDescription to be modified.*/
    Fed9UXMLDescriptionFromFile(const std::string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips=true);

    /**Method which modifies the Fed9UDescription. 
     It modifies the object member reference to the Fed9UDescription.*/
    //    vector<Fed9UDescription*>*  makeNewFed9UDescription(void) throw (Fed9U::Fed9UXMLDescriptionException);

  private:
    std::string theXMLFileName;

  };

}

#endif
#endif

#endif //_Fed9UXMLDescriptionFromFile_H_
