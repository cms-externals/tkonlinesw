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

