/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2015, Christian Bonnin, Strasbourg, Institut Pluridisciplinaire Hubert Curien
*/

#ifndef FECUTCARINGDEVICE_H
#define FECUTCARINGDEVICE_H

#include "tscTypes.h"
#include "FecExceptionHandler.h"
#include "FecRingDevice.h"
#include "uhal/uhal.hpp"


/** This class gives all the methods to read / write a word in the different registers of the uTCA FEC ie.:
 * FIFO receive, FIFO transmit, FIFO return, control register 0 and 1, status register 0 and 1
 * This class inherits from FecRingDevice in order to have the same methods for the type of FECs: VME, PCI, USB, uTCA
 * This class inherits from FecRingDevice which contains all the high level methods for CCU, channels (i2c, pia, memory) 
 */
class FecUtcaRingDevice: public FecRingDevice {

 private:
        //static std::string strUhalConnectionFile_, strBoardId_;
        bool useNonIncBlocks;
        std::string strError;
///uHal board object
	static uhal::HwInterface *hwBoard;
        /** Send one command to the FEC local bus 
        * @param uCmd Command
        * @param uSetValue Write value
        * @param pGetValue Pointer to the return value. Null for write commands
        * @return True if an error occured */
        bool sendCommand(uint32_t uCmd, tscType32 uSetValue, tscType32 *pGetValue);
        /** Read a control or a status register value
        * @param cRegType C for a control or S for a status register
        * @param cRegNum Register number (0 or 1)
        * @return Read value */
        tscType32 readRegisterValue(char cRegType, char cRegNum);
        /** Write a given value into a control register */
        void setControlRegister(char cRegNum, tscType16 uValue);

 public:

  /** Number of slot in the uTCA bus
   */
  static const unsigned int minUtcaFecSlot = 0 ;
  static const unsigned int maxUtcaFecSlot = 0 ;

  /** Number of ring per uTCA FEC
   */
  static const unsigned int minUtcaFecRing= 0 ;
  static const unsigned int maxUtcaFecRing= 3 ;
  /** \brief Build an access to the uTCA FEC
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param init - initialise or not the FEC ring
   */
  FecUtcaRingDevice ( tscType8 fecSlot, tscType8 ringSlot, bool init = true, bool invertClockPolarity = false ) throw ( FecExceptionHandler ) ;

 /** \brief Close the device driver
  */
  virtual ~FecUtcaRingDevice ( ) throw ( FecExceptionHandler ) ;
/** Initialize uHAL connection */
  static void configureUhal (const std::string& connectionFile, const std::string& boardId);
/** Release uHAL connection object */
  static void releaseUhal();
  /******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
  ******************************************************/

  /** \brief Set the control register 0  the FEC
   */ 
  void setFecRingCR0 ( tscType16 ctrl0Value, bool force = false )  throw ( FecExceptionHandler ) ;

  /** \brief Get the control register 0  the FEC
   * \return value read
   */
  tscType16 getFecRingCR0( ) throw ( FecExceptionHandler ) ;

  /** \brief Set the control register 1  the FEC
   * \param ctrl1Value - value to be set
   */ 
  void setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler ) ;

  /** \brief Get the control register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingCR1( ) throw ( FecExceptionHandler ) ;

  /** \brief Get the status register 0  the FEC
   * \return value read
   */
  tscType32 getFecRingSR0(unsigned long ) throw ( FecExceptionHandler ) ;

  /** \brief Get the status register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingSR1( ) throw ( FecExceptionHandler ) ;
	
  /** \brief return the firmware version of the FEC
   */
  tscType16 getFecFirmwareVersion( ) throw ( FecExceptionHandler ) ;

/** Read a string describing the Firmware version */
  std::string getFecFirmwareStringVersion( );
/** Monitoring status of the current ring
  \param bTx True for Tx and False for Rx status
  \return Byte with the following bit meaning: 0:Clock ring A, 1:Data ring A, 2:Clock ring B, 3:Data ring B
  */
  uint8_t getMonitoringStatus(bool bTx);

  /******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
   ******************************************************/

  /** \brief return a word from the FIFO receive
   */
  tscType32 getFifoReceive( ) throw ( FecExceptionHandler ) ;
	
  /** \brief write a word in the FIFO receive
   * \param value - value to be written
   */
  void setFifoReceive( tscType32 value ) throw ( FecExceptionHandler ) ;
  
  /** \brief return a word from the FIFO return
   */
  tscType8 getFifoReturn( )  throw ( FecExceptionHandler ) ;

  /** \brief write a word in the FIFO return
   * \param value - value to be written
   */
  void setFifoReturn( tscType8 value )  throw ( FecExceptionHandler ) ;
  
  /** \brief return a word from the FIFO transmit
   */
  tscType32 getFifoTransmit( )  throw ( FecExceptionHandler ) ;

  /** \brief write a word in the FIFO transmit
   * \param value - value to be written
   */
  void setFifoTransmit( tscType32 value )  throw ( FecExceptionHandler ) ;


   /******************************************************
	HARD RESET
    ******************************************************/	

  /** \brief issue a PLX reset
   */
  void fecHardReset ( )  throw ( FecExceptionHandler ) ;

  /******************************************************
		IRQ enable / disable
   ******************************************************/	

  /** \brief Enable or disable the PLX irq (level is not used)
   */
  void setIRQ ( bool enable, tscType8 level=1 ) throw ( FecExceptionHandler ) ;
 /** Empty the RECeive FIFO with a readBlock after reading its size
 * @return Content of the FIFO */
  std::vector<uint32_t> readBlockReceiveFifo();
  const std::string& getError() const {return strError;}
} ;

#endif

