/*
This file is part of CMS Preshower Control Software project.

Author: Wojciech BIALAS , CERN, Geneva, Switzerland

based on apvAccess class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>

#include "tscTypes.h"

#include "gohDefinition.h"
#include "gohAccess.h"

/** Constructor to store the access to the GOH (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
gohAccess::gohAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress )
  : deviceAccess (GOH,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress )
  
{

}

/** Constructor to store the access to the GOH (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
gohAccess::gohAccess (FecAccess *fec, 
                      keyType key) 
  : deviceAccess (GOH,
                fec,
                key) {

}



/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
gohAccess::~gohAccess ( ) {

  // ~deviceAccess ( ) ;
}

/** 
 * \param gohValues - GOH description
 */
void gohAccess::setDescription ( class gohDescription& gohValues ) {

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = gohValues.clone() ;
}

/** Take a description value of an GOH and download the values in a specific GOH
 * \param gohValues - all the values for an GOH
 * \exception FecExceptionHandler
 */
void gohAccess::setValues ( class gohDescription& gohValues ) {
  
  int i ; 
  for(i=0;i<GOH_REG_NUM;i++) { 
    if (!ro_[i]) setValue(i,gohValues.getValue(i)) ; 
  }   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = gohValues.clone() ;
}

/** Build a description of an GOH and return it
 * \return the GOH description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
gohDescription *gohAccess::getValues ( ) {

   gohDescription *gohValues = new gohDescription(getKey()); 

   if (gohValues) { 
      int i; 
      for(i=0;i<GOH_DESC_NUM;i++) { 
	 try { 
	 gohValues->setValue(i,getValue(i));
	 } catch (FecExceptionHandler& e) { 
	    delete gohValues; 
	    throw; // i.e. re-throw
	 } 
      }
      gohValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;
   }

  return (gohValues) ;
}

/** Set the GOH CR register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void gohAccess::setControl0( tscType8 cr ) {

   accessToFec_->writeOffset (accessKey_, (GOH_ADDRESS_OFFSET), GOH_CONTROL_0) ;
   accessToFec_->writeOffset (accessKey_, (GOH_DATA_OFFSET), cr) ;
}




void gohAccess::setValue(int reg, tscType8 val) { 
  if (!ro_[reg]) { // if not read-only
    accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, reg) ;
    accessToFec_->writeOffset (accessKey_, GOH_DATA_OFFSET, val) ;
  } 
} 

/** Get GOH register value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getValue (int reg )  {
  accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, reg) ;
  return (accessToFec_->readOffset (accessKey_,GOH_DATA_OFFSET)) ;
}
/** Get GOH Error
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getMaskedValue (int reg )  {
  accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, reg) ;
  return (rmask_[reg] & (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET))) ;
}





/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getControl0 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_CONTROL_0) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getControl1 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_CONTROL_1) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getControl2 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_CONTROL_2) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getControl3 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_CONTROL_3) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getStatus0 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_STATUS_0) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Get GOH control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 gohAccess::getStatus1 ( )  {
   accessToFec_->writeOffset (accessKey_, GOH_ADDRESS_OFFSET, GOH_STATUS_1) ;
   return (accessToFec_->readOffset (accessKey_, GOH_DATA_OFFSET)) ;
}

/** Take a description value of a goh and build a block of frames to be set in the hardware
 * \param gohValues - all the values for a goh chip
 * \param vAccess - block of frames
 */
void gohAccess::getBlockWriteValues ( class gohDescription& gohValues, accessDeviceTypeList &vAccess ) {

  for(int i = 0 ; i < GOH_REG_NUM ; i++) { 
    if (!ro_[i]) { // if not read-only
      // accessToFec_->write (accessKey_, (reg << 1), val) ;
      accessDeviceType setIt = { getKey(), NORMALMODE, MODE_WRITE, GOH_ADDRESS_OFFSET, 
				 offset_[i], false, 0, 0, 0, NULL} ;
      vAccess.push_back (setIt) ;
      accessDeviceType setItData = { getKey(), NORMALMODE, MODE_WRITE, GOH_DATA_OFFSET, 
				 gohValues.getValue(i), false, 0, 0, 0, NULL} ;
      vAccess.push_back (setItData) ;
    } 
  }
}

const char* gohAccess::names[] = { "Control0" , "Control1" , "Control2","Control3",
				   "Status0","Status1"} ; 
const tscType8 gohAccess::rmask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff} ; // i.e. all bits are checked 

const bool gohAccess::ro_[] = { 
  false,false, false, false, true, true };

const tscType8 gohAccess::offset_[] = { 
  GOH_CONTROL_0,GOH_CONTROL_1,
  GOH_CONTROL_2,GOH_CONTROL_3,
  GOH_STATUS_0,GOH_STATUS_1 }; 

 

