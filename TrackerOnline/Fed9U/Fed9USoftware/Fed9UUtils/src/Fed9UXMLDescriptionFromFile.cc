#include <stdint.h>
#include <inttypes.h>

/**Matthew Pearson, 20/10/04.*/
/**Class which inherits from Fed9UXMLDescription to provide a file based interface. */

//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UXMLDescription."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UXMLDescriptionFromFile.hh"

//Std headers
#include<iostream>
#include<fstream>

namespace Fed9U {

  using std::cout;
  using std::endl;
  using std::ifstream;

  /**Constructor. This takes as arguments the XML file name and the Fed9UDescription to be modified.*/
  Fed9UXMLDescriptionFromFile::Fed9UXMLDescriptionFromFile(const std::string &XMLOverrideFile, Fed9UDescription &defaultFed9UDescription , std::vector<Fed9UDescription*>* desclist, bool usingStrips) : Fed9UXMLDescription(XMLOverrideFile, defaultFed9UDescription, desclist, usingStrips), theXMLFileName(XMLOverrideFile) {
    
  }

  /**Method which modifies the Fed9UDescription. 
     It modifies the object member reference to the Fed9UDescription.*/
  /* vector<Fed9UDescription*> * Fed9UXMLDescriptionFromFile::makeNewFed9UDescription(void) throw (Fed9UXMLDescriptionException) {
    try {
      ifstream theXMLBuf(theXMLFileName.c_str());
      if (!theXMLBuf.is_open()) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"ERROR: Did not open file correctly in Fed9UXMLDescriptionFromFile::makeNewFed9UDescription()."));
      }
      makeNewFed9UDescriptionFromStream(theXMLBuf);
      theXMLBuf.close();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Fed9UXMLDescriptionException caught in Fed9UXMLDescriptionFromFile::makeNewFed9UDescription()."));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Caught unknown exception."));
    }
    return theFed9UDescriptionList_;
  }
  */
} //End of Fed9U namespace


#endif
#endif
