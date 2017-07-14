/**Matthew Pearson, 2/12/03. */
/**Class which reads in an XML stream and extracts the parameters in order to overide the default                             
   Fed9UDescription. The constructor takes as a parameter a Fed9UDescription. The makeNewFed9UDescriptionFromStream                                   
   method takes as a parameter the XML stream. This contains the XML information (the structure of which is defined by                               
   the Fed9USoftware XML schema file) which is used to set specific parameters in the Fed9UDescription,                                      
   depending on which XML tags and atttributes are in the stream. The stream does not have to contain a description                                   
   of a whole FED, only a subset of it (for example, all the settings for a specified channel). However, if the user                                  
   wishes, a complete XML description of a FED can be used by this class to completely override every parameter in a Fed9UDescription.                
   This class provides the stream interface so that it can be reused by a derived class which will provided an iterface to,                            
   for example, a file or a database.*/

//NOTE: Elements of this code were taken from the Xerces 2.1 library samples.

//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UXMLDescription."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#include <xercesc/util/Base64.hpp>
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UXMLDescription.hh"
#include "Fed9UTempControl.hh"
#include "Fed9UStripDescription.hh"
#include "Fed9UTtcrxDescription.hh"
#include "Fed9ULogTemplate.hh"

//Xerces headers
#include "Fed9UDOMCountErrorHandler.hh"
#include "Fed9UStrX.hh"
#include "Fed9UXStr.hh"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"

#include <iostream>
#include <sstream>
#include <string>

//Database headers
/*#ifdef DATABASE
#include "DbFedAccess.h"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "xercesc/util/BinInputStream.hpp"
#endif*/


namespace Fed9U {

  using xercesc::DOMNode;
  using xercesc::DOMNamedNodeMap;
  using xercesc::DOMDocument;
  using xercesc::DOMLSInput;
  using xercesc::DOMLSParser;
  using xercesc::XMLPlatformUtils;
  using xercesc::XMLUni;
  using xercesc::XMLString;
  using xercesc::XMLException;
  using xercesc::DOMException;
  using xercesc::DOMImplementation;
  using xercesc::DOMImplementationRegistry;
  using xercesc::DOMImplementationLS;
  using xercesc::DOMLSInput;
  using xercesc::InputSource;
  using xercesc::Wrapper4InputSource;
  using xercesc::BinInputStream;
  using xercesc::Base64;

  using std::cout;
  using std::ostringstream;
  using std::endl;
  using std::cerr;
  using std::ios;
  using std::string;
  using std::stringstream;
  
  //Constructor. This takes as arguments the XML file name and the Fed9UDescriptions to be modified.
  Fed9UXMLDescription::Fed9UXMLDescription(const string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist, bool usingStrips) throw (Fed9UXMLDescriptionException) 
    : theXMLOverideFile(XMLOverideFile), 
      theFed9UDescription(defaultFed9UDescription), 
      theFed9UDescriptionList_(desclist),
      numberOfFeds_(0),
      usingStrips_(usingStrips),
      debugOutput_(false)
  {
    try {
      ICUTILS_VERIFYX(desclist != NULL ,Fed9UXMLDescriptionException)(countApvs).code(Fed9UXMLDescriptionException::ERROR_UNKNOWN).error().msg("desclist pointer is null... please make sure you pass a valid description list vector to the contstructor");
      makeDOMBuilder();
    }
    catch (const XMLException& e) {
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception during DOMBuilder init."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception during DOMBuilder init."));
    }
  }


  //Constructor. This takes in the Fed9UDescription to be modified.
  Fed9UXMLDescription::Fed9UXMLDescription(Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist, bool usingStrips) throw (Fed9UXMLDescriptionException) : theFed9UDescription(defaultFed9UDescription), theFed9UDescriptionList_(desclist), 
       numberOfFeds_(0), usingStrips_(usingStrips), debugOutput_(false) {
    try {
      makeDOMBuilder();
    }
    catch (const XMLException& e) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception during DOMBuilder init."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception during DOMBuilder init."));
    }
  }
  
  //Destructor
  Fed9UXMLDescription::~Fed9UXMLDescription()
  {
    try {
      //theDOMBuilder->release();
      //XMLPlatformUtils::Terminate(); //JRF removing this to prevent xerces crashes when other threads are still using it. 
    }
    catch (...) {
      cerr << "Unknown exception caught in Fed9UXMLDescription::~Fed9UXMLDescription(). " << endl;
    }
  }
  
  //Method which performs the Fed9UDescription override. 
  //It modifies the object member reference to the Fed9UDescription.
  std::vector<Fed9UDescription*> * Fed9UXMLDescription::makeNewFed9UDescription(void)  throw (Fed9UXMLDescriptionException) { 
    try {    
      //reset document pool
      cout << "Reset the document Pool" << endl;
      theDOMBuilder->resetDocumentPool();
      
      try {
	cout << " try to parse uri " << endl;
	//Parse the XML input file
	const char * const theCharXMLFile = theXMLOverideFile.c_str();
	cout << "XML File to parse = (" << theXMLOverideFile <<  ")" << endl;
	doc = theDOMBuilder->parseURI(theCharXMLFile);
	cout << " parsed url no errors " << endl;
      }
      catch (const XMLException& e) {
	//NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception
	cout << "XMLException caught here when trying to parse URI" << endl;
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
      }
      catch (const DOMException& e) {
	cout << "DOMException caught here when trying to parse URI" << endl;
	ostringstream code;
	code << e.code;
	string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
	//NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
      }
      catch (std::exception &e) {
	cout << "exception caught here when trying to parse URI" << endl;
	RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught during parse."));
      }
      catch (...) {
	cout << "unknown exception caught here when trying to parse URI" << endl;
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught during parse."));
      }
      
      //Traverse DOM tree and get a new Fed9UDescription. Check there were no parsing errors caught bu Xerces.
      if (errorHandler.getSawErrors()) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_PARSE,
					   "No new Fed9UDescription will be made. Please check the XML against the XML Schema. This was an error handled by the Xerces error handler."));
      }
      else if (doc) {
	countApvs = 0;
	traverseChildElements((DOMNode*)doc->getDocumentElement());
	// once we have been through the whole document we can then copy the last description into the vector if we have more than one fed
	// we store the last fed description in the vector to complete the set
	cout << "Try to push back description into vector" << endl;
	Fed9UDescription * tmpFedDescriptionPtr = new Fed9UDescription(theFed9UDescription); // create a new copy of the local description
	cout << "constructed new description copy!!! description list pointer = " << std::hex <<theFed9UDescriptionList_ << std::dec <<  endl;
	theFed9UDescriptionList_->push_back(tmpFedDescriptionPtr); // place it in the vector
	cout << "pushed back description into vector" << endl;	
	ICUTILS_VERIFYX((countApvs==0)||(countApvs==24),Fed9UXMLDescriptionException)(countApvs).code(Fed9UXMLDescriptionException::ERROR_NUM_APV).error().msg("One has to set the medianOverride for either 0 or 24 APVS (ie. a whole FE FPGA.)");
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught in Fed9UXMLDescription::makeNewFed9UDescription"));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught in Fed9UXMLDescription::makeNewFed9UDescription."));
    }
    return theFed9UDescriptionList_;
  }
  
  
  //Method which performs the Fed9UDescription override. 
  //It takes in a istream, containing the XML buffer, and modifies the object member reference to the Fed9UDescription.
  std::vector<Fed9UDescription*>* Fed9UXMLDescription::makeNewFed9UDescriptionFromStream(std::istream &is) throw (Fed9UXMLDescriptionException) {

    try {    
      //reset document pool
      theDOMBuilder->resetDocumentPool();

      //Parse the XML input stream
      DOMLSInput* domInputSource = NULL;
      // XMLBuffer contains the XML data and convert it to an input source 
      //Find length of istream
      is.seekg(0, ios::end);
      int length = is.tellg();
      is.seekg(0, ios::beg);
      //Convert to character buffer (is there a more efficient way to do this!)
      char *theCharBuf = new char[length];
      is.read(theCharBuf, length);
      //Make a MemBufInputSource
      string xmlBufferId = "xmlBuffer";
      XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = new XERCES_CPP_NAMESPACE::MemBufInputSource((const XMLByte*)theCharBuf, length, X(xmlBufferId.c_str()), false, XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);      
      //Convert the input source to a DOMInputSource 
      domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource);
      //Now parse to make the DOM
      try {
	doc = theDOMBuilder->parse(domInputSource);
      }
      catch (const XMLException& e) {
        //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception                       
        THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
      }
      catch (const DOMException& e) {
        ostringstream code;
        code << e.code;
        string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
        //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                                          
        THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
      }
      catch (std::exception &e) {
        RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught during parse."));
      }
      catch (...) {
        THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught during parse."));
      }

      //Traverse DOM tree and get a new Fed9UDescription. Check there were no parsing errors.
      if (errorHandler.getSawErrors()) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_PARSE, "No new Fed9UDescription will be made. Please check the XML against the XML Schema. This was an error handled by the Xerces error handler."));
      }
      else if (doc) {
	countApvs = 0;
	traverseChildElements((DOMNode*)doc->getDocumentElement());
	// once we have been through the whole document we can then copy the last description into the vector
	// we store the last fed description in the vector to complete the set, but only if there are more than 0 feds!!!
	if (numberOfFeds_ > 0) {
	  Fed9UDescription * tmpFedDescriptionPtr = new Fed9UDescription(theFed9UDescription); // create a new copy of the local description
	  theFed9UDescriptionList_->push_back(tmpFedDescriptionPtr); // place it in the vector
	  cout << "we add the last FED to the vector, we now have " << numberOfFeds_ << " feds. the fedID for the last fed is " << (*theFed9UDescriptionList_)[numberOfFeds_-1]->getFedId() << endl; 
	
	ICUTILS_VERIFYX((countApvs==0)||(countApvs==24),Fed9UXMLDescriptionException)(countApvs).code(Fed9UXMLDescriptionException::ERROR_NUM_APV).error().msg("One has to set the medianOverride for either 0 or 24 APVS (ie. a whole FE FPGA.)");
	}
      }
      if (domInputSource)
	delete domInputSource;
      //      if (xmlInputSource) 
      //	delete xmlInputSource;
      if (theCharBuf)
	delete theCharBuf;
 
      
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught in Fed9UXMLDescription::makeNewFed9UDescriptionFromStream."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught in Fed9UXMLDescription::makeNewFed9UDescriptionFromStream."));
    }
    
    return theFed9UDescriptionList_;

  }
  
  
  //****************************************************************************************************
  //PRIVATE METHODS
  //****************************************************************************************************
  
  //Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.
  void Fed9UXMLDescription::initializeXerces(void) throw (Fed9UXMLDescriptionException)
  {
    try {
	//if (XMLPlatformUtils::fgTransService == NULL)
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& e) {
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception                                                                 
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception caught during XMLPlatformUtils::Initialize()."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unexpected exception caught during XMLPlatformUtils::Initialize()."));
    }
  }

  //*******************************************************************
  //Method which will construct and initialize the DOMBuilder.
  void Fed9UXMLDescription::makeDOMBuilder(void) throw (Fed9UXMLDescriptionException) {
    try {
      //Initialize the Xerces APIs
      initializeXerces();
      
      XMLCh tempStr[100];
      XMLString::transcode("LS", tempStr, 99);
      DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
      //DOMBuilder* theDOMBuilder = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
      theDOMBuilder = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
      
      //Set the next 3 options to false if one does not want to use automatic XML Schema verifying.
      theDOMBuilder->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
      theDOMBuilder->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
      theDOMBuilder->getDomConfig()->setParameter(XMLUni::fgXercesSchemaFullChecking, true);
      
      /*Do we want to enable validation error reporting?
	fgDOMValidateIfSchema will enable it if a schema is specified.
	fgDOMValidation will disable/disable it.
	Note: The schema file must have the same name as the XML file, but with a .xsd extension. */
      theDOMBuilder->getDomConfig()->setParameter(XMLUni::fgDOMValidateIfSchema, true);   
      //theDOMBuilder->setFeature(XMLUni::fgDOMValidation, false);
      //theDOMBuilder->setFeature(XMLUni::fgDOMValidation, true);
      
      //And create our error handler and install it

      theDOMBuilder->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler,&errorHandler);
      
      //reset error count first
      errorHandler.resetErrors();
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }
  
  //*****************************************************************
  //Method to recursively find the child elements under the given Node.
  //This is based on code from the Xerces 2.3 library samples.
  void Fed9UXMLDescription::traverseChildElements(DOMNode *n) throw (Fed9UXMLDescriptionException)
  {
    char * name=NULL;
    try {
      DOMNode *child;
      
      if (n) {
	//Send the node information to the Fed9UDescription
	setFed9UDescription(n);
	//Find the next node. 
	
	// if we are instructed not to use Strips in the input, then we ignore all child elements of each APV node ( these are the strips )
	name = XMLString::transcode(n->getNodeName());
	if ( string(name ) == "apv" && !usingStrips_) { 
	  XMLString::release(&name);
	  return;
	}

	
	// if we get here then we must be either using strips and traversing strips or traversing another parent node other than strips
	for (child = n->getFirstChild(); child != 0; child=child->getNextSibling()) {
	  traverseChildElements(child);
	}
      }
    }
    catch (std::exception &e) {
      if (name != NULL)
	XMLString::release(&name);
	  
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      if (name != NULL)
	XMLString::release(&name);
	  
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
    if (name != NULL)
      XMLString::release(&name);
  }
  
  //**********************************************************************************************
  //Method which reads in a node, finds if it has attributes and sets these in the Fed9UDescription.
  void Fed9UXMLDescription::setFed9UDescription(DOMNode *n) throw (Fed9UXMLDescriptionException)
  {
    try {
      //      cout << " we write the node into the description" << endl;
      //Does this node have attributes? If so, then get them!
      if (n->hasAttributes()) {
	DOMNamedNodeMap *theAttributesDOMNamedNodeMap = 0;
	theAttributesDOMNamedNodeMap = n->getAttributes();
	
	//Get the node name, and convert to string format.
	char *theNodeName = XMLString::transcode(n->getNodeName());  
	string theStringNodeName(theNodeName);
	if (theStringNodeName=="fed") {
	  // each time we find a fed tag we must increment the number of fed descriptions.
	  // if we only have one feddescription it will stay in the member variable theFedDescription
	  // if we have more we will push them into a vector which will be returned.
	  numberOfFeds_ += 1;
	  if (numberOfFeds_ > 1) {
	    // if we are working on more than one fed we store the previous fed description and then overwrite the local description
	    Fed9UDescription * tmpFedDescriptionPtr = new Fed9UDescription(theFed9UDescription); // create a new copy of the local description
	    theFed9UDescriptionList_->push_back(tmpFedDescriptionPtr); // place it in the vector
	    cout << "we add another FED to the vector, we now have " << (numberOfFeds_-1) << " feds. the fedID for the last fed is " << (*theFed9UDescriptionList_)[numberOfFeds_-2]->getFedId() << endl; 
	  }
	  setFed9UAttributes(theAttributesDOMNamedNodeMap);
	}
	else if (theStringNodeName=="feFpga") {
	  setFeFpgaAttributes(theAttributesDOMNamedNodeMap);
	}
 	else if (theStringNodeName=="channelPair") {
	  setChannelPairAttributes(theAttributesDOMNamedNodeMap);
	}
	else if (theStringNodeName=="channel") {
	  setChannelAttributes(theAttributesDOMNamedNodeMap);
	}
	else if (theStringNodeName=="apv") {
	  setApvAttributes(theAttributesDOMNamedNodeMap);
	}
	else if (theStringNodeName=="strip") {
	  setStripAttributes(theAttributesDOMNamedNodeMap);
	}
	else if (theStringNodeName=="strips") {
	  setStripsAttribute(theAttributesDOMNamedNodeMap);
	}
	XMLString::release(&theNodeName); //Clean up from the XMLString::transcode
      }
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }
  
  //*******************************************************
  //Method which will set the attributes for the fed9U node.
  void Fed9UXMLDescription::setFed9UAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  { 
    try {
    //There are four possible type of node value, integer (signed and unsigned), float and string.
    unsigned long theIntNodeValue = 0;
    int theSignedIntNodeValue = 0;
    float theFloatNodeValue = 0.0;
    string theStringNodeValue;
    //This is to flag nodes which are not found, and which should be found.
    bool notFound = false; 
    
    //Get the fed ID
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
    if (notFound) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: fed id."));
    }
    else {
      theFed9UDescription.setFedId(theIntNodeValue);
    }
  
    //Get the fed Hardware ID
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "hardwareId", notFound);
    if (notFound) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: fed hardware id."));
    }
    else {
      theFed9UDescription.setFedHardwareId(theIntNodeValue);
    }
    
    //Get the description name
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "name", notFound);
    if (!notFound) {
      theFed9UDescription.setName(theStringNodeValue);
    }

    //Get the fake event file name
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "fakeEventFile", notFound);
    if (!notFound) {
      theFed9UDescription.setFakeEventFile(theStringNodeValue);
    }

    //Get the fake event trigger delay
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fakeEventTriggerDelay", notFound);
    if (notFound) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: fakeEventTriggerDelay."));
    }
    else {
      theFed9UDescription.setFakeEventTriggerDelay(theIntNodeValue);
    }
 
    //Get the feFirmwareVersion 
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "feFirmwareVersion", notFound);
    theIntNodeValue =  strtol (theStringNodeValue.c_str(), NULL, 16);
    if (!notFound) {
      std::cout << "in string stream! " << std::hex << theIntNodeValue << std::dec << std::endl;
      theFed9UDescription.setFeFirmwareVersion(theIntNodeValue);
    }
    //Get the beFirmwareVersion 
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "beFirmwareVersion", notFound);
    theIntNodeValue =  strtol (theStringNodeValue.c_str(), NULL, 16);
    if (!notFound) {
      theFed9UDescription.setBeFirmwareVersion(theIntNodeValue);
    }
    //Get the vmeFirmwareVersion 
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "vmeFirmwareVersion", notFound);
    theIntNodeValue =  strtol (theStringNodeValue.c_str(), NULL, 16);
    if (!notFound) {
      theFed9UDescription.setVmeFirmwareVersion(theIntNodeValue);
    }

    //Get the delayFirmwareVersion 
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "delayFirmwareVersion", notFound);
    theIntNodeValue =  strtol (theStringNodeValue.c_str(), NULL, 16);
    if (!notFound) {
      theFed9UDescription.setDelayFirmwareVersion(theIntNodeValue);
    }

    //Get the fedVersion 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fedVersion", notFound);
    if (!notFound) {
      theFed9UDescription.setFedVersion(theIntNodeValue);
    }
 
    //Get the epromVersion 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "epromVersion", notFound);
    if (!notFound) {
      theFed9UDescription.setEpromVersion(theIntNodeValue);
    }
     
    //Get the OptoRx Resistor Value 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "optoRxResistor", notFound);
    if (!notFound) {
      theFed9UDescription.setOptoRXResistor(theIntNodeValue);
    }  

    //Get the crate slot number
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "crateSlot", notFound);
    if (!notFound) {
      //Multiply by 10000 to make it a base address - we can talk about this later, don't think i'll forget...
      theIntNodeValue <<= 16;
      theIntNodeValue &= 0x1f0000;
      theFed9UDescription.setBaseAddress(theIntNodeValue);
    }
    
    //Get the crate number 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "crate", notFound);
    if (!notFound) {
      theFed9UDescription.setCrateNumber(theIntNodeValue);
    }

    //Get the VME controller Daisy chain number ( crate number )
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "vmeControllerDaisyChainId", notFound);
    if (!notFound) {
      theFed9UDescription.setVmeControllerDaisyChainId(theIntNodeValue);
    }
    
    //Get the enable/disable attribute
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "state", notFound);
    if (!notFound) {
      if (theStringNodeValue=="ENABLE") {
	theFed9UDescription.setFedBeFpgaDisable(0);
      }
    else if (theStringNodeValue=="DISABLE") {
      theFed9UDescription.setFedBeFpgaDisable(1);
    }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed state"));
      }
    }
    
    //Get the fed mode
    notFound = false;
    bool inScopeMode = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "mode", notFound);
    cout << "******************** mode string node value = " << theStringNodeValue << endl;
    if (!notFound) {
      if (theStringNodeValue=="ZERO_SUPPRESSION") {
	theFed9UDescription.setDaqMode(FED9U_MODE_ZERO_SUPPRESSED);
      }
    else if (theStringNodeValue=="PROCESSED_RAW") {
      theFed9UDescription.setDaqMode(FED9U_MODE_PROCESSED_RAW);
    }
    else if (theStringNodeValue=="VIRGIN_RAW") {
	  theFed9UDescription.setDaqMode(FED9U_MODE_VIRGIN_RAW);
    }
    else if (theStringNodeValue=="SCOPE") {
	   theFed9UDescription.setDaqMode(FED9U_MODE_SCOPE);
	  inScopeMode = true; //This is used to test if the scopeLength attribute is set.
    }
    else if (theStringNodeValue=="NONE") {
         theFed9UDescription.setDaqMode(FED9U_MODE_NONE);
     }
                          
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed mode")); 
      }
    }

    //Get the fed super mode
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "superMode", notFound);
    cout << "******************** super mode string node value = " << theStringNodeValue << endl;
    if (!notFound) {
      if (theStringNodeValue=="FAKE") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE);
      }
      else if (theStringNodeValue=="ZERO_LITE") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_ZERO_LITE);
      }
      else if (theStringNodeValue=="FAKE_ZERO_LITE") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_ZERO_LITE);
      }
      else if (theStringNodeValue=="NORMAL") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_NORMAL);
      }
	  else if (theStringNodeValue=="FAKE_HI_LO") {
		 theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_HI_LO);
	  }
	  else if (theStringNodeValue=="ZERO_LITE_HI_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_ZERO_LITE_HI_LO);
	  }
	  else if (theStringNodeValue=="FAKE_ZERO_LITE_HI_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_ZERO_LITE_HI_LO);
	  }
	  else if (theStringNodeValue=="NORMAL_HI_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_NORMAL_HI_LO);
	  }
	  else if (theStringNodeValue=="FAKE_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_LO);
	  }
	  else if (theStringNodeValue=="ZERO_LITE_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_ZERO_LITE_LO);
	  }
	  else if (theStringNodeValue=="FAKE_ZERO_LITE_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_ZERO_LITE_LO);
	  }
	  else if (theStringNodeValue=="NORMAL_LO") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_NORMAL_LO);
	  }
	  else if (theStringNodeValue=="FAKE_10") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_FAKE_10);
	  }
	  else if (theStringNodeValue=="NORMAL_10") {
		  theFed9UDescription.setDaqSuperMode(FED9U_SUPER_MODE_NORMAL_10);
	  }
		
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed superMode")); 
      }
    }


    //Get the scope length if we are in scope mode. If we are in scope mode and scopelength is not found then throw an error.
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "scopeLength", notFound);
    if (!notFound) {
      theFed9UDescription.setScopeLength(theIntNodeValue);
    } else if ((notFound)&&(inScopeMode)) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,
					 "The mode attribute was set to SCOPE but the scopeLength attribute has not been found."));
    }

    //Get the fed clock source
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "clockSource", notFound);
    if (!notFound) {
      if (theStringNodeValue=="TTC") {
	theFed9UDescription.setClock(FED9U_CLOCK_TTC);
      }
      else if (theStringNodeValue=="BACKPLANE") {
	theFed9UDescription.setClock(FED9U_CLOCK_BACKPLANE);
      }
      else if (theStringNodeValue=="INTERNAL") {
	theFed9UDescription.setClock(FED9U_CLOCK_INTERNAL);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed clockSource")); 
      }
    }
    
    //Get the fed trigger source
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "triggerSource", notFound);
    if (!notFound) {
      if (theStringNodeValue=="TTC") {
	theFed9UDescription.setTriggerSource(FED9U_TRIG_TTC);
      }
      else if (theStringNodeValue=="BACKPLANE") {
	theFed9UDescription.setTriggerSource(FED9U_TRIG_BACKPLANE);
      }
      else if (theStringNodeValue=="SOFTWARE") {
	theFed9UDescription.setTriggerSource(FED9U_TRIG_SOFTWARE);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed triggerSource"));
      }
    }
  
    //Get the fed read route
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "readRoute", notFound);
    if (!notFound) {
      if (theStringNodeValue=="SLINK64") {
	theFed9UDescription.setBeFpgaReadRoute(FED9U_ROUTE_SLINK64);
      }
      else if (theStringNodeValue=="VME") {
	theFed9UDescription.setBeFpgaReadRoute(FED9U_ROUTE_VME);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed readRoute"));
      }
    }

    //Get the Hal Bus Adaptor
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "halBusAdaptor", notFound);
    if (!notFound) {
      if (theStringNodeValue=="SBS") {
	theFed9UDescription.setBusAdaptorType(FED9U_HAL_BUS_ADAPTOR_SBS);
      }
      else if (theStringNodeValue=="CAEN_PCI") {
	theFed9UDescription.setBusAdaptorType(FED9U_HAL_BUS_ADAPTOR_CAEN_PCI);
      }
      else if (theStringNodeValue=="CAEN_USB") {
	theFed9UDescription.setBusAdaptorType(FED9U_HAL_BUS_ADAPTOR_CAEN_USB);
      }
      else if (theStringNodeValue=="VXI") {
	theFed9UDescription.setBusAdaptorType(FED9U_HAL_BUS_ADAPTOR_VXI);
      }
      else if (theStringNodeValue=="DUMMY") {
	theFed9UDescription.setBusAdaptorType(FED9U_HAL_BUS_ADAPTOR_DUMMY);
      }
     else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: hal bus adaptor type"));
      }
    }
  
    //Get the test register for the BE FPGA.
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "testRegister", notFound);
    if (!notFound) {
      theFed9UDescription.setTestRegister(theIntNodeValue);
    }

    //Set the max temp for the LM82 chip for the BE FPGA
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempLm82BeFpga", notFound);
    if (!notFound) {
      theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND); 
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the LM82 high.
      theFed9UTempControl.setLm82High(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Set the max temp for the BE FPGA
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempFpgaBeFpga", notFound);
    if (!notFound) {
      theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND); 
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the FPGA temp.
      theFed9UTempControl.setFpgaHigh(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Set the max critical temp for the BE FPGA.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempCriticalBeFpga", notFound);
    if (!notFound) {
      theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND); 
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the critical temp.
      theFed9UTempControl.setCritical(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Set the TTCrx L1AcceptCoarseDelay parameter.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxL1AcceptCoarseDelay", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the coarse delay
      theFed9UTtcrxDescription.setL1AcceptCoarseDelay(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    } 

    //Set the TTCrx L1AcceptFineDelay parameter 
    //(in other words, set the Clock40DeskewOneFineDelay parameter - see implmentation details in Fed9UTtcrxDescription). 
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxL1AcceptFineDelay", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the fine delay
      theFed9UTtcrxDescription.setClockDesOneFineDelay(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx BroadcastStrobeTwoCoarseDelay parameter. Note: the setting of this parameter is tested for in the setting of the corresponding fine delay below.
    notFound=false;
    bool setBrcstStrTwoCoarseDelay = false;
    u16 theBrcstStrTwoCoarseDelay = 0;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxBrcstStrTwoCoarseDelay", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setBrcstStrTwoCoarseDelay(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
      setBrcstStrTwoCoarseDelay = true;
      theBrcstStrTwoCoarseDelay = theIntNodeValue;
    }

    //NOTE: It is important that the following parameter is set before the next one.  
    //Set the TTCrx Clock40DeskewedTwo operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxClock40DeskwedTwo", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setDeskewedClock2Selected(true);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setDeskewedClock2Selected(false);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxClock40DeskwedTwo"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }


    //Set the TTCrx BroadcastStrobeTwoFineDelay parameter. The code in Fed9UTtcrxDescription deals with this in a special way, and so I use the Fed9UTtcrxDescription::setBrcstStrTwoDelay(u16 coarseDelay, u16 fineDelay) method to set this fine delay. I therefore enforce the user of the XML file to set the BroadcastStrobeTwoCoarseDelay parameter also.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxBrcstStrTwoFineDelay", notFound);
    if (!notFound) {
      if (!setBrcstStrTwoCoarseDelay) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"You have tried to set the fine BroadcastStrobeTwo delay, without setting the coarse delay. Please set the ttcrxBrcstStrTwoCoarseDelay attribute and this error will go away."));
      }
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setBrcstStrTwoDelay(theBrcstStrTwoCoarseDelay,theIntNodeValue);
      cout << "WARNING: if you set ttcrxBrcstStrTwoFineDelay, then this will overwrite the ttcrxL1AcceptFineDelay if the ttcrxClock40DeskwedTwo is set to DISABLE." << endl;
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx DLL pump current parameter.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxDLLPumpCurrent", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setDllPumpCurrent(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx PLL pump current parameter.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxPLLPumpCurrent", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setPllPumpCurrent(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx Individually addressed command ID.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxIacId", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setIacId(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx I2C ID.
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ttcrxI2cId", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      theFed9UTtcrxDescription.setI2cId(theIntNodeValue);
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx Hamming Error Checking flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxHammingChecking", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setHammingCheckingDisable(false); //Enable Hamming checking.
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setHammingCheckingDisable(true);  //Disable Hamming checking.
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed ttcrxHammingChecking"));
      }
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx Bunch Counter operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxBunchCounter", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now read back the existing bits, so we do not overwrite the one we are not setting here.
      int theCounterBits = theFed9UTtcrxDescription.getCounterOperation();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setCounterOperation(FED9U_TTCRX_ENABLE_BUNCH_CTR | theCounterBits);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setCounterOperation(~FED9U_TTCRX_ENABLE_BUNCH_CTR & theCounterBits);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxBunchCounter"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx Event Counter operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxEventCounter", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now read back the existing bits, so we do not overwrite the one we are not setting here.
      int theCounterBits = theFed9UTtcrxDescription.getCounterOperation();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setCounterOperation(FED9U_TTCRX_ENABLE_EVENT_CTR | theCounterBits);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setCounterOperation(~FED9U_TTCRX_ENABLE_EVENT_CTR & theCounterBits);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxEventCounter"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx Clock40DeskewedTwoOutput operation flag. This ia a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxClock40DeskwedTwoOutput", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setDeskewedClock2Disable(false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setDeskewedClock2Disable(true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxClock40DeskwedTwoOutput"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }
  

    //Set the TTCrx L1 clock accept output operation flag. This ia a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxClockL1AcceptOutput", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setClockL1AcceptDisable(false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setClockL1AcceptDisable(true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxClockL1AcceptOutput"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx parallel output bus operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxParallelOutputBus", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setParrallelOutputDisable(false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setParrallelOutputDisable(true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxParallelOutputBus"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx serial output operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxSerialBOutput", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setSerialBDisable(false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setSerialBDisable(true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxSerialBOutput"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //Set the TTCrx NonDeskwedClock40Output operation flag. This is a control register flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "ttcrxNonDeskwedClock40Output", notFound);
    if (!notFound) {
      Fed9UTtcrxDescription theFed9UTtcrxDescription;
      //Read the old values in the description.
      theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();
      //Now set the parameter.
      if (theStringNodeValue=="ENABLE") {
	theFed9UTtcrxDescription.setNonDeskewedClockDisable(false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UTtcrxDescription.setNonDeskewedClockDisable(true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed ttcrxNonDeskwedClock40Output"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setTtcrx(theFed9UTtcrxDescription);
    }

    //VOLTAGE MONITOR

    //Get the voltage monitor stand by flag
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "volMonStandBy", notFound);
    if (!notFound) {
      //Initialize a Fed9UVoltageControl object from the Fed9UDescription.
      //Read in the existing parameters for the voltage monitor.
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter
      if (theStringNodeValue=="ENABLE") {
	theFed9UVoltageControl.setStandBy(true);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UVoltageControl.setStandBy(false);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed volMonStandBy"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor resetStatusRegister flag.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "volMonStatusRegisterReset", notFound);
    if (!notFound) {
      //Initialize a Fed9UVoltageControl object from the Fed9UDescription.
      //Read in the existing parameters for the voltage monitor.
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter
      if (theStringNodeValue=="ENABLE") {
	theFed9UVoltageControl.setResetStatusRegister(true);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UVoltageControl.setResetStatusRegister(false);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed volMonStatusRegisterReset"));
      }
      //Finally set the parameter in the description.
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min2.5V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMin2point5V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.set2Point5VoltMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max2.5V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMax2point5V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.get2Point5VoltMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMax2point5V attribute smaller than volMonMin2point5V.");
      //Now set the parameter.
      theFed9UVoltageControl.set2Point5VoltMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }
  
    //Get the voltage monitor min3.3V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMin3point3V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.set3Point3VoltMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max3.3V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMax3point3V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.get3Point3VoltMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMax3point3V attribute smaller than volMonMin3point3V.");
      //Now set the parameter.
      theFed9UVoltageControl.set3Point3VoltMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min5V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMin5V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.set5VoltMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max5V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMax5V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.get5VoltMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMax5V attribute smaller than volMonMin5V.");
      //Now set the parameter.
      theFed9UVoltageControl.set5VoltMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min12V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMin12V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.set12VoltMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max12V parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMax12V", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.get12VoltMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMax12V attribute smaller than volMonMin12V.");
      //Now set the parameter.
      theFed9UVoltageControl.set12VoltMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min core voltage parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMinCoreV", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.setCoreVoltageMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max core voltage parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMaxCoreV", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.getCoreVoltageMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMaxCoreV attribute smaller than volMonMinCoreV.");
      //Now set the parameter.
      theFed9UVoltageControl.setCoreVoltageMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min supply voltage parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMinSupplyV", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.setSupplyVoltageMin(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max supply voltage parameter.
    notFound=false;
    theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "volMonMaxSupplyV", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max voltage does not exceed minimum.
      float minFloatValue = theFed9UVoltageControl.getSupplyVoltageMin();
      ICUTILS_VERIFYX(minFloatValue<=theFloatNodeValue, Fed9UXMLDescriptionException)(minFloatValue)(theFloatNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMaxSupplyV attribute smaller than volMonMinSupplyV.");
      //Now set the parameter.
      theFed9UVoltageControl.setSupplyVoltageMax(theFloatNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min external temp parameter.
    notFound=false;
    theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonMinExternalTemp", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.setExternalTempMin(theSignedIntNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max external temp parameter.
    notFound=false;
    theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonMaxExternalTemp", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max temperature does not exceed minimum.
      int minIntValue = theFed9UVoltageControl.getExternalTempMin();
      ICUTILS_VERIFYX(minIntValue<=theSignedIntNodeValue, Fed9UXMLDescriptionException)(minIntValue)(theSignedIntNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMaxExternalTemp attribute smaller than volMonMinExternalTemp.");
      //Now set the parameter.
      theFed9UVoltageControl.setExternalTempMax(theSignedIntNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor min internal temp parameter.
    notFound=false;
    theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonMinInternalTemp", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Now set the parameter.
      theFed9UVoltageControl.setInternalTempMin(theSignedIntNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor max internal temp parameter.
    notFound=false;
    theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonMaxInternalTemp", notFound);
    if (!notFound) {
      Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
      //Check that max temperature does not exceed minimum.
      int minIntValue = theFed9UVoltageControl.getInternalTempMin();
      ICUTILS_VERIFYX(minIntValue<=theSignedIntNodeValue, Fed9UXMLDescriptionException)(minIntValue)(theSignedIntNodeValue).code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).error().msg("You have tried to set the volMonMaxInternalTemp attribute smaller than volMonMinInternalTemp.");
      //Now set the parameter.
      theFed9UVoltageControl.setInternalTempMax(theSignedIntNodeValue);
      theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
    }

    //Get the voltage monitor offset temp parameter. However, first read the offset temp select parameter,
    //which selects between internal and external. If this is not set in the XML then the default value 
    //in the description will be used.
    notFound=false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "volMonWhichOffsetTemp", notFound);
    if (!notFound) {
      bool notFound2 = false;
      theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonOffsetTemp", notFound2);
      if (!notFound2) {
	//If both attributes are found, then get the voltage monitor description.
	Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
	if (theStringNodeValue=="INTERNAL") {
	  //Now set the parameter.
	  theFed9UVoltageControl.setTempOffset(theSignedIntNodeValue, true);
	}
	else if (theStringNodeValue=="EXTERNAL") {
	  //Now set the parameter.
	  theFed9UVoltageControl.setTempOffset(theSignedIntNodeValue, false);
	}
	else {
	  THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE, "The attribute is: fed volMonWhichOffsetTemp"));
	}
	//Finally both parameters in the description.
	theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
      }
      else {
	cout << "WARNING: You have tried to set volMonWhichOffsetTemp without setting volMonOffsetTemp. You cannot do this, and no action will be taken. You have to either set both attributes, or just volMonOffsetTemp alone." << endl;
      }
    }
    //So if the volMonWhichOffsetTemp attribute is not found, try to set the volMonOffsetTemp alone.
    else {
      bool notFound2 = false;
      theSignedIntNodeValue = getSignedIntNodeValue(theAttributesDOMNamedNodeMap, "volMonOffsetTemp", notFound2);
      if (!notFound2) {
	//So if just the volMonOffsetTemp parameter is found, use the value for 
	//volMonWhichOffsetTemp which is already in the description.
	Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
	//Read existing value for volMonWhichOffsetTemp
	bool volMonWhichOffsetTemp = theFed9UVoltageControl.getOffsetTempSelect();
	//Now set the parameter
	theFed9UVoltageControl.setTempOffset(theSignedIntNodeValue, volMonWhichOffsetTemp);
	theFed9UDescription.setVoltageMonitor(theFed9UVoltageControl);
      }
    }


    //EPROM
    // this has been removed and replaced with fedHardwareId
    //Get the eprom serial number
    //notFound = false;
    //theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "epromSerialNumber", notFound);
    //if (!notFound) {
    //  Fed9UEpromDescription theFed9UEpromDescription = theFed9UDescription.getEprom();
    //  theFed9UEpromDescription.setFedSerialNumber(theIntNodeValue);
    //  theFed9UDescription.setEprom(theFed9UEpromDescription);
    // }


    //Get the global coarse clock delay
    /*  notFound = false;
	theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "globalClockCoarseShift", notFound);
	if (!notFound) {
	theFed9UDescription.setGlobalCoarseSkew(theIntNodeValue);
	}*/

    //Get the global fine clock delay
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "globalClockFineShift", notFound);
    if (!notFound) {
      theFed9UDescription.setGlobalFineSkew(theIntNodeValue);
    }
    
    //**********************************************************************
    // (8/12/05) JF Added eventType to description
    
    
    //Get the event type
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "eventType", notFound);
    if (!notFound) {
      theFed9UDescription.setDaqEventType(theIntNodeValue);
    }
    
    //**********************************************************************

    //**********************************************************************
    // (8/12/05) JEC Added fov to description
    
    //Get the FOV
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fov", notFound);
    if (!notFound) {
      theFed9UDescription.setDaqFov(theIntNodeValue);
    }
    
    //**********************************************************************

    //**********************************************************************
    // (10/1/06) JEC Added Tracker Header Format Type to description
    
    //Get the header format mode
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "headerType", notFound);
    if (!notFound) {
      if (theStringNodeValue=="FULLDEBUG") {
	theFed9UDescription.setHeaderFormatType(FED9U_HEADER_FULLDEBUG);
      }
      else if (theStringNodeValue=="APVERROR") {
	theFed9UDescription.setHeaderFormatType(FED9U_HEADER_APVERROR);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fed headertype"));
      }
    }
    
    //**********************************************************************

    //**********************************************************************
    // (23/2/06) JEC Added Bunch Crossing Offset to description

    // get the bunch crossing offset
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "bxOffset", notFound);
    if (!notFound) {
      theFed9UDescription.setBunchCrossingOffset(theIntNodeValue);
    }

    //**********************************************************************


    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
    
  }

  //********************************************************
  //Method which will set the attributes for the feFpga node.
  void Fed9UXMLDescription::setFeFpgaAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    try {
    //There are two possible type of node value, integer and string.
    unsigned long theIntNodeValue = 0;
    string theStringNodeValue;

    //This is to flag nodes which are not found, and which should be found.
    bool notFound = false; 

    //Get the feFpga ID
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
    if (notFound) {   
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: feFpga id."));
    }
    else {
      theFed9UAddress.setExternalFedFeUnit(theIntNodeValue); //Set FE unit in theFed9UAddress. using the external interface since this is a user interface and numbering can vary depending on the user setup
    }

    //Set the max temp for the LM82 chip
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempLm82", notFound);
    if (!notFound) {
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the LM82 temp.
      theFed9UTempControl.setLm82High(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Set the max temp for the FPGA
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempFpga", notFound);
    if (!notFound) {
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the FPGA temp.
      theFed9UTempControl.setFpgaHigh(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Set the max critical temp 
    notFound=false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "maxTempCritical", notFound);
    if (!notFound) {
      //Read the old temperature values from the Fed9UDescription so that we don't overwrite the previous temperatures.
      Fed9UTempControl theFed9UTempControl;
      theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
      //Now set the critical temp.
      theFed9UTempControl.setCritical(theIntNodeValue);
      theFed9UDescription.setTempControl(theFed9UAddress, theFed9UTempControl);
    }

    //Get the enable/disable state
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "state", notFound);
    if (!notFound) {
      if (theStringNodeValue=="ENABLE") {
	theFed9UDescription.setFedFeUnitDisable(theFed9UAddress, 0);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UDescription.setFedFeUnitDisable(theFed9UAddress, 1);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: fe state"));
      }
    }

    //Get the OptoRx input offset
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "optoRxInputOffset", notFound);
    if (!notFound) {
      theFed9UDescription.setOptoRxInputOffset(theFed9UAddress, theIntNodeValue);
    }

    //Get the OptoRx output offset
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "optoRxOutputOffset", notFound);
    if (!notFound) {
      theFed9UDescription.setOptoRxOutputOffset(theFed9UAddress, theIntNodeValue);
    }

    //Get the OptoRx capacitor value.
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "optoRxCapacitor", notFound);
    if (!notFound) {
      theFed9UDescription.setOptoRxCapacitor(theFed9UAddress, theIntNodeValue);
    }

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }



  //*********************************************************
  //Method which will set the attributes for the channel pair node.
  void Fed9UXMLDescription::setChannelPairAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    try {
      //There is one possible sort of node value u16 only
      unsigned long theIntNodeValue = 0;
      
      //This is to flag nodes which are not found, and which should be found.
      bool notFound = false; 
      
      //Get the channelPair ID
      theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
      if (notFound) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: channel Pair id."));
      }
      else {
	//Set the channel using the mapping in Fed9UChannelInterface.
	theFed9UAddress.setExternalFeUnitChannelPair(theIntNodeValue);
      }
      
      //Get the fakeEventRandomSeed value
      theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fakeEventRandomSeed", notFound);
      if (notFound) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: fakeEventRandomSeed for channel pair."));
      }
      else {
	//Set the value
	theFed9UDescription.setFakeEventRandomSeed(theFed9UAddress,theIntNodeValue);
      }
      
      //Get the fakeEventRandomMask value
      theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fakeEventRandomMask", notFound);
      if (notFound) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: fakeEventRandomMask for channel pair."));
      }
      else {
	//Set the value
	theFed9UDescription.setFakeEventRandomMask(theFed9UAddress,theIntNodeValue);
      }
      
      
    }catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }
  

  //*********************************************************
  //Method which will set the attributes for the channel node.
  void Fed9UXMLDescription::setChannelAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    try {
    //There are two possible type of node value, integer and string.
    unsigned long theIntNodeValue = 0;
    string theStringNodeValue;

    //This is to flag nodes which are not found, and which should be found.
    bool notFound = false; 

    //Get the channel ID
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
    if (notFound) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: channel id."));
    }
    else {
      //Set the channel using the mapping in Fed9UChannelInterface.
      theFed9UAddress.setExternalFeUnitChannel(theIntNodeValue);
    }

    //Get the course delay
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "coarseDelay", notFound);
    if (!notFound) {
      theFed9UDescription.setCoarseDelay(theFed9UAddress, theIntNodeValue);
    }

    //Get the fine delay
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "fineDelay", notFound);
    if (!notFound) {
      theFed9UDescription.setFineDelay(theFed9UAddress, theIntNodeValue);
    }

    //Get the frame threshold 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "threshold", notFound);
    if (!notFound) {
      if (!((theIntNodeValue%32)==0)) {
	unsigned long theNewThreshold = static_cast<unsigned long>(static_cast<float>(theIntNodeValue)/32.0 + 0.5) * 32;
	string theErrorMessage = "ERROR: You have tried to set the channel threshold to a value which is not a multiple of 32. I have calculated what you want to put as: ";
	ostringstream theNumString;
	theNumString << theNewThreshold;
	theErrorMessage += theNumString.str();
	theNumString.str("");
	theErrorMessage += " The value in the xml was set to :";
	theNumString << theIntNodeValue;
	theErrorMessage += theNumString.str();
	theErrorMessage += ". The value has been automatically updated in the description, you should put this into your XML configuration to avoid this problem"; 
	Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_INTERMEDIATE) << "We are about to set the threshold to :" << theIntNodeValue << std::endl;
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_WARNING) << theErrorMessage << std::endl;	
// THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE, theErrorMessage.c_str()));
      }
      theFed9UDescription.setFrameThreshold(theFed9UAddress, theIntNodeValue);
    }

    //Get the TrimDAC offset 
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "trimDacOffset", notFound);
    if (!notFound) {
      theFed9UDescription.setTrimDacOffset(theFed9UAddress, theIntNodeValue);
    }

    //Get the ADC input range mode type
    notFound = false;

    const u16 channelPairMap[12] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8};
    Fed9UAdcControls tempControls;
    if ( channelPairMap[theFed9UAddress.getFeUnitChannel()] <  theFed9UAddress.getFeUnitChannel() ) {
      // store previous value to compare later
      tempControls = theFed9UDescription.getAdcControls(theFed9UAddress);
      //      cout << "values stored in tempControls = " << (tempControls._dfsen?"true":"false") << (tempControls._dfsval?"true":"false") << (tempControls._s1?"true":"false") << endl;
    }

    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "adcInputRangeMode", notFound);
    if (!notFound) {
      if (theStringNodeValue=="1V") {
	//cout << "1V" << endl;
	Fed9UAdcControls theFed9UAdcControlsFor1V(true, false, true, false);
	theFed9UDescription.setAdcControls(theFed9UAddress, theFed9UAdcControlsFor1V);
      }
      else if (theStringNodeValue=="2V") {
	//cout << "2V" << endl;
	Fed9UAdcControls theFed9UAdcControlsFor2V(false, false, true, false);
	theFed9UDescription.setAdcControls(theFed9UAddress, theFed9UAdcControlsFor2V);
      }
      else if (theStringNodeValue=="DISABLE") {
	//cout << "DISABLE" << endl;
	Fed9UAdcControls theFed9UAdcControlsDisable(false, false, false, false);
	theFed9UDescription.setAdcControls(theFed9UAddress, theFed9UAdcControlsDisable);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: apv adcInputRangeMode"));
      }
    }
    // use the following vector to point each channel to it's pair

    //    std::cout << "about to check channel pair in XML read!" << std::endl; 
    //    std::cout << "channel = " << (u16)theFed9UAddress.getFeUnitChannel() << ", mapped channel = " 
    //	      << channelPairMap[theFed9UAddress.getFeUnitChannel()]  
    //	      <<  std::endl; 
    if ( channelPairMap[theFed9UAddress.getFeUnitChannel()] <  theFed9UAddress.getFeUnitChannel() ) {
      //   std::cout << "If we get here then we are about to do the comparrison!" << std::endl; 
      try {            
	Fed9UAddress tempAdd = theFed9UAddress;
	tempAdd.setFeUnitChannel(channelPairMap[theFed9UAddress.getFeUnitChannel()]);
	ICUTILS_VERIFYX( theFed9UDescription.getAdcControls(theFed9UAddress) 
			 == theFed9UDescription.getAdcControls(tempAdd)
			 ,Fed9UXMLDescriptionException)
	  ((u16)theFed9UAddress.getFedFeUnit())
	  ((u16)theFed9UAddress.getFeUnitChannel())
	  (channelPairMap[theFed9UAddress.getFeUnitChannel()])
	  .code(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE).msg("WARNING!!! the adcInputRangeMode for this pair of channels do not match").error();
	
      } catch (ICUtils::ICException & exc ) {
	Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << exc.what();
      }
      
      //std::cout << "result of comparrison = " << ( ( theFed9UDescription.getAdcControls(theFed9UAddress) 
      //					     == tempControls ) ? "true" : "false" )<< std::endl; 
    }
    
    //Get the data complement state (note the reverse logic - disabling the complement corresponds to setting a true in Fed9UDescription.)
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "dataComplement", notFound);
    if (!notFound) {
      if (theStringNodeValue=="ENABLE") {
	theFed9UDescription.setComplement(theFed9UAddress, false);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UDescription.setComplement(theFed9UAddress, true);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: channel dataComplement"));
      }
    }

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  } 

  //*********************************************************
  //Method which will set the attributes for the channel node.
  void Fed9UXMLDescription::setApvAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    try {
    //There are two possible type of node value, integer and string.
    unsigned long theIntNodeValue = 0;
    string theStringNodeValue;
    //This is to flag nodes which are not found, and which should be found.
    bool notFound = false; 

    //Get the avp ID
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
    if (notFound) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: apv id"));
    }
    else {
      theFed9UAddress.setExternalChannelApv(theIntNodeValue);
    }

    //Get the enable/disable state
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "state", notFound);
    if (!notFound) {
      if (theStringNodeValue=="ENABLE") {
	theFed9UDescription.setApvDisable(theFed9UAddress, 0);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UDescription.setApvDisable(theFed9UAddress, 1);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: apv state"));
      }
    }
  
    // <GR date="27/07/06"> Blah
    //Get the APV Fake Event enable/disable state
    notFound = false;
    theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "fakeEventState", notFound);
    if (!notFound) {
      if (theStringNodeValue=="ENABLE") {
	theFed9UDescription.setApvFakeEventDisable(theFed9UAddress, 0);
      }
      else if (theStringNodeValue=="DISABLE") {
	theFed9UDescription.setApvFakeEventDisable(theFed9UAddress, 1);
      }
      else {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: apv fake event state"));
      }
    }
    // </GR>

    //Get the median override value. Also disable the median calculation in the FE chip.
    notFound = false;
    theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "medianOverride", notFound);
    if (!notFound) {
      theFed9UDescription.setMedianOverride(theFed9UAddress, theIntNodeValue);
      //Now disable the median calculation in the FED. This expects a feFpga address, but one has already been set.
      // <JRF> edited this code so that it only disables the median override if the value is non zero
      if (theIntNodeValue != 0)
	theFed9UDescription.setMedianOverrideDisable(theFed9UAddress, false); //Enable the median override.
      else
	theFed9UDescription.setMedianOverrideDisable(theFed9UAddress, true); //Disable the median override.

      //Count up the number of these set. I only allow 24 because the median calculation in the FED is disabled for a whole front-end.
      ++countApvs;
      if (countApvs==25) {
	countApvs=1;
      }
    }

    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  
  }

  //*************************************************************
  //Method which will set the attributes for the strip node.
  void Fed9UXMLDescription::setStripAttributes(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    try {
      //There are two possible type of node value, integer and string.
      unsigned long theIntNodeValue = 0;
      string theStringNodeValue;
      float theFloatNodeValue = 0.0;
      //This is to flag nodes which are not found, and which should be found.
      bool notFound = false; 
      
      //Get the strip ID
      theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "id", notFound);
      if (notFound) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND,"The attribute expected was: strip id"));
      }
      else {
	theFed9UAddress.setExternalApvStrip(theIntNodeValue);
      }
      
      //Instantiate a Fed9UStripDescription
      Fed9UStripDescription theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(theFed9UAddress);
      
      //Get the enable/disable state
      notFound = false;
      theStringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "state", notFound);
      if (!notFound) {
	if (theStringNodeValue=="ENABLE") {
	  theFed9UStripDescription.setDisable(false);
	}
	else if (theStringNodeValue=="DISABLE") {
	  theFed9UStripDescription.setDisable(true);
	}
	else {
	  THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN_ATTRIBUTE_VALUE,"The attribute is: strip state"));
	}
      }
      
      //Get the pedestal value 
      notFound = false;
      theIntNodeValue = getIntNodeValue(theAttributesDOMNamedNodeMap, "ped", notFound);
      if (!notFound) {
	theFed9UStripDescription.setPedestal(theIntNodeValue);
      }
      
      //Get the noise value 
      notFound = false;
      theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "noise", notFound);
      if (!notFound) {
	theFed9UStripDescription.setNoise(theFloatNodeValue);
      }
      
      //Get the low cluster threshold
      notFound = false;
      theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "low", notFound);
      if (!notFound) {
	theFed9UStripDescription.setLowThresholdFactor(theFloatNodeValue);
      }
      
      //Get the high cluster threshold
      notFound = false;
      theFloatNodeValue = getFloatNodeValue(theAttributesDOMNamedNodeMap, "high", notFound);
      if (!notFound) {
	theFed9UStripDescription.setHighThresholdFactor(theFloatNodeValue);
      }
      
      //Check that the high threshold is greater than the low one.
      /*if (theFed9UStripDescription.getHighThreshold() < theFed9UStripDescription.getLowThreshold()) {
	THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_ILLEGAL_ATTRIBUTE,"The high cluster threshold appears to be smaller than the low cluster threshold."));
	}*/
      
      //Load the strip information back into the Fed9UDescription
      theFed9UDescription.getFedStrips().setStrip(theFed9UAddress, theFed9UStripDescription);
      
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
    
  }
  

 //*************************************************************
  //Method which will set the attributes for the strips node.
  void Fed9UXMLDescription::setStripsAttribute(DOMNamedNodeMap *theAttributesDOMNamedNodeMap) throw (Fed9UXMLDescriptionException)
  {
    // each strip is stored as 32 bits of info in a string blob, we have to loop over each strip and extract the data from
    // each of the 32 bit words. The data is stored in the following bits
    //
    // *****************************************
    // * Value       *   bits   *   range      *
    // *****************************************
    // * Pedestal    *   [31-22]*   0-1023     *
    // * Noise       *   [21-13]* 0.0-51.1(0.1)*
    // * High Factor *   [12-7] * 0.0-12.6(0.2)*
    // * Low Factor  *   [6-1]  * 0.0-12.6(0.2)*
    // * Disable     *   [0]    * 0,1          *
    // *****************************************
    //
    try {
      string stringNodeValue;
      //This is to flag nodes which are not found, and which should be found.
      bool notFound = false; 
      stringNodeValue = getStringNodeValue(theAttributesDOMNamedNodeMap, "data", notFound);
      ICUTILS_VERIFYX(stringNodeValue.size() == 696,Fed9UXMLDescriptionException)(stringNodeValue.size())(1024).code(Fed9UXMLDescriptionException::ERROR_UNKNOWN).error().msg("the string value for the strip is not the correct length");
      //std::stringstream ssNodeValue;
      // ssNodeValue << std::hex << stringNodeValue.c_str();
      // loop over the internal strips numbering 
      float low = 0.0;
      float high = 0.0;
      i16 ped = 0;
      float noise = 0.0;
      bool disable;
      //std::stringstream ss;
      //ss << std::hex;
      //      u32 stripData=0;
      Fed9UStripDescription theFed9UStripDescription;
      u32 * stripsBuf;
      XMLByte* decodedBinary;
      XMLSize_t decodedLength;
      stripsBuf = reinterpret_cast<u32*> ( decodedBinary = Base64::decode(reinterpret_cast<XMLByte*>(const_cast<char*>(stringNodeValue.c_str())), &decodedLength  ) ) ;
      
      for (int i=0;i<STRIPS_PER_APV;i++) {
	theFed9UAddress.setApvStrip(i);
	//set the fed9UStripDescription
	theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(theFed9UAddress);
	//stripData = 0;
	//ss << std::hex << ssNodeValue.str().substr(i*8,8);
	//ss >> stripData;
	//	std::cout << std::hex << "strip i = " << i << "data = "<< stripData << endl;
	//ss.clear();
	/*	if (i == 0)
	  std::cout << std::hex << stripData << endl;
	*/
	low = static_cast<float>( ( stripsBuf[i] >> 1 )    & 0x0000003F ) / 5.0;
	high = static_cast<float>( ( stripsBuf[i] >> 7 )   & 0x0000003F ) / 5.0;
	noise = static_cast<float>( ( stripsBuf[i] >> 13 ) & 0x000001FF ) / 10.0;
	ped = static_cast<i16>( ( stripsBuf[i] >> 22 )     & 0x000003FF );
	disable = static_cast<bool>(   stripsBuf[i]        & 0x00000001 );
	theFed9UStripDescription.setLowThresholdFactor(low);
	theFed9UStripDescription.setHighThresholdFactor(high);
	theFed9UStripDescription.setNoise(noise);
	theFed9UStripDescription.setPedestal(ped);
	theFed9UStripDescription.setDisable(disable);
	//Load the strip information back into the Fed9UDescription
	theFed9UDescription.getFedStrips().setStrip(theFed9UAddress, theFed9UStripDescription);
	
	
      }
      delete decodedBinary;
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  } 
  


  //**************************************************************
  //Method to get the integer node value from a DOMNamedNodeMap
  unsigned long Fed9UXMLDescription::getIntNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException)
  {
    try {
      unsigned long theIntNodeValue = 0;
      if (theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()))) {
	DOMNode *theAttributeNode = theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()));
	char *theCharNodeValue = XMLString::transcode(theAttributeNode->getNodeValue());
	theIntNodeValue = static_cast<unsigned long>(atoi(theCharNodeValue));
	XMLString::release(&theCharNodeValue); //Clean up from the XMLString::transcode
      }
      else {
	notFound = true;
      }
      return theIntNodeValue;
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }

  }

  //**************************************************************
  //Method to get the signed integer node value from a DOMNamedNodeMap
  int Fed9UXMLDescription::getSignedIntNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException)
  {
    try {
      int theSignedIntNodeValue = 0;
      if (theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()))) {
	DOMNode *theAttributeNode = theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()));
	char *theCharNodeValue = XMLString::transcode(theAttributeNode->getNodeValue());
	theSignedIntNodeValue = static_cast<int>(atoi(theCharNodeValue));
	XMLString::release(&theCharNodeValue); //Clean up from the XMLString::transcode
      }
      else {
	notFound = true;
      }
      return theSignedIntNodeValue;
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }

  //**************************************************************
  //Method to get the float node value from a DOMNamedNodeMap
  float Fed9UXMLDescription::getFloatNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException)
  {
    try {
      float theFloatNodeValue = 0.0;
      if (theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()))) {
	DOMNode *theAttributeNode = theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()));
	char *theCharNodeValue = XMLString::transcode(theAttributeNode->getNodeValue());
	theFloatNodeValue = static_cast<float>(atof(theCharNodeValue));
	XMLString::release(&theCharNodeValue); //Clean up from the XMLString::transcode
      }
      else {
	notFound = true;
      }
      return theFloatNodeValue;
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }

  //**************************************************************
  //Method to get the string node value from a DOMNamedNodeMap
  string Fed9UXMLDescription::getStringNodeValue(DOMNamedNodeMap *theAttributesDOMNamedNodeMap, string theAttributeName, bool &notFound) throw (Fed9UXMLDescriptionException)
  {
    try {
      string theStringNodeValue;
      if (theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()))) {
	DOMNode *theAttributeNode = theAttributesDOMNamedNodeMap->getNamedItem(X(theAttributeName.c_str()));
	char *theCharNodeValue = XMLString::transcode(theAttributeNode->getNodeValue());
	theStringNodeValue = string(theCharNodeValue);
	XMLString::release(&theCharNodeValue); //Clean up from the XMLString::transcode
      }
      else {
	notFound = true;
      }
      return theStringNodeValue;
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      string errMesg = "DOMException error. Message: " + string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"unknown exception."));
    }
  }

    /**Method to get the value of usingStrips_*/
  bool Fed9UXMLDescription::getUsingStrips() {
    return usingStrips_;
  }

  /**Method to set the value of usingStrips_*/
  void Fed9UXMLDescription::setUsingStrips(bool usingStrips) {
    usingStrips_ = usingStrips;
  }

  /** Method to set the value of debugOutput_. If this is set to true the the buffer will be displayed to standard output for debugging */
  void Fed9UXMLDescription::setDebugOutput(bool value) {
    debugOutput_ = value;
  }

}
#endif
#endif
