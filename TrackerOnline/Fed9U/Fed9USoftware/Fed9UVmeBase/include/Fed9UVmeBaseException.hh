/**Matthew Pearson, July 2003
   Exception class which inherits from ICException*/

#ifndef _Fed9UVmeBaseException_H_
#define _Fed9UVmeBaseException_H_

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UVmeBaseException class to be used when throwing errors within 
   the Fed9UVmeBase class and associated utility classes (Fed9UConstructCommand and FedHalInterface).*/

#define FED9U_VMEBASE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED9UVMEBASE, "ERROR_FED9UVMEBASE") \
  IC_DEF_ERROR(ERROR_FED9UCONSTRUCTCOMMAND, "ERROR_FED9UCONSTRUCTCOMMAND") \
  IC_DEF_ERROR(ERROR_FED9UHALINTERFACE, "ERROR_FED9UHALINTERFACE")
  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UVmeBaseException, FED9U_VMEBASE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif



