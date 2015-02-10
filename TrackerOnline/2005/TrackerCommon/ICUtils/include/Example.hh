// The ICExDecl.hh file contains everything needed for
// declaring a new ICException sub type
#include "ICExDecl.hh"

// We need to define the error codes to use with this exception class
#define CODES                                               \
  IC_DEF_ERROR(ERR_SOMETHING, "Something went wrong")       \
  IC_DEF_ERROR(ERR_ELSE,      "Something else went wrong")  \
  IC_DEF_ERROR(ERR_DUNNO,     "Don't know what went wrong")

// These two lines declare the new exception class
// additional variables/methods may be added between the two lines if necessary
IC_EXCEPTION_CLASS_BEGIN(TestException, CODES)
IC_EXCEPTION_CLASS_END
