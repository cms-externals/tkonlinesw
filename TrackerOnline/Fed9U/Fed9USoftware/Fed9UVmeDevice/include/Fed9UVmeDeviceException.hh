#ifndef _Fed9UVmeDeviceException_H_
#define _Fed9UVmeDeviceException_H_
#include "ICExDecl.hh"

namespace Fed9U {

#define FED9U_VME_DEVICE_EXCEPTION_CODE_LIST \
  IC_DEF_ERROR(ERROR_FED9UVMEDEVICE, "An error has been thrown from Fed9UVmeDevice") \
  IC_DEF_ERROR(ERROR_INIT_FAILED, "The initialisation function has failed") \
  IC_DEF_ERROR(ERROR_START_FAILED, "The FED has failed to start") \
  IC_DEF_ERROR(ERROR_STOP_FAILED, "The FED has failed to stop") \
  IC_DEF_ERROR(ERROR_FED9UTTCRXDEVICE, "An error has been thrown from Fed9UTtcrxDevice") \
  IC_DEF_ERROR(ERROR_FED9UVOLTAGEMONITOR, "An error has been thrown from Fed9UVoltageMonitor") \
  IC_DEF_ERROR(ERROR_FED9USYSTEMACE, "An error has been thrown from Fed9USystemAce") \
  IC_DEF_ERROR(ERROR_CONFIGUREFED9U, "An error has been thrown from Fed9UVmeBase")

/**
 * \brief  Class for error handling within Fed9UVmeDevice.
 * \author Gareth Rogers, RAL
 * \date   2003/07
 *
 * This class provides a list of errors which can occur in the Fed9UVmeDevice and can also be thrown
 * as part of the exception type Fed9UVmeDeviceException.
 */
  IC_EXCEPTION_CLASS_BEGIN(Fed9UVmeDeviceException, FED9U_VME_DEVICE_EXCEPTION_CODE_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END
}

#endif//_Fed9UVmeDeviceException_H_
