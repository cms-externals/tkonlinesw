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
  
  Copyright 2003 - 2004, Frederic DROUHIN, Laurent Gross - Universite de Haute-Alsace, Mulhouse-France, Institut de Recherche Subatomique de Strasbourg
*/

#ifndef FECPCIRINGDEVICE_H
#define FECPCIRINGDEVICE_H

#include "tscTypes.h"
#include "FecExceptionHandler.h"
#include "FecRingDevice.h"
#include "datatypes.h"
#include "glue.h"

#define PCIDRIVERMODE (O_RDWR | O_NONBLOCK)
#define DEVICEDRIVERNAME "/dev/fecpmc%02d"

/** This class gives all the methods to read / write a word in the different registers of the PCI FEC ie.:
 * FIFO receive, FIFO transmit, FIFO return, control register 0 and 1, status register 0 and 1
 * This class works for any kind of PCI FEC, eletrical (latice and PLX one) and optical
 * This class inherits from FecRingDevice in order to have the same methods for the type of FECs: VME, PCI, USB
 * This class inherits from FecRingDevice which contains all the high level methods for CCU, channels (i2c, pia, memory) 
 */
class FecPciRingDevice: public FecRingDevice {

 private:
  /** Device driver descriptor for the FEC Driver
   */
  int deviceDescriptor_ ;

 public:

  /** Number of slot in the PCI bus
   */
  static const unsigned int MINPCIFECSLOT = 0 ;
  static const unsigned int MAXPCIFECSLOT = 4 ;

  /** Number of ring per PCI FEC
   */
  static const unsigned int MINPCIFECRING = 0 ;
  static const unsigned int MAXPCIFECRING = 0 ;

  /** \brief Build an access to the PCI FEC
   * \param fecSlot - FEC slot
   * \param ringSlot - ring slot on the corresponding FEC
   * \param init - initialise or not the FEC ring
   */
  FecPciRingDevice ( tscType8 fecSlot, tscType8 ringSlot, bool init = true, bool invertClockPolarity = false ) noexcept(false) ;

 /** \brief Close the device driver
  */
  virtual ~FecPciRingDevice ( ) noexcept(false) ;


  /******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
  ******************************************************/

  /** \brief Set the control register 0  the FEC
   */ 
  void setFecRingCR0 ( tscType16 ctrl0Value, bool force = false )  noexcept(false) ;

  /** \brief Get the control register 0  the FEC
   * \return value read
   */
  tscType16 getFecRingCR0( ) noexcept(false) ;

  /** \brief Set the control register 1  the FEC
   * \param ctrl1Value - value to be set
   */ 
  void setFecRingCR1( tscType16 ctrl1Value ) noexcept(false) ;

  /** \brief Get the control register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingCR1( ) noexcept(false) ;

  /** \brief Get the status register 0  the FEC
   * \return value read
   */
  tscType32 getFecRingSR0( ) noexcept(false) ;

  /** \brief Get the status register 1  the FEC
   * \return value read
   */
  tscType16 getFecRingSR1( ) noexcept(false) ;
	
  /** \brief return the firmware version of the FEC
   */
  tscType16 getFecFirmwareVersion( ) noexcept(false) ;

  /******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
   ******************************************************/

  /** \brief return a word from the FIFO receive
   */
  tscType32 getFifoReceive( ) noexcept(false) ;
	
  /** \brief write a word in the FIFO receive
   * \param value - value to be written
   */
  void setFifoReceive( tscType32 value ) noexcept(false) ;
  
  /** \brief return a word from the FIFO return
   */
  tscType8 getFifoReturn( )  noexcept(false) ;

  /** \brief write a word in the FIFO return
   * \param value - value to be written
   */
  void setFifoReturn( tscType8 value )  noexcept(false) ;
  
  /** \brief return a word from the FIFO transmit
   */
  tscType32 getFifoTransmit( )  noexcept(false) ;

  /** \brief write a word in the FIFO transmit
   * \param value - value to be written
   */
  void setFifoTransmit( tscType32 value )  noexcept(false) ;


   /******************************************************
	HARD RESET
    ******************************************************/	

  /** \brief issue a PLX reset
   */
  void fecHardReset ( )  noexcept(false) ;

  /******************************************************
		IRQ enable / disable
   ******************************************************/	

  /** \brief Enable or disable the PLX irq (level is not used)
   */
  void setIRQ ( bool enable, tscType8 level=1 ) noexcept(false) ;
} ;

#endif
