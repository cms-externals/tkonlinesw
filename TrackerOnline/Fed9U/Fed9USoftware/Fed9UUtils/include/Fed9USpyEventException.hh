#ifndef H_Fed9USpyEventException
#define H_Fed9USpyEventException

#include "ICExDecl.hh"

namespace Fed9U {

/*
 * \brief Fed9USpyEventException class to be used when throwing errors from the Fed9USpyEvent class.
 */

#define FED9U_SPY_EVENT_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_EVENT_BUFFER_FULL, "ERROR: The Buffer is full. No more data can be added to the event object.")  \
  IC_DEF_ERROR(ERROR_DELAY_CHIP_DATA_NOT_FOUND, "ERROR: The data for this delay chip was not found in the event object.")  \
  IC_DEF_ERROR(ERROR_INDEX_OUT_OF_RANGE, "ERROR: The sample index is out of range. ") \
  IC_DEF_ERROR(ERROR_PIPELINE_ADDRESS_MISMATCH, "ERROR: The pipeline addresses for data in event do not match. ") \
  IC_DEF_ERROR(ERROR_NO_TICK_MARK, "ERROR: The channel does not contain a tick mark. ")


  IC_EXCEPTION_CLASS_BEGIN(Fed9USpyEventException, FED9U_SPY_EVENT_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif //H_Fed9USpyEventException

