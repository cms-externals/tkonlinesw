#ifndef H_Fed9USpyApvDataException
#define H_Fed9USpyApvDataException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9USpyApvDataException class to be used when throwing errors from the Fed9USpyApvData class.
 */

#define FED9U_SPY_APV_DATA_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_OUT_OF_RANGE, "ERROR: The sample is out of range") \
  IC_DEF_ERROR(ERROR_TICK_MARK_NOT_FOUND, "ERROR: The tick mark was not found") \
  IC_DEF_ERROR(ERROR_APV_HEADER, "ERROR: The information could not be extracted from the APV header")


  IC_EXCEPTION_CLASS_BEGIN(Fed9USpyApvDataException, FED9U_SPY_APV_DATA_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9USpyApvDataException

