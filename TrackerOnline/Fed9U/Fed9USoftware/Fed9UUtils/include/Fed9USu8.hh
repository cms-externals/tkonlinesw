/** Jo Cole February 2007 */

#ifndef H_Fed9USu8
#define H_Fed9USu8

#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#include "Fed9UAddress.hh"
#include <iosfwd>
#include <vector>
#include <iostream>

namespace Fed9U {

  //<JEC date=21/03/07>
  /** Replacement for Fed9UEventIterator class.  Used in Fed9UEventStreamLine and Fed9UEventUnitStreamLine.*/
  
  class Fed9USu8 {
    
  public:
    /** Default constructor. */
    Fed9USu8() : _dataPtr(0), _off(0), _initialOffset(0) {}
    /** Standard constructor. */
    Fed9USu8(u8 * dataPtr) : _dataPtr(dataPtr), _off(0), _initialOffset(0) {}
    Fed9USu8(u8 * dataPtr, u32 offset) : _dataPtr(dataPtr), _off(offset), _initialOffset(offset) {}
    /** Default destructor. */
    ~Fed9USu8() {}

    /** Separate method to set the private data members. */
    void set(u8 * dataPtr = 0, u32 offset = 0) {_dataPtr = dataPtr; _off = offset; _initialOffset = offset;}
    /** Reset the offset to whatever it was either when it was instantiated or when the last set command was called.*/
    void reset() {_off = _initialOffset;}
    /** Delete the pointer.*/
    void clear() {  if (_dataPtr != NULL ) delete[] _dataPtr; }
    /** Return the value from the buffer at a specific offset from where the buffer is pointing to.
     *  Bear in mind that if _off != 0, it will play a role in deciding which value you return.
     */
    u8 getu8(u32 offset) const {return (*this)[offset];}
    /** Determines if the pointer has been set yet or not.*/
    bool comparePtr() const {
      if (_dataPtr == NULL) {return true;}
      return false;
    }

    /** getter for the underlying u8 pointer.*/
    u8 * getPointer() const {return _dataPtr;}
    /** getter for the value of the offset.*/
    u32 getOffset() const {return _off;}
    
    /** overloaded [] operator.  Takes care of the data packing versus data access ordering in the buffer. */
    u8 & operator [] (u32 offset) const {return _dataPtr[(_off + offset) ^ 3];}
    /** overloaded dereference operator.  Returns value at the current location of the pointer. 
     *  NB.  The value of _off matters !*/
    u8 & operator * () const { return (*this)[0]; }

    /** overloaded prefix and postfix increment operators.*/
    Fed9USu8 & operator ++ () {++_off; return *this;}
    Fed9USu8 operator ++ (int) {
      Fed9USu8 temp(*this);
      (*this) += 1;
      return temp;
    }

    /** overloaded increment and assign operator.*/
    const Fed9USu8 & operator += (u32 rhs) {_off += rhs; return *this;}
    const Fed9USu8 & operator += (int rhs) {_off += static_cast<u32>(rhs); return *this;}

    /** overloaded + operator.*/
    const Fed9USu8 & operator + (u32 rhs) {_off += rhs; return *this;}
    
    /** overloaded prefix decrement operator and decrement and assign operator.*/
    Fed9USu8 & operator --() {
      ICUTILS_VERIFY(_off > 0)(_off).error();
      --_off; return *this;
    }
    Fed9USu8 & operator -=(u32 rhs) {
      ICUTILS_VERIFY(_off > rhs)(_off)(rhs).error();
      _off -= rhs; return *this;
    }
    
    /** overloaded assignment operator*/
    const Fed9USu8 & operator =(const Fed9USu8 & rhs) {
      if (this == &rhs) {return *this;}
      _dataPtr = rhs.getPointer();
      _off = rhs.getOffset();
      _initialOffset = rhs.getInitialOffset();
      return *this;
    }
    
  private:
    /** getter for the initial offset value.  This should only be needed within this class, as initialOffset should only
     * be set when the object is instantiated or the set command is called, when it is set equal to the main offset.
     * Needed for the assignment operator.*/
    u32 getInitialOffset() const {return _initialOffset;}
    
    /** The actual pointer for which this class is a wrapper.*/
    u8 * _dataPtr;
    /** _off is an offset which is needed if you want to access anything other than the first byte of the 
     * buffer when you use the overloaded dereference operator.
     *
     * _initialOffset is an extra offset in case you want to reset the value of _off.  This is needed
     * in Fed9UEventUnitStreamLine when returning a Fed9USu8 special pointer.*/
    u32 _off, _initialOffset;
  };
  //</JEC>
}

#endif // H_Fed9USu8
