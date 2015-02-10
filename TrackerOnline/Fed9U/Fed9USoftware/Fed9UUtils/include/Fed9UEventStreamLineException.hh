/*   Exception class for Fed9UEventStreamLine which inherits from ICException*/

#ifndef _Fed9UEventStreamLineException_H_
#define _Fed9UEventStreamLineException_H_

//Check if using Xerces version 1
//#if XERCES > 1

//Check Xerces version
//#include "xercesc/util/XercesVersion.hpp"
//#if _XERCES_VERSION >= 20300

#include "ICExDecl.hh"

namespace Fed9U {

/**Fed9UEventStreamLineException class to be used when throwing errors within the Fed9UEventStreamLine class.*/

#define FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED9UEVENTSTREAMLINE, "ERROR:  An error occurred in Fed9UEventStreamLine.")  \
  IC_DEF_ERROR(ERROR_FEUNIT_DISABLED, "ERROR:  Attempt to access a disabled FE-unit.")  \
  IC_DEF_ERROR(ERROR_DAQ_MODE_UNKNOWN, "ERROR: The daq mode set in the event data is unknown/ or unhandled.")  \
  IC_DEF_ERROR(ERROR_GET_SAMPLES_FAILED, "ERROR: The method getSamples() has failed to complete correctly.")  \
  IC_DEF_ERROR(ERROR_FRONT_END_PACKET_CODE_CORRUPT, "ERROR: The packet code on one of the front ends was corrupt.") \
  IC_DEF_ERROR(ERROR_FEUNIT_OUT_OF_BOUNDS, "ERROR:  Input FE-FPGA number must run between 1 and 8. ") \
  IC_DEF_ERROR(ERROR_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input channel number must run between 1 and 12. ") \
  IC_DEF_ERROR(ERROR_APV_OUT_OF_BOUNDS, "ERROR:  Input APV number must be either 1 or 2. ") \
  IC_DEF_ERROR(ERROR_BAD_FRONT_END_PACKET_CODE, "ERROR: Information requested which does not exist in this payload format.") \
  IC_DEF_ERROR(ERROR_DATA_BUFFER_CORRUPT, "ERROR: Data buffer is either empty or corrupted.") \
  IC_DEF_ERROR(ERROR_FED_CHANNEL_OUT_OF_BOUNDS, "ERROR:  Input FED channel number must run between 1 and 96. ") \
  IC_DEF_ERROR(ERROR_UNKNOWN, "ERROR: An unknown exception was caught.")

  
  IC_EXCEPTION_CLASS_BEGIN(Fed9UEventStreamLineException, FED9U_EVENT_STREAMLINE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

//#endif

//#endif

#endif
