#ifndef H_Fed9UDescriptionException
#define H_Fed9UDescriptionException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9UDescriptionException class to be used when throwing errors from the Fed9USpyEvent class.
 */

#define FED9U_DESCRIPTION_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_NO_ADDRESS_TABLE, "ERROR: The Address table was not found.")  \


  IC_EXCEPTION_CLASS_BEGIN(Fed9UDescriptionException, FED9U_DESCRIPTION_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9UDescriptionException

