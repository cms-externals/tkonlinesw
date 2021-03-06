//Jonathan Fulcher, Feb 2005
//Exception class for Fed9UDeviceFactory which inherits from ICException.


//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UXMLDescriptionException."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300


#include"Fed9UDeviceFactoryException.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"

namespace Fed9U {

#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  IC_EXCEPTION_IMPLEMENTATION(Fed9UDeviceFactoryException, FED9U_DEVICEFACTORY_EXCEPTION_CODES_LIST)

}

#endif
#endif
