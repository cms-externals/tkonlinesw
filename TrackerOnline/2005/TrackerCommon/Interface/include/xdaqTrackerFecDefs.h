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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef XDAQTRACKERFECDEFS_H
#define XDAQTRACKERFECDEFS_H

  // Maximum i2o Message size = 256kb
#define MAXI2OMESSAGESIZE 262144

#include "tscTypes.h"

  // -------------------------------------------------------------------------
  // Definition of the messages

#define FEC_HARD_RESET        0x01 // Reset the PLX
#define FEC_SOFT_RESET        0x02 // Reset the FEC
#define PIA_RESET             0x03 // Reset through the PIA
#define APV_SETVALUES         0x04 // Set the values and return the upload values
#define APV_GETVALUES         0x05 // Upload the values and return it
#define LASERDRIVER_SETVALUES 0x06 // Set the values and return the upload values
#define LASERDRIVER_GETVALUES 0x07 // Upload the values and return it
#define DOH_SETVALUES         0x08 // Set the values and return the upload values
#define DOH_GETVALUES         0x09 // Upload the values and return it
#define MUX_SETVALUES         0x10 // Set the values and return the upload values
#define MUX_GETVALUES         0x11 // Upload the values and return it
#define PLL_SETVALUES         0x12 // Set the values and return the upload values
#define PLL_GETVALUES         0x13 // Upload the values and return it
#define PLL_COLDRESET         0x14 // Cold reset for the PLL
#define DCU_GETVALUES         0x15 // return the upload values
#define DCU_DETECTDCUS        0x16 // return the detected DCUs

#define GET_RUN               0x20 // retreive the run number
#define UPDATE_RUN            0x21 // database must be updated

#define APV_CALIB             0x25 // Upload the values and return it (CSEL, CRDV, LATENCY)
#define APV_VPSP              0x26 // Upload the VPSP and return it

#define FEC_GETHARDID         0x27 // Ask for the corresponding FEC hardware ID

#define FEC_TEMP_SEND         0x28 // Send the FEC temperature to the DcuFilter

#define FEC_RECONFIGURING     0x29 // Reconfigure hardware

#define DBCLIENT_DATABASECHANGED 0x2a // FEC DB changed
#define FEC_INCREMENT_DELAY_STEP             0x41
#define FEC_SET_DELAY_PARAMETERS             0x42
#define FEC_START_DELAY_MAP                  0x43

// Multiple frames commands
#define APV_SETALLVALUES         0x100  // Set all the APV with the description set in the message
#define APV_ALLAPVCALIB          0x101  // Set all the APV with (CSEL, CRDV, LATENCY), description must be set in the message
#define APV_ALLVPSP              0x102  // Set all the APV with VPSP, description must be set in the message
#define LASERDRIVER_SETALLVALUES 0x103  // Set all the laserdriver with the description set in the message
#define MUX_SETALLVALUES         0x104  // Set all the MUX with the description set in the message
#define PLL_SETALLVALUES         0x105  // Set all the PLL with the description set in the message
#define PLL_SETALLADDDELAY       0x106  // Add to all PLL, the delay set in the message
#define PLL_ALLCOLDRESET         0x107  // Reset all the PLL with the cold procedure (same than the set cold reset in FecSupervisor)
#define DCU_GETALLVALUES         0x108  // Retreive all the DCU values (message limited to 256kb)
#define APV_ALLAPVMODE           0x109  // Set all the APV with MODE, description must be set in the message
#define APV_ALLAPVLATENCY        0x110  // Set all the APV with LATENCY, description must be set in the message
#define APV_PRECALIB             0x111  // set the APV with mode, isha, vfs, ical
#define APV_ALLPRECALIB          0x112  // set all the APV with mode, isha, vfs, ical
#define APV_PREDELAY             0x113  // set the APV with mode, Latency
/** To suspend the device readout and resume it
 * One unsigned int should be added: 
 *  - 0 to simply or suspend the readout
 *  - > 0 to set a different timing in the readout
 * These i2o messages return the timing for the device or dcu readout
 */
#define I2O_SUSPENDDEVICEREADOUT 0x200 // Suspend the device readout
#define I2O_RESUMEDEVICEREADOUT  0x201 // Resume the device readout
#define I2O_SUSPENDDCUREADOUT    0x202 // Suspend the DCU readout
#define I2O_RESUMEDCUREADOUT     0x203 // Resume the DCU readout

  // ---------------------------------------------------------------------------
  // Definition of the parameters needed to initialise the FecSupervisor

/** FEC type based on the enumeration defined in tscTypes.h
 * The following values are available, please do not use directly the string
 * fecBusTypeList[FECPCI] 
 * fecBusTypeList[FECVME] 
 * fecBusTypeList[FECUSB] 
 */
const std::string fecBusTypeList[3] = {"PCI_FEC", "VME_FEC", "USB_FEC"} ;

/** Bus adapter type based on the enumeration defined in tscTypes.h, only used for the VME FEC
 * The following values are available, please do not use directly the string
 * strBusAdapterList[SBS]
 * strBusAdapterList[CAENPCI]
 * strBusAdapterList[CAENUSB]
 */
const std::string strBusAdapterList[3] = {"SBS", "CAENPCI", "CAENUSB"} ;

/** State for the FEC supervisor finite state machine
 */
const std::string INITIALISE   = "Initialise";
const std::string CONFIGURE    = "Configure"    ;
const std::string CONFIGUREDCU = "ConfigureDcu" ;
const std::string ENABLE       = "Enable"    ;
const std::string HALT         = "Halt"      ;
const std::string HALTDCU      = "HaltDcu"   ;
const std::string DESTROY      = "Destroy"   ;
const std::string RESTART      = "Restart"   ; // not used
const std::string STOP         = "Stop"      ;
const std::string FAIL         = "Fail";

#endif
