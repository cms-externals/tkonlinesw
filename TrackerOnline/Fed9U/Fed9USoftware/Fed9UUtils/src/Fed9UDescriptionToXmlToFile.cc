/**Matthew Pearson, 17 Feb 2004.
Class which will write a Fed9UDescription to a file in XML format.
This class uses the Xerces 2.3 library.*/

//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UDescriptionToXml."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
//#pragma message(_XERCES_VERSION)
#if _XERCES_VERSION >= 20300

//Fed9U Headers
#include "Fed9UDescriptionToXmlToFile.hh"

//Standard includes
#include <fstream>

namespace Fed9U {

  //using std::ofstream;


  Fed9UDescriptionToXmlToFile::Fed9UDescriptionToXmlToFile(const std::string &targetXMLFileName, Fed9UDescription &Fed9UDescriptionToWrite) : Fed9UDescriptionToXml(Fed9UDescriptionToWrite), theTargetXMLFileName(targetXMLFileName) {
    
  }


  Fed9UDescriptionToXmlToFile::Fed9UDescriptionToXmlToFile(const std::string &targetXMLFileName, std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite ) throw (Fed9UXMLDescriptionException) : Fed9UDescriptionToXml(targetXMLFileName, Fed9UDescriptionsToWrite ), theTargetXMLFileName(targetXMLFileName) {
  
  }


  void Fed9UDescriptionToXmlToFile::writeXmlFile(void) throw (Fed9UXMLDescriptionException) {
    try {
      if (debugOutput_)
	std::cout << "calling :: writeXmlFile() ... filename = " << theTargetXMLFileName << std::endl;
      std::ofstream fileo(theTargetXMLFileName.c_str());
     if (debugOutput_)
	std::cout << "constructed ofstream!!!)" << std::endl;
      ICUTILS_VERIFY( fileo.is_open() ).error().msg("cannot open file for writing");
      try {
	if (debugOutput_)
	  std::cout << "stream out!" << std::endl;
	streamOutDescription(&fileo);
	if (debugOutput_)
	  std::cout << "streamed out!" << std::endl;
      }
      catch (Fed9UXMLDescriptionException &e) {
	std::string theErrorMsg = "Fed9UXMLDescriptionException caught in Fed9UDescriptionToXmlToFile::writeXmlFile. ";
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,theErrorMsg.c_str()));
      }
      fileo.close();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Fed9UXMLDescriptionException caught during the creation of the XML file."));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Caught std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Caught unknown exception."));
    }
  }
    
} //End of Fed9U namespace

#endif
#endif
