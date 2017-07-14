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
