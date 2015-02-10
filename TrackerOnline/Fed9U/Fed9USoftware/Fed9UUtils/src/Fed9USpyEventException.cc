
//Exception class for Fed9USpyEvent which inherits from ICException.



#include"Fed9USpyEvent.hh"

// The ICExImpl.hh contains everything needed to define the complete
// implementation of the new ewxception class.
// NOTE: It MUST be included AFTER declaration of the class, NOT BEFORE.
#include "ICExImpl.hh"

namespace Fed9U {

#undef FED9U_DEF_ERROR
#define FED9U_DEF_ERROR FED9U_GENERATE_STRINGS

  IC_EXCEPTION_IMPLEMENTATION(Fed9USpyEventException, FED9U_SPY_EVENT_EXCEPTION_CODES_LIST)

}

