#ifndef _Fed9UEventException_H_
#define _Fed9UEventException_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**
 * \brief Fed9UEventException class to be used when throwing errors within the Fed9UEvent class.
 */
#define FED9U_EVENT_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_DAQ_MODE_UNKNOWN, "ERROR: The daq mode set in the event data is unknown/ or unhandled.")  \
  IC_DEF_ERROR(ERROR_GET_SAMPLES_FAILED, "ERROR: The method getSamples() has failed to complete correctly.")  \
  IC_DEF_ERROR(ERROR_FRONT_END_PACKET_CODE_CORRUPT, "ERROR: The packet code on one of the front ends was corrupt.") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")

  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UEventException, FED9U_EVENT_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif

#endif

#endif
