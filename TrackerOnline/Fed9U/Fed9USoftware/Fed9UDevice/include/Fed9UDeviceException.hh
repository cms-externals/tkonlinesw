#ifndef H_Fed9UDeviceException
#define H_Fed9UDeviceException

#include "ICExDecl.hh"
#include <string>

namespace Fed9U {

  /**
   * \class Fed9UDeviceException
   * \brief Exception class to be used when throwing errors within the Fed9UDevice class
   * \author Jonathan Fulcher 
   * \author Emlyn Corrin
   * \version 0.0
   * \date 16 Jul 2003
   * 
   ***********************************************/

#define FED9U_DEVICE_EXCEPTION_CODES_LIST \
  IC_DEF_ERROR(ERROR_FED_READOUT, "Unable to read event from FED") \
  IC_DEF_ERROR(ERROR_FED_TRIM_DAC_CAL, "Unable to complete TrimDAC calibration routine") \
  IC_DEF_ERROR(ERROR_FED_INIT, "FED failed to initialize") \
  IC_DEF_ERROR(ERROR_FED_SOAK_TEST, "Fed9UDevice::soakTest aborted. Unable to handle exception.") \
  IC_DEF_ERROR(ERROR_MEMORY, "Failed to allocate memory.") \
  IC_DEF_ERROR(ERROR_FED_REGISTER_READ, "Error reading FED register.") \
  IC_DEF_ERROR(ERROR_FED_SPY_READOUT, "Failed to read spy channel data.")

  IC_EXCEPTION_CLASS_BEGIN(Fed9UDeviceException, FED9U_DEVICE_EXCEPTION_CODES_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END

}

#endif // H_Fed9UDeviceException
