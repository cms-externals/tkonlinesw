// #####################################################################################
// # Copyright (c) 2004 Imperial College London, Rutherford Appleton Laboratory        #
// # (Jonathan Fulcher, Emlyn Corrin, Matthew Pearson, Gareth Rogers, Matt Noy)        #
// #                                                                                   #
// # Permission is hereby granted, free of charge, to any person obtaining a copy      #
// # of this software and associated documentation files (the "Software"), to use      #
// # and modify for personal use only. No rights are given to deal in the Software,    #
// # publish, distribute, sublicense, and/or sell copies of the Software.              #
// # Support will be available from the authors to anyone using unmodified versions    #
// # of the Software. If the user modifies any part of the Software, no liability      #
// # is accepted on the part of the author and no support will be hitherto available.  #
// #                                                                                   #
// # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        #
// # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          #
// # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       #
// # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, #
// # WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN   #
// # CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
// #####################################################################################

/**jonathan Fulcher, 11/11/04.*/
/**Class which inherits from Fed9UXMLDescription to provide a file based interface. */

#ifndef _Fed9UXMLDescriptionFromDb_H_
#define _Fed9UXMLDescriptionFromDb_H_

//if the database is not in use we don't need this class at all.
#ifdef DATABASE
#include "DbFedAccess.h"


//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UUtils.hh"


//Std headers
#include<string>

namespace Fed9U {

  //using.*std::string;

  class Fed9UXMLDescriptionFromDb : public Fed9UXMLDescription
  {
  public:
    /**Constructor. This takes as arguments the XML file name and the Fed9UDescription to be modified.*/
    Fed9UXMLDescriptionFromDb( DbFedAccess *, Fed9UDescription &defaultFed9UDescription , std::vector<Fed9UDescription*>* desclist, bool usingStrips = true);
    ~Fed9UXMLDescriptionFromDb();

    xercesc::DOMLSInput* getDatabaseDOMInputSource( i16 fedId, bool hardwareId,  int versionMajor , int versionMinor, std::string partition, int maskMajor, int maskMinor);

    /*    
    void setFed9UDescriptionDatabase( Fed9UDescription, u32 major, u32 minor );
    */

    /**Method which modifies the Fed9UDescription. 
       It modifies the object member reference to the Fed9UDescription.*/
    std::vector<Fed9UDescription*> * makeNewFed9UDescription(i16 fedId, bool hardwareId, int versionMajor, int versionMinor, std::string partition, int maskMajor, int maskMinor);
    std::vector<Fed9UDescription*> * download(i16 fedId, bool hardwareId, int versionMajor, int versionMinor, std::string partition, int maskMajor, int maskMinor );

  private:
    XMLByte * xmlBuffer_;
    DbFedAccess *  dbFedAccess_;  
  };

}

#endif
#endif
#endif

#endif //_Fed9UXMLDescriptionFromDb_H_
/**Jonathan Fulcher, 27 Oct 2004.
Class to upload the conents of a Fed9UDescription to a database in XML format.
This class uses the Xerces 2.3 library.*/

#ifndef _Fed9UDescriptionToXmlToDb_H_
#define _Fed9UDescriptionToXmlToDb_H_

#ifdef DATABASE

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

//Fed9U headers
#include "Fed9UUtils.hh"


//Standard headers
#include<string>

//Database headers

#include "DbFedAccess.h"

namespace Fed9U {

  //using std::string;


  class Fed9UDescriptionToXmlToDb : public Fed9UDescriptionToXml 
  {
  public:
    /**Constructor takes a reference to the Fed9UDescription to be uploaded, also the DbFedAccess which is the accessor to the database, the IP of the supervisor making the upload. */
    Fed9UDescriptionToXmlToDb(Fed9UDescription &Fed9UDescriptionToWrite, 
			      DbFedAccess * dbFedAccess) noexcept(false);

    Fed9UDescriptionToXmlToDb(std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite, 
			      DbFedAccess * dbFedAccess ) noexcept(false);

    /**Method which writes the XML to the db. */
    void upload(u16,u16, std::string partition="null");

    /**Method which writes the XML to the db, now you can select between three modes of upload
     * updateVersion=0 - Minor version update
     * updateVersion=1 - Major version update
     * updateVersion=2 - Append the uploaded feds to the same version.
     */
    void upload(int updateVersion=0,std::string partition="null");
   
  private:
    /**Method which constructs the DOM document.*/
    virtual void constuctDOMDocument(void) noexcept(false);

    /**Method which streams out the fast style description to a stream pointer
     * this is only required for the database upload so is implemented in this derived class
     */
    void streamOutFastDescription(std::string * buf) noexcept(false);

    virtual void addDOMElements(xercesc::DOMElement *rootElement) noexcept(false);

    DbFedAccess*  dbFedAccess_;
      
  };

}

#endif
#endif
#endif
#endif

/**jonathan Fulcher, Dec 2005
   Exception class for Fed9UDeviceFactory which inherits from ICException*/

#ifndef _Fed9UDeviceFactoryException_H_
#define _Fed9UDeviceFactoryException_H_

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UDeviceFactoryException class to be used when throwing errors within the Fed9UDeviceFactory class.*/

#define FED9U_DEVICEFACTORY_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FILE_OPEN, "ERROR: file failed to open.") \
  IC_DEF_ERROR(ERROR_BAD_VERSION, "ERROR: Major version number returned was negative.") \
  IC_DEF_ERROR(ERROR_DEPRECATED_METHOD, "ERROR: deprecated method, don't use!") \
  IC_DEF_ERROR(ERROR_FILE_ADD, "ERROR: failed to add file to hash map") \
  IC_DEF_ERROR(ERROR_NULL_POINTER, "ERROR: Null pointers") \
  IC_DEF_ERROR(ERROR_FAILED_FILE_SAVE, "ERROR: Failed to save Description file") \
  IC_DEF_ERROR(ERROR_EMPTY_FILENAME, "ERROR: Empty file name to write description, must have name for file!") \
  IC_DEF_ERROR(ERROR_DESCRIPTION_FROM_DB, "ERROR: failed to create description from DB.") \
  IC_DEF_ERROR(ERROR_SET_TID, "ERROR: failed when setting TiD.") \
  IC_DEF_ERROR(ERROR_DB_CONNECTION, "ERROR: db connection failed.") \
  IC_DEF_ERROR(ERROR_EMPTY_FILE_LIST, "ERROR: There are no files in the hash map!!! don't forget to add files to the list first using addFedFileName().") \
  IC_DEF_ERROR(ERROR_FEDID_MISMATCH, "ERROR: The fedid in the xml buffer does not match the fedid under which the description was loaded!") \
  IC_DEF_ERROR(ERROR_NO_FILE_WITH_FEDID, "ERROR: hash_map of files does not contain a description with the desired FedId.") \
  IC_DEF_ERROR(ERROR_NO_DATA, "ERROR: No data in database.") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")
  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UDeviceFactoryException, FED9U_DEVICEFACTORY_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif
#ifndef FED9UDEVICEFACTORY_H
#define FED9UDEVICEFACTORY_H

  // Name of the Fed9USupervisor
#define FED9UOUTPUTFILE "TempDescription.xml"

#include <string>
#include "Fed9UUtils.hh" 
#include "Fed9UDeviceFactoryException.hh" 

// To retreive the IP address of the current machine
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hashMapDefinition.h"
#ifndef HASHMAPDEFINITION_H
//using.*std;

// Compatibility with the G++ version 3.X
// Retreive from http://gcc.gnu.org/onlinedocs/libstdc++/faq/
#ifdef __GNUC__
#if __GNUC__ < 3
#include <hash_map>
namespace Sgi { using ::hash_map; }; // inherit globals
#else
#include <ext/hash_map>
#if __GNUC_MINOR__ == 0
namespace Sgi = std;               // GCC 3.0
#else
namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
#endif
#endif
#else      // ...  there are other compilers, right?
namespace Sgi = std;
#endif
#endif
using namespace Sgi;


#ifndef UNKNOWNFILE
// File unknown
#  define UNKNOWNFILE "Unknown"
#endif

#ifdef DATABASE
#include "DbFedAccess.h"
class DbFedAccess ;
#endif

#include "DeviceFactoryInterface.h"

namespace Fed9U {

  // Hash map of a list of FED9UDescriptions in regards of the FED hard id
  typedef hash_map<Fed9U::u16, Fed9U::Fed9UDescription *> Fed9UHashMapType ;
  
  /** This class manages all FED descriptions:
   * It is mandatory that the XML schema merges all the XML descriptions already defined for the FED
   */
  class Fed9UDeviceFactory : public DeviceFactoryInterface {
    
  private:
    
#ifdef DATABASE
    
    /** FED version
     */
    unsigned int versionFedMajor_, versionFedMinor_ ;
    
#endif
    
    /** Store the description found versus the FED hardware ID
     */
    Fed9UHashMapType fed9UDescriptions_ ;

    std::vector<Fed9UDescription*>* fed9UDescriptionList_;

    bool usingStrips_;

    std::string partition_;

    bool debugOutput_;

    bool usingBinaryStrips_;

  public:
    
    /** \brief Build a FED factory
     */
    Fed9UDeviceFactory ( ) noexcept;

    /* depricated
     * this method should be removed when all code that uses it has been updated.
     */
    Fed9UDeviceFactory ( std::string filename ) noexcept;

#ifdef DATABASE
    /* This method is used by the DeviceFactory (FD/2006-12-19)
     */
    Fed9UDeviceFactory ( std::string login, std::string password, std::string path, bool threaded = false ) noexcept;
    
    /** \brief Build a FED factory for FED database
     */
    Fed9UDeviceFactory ( DbFedAccess *dbFedAccess )
      noexcept ;
#endif
    
    /** \brief Disconnect the database (if it is set)
     */  
    ~Fed9UDeviceFactory ( ) ;
    
    
    /** 
     * \brief Copy a vector of descriptions into another one with a clone of the descriptions
     * \warning the remote method must delete the vector created
     */
    static void vectorCopy ( std::vector<Fed9UDescription*>& dst, std::vector<Fed9UDescription*> & src ) ;

    
    /** \brief set instance
     */
    //    void setInstance( u16 instance ) noexcept(false)  ;
    
    /** \brief Add a new file name in the descriptions
     */
    void addFileName ( std::string fileName ) noexcept(false) ;

    /** \brief Add a new file name after deleting all current files in hash map
     */
    void setInputFileName ( std::string fileName ) noexcept(false) ;

    /** \brief delete all current descriptions in hash map and description list
     */
    void clearDescriptions ( ) noexcept(false) ;
    
#ifdef DATABASE
    /** \brief Set a database access
     */
    void setDatabaseAccess ( std::string login, std::string password, std::string path ) 
      noexcept(false) ;

    void setDatabaseAccess ( ) 
      noexcept(false) ;
    
    /** \brief Set the database access
     */
    void setDatabaseAccess ( DbFedAccess *dbFedAccess) 
      noexcept(false) ;
    
    DbFedAccess * getDatabaseAccess ( ) noexcept;
  
#endif
    

    /** \brief Retreive all Fed descriptions for one partition.
     */
    std::vector<Fed9UDescription*>* getFed9UDescriptions(std::string partition, i16 major = -1, i16 minor = -1, int maskMajor = 0, int maskMinor = 0) noexcept(false) ;

    /** \brief Retreive information for a given FED / version
     */
    Fed9UDescription getFed9UDescription(i16 fedId, bool hardwareID, std::string partition, i16 major = -1, i16 minor = -1, int maskMajor = 0, int maskMinor = 0) noexcept(false) ;

    /** \brief Retreive information for a given FED / version
     */
    Fed9UDescription getFed9UDescription(i16 fedId, bool hardwareID, i16 major = -1, i16 minor = -1, int maskMajor = 0, int maskMinor = 0) noexcept(false) ;
    
    /** \brief Set the vector of FED descriptions into the output
     */
    Fed9UDeviceFactory & setFed9UDescriptions(std::vector<Fed9UDescription*>  f, std::string partition, u16 *versionMajor = NULL, u16 *versionMinor = NULL , int majVersion = 0) noexcept(false) ;

    /** \brief Set the FED description into the output
     */
    Fed9UDeviceFactory & setFed9UDescription(Fed9UDescription & f, u16 *versionMajor = NULL, u16 *versionMinor = NULL , int majVersion = 0) noexcept(false) ;

    /** \brief Set the FED description into the output and for DATABASE we can specify the partition
     */
    Fed9UDeviceFactory & setFed9UDescription(Fed9UDescription & f, std::string partition, u16 *versionMajor = NULL, u16 *versionMinor = NULL , int majVersion = 0) noexcept(false) ;
    
    
    /** \brief Set a flag which informs the fedDeviceFactory if we are to upload and download Strips, if this is set to false then no strips are uploaded or downloaded.
     */
    Fed9UDeviceFactory & setUsingStrips(bool usingStrips) 
    noexcept ;
    
    /** \brief Get a flag which informs the fedDeviceFactory if we are to upload and download Strips, if this is set to false then no strips are uploaded or downloaded.
     */
    bool getUsingStrips() noexcept;

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
    
   /** Method to set the value of debugOutput_. If this is set to true the the buffer will be displayed to standard output for debugging */
    void setDebugOutput(bool value=true);

    bool getUsingDb() noexcept { 
#ifdef DATABASE    
      return useDatabase_; 
#else
      return false;
#endif
    }
  }; // CLASS fed9UDeviceFactory
  
} // namespace Fed9U


#endif
