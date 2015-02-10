#ifndef FED9UBUFFERCREATOR_H
#define FED9UBUFFERCREATOR_H

namespace Fed9U {
  
  class Fed9UBufferDescription;

  /**
   * \brief  An abstract base class to provide an interface for creating Fed9U buffers.
   * \author Matthew Pearson, RAL.
   * \date   02/2005.
   */
  class Fed9UBufferCreator {
    
  public:
    
    /**
     * \brief A pure virtual function that is used to generate the Fed9U buffer in a given implementation.
     * \param Fed9UBufferDescription* Pointer to a description of the buffer that is defined by the Fed9UBufferDescription.
     *        It is from this that implementations of this method should generate the buffer.
     */
    virtual void generateBuffer(Fed9UBufferDescription *) = 0;
    
    /**
     * \brief Destructor.
     */
    virtual ~Fed9UBufferCreator() {};
    
  protected:
    
    /**
     * \brief Constructor.
     *
     * Declared protected as this is a pABC only dervied classes can instantiate an object.
     */
    Fed9UBufferCreator(void) {};
  
  };
  
}

#endif //FED9UBUFFERCREATOR_H
