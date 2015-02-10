/**Matthew Pearson, Dec 2003
   Exception class for Fed9UXMLDescription which inherits from ICException*/

#ifndef _Fed9UXMLDescriptionException_H_
#define _Fed9UXMLDescriptionException_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UXMLDescriptionException class to be used when throwing errors within the Fed9UXMLDescription class.*/

#define FED9U_XMLDESCRIPTION_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_PARSE, "ERROR: An error occured during the XML parsing.") \
  IC_DEF_ERROR(ERROR_UNKNOWN_ATTRIBUTE_VALUE, "ERROR: An illegal attribute value has been found.") \
  IC_DEF_ERROR(ERROR_REQUIRED_ATTRIBUTE_NOT_FOUND, "ERROR: An attribute that was expected has not been found.") \
  IC_DEF_ERROR(ERROR_ILLEGAL_ATTRIBUTE, "ERROR: You have set an attribute to an illegal value.") \
  IC_DEF_ERROR(ERROR_XML, "ERROR: caught XMLException.") \
  IC_DEF_ERROR(ERROR_DOM, "ERROR: caught DOMException.") \
  IC_DEF_ERROR(ERROR_FILE_OPEN, "ERROR: file failed to open.") \
  IC_DEF_ERROR(ERROR_NUM_APV, "ERROR: you have tried to set the medianOverride for a number of APVs which does not equal 24.") \
  IC_DEF_ERROR(ERROR_MEMORY, "ERROR: ran out of memory.") \
  IC_DEF_ERROR(ERROR_CLOB, "ERROR: Clob error from Database") \
  IC_DEF_ERROR(ERROR_NO_DATA, "ERROR: No data in database.") \
  IC_DEF_ERROR(ERROR_INCORRECT_USAGE, "ERROR: you do not have the correct combination of parameters for this method.") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught. This is not an XMLException or DOMException.")
  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UXMLDescriptionException, FED9U_XMLDESCRIPTION_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif

#endif

#endif
