
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
