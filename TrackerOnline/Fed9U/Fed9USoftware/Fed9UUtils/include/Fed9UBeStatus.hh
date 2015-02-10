#ifndef H_Fed9UBeStatus
#define H_Fed9UBeStatus

#include "TypeDefs.hh"

#include <iostream>
#include <sstream>
#include <string>


namespace Fed9U {

  //using.*std::istream;
  //using.*std::ostream;

  /**
   * \brief  Describes the contents of the Be Status Register.
   * \author Jonathan Fulcher
   *
   * It has methods to return the individual values or a string listing any errors that have occured.
   * It has no access to the actual FED registers and must be constructed using a previously read value
   * from the FED.
   */
  class Fed9UBeStatus {

  public:

    /**
     * \brief Constructor.
     * \param status An unsigned integer that has been read from the FED BE FPGA status register.
     */
    Fed9UBeStatus( u32 status): _status(status){}
    
    /**
     * \brief Destructor.
     */
    ~Fed9UBeStatus() {}
    

    /**
     * \brief  Returns a string which describes any errors which are set within the be status register
     * \return string The string containing the error list.
     */
    std::string getStatusString() const  {
      std::stringstream info;
      info << "Be Status Register Info: "
	   << ( getInternalFreeze()?"Internal Freeze! ":"" )
	   << ( getSLinkDown()?"S-link Link Down! ":"" )
	   << ( getSLinkFull()?"S-Link Link Full! ":"" )
	   << ( getBackPressure()?"Back Pressure Flag Enabled! ":"")
	   << ( getBxErrorAtOrbitReset()?"BX Error At Orbit Reset! ":"")
	   << ( getTtcReady()?"TTC Ready! ":"TTC Not Ready! ")
	   << ( getTrackerHeaderMonitorDataReady()?"Tracker Header Monitor Data Ready! ":"Tracker Header Monitor Data NOT Ready! ")
	   << ( getQdrFull()?"QDR Full! ":"")
	   << ( getFrameAddressFifoFull()?"Frame Address FIFO Full! ":"")
	   << ( getTotalLengthFifoFull()?"Total Length FIFO Full! ":"")
	   << ( getTrackerHeaderFifoFull()?"Tracker Header FIFO Full! ":"")
	   << ( getL1ABxFifoFull()?"L1A and BX FIFO Full! ":"")
	   << ( getFeEventLengthFifoFull()?"FE Event Length FIFO Full! ":"")
	   << ( getFeFpgaFull()?"Front End FPGA Full! ":"")
	   << ( getQdrPartialFull()?"QDR Partially Full! ":"")
	   << ( getFrameAddressFifoPartialFull()?"Frame Address Fifo Partially Full! ":"")
	   << ( getTotalLengthFifoPartialFull()?"Total Length Fifo Partially Full! ":"")
	   << ( getTrackerHeaderFifoPartialFull()?"Tracker Header FIFO Partially Full! ":"")
	   << ( getL1ABxFifoPartialFull()?"L1A & BX FIFO Partially Full! ":"")
	   << ( getFeEventLengthFifoPartialFull()?"FE Event Length FIFO Partially Full! ":"")
	   << ( getFeFpgaPartialFull()?"FE Fpga Partially Full! ":"")
	   << ( getQdrEmpty()?"QDR Empty! ":"")
	   << ( getFrameAddressFifoEmpty()?"Frame Address FIFO Empty! ":"" )
	   << ( getTotalLengthFifoEmpty()?"Total Length FIFO Empty! ":"") 
	   << ( getTrackerHeaderFifoEmpty()?"Tracker Header FIFO Empty! ":"")
	   << ( getL1ABxFifoEmpty()?"L1A & BX FIFO Empty! ":"")
	   << ( getFeEventLengthFifoEmpty()?"Fe Event Length FIFO Empty! ":"")
	   << ( getFeFpgaEmpty()?"FE FPGA Empty! ":"")
	   << std::endl;
      return info.str();
    }

    /**
     * \name Getters
     *
     * All return booleans and a boolean true indicates that the error condition is present and boolean false it is not.
     */
    //@{

    /**
     * \brief  When the FIFO becomes full the internal freeze is asserted to prevent over writing of event data.
     * \return bool Boolean true if the internal freeze is present, false if not.
     *
     * Once the internal freeze has been asserted the BE FPGA must be issued a soft reset command to reset of the counters
     */
    bool getInternalFreeze() const {
      return (_status >> 1) & 0x1;
    }

    /**
     * \brief  Status of the S-link connection.
     * \return bool Boolean true if the S-link is down, false if not.
     */
    bool getSLinkDown() const {
      return (_status >> 2) & 0x1;
    }

    /**
     * \brief  A status bit to indicate when the S-link event buffer is full.
     * \return bool Boolean true if the S-link buffer is full, false if not.
     */
    bool getSLinkFull() const {
      return (_status >> 3) & 0x1;
    }

    /**
     * \brief  When either the VME or S-link event readout buffers become full, the back pressure signal is asserted until that event fragment is readout.
     * \return bool Boolean true if back pressure is asserted, false if not.
     */
    bool getBackPressure() const {
      return (_status >> 4) & 0x1;
    }

    /**
     * \brief  Indicates if there was an error with the bunch crossing when an orbit reset was issued.
     * \return bool Boolean true if there an error is present, false if not.
     *
     * In order for this flag to operate correctly the bunch crossing offset must be set before the bunch crossing total is set.
     * This flag is clear once the bunch crossing total has been read.
     */
    bool getBxErrorAtOrbitReset() const {
      return (_status >> 5) & 0x1;
    }

    /**
     * \brief  Indicates the status of the TTC
     * \return bool Boolean true if the TTC is ready, false if not.
     */
    bool getTtcReady() const {
      return (_status >> 6) & 0x1;
    }

    /**
     * \brief  Monitors the construction of the tracker header, in order to know when it has been constructed correctly.
     * \return bool Boolean true if the header data is ready, false if not.
     */
    bool getTrackerHeaderMonitorDataReady() const {
      return (_status >> 7) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is full, false if not.
     */
    bool getQdrFull()  const {
      return (_status >> 8) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getFrameAddressFifoFull() const  {
      return (_status >> 9) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getTotalLengthFifoFull() const {
      return (_status >> 10) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getTrackerHeaderFifoFull() const {
      return (_status >> 11) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getL1ABxFifoFull() const {
      return (_status >> 12) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is full, false if not.
     */
    bool getFeEventLengthFifoFull() const {
      return (_status >> 13) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if a FE FPGA is full, false if not.
     */
    bool getFeFpgaFull() const {
      return (_status >> 14) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is partially (87%) full, false if not.
     */
    bool getQdrPartialFull() const {
      return (_status >> 16) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getFrameAddressFifoPartialFull() const {
      return (_status >> 17) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getTotalLengthFifoPartialFull() const {
      return (_status >> 18) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getTrackerHeaderFifoPartialFull() const {
      return (_status >> 19) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getL1ABxFifoPartialFull() const {
      return (_status >> 20) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is partially (87%) full, false if not.
     */
    bool getFeEventLengthFifoPartialFull() const {
      return (_status >> 21) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if the FE FPGA is partially (87%) full, false if not.
     */
    bool getFeFpgaPartialFull() const {
      return (_status >> 22) & 0x1;
    }

    /**
     * \brief  The QDRs are used to store the event frames once they have been constructed.
     * \return bool Boolean true if the QDR is empty, false if not.
     */
    bool getQdrEmpty() const {
      return (_status >> 24) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the addresses of the frames that are being/have been processed by the BE FPGA before being readout. 
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getFrameAddressFifoEmpty() const {
      return (_status >> 25) & 0x1;
    }

    /**
     * \brief  The total length of each event that is present in the BE FPGA QDRs is saved in a FIFO.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getTotalLengthFifoEmpty() const {
      return (_status >> 26) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the tracker header for each event stored in the BE FPGA QDRs.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getTrackerHeaderFifoEmpty() const {
      return (_status >> 27) & 0x1;
    }

    /**
     * \brief  A FIFO exists to store the level 1 accepts and bunch crossing counters that are written to the DAQ header.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getL1ABxFifoEmpty() const {
      return (_status >> 28) & 0x1;
    }

    /**
     * \brief  A FIFO is used to store the length of the events sent from each FE unit to the BE FPGA.
     * \return bool Boolean true if the FIFO is empty, false if not.
     */
    bool getFeEventLengthFifoEmpty() const {
      return (_status >> 29) & 0x1;
    }

    /**
     * \brief  As data is received from the OptoRxs it is stored in the FE FPGAs before being sent on to the BE FPGA for event construction.
     * \return bool Boolean true if the FE FPGA buffer is empty, false if not.
     */
    bool getFeFpgaEmpty() const {
      return (_status >> 30) & 0x1;
    }
    //@}

  private:
    u32 _status; //!< Holds the status word, from which the status bits are decoded.
  };

  //using.*std::istream;
  //using.*std::ostream;

  std::ostream& operator<<(std::ostream& os, const Fed9UBeStatus& sd);
  
}

#endif // H_Fed9UBeStatus
