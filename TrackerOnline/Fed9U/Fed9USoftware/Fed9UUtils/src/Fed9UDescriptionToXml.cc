//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UDescriptionToXml."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
//#pragma message(_XERCES_VERSION)
#if _XERCES_VERSION >= 20300

//Xerces headers
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/Base64.hpp>

#include "Fed9UStrX.hh"
#include "Fed9UXStr.hh"

//Fed9U headers
#include "Fed9UDescriptionToXml.hh"

//Standard includes
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>

namespace Fed9U {

  using xercesc::DOMDocument;
  using xercesc::DOMElement;
  using xercesc::DOMException;
  using xercesc::DOMImplementation;
  using xercesc::DOMImplementationRegistry;
  using xercesc::DOMImplementationLS;
  using xercesc::DOMLSSerializer;
  using xercesc::XMLPlatformUtils;
  using xercesc::XMLException;
  using xercesc::XMLString;
  using xercesc::XMLUni;
  using xercesc::XMLFormatTarget;
  using xercesc::LocalFileFormatTarget;
  using xercesc::MemBufFormatTarget;
  using xercesc::Base64;
  using std::cerr;
  using std::endl;
  using std::hex ;
  using std::dec ;
  using std::ostringstream;
  using std::ifstream;
  using std::ofstream;


  Fed9UDescriptionToXml::Fed9UDescriptionToXml(const std::string &targetXMLFileName,  std::vector<Fed9UDescription*>  Fed9UDescriptionsToWrite , bool usingStrips, bool usingBinaryStrips) throw (Fed9UXMLDescriptionException): 
    doc(NULL),
    theDOMWriter(NULL),
    theDOMOutput(NULL),
    theTargetXMLFileName(targetXMLFileName),
    theFed9UDescription(*(new Fed9UDescription(*(Fed9UDescriptionsToWrite[0])))),
    theFed9UDescriptionList(Fed9UDescriptionsToWrite),
    usingStrips_(usingStrips),
    debugOutput_(false),
    usingBinaryStrips_(usingBinaryStrips)
  {
    //Initialize the Xerces APIs
    try {
      initializeXerces();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN, "Fed9UXMLDescriptionException caught during a call to Fed9UDescriptionToXml::initializeXerces()."));
    }
  }


  Fed9UDescriptionToXml::Fed9UDescriptionToXml( std::vector<Fed9UDescription*>  Fed9UDescriptionsToWrite , bool usingStrips, bool usingBinaryStrips) throw (Fed9UXMLDescriptionException): 
    doc(NULL),
    theDOMWriter(NULL),
    theDOMOutput(NULL),
    theTargetXMLFileName(""),
    theFed9UDescription(*(new Fed9UDescription(*(Fed9UDescriptionsToWrite[0])))),
    theFed9UDescriptionList(Fed9UDescriptionsToWrite),
    usingStrips_(usingStrips),
    debugOutput_(false),
    usingBinaryStrips_(usingBinaryStrips)
  {
    //Initialize the Xerces APIs
    try {
      initializeXerces();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN, "Fed9UXMLDescriptionException caught during a call to Fed9UDescriptionToXml::initializeXerces()."));
    }
  }


  Fed9UDescriptionToXml::Fed9UDescriptionToXml(const std::string &targetXMLFileName, Fed9UDescription &Fed9UDescriptionToWrite, bool usingStrips, bool usingBinaryStrips) throw (Fed9UXMLDescriptionException) : 
    doc(NULL),
    theDOMWriter(NULL),
    theDOMOutput(NULL),
    theTargetXMLFileName(targetXMLFileName),
    theFed9UDescription(*(new Fed9UDescription(Fed9UDescriptionToWrite))),
    usingStrips_(usingStrips),
    debugOutput_(false),
    usingBinaryStrips_(usingBinaryStrips)
  {
    //Initialize the Xerces APIs
    try {
      initializeXerces();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN, "Fed9UXMLDescriptionException caught during a call to Fed9UDescriptionToXml::initializeXerces()."));
    }
  }


  Fed9UDescriptionToXml::Fed9UDescriptionToXml( Fed9UDescription &Fed9UDescriptionToWrite, bool usingStrips, bool usingBinaryStrips) throw (Fed9UXMLDescriptionException) :    
    doc(NULL),
    theDOMWriter(NULL),
    theDOMOutput(NULL),
    theFed9UDescription(*(new Fed9UDescription(Fed9UDescriptionToWrite))),
    usingStrips_(usingStrips), 
    debugOutput_(false),
    usingBinaryStrips_(usingBinaryStrips) {
    //Initialize the Xerces APIs
    try {
      initializeXerces();
    }
    catch (Fed9UXMLDescriptionException &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN, "Fed9UXMLDescriptionException caught during a call to Fed9UDescriptionToXml::initializeXerces()."));
    }
  }


  Fed9UDescriptionToXml::~Fed9UDescriptionToXml() {
    try {
      if (&theFed9UDescription)
	delete &theFed9UDescription;
      if (theDOMWriter)
	theDOMWriter->release();
      if (doc)
	doc->release();
      //XMLPlatformUtils::Terminate();
    }
  
    catch (...) {
      cerr << "Exception caught in destructor of Fed9UDescriptionToXml. " << endl;
    }
  }

  
  void Fed9UDescriptionToXml::streamOutDescription(std::ostream * os) throw (Fed9UXMLDescriptionException)
  {
    try {
      constuctDOMDocument();
  
      //Define an XML format target (point to object of type LocalFileFormatTarget to write to a file).
      MemBufFormatTarget *theXMLFormatTarget = new MemBufFormatTarget();
      
      //Get the DOM from the XML parser and stream to object pointed of type XMLFormatTarget.
      theDOMOutput->setByteStream(theXMLFormatTarget);
      theDOMWriter->write(doc,theDOMOutput);
      if (debugOutput_)
	std::cout << "debug output = " << (debugOutput_ ? "true \n" : "false \n" ) << theXMLFormatTarget->getRawBuffer() << std::endl;
      (*os) << theXMLFormatTarget->getRawBuffer();
      //Clean up
      if (theXMLFormatTarget) {
	delete theXMLFormatTarget;
      }
    }
    catch (const XMLException& e) {
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException error during creation of XML file. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception during creating of XML file."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion during creation of XML file."));
    }
  }
  
  /* DOMDocument* getDescriptionDomDocument(void) 
  {
  }*/


  void Fed9UDescriptionToXml::writeXmlFile(void) throw (Fed9UXMLDescriptionException)
  {
    try {
      constuctDOMDocument();
      
      //Define an XML format target (point to object of type LocalFileFormatTarget to write to a file).
      LocalFileFormatTarget *theXMLFormatTarget = new LocalFileFormatTarget(const_cast<const char* const>(theTargetXMLFileName.c_str()));
      
      //Get the DOM from the XML parser and stream to object pointed of type XMLFormatTarget.
      theDOMOutput->setByteStream(theXMLFormatTarget);
      theDOMWriter->write(doc,theDOMOutput);

      //Clean up
      if (theXMLFormatTarget) {
	delete theXMLFormatTarget;
      }
    }
    catch (const XMLException& e) {
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception                                              
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException error during creation of XML file. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception            
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception during creating of XML file."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion during creation of XML file."));
    }
  }


  //****************************************************************************************************
  //PRIVATE METHODS
  //****************************************************************************************************


  void Fed9UDescriptionToXml::constuctDOMDocument(void) throw (Fed9UXMLDescriptionException) {
  
    try {
      //Get a DOM Core implementation.
      DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
      
      //Create a DOM document with root element using the DOMImplementation. 
      //The first argument of createDocumentis the root element namespace name, 
      //the second argument is the name of the root element and the third argument is DOMDocumentType.
      doc = impl->createDocument(X("http://fed9u.web.cern.ch/fed9u/configuration"),X("fed9U:CMSTrackerFeds"),0);
      //Add the schema statements to the root element
      DOMElement* rootElement = doc->getDocumentElement();
      rootElement->setAttribute(X("xmlns:xsi"), X("http://fed9u.web.cern.ch/fed9u/configuration"));
      rootElement->setAttribute(X("xsi:noNamespaceSchemaLocation"), X("ConfigFed9U.xsd"));
            
      //Add Fed9U elements using the Fed9UDescription to assign the attributes.
      addDOMElements(rootElement);
      //Now write the DOM object to an XML file. Start by setting up a DOMWriter.
      //Instantiate a DOMWriter
      XMLCh tempStr[100];
      XMLString::transcode("LS", tempStr, 99);
      impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
      theDOMWriter = ((DOMImplementationLS*)impl)->createLSSerializer();
      theDOMOutput = ((DOMImplementationLS*)impl)->createLSOutput();
      
      //Plug in error handler  
      //      theDOMWriter->setErrorHandler(&errorHandler);
      theDOMWriter->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler,&errorHandler);
      //reset error count
      errorHandler.resetErrors();

      //Include whitespaces in the output
      if (theDOMWriter->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
	theDOMWriter->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
      }
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException error during creation of DOMDocument. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception         
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception during creation of DOMDocument."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion during creation of DOMDocument."));
    }
  }
    

  //Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.
  void Fed9UDescriptionToXml::initializeXerces(void) throw (Fed9UXMLDescriptionException)
  {
    try {
      if (XMLPlatformUtils::fgTransService == NULL)   
            XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& e) {
      //NOTE: Cannot use RETHROW here since XMLException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_XML, Fed9UStrX(e.getMessage()).localForm()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }

  
  void Fed9UDescriptionToXml::addDOMElements(DOMElement *rootElement) throw (Fed9UXMLDescriptionException)
  {
    //    DOMElement* rootElement = doc->getDocumentElement();

    try {


      u32 numberOfFeds = (theFed9UDescriptionList.size() == 0) ? 1 : theFed9UDescriptionList.size();
      
      for ( u32 ifeds = 0 ; ifeds < numberOfFeds ; ifeds ++ ) {
	if ( theFed9UDescriptionList.size() > 0 )
	  theFed9UDescription = *theFed9UDescriptionList[ifeds];

	//Create a fed element and attach it to the root element. Then set the attributes from the Fed9UDescription.
	DOMElement* fedChildElement = doc->createElement(X("fed"));
	rootElement->appendChild(fedChildElement);
	addFedAttributes(fedChildElement);
	//Now add child elements to fed
	unsigned short i=0, j=0, k=0, l=0, m=0;
	//Create feFpga child elements to fed. Then set attributes from Fed9UDescription.
	for (i=0; i<=7; ++i) {
	  DOMElement* feFpgaChildElement = doc->createElement(X("feFpga"));
	  fedChildElement->appendChild(feFpgaChildElement);
	  addFeFpgaAttributes(feFpgaChildElement,i);
	  // Create channel pair child elements to feFpga, then set attribus from Fed9uDescription
	  for (m=0; m<6; m++) {
	    DOMElement* channelPairChildElement = doc->createElement(X("channelPair"));
	    feFpgaChildElement->appendChild(channelPairChildElement);
	    addChannelPairAttributes(channelPairChildElement, m);
	    
	    //Create channel child elements to channel pair. Then set attributes from Fed9UDescription.
	    for (j=0; j<2; ++j) {
	      DOMElement* channelChildElement = doc->createElement(X("channel"));
	      channelPairChildElement->appendChild(channelChildElement);
	      addChannelAttributes(channelChildElement, 2*m+j);
	      //Create APV child elements to channel. Then set attributes from Fed9UDescription.
	      for (k=0; k<=1; ++k) {
		DOMElement* apvChildElement = doc->createElement(X("apv"));
		channelChildElement->appendChild(apvChildElement);
		addApvAttributes(apvChildElement, k);
		// If usingStrips_ is true then we create strip child elements to APV. Then set attributes from Fed9UDescription.
		if (usingStrips_) {
		  if (usingBinaryStrips_) {
		    // now rather than writing one tag for each strip, we loop over all strips and create one binary blob which we write to the 
		    // <strips> tag, this is done within the method addStripsAttribute, note that this is a copy of what is in the class Fed9UDescriptionToXmlToDb
		    // so the overloaded method is now redundant and should be removed
		    DOMElement* stripChildElement = doc->createElement(X("strips"));
		    apvChildElement->appendChild(stripChildElement);
		    addStripsAttribute(stripChildElement);
		  }
		  else {
		    // If usingStrips_ is true then we create strip child elements to APV. Then set attributes from Fed9UDescription.
		    for (l=0; l<=127; ++l) {
		      DOMElement* stripChildElement = doc->createElement(X("strip"));
		      apvChildElement->appendChild(stripChildElement);
		      addStripAttributes(stripChildElement, l);
		    }
		  }
		}
		
		
		
	      }
	    }
	  }
	}
      }
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                               
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion."));
    }
  }


  void Fed9UDescriptionToXml::addFedAttributes(DOMElement* childElement) throw (Fed9UXMLDescriptionException)
  {
    try {
    bool fedParamBool = false;
    ostringstream numberString; //Define ostringstream to convert integers to a string
  
    //Set opto Rx Resistor Value 
    numberString << theFed9UDescription.getOptoRXResistor();
    childElement->setAttribute(X("optoRxResistor"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set fed id 
    numberString << theFed9UDescription.getFedId();
    childElement->setAttribute(X("id"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set fed Hardware id 
    numberString << theFed9UDescription.getFedHardwareId();
    childElement->setAttribute(X("hardwareId"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set the description name
    std::string theDescriptionName= theFed9UDescription.getName();
    childElement->setAttribute(X("name"), X(theDescriptionName.c_str()));
    
    //Set the fake event file name
    std::string theFakeEventFile= theFed9UDescription.getFakeEventFile();
    childElement->setAttribute(X("fakeEventFile"), X(theFakeEventFile.c_str()));

    //Set fake event trigger delay
    numberString << theFed9UDescription.getFakeEventTriggerDelay();
    childElement->setAttribute(X("fakeEventTriggerDelay"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set feFirmwareVersion 
    numberString << hex << theFed9UDescription.getFeFirmwareVersion(); 
    childElement->setAttribute(X("feFirmwareVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.
    
    //Set beFirmwareVersion 
    numberString << hex << theFed9UDescription.getBeFirmwareVersion();
    childElement->setAttribute(X("beFirmwareVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.
    
    //Set vmeFirmwareVersion 
    numberString << hex << theFed9UDescription.getVmeFirmwareVersion();
    childElement->setAttribute(X("vmeFirmwareVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.
    
    //Set delayFirmwareVersion 
    numberString << hex << theFed9UDescription.getDelayFirmwareVersion();
    childElement->setAttribute(X("delayFirmwareVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set fedVersion 
    numberString << dec << theFed9UDescription.getFedVersion();
    childElement->setAttribute(X("fedVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set epromVersion 
    numberString << theFed9UDescription.getEpromVersion();
    childElement->setAttribute(X("epromVersion"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set the crate number
    numberString << theFed9UDescription.getCrateNumber();
    childElement->setAttribute(X("crate"), X((numberString.str()).c_str()));
    numberString.str("");

    //Set the VME controller daisy chain ID
    numberString << theFed9UDescription.getVmeControllerDaisyChainId();
    childElement->setAttribute(X("vmeControllerDaisyChainId"), X((numberString.str()).c_str()));
    numberString.str("");

    //Set the global coarse shift
    numberString << theFed9UDescription.getGlobalCoarseSkew();
    childElement->setAttribute(X("globalClockCoarseShift"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set the global fine shift
    numberString << theFed9UDescription.getGlobalFineSkew();
    childElement->setAttribute(X("globalClockFineShift"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.

    //Set fed base address
    numberString << static_cast<unsigned long>((theFed9UDescription.getBaseAddress()>>16)&0x1f); //Get the crate slot number from the base address.
    childElement->setAttribute(X("crateSlot"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the fed disable/enable state
    fedParamBool = theFed9UDescription.getFedBeFpgaDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("state"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("state"), X("DISABLE")); 
    }

    //Set the fed mode
    Fed9UDaqMode theFed9UDaqMode = theFed9UDescription.getDaqMode();
    if (theFed9UDaqMode==FED9U_MODE_ZERO_SUPPRESSED) {
      childElement->setAttribute(X("mode"), X("ZERO_SUPPRESSION"));
    }
    else if (theFed9UDaqMode==FED9U_MODE_PROCESSED_RAW) {
      childElement->setAttribute(X("mode"), X("PROCESSED_RAW"));
    }
    else if (theFed9UDaqMode==FED9U_MODE_VIRGIN_RAW) {
      childElement->setAttribute(X("mode"), X("VIRGIN_RAW"));
    }
    else if (theFed9UDaqMode==FED9U_MODE_SCOPE) {
      childElement->setAttribute(X("mode"), X("SCOPE"));
    }

    //Set the fed mode
    Fed9UDaqSuperMode theFed9UDaqSuperMode = theFed9UDescription.getDaqSuperMode();
    if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE) {
      childElement->setAttribute(X("superMode"), X("FAKE"));
    }
    else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_ZERO_LITE) {
      childElement->setAttribute(X("superMode"), X("ZERO_LITE"));
    }
    else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_ZERO_LITE) {
      childElement->setAttribute(X("superMode"), X("FAKE_ZERO_LITE"));
    }
    else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_NORMAL) {
      childElement->setAttribute(X("superMode"), X("NORMAL"));
    }
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_HI_LO) {
		childElement->setAttribute(X("superMode"), X("FAKE_HI_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_ZERO_LITE_HI_LO) {
		childElement->setAttribute(X("superMode"), X("ZERO_LITE_HI_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_ZERO_LITE_HI_LO) {
		childElement->setAttribute(X("superMode"), X("FAKE_ZERO_LITE_HI_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_NORMAL_HI_LO) {
		childElement->setAttribute(X("superMode"), X("NORMAL_HI_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_LO) {
		childElement->setAttribute(X("superMode"), X("FAKE_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_ZERO_LITE_HI_LO) {
		childElement->setAttribute(X("superMode"), X("ZERO_LITE_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_ZERO_LITE_HI_LO) {
		childElement->setAttribute(X("superMode"), X("FAKE_ZERO_LITE_LO"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_NORMAL_HI_LO) {
		childElement->setAttribute(X("superMode"), X("NORMAL_LO"));
	}	
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_10) {
		childElement->setAttribute(X("superMode"), X("FAKE_10"));
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_NORMAL_10) {
		childElement->setAttribute(X("superMode"), X("NORMAL_10"));
	}
    //Set the scope mode length
    numberString << theFed9UDescription.getScopeLength();
    childElement->setAttribute(X("scopeLength"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the fed clock source
    Fed9UClockSource theFed9UClockSource = theFed9UDescription.getClock();
    if (theFed9UClockSource==FED9U_CLOCK_TTC) {
      childElement->setAttribute(X("clockSource"), X("TTC"));
    }
    else if (theFed9UClockSource==FED9U_CLOCK_BACKPLANE) {
      childElement->setAttribute(X("clockSource"), X("BACKPLANE"));
    }
    else if (theFed9UClockSource==FED9U_CLOCK_INTERNAL) {
      childElement->setAttribute(X("clockSource"), X("INTERNAL"));
    }

    //Set the fed trigger source
    Fed9UTrigSource theFed9UTrigSource = theFed9UDescription.getTriggerSource();
    if (theFed9UTrigSource==FED9U_TRIG_TTC) {
      childElement->setAttribute(X("triggerSource"), X("TTC"));
    }
    else if (theFed9UTrigSource==FED9U_TRIG_BACKPLANE) {
      childElement->setAttribute(X("triggerSource"), X("BACKPLANE"));
    }
    else if (theFed9UTrigSource==FED9U_TRIG_SOFTWARE) {
      childElement->setAttribute(X("triggerSource"), X("SOFTWARE"));
    }

    //Set the fed read route
    Fed9UReadRoute theFed9UReadRoute = theFed9UDescription.getBeFpgaReadRoute();
    if (theFed9UReadRoute==FED9U_ROUTE_SLINK64) {
      childElement->setAttribute(X("readRoute"), X("SLINK64"));
    }
    else if (theFed9UReadRoute==FED9U_ROUTE_VME) {
      childElement->setAttribute(X("readRoute"), X("VME"));
    }

    //Set the Hal Bus Adaptor Type
    Fed9UHalBusAdaptor theBusAdaptorType = theFed9UDescription.getBusAdaptorType();
    if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_SBS) {
      childElement->setAttribute(X("halBusAdaptor"), X("SBS"));
    }
    else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_CAEN_PCI) {
      childElement->setAttribute(X("halBusAdaptor"), X("CAEN_PCI"));
    }
    else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_CAEN_USB) {
      childElement->setAttribute(X("halBusAdaptor"), X("CAEN_USB"));
    }
    else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_VXI) {
      childElement->setAttribute(X("halBusAdaptor"), X("VXI"));
    }
    else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_DUMMY) {
      childElement->setAttribute(X("halBusAdaptor"), X("DUMMY"));
    }

    //Set the test register for the BE FPGA
    numberString << theFed9UDescription.getTestRegister();
    childElement->setAttribute(X("testRegister"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set Fed9UAddress to read the BE FPGA parameters
    theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND); 

    //Initialize a Fed9UTempContol object from the Fed9UDescription.
    Fed9UTempControl theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);

    //Set the max temp for the LM82 chip for the BE FPGA
    numberString << theFed9UTempControl.getLm82High();
    childElement->setAttribute(X("maxTempLm82BeFpga"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the max temp for the BE FPGA
    numberString << theFed9UTempControl.getFpgaHigh();
    childElement->setAttribute(X("maxTempFpgaBeFpga"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the critical temp for the BE FPGA
    numberString << theFed9UTempControl.getCritical();
    childElement->setAttribute(X("maxTempCriticalBeFpga"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //TTCrx

    //Initialize a Fed9UTtcrxDescription object from the Fed9UDescription.
    Fed9UTtcrxDescription theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();

    //Set the TTCrx L1Accept Coarse delay
    numberString << theFed9UTtcrxDescription.getL1AcceptCoarseDelay();
    childElement->setAttribute(X("ttcrxL1AcceptCoarseDelay"), X((numberString.str()).c_str())); 
    numberString.str(""); 
  
    //Set the TTCrx L1Accept Fine delay
    numberString << theFed9UTtcrxDescription.getClockDesOneFineDelay();
    childElement->setAttribute(X("ttcrxL1AcceptFineDelay"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the TTCrx BroadcastStrobeTwoCoarseDelay parameter.
    numberString << theFed9UTtcrxDescription.getBrcstStrTwoCoarseDelay();
    childElement->setAttribute(X("ttcrxBrcstStrTwoCoarseDelay"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the TTCrx BroadcastStrobeTwoFineDelay parameter. 
    //This depends on what is set for the Clock40DeskewedTwo operation flag
    if (true == theFed9UTtcrxDescription.getDeskewedClock2Selected()) {
      numberString << theFed9UTtcrxDescription.getClockDesTwoFineDelay();
    } 
    else {
      numberString << theFed9UTtcrxDescription.getClockDesOneFineDelay();
    }
    childElement->setAttribute(X("ttcrxBrcstStrTwoFineDelay"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the TTCrx DLL pump current parameter.
    numberString << theFed9UTtcrxDescription.getDllPumpCurrent();
    childElement->setAttribute(X("ttcrxDLLPumpCurrent"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the TTCrx PLL pump current parameter.
    numberString << theFed9UTtcrxDescription.getPllPumpCurrent();
    childElement->setAttribute(X("ttcrxPLLPumpCurrent"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the TTCrx Individually addressed command ID.
    numberString << theFed9UTtcrxDescription.getIacId();
    childElement->setAttribute(X("ttcrxIacId"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the TTCrx I2C ID.
    numberString << theFed9UTtcrxDescription.getI2cId();
    childElement->setAttribute(X("ttcrxI2cId"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the TTCrx Hamming Error Checking flag.
    fedParamBool = theFed9UTtcrxDescription.getHammingCheckingDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxHammingChecking"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxHammingChecking"), X("DISABLE")); 
    }

    //Set the TTCrx Bunch Counter operation flag. This is a control register flag.
    unsigned short fedParamNum = theFed9UTtcrxDescription.getCounterOperation();
    if (fedParamNum & FED9U_TTCRX_ENABLE_BUNCH_CTR ) {
      childElement->setAttribute(X("ttcrxBunchCounter"), X("ENABLE")); 
    }
    else {
      childElement->setAttribute(X("ttcrxBunchCounter"), X("DISABLE")); 
    }

    //Set the TTCrx Event Counter operation flag. This is a control register flag.
    fedParamNum = theFed9UTtcrxDescription.getCounterOperation();
    if (fedParamNum & FED9U_TTCRX_ENABLE_EVENT_CTR) {
      childElement->setAttribute(X("ttcrxEventCounter"), X("ENABLE")); 
    }
    else {
      childElement->setAttribute(X("ttcrxEventCounter"), X("DISABLE")); 
    }

    //Set the TTCrx Clock40DeskewedTwo operation flag. This is a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getDeskewedClock2Selected();
    if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxClock40DeskwedTwo"), X("ENABLE")); 
    }
    else if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxClock40DeskwedTwo"), X("DISABLE")); 
    }

    //Set the TTCrx Clock40DeskewedTwoOutput operation flag. This ia a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getDeskewedClock2Disable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxClock40DeskwedTwoOutput"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxClock40DeskwedTwoOutput"), X("DISABLE")); 
    }

    //Set the TTCrx L1 clock accept output operation flag. This ia a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getClockL1AcceptDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxClockL1AcceptOutput"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxClockL1AcceptOutput"), X("DISABLE")); 
    }

    //Set the TTCrx parallel output bus operation flag. This is a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getParrallelOutputDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxParallelOutputBus"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxParallelOutputBus"), X("DISABLE")); 
    }

    //Set the TTCrx serial output operation flag. This is a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getSerialBDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxSerialBOutput"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxSerialBOutput"), X("DISABLE")); 
    }

    //Set the TTCrx NonDeskwedClock40Output operation flag. This is a control register flag.
    fedParamBool = theFed9UTtcrxDescription.getNonDeskewedClockDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("ttcrxNonDeskwedClock40Output"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("ttcrxNonDeskwedClock40Output"), X("DISABLE")); 
    }
  
    //Voltage Monitor

    //Initialize a Fed9UVoltageControl object from the Fed9UDescription.
    Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();

    //Set the voltage monitor stand by flag. 
    fedParamBool = theFed9UVoltageControl.getStandBy();
    if (fedParamBool==true) {
      childElement->setAttribute(X("volMonStandBy"), X("ENABLE"));
    }
    else if (fedParamBool==false) {
      childElement->setAttribute(X("volMonStandBy"), X("DISABLE"));
    }

    //Set the voltage monitor resetStatusRegister flag.
    fedParamBool = theFed9UVoltageControl.getResetStatusRegister();
    if (fedParamBool==true) {
      childElement->setAttribute(X("volMonStatusRegisterReset"), X("ENABLE"));
    }
    else if (fedParamBool==false) {
      childElement->setAttribute(X("volMonStatusRegisterReset"), X("DISABLE"));
    }

    //Set the voltage monitor min2.5V parameter.
    numberString << theFed9UVoltageControl.get2Point5VoltMin();
    childElement->setAttribute(X("volMonMin2point5V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor max2.5V parameter.
    numberString << theFed9UVoltageControl.get2Point5VoltMax();
    childElement->setAttribute(X("volMonMax2point5V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor min3.3V parameter.
    numberString << theFed9UVoltageControl.get3Point3VoltMin();
    childElement->setAttribute(X("volMonMin3point3V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor max3.3V parameter.
    numberString << theFed9UVoltageControl.get3Point3VoltMax();
    childElement->setAttribute(X("volMonMax3point3V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor min5V parameter.
    numberString << theFed9UVoltageControl.get5VoltMin();
    childElement->setAttribute(X("volMonMin5V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor max5V parameter.
    numberString << theFed9UVoltageControl.get5VoltMax();
    childElement->setAttribute(X("volMonMax5V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor min12V parameter.
    numberString << theFed9UVoltageControl.get12VoltMin();
    childElement->setAttribute(X("volMonMin12V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor max12V parameter.
    numberString << theFed9UVoltageControl.get12VoltMax();
    childElement->setAttribute(X("volMonMax12V"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor minCoreV parameter.
    numberString << theFed9UVoltageControl.getCoreVoltageMin();
    childElement->setAttribute(X("volMonMinCoreV"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor maxCoreV parameter.
    numberString << theFed9UVoltageControl.getCoreVoltageMax();
    childElement->setAttribute(X("volMonMaxCoreV"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor maxSupplyV parameter.
    numberString << theFed9UVoltageControl.getSupplyVoltageMin();
    childElement->setAttribute(X("volMonMinSupplyV"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor maxSupplyV parameter.
    numberString << theFed9UVoltageControl.getSupplyVoltageMax();
    childElement->setAttribute(X("volMonMaxSupplyV"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor minExternalTemp parameter.
    numberString << theFed9UVoltageControl.getExternalTempMin();
    childElement->setAttribute(X("volMonMinExternalTemp"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor maxExternalTemp parameter.
    numberString << theFed9UVoltageControl.getExternalTempMax();
    childElement->setAttribute(X("volMonMaxExternalTemp"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor minInternalTemp parameter.
    numberString << theFed9UVoltageControl.getInternalTempMin();
    childElement->setAttribute(X("volMonMinInternalTemp"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor maxInternalTemp parameter.
    numberString << theFed9UVoltageControl.getInternalTempMax();
    childElement->setAttribute(X("volMonMaxInternalTemp"), X((numberString.str()).c_str())); 
    numberString.str("");
  
    //Set the voltage monitor offsetTemp parameter.
    numberString << theFed9UVoltageControl.getTempOffset();
    childElement->setAttribute(X("volMonOffsetTemp"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the voltage monitor whichOffsetTemp flag.
    fedParamBool = theFed9UVoltageControl.getOffsetTempSelect();
    if (fedParamBool==true) {
      childElement->setAttribute(X("volMonWhichOffsetTemp"), X("INTERNAL"));
    }
    else if (fedParamBool==false) {
      childElement->setAttribute(X("volMonWhichOffsetTemp"), X("EXTERNAL"));
    }

    //EPROM
    // we have removed this and replaced with fedHardwareId
    //Initialize a Fed9UEpromDescription from the Fed9UDescription
    //Fed9UEpromDescription theFed9UEpromDescription = theFed9UDescription.getEprom();

    //Set the eprom serial number
    //numberString << theFed9UEpromDescription.getFedSerialNumber();
    //childElement->setAttribute(X("epromSerialNumber"), X((numberString.str()).c_str()));
    //numberString.str("");



    //Set the globalClockFineShift parameter.
    numberString << theFed9UDescription.getGlobalFineSkew();
    childElement->setAttribute(X("globalClockFineShift"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the globalClockCoarseShift parameter.
    /*numberString << theFed9UDescription.getGlobalCoarseSkew();
      childElement->setAttribute(X("globalClockCoarseShift"), X((numberString.str()).c_str())); 
      numberString.str("");*/
    
    //**********************************************************************
    // (8/12/05) JF Added eventType to description
 
    //Set the eventType parameter.
    numberString << theFed9UDescription.getDaqEventType();
    childElement->setAttribute(X("eventType"), X((numberString.str()).c_str())); 
    numberString.str("");

    //**********************************************************************
    //**********************************************************************
    // (8/12/05) JEC Added fov to description
 
    //Set the fov parameter.
    numberString << theFed9UDescription.getDaqFov();
    childElement->setAttribute(X("fov"), X((numberString.str()).c_str())); 
    numberString.str("");

    //**********************************************************************
    //**********************************************************************
    // (10/1/06 JEC Added headerMode to description
    
    //Set the headerMode parameter
    Fed9UHeaderFormat theFed9UHeaderFormat = theFed9UDescription.getHeaderFormatType();
    if (theFed9UHeaderFormat==FED9U_HEADER_FULLDEBUG) {
      childElement->setAttribute(X("headerType"), X("FULLDEBUG"));
    }
    else if (theFed9UHeaderFormat==FED9U_HEADER_APVERROR) {
      childElement->setAttribute(X("headerType"), X("APVERROR"));
    }
    //**********************************************************************
    //**********************************************************************
    // (23/2/06) JEC Added bxOffset to description

    // set the bunch crossing offset value
    numberString << theFed9UDescription.getBunchCrossingOffset();
    childElement->setAttribute(X("bxOffset"), X((numberString.str()).c_str())); 
    numberString.str("");

    //**********************************************************************

    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                        
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion."));
    }
  }


  void Fed9UDescriptionToXml::addFeFpgaAttributes(DOMElement* childElement, unsigned short feId) throw (Fed9UXMLDescriptionException)
  {
    try {
    bool fedParamBool = false;
    ostringstream numberString;

    //Set the Fed9UAddress using the the feId
    //Set feFpga id 
    numberString << static_cast<u16>( theFed9UAddress.setFedFeUnit(feId).setFeUnitChannel(0).getExternalFedFeUnit());

    childElement->setAttribute(X("id"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
    numberString.str("");  //Clear stream ready for next input.


    //Initialize a Fed9UTempControl object from the Fed9UDescription
    Fed9UTempControl theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);

    //Set the max temp for the LM82 chip on the FE FPGA
    numberString << theFed9UTempControl.getLm82High();
    childElement->setAttribute(X("maxTempLm82"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the max temp for the FE FPGA
    numberString << theFed9UTempControl.getFpgaHigh();
    childElement->setAttribute(X("maxTempFpga"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the critical temp for the FE FPGA
    numberString << theFed9UTempControl.getCritical();
    childElement->setAttribute(X("maxTempCritical"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the enable/disable state
    fedParamBool = theFed9UDescription.getFedFeUnitDisable(theFed9UAddress);
    if (fedParamBool==false) {
      childElement->setAttribute(X("state"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("state"), X("DISABLE")); 
    }

    //Set the OptoRx input offset
    numberString << theFed9UDescription.getOptoRxInputOffset(theFed9UAddress);
    childElement->setAttribute(X("optoRxInputOffset"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the OptoRx output offset
    numberString << theFed9UDescription.getOptoRxOutputOffset(theFed9UAddress);
    childElement->setAttribute(X("optoRxOutputOffset"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the  OptoRx capacitor value.
    numberString << theFed9UDescription.getOptoRxCapacitor(theFed9UAddress);
    childElement->setAttribute(X("optoRxCapacitor"), X((numberString.str()).c_str())); 
    numberString.str(""); 
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                                                         
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }


  void Fed9UDescriptionToXml::addChannelPairAttributes(DOMElement* childElement, unsigned short channelPairId) throw (Fed9UXMLDescriptionException)
  {
    try {
      bool fedParamBool = false;
      ostringstream numberString;
      
      //Set channel pair id 
      numberString << static_cast<u16>(theFed9UAddress.setFeUnitChannelPair(channelPairId).getExternalFeUnitChannelPair());
      childElement->setAttribute(X("id"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
      numberString.str("");  //Clear stream ready for next input.

      //set the fakeEventRandomSeed
      numberString << theFed9UDescription.getFakeEventRandomSeed(theFed9UAddress);
      childElement->setAttribute(X("fakeEventRandomSeed"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
      numberString.str("");  //Clear stream ready for next input.

      //set the fakeEventRandomMask
      numberString << theFed9UDescription.getFakeEventRandomMask(theFed9UAddress);
      childElement->setAttribute(X("fakeEventRandomMask"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
      numberString.str("");  //Clear stream ready for next input.



    }   
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }

  }
  

  void Fed9UDescriptionToXml::addChannelAttributes(DOMElement* childElement, unsigned short channelId) throw (Fed9UXMLDescriptionException)
  {
    try {
      bool fedParamBool = false;
      ostringstream numberString;
      
      //Set channel id 
      numberString << static_cast<u16>(theFed9UAddress.setFeUnitChannel(channelId).getExternalFeUnitChannel());
      childElement->setAttribute(X("id"), X((numberString.str()).c_str()));  //Have to convert to a char string before passing to XStr
      numberString.str("");  //Clear stream ready for next input.
      
      //Set the coarse delay
      numberString << theFed9UDescription.getCoarseDelay(theFed9UAddress);
      childElement->setAttribute(X("coarseDelay"), X((numberString.str()).c_str())); 
      numberString.str(""); 
      
      //Set the fine delay
      numberString << theFed9UDescription.getFineDelay(theFed9UAddress);
      childElement->setAttribute(X("fineDelay"), X((numberString.str()).c_str())); 
      numberString.str(""); 
      
      //Set the frame threshold
      numberString << theFed9UDescription.getFrameThreshold(theFed9UAddress);
      childElement->setAttribute(X("threshold"), X((numberString.str()).c_str())); 
      numberString.str(""); 
      
      //Set the TrimDAC offset.
      numberString << theFed9UDescription.getTrimDacOffset(theFed9UAddress);
      childElement->setAttribute(X("trimDacOffset"), X((numberString.str()).c_str())); 
      numberString.str(""); 
      
      //Set the  ADC input range mode type
      Fed9UAdcControls theFed9UAdcControls = theFed9UDescription.getAdcControls(theFed9UAddress);
      bool adcParam[4] = {false};
      adcParam[0] = theFed9UAdcControls._dfsen;
      adcParam[1] = theFed9UAdcControls._dfsval;
      adcParam[2] = theFed9UAdcControls._s1;
      adcParam[3] = theFed9UAdcControls._s2;

      if ((adcParam[0]==true)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
	childElement->setAttribute(X("adcInputRangeMode"), X("1V")); 
      }
      else if ((adcParam[0]==false)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
	childElement->setAttribute(X("adcInputRangeMode"), X("2V")); 
      }
      else if ((adcParam[0]==false)&&(adcParam[1]==false)&&(adcParam[2]==false)&&(adcParam[3]==false)) {
	childElement->setAttribute(X("adcInputRangeMode"), X("DISABLE")); 
      }
      
      //Set the data complement flag (note the reverse logic - in Fed9UDescription, and Fed9UVmeDevice, a true will disable the complement, and vice-versa). 
      fedParamBool = theFed9UDescription.getComplement(theFed9UAddress);
      if (fedParamBool==false) {
	childElement->setAttribute(X("dataComplement"), X("ENABLE")); 
      }
      else if (fedParamBool==true) {
	childElement->setAttribute(X("dataComplement"), X("DISABLE")); 
      }
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }


  void Fed9UDescriptionToXml::addApvAttributes(DOMElement* childElement, unsigned short apvId) throw (Fed9UXMLDescriptionException)
  {
    try {
    bool fedParamBool = false;
    ostringstream numberString;

    //Set apv id 
    numberString << static_cast<u16>(theFed9UAddress.setChannelApv(apvId).getExternalChannelApv());
    childElement->setAttribute(X("id"), X((numberString.str()).c_str())); 
    numberString.str("");  

    //Set the Fed9UAddress using the the apvId
    theFed9UAddress.setChannelApv(apvId);

    //Set the enable/disable state
    fedParamBool = theFed9UDescription.getApvDisable(theFed9UAddress);
    if (fedParamBool==false) {
      childElement->setAttribute(X("state"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("state"), X("DISABLE")); 
    }

    // <GR date="27/07/06"> Blah
    //Set the APV Fake Event enable/disable state
    fedParamBool = theFed9UDescription.getApvFakeEventDisable(theFed9UAddress);
    if (fedParamBool==false) {
      childElement->setAttribute(X("fakeEventState"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("fakeEventState"), X("DISABLE")); 
    }
    // </GR>

    //Set the median override value
    numberString << theFed9UDescription.getMedianOverride(theFed9UAddress);
    childElement->setAttribute(X("medianOverride"), X((numberString.str()).c_str())); 
    numberString.str(""); 
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                                                         
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }


  void Fed9UDescriptionToXml::addStripAttributes(DOMElement* childElement, unsigned short stripId) throw (Fed9UXMLDescriptionException)
  {
    try {
    bool fedParamBool = false;
    ostringstream numberString;

    //Set strip id 
    numberString << static_cast<u16>(theFed9UAddress.setApvStrip(stripId).getExternalApvStrip());
    childElement->setAttribute(X("id"), X((numberString.str()).c_str())); 
    numberString.str("");  

    //Set the Fed9UAddress using the the stripId, and get the corresponding Fed9UStripDescription from Fed9UDescription.
    theFed9UAddress.setApvStrip(stripId);
    const Fed9UStripDescription theFed9UStripDescription = theFed9UDescription.getFedStrips().getStrip(theFed9UAddress);

    //Set the enable/disable state
    fedParamBool = theFed9UStripDescription.getDisable();
    if (fedParamBool==false) {
      childElement->setAttribute(X("state"), X("ENABLE")); 
    }
    else if (fedParamBool==true) {
      childElement->setAttribute(X("state"), X("DISABLE")); 
    }

    //Set the pedestal
    numberString << theFed9UStripDescription.getPedestal();
    childElement->setAttribute(X("ped"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the low cluster threshold
    numberString << theFed9UStripDescription.getLowThresholdFactor();
    childElement->setAttribute(X("low"), X((numberString.str()).c_str())); 
    numberString.str(""); 

    //Set the high cluster threshold
    numberString << theFed9UStripDescription.getHighThresholdFactor();
    childElement->setAttribute(X("high"), X((numberString.str()).c_str())); 
    numberString.str("");

    //Set the high cluster threshold
    numberString << theFed9UStripDescription.getNoise();
    childElement->setAttribute(X("noise"), X((numberString.str()).c_str())); 
    numberString.str("");
    }
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }


  void Fed9UDescriptionToXml::addStripsAttribute(DOMElement* childElement) throw (Fed9UXMLDescriptionException)
  {
    try {


      char stripsBuf[STRIPS_PER_APV*4];
      getStripDataBuffer(stripsBuf);

//       // now we must loop over all strips building all the info for each 
//       // strip (pedestal, lowthresholdfactor, highthresholdfactor, noise, disable)
//       // into one blob. To do this we use a stringstream streaming in values as hex
//       // before that we have to limit the values to the correct precision and number of bits before constructing the binary buffer
//       // each strip will contain the data in the following format:
//       //
//       // *****************************************
//       // * Value       *   bits   *   range      *
//       // *****************************************
//       // * Pedestal    *   [31-22]*   0-1023     *
//       // * Noise       *   [21-13]* 0.0-51.1(0.1)*
//       // * High Factor *   [12-7] * 0.0-12.6(0.2)*
//       // * Low Factor  *   [6-1]  * 0.0-12.6(0.2)*
//       // * Disable     *   [0]    * 0,1          *
//       // *****************************************
//       //
      
//       u32 stripData;
//       u32 low = 0;
//       u32 high = 0;
//       u32 ped = 0;
//       u32 noise = 0;
//       const Fed9UStripDescription * theFed9UStripDescription; 
//       char stripsBuf[STRIPS_PER_APV*4];
//       memset(stripsBuf,1,STRIPS_PER_APV*4);

//       //charData.str("");
//       //charData << std::hex << std::setfill('0') << std::setw(8);
//       for (int i=0;i<STRIPS_PER_APV;i++) {
// 	//stripData = 0;
// 	//Set the Fed9UAddress using the the stripId, and get the corresponding Fed9UStripDescription from Fed9UDescription.
// 	theFed9UAddress.setApvStrip(i);
// 	theFed9UStripDescription = &theFed9UDescription.getFedStrips().getStrip(theFed9UAddress);
	
// 	low = (static_cast<u32>(theFed9UStripDescription->getLowThresholdFactor()*5.0 + 0.5) ) & 0x3F; 
// 	high = (static_cast<u32>(theFed9UStripDescription->getHighThresholdFactor()*5.0 + 0.5) ) & 0x3F;
// 	noise = static_cast<u32>(theFed9UStripDescription->getNoise()*10.0 + 0.5) & 0x01FF;
// 	ped = static_cast<u32>(theFed9UStripDescription->getPedestal()) & 0x03FF;
	
// 	(reinterpret_cast<u32*>(stripsBuf))[i] = (ped << 22) | (noise << 13) | (high << 7) | (low << 1) | ( theFed9UStripDescription->getDisable() ? 0x1 : 0x0 );
// 	//	if( i == 0 ) {
// 	// std::cout << "the strip data is :" << endl;
// 	//std::cout << std::dec << "low = " << low << ", high= " << high << ", noise= " << noise << ", ped= " << ped << std::hex << std::setfill('0') << std::setw(8) << stripData << " " << endl;
// 	//}
// 	//charData << std::hex << std::setfill('0') << std::setw(8) << stripData;
	
//       }
//       //      stripsBuf[0] = 'H';


      XMLSize_t outputlength;
      XMLSize_t length = 512;
      XMLByte* encodedXML;

      //std::cout << "Base 64 encoded strips string = " <<  reinterpret_cast<char*>(Base64::encode(reinterpret_cast<const XMLByte*>(stripsBuf),length,&outputlength))  
      //		<< "length = " << outputlength << std::endl;
      childElement->setAttribute(X("data"), X(reinterpret_cast<char*>(encodedXML=Base64::encode(reinterpret_cast<const XMLByte*>(stripsBuf),length,&outputlength))));
      delete encodedXML;
    } 
    catch (const DOMException& e) {
      ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  
  }


  void Fed9UDescriptionToXml::getStripDataBuffer(char * stripsBuf) throw (Fed9UXMLDescriptionException) {
    // now we must loop over all strips building all the info for each 
    // strip (pedestal, lowthresholdfactor, highthresholdfactor, noise, disable)
    // into one blob. To do this we use a stringstream streaming in values as hex
    // before that we have to limit the values to the correct precision and number of bits before constructing the binary buffer
    // each strip will contain the data in the following format:
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
      u32 stripData;
      u32 low = 0;
      u32 high = 0;
      u32 ped = 0;
      u32 noise = 0;
      const Fed9UStripDescription * theFed9UStripDescription; 
      memset(stripsBuf,1,STRIPS_PER_APV*4);

      //charData.str("");
      //charData << std::hex << std::setfill('0') << std::setw(8);
      for (int i=0;i<STRIPS_PER_APV;i++) {
	//stripData = 0;
	//Set the Fed9UAddress using the the stripId, and get the corresponding Fed9UStripDescription from Fed9UDescription.
	theFed9UAddress.setApvStrip(i);
	theFed9UStripDescription = &theFed9UDescription.getFedStrips().getStrip(theFed9UAddress);
	
	low = (static_cast<u32>(theFed9UStripDescription->getLowThresholdFactor()*5.0 + 0.5) ) & 0x3F; 
	high = (static_cast<u32>(theFed9UStripDescription->getHighThresholdFactor()*5.0 + 0.5) ) & 0x3F;
	noise = static_cast<u32>(theFed9UStripDescription->getNoise()*10.0 + 0.5) & 0x01FF;
	ped = static_cast<u32>(theFed9UStripDescription->getPedestal()) & 0x03FF;
	
	(reinterpret_cast<u32*>(stripsBuf))[i] = (ped << 22) | (noise << 13) | (high << 7) | (low << 1) | ( theFed9UStripDescription->getDisable() ? 0x1 : 0x0 );

	//	std::cout << std::dec << "strip = " << i << ", low = " << low << ", high = " << high << ", noise representation = " << noise << ", actual noise = " << theFed9UStripDescription->getNoise() << ", ped = " << ped << ", strip Data in Hex = 0x" << std::hex << std::setfill('0') << std::setw(8) << (reinterpret_cast<u32*>(stripsBuf))[i] << " " << endl;
	
		
      }
    } catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown exception."));
    }
  }
  

  bool Fed9UDescriptionToXml::getUsingStrips() {
    return usingStrips_;
  }


  void Fed9UDescriptionToXml::setUsingStrips(bool usingStrips) {
    usingStrips_ = usingStrips;
  }

  bool Fed9UDescriptionToXml::getUsingBinaryStrips() {
    return usingBinaryStrips_;
  }


  void Fed9UDescriptionToXml::setUsingBinaryStrips(bool usingBinaryStrips) {
    std::cout << "in Desc to XML : usingBinaryStrips_ = " << (usingBinaryStrips_?"true":"false") << std::endl;
    usingBinaryStrips_ = usingBinaryStrips;
  }


  void Fed9UDescriptionToXml::setDebugOutput(bool value) {
    debugOutput_ = value;
    std::cout << "debug output = " << (debugOutput_ ? "true":"false") << std::endl;
  }
}

#endif
#endif
