//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UXMLDescriptionException."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300


#include"Fed9UEventException.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"

namespace Fed9U {

#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  IC_EXCEPTION_IMPLEMENTATION(Fed9UEventException, FED9U_EVENT_EXCEPTION_CODES_LIST)

}

#endif
#endif
