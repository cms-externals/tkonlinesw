#include "Fed9USpyApvData.hh"

namespace Fed9U {
  
  u16 Fed9USpyApvData::getSample(u16 index) const noexcept(false)
  {
    ICUTILS_VERIFY(index < SAMPLES_PER_APV)(index).msg("Trying to get sample with index greater than number of samples per APV.")
    .error().code(Fed9USpyApvDataException::ERROR_OUT_OF_RANGE);
    return samples_[index];
  }
  Fed9USpyApvData& Fed9USpyApvData::setSample(u16 index, i16 value) noexcept(false)
  {
    ICUTILS_VERIFY(index < SAMPLES_PER_APV)(index).msg("Trying to set sample with index greater than number of samples per APV.")
    .error().code(Fed9USpyApvDataException::ERROR_OUT_OF_RANGE);
    samples_[index] = value; return *this;
  }
  u8 Fed9USpyApvData::getTickOffset(i16 threshold) const noexcept(false)
  {
    u8 tickOffset=0;
    //skip low bits at the begining
    while ( samples_[tickOffset]<threshold && tickOffset<SAMPLES_PER_APV ) tickOffset++;
    ICUTILS_VERIFY(tickOffset<SAMPLES_PER_APV-12).msg("No samples above threshold in range.")
    .error().code(Fed9USpyApvDataException::ERROR_TICK_MARK_NOT_FOUND);
    ICUTILS_VERIFY((samples_[tickOffset]>threshold) && (samples_[tickOffset+1]>threshold) && (samples_[tickOffset+2]>threshold))
    .msg("Tick mark too short.").error().code(Fed9USpyApvDataException::ERROR_TICK_MARK_NOT_FOUND);
    return tickOffset;
  }
  bool Fed9USpyApvData::getErrorBit(i16 threshold) const noexcept(false)
  {
    u8 errorBitOffset = 11;	//3 samples for tick marks, 8 for address
    try {
      errorBitOffset += getTickOffset(threshold);
    } catch (const Fed9USpyApvDataException& e) {
      RETHROW(e,Fed9USpyApvDataException(Fed9USpyApvDataException::ERROR_APV_HEADER, "Cannot get tick offset. Need tick mark to find start of header. "));
    }
    return samples_[errorBitOffset] > threshold;
  }
  u8 Fed9USpyApvData::getPipelineAddress(i16 threshold) const noexcept(false)
  {
    u8 result = 0x0;
    //find the tick mark
    u8 tickOffset;
    try {
      tickOffset += getTickOffset(threshold);
    } catch (const Fed9USpyApvDataException& e) {
      RETHROW(e,Fed9USpyApvDataException(Fed9USpyApvDataException::ERROR_APV_HEADER, "Cannot get tick offset. Need tick mark to find start of header. "));
    }
    //skip tick mark
    tickOffset+=3;
    //loop over bits in header
    for (int k=0; k<8; k++) {
      bool high=false;	//bool to store if this bit is high
      high = samples_[k+tickOffset]>threshold;
      if (high) result |= (0x80>>k);
    }
    return result;
  }
  
} //namespace Fed9U
