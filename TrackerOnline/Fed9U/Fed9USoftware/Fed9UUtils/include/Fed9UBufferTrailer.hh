#ifndef FED9UBUFFERTRAILER_H
#define FED9UBUFFERTRAILER_H

#include "ICExDecl.hh"

namespace Fed9U {

  using ICUtils::ICException;

  class Fed9UBufferDescription;

  /**
   * \brief  This class provides methods to construct the trailer part of the Fed9U buffer.
   * \author Matthew Pearson, RAL.
   * \date   02/2005
   *
   * It should be used to compose classes which inherit from Fed9UBufferCreator.
   */
  class Fed9UBufferTrailer {

  public:

    /**
     * \brief Method which attaches the DAQ trailer to the end of the FED buffer.
     * \param theBufDesc Pointer to the buffer description that is to be updated.
     * \throw ICException Throws this exception for all error and catches all errors and rethrows as this exception type.
     */
    void generateFed9UDaqTrailer(Fed9UBufferDescription *theBufDesc) noexcept(false);

  };

}

#endif //FED9UBUFFERTRAILER_H
