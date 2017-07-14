/*
This file is part of CMS Preshower Control Software project.

Author: Wojciech BIALAS , CERN, Geneva, Switzerland

based on apvAccess class by
Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>

#include "tscTypes.h"

#include "kchipDefinition.h"
#include "kchipAccess.h"

/** Constructor to store the access to the KCHIP3B (use without the database)
 * \param fec - fec hardware access
 * \param fecSlot - FEC slot
 * \param ringSlot - Ring slot
 * \param ccuAddress - CCU address
 * \param i2cChannel - i2c channel
 * \param i2cAddress - Device address
 * \exception FecExceptionHandler
 */
kchipAccess::kchipAccess (FecAccess *fec, 
                      tscType16 fecSlot,
                      tscType16 ringSlot,
                      tscType16 ccuAddress,
                      tscType16 i2cChannel,
                      tscType16 i2cAddress )
  : deviceAccess (KCHIP,
                fec, 
                fecSlot, 
                ringSlot, 
                ccuAddress,
                i2cChannel, 
                i2cAddress )
  
{

}

/** Constructor to store the access to the KCHIP3B (use without the database)
 * \param fec - fec hardware access
 * \param key - Key for the device
 * \exception FecExceptionHandler
 */
kchipAccess::kchipAccess (FecAccess *fec, 
                      keyType key) 
  : deviceAccess (KCHIP,
                fec,
                key) {

}

/** This destructor remove the access from the FecAccess class
 * \exception FecExceptionHandler
 */
kchipAccess::~kchipAccess ( ) {

  // ~deviceAccess ( ) ;
}

/**
 * \param kchipValues - KCHIP description
 */
void kchipAccess::setDescription ( class kchipDescription& kchipValues ) {   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = kchipValues.clone() ;
}

/** Take a description value of an KCHIP3B and download the values in a specific KCHIP3B
 * \param kchipValues - all the values for an KCHIP3B
 * \exception FecExceptionHandler
 */
void kchipAccess::setValues ( class kchipDescription& kchipValues ) {
  
  int i ; 
  for(i=0;i<KCHIP_DESC_NUM;i++) { // 
    setValueFromDescription(i,kchipValues.getValue(i)) ; 
  }   

  // Create a copy of the data set
  if (deviceValues_ != NULL) delete deviceValues_ ;
  deviceValues_ = kchipValues.clone() ;
}

/** Build a description of an KCHIP3B and return it
 * \return the KCHIP3B description values
 * \exception FecExceptionHandler
 * \warning a new object reference is dynamically defined. Do not
 * forget to delete it.
 */
kchipDescription *kchipAccess::getValues ( ) {

   kchipDescription *kchipValues = new kchipDescription(getKey());
   if (kchipValues) { 
      int i; 
      for(i=0;i<KCHIP_DESC_NUM;i++) { 
	 try { 
	 kchipValues->setValue(i,getDescriptionValue(i));
	 } catch (FecExceptionHandler& e) { 
	    delete kchipValues; 
	    throw; // i.e. re-throw
	 } 
      }

      kchipValues->setFecHardwareId ( fecHardwareId_, crateId_ ) ;

   }
  return (kchipValues) ;
}

/** Set the KCHIP3B CR0 register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCR( tscType8 cr ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CFG_REG, cr) ;
}

/** Set the KCHIP3B Extended CR register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setECR ( tscType8 ecr ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_ECFG_REG, ecr) ;
}

/** Set the KCHIP3B ID low byte register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setKidLow ( tscType8 kid_l ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_KID_L_REG, kid_l) ;
}


/** Set the KCHIP3B ID high byte register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setKidHigh ( tscType8 kid_h ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_KID_H_REG, kid_h) ;
}

/** Set the KCHIP3B T1 command mask register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setMaskT1Cmd ( tscType8 mask_t1cmd ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_MASK_T1CMD_REG, mask_t1cmd) ;
}



/** Set the KCHIP3B latency register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setLatency ( tscType8 latency  ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_LATENCY_REG, latency) ;
}

/** Set the KCHIP3B GOH interval busy register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setGintBusy ( tscType8 gintbusy ) {
  accessToFec_->writeOffset (accessKey_, KCHIP_GINT_BUSY_REG, gintbusy) ;
}

/** Set the KCHIP GOH idle cycles register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setGintIdle ( tscType8 gintidle ) {

  accessToFec_->write (accessKey_, KCHIP_GINT_IDLE_REG, gintidle) ;
}

/** Set the KCHIP fifo map register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoMap ( tscType8 fifomap ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFOMAP_REG, fifomap) ;
}

/** Set the KCHIP fifo data low register
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoDataLow ( tscType8 fifodata_l ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFODATA_L_REG, fifodata_l) ;
}

/** Set the KCHIP data fifo high register 
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setFifoDataHigh ( tscType8 fifodata_h ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_FIFODATA_H_REG, fifodata_h) ;
}

/** Set the KCHIP calibration pulse delay
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCalPulseDelay ( tscType8 calpulse_delay ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG, calpulse_delay) ;
}

/** Set the KCHIP calibration pulse width
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setCalPulseWidth ( tscType8 calpulse_width ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG, calpulse_width) ;
}

/** Set the KCHIP latency for ADC interface
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setAdcLatency ( tscType8 adclatency ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_ADC_LATENCY_REG, adclatency) ;
}

/** Set the KCHIP Pace Fifo Overflow limit register 
 * \param Value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setPFifoOverflow ( tscType8 pfifo_overflow ) {

  accessToFec_->writeOffset (accessKey_, KCHIP_PFIFO_OVERFLOW_REG, pfifo_overflow) ;
}

/** Set the KCHIP physical register pointed by description index (reg)
 * \param physical register in device description metrix    
 * \param value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setValueFromDescription(int reg, tscType8 val) { 
   int physicalReg = lookup_[reg] ; // lookup table will never point to reserved registers...
  //cout << "Writing desc reg. " << reg << " physical reg. " << physicalReg ; 
  //cout << " value: " << (int) val << endl ;  
  if (!ro_[physicalReg] & !debugonly_[physicalReg]) { // if not read-only, nor debug only 
    accessToFec_->writeOffset (accessKey_,physicalReg, val) ;
    //cout << "Done." << endl ; 
  } else { 
     //cout << "sorry it is RO." << endl ;  
  }
} 

/** Set the KCHIP physical register pointed by physical register offset
 * \param physical register offset (i2c address offset)   
 * \param value to be set
 * \exception FecExceptionHandler
 */
void kchipAccess::setValue(int reg, tscType8 val) { 
  if (!ro_[reg] & !debugonly_[reg] & !reserved_[reg]) { // if not read-only,  not debug, not reserved
    accessToFec_->writeOffset (accessKey_,reg, val) ;
  } 
} 

/** Get KCHIP register value
 * \param physical register offset (i2c address offset)
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getValue (int reg )  {
   if (!debugonly_[reg] & !reserved_[reg]) 
      return (accessToFec_->readOffset (accessKey_, reg)) ;
   else 
      return 0 ;  
}

/** Get KCHIP register value masked with comparision bit mask
 * \param physical register offset (i2c address offset)
 * \return return the masked value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getMaskedValue (int reg )  {
   if (!debugonly_[reg] & !reserved_[reg])
      return (rmask_[reg] & (accessToFec_->readOffset (accessKey_,reg))) ;
   else 
      return 0 ; 
}

/** Get KCHIP register value pointed
 * by description index
 * \param register description index
 * \return return the masked value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getDescriptionValue (int reg )  {
  int physicalReg = lookup_[reg] ;
  if (!debugonly_[physicalReg]) 
         return (accessToFec_->readOffset (accessKey_,physicalReg)) ;
  else 
     return 0 ; 
}

/** Get KCHIP register value masked with comparision mask, pointed
 * by description index
 * \param register description index 
 * \return return the masked value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getDescriptionMaskedValue (int reg )  {
  int physicalReg = lookup_[reg];
  return (rmask_[physicalReg] & (accessToFec_->readOffset(accessKey_,physicalReg))) ;
}

/** Get KCHIP Control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCR ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CFG_REG)) ;
}

/** Get KCHIP Extended Control register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getECR ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_ECFG_REG)) ;
}

/** Get KCHIP chip ID low byte (laser blown)
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getChipID0 ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CHIPID0_REG)) ;
}

/** Get KCHIP chip ID high byte (laser blown)
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getChipID1 ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CHIPID1_REG)) ;
}

/** Get KCHIP3B ID register low byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getKidLow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_KID_L_REG)) ;
}

/** Get KCHIP3B ID register high byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getKidHigh ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_KID_H_REG )) ;
}

/** Get KCHIP3B T1 command mask regiter
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getMaskT1Cmd ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_MASK_T1CMD_REG)) ;
}

/** Get KCHIP3B last T1 command register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getLastT1Cmd ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_LAST_T1CMD_REG)) ;
}

/** Get KCHIP latency register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getLatency ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_LATENCY_REG)) ;
}

/** Get KCHIP event counter
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getEvCnt ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_EVCNT_REG)) ;
}

/** Get KCHIP bunch counter low byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getBnchCntLow ( )  {

  return (accessToFec_->readOffset (accessKey_, KCHIP_BNCHCNT_L_REG)) ;
}

/** Get KCHIP bunch counter high byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getBnchCntHigh ( ) {

  return (accessToFec_->readOffset (accessKey_,KCHIP_BNCHCNT_H_REG)) ;
}

/** Get KCHIP GOH busy interval register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getGintBusy ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_GINT_BUSY_REG)) ;
}

/** Get KCHIP goh idle cycles register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getGintIdle ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_GINT_IDLE_REG)) ;
}

/** Get KCHIP fifo map register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoMap ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFOMAP_REG)) ;
}

/** Get KCHIP data fifo low byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoDataLow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFODATA_L_REG)) ;
}

/** Get KCHIP data fifo high byte
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getFifoDataHigh ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_FIFODATA_H_REG)) ;
}

/** Get KCHIP status 0 register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSR0 ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SR0_REG)) ;
}

/** Get KCHIP status 1 register
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSR1 ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SR1_REG)) ;
}

/** Get KCHIP Single Event Upset counter value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getSeuCounter ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_SEU_COUNTER_REG)) ;
}

/** Get KCHIP calibration puse delay value
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCalPulseDelay ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CALPULSE_DELAY_REG)) ;
}

/** Get KCHIP calibration pulse width
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getCalPulseWidth ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_CALPULSE_WIDTH_REG)) ;
}

/** Get KCHIP analog-digital interface latency settings
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getAdcLatency ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_ADC_LATENCY_REG)) ;
}

/** Get KCHIP Pace fifo overflow level
 * \return return the value read
 * \exception FecExceptionHandler
 */
tscType8 kchipAccess::getPFifoOverflow ( ) {

  return (accessToFec_->readOffset (accessKey_, KCHIP_PFIFO_OVERFLOW_REG)) ;
}

/** Take a description value of a kchip a build a block of frames to be set in the hardware
 * \param kchipValues - all the values for a kchip chip
 * \param vAccess - block of frames
 */
void kchipAccess::getBlockWriteValues ( class kchipDescription& kchipValues, std::list<accessDeviceType> &vAccess ) {

  for(int i = 0 ; i < KCHIP_DESC_NUM ; i++) {
    tscType16 physicalReg = lookup_[i];
    if (!ro_[physicalReg] & !debugonly_[physicalReg]) { // if not read-only nor debug register
      // accessToFec_->write (accessKey_, (reg << 1), val) ;
      accessDeviceType setIt = { getKey(), NORMALMODE, MODE_WRITE, physicalReg, kchipValues.getValue(i),
                               false, 0, 0, 0, NULL} ;
      vAccess.push_back (setIt) ;
    }
  }
}


const char* kchipAccess::names[] = { 
  "Config" , "Extended Config" , "KID low","KID high",
  "Mask T1","Last T1","Latency","Event Counter","Bunch Counter low",
  "Bunch Counter high", "Reserved (0xA)","GOL interface Busy",
  "GOL interface IDLE","Fifo Map","Fifo Data low","Fifo Data high",
  "Status 0","Status 1","SEU Counter","Cal. Pulse Delay","Cal. Pulse Width",
  "ADC latency","Pace Fifo Overflow","Reserved (0x17)","Reserved (0x18)",
  "Reserved (0x19)","Reserved (0x1a)","Reserved (0x1b)","Reserved (0x1c)",
  "Reserved (0x1d)","ChipID 0","ChipID 1" } ; 
			    
const tscType8 kchipAccess::rmask_[] = { 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff 

} ; // i.e. all bits are checked 

const bool kchipAccess::ro_[] = { 
  false,false,false,false,false, true,false, true,
   true, true,false,false,false,false,false,false, 
   true, true, true,false,false,false,false,false,
  false,false,false,false,false,false, true, true 

};

const bool kchipAccess::reserved_[] = { 
   false,false,false,false,false,false,false,false,
   false,false, true,false,false,false,false,false,
   false,false,false,false,false,false,false, true,
    true, true, true, true, true, true,false,false
};

const bool kchipAccess::debugonly_[] = { 
  false,false,false,false,false,false,false,false,
  false,false,false,false,false, true, true, true,
  false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false
};



const int kchipAccess::lookup_[] = { 
  KCHIP_CFG_REG,
  KCHIP_ECFG_REG, 
  KCHIP_KID_L_REG, 
  KCHIP_KID_H_REG,
  KCHIP_MASK_T1CMD_REG,
  KCHIP_LAST_T1CMD_REG,
  KCHIP_LATENCY_REG, 
  KCHIP_EVCNT_REG,
  KCHIP_BNCHCNT_L_REG,
  KCHIP_BNCHCNT_H_REG, 
  KCHIP_GINT_BUSY_REG,
  KCHIP_GINT_IDLE_REG, 
//   KCHIP_FIFOMAP_REG, 
//   KCHIP_FIFODATA_H_REG,
//   KCHIP_FIFODATA_L_REG,
  KCHIP_SR0_REG,
  KCHIP_SR1_REG,
  KCHIP_SEU_COUNTER_REG,
  KCHIP_CALPULSE_DELAY_REG,
  KCHIP_CALPULSE_WIDTH_REG,
  KCHIP_ADC_LATENCY_REG,
  KCHIP_PFIFO_OVERFLOW_REG,
  KCHIP_CHIPID0_REG,
  KCHIP_CHIPID1_REG
} ; 

