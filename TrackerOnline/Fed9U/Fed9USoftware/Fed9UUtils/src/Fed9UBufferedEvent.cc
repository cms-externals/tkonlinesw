#include <stdint.h>
#include <inttypes.h>

#ifdef EVENT_STREAMLINE
#include "Fed9UEventStreamLine.hh"
#else
#include "Fed9UEvent.hh"
#endif
#include "Fed9UBufferedEvent.hh"
#include "ICAssert.hh"
#include "Fed9UDescription.hh"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <climits>


namespace Fed9U
{
  Fed9UBufferedEvent::Fed9UBufferedEvent():
#ifdef EVENT_STREAMLINE
    Fed9UEventStreamLine(),
#else
    Fed9UEvent(),
#endif
    _eventBufferLength(0),
    _isOnline(true)
  {
  }
  

  Fed9UBufferedEvent::Fed9UBufferedEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize):
#ifdef EVENT_STREAMLINE
    Fed9UEventStreamLine(),
#else
    Fed9UEvent(),
#endif
    _eventBufferLength(bufsize),
    _isOnline(true)
  {
    //constructs in online enviromnemt 
    loadEvent(buffer, currentDescription, bufsize);
  }
  
  
  void Fed9UBufferedEvent::loadEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize)
  {
    _eventBufferLength=bufsize;
    chargeEventBuffer(buffer, bufsize);
#ifdef EVENT_STREAMLINE
    Init(getEventBufferPointer(), currentDescription);
#else
    Init(getEventBufferPointer(), currentDescription, bufsize);
#endif
    
    //here I need to place the code that obtains the data from the Fed9UDescription.
    //then these data will be written to file... not sure what I should get at the moment
    
    _daqMode=currentDescription->getDaqMode();
  }


  Fed9UBufferedEvent::Fed9UBufferedEvent(std::istream & input, Fed9UDescription * fedDescriptionAtTheTime):
#ifdef EVENT_STREAMLINE
    Fed9UEventStreamLine(),
#else
    Fed9UEvent(),
#endif
    _isOnline(false)
  {
    //constructs from an event file stream
    
    //ICUTILS_VERIFY(input.is_open() == true)(input.is_open()).msg(" Fed9UBufferedEvent::Fed9UBufferedEvent(std::istream &) : Input file stream not open...");
    getEventFromFile(input, fedDescriptionAtTheTime);
    //Init(getEventBufferPointer(), fedDescriptionAtTheTime, _eventBufferLength);
  }
  
  
  Fed9UBufferedEvent::~Fed9UBufferedEvent(){}
  
  
  void Fed9UBufferedEvent::chargeEventBuffer(u32 * buf, u32 bufLen)
  {
    //copies contents of external event buffer into internal event buffer
    if(_eventBuffer.size()>0)//empty buffer if it contains an old event.
      _eventBuffer.clear();
    _eventBuffer.reserve(bufLen);
    for(u32 j=0;j<bufLen;j++)
      _eventBuffer.push_back(buf[j]);
  }
  

  u32 Fed9UBufferedEvent::writeBufferedEventToFile(std::ostream & output)
  {
    //cout<<"DBG:  Fed9UBufferedEvent::writeBufferedEventToFile\n        Writing "<<_eventBufferLength<<"   words to file. "<<endl;
    
    //ICUTILS_VERIFY(output.is_open() == true)(output.is_open()).msg("Fed9UBufferedEvent::writeBufferedEventToFile(std::ostream &) : Output file stream not open...");    
    output<<std::dec<<_eventBufferLength<<'\t';
    output<<_daqMode<<'\t';
    for(std::vector<u32>::iterator it=_eventBuffer.begin(); it!=_eventBuffer.end(); it++)
      output<<(*it)<<'\t';
    
    output<<'\n';//? newline for end of event ? 
    return 0;
  }
  
  
  u32 Fed9UBufferedEvent::getEventFromFile(std::istream & in, Fed9UDescription * desc)
  {
    //takes the stream object and fills the internal buffers with the contents
    
    if(_eventBuffer.size()>0)//empty buffer if it contains an old event.
      _eventBuffer.clear();
    
    in>>std::dec>>_eventBufferLength;
    //cout<<"DBG:  Fed9UBufferedEvent::getEventFromFile\n        going to read in : "<<_eventBufferLength<<"   words from file. "<<endl;
    

//ICUTILS_VERIFY(in.good()==true)(in.good()).msg("Fed9UBufferedEvent::getEventFromFile(std::istream &) : Bad input stream during event read operation.");
    _eventBuffer.reserve(_eventBufferLength);
    in>>_daqMode;
    //ICUTILS_VERIFY(in.good()==true)(in.good()).msg("Fed9UBufferedEvent::getEventFromFile(std::istream &) : Bad input stream during event read operation.");
    for(u32 j=0; j<_eventBufferLength; j++)
      {
        u32 temp=0;
        in>>temp;
        //ICUTILS_VERIFY(in.good()==true)(in.good()).msg("Fed9UBufferedEvent::getEventFromFile(std::istream &) : Bad input stream during event read operation.");
        _eventBuffer.push_back(temp);
      }

#ifdef EVENT_STREAMLINE
    Init(getEventBufferPointer(), desc);
#else
    Init(getEventBufferPointer(), desc, _eventBufferLength);
#endif
    
    return 0;
  }
  
  
  u32 * Fed9UBufferedEvent::getEventBufferPointer()
  {
    //returns a pointer to the first element in the event buffer
    //return the address of the dereferenced iterator returned by begin() 
    return &(*_eventBuffer.begin());
  }  
}
