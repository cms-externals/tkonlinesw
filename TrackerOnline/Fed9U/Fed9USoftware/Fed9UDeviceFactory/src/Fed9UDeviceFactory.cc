  //#define DEBUGMSGERROR
#include <inttypes.h>
#include <stdint.h>

#include <time.h>  // for time function

#include "Fed9UDeviceFactory.hh"
#include "Fed9UDescriptionToXmlToFile.hh"
#include "Fed9UXMLDescriptionFromFile.hh"

  //#ifdef XDAQSHAREDOBJECT
  //#  include "xdaq.h"
  //#endif

#ifdef DATABASE
#include "DbFedAccess.h"
#include "Fed9UXMLDescriptionFromDb.hh"
#include "Fed9UDescriptionToXmlToDb.hh"
#endif

/** Build a FedDeviceFactory to retreive information from database or file
 * \param outputFileName - name of the XML output file
 * \exception Possible exceptions are:
 *     file not found
 *     bad XML format
 * \see addFileName to define the input file
 * 
 * Different type of configuration is possible:
 *     input               output            method to be called
 *     File                File              Fed9UDeviceFactory(string) addFileName(string)
 *     File                Database          Fed9UDeviceFactory(string,string,string) addFileName(string)
 *     Database            Database          Fed9UDeviceFactory(string,string,string)
 *     Datatabaase         File              Fed9UDeviceFactory(string), setDatabaseAccess(string,string,string,uint,lcassName)
 */

namespace Fed9U {

  /** Create an access to the database to FED database
   */

  Fed9UDeviceFactory::Fed9UDeviceFactory ( ) 
    noexcept : DeviceFactoryInterface () , partition_("null"), debugOutput_(false) {

    // create the empty vector on the heap, this class owns the vector so we must delete it in the destructor
    // we need this vector whether we are using DB or not
    fed9UDescriptionList_ = new std::vector<Fed9UDescription*>; 

#ifdef DATABASE
    try {
      // initially we set using strips to true, if the user wants to ignore strips he must explicity set it to false
      setUsingStrips ( true );
      // same goes for using binary strips!
      setUsingBinaryStrips ( true );

      if (debugOutput_)
	std::cout << "description list pointer= " << std::hex << fed9UDescriptionList_ << std::dec << std::endl;
      setDatabaseAccess () ;
    } catch ( std::exception & exc ) {
      // if there was an error getting the login information for the database we let the user know that they are not going to be using the DB
      // if there was an error connecting to the database we set a flag to state that DB Access is not available
      setUsingFile();
      dbAccess_ = NULL ;
      //if (debugOutput_)
	std::cout << "An error occurred during setDatabase: " << exc.what() << std::endl; 
    }catch ( ... ) {
      setUsingFile();
      dbAccess_ = NULL ;
      std::cerr << "An Unknown Exception error occurred during setDatabase: maybe you don't have CONFDB set. " << std::endl; 
    }
#endif 
  }


#ifdef DATABASE

  Fed9UDeviceFactory::Fed9UDeviceFactory ( std::string login, std::string password, std::string path, bool threaded ) noexcept: DeviceFactoryInterface ( login, password, path, threaded ) , partition_("null"), debugOutput_(false){
    try {
      // initially we set using strips to true, if the user wants to ignore strips he must explicity set it to false
      setUsingStrips ( true );
      // same goes for using binary strips!
      setUsingBinaryStrips ( true );

      fed9UDescriptionList_ = new std::vector<Fed9UDescription*>; // create the empty vector on the heap, this class owns the vector so we must delete it in the destructor
      if (debugOutput_)
	std::cout << "!!!!!!!!!description list pointer  " << hex << fed9UDescriptionList_ << std::endl;
      setDatabaseAccess ( login, password, path ) ;
    } catch ( std::exception & exc ) {
      // if there was an error getting the login information for the database we let the user know that they are not going to be using the DB
      // if there was an error getting the login information for the database we set a flag to state that DB Access is not available
      setUsingFile();
      dbAccess_ = NULL ;
      if (debugOutput_)
	std::cout << "An error occurred during setDatabase: " << exc.what() << std::endl; 
    }catch ( ... ) {
      setUsingFile();
      dbAccess_ = NULL ;
      std::cerr << "An Unknown Exception error occurred during setDatabase: maybe you don't have CONFDB set. " << std::endl; 
    }
  }
  
  /* deprecated
   * this method should be removed when all code that uses it has been updated.
   */
  Fed9UDeviceFactory::Fed9UDeviceFactory ( std::string filename )  noexcept : partition_("null") , debugOutput_(false)
  {
    std::stringstream errorOut;
    errorOut << "This method is deprocated please do not use.!!!" << filename << std::endl;
    THROW( Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_DEPRECATED_METHOD,errorOut.str().c_str()));
  }
  
  /** Create an access to the FED database
   * \param dbFedAccess - database access
   */
  Fed9UDeviceFactory::Fed9UDeviceFactory (DbFedAccess *dbFedAccess ) noexcept : DeviceFactoryInterface ( dbFedAccess ), partition_("null"), debugOutput_(false){
    try {   
      // initially we set using strips to true, if the user wants to ignore strips he must explicity set it to false
      setUsingStrips ( true );
      // same goes for using binary strips!
      setUsingBinaryStrips ( true );

      fed9UDescriptionList_ = new std::vector<Fed9UDescription*>; // create the empty vector on the heap, this class owns the vector so we must delete it in the destructor
      setDatabaseAccess ( dbFedAccess ) ;
     } catch ( std::exception & exc ) {
      // if there was an error getting the login information for the database we let the user know that they are not going to be using the DB
      // if there was an error getting the login information for the database we set a flag to state that DB Access is not available
      setUsingFile();
      dbAccess_ = NULL ;
      std::cerr << "An error occurred during setDatabase: " << exc.what() << std::endl; 
    } catch ( ... ) {
      setUsingFile();
      dbAccess_ = NULL ;
      std::cerr << "An Unknown Exception error occurred during setDatabase: maybe you don't have CONFDB set. " << std::endl; 
    }
  }
#endif

  /** Disconnect the database (if it is set)
   */  
  Fed9UDeviceFactory::~Fed9UDeviceFactory ( ) {
    // Remove all the FED description from the hash_map
    if (debugOutput_)
      std::cout << "!!!!!!!!!!Deleting FedDeviceFactory" << std::endl;
    if (fed9UDescriptionList_ != NULL) {
      for (std::vector<Fed9UDescription*>::iterator p=fed9UDescriptionList_->begin();p!=fed9UDescriptionList_->end();p++) {
	if (*p!= 0) {
	  std::cout << "fed pointer for freddy at Delete! = " << *p << std::endl;
	  delete *p;
	}
      }
    }
    if (fed9UDescriptionList_ != NULL)
      delete fed9UDescriptionList_;
    for (Fed9UHashMapType::iterator p=fed9UDescriptions_.begin();p!=fed9UDescriptions_.end();p++) {
      delete p->second ;
    }
  }


  void Fed9UDeviceFactory::vectorCopy ( std::vector<Fed9UDescription*>& dst, std::vector<Fed9UDescription*> & src ) {
    
    if (! src.empty()) {
      
      for (std::vector<Fed9UDescription*>::iterator description = src.begin() ; description != src.end() ; description ++) {
	
	dst.push_back ( (*description)->clone() ) ;
      }
    }
  }
  
  void Fed9UDeviceFactory::clearDescriptions ( ) noexcept(false) {
    for (Fed9UHashMapType::iterator p=fed9UDescriptions_.begin();p!=fed9UDescriptions_.end();p++)
	delete p->second ;
    if (fed9UDescriptionList_ != NULL) {
      for (std::vector<Fed9UDescription*>::iterator p=fed9UDescriptionList_->begin();p!=fed9UDescriptionList_->end();p++) {
	if (*p!= 0) {
	  std::cout << "fed pointer for freddy at Delete! = " << *p << std::endl;
	  delete *p;
	}
      }
      fed9UDescriptionList_->clear();
    }
  }

  /** Set a file as the new input, same method than addFileName but the previous devices and pia are deleted
   * \param inputFileName - new input file
   */
  void Fed9UDeviceFactory::setInputFileName ( std::string inputFileName ) noexcept(false) {
    try {
      // Remove all the FED description from the hash_map
      for (Fed9UHashMapType::iterator p=fed9UDescriptions_.begin();p!=fed9UDescriptions_.end();p++)
	delete p->second ;
      
      // add a new file
      addFileName (inputFileName) ;
    } catch ( Fed9UDeviceFactoryException & exc ) {
      RETHROW(exc,Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_FILE_ADD,"Failed when adding new file name to hash map"));
    } catch ( ... ) {
      THROW(Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_UNKNOWN,"Unknown error when setting Fed Input File Name"));
    }
  }
  /** Add a new file name and parse it to retreive the information needed
   * \param fileName - name of the XML file
   */
  void Fed9UDeviceFactory::addFileName ( std::string fileName ) noexcept(false) {
    try {
      // Parse the file and create a FED description
      // Then add the description to the hash map
      if (debugOutput_)
	std::cout << " adding filename !!!" << std::endl;
      Fed9UDescription * fed9UDescription = new Fed9UDescription();
      if (debugOutput_)
	std::cout << " created description object !!!" << std::endl;
      Fed9UXMLDescription xmlDescription(fileName, *fed9UDescription, fed9UDescriptionList_);
      if (debugOutput_)
	std::cout << " created XML description object !!!" << std::endl;
      xmlDescription.setDebugOutput(debugOutput_);
      xmlDescription.makeNewFed9UDescription();
      if (debugOutput_)
	std::cout << " called makeNewFed9UDescription() !!!" << std::endl;
      if (debugOutput_)
	std::cout << "adding fed id " << fed9UDescription->getFedId() << " to hash map" << std::endl;
      fed9UDescriptions_[fed9UDescription->getFedId()] = fed9UDescription;    
      if (debugOutput_)
	std::cout << "fed id in hash map at location " << fed9UDescription->getFedId() << " is " << fed9UDescriptions_[fed9UDescription->getFedId()]->getFedId() << std::endl;
    } catch ( ICUtils::ICException & exc) {
     RETHROW(exc, Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_FILE_ADD,"Failed when adding new file name to hash map"));
    }
  }

#ifdef DATABASE
  /** 
   * \param login - login to database
   * \param password - password to database
   * \param path - path to database
   * \param input - true: download from the database, false: upload into the database
   * \warning in the destructor of Fed9UDeviceFactory, the dbFedAccess is deleted
   */
  void Fed9UDeviceFactory::setDatabaseAccess ( std::string login, std::string password, std::string path) 
    noexcept(false)
  {
    
    // For FED database
    if (dbAccess_ != NULL)
      if (login != dbAccess_->getDbLogin() || password != dbAccess_->getDbPassword() || path != dbAccess_->getDbPath()) {
	delete dbAccess_ ;
	dbAccess_ = NULL ;
      }
  
    if (dbAccess_ == NULL) {
#ifdef DEBUGMSGERROR
      if (debugOutput_)
	std::cout << "Create a new access to the FED database for " << login << "/" << password << "@" << path << std::endl ;
#endif
      dbAccess_ = new DbFedAccess (login, password, path, threaded_) ;
      if (debugOutput_)
	std::cout << "ptr to DBAccess = " << dbAccess_ << std::endl; 
    }
    deleteDbAccess_ = true ;
    setUsingDb();

#ifdef DEBUGMSGERROR
      if (debugOutput_)
	std::cout << "Created access with no error" << std::endl ;
#endif
  }


  /** 
   * \param dbFedAccess - FED access to the database
   * \warning if this method is used, the access to the database must be deleted by the owner/creator of the dbAccess_
   */
  void Fed9UDeviceFactory::setDatabaseAccess ( DbFedAccess *dbFedAccess )
    noexcept(false) {
    DeviceFactoryInterface::setDatabaseAccess ((DbAccess *)dbFedAccess) ; 
  }

  void Fed9UDeviceFactory::setDatabaseAccess ()
    noexcept(false) {
    std::string login="null", passwd="null", path="null" ;
    DbFedAccess::getDbConfiguration (login, passwd, path) ;
    std::cout << "got the configuration from dbFedAccess in " << login << "/" << passwd << "@" << path << "..." << std::endl;
    ICUTILS_VERIFY(login != "null" && login != "nil")(login).error();
    setDatabaseAccess ( login, passwd, path ) ;
  }


  /**
   * \return FED database access, can be null
   */
  DbFedAccess * Fed9UDeviceFactory::getDatabaseAccess ( ) noexcept{
    return reinterpret_cast<DbFedAccess *>(dbAccess_) ;
  }

#endif
  /** Get all descriptions for one partition. you can specify version if you like
   */
  std::vector<Fed9UDescription*>* Fed9UDeviceFactory::getFed9UDescriptions(std::string partition, i16 major, i16 minor, int maskMajor, int maskMinor) noexcept(false) {
    // here we must download all descriptions for one partition, this is handled by FedXMLDescriptionFromDb and we retrieve a vector of descriptions.
    if (useDatabase_) {
      if (fed9UDescriptionList_ != NULL) {
	for (std::vector<Fed9UDescription*>::iterator p=fed9UDescriptionList_->begin();p!=fed9UDescriptionList_->end();p++) {
	  if (*p!= 0) {
	    std::cout << "fed pointer for freddy at Delete! = " << *p << std::endl;
	    delete *p;
	  }
	}
	fed9UDescriptionList_->clear();
      }
    }
    getFed9UDescription ( -1 , false, partition, major, minor, maskMinor, maskMajor );
    return fed9UDescriptionList_;
  }
  /** Get the hash map of FedDescriptions
   */

  /** Retreive information for the FED from a specific partition
   */
  Fed9UDescription Fed9UDeviceFactory::getFed9UDescription ( i16 fedId, bool hardwareId, std::string partition, i16 versionMajor, i16 versionMinor, int maskMajor, int maskMinor ) noexcept(false)  {
    partition_=partition;
    return getFed9UDescription ( fedId, hardwareId, versionMajor, versionMinor, maskMajor, maskMinor );
    partition_="null";
  }


  /** Retreive information for the FED
   */
  Fed9UDescription Fed9UDeviceFactory::getFed9UDescription ( i16 fedId, bool hardwareId, i16 versionMajor, i16 versionMinor, int maskMajor, int maskMinor ) noexcept(false)  {
    if (debugOutput_)
      std::cout << " about to create description " << std::endl;
    Fed9UDescription fed9UDescription ;
    if (debugOutput_)
      std::cout << " created description tempalate " << std::endl;
#ifdef DATABASE
#ifdef DEBUGMSGERROR
    if (debugOutput_)
      std::cout << "about to check if we are using Fed Db. usingFedDb_ =  " << static_cast<u16>(useDatabase_) << std::endl ;
#endif
    if ( useDatabase_ ) {
      // check that the database access is opened and we have a connection to the database
      ICUTILS_VERIFYX( dbAccess_ != NULL ,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_DB_CONNECTION).error();      
#ifdef DEBUGMSGERROR
     
#endif
	
      // the version of the download is set by the user in versionMajor, versionMinor
      try {
	if (debugOutput_)
	  std::cout << "If we get here then we are using the Database for the download!!!" << std::endl;
	Fed9UXMLDescriptionFromDb dbDescription(reinterpret_cast<DbFedAccess*>(dbAccess_), fed9UDescription, fed9UDescriptionList_) ;
	dbDescription.setUsingStrips(usingStrips_);
	dbDescription.setDebugOutput(debugOutput_);
	dbDescription.makeNewFed9UDescription( fedId, hardwareId, versionMajor, versionMinor, partition_, maskMajor, maskMinor ) ;  // by database
	if (debugOutput_)
	  std::cout << " downloaded description and written into fed9uDescription " << std::endl;
      } catch (Fed9UXMLDescriptionException & exc) {
	RETHROW(exc,Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_DESCRIPTION_FROM_DB,"error when creating description from DB "));
      } catch ( ... ) {
	THROW(Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_UNKNOWN,"Unknown error occurred when creating description from DB"));
      }
      if (debugOutput_)
	std::cout << " about to check the fedid " << std::endl;
      // Check the fed id and make an exception if it is not correct
      ICUTILS_VERIFYX (fedId == fed9UDescription.getFedId() || fedId == -1, Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_FEDID_MISMATCH).error();
      if (debugOutput_)
	std::cout << " about to return the description" << std::endl;
      return fed9UDescription ;
    }
    else
#endif
      {
	if (debugOutput_)
	  std::cout << "If we get here then we are using files for the download!!" << std::endl;
	// if we arrive here we have either built with no database or we are not using the database (usingFedDb_ = false)
	// at this point we must check that we have descriptions in the hash map and if so we can return the required description
	ICUTILS_VERIFYX ( fed9UDescriptions_.size(), Fed9UDeviceFactoryException)(fed9UDescriptions_.size()).
	  code(Fed9UDeviceFactoryException::ERROR_EMPTY_FILE_LIST).error();
	if (fedId == -1) {
	  // we are getting the whole list in one go.
	  return *((*fed9UDescriptionList_)[0]);
	} else {
	  ICUTILS_VERIFYX (  fed9UDescriptions_[fedId] != NULL ,Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_NO_FILE_WITH_FEDID).error();
	  ICUTILS_VERIFYX (  fed9UDescriptions_[fedId]->getFedId() == fedId , Fed9UDeviceFactoryException)(fed9UDescriptions_[fedId]->getFedId())(fedId).
	  code(Fed9UDeviceFactoryException::ERROR_FEDID_MISMATCH).error();
	  return *fed9UDescriptions_[fedId];
	}
      }
    
  }

    /** Upload a vector of Descriptions into the database or a file specifying the partition (ignored for files)
     * \param vector<Fed9UDescription*> & - reference to vector of descriptions of the FED to be uploaded
     * \param partition - the partition into which to upload the description
     */
  Fed9UDeviceFactory & Fed9UDeviceFactory::setFed9UDescriptions(std::vector<Fed9UDescription*>  f, std::string partition, u16 *versionMajor , u16 *versionMinor , int updateVersion ) noexcept(false) {
    partition_ = partition;

    
    // now we must upload or save a file from all the descriptions in the vector
    ICUTILS_VERIFYX ( versionMajor != NULL && versionMinor != NULL , Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_NULL_POINTER).error(); 
    
#ifdef DATABASE
    if ( useDatabase_ ) {
      ICUTILS_VERIFYX( dbAccess_ != NULL ,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_DB_CONNECTION).error();      
     
      // Upload to database
      // Create an fed9UDescriptionToXMLToDb for the upload in database      
      Fed9UDescriptionToXmlToDb theFed9UDescriptionToXmlToDb(f, reinterpret_cast<DbFedAccess *>(dbAccess_) );
      theFed9UDescriptionToXmlToDb.setUsingStrips(usingStrips_);
      theFed9UDescriptionToXmlToDb.setUsingBinaryStrips(usingBinaryStrips_);
      theFed9UDescriptionToXmlToDb.setDebugOutput(debugOutput_);
      int tmpVM;
      if (updateVersion) {
	//get the next Major version!!!
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMajorVersion();
	//make sure the version is positive if not throw and error
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      

	*versionMajor = static_cast<u16>(tmpVM);     
	*versionMinor = 0 ;
	// Upload to the database and increment the major version
	if (partition_ == "null"){
	  theFed9UDescriptionToXmlToDb.upload(updateVersion);
	} else {
	  theFed9UDescriptionToXmlToDb.upload(updateVersion, partition_);
	}
      }
      else {

	// retreive the current version
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMajorVersion() - 1;
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      
	*versionMajor = tmpVM;	
	
	// retrieve the next minor version from db
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMinorVersion(*versionMajor) ;
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      
	*versionMinor = tmpVM;
	
	// Upload to the Database not incrementing the major version
	if (partition_ == "null"){
	  theFed9UDescriptionToXmlToDb.upload(updateVersion);
	}else{
	  theFed9UDescriptionToXmlToDb.upload(updateVersion, partition_);
	}
      }


#ifdef DEBUMSGERROR
      if (debugOutput_)
	std::cout << "Create a new version " << *versionMajor << "." << *versionMinor << std::endl ;
#endif
    }
    else
#endif
       
    {  
      
      try {
	
	ICUTILS_VERIFYX(outputFileName_ != "", Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_EMPTY_FILENAME).error();
	Fed9UDescriptionToXmlToFile xmlfile( outputFileName_, f);
	xmlfile.setUsingStrips(usingStrips_);
	xmlfile.setUsingBinaryStrips(usingBinaryStrips_);
	xmlfile.setDebugOutput(debugOutput_);
	xmlfile.writeXmlFile();
      } catch (ICUtils::ICException & exc) {
	RETHROW(exc,Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_FAILED_FILE_SAVE,"Failed to save description file"));
      } catch ( ... ) {
	THROW(Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_UNKNOWN,"Failed to save description file"));
      }
    }
    
    partition_ = "null";
    return *this;

  }

  /** Upload a FED9UDescription into the database or a file specifying the partition 
   * \param Fed9UDescription - description of the FED to be uploaded
   * \param partition - the partition into which to upload the description
   */
  Fed9UDeviceFactory & Fed9UDeviceFactory::setFed9UDescription(Fed9UDescription & descriptionToUpload, std::string partition, u16 *versionMajor, u16 *versionMinor, int updateVersion ) noexcept(false) {
    partition_ = partition;
    setFed9UDescription(descriptionToUpload, versionMajor, versionMinor, updateVersion );
    partition_ = "null";
    return * this;
  }

  /** Upload a FED9UDescription into the database or a file
   * \param Fed9UDescription - description of the FED to be uploaded
   */
  Fed9UDeviceFactory & Fed9UDeviceFactory::setFed9UDescription(Fed9UDescription & descriptionToUpload, u16 *versionMajor, u16 *versionMinor, int updateVersion ) noexcept(false)
       {

    ICUTILS_VERIFYX ( versionMajor != NULL && versionMinor != NULL , Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_NULL_POINTER).error(); 

#ifdef DATABASE
    if ( useDatabase_ ) {
      ICUTILS_VERIFYX( dbAccess_ != NULL ,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_DB_CONNECTION).error();      
     
	  // Upload to database
      // Create an fed9UDescriptionToXMLToDb for the upload in database      
      Fed9UDescriptionToXmlToDb theFed9UDescriptionToXmlToDb(descriptionToUpload, reinterpret_cast<DbFedAccess *>(dbAccess_) );
      theFed9UDescriptionToXmlToDb.setUsingStrips(usingStrips_);
      theFed9UDescriptionToXmlToDb.setUsingBinaryStrips(usingBinaryStrips_);
      theFed9UDescriptionToXmlToDb.setDebugOutput(debugOutput_);
      int tmpVM;
      if (updateVersion) {
	//get the next Major version!!!
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMajorVersion();
	//make sure the version is positive if not throw and error
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      

	*versionMajor = static_cast<u16>(tmpVM);     
	*versionMinor = 0 ;
	// Upload to the database and increment the major version
	if (partition_ == "null"){
	  theFed9UDescriptionToXmlToDb.upload(updateVersion);
	} else {
	  theFed9UDescriptionToXmlToDb.upload(updateVersion, partition_);
	}
      }
      else {

	// retreive the current version
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMajorVersion() - 1;
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      
	*versionMajor = tmpVM;	
	
	// retrieve the next minor version from db
	tmpVM = reinterpret_cast<DbFedAccess*>(dbAccess_)->getNextMinorVersion(*versionMajor) ;
	ICUTILS_VERIFYX( tmpVM >= 0,Fed9UDeviceFactoryException ).code(Fed9UDeviceFactoryException::ERROR_BAD_VERSION).error();      
	*versionMinor = tmpVM;
	
	// Upload to the Database not incrementing the major version
	if (partition_ == "null"){
	  theFed9UDescriptionToXmlToDb.upload(updateVersion);
	}else{
	  theFed9UDescriptionToXmlToDb.upload(updateVersion, partition_);
	}
      }

      // std::cout << "IF WE SEE THIS THEN THE PROBLEM IS WITH THE DESTRUCTOR OF THE Fed9UDescriptionToXML class!!!" << std::endl;
#ifdef DEBUMSGERROR
      if (debugOutput_)
	std::cout << "Create a new version " << *versionMajor << "." << *versionMinor << std::endl ;
#endif
    }
    else
#endif
      {  
	try {
	  
	  ICUTILS_VERIFYX(outputFileName_ != "", Fed9UDeviceFactoryException).code(Fed9UDeviceFactoryException::ERROR_EMPTY_FILENAME).error();
	  //std::cout << outputFileName_ << std::endl;
	  Fed9UDescriptionToXmlToFile xmlfile( outputFileName_, descriptionToUpload);
	  xmlfile.setUsingStrips(usingStrips_);
	  xmlfile.setUsingBinaryStrips(usingBinaryStrips_);
	  xmlfile.setDebugOutput(debugOutput_);
	  xmlfile.writeXmlFile();
	} catch (ICUtils::ICException & exc) {
	  RETHROW(exc,Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_FAILED_FILE_SAVE,"Failed to save description file"));
	} catch ( ... ) {
	  THROW(Fed9UDeviceFactoryException(Fed9UDeviceFactoryException::ERROR_UNKNOWN,"Failed to save description file"));
	}
      }
    
    return *this;
  }

  /**Set a flag which informs the fedDeviceFactory if we are to upload and download Strips, if this is set to false then no strips are uploaded or downloaded.
   */
  Fed9UDeviceFactory & Fed9UDeviceFactory::setUsingStrips(bool usingStrips) 
    noexcept {
    usingStrips_ = usingStrips;
    return * this;
  }
  
  /**Get a flag which informs the fedDeviceFactory if we are to upload and download Strips, if this is set to false then no strips are uploaded or downloaded.
   */
  bool Fed9UDeviceFactory::getUsingStrips() noexcept {
    return usingStrips_;
  }

  bool Fed9UDeviceFactory::getUsingBinaryStrips() {
    return usingBinaryStrips_;
  }


  void Fed9UDeviceFactory::setUsingBinaryStrips(bool usingBinaryStrips) {
    usingBinaryStrips_ = usingBinaryStrips;
    std::cout << "usingBinaryStrips_ = " << (usingBinaryStrips_?"true":"false") << std::endl;
  }

  /** Method to set the value of debugOutput_. If this is set to true the the buffer will be displayed to standard output for debugging */ 
  void Fed9UDeviceFactory::setDebugOutput(bool value) {
    debugOutput_=value;
    std::cout << "debugoutput = "<<( debugOutput_? "true" : "false:") << std::endl; 
  }

  /** \brief set instance
   */
  //  void Fed9UDeviceFactory::setInstance( u16 instance) noexcept {
  //  instance_ = instance;
  //}

}
