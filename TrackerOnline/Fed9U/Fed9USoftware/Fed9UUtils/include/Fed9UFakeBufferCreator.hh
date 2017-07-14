/** Jo Cole September 2006 */

#ifndef H_Fed9UFakeBufferCreator
#define H_Fed9UFakeBufferCreator

#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#include "Fed9UAddress.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  using namespace std;

  /** New fake event buffer construction class.  Used by streamlined event unpacking objects when using fake events.*/

  class Fed9UFakeBufferCreator {
    
  public:
    /** Default constructor. */
    Fed9UFakeBufferCreator() : _mode(FED9U_EVENT_MODE_FAKE_FULL), _size(0) {};
    /** Standard constructor. */
    //added new parameter, see .C file for details (8/18/2015 AAB)
    Fed9UFakeBufferCreator(istream & is, int uniqueChannels = 1);

    /** Default destructor. */
    ~Fed9UFakeBufferCreator();

    /** Returns the size of the created buffer (in bytes).*/
    u32 getBufferSize() const {return _size;}
    /** Returns a point to the created buffer.*/
    u32 * getPointerToBuffer() const {return _buffer;}
    /** Returns the data mode (required by Fed9UEventStreamLine when using fake events).*/
    Fed9UEventDataMode getDataMode() const {return _mode;}

  private:
    u32 * _buffer;
    Fed9UEventDataMode _mode;
    u32 _size; // buffer size in bytes
    
  };
  
}

#endif // H_Fed9UFakeBufferCreator
