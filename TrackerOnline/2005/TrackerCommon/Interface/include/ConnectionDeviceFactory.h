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

#include <string>
#include <map>
using std::string ;

#include "xercesc/sax/InputSource.hpp"
using xercesc::DOMInputSource;

#include "TrackerParser.h"

#ifdef DATABASE
#include "DbFedFecAccess.h"
#endif

/** This class manage all CONNECTION descriptions:
 * It is mandatory the XML schema merge all the XML description already defined for the FEC and the CONNECTION
 */
class FedFecConnectionDeviceFactory {

 private:
  
#ifdef DATABASE 
  /** CONNECTION version
   */
  unsigned int versionConnectionMajor_, versionConnectionMinor_ ;
  
  /** If true the dbFecAccess_ will be deleted in the desctructor of this class
   * See setDatabaseAccess (string, string, string) and setDatabaseAccess (DbFedFecAccess)
   */
  bool deleteDbFedFecAccess_ ;

  /** Database FEC access
   */
  DbFedFecAccess *dbFedFecAccess_ ;
#endif
  
  TrackerParser* parser_;
  string xmlOutput_;

  // -----------------------------------------------------------------------------------------------------
  // Private method to get a buffer from the database and to set a buffer to the database
  // -----------------------------------------------------------------------------------------------------
#ifdef DATABASE
  /** Upload a DOMInputSource to the database
   */
  void getDatabaseDOMInputSource ( DOMInputSource &domInputSource ) noexcept(false) {

    if (dbFedFecAccess_ != NULL) {

      
    }
  }

  /** Retreive a DOMInputSource from the database
   */
  DOMInputSource* getDatabaseDOMInputSource( std::string partitionName ) noexcept(false) {    

    if (dbFedFecAccess_ != NULL) {

      //download the fed parameters from the database
      XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = NULL;

      oracle::occi::Clob *xmlClob = NULL;
      XMLByte *xmlBuffer = NULL;       
      DOMInputSource* domInputSource;

      // ?????????????????????????????????????????? supervisorIp, xdaqId, className must be replaced by partitionName ????????????????????????????????????
      if ((xmlClob = dbAccess->getXMLClob(partitionName, supervisorIp, xdaqId, className )) != NULL){

	unsigned int resultLength = (*xmlClob).length();

#ifdef DEBUMSGERROR
	std::cout << "resultLength : " << resultLength << std::endl;
#endif

	if (resultLength>0) {

	  if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char))) != NULL) {
	    if (resultLength == (*xmlClob).read(resultLength, xmlBuffer, resultLength+1)){
	      
	      const std::string xmlBufferId = "myXmlBuffer";

	      XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = new XERCES_CPP_NAMESPACE::MemBufInputSource ((const XMLByte*)xmlBuffer, 
														     strlen((const char*)xmlBuffer), 
														     (const XMLCh *)xmlBufferId.c_str(), 
														     false, 
														     XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);      
	      
	      // convert the input source to a DOMInputSource  
	      domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;

#ifdef DEBUGMSGERROR
	      std::cout << xmlBuffer << std::endl;
#endif
	    }
	    else {
#ifdef DEBUGMSGERROR
	      std::cerr << "failed while reading the XML Clob"  << std::endl;
#endif
	      throw exception ;
	    }
	  }
	  else {
#ifdef DEBUGMSGERROR
	    std::cerr << "failed during memory allocation"  << std::endl;  	
#endif
	      throw exception ;
	  }
	}
	else {
#ifdef DEBUGMSGERROR
	  std::cerr << "no data found... " << std::endl;
#endif
	  throw exception ;
	}
      }
      else {
#ifdef DEBUGMSGERROR
	std::cerr << "failed while trying to get the XML Clob"  << std::endl;
#endif
	throw exception ;
      } 
    }
    else {

#ifdef DEBUGMSGERROR
      std::cerr << "Database is not connected" << std::endl ;
#endif
    }
    
    return domInputSource;
  }
#endif

 public:


  /** \brief Build a FEC factory
   */
  FedFecConnectionDeviceFactory ( string outputFileName=UNKNOWFILE )
    {
#ifdef DATABASE
      dbFecFedDbAccess_ = NULL ;
#endif
      
      parser_=new TrackerParser();
    }

#ifdef DATABASE
  /** \brief Buid a CONNECTION factory for database
   */
  FedFecConnectionDeviceFactory ( string login, string password, string path)
    noexcept(false) {

    dbFecFedAccess_ = NULL ;
    
    setFecFedConnectionDatabaseAccess ( login, passwd, path ) ;
  }
 
  /** \brief Build a FEC factory for CONNECTION database
   */
  FedFecConnectionDeviceFactory ( DbFedFecAccess *dbFedFecAccess)
    noexcept(false) {
    
    dbFecAccess_ = NULL ;
    
    setFecFedConnectionDatabaseAccess ( dbFedFecAccess ) ;
  }

  /** \brief create the database access
   * \param login to the database
   * \param password to the database
   * \param path to the database
   */
  void setFecFedConnectionDatabaseAccess ( string login, string password, string path )
    noexcept(false) {

    // For FEC/FED database
    if (dbFedFecAccess_ != NULL)
      if (login != dbFedFecAccess_->getDbLogin() || password != dbFedFecAccess_->getDbPassword() || path != dbFedFecAccess_->getDbPath()) {
	delete dbFecAccess_ ;
	dbFecAccess_ = NULL ;
      }
  
    if (dbFedFecAccess_ == NULL) {
#ifdef DEBUGMSGERROR
      std::cout << "Create a new access to the FEC database for " << login << "/" << password << "@" << path << std::endl ;
#endif
      dbFedFecAccess_ = new DbFedFecAccess (login, password, path) ;
    }
    deleteDbFedFecAccess_ = true ;
  }

  /** \brief Set the database access
   * \param database access
   */
  void setFecFedConnectionDatabaseAccess ( DbFedFecAccess *dbFedFecAccess)
    noexcept(false) {
   
    dbFedFecAccess_ = dbFecAccess ;
    deleteDbFedFecAccess_ = false ;
  }

  /** \brief retreive the database connection
   * \return the database access
   */
  DbFedFecAccess *getFecFedConnectionDatabaseAccess ( ) {

    return dbFecFedAccess_ ;
  }
#endif

  /** \brief Disconnect the database (if it is set)
   */  
  ~FecFedConnectionDeviceFactory ( ) 
    {
      delete parser_;

#ifdef DATABASE
      // Delete the database access
      if (deleteDbFedFecAccess_ && dbFedFecAccess_ != NULL) {

	delete dbFedFecAccess_ ;
      }
#endif
    }

  /** \brief set the output file
   */
  void setConnectionOutputFileName ( string outputFileName ) 
    {
      xmlOutput_.clear();
      xmlOutput_.append(outputFileName.c_str());
      
    }

  /** \brief Add a new file name in the descriptions
   */
  void addConnectionFileName ( string fileName )
    {
      parser_->parseFile(fileName);
    }

  /** \brief Retreive information for a given CONNECTION / version
   */
  FedChannelConnectionDescription* getFedChannelConnection(int i) { return parser_->getFedChannelConnection(i);}

  FedChannelConnectionDescription* getFedChannelConnectionByDcu(int dcu) { return  parser_->getFedChannelConnectionByDcu(i);}
  FedChannelConnectionDescription* getFedChannelConnection(int fedid,int fedch) {  return parser_->getFedChannelConnection(fedid,fedch);
  }

  /** \brief Set the CONNECTION description into the output
   */
  void addFedChannelConnection( FedChannelConnectionDescription* fch, unsigned int *versionMajor = NULL, unsigned int *versionMinor = NULL)  
  {
    parser_->addFedChannelConnection(fch);
  }


 /** Retreive the IP address of the machine
  */
 
} ;

#endif
