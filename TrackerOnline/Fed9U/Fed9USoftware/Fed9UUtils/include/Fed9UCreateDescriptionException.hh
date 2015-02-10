#ifndef _Fed9UCreateDescriptionException_H_
#define _Fed9UCreateDescriptionException_H_
#include "ICExDecl.hh"

namespace Fed9U {

  /**
   * \brief  Defines the error list for use with the class Fed9UCreateDescription.
   * \author Gareth Rogers, RAL
   * \date   05/2004
   *
   * Possible errors defined in the error list are:
   * \li ERROR_CREATE_DESCRIPTION Error occured while creating the description object.
   */
#define FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST \
  IC_DEF_ERROR(ERROR_CREATE_DESCRIPTION, "An error has been thrown from Fed9UCreateDescription.") \

  IC_EXCEPTION_CLASS_BEGIN(Fed9UCreateDescriptionException, FED9U_CREATE_DESCRIPTION_EXCEPTION_CODE_LIST)
    // User defined methods go here
  IC_EXCEPTION_CLASS_END
}

#endif
