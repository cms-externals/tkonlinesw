#ifndef H_Fed9UCrc
#define H_Fed9UCrc

#include "TypeDefs.hh"

namespace Fed9U{
  //calculate the CRC for a FED buffer ignoring the CRC bytes. Pass true for the final argument if the 32bit word order is swapped (ie VME order)
  u16 calculateFEDBufferCRC(const u8* buffer, const size_t length, bool wordSwap);
}

#endif //ndef H_Fed9UCrc
