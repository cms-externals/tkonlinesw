#ifndef _Fed9UDescriptionToXml_H_
#define _Fed9UDescriptionToXml_H_

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

//Standard headers
#include<string>

namespace Fed9U {

//   using xercesc::DOMDocument;
//   using xercesc::DOMWriter;
//   using xercesc::DOMElement;

  //using.*std::string;

  /**
   * \brief  Class which will write a Fed9UDescription to stream in XML format.
   * \author Matthew Pearson, RAL
   * \date   17/02/2004
   *
   * This class uses the Xerces 2.3 library. The input to the constructor is a reference to a Fed9UDescription.
   * This is then converted to an XML format stream using the streamOutDescription method. The XML stream will conform to
   * the XML schema for the Fed9USoftware. This class is intended to be a base class - a derived class can implement a
   * file/database/etc interface to the stream interface provided by this class.
   */
  class Fed9UDescriptionToXml
  {
  public:

    /*\name Constructors and destructors*/
    //@{

    /**
     * \brief Constructor. Writes a single description to a User defined XML file.
     * \param targetXMLFileName Name of the file to be written to.
     * \param Fed9UDescriptionToWrite Reference to the Fed9UDescription object that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This constructor can be used to directly create an XML file.
     */
    Fed9UDescriptionToXml(const std::string &targetXMLFileName,  Fed9UDescription &Fed9UDescriptionToWrite, bool usingStrips = true, bool usingBinaryStrips = true) noexcept(false);

    /**
     * \brief Constructor. Takes a Fed9UDescription object to write to the database.
     * \param Fed9UDescriptionToWrite Reference to Fed9UDescription object to be written.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * As no file name is set during construction this cannot be used to make an XML file directly and is for use with the database.
     */
    Fed9UDescriptionToXml(Fed9UDescription &Fed9UDescriptionToWrite , bool usingStrips = true, bool usingBinaryStrips = true) noexcept(false);

    /**
     * \brief Constructor. This takes a vector of Fed9UDescription pointers from which to make the XML file.
     * \param targetXMLFileName Name of the XML file to which the XML description is to be written.
     * \param Fed9UDescriptionToWrite Vector of Fed9UDescription pointers, each element contains a pointer to a Fed9UDescription object
     *        that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * All the Fed9UDescription objects pointed to by the elements of the Fed9UDescription vector will be written to the single file specified
     * by targetXMLFileName. Sufficient information is given to this constructor to produce an XML file.
     */
    Fed9UDescriptionToXml(const std::string &targetXMLFileName,  std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite , bool usingStrips = true, bool usingBinaryStrips = true) noexcept(false);

    /**
     * \brief Constructor. This takes a vector of Fed9UDescription pointers to write to the database.
     * \param Fed9UDescriptionToWrite Vector of Fed9UDescription pointers, each element contains a pointer to a Fed9UDescription object
     *        that is to be written to file.
     * \param usingStrips If boolean true, it will write the strip information as a binary blob in the strip tag, rather than each
     *        strip parameter with its own tag, if false. Defaulted to true.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * Insufficient information is passed to the constructor to create an XML file, no file name is given. It is instead used to writing the  
     * to the database. All objects pointed to by the Fed9UDescription vector will be written to the database.
     */
    Fed9UDescriptionToXml( std::vector<Fed9UDescription*>  Fed9UDescriptionsToWrite , bool usingStrips = true, bool usingBinaryStrips = true) noexcept(false);

    /**
     * \brief Destructor.
     */
    virtual ~Fed9UDescriptionToXml();
    //@}


    /**
     * \brief Method which converts the Fed9UDescription to an XML buffer stream.
     * \param os Pointer to the output stream to write the XML data.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This must be called after using the constructor to this class which only takes in a reference to a Fed9UDescription and not an XML file name.
     */
    void streamOutDescription(std::ostream * os) noexcept(false);

    /**
     * \brief Method which converts the Fed9UDescription to an XML file.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     *
     * This must be called after using the file based constructor to this class.
     */
    void writeXmlFile(void) noexcept(false);

    /**
     * \brief  Whether to write the strip data into the  XML or not.
     * \return bool True if writing strip data to xml, false if not.
     */
    bool getUsingStrips();

    /**
     * \brief Whether to write the strip data to the xml.
     * \param usingStrips If set true then strip data is written, false not written. Defaulted to true.
     */
    void setUsingStrips(bool usingStrips=true);

    /**
     * \brief  Whether to write the strip data as a binary data chunk with a single XML tag for all strips or to write each bit of individual strip data
     *         to its own tag.
     * \return bool True if writing binary data, false if human readable.
     */
    bool getUsingBinaryStrips();

    /**
     * \brief Whether to write the strip data as a binary data chunk with a single XML tag for all strips or to write each bit of individual strip data
     *        to its own tag.
     * \param usingStrips If set true then binary data is written, false it is human readable. Defaulted to true.
     */
    void setUsingBinaryStrips(bool usingBinaryStrips=true);

    /**
     * \brief Debug print statements can be optionally sent to the standard cout stream.
     * \param value If true debug statements will be printed, false they will not. Defaulted to true.
     */
    void setDebugOutput(bool value=true);

  protected:

    /**
     * \brief Method which constructs the DOM document.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    virtual void constuctDOMDocument(void) noexcept(false);

    /**
     * \brief Method to initialize the Xerces XML parser. This method must be called before using any Xerces APIs.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void initializeXerces(void) noexcept(false);

    /**
     * \brief Method which adds elements to a DOMDocument using the Fed9UDescription to get the attribute values.
     * \param rootElement Pointer to DOM element that will contain the data to be added to the DOM document.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    virtual void addDOMElements(xercesc::DOMElement *rootElement) noexcept(false);

    /**
     * \brief Method which extracts the FED parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains FED parameters.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addFedAttributes(xercesc::DOMElement* childElement) noexcept(false);

    /**
     * \brief Method which extracts the FE unit parameters from the Fed9UDescription and adds them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains FE unit parameters.
     * \param feId ID of the FE unit that is being addressed. Range:0-7, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addFeFpgaAttributes(xercesc::DOMElement* childElement, unsigned short feId=0) noexcept(false);


    /**
     * \brief Method which extracts the channel Pair parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains channel pair parameters.
     * \param channelPairId ID of the FE unit channel pair whose parameters are being added. Range:0-5, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addChannelPairAttributes(xercesc::DOMElement* childElement, unsigned short channelPairId) noexcept(false);

    /**
     * \brief Method which extracts the channel parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains channel parameters.
     * \param channelId ID of the FE unit channel whose parameters are being added. Range:0-11, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addChannelAttributes(xercesc::DOMElement* childElement, unsigned short channelId=0) noexcept(false);

    /**
     * \brief Method which extracts the APV parameters from the Fed9UDescription and add them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains APV parameters.
     * \param apvId ID of the channel APV whose parameters are being added. Range: 0-1, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addApvAttributes(xercesc::DOMElement* childElement, unsigned short apvId=0) noexcept(false);

    /**
     * \brief Method which extracts the strip parameters from the Fed9UDescription and adds them as element attributes.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains strip parameters.
     * \param stripId ID of the APV strip whose parameters are being added. Range: 0-127, internal numbering, defaulted to 0.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     *        by other functions during the construction call.
     */
    void addStripAttributes(xercesc::DOMElement* childElement, unsigned short stripId=0) noexcept(false);
  
    /**
     * \brief Method which extracts all the APV strip parameters from the Fed9UDescription and adds them as a single element attribute.
     * \param childElement Pointer to the element, which is a subset of the root element that will be added to the DOM document. Contains strip parameters.
     * \throw Fed9UXMLDescriptionException Throw for any error conditions that occur during construction, also rethrows any errors thrown.
     */
    void addStripsAttribute(xercesc::DOMElement* childElement) noexcept(false);

    /**
     * \brief All the strip data on an APV can be encoded as a single chunk of binary data. This is in a non human readable form.
     *        This is used to return it in a human readable form.
     * \param stripsBuf A character point that will be filled with the strip data in a human readable form.
     */
    void getStripDataBuffer(char * stripsBuf) noexcept(false);

    xercesc::DOMDocument *doc;                                    //!< DOM document where the XML data is to be stored.
    xercesc::DOMLSSerializer *theDOMWriter;                             //!< DOM writer object to write the data in the XML format.
    xercesc::DOMLSOutput *theDOMOutput;
    Fed9UDOMCountErrorHandler errorHandler;              //!< Fed9U DOM error handler used to detect errors and print the error strings.
    std::string theTargetXMLFileName;                         //!< Name of the XML file to be written to.
    Fed9UDescription &theFed9UDescription;               //!< Fed9UDescription object that is currently being dealt with.
    std::vector <Fed9UDescription*>  theFed9UDescriptionList; //!< Vector of all the pointers Fed9UDescription object to be processed.
    Fed9UAddress theFed9UAddress;                        //!< Fed9UAddress object used to for accessing the various parameters in the Fed9UDescription.
    bool usingStrips_;                                   //!< If true then the strip information for an APV is written as binary, false human readable.
    bool debugOutput_;                                   //!< If true then the debug printout is displayed during member functions calls that contain debug printout.
    bool usingBinaryStrips_;                             //!< If true then the xml buffer created puts the strips data into a binary blob, one for each APV. if false we have an xml tag per strip.
  };

}

#endif

#endif

#endif
