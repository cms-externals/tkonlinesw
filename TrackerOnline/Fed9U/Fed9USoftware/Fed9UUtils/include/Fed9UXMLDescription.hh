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

#ifndef _Fed9UXMLDescription_H_
#define _Fed9UXMLDescription_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Xerces headers
#include <xercesc/dom/DOM.hpp>

//Fed9U headers
#include "Fed9UDescription.hh"
#include "Fed9UXMLDescriptionException.hh"
#include "Fed9UDOMCountErrorHandler.hh"

#include <string>

namespace Fed9U {

  //using xercesc::DOMNode;
  //using xercesc::DOMNamedNodeMap;
  //using xercesc::DOMDocument;
  //using xercesc::DOMInputSource;
  //using xercesc::DOMBuilder;

  //using.*std::istream;
  //using.*std::string;

class Fed9UXMLDescription
{

public:
  /**Constructor. This takes as arguments the XML file name and the Fed9UDescription to be modified.*/
  Fed9UXMLDescription(const std::string &XMLOverideFile, Fed9UDescription &defaultFed9UDescription, std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) noexcept(false);

  /**Constructor. This takes in the Fed9UDescription to be modified. This is the constructor that should be used by derived classes.*/
  Fed9UXMLDescription(Fed9UDescription &defaultFed9UDescription , std::vector<Fed9UDescription*>* desclist = NULL, bool usingStrips = true) noexcept(false);

  /**Destructor. This does some clean up.*/
  virtual ~Fed9UXMLDescription();

  /**Method which performs the Fed9UDescription override. It modifies the object member reference to the Fed9UDescription. 
     This method should only be used if the user has used the constructor which takes in a file name.*/
  std::vector<Fed9UDescription*>* makeNewFed9UDescription(void) noexcept(false);

  /**Method which performs the Fed9UDescription overide. 
     It takes in a istream, containing the XML buffer, and modifies the object member reference to the Fed9UDescription.
     This method should be used by derived classes which use the stream interface to this class.*/
  std::vector<Fed9UDescription*>* makeNewFed9UDescriptionFromStream(std::istream &is) noexcept(false);

  /**Method to get the value of usingStrips_*/
  bool getUsingStrips();

  /**Method to set the value of usingStrips_*/
  void setUsingStrips(bool usingStrips=true);
  
  /** Method to set the value of debugOutput_. If this is set to true the the buffer will be displayed to standard output for debugging */
  void setDebugOutput(bool value=true);

protected:
  /**Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.*/
  void initializeXerces(void) noexcept(false);

  /**Method which will construct and initialize the DOMBuilder.*/
  void makeDOMBuilder(void) noexcept(false);

  /**Method to recursively find the child elements under the given Node.*/
  void traverseChildElements(xercesc::DOMNode *n) noexcept(false);

  /**Method which reads in a node, finds if it has attributes and sets these in the Fed9UDescription.*/
  void setFed9UDescription(xercesc::DOMNode *n) noexcept(false);

  /**Method which will set the attributes for the fed9U node.*/
  void setFed9UAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);

  /**Method which will set the attributes for the feFpga node.*/
  void setFeFpgaAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);
 
  /**Method which will set the attributes for the channel pair node.*/
  void setChannelPairAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);
 
  /**Method which will set the attributes for the channel node.*/
  void setChannelAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);
  
  /**Method which will set the attributes for the channel node.*/
  void setApvAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);

  /**Method which will set the attributes for the strip node.*/
  void setStripAttributes(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);
  
  /**Method which will set the attributes for the strips node.*/
  void setStripsAttribute(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap) noexcept(false);

  /**Method to get the integer node value from a DOMNamedNodeMap*/
  unsigned long getIntNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) noexcept(false);

  /**Method to get the signed integer node value from a DOMNamedNodeMap*/
  int getSignedIntNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap,  std::string theAttributeName, bool &notFound) noexcept(false);

  /**Method to get the float node value from a DOMNamedNodeMap*/
  float getFloatNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) noexcept(false);

  /**Method to get the string node value from a DOMNamedNodeMap*/
  std::string getStringNodeValue(xercesc::DOMNamedNodeMap *theAttributesDOMNamedNodeMap, std::string theAttributeName, bool &notFound) noexcept(false);
  
  
  xercesc::DOMDocument *doc;
  xercesc::DOMLSParser *theDOMBuilder;
  Fed9UDOMCountErrorHandler errorHandler;

  std::string theXMLOverideFile;
  Fed9UAddress theFed9UAddress;
  Fed9UDescription &theFed9UDescription;
  std::vector<Fed9UDescription*> * theFed9UDescriptionList_; // this vector will contain the descriptions if there are more than one.
  u32 numberOfFeds_;
  unsigned short countApvs;
  bool usingStrips_;
  bool debugOutput_;
};


}
#endif
#endif
#endif
