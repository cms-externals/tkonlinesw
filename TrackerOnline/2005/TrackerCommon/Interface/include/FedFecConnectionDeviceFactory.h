/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003
*/
#ifndef FEDFECCONNECTIONDEVICEFACTORY_H
#define FEDFECCONNECTIONDEVICEFACTORY_H

#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif
#include <iostream>
#include <fstream>
#include <sstream>

// use in XMLFec to declare the type XMLURL
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>

// use in XMLFec to declare XMLReaderFactory
#include <xercesc/sax2/XMLReaderFactory.hpp>

// declaration of the attribute SAX2XMLReader* parser_
#include <xercesc/sax2/SAX2XMLReader.hpp>

#include "xercesc/sax/InputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "xercesc/dom/DOM.hpp"

#ifdef DATABASE
#  include "DbConnectionsAccess.h"
#endif

// Parser for the Tracker
#include "TrackerParser.h"

// String conversion
#include "stringConv.h"
/** This class manage all CONNECTION descriptions:
 * It is mandatory the XML schema merge all the XML description already defined for the FEC and the CONNECTION
 */
class FedFecConnectionDeviceFactory  : public DeviceFactoryInterface {

 private:

  /** Connection version
   */
  unsigned int versionMajor_, versionMinor_ ;

  /** Version already initialised
   */
  bool initDbVersion_ ;

  TrackerParser *trackerParser_;
  std::string localPartitionName_;
  //   DbConnectionsAccess *dbAccess_;
  //   std::string outputFileName_;




  //   /** CONNECTION version
  //    */
  //   unsigned int versionConnectionMajor_, versionConnectionMinor_ ;
 

  

  // -----------------------------------------------------------------------------------------------------
  // Private method to get a buffer from the database and to set a buffer to the database
  // -----------------------------------------------------------------------------------------------------

  xercesc::DOMBuilder *makeDOMBuilder( ) {

    xercesc::DOMBuilder *theDOMBuilder ;

    try {
      XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& e) {
      cout << "ERROR during Xerces initialization! :\n" << e.getMessage() << endl;
    }
  
    XMLCh tempStr[100];
    xercesc::XMLString::transcode("LS", tempStr, 99);
    xercesc::DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
    //DOMBuilder* theDOMBuilder = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    theDOMBuilder = ((xercesc::DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, NULL);
    
    //Set the next 3 options to false if one does not want to use automatic XML Schema verifying.
    theDOMBuilder->setFeature(XMLUni::fgDOMNamespaces, false);
    theDOMBuilder->setFeature(XMLUni::fgXercesSchema, false);
    theDOMBuilder->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
  
    /*Do we want to enable validatioDOMBuilder *theDomBuilder;n error reporting?
      fgDOMValidateIfSchema will enable it if a schema is specified.
      fgDOMValidation will disable/disable it.
      Note: The schema file must have the same name as the XML file, but with a .xsd extension. */
    theDOMBuilder->setFeature(XMLUni::fgDOMValidateIfSchema, false);   
    //theDOMBuilder->setFeature(XMLUni::fgDOMValidation, false);
    //theDOMBuilder->setFeature(XMLUni::fgDOMValidation, true);
  
    return theDOMBuilder ;
  }

#ifdef DATABASE
  /** Upload a DOMInputSource to the database
   * This method will create for each connection (retreive from the Tracker parser) the corresponding string that will be uploaded finally to the database
   * The DTD must not be specified at this point
   */
  void setDatabaseDOMInputSource ( std::string partitionName, bool majorVersion = true ) throw (oracle::occi::SQLException) {


    // database connection
    if (dbAccess_ != NULL) {

      if (trackerParser_->getNumberOfFedChannel() != 0) {
	std::string buffer="<?xml version=\"1.0\" ?><ROWSET>";
	//std::string buffer="<?xml version=\"1.0\" ?><TrackerDescription><FedChannelList>";
	//std::string buffer="<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?><TrackerDescription><FedChannelList>";
	// for each connection
	for (int i = 0 ; i < trackerParser_->getNumberOfFedChannel(); i ++) {

	  // Retreive the connection
	  FedChannelConnectionDescription *connection = trackerParser_->getFedChannelConnection(i) ;
	  // Build the buffer dedicated to the database
	  // std::cout << "<RAWCONNECTION><FEDID>" << toString(connection->getFedId()) << "</FEDID><FEDCHANNEL>" << toString(connection->getFedChannel()) << "</FEDCHANNEL><APVADDRESS>" << toString(connection->getApv()) << "</APVADDRESS><DCUHARDID>" << toString(connection->getDcuHardId()) << "</DCUHARDID><ENABLED>" << (connection->getEnabled() ? "T": "F") << "</ENABLED></RAWCONNECTION>" ;

	  buffer += "<RAWCONNECTION><FEDID>" + toString(connection->getFedId()) + "</FEDID><FEDCHANNEL>" + toString(connection->getFedChannel()) +
	    "</FEDCHANNEL><APVADDRESS>" + toString(connection->getApv()) + "</APVADDRESS><DCUHARDID>" + toString(connection->getDcuHardId()) + "</DCUHARDID><ENABLED>" + (connection->getEnabled() ? "T": "F") + "</ENABLED></RAWCONNECTION>" ;

	}
      
	// end of the buffer
	buffer += "</ROWSET>" ;
      
	//buffer += "</TrackerDescription></FedChannelList>" ;
	
	//std::cout << buffer << std::endl ;
	//cout << "Real upload" <<endl;
        DbConnectionsAccess* d = (DbConnectionsAccess*)  dbAccess_;
	try 
	  {
	    //             XML buffer, partition name, major or minor version
	    std::cout << "Upload buffer in database: " << majorVersion << " for partition " << partitionName << std::endl ;

	    d->setXMLClob (buffer,partitionName,majorVersion);
	  }
	catch ( oracle::occi::SQLException &e)
	  {
	    cout << "Error in upload" << e.what() <<endl;
	    std::string errorMessage = e.getMessage();
	    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
	      errorMessage = d->getErrorMessage();
	      std::cout << errorMessage << std::endl;
	    }
	  }
	catch (...)
	  {
	    cout <<" Pourqoui moi........................"<<endl;
	  }
#ifdef DATABASEDEBUG
	std::cout << "data uploaded !!! " << std::endl;
#endif
      }
      else {

#ifdef DATABASEDEBUG
	cerr << "No connection is available" << endl ;
#endif
	//throw (exception) ;
      }
    }
    else {

#ifdef DATABASEDEBUG
      cerr << "No database access created" << endl ;
#endif
      //throw (exception) ;
    }
  }

  /** Retreive a DOMInputSource from the database
   * \param partitionName - name of the partition
   * \param dbAccess - database access
   * \warning this method initialize xerces
   */
  xercesc::DOMInputSource *getDatabaseDOMInputSource( std::string partitionName, unsigned int versionMajorId = 0, unsigned int versionMinorId = 0 )
    throw (exception) {    

    xercesc::DOMInputSource* domInputSource = NULL ;
    //    XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = NULL;
  
    if (dbAccess_ != NULL) {
    
      //download the fed parameters from the database
      XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = NULL;
    
      oracle::occi::Clob *xmlClob = NULL;
      XMLByte *xmlBuffer = NULL ;

      if ( (versionMajorId != 0) && (versionMinorId != 0) )
	xmlClob = ((DbAccess *)dbAccess_)->getXMLClobWithVersion(partitionName,versionMajorId,versionMinorId) ;
      else
	xmlClob = ((DbAccess *)dbAccess_)->getXMLClob(partitionName) ;

      // Retreive the buffer in database
      if (xmlClob != NULL){

	unsigned int resultLength = (*xmlClob).length();

#ifdef DATABASEDEBUG
	std::cout << "resultLength : " << resultLength << std::endl;
#endif

	if (resultLength > 0) {

	  if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char))) != NULL) {

	    if (resultLength == (*xmlClob).read(resultLength, xmlBuffer, resultLength+1)){

	      const std::string xmlBufferId = "myXmlBuffer";

	      // create an InputSource from the buffer
	      xmlInputSource = new XERCES_CPP_NAMESPACE::MemBufInputSource ( (const XMLByte*)xmlBuffer, strlen((const char*)xmlBuffer), (const XMLCh *)xmlBufferId.c_str(), false, XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);
	      // convert the input source to a DOMInputSource  
	      domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;

#ifdef DATABASEDEBUG	    
	      std::cout << xmlBuffer << std::endl;
#endif
	    }
	    else {
#ifdef DATABASEDEBUG
	      std::cerr << "failed while reading the XML Clob"  << std::endl;
#endif
	      //throw exception ;
	    }
	  }
	  else {
#ifdef DATABASEDEBUG
	    std::cerr << "failed during memory allocation"  << std::endl;  	
#endif
	    //throw exception ;
	  }
	}
	else {
#ifdef DATABASEDEBUG
	  std::cerr << "no data found... " << std::endl;
#endif
	  //throw exception ;
	}
      }
      else {

#ifdef DATABASEDEBUG
	std::cerr << "failed while trying to get the XML Clob"  << std::endl;
#endif
	//throw exception ;
      }
    }
    else {
#ifdef DATABASEDEBUG
      std::cerr << "Database is not connected" << std::endl ;
#endif
    }

    return domInputSource;
  }
#endif

  
 public:

  FedFecConnectionDeviceFactory(): DeviceFactoryInterface()
    {
      trackerParser_=NULL;
#ifdef DATABASE
      dbAccess_=NULL;
#endif
    }

  FedFecConnectionDeviceFactory( string login, string password, string path, bool threaded = false ): DeviceFactoryInterface(login,password,path,threaded)
    {
      trackerParser_=NULL;
#ifdef DATABASE
      dbAccess_=NULL;
      setFedFecConnectionDatabaseAccess ( login, password, path ) ;
#endif
    }
  
  ~FedFecConnectionDeviceFactory()
    {
      if (trackerParser_!=NULL)
	{
	  trackerParser_->purge();
	  delete trackerParser_;
	}
    }
  virtual void  createInputFileAccess ()
    {

      trackerParser_ = new TrackerParser();
#ifdef DATABASE
      dbAccess_=NULL;
#endif
    }

#ifdef DATABASE
  /**
   */
  void setFedFecConnectionDatabaseAccess ( string login, string password, string path ) throw ( oracle::occi::SQLException ) {

    // For database
    if (dbAccess_ != NULL)
      if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
	delete dbAccess_ ;
	dbAccess_ = NULL ;
      }

    if (dbAccess_ == NULL) {

      std::cout << "Create a new access to the FEC database for " << login << "/" << password << "@" << path << std::endl ;

      dbAccess_ = new DbConnectionsAccess (login, password, path, threaded_) ;

      deleteDbAccess_ = true ;
      useDatabase_ = true ;
    }
  }

  /**
   *
   */
  virtual void  createInputDBAccess ()
    {
      // Download from the database

      try {
	// initialize xerces
	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

	// create database access
	if (dbAccess_ == NULL)
	  dbAccess_ = new DbConnectionsAccess();


	// Create the parser
	trackerParser_ = new TrackerParser () ;

	deleteDbAccess_ = true ;
	useDatabase_ = true ;
      }
      catch  (oracle::occi::SQLException e) {
	std::cerr << e.getMessage() << std::endl;
      }
      catch (const XMLException& e) {
	std::cerr << "Error during parsing" << endl ;
      }
      catch (const DOMException& e) {
	std::cerr << "Error during parsing" << endl ;
      }
      catch (...) {
	std::cerr << "Unexpected exception during DOMBuilder init." << endl ;
      }



    }

  /**
   */
  virtual void setDatabaseAccess ( string login, string password, string path ) {

    try {
      // initialize xerces
      XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();

      // create database access
      dbAccess_ = new DbConnectionsAccess(login, password, path, threaded_);


      // Create the parser
      trackerParser_ = new TrackerParser () ;

      deleteDbAccess_ = true ;
      useDatabase_ = true ;
    }
    catch  (oracle::occi::SQLException e) {
      std::cerr << e.getMessage() << std::endl;
    }
    catch (const XMLException& e) {
      std::cerr << "Error during parsing" << endl ;
    }
    catch (const DOMException& e) {
      std::cerr << "Error during parsing" << endl ;
    }
    catch (...) {
      std::cerr << "Unexpected exception during DOMBuilder init." << endl ;
    }
  }

  /**
   */
  virtual void setDatabaseAccess ( )
    {
      createInputDBAccess ();
    }

  /**
   *
   * @param partition
   * @param major - if 0 is used then the current version is used
   * @param minor - if 0 is used then the current version is used
   */
  virtual void  setInputDBVersion (string partition, int major=0, int minor=0)
    {
      localPartitionName_ = partition;
      versionMajor_=major;
      versionMinor_=minor;
      // Download from the database
      try {

	std::cout << "****************************************************" << std::endl ;

	// Upload from the database
	xercesc::DOMInputSource *domInputSource = getDatabaseDOMInputSource ( partition ) ;


	std::cout << "****************************************************" << std::endl ;

	// crate the DOM builder
	xercesc::DOMBuilder *theDOMBuilder = makeDOMBuilder ( ) ;

	// Parse the DOM document
	DOMDocument *doc = theDOMBuilder->parse(*domInputSource) ;
	if (doc != NULL)
	  trackerParser_->parse (doc) ;
	else
	  std::cerr << "BUFFER NOT PARSED ERROR" << std::endl ;

	// display the connection
	cout << "Number of connections " << dec << trackerParser_->getNumberOfFedChannel() << endl ;

	// delete domInputSource or delete doc ?????
	delete doc ;

      }
      catch  (oracle::occi::SQLException e) {
	std::cerr << e.getMessage() << std::endl;
      }
      catch (const XMLException& e) {
	std::cerr << "Error during parsing" << endl ;
      }
      catch (const DOMException& e) {
	std::cerr << "Error during parsing" << endl ;
      }
      catch (...) {
	std::cerr << "Unexpected exception during DOMBuilder init." << endl ;
      }
    }
#endif

  /**
   *
   * @param name
   */
  virtual void  addFileName (string name)
    {
      trackerParser_->parseFile(name);
      cout << "Number of connections " << dec << trackerParser_->getNumberOfFedChannel() << endl ;

    }

  virtual void setInputFileName ( string inputFileName )
    {
      addFileName(inputFileName);
    }
  /**
   *
   * @param major
   * @param minor
   */
  virtual void  setOutputVersion (int major=0, int minor=0)
    {
      // To be discussed with Fred
      return;
    }

  /**
   *
   */
  virtual void  write ()
    {
      ofstream outf(outputFileName_.c_str());
      outf<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?><TrackerDescription><FedChannelList>" <<endl;;


      // for each connection
      for (int i = 0 ; i < trackerParser_->getNumberOfFedChannel(); i ++)
	trackerParser_->getFedChannelConnection(i)->toXML(outf); ;


      // end of the buffer
      outf<<"</FedChannelList></TrackerDescription>" <<endl;
    }

#ifdef DATABASE
  /**
   *
   */
  virtual void  upload ()
    {
      setDatabaseDOMInputSource(localPartitionName_);
    }

  /**
   *
   */
  virtual void  disableDB ()
    {
      if (dbAccess_!=NULL)
	delete dbAccess_;
      dbAccess_=NULL;
    }
#endif    
  
  /**
   * 
   * @param confdb 
   */
  virtual void  setConfDB (string confdb) 
    {
      setenv("CONFDB",confdb.c_str(),1);
    }
  
  /** \brief Retreive information for a given CONNECTION / version
   */
  int getNumberOfFedChannel() {return trackerParser_->getNumberOfFedChannel();}
  FedChannelConnectionDescription* getFedChannelConnection(int i) { return trackerParser_->getFedChannelConnection(i);}

  FedChannelConnectionDescription* getFedChannelConnectionByDcu(int dcu) { return 
									     trackerParser_->getFedChannelConnectionByDcu(dcu);}

  vector<FedChannelConnectionDescription*> getFedChannelListByDcu(int dcu) { 
    vector<FedChannelConnectionDescription*> list; 
    list.clear();
    for (int i=0;i<trackerParser_->getNumberOfFedChannel();i++)
      {
	if (trackerParser_->getFedChannelConnection(i)->getDcuHardId() == (unsigned int) dcu)
	  list.push_back(trackerParser_->getFedChannelConnection(i));
	
      }
    return list;
  }

  FedChannelConnectionDescription* getFedChannelConnection(int fedid,int fedch) {  return trackerParser_->getFedChannelConnection(fedid,fedch);
  }

  /**
   */
  void addFedChannelConnection( FedChannelConnectionDescription* fch)  
    {
      trackerParser_->addFedChannelConnection(fch);
    }

  TrackerParser* getTrackerParser() { return trackerParser_;}
} ;

#endif
