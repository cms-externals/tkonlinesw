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
