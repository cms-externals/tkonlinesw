#ifndef H_Fed9UCounters
#define H_Fed9UCounters

#include "TypeDefs.hh"
#include <iostream>

namespace Fed9U {

  /**
   * \brief A struct to hold the value of all the counters on the FED.
   *
   * The following counters are stored in the struct, in unsigned long format:
   * \li triggerNumber Number of triggers received since last reset.
   * \li qdrFrameCount Number of frames stored in the BE FPGA QDR buffers.
   * \li qdrBufferLevel Fill level of the QDR buffer, number of 64-bit words remaining in the buffer.
   * \li qdrTotalFrameCount Total number of frames received since last reset.
   * \li feLevel Fill level of the FE unit event buffers, number of 64-bit words remaining in the buffer.
   *
   * Two constructors are provided, plus the assigment operator.
   */
  struct Fed9UCounters {

    /**
     * \name Counter members.
     *
     * Values stored in each counter data member are defined in the class documentation.
     */
    //@{
    u32 triggerNumber, qdrFrameCount, qdrBufferLevel, qdrTotalFrameCount, feLevel, bunchCount;
    //@}

    /**
     * \brief Allows each individual counter to be set.
     * \param x0 Trigger number.
     * \param x1 QDR frame count.
     * \param x2 QDR buffer level.
     * \param x3 QDR total frame count.
     * \param x4 FE level.
     */
    Fed9UCounters(u32 x0 = 0, u32 x1 = 0, u32 x2 = 0, u32 x3 = 0, u32 x4 = 0, u32 x5 = 0) :
      triggerNumber(x0), qdrFrameCount(x1), qdrBufferLevel(x2),
      qdrTotalFrameCount(x3), feLevel(x4), bunchCount(x5)
    { }

    /**
     * \brief Takes a vector for BE FPGA counters and single parameter for the FE level.
     * \param x A vector of unsigned ints containing the BE FPGA counters. Contents of each element is as follows:
     *        \li 0: Trigger number.
     *        \li 1: QDR frame count.
     *        \li 2: QDR buffer level.
     *        \li 3: QDR total frame count.
     * \param x4 FE level.
     */
    Fed9UCounters(const std::vector<u32> & x, u32 x4 = 0) :
      triggerNumber(x[0]), qdrFrameCount(x[1]), qdrBufferLevel(x[2]),
      qdrTotalFrameCount(x[3]), feLevel(x4), bunchCount(x[4])
    { }

    //Fed9UCounters & setFeLevel(int i, u32 val) { feLevel[i] = val; }

    /**
     * \brief  Comparison operator. Compares a Fed9UCounter struct to this counter struct comparing the values of the data member.
     * \param  rhs Right side Fed9UCounter object to compare to this object.
     * \return bool True if the two objects have identical values.
     */
    bool operator==(const Fed9UCounters & rhs) const {
      return triggerNumber==rhs.triggerNumber && qdrFrameCount==rhs.qdrFrameCount
	&& qdrBufferLevel==rhs.qdrBufferLevel && qdrTotalFrameCount==rhs.qdrTotalFrameCount
	&& feLevel==rhs.feLevel && bunchCount==rhs.bunchCount;
    }
  };
  
  /**
   * \brief  Output stream operator for Fed9UCounter struct.
   * \param  os Reference to the output stream to send counter values to.
   * \param  fc Fed9UCounters reference to send to the stream.
   * \return ostream Reference to the ostream used to write to.
   *
   * Parameters are sent in the order trigger number, QDR total frame count, QDR frame count and QDR buffer level.
   * FE level are not written to the stream. Explanatory text is also written. 
   */
  inline std::ostream & operator<<(std::ostream & os, const Fed9UCounters & fc) {
    return os << "Triggers=" << fc.triggerNumber
	      << ", QDR: Total events processed=" << fc.qdrTotalFrameCount
	      << " events in memory=" << fc.qdrFrameCount
	      << " data (64 bit words)=" << fc.qdrBufferLevel
	      << ", Bunch Count=" << fc.bunchCount 
      ; //<< ", FE Level=" << fc.feLevel;
  }

}

#endif // H_Fed9UCounters
