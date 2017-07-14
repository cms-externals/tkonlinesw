#include "Fed9UFakeBufferCreator.hh"
#include "Fed9UBufferGenerator.hh"
#include "Fed9UBufferCreator.hh"
#include "Fed9UBufferCreatorRaw.hh"
#include "Fed9UBufferCreatorScopeMode.hh"
#include "ICAssert.hh"
#include "Fed9UDescription.hh"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <climits>
#include <bitset>
#include <cstring>
using namespace std;

namespace Fed9U {

//added support for multiple channels (unique channels must divide 96 evenly) (AAB 8/18/2015)
//passing in a '1' for "unique channels" gives the old performance
  Fed9UFakeBufferCreator::Fed9UFakeBufferCreator(istream & is, int uniqueChannels) : _mode(FED9U_EVENT_MODE_FAKE_FULL), _size(0) {
    // fileType can be 0, 49 or 50.  0 means binary full raw event buffer, 49 means single APV frame file in text, 
    // 50 means full FED data in text read back from Fake Event registers.
    u8 fileType;
    // read in the file type from event file
    is.read(reinterpret_cast<char*>(&fileType),1);

    // in binary file mode we can read the binary file in directly
    if (fileType == 0 ) {
      // First read in the size of the FED buffer, this is stored in the first 4 bytes
      is.read(reinterpret_cast<char*>(&_size),4);
      //create a buffer that will be managed by the fed9UEvent class in this instance (we have set _deleteBuffer to true)
      _buffer = new u32[_size/4]; //the size stored in the buffer file is number of bytes.
      // read in the buffer of length size bytes.
      is.read(reinterpret_cast<char*>(_buffer),static_cast<int>(_size));
      _mode =  FED9U_EVENT_MODE_FAKE_FULL;
    } 
    else if(fileType == 49) { // in text APV frame file mode we must read in the text and interpret them into a FED buffer
      // text fed buffer is simply one sample per line, for both apvs in the channel... thus 280 samples for the complete frame.
      const unsigned short strips_per_fed = 96 * 256;
      vector<unsigned short> data_buffer;
      data_buffer.reserve(strips_per_fed);
      Fed9U::Fed9UBufferCreator* creator = new Fed9U::Fed9UBufferCreatorRaw();

      // generate FED buffer
      Fed9U::Fed9UBufferGenerator generator( creator );
      
      vector<u16> samples;
      u16 sample;

      //changed to support loading in more than 1 unique channel if wanted (AAB 8/18/2015)
      for (u32 i = 0 ; i < 256*uniqueChannels ; i ++ ) {
	is >> sample;
	samples.push_back(sample);
      }
      
      for ( u32 j = 0 ; j < 96/uniqueChannels ; j ++ ) {
	for (u32 i = 0 ; i < 256*uniqueChannels ; i ++ ) {
	  data_buffer.push_back(samples[i]);
	}
      }
      generator.generateFed9UBuffer( data_buffer );

      // calculate size of FED buffer in units of bytes (unsigned char)
      _size = generator.getBufferSize() * 4;      
      _buffer = new u32[_size/4];
      
      // copy FED buffer to buffer using Fed9UBufferGenerator
      unsigned int* ints = reinterpret_cast<unsigned int*>( _buffer );
      generator.getBuffer( ints );
     
      _mode =  FED9U_EVENT_MODE_FAKE_SINGLE;
    }
    else if(fileType == 50) { // if we are in text APV frame file mode we must read in the text and interpret them into a FED buffer
      // text fed buffer is simply one sample per line, for both apvs in the channel... thus 280 samples for the complete frame.
      const unsigned short strips_per_fed = 96 * 256;
      vector<unsigned short> data_buffer;
      data_buffer.reserve(strips_per_fed);
      Fed9U::Fed9UBufferCreator* creator = new Fed9U::Fed9UBufferCreatorRaw();
      
      // generate FED buffer
      Fed9U::Fed9UBufferGenerator generator( creator );
      u16 sample;
      for (u32 i = 0 ; i < (256 * 96) ; i ++ ) {
	is >> sample;
	data_buffer.push_back(sample);
      }
      generator.generateFed9UBuffer( data_buffer );
      
      // calculate size of FED buffer in units of bytes (unsigned char)
      _size = generator.getBufferSize() * 4;      
      _buffer = new u32[_size/4];
      
      // copy FED buffer to buffer using Fed9UBufferGenerator
      unsigned int* ints = reinterpret_cast<unsigned int*>( _buffer );
      generator.getBuffer( ints );
     
      _mode =  FED9U_EVENT_MODE_FAKE_FULL;
    }
    else {
      ICUTILS_VERIFY(1==2).error().msg("Fed9UEvent input stream constructor does not recognise this input format");
    }
  }

  Fed9UFakeBufferCreator::~Fed9UFakeBufferCreator() {
    delete _buffer;
  }
  
}

