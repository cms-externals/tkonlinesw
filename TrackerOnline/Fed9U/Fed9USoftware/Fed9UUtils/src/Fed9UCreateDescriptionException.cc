
//Date: May 2004

/**
 * Class for error handling within the Fed9UCreateDescription.
 * This class provides a list of errors which can occur
 * in the Fed9UCreateDescription.
 */

#include"Fed9UCreateDescriptionException.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"

namespace Fed9U {

#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  IC_EXCEPTION_IMPLEMENTATION(Fed9UCreateDescriptionException, FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST)

}
