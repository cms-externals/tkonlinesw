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
*/

#include <iostream>

#include <unistd.h> // for usleep
#include "tscTypes.h"
/*
#include "keyType.h"
#include "cmdDescription.h"
#include "dderrors.h"
*/
#include "FecUtcaRingDevice.h"

using namespace std;
//Uncomment to toggle debug messages on
//#define FECUTCARINGDEVICE_DEBUG


#define REG_CONTROL0     0x00000000
#define REG_CONTROL1     0x00000004
#define REG_STATUS0      0x00000008
#define REG_STATUS1      0x0000000c
#define REG_SOURCE       0x00000010
#define REG_VERSION      0x00000010
#define REG_TRA_FIFO     0x00000020
#define REG_RET_FIFO     0x00000024
#define REG_REC_FIFO     0x00000028

#define WRITE_ERROR     1
#define READ_ERROR      2

//static members have to be declared
//std::string FecUtcaRingDevice::strUhalConnectionFile_, FecUtcaRingDevice::strBoardId_;
uhal::HwInterface *FecUtcaRingDevice::hwBoard=NULL;

/******************************************************
		CONSTRUCTOR - DESTRUCTOR
******************************************************/

/* Open the uHal connection
 * \param fecSlot - slot of the FEC
 * \param ring - ring slot
 * \param init - initialise the FEC (with reset at the starting)
 */
FecUtcaRingDevice::FecUtcaRingDevice (tscType8 fecSlot, tscType8 ringSlot, bool init, bool invertClockPolarity ) throw ( FecExceptionHandler ) :
  FecRingDevice ( fecSlot, ringSlot, FECUTCA ) {
  //try{
  //} catch(){
      //RAISEFECEXCEPTIONHANDLER (code, msg, FATALERRORCODE);
  //}
  useNonIncBlocks=false;
  fecHardReset ();
  // Initialise or not the FEC
  try{
    FecRingDevice::setInitFecRingDevice ( init, invertClockPolarity ) ;
  } catch (FecExceptionHandler &e) {
#ifdef FECUTCARINGDEVICE_DEBUG
    std::cerr << "Unable to initialise the ring on the ring " << std::dec << (int)fecSlot << "." << (int)ringSlot << ": " << e.what() << std::endl ;
#endif
    strError=e.what();
  }

  // set timeout loops to maximum (same as PCI FEC )
  loopInTimeWriteFrame_  = 100000; 
  loopInTimeDirectAck_ = 100000; 
  loopInTimeReadFrame_ = 200 ; 

// adsz_delay
  //hwBoard->getNode("mFEC_ctrl.adsz_pulse_duration").write(0); // 0 : 1 cycle / 1 : 2 cycles
//GEO_ADDR
  //hwBoard->getNode("mFEC_ctrl.geo_addr").write(0);
  //hwBoard->dispatch();
}

/** Close the uHal connection
 */
FecUtcaRingDevice::~FecUtcaRingDevice ( ) throw ( FecExceptionHandler )
{
    //delete hwBoard;
}

void FecUtcaRingDevice::configureUhal (const std::string& connectionFile, const std::string& boardId){
        string strUhalConnectionFile;
        if (connectionFile.substr(0,7).compare("file://"))
                strUhalConnectionFile = string("file://")+connectionFile;
        else
                strUhalConnectionFile = connectionFile;

        //strBoardId_ = boardId;
        uhal::ConnectionManager lConnectionManager ( strUhalConnectionFile );
        hwBoard = new uhal::HwInterface(lConnectionManager.getDevice ( boardId ));
}

void FecUtcaRingDevice::releaseUhal (){
        delete hwBoard;
        hwBoard=NULL;
}
/******************************************************
	CONTROL & STATUS RTEGISTERS ACCESS
******************************************************/

/** Write the value given as parameter in FEC control 0 register 
 * \param ctrl0Value - value of the CR0
 * \param force - if force is set then the value is applied blindly. if force is not set then the invert clock polarity is managed following the parameter in the class.
 */
void FecUtcaRingDevice::setFecRingCR0 ( tscType16 ctrl0Value, bool force ) throw ( FecExceptionHandler ) {

  if (!force) {
    if (invertClockPolarity_) ctrl0Value |= FEC_CR0_POLARITY ;
    else ctrl0Value &= (~FEC_CR0_POLARITY) ;
  }

  setControlRegister('0', ctrl0Value);
  /*if (sendCommand(REG_CONTROL0, ctrl0Value, NULL)) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (1,
					   "Unable to access the FEC control register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: writing value 0x" << hex << ctrl0Value << " into CR0 (ring "<<(int)getRingSlot()<<")"<< std::endl ;
#endif

}



/** Reads a value from FEC ctrl0 
 *\return the value read
 */
tscType16 FecUtcaRingDevice::getFecRingCR0( ) throw ( FecExceptionHandler ) {

  tscType32 ctrl0Value = readRegisterValue('C','0');

  /*if (sendCommand(REG_CONTROL0, 0, &ctrl0Value)) {

    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (2,
					   "Unable to access the FEC control register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/

#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: reading value 0x" << hex << ctrl0Value << " from CR0 (ring "<<(int)getRingSlot()<<")"<< std::endl ;
#endif

  return (tscType16)ctrl0Value;
}

/** Write the value given as parameter in FEC control 1 register 
 * \param value to be written
 */
void FecUtcaRingDevice::setFecRingCR1( tscType16 ctrl1Value ) throw ( FecExceptionHandler ) {

  setControlRegister('1', ctrl1Value);
  /*if (sendCommand(REG_CONTROL1, ctrl1Value, NULL)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (WRITE_ERROR,
					   "Unable to access the FEC control register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: writing value 0x" << hex << ctrl1Value << " in CR1 (ring "<<(int)getRingSlot()<<")"<< std::endl ;
#endif

}



/** Reads a value from FEC ctrl1 
 * \return value of the FEC CR1 for that ring
 */
tscType16 FecUtcaRingDevice::getFecRingCR1( ) throw ( FecExceptionHandler ) {

  tscType32 ctrl1Value=readRegisterValue('C','1');

  /*if (sendCommand(REG_CONTROL1, 0, &ctrl1Value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to access the FEC control register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
  
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: reading value 0x" << hex << ctrl1Value << " from CR1 (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

  return (tscType16)ctrl1Value;
}


/** Reads a value from FEC SR0
 * \return the value of the FEC SR0
 */
tscType32 FecUtcaRingDevice::getFecRingSR0(unsigned long sleeptime) throw ( FecExceptionHandler ) {

  tscType32 sr0Value = readRegisterValue('S','0');
  
  /*if (sendCommand(REG_STATUS0, 0, &sr0Value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to access the FEC status register 0",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: reading value 0x" << hex << sr0Value << " from SR0 (ring "<<(int)getRingSlot()<<")"<< std::endl ;
#endif 

  return (tscType32)sr0Value;
}


/** Reads a value from FEC SR1
 * \return the value of the FEC SR1
 */
tscType16 FecUtcaRingDevice::getFecRingSR1( ) throw ( FecExceptionHandler ) {

  tscType32 sr1Value = readRegisterValue('S','1');

/*  if (sendCommand(REG_STATUS1, 0, &sr1Value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to access the FEC status register 1",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: reading value 0x" << hex << sr1Value << " from SR1 (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif
  return (tscType16)sr1Value;
}

/** Return the version of the firmware
 * \return Firmware version
 */
tscType16 FecUtcaRingDevice::getFecFirmwareVersion( ) throw ( FecExceptionHandler ) {

/*  tscType32 fecVersion ;

  if (sendCommand(REG_VERSION, 0, &fecVersion)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to read the FEC firmware version",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }

#ifdef FECUTCARINGDEVICE_DEBUG
    std::cout << "DEBUG: Firmware version: 0x" << hex << fecVersion << " from FEC (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif
*/
  uhal::ValWord<tscType32> valVersion = hwBoard->getNode("FEC_Rd.mfec_version").read();
  hwBoard->dispatch();
  return (tscType16)(valVersion.value()<<8) ;
}

/******************************************************
	FIFO ACCESS - NATIVE 32 BITS FORMAT
	NATIVE FORMAT ACCESS ALLOWS R/W OPERATIONS
******************************************************/

/** Return a word from the FIFO receive
 * \param return a word from the FIFO receive
 */
tscType32 FecUtcaRingDevice::getFifoReceive( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fiforec_value=readRegisterValue('R','R');

  /*if (sendCommand(REG_REC_FIFO, 0, &fiforec_value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to access the FIFO receive",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: Value 0x" << hex << fiforec_value << " read from fifo receive (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

  return (tscType32)fiforec_value;
}




/** set a word in the FIFO receive
 * \param fiforecValue - word to be set
 */
void FecUtcaRingDevice::setFifoReceive( tscType32 fiforec_value )throw ( FecExceptionHandler ) {

  //if (sendCommand(REG_REC_FIFO, fiforec_value, NULL)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (WRITE_ERROR,
					   "Receive FIFO cannot be written",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  //}
#ifdef FECUTCARINGDEVICE_DEBUG
//  std::cout << "DEBUG : writing value 0x" << hex << fiforec_value << " to fifo receive (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

}

/** Return a word from the FIFO return
 * \param return a word from the FIFO return
 */
tscType8 FecUtcaRingDevice::getFifoReturn( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fiforet_value;

  if (sendCommand(REG_RET_FIFO, 0, &fiforet_value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "Unable to access the FIFO return",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: Value 0x" << hex << fiforet_value << " read from fifo return (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

  return (tscType8)fiforet_value;
}




/** set a word in the FIFO return
 * \param fiforet_value - word to be set
 */
void FecUtcaRingDevice::setFifoReturn( tscType8 fiforet_value )throw ( FecExceptionHandler ) {

  if (sendCommand(REG_RET_FIFO, fiforet_value, NULL)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (WRITE_ERROR,
					   "Unable to access the FIFO return",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fiforet_value << " to fifo return (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

}



/** Return a word from the FIFO transmit
 * \param return a word from the FIFO transmit
 */
tscType32 FecUtcaRingDevice::getFifoTransmit( ) throw ( FecExceptionHandler ) {

  DD_FEC_FIFO_DATA_32 fifotra_value;

//  if (sendCommand(REG_TRA_FIFO, 0, &fifotra_value)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (READ_ERROR,
					   "The transmit FIFO cannot be read",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
//  }
#ifdef FECUTCARINGDEVICE_DEBUG
//  std::cout << "DEBUG: Value 0x" << hex << fifotra_value << " read from fifo transmit (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

  return (tscType32)fifotra_value;
}



/** set a word in the FIFO transmit
 * \param fifotra_value - word to be set
 */
void FecUtcaRingDevice::setFifoTransmit( tscType32 fifotra_value )throw ( FecExceptionHandler ) {

/*
  if (sendCommand(REG_TRA_FIFO, fifotra_value, NULL)) {
    RAISEFECEXCEPTIONHANDLER_HARDPOSITION (WRITE_ERROR,
					   "Unable to access the FIFO transmit",
					   ERRORCODE,
					   buildFecRingKey(getFecSlot(), getRingSlot())) ;
  }*/
  string strPath="FIFO.ring";
  strPath.append(1,'0'+getRingSlot());
  strPath.append(".TRA");
  hwBoard->getNode(strPath.c_str()).write(fifotra_value);
  hwBoard->dispatch();


#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG : writing value 0x" << hex << fifotra_value << " to fifo transmit (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif

}






/******************************************************
		HARD RESET
******************************************************/	

/** Hard reset of the FEC board or the ring board => PLX reset is issued
 */
void FecUtcaRingDevice::fecHardReset ( ) throw ( FecExceptionHandler ) {
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG: Hard reset"<< std::endl ;
#endif
// init reset
  for (uint8_t uRing=minUtcaFecRing; uRing<=maxUtcaFecRing; uRing++){
    string strWrite="FEC_Rd_Wr.ring";
    strWrite.append(1,'0'+uRing);
    hwBoard->getNode(strWrite+".aclr_n").write(0);              // 0 : reset FSM ctrl
  }
//init shift clock
  hwBoard->getNode("FEC_Rd_Wr.fec_tx_clk_shift").write(1);    // 0:0° / 1:180°
  hwBoard->getNode("FEC_Rd_Wr.fec_rx_clk_shift").write(1);    // 0:0° / 1:180°
  hwBoard->dispatch();
  usleep(100000);


// reset released
  for (uint8_t uRing=minUtcaFecRing; uRing<=maxUtcaFecRing; uRing++){
    string strWrite="FEC_Rd_Wr.ring";
    strWrite.append(1,'0'+uRing);
    hwBoard->getNode(strWrite+".aclr_n").write(1);              // 0 : reset FSM ctrl
  }
  hwBoard->dispatch();

#ifdef IRQMANAGER
#endif

  // Re-initialise all tables used in FecRingDevice
  FecRingDevice::initAfterHardReset() ;
}

/******************************************************
		IRQ enable / disable
******************************************************/	

/** Enable or disable the IRQ
 * \param enable - enable or disable the IRQ on the PLX
 * \param level - not used, only for code consistent
 */
void FecUtcaRingDevice::setIRQ ( bool enable, tscType8 level ) throw (FecExceptionHandler) {

#ifdef IRQMANAGER
#endif
}

bool FecUtcaRingDevice::sendCommand(uint32_t uCmd, tscType32 uSetValue, tscType32 *pGetValue) {
    uhal::ValWord<uint32_t> lVal, lRet;
    string strWrite="FEC_Rd_Wr.ring";
    strWrite.append(1,'0'+getRingSlot());
    hwBoard->getNode(strWrite+".mfec_address").write(uCmd);
    hwBoard->getNode(strWrite+".mfec_write_data").write(uSetValue);
    hwBoard->getNode(strWrite+".access_mode").write(pGetValue==NULL ? 1 : 0); 
    hwBoard->dispatch();
    string strAccessAck="FEC_Rd.access_ack.ring";
    strAccessAck.append(1,'0'+getRingSlot());
    uint32_t nTry=0;
    do{//Wait for access acknowledge
    	lVal=hwBoard->getNode(strAccessAck).read();
    	hwBoard->dispatch();
        if (lVal==0){
            usleep(100000);
            hwBoard->getNode(strWrite+".access_request").write(1);
            if (++nTry > 10)
                    return true;
        }
    } while (lVal==0); 

    uhal::ValWord<uint32_t> AccessACK_read = hwBoard->getNode(strAccessAck).read();
    hwBoard->dispatch();
    if (pGetValue!=NULL){
        if (AccessACK_read == 1) {
                string strRead="FEC_Rd.mfec_read_data.ring";
                strRead.append(1,'0'+getRingSlot());
                lRet=hwBoard->getNode(strRead).read();
        } else 
                return true;
    }
    hwBoard->getNode(strWrite+".access_request").write(0);// 0: no access request,1: access to perform
    hwBoard->dispatch();
    nTry=0;
    do{//Wait for access acknowledge
    	lVal=hwBoard->getNode(strAccessAck).read();
    	hwBoard->dispatch();
        if (lVal!=0){
            usleep(10000);
            hwBoard->getNode(strWrite+".access_request").write(1);
            if (++nTry > 100)
                    return true;
        }
    } while (lVal!=0); 
    if (pGetValue!=NULL)
        *pGetValue=lRet.value();

    return false;
}

tscType32 FecUtcaRingDevice::readRegisterValue(char cRegType, char cRegNum){
  uhal::ValWord<tscType32> value;
  string strPath;
  switch (cRegType){
          case 'S':strPath= "FEC_Rd.status.ring";break;
          case 'C':strPath= "FEC_Rd_Wr.control.ring";break;
          default :strPath= "FIFO.ring";break;//R
  }
  strPath.append(1,'0'+getRingSlot());
  strPath.append(".");
  switch (cRegType){
          case 'R':strPath.append("REC");break;
          default:
                strPath.append(1, cRegType);
                strPath.append("R");
                strPath.append(1, cRegNum);
  }
  value = hwBoard->getNode(strPath.c_str()).read();
  hwBoard->dispatch();

  return value;
}

void FecUtcaRingDevice::setControlRegister(char cRegNum, tscType16 uValue){
        string strPath = "FEC_Rd_Wr.control.ring";
        strPath.append(1,'0'+getRingSlot());
        strPath.append(".CR");
        strPath.append(1, cRegNum);
        hwBoard->getNode(strPath.c_str()).write(uValue);
        hwBoard->dispatch();
}


std::string FecUtcaRingDevice::getFecFirmwareStringVersion( ) {
#define VERSION_LENGTH  8
        string strRet;
        uhal::ValWord<uint32_t> arrVal[VERSION_LENGTH+5];
        char szPath[50];
        for (int iCar=0; iCar<VERSION_LENGTH; iCar++){
                snprintf(szPath, 50, "FEC_Rd.ascii_code_%d", iCar+1);
                arrVal[iCar]=hwBoard->getNode(szPath).read();
        }
        arrVal[VERSION_LENGTH+0]=hwBoard->getNode("FEC_Rd.version.year").read();
        arrVal[VERSION_LENGTH+1]=hwBoard->getNode("FEC_Rd.version.month").read();
        arrVal[VERSION_LENGTH+2]=hwBoard->getNode("FEC_Rd.version.day").read();
        arrVal[VERSION_LENGTH+3]=hwBoard->getNode("FEC_Rd.version.archi").read();
        arrVal[VERSION_LENGTH+4]=hwBoard->getNode("FEC_Rd.version.firmware").read();
        hwBoard->dispatch();
        for (int iCar=0; iCar<VERSION_LENGTH; iCar++)
                strRet.append(1, (char)arrVal[iCar]);

        snprintf(szPath, 50, " %d-%d-%d %02X_%02X", arrVal[VERSION_LENGTH].value()+2000, arrVal[VERSION_LENGTH+1].value(), arrVal[VERSION_LENGTH+2].value(), 
                        arrVal[VERSION_LENGTH+3].value(), arrVal[VERSION_LENGTH+4].value());
        strRet.append(szPath);
        return strRet;
}

std::vector<uint32_t> FecUtcaRingDevice::readBlockReceiveFifo(){
        string strPath= "FEC_Rd.rec_fifo_words_nb.ring";
        strPath.append(1,'0'+getRingSlot());
        uhal::ValWord<uint32_t> value = hwBoard->getNode(strPath.c_str()).read();
        hwBoard->dispatch();
        strPath= "FIFO.ring";
        strPath.append(1,'0'+getRingSlot());
        strPath.append(".REC");
        uhal::ValVector<uint32_t> vvcRet = hwBoard->getNode(strPath.c_str()).readBlock(value);
#ifdef FECUTCARINGDEVICE_DEBUG
  std::cout << "DEBUG : Read block of "<<value.value()<< " values from receive fifo (ring "<<(int)getRingSlot()<<")" << std::endl ;
#endif
        hwBoard->dispatch();
        return vvcRet.value();
}

uint8_t FecUtcaRingDevice::getMonitoringStatus(bool bTx) {
        uhal::ValWord<uint32_t> vStatus = hwBoard->getNode("FEC_Rd.monitoring").read();
        hwBoard->dispatch();
        uint32_t uStat= bTx ? vStatus.value()>>16 : (vStatus.value()&0xFFFF);
        return uStat>>(getRingSlot()*4);
}
