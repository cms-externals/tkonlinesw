#ifndef _TscXmlDescription_h_
#define _TscXmlDescription_h_
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
/**
   \class TscXmlDescription
   \author L.Mirabito
   \date October 2002
   \version 1.0
   \brief Xml mapping class for TSC definitions


 */
class TscXmlRegister 
{
private:
  unsigned short offset_;
  unsigned long  value_;
public:
  unsigned short getOffset(){return offset_;}
  unsigned long  getValue() {return value_;}
  void setOffset( unsigned short v) {offset_=v;}
  void setValue( unsigned long v) {value_=v;}
};
class TscXmlDescription {

 public:
  ~TscXmlDescription()
    {
      for (unsigned int i=0;i<register_.size();i++) delete register_[i];
    }
private:
  int fNumber;
  string fDeviceName;
  unsigned short fResetLatency;
  unsigned short fClockMin;
  unsigned short fTriggerCount;
  unsigned short fTriggerRegister;
  unsigned short fInterrupt;
  unsigned short fInhibitTrigger;
  unsigned short fCalibration;
  unsigned short fReset;
  unsigned short fExternalTrigger;
  unsigned short fInternalTrigger;
  unsigned short fSoftTrigger;
  unsigned short fTriggerFrequency;
  unsigned short fFedLatency;
  unsigned short fApvLatency;
  unsigned short fFedDelayTrigger;
  unsigned short fFedDelayClock;
  unsigned short fPulserDelay;
  unsigned short fCalibrationLatency;
  unsigned short fResetApv;
  unsigned short fReenableTrigger;
  unsigned short fResetPll;
  unsigned short fSendSoftTrigger;
  unsigned short fResetFed;
  unsigned short fGatePositionCalibrationDelay;
  unsigned short fGatePositionDelay;
  unsigned short fGateWidthCalibrationDelay;
  unsigned short fGateWidthDelay;
  unsigned short fGateDelayRegister;
  vector<TscXmlRegister*> register_;
public:
  inline void setNumber(int i) {fNumber=i;} ///<set TSC Number
  inline int getNumber() {return fNumber;} ///<set TSC Number
  inline void setDeviceName(string s) {fDeviceName=s;} ///<:Set device name
  inline string getDeviceName() {return fDeviceName;} ///< Get device name
  /* Setters / getters */
  /// Set latency after trigger before reset 
  inline void setResetLatency(int  a){fResetLatency= a;} 
  /// Get latency after trigger before reset 
  inline int getResetLatency(){return fResetLatency;}


  /// Set minimum number of clock between trigger
  inline void setClockMin(int  a){fClockMin= a;}  
  /// Get minimum number of clock between trigger
  inline int getClockMin(){return fClockMin;}

  /// Set Trigger Maximum Count
  inline void setTriggerCount(int  a){fTriggerCount= a;}  
  /// Get Trigger Maximum Count
  inline int getTriggerCount(){return fTriggerCount;}


  /// Set Trigger Register
  inline void setTriggerRegister(int  a){fTriggerRegister= a;}  
  /// Get Trigger Register
  inline int getTriggerRegister(){return fTriggerRegister;}


  /// Enable Interrupt
  inline void enableInterrupt() {fInterrupt=1;}
  
  /// disable Interrupt
  inline void disableInterrupt() {fInterrupt=0;}
  
  /// Interrupt status
  inline bool getInterrupt(){return fInterrupt==1;}

  /// Inhibit trigger
  inline void enableInhibitTrigger() {fInhibitTrigger=1;}
  
  /// disableInhibitTrigger
  inline void disableInhibitTrigger() {fInhibitTrigger=0;}
  
  /// InhibitTrigger status
  inline bool getInhibitTrigger(){return fInhibitTrigger==1;}

  /// enable calibration
  inline void enableCalibration() {fCalibration=1;}
  
  /// disableCalibration
  inline void disableCalibration() {fCalibration=0;}
  
  /// Calibration status
  inline bool getCalibration(){return fCalibration==1;}


  /// enable Reset
  inline void enableReset() {fReset=1;}
  
  /// disable Reset
  inline void disableReset() {fReset=0;}
  
  /// Reset status
  inline bool getReset(){return fReset==1;}



  /// set External trigger
  inline void setExternalTrigger() {fExternalTrigger=1;fInternalTrigger=0;fSoftTrigger=0;}
  
  /// External Trigger status
  inline bool getExternalTrigger(){return fExternalTrigger==1;}

  /// set Internal trigger
  inline void setInternalTrigger(){fExternalTrigger=0;fInternalTrigger=1;fSoftTrigger=0;}
  
  /// Internal Trigger status
  inline bool getInternalTrigger(){return fInternalTrigger==1;}

  /// set Soft trigger
  inline void setSoftTrigger() {fExternalTrigger=0;fInternalTrigger=0;fSoftTrigger=1;}
  
  /// Soft Trigger status
  inline bool getSoftTrigger(){return fSoftTrigger==1;}


  /// Set Internal Trigger Frequency in 25 micro sec unit
  inline void setTriggerFrequency(int  a){fTriggerFrequency= a;}  
  /// Get Internal Trigger Frequency in 25 micro sec unit
  inline int getTriggerFrequency(){return fTriggerFrequency;}


  /// Set latency after trigger before Fed
  inline void setFedLatency(int  a){fFedLatency= a;}  
  /// Get latency after trigger before Fed
  inline int getFedLatency(){return fFedLatency;}

  /// Set Apv latency 
  inline void setApvLatency(int  a){fApvLatency= a;}  
  /// Get Apv latency 
  inline int getApvLatency(){return fApvLatency;}


  /// Set Fed Delay trigger
  inline void setFedDelayTrigger(int  a){fFedDelayTrigger= a;}  
  /// Get Fed Delay Trigger
  inline int getFedDelayTrigger(){return fFedDelayTrigger;}


  /// Set Fed Delay Clock
  inline void setFedDelayClock(int  a){fFedDelayClock= a;}  
  /// Get Fed Delay Clock
  inline int getFedDelayClock(){return fFedDelayClock;}

  /// Set Pulser Delay
  inline void setPulserDelay(int  a){fPulserDelay= a;}  
  /// Get Pulser Delay
  inline int getPulserDelay(){return fPulserDelay;}


  /// Set Calibration latency 
  inline void setCalibrationLatency(int  a){fCalibrationLatency= a;}  
  /// Get Calibration latency 
  inline int getCalibrationLatency(){return fCalibrationLatency;}

  /// Reset Apv
  inline void setResetApv(bool a) {fResetApv=(a)?1:0;}
  inline bool getResetApv() {return fResetApv==1;}

  /// Reenable trigger
  inline void setReenableTrigger( bool a ) {fReenableTrigger=(a)?1:0;}
  inline bool getReenableTrigger() {return fReenableTrigger==1;}

  /// Reset Pll
  inline void setResetPll(bool a) {fResetPll=(a)?1:0;}
  inline bool getResetPll() {return fResetPll==1;}

  /// Soft Trigger
  inline void setSendSoftTrigger(bool a) {fSendSoftTrigger=(a)?1:0;}
  inline bool getSendSoftTrigger() {return fSendSoftTrigger==1;}

  /// Reset Fed
  inline void setResetFed(bool a) {fResetFed=(a)?1:0;}
  inline bool getResetFed() {return fResetFed==1;}


  /// Set gate position calibration delay
  inline void setGatePositionCalibrationDelay(unsigned char del)
  {fGatePositionCalibrationDelay=del;}
  /// Get gate position calibration delay
  inline unsigned char getGatePositionCalibrationDelay() { return fGatePositionCalibrationDelay;}
  
  /// Set gate position user delay
  inline void setGatePositionDelay(unsigned char del)
  {fGatePositionDelay=del;}
  /// Get gate position user delay
  inline unsigned char getGatePositionDelay() { return fGatePositionDelay;}

  /// Set gate width calibration delay
  inline void setGateWidthCalibrationDelay(unsigned char del)
  {fGateWidthCalibrationDelay=del;}
  /// Get gate width calibration delay
  inline unsigned char getGateWidthCalibrationDelay() { return fGateWidthCalibrationDelay;}
  
 
 /// Set gate width user delay
  inline void setGateWidthDelay(unsigned char del)
  {fGateWidthDelay=del;}
 /// Set gate width user delay
  inline unsigned char getGateWidthDelay()
  {return fGateWidthDelay;}
 
 /// Get Gate Delay register
  inline long getGateDelayRegister(){return fGateDelayRegister;}

 /// set Gate Delay register
  inline void setGateDelayRegister(long a){fGateDelayRegister=a;}

  /// Add register
  inline void addTscXmlRegister(TscXmlRegister* r) {register_.push_back(r);}

  inline int getNumberOfRegister(){return register_.size();}
  /// Get TscXmlRegister
  inline TscXmlRegister* getTscXmlRegister(int i) {return register_[i];}


  /// XML output
  void toXML(ostream &os)
    {
      os << "<Tsc "<<
	" TscDevice=\"" << fNumber << "\"" <<
	" resetlatency =\"" << fResetLatency << "\"" <<
	" clockmin =\"" << fClockMin << "\"" <<
	" triggercount =\"" << fTriggerCount << "\"" <<
	" triggerregister =\"" << fTriggerRegister << "\"" <<
	" interrupt =\"" << fInterrupt << "\"" <<
	" triggerfrequency =\"" << fTriggerFrequency << "\"" <<
	" fedlatency =\"" << fFedLatency << "\"" <<
	" apvlatency =\"" << fApvLatency << "\"" <<
	" feddelaytrigger =\"" << fFedDelayTrigger << "\"" <<
	" feddelayclock =\"" << fFedDelayClock << "\"" <<
	" pulserdelay =\"" << fPulserDelay << "\"" <<
	" caliblatency =\"" << fCalibrationLatency << "\"" <<
	" gateposition =\"" << fGatePositionDelay << "\"" <<
	" gatewidth =\"" << fGateWidthDelay << "\"" <<
	"/>" << endl;
    }
};
#endif




