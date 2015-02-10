//Jonathan Fulcher, 11/11/04
//Class which reads in an XML buffer from a database connection and extracts the parameters in order to overide the default
//Fed9UDescription. 

// don't bother building this class if the database is not defined
#ifdef DATABASE

//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UXMLDescription."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UXMLDescriptionFromDb.hh"

//Xerces headers
#include "Fed9UDOMCountErrorHandler.hh"
#include "Fed9UStrX.hh"
#include "Fed9UXStr.hh"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"

#include <iostream>
#include <sstream>


#include "DbFedAccess.h"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "xercesc/util/BinInputStream.hpp"



namespace Fed9U {
  u32 xmlBufferLength=0;
//   using xercesc::XMLPlatformUtils;
//   using xercesc::XMLUni;
//   using xercesc::XMLString;
//   using xercesc::XMLException;
//   using xercesc::DOMException;
//   using xercesc::DOMImplementation;
//   using xercesc::DOMImplementationRegistry;
//   //  using xercesc::DOMBuilder;
//   using xercesc::DOMImplementationLS;
//   //  using xercesc::DOMDocument;
//   using xercesc::DOMInputSource;
//   using xercesc::InputSource;
//   using xercesc::Wrapper4InputSource;
//   using xercesc::BinInputStream;

//   using std::cout;
//   using std::ostringstream;
//   using std::endl;
//   using std::cerr;
//   using std::ios;

  Fed9UXMLDescriptionFromDb::Fed9UXMLDescriptionFromDb ( DbFedAccess * dbFedAccess, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist, bool usingStrips ) : Fed9UXMLDescription(defaultFed9UDescription, desclist, usingStrips),
    xmlBuffer_(NULL),dbFedAccess_(dbFedAccess)
    {  }
 
  //Destructor
  Fed9UXMLDescriptionFromDb::~Fed9UXMLDescriptionFromDb()
  {
    try {
      //      std::cout << ((void *)xmlBuffer_) << " allocated (freddy delete in destructor)" << std::endl ;
      if ( xmlBuffer_ ) free((void*) xmlBuffer_);
    }
    catch (...) {
      cerr << "Unknown exception caught in Fed9UXMLDescription::~Fed9UXMLDescription(). " << endl;
    }
  }  


  // method to get the input source from the database 
  // FD change to include the masking version
  xercesc::DOMInputSource* Fed9UXMLDescriptionFromDb::getDatabaseDOMInputSource( i16 fedId, bool hardwareId,  int versionMajor , int versionMinor, std::string partition, int maskMajor, int maskMinor) {    
    ICUTILS_VERIFYX(fedId>=0 || partition !="null",Fed9UXMLDescriptionException)(countApvs).code(Fed9UXMLDescriptionException::ERROR_INCORRECT_USAGE).error().msg("if you are using no fedid you must have a partition!");
    //download the FED parameters from the database  
    oracle::occi::Clob *xmlClob = NULL;  
    if (xmlBuffer_ != NULL ) {
      //std::cout << ((void *)xmlBuffer_) << " allocated (freddy delete in get method)" << std::endl ;
      free((void*)xmlBuffer_);
    }
    xmlBuffer_ = NULL;
    xercesc::DOMInputSource* domInputSource;
    std::string xmlBufferId = "xmlBuffer" ;

    // if versionMajor is -1 or 0 then we get the latest version.
    if (versionMajor > 0) { // old scheme was to use -1 but now we just check for at least a positive major version...
      ICUTILS_VERIFY(versionMinor >= 0).error().msg("you must provide valid minor version!!!!");
      if (partition == "null")
	ICUTILS_VERIFY(0==1).error().msg("you must provide the partition!!!!");

	//xmlClob = dbFedAccess_->getXMLClob( fedId, hardwareId, versionMajor, versionMinor, usingStrips_);
      else 
	if (fedId >= 0) {
	  xmlClob = dbFedAccess_->getXMLClobWithVersion( partition, fedId, hardwareId, versionMajor, versionMinor, maskMajor, maskMinor, usingStrips_);
	} else {
	  xmlClob = dbFedAccess_->getXMLClobWithVersion( partition, versionMajor, versionMinor, maskMajor, maskMinor, usingStrips_);
	}
    } else {
      if (partition == "null")
	ICUTILS_VERIFY(0==1).msg("you must provide the partition!!!!").error();
	//xmlClob = dbFedAccess_->getXMLClob( fedId, hardwareId , usingStrips_);
      else
	if (fedId >= 0) {
	  xmlClob = dbFedAccess_->getXMLClob( partition, fedId, hardwareId , usingStrips_);
	} else {
	  xmlClob = dbFedAccess_->getXMLClob( partition, usingStrips_);
	}
    }
    if (xmlClob != NULL){    
      unsigned int resultLength = (*xmlClob).length();
#ifdef MESSAGE    
      std::cout << "resultLength : " << resultLength << std::endl;
#endif    
      if (resultLength>0) {      
	if ((xmlBuffer_ = (XMLByte *) calloc(resultLength+1, sizeof(char))) != NULL) {     
	  if (resultLength == (*xmlClob).read(resultLength, xmlBuffer_, resultLength+1)) {
	    // XMLBuffer_ contains the XML data and convert it to an input source    
	    xercesc::MemBufInputSource* xmlInputSource = new xercesc::MemBufInputSource (
									   (const XMLByte*)xmlBuffer_, 
									   strlen((const char*)xmlBuffer_), 
									   (const XMLCh *)xmlBufferId.c_str(), 
									   false, 
									   xercesc::XMLPlatformUtils::fgMemoryManager);      

	    // convert the input source to a DOMInputSource  
  	    domInputSource = new xercesc::Wrapper4InputSource((xercesc::InputSource*)xmlInputSource) ;
	    xmlBufferLength=resultLength;

	    //std::cout << ((void *)xmlBuffer_) << " allocated (freddy)" << std::endl ;
	  }       
	  else { 
	    THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_CLOB, "failed while reading the XML Clob"));
	  } 
	}
	else { 
	    THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_MEMORY, "failed during memory allocation" ));    
	}  
      }  
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_NO_DATA, "no data found" ));  
      }
    }  
    else {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_CLOB, "failed while trying to get the XML Clob" ));  
    }
    return domInputSource;
  }

  /*deprocated
  //Build a new method for the parsing 
  // Note the version will managed by the DeviceFactory wihtout any human interventionvoid 
  void Fed9UXMLDescriptionFromDb::setFed9UDescriptionDatabase( Fed9UDescription, u32 major, u32 minor ) {  
  // retreive the XML buffer from the Fed9UDescription  
  const char *xmlBuffer_ ;  
  // write it  
  //std::string writeString ="BEGIN pkgFedXML.UploadXMLClob(:a, :b, :c);END;";  
  dbFedAccess_->setXMLClob ( xmlBuffer_, major, minor);
  }
  */

  std::vector<Fed9UDescription*>* Fed9UXMLDescriptionFromDb::download(i16 fedId, bool hardwareId, int versionMajor, int versionMinor, std::string partition, int maskMajor, int maskMinor) { 
    return makeNewFed9UDescription(fedId, hardwareId, versionMajor, versionMinor, partition, maskMajor, maskMinor);
  }


  //Method which performs the Fed9UDescription override. 
  //It modifies the object member reference to the Fed9UDescription.
  std::vector<Fed9UDescription*>* Fed9UXMLDescriptionFromDb::makeNewFed9UDescription(i16 fedId, bool hardwareId, int versionMajor, int versionMinor, std::string partition, int maskMajor, int maskMinor) { 
    try {
      //reset document pool
      theDOMBuilder->resetDocumentPool();
      //     debugOutput_=true;
      xercesc::DOMInputSource* domInputSource = NULL;
      if (debugOutput_) {
	std::cout << "About to download buffer!!!" << std::dec << std::endl;
	std::cout << "Parameters are: fedid = " << fedId 
		  << " hardware id bool = " << (hardwareId?"true":"false") 
		  << " versionMajor = " << versionMajor 
		  << " versionMinor = " << versionMinor 
		  << " partition = " << partition << std::endl;
      }
      
      domInputSource = getDatabaseDOMInputSource(fedId, hardwareId,versionMajor,versionMinor,partition, maskMajor, maskMinor);
      
      if (debugOutput_) {
	std::cout << "Downloaded buffer now we look at it!!!" << std::endl;
	//Some debug code to look at the xml buffer
	XMLByte *xmlBuffer1 = NULL;
	xmlBuffer1 = (XMLByte *) calloc(xmlBufferLength+1, sizeof(char));
	xercesc::BinInputStream* bob = domInputSource->makeStream();
	bob->readBytes(xmlBuffer1,1000);//xmlBufferLength);
	xmlBuffer1[242] = ' ';
	std::cout << xmlBuffer1 << std::endl;
      }

      doc = theDOMBuilder->parse(*domInputSource);
      if (debugOutput_) {
	std::cout << "Just finished parsking dom" << std::endl;
      }
      
      // if you don't trust Xerces.. then delete the buffer
      if (domInputSource) delete domInputSource;
    }  
    catch (const xercesc::XMLException& e) {
      std::cout << "Error during parsing: " << theXMLOverideFile << std::endl
	   << "Exception message is: " << std::endl << Fed9UStrX(e.getMessage()) << std::endl;
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML,""));
    }
    catch (const xercesc::DOMException& e) {
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      std::cout << "DOM Error during parsing: " << theXMLOverideFile << std::endl
		<< "DOMException code is: " << e.code << std::endl;
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM,""));
    }
    catch  (oracle::occi::SQLException & e) {
      std::string errorMessage = e.getMessage();
      if ((errorMessage.find("exit called from Java code with status 0",0))
	  != std::string::npos 
	  || (errorMessage.find("Java call terminated byuncaught Java exception",0)) 
	  != std::string::npos) {
	errorMessage = dbFedAccess_->getErrorMessage();
	std::cout << errorMessage << std::endl;
	std::cout << e.what() << std::endl;
      }
      std::cout << "sqlException Thrown during parsing: " << theXMLOverideFile << std::endl;
      
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,""));
    }    
    catch (...) {
      std::cout << "Unexpected exception during parsing: " << theXMLOverideFile << std::endl;
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,""));
    }
    try {    
      if (debugOutput_) {
	std::cout << "about to get elements from dom" << std::endl;
      }
      
      //Traverse DOM tree and get a new Fed9UDescription. Check there were no parsing errors.
      if (errorHandler.getSawErrors()) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_PARSE,
					   "No new Fed9UDescription will be made. Please check the XML against the XML Schema. This was an error handled by the Xerces error handler."));
      }
      else if (doc) {
	countApvs = 0;
	traverseChildElements((xercesc::DOMNode*)doc->getDocumentElement());
	if (debugOutput_) {
	  std::cout << "called getDocumentElement()" << std::endl;
	}
	
	if (numberOfFeds_ > 0) {
	  Fed9UDescription * tmpFedDescriptionPtr = new Fed9UDescription(theFed9UDescription); // create a new copy of the local description
	  std::cout << "fed pointer for freddy! = " << tmpFedDescriptionPtr << std::endl;
	  theFed9UDescriptionList_->push_back(tmpFedDescriptionPtr); // place it in the vector
	  if (debugOutput_) {
	    cout << "we add the last FED to the vector, we now have " << numberOfFeds_ << " feds. the fedID for the last fed is " << (*theFed9UDescriptionList_)[numberOfFeds_-1]->getFedId() << endl; 
	  }
	  ICUTILS_VERIFYX((countApvs==0)||(countApvs==24),Fed9UXMLDescriptionException)(countApvs).code(Fed9UXMLDescriptionException::ERROR_NUM_APV).error().msg("One has to set the medianOverride for either 0 or 24 APVS (ie. a whole FE FPGA.)");
	}
	
	if (numberOfFeds_ == 0)
	  theFed9UDescriptionList_->clear();

	

	if (debugOutput_) {
	  std::cout << "added description to vector, number of Feds_ = " << numberOfFeds_ << "number of descriptions in list = " <<  theFed9UDescriptionList_->size() <<  std::endl;
	}
	
	if (debugOutput_ && numberOfFeds_ > 0) {
	  std::cout << "we add the last FED to the vector, we now have " << numberOfFeds_ << " feds. the fedID for the last fed is " << (*theFed9UDescriptionList_)[numberOfFeds_-1]->getFedId() << std::endl; 
	}
      }
      
      
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught in Fed9UXMLDescription::makeNewFed9UDescriptionFromStream."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught in Fed9UXMLDescription::makeNewFed9UDescriptionFromStream."));
    }
    cout  << "Fed9UDeviceFactory::Address of Pointer: " << hex << theFed9UDescriptionList_ << std::endl;
    return theFed9UDescriptionList_;
  }
}
  
#endif
#endif
#endif

