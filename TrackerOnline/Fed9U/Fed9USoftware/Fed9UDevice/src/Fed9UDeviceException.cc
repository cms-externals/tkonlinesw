#include "Fed9UDeviceException.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"


namespace Fed9U {
  
#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  //<JEC date=13/9/07> change suggested by KH for compilation on SLC4
  //  IC_EXCEPTION_IMPLEMENTATION(Fed9UDeviceException, FED9U_DEVICE_EXCEPTION_CODES_LIST);
  IC_EXCEPTION_IMPLEMENTATION(Fed9UDeviceException, FED9U_DEVICE_EXCEPTION_CODES_LIST)
  //</JEC>
}
