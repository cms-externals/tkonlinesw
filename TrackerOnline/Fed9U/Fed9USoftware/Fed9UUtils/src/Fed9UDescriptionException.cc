
//Exception class for Fed9UDescription which inherits from ICException.



#include"Fed9UDescription.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"

namespace Fed9U {

#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  IC_EXCEPTION_IMPLEMENTATION(Fed9UDescriptionException, FED9U_DESCRIPTION_EXCEPTION_CODES_LIST)

}

