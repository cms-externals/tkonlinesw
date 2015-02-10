
#ifndef ROOT_TBEventHeader
#define ROOT_TBEventHeader
#include "TBDefinition.h"
using namespace std;
/*
Main Event Header Definition

Type 2:
  Add process mode

*/
class TBEventHeader : public TNamed {
  
private:
  Int_t fRun;         // Run Number
  Int_t fSpill;
  Int_t fBx;     // Number in Spill
  Int_t fTriggerType;      // trigger type (1 = physics, 3 = random)
  Int_t fEvent;       // Event Number
  Int_t fInspill;     // Number in Spill
  Int_t fDBVersion; //DB version
  Int_t fTime;        // Time
  Int_t fNChannel;
  Int_t fChannels[10];
  Int_t fLatency;
  Int_t fCalSelection;
  Int_t fCalChannel;
  Int_t fAlignDelay;
  Int_t fOptoGain;
  Int_t fOptoBias;
  
 
  Int_t fPllCoarse;      // pattern unit
  Int_t fPllFine;      // pattern unit
  Int_t fTscFine;      // pattern unit

  Int_t fProcess;     //Process Mode
  Int_t fVpsp;     //Vpsp
  Int_t fVpspI2c; // Vpspi2c
public:

  enum{PHYSIC=1,PEDESTAL=2,CALIBRATION=3,CALIBRATION_DECO=33,GAINSCAN=4,TIMING=5,LATENCY=6,DELAY=7,PHYSIC10=10,CONNECTION=11, DELAY_TTC=8,TIMING_FED=12,BARE_CONNECTION=13,VPSPSCAN=14,SCOPERUN=15,FAST_CONNECTION=16};  

  enum {SCOPE=0,VIRGIN=1,PROCESS=2,CLUSTER=3};
  TBEventHeader();
  TBEventHeader(Int_t* pilot){memcpy(&fRun,pilot,9*sizeof(Int_t));}
  ~TBEventHeader(){;}
   inline void Update(Int_t* pilot){memcpy(&fRun,pilot,9*sizeof(Int_t));}
   inline Int_t GetRun() const { return fRun ; }
   inline Int_t GetEvent() const { return fEvent ; }
   inline Int_t GetBx() const { return fBx ; }
   inline Int_t GetInspill() const { return fInspill ; }
   inline Int_t GetTime() const { return fTime ; }
   inline Int_t GetSpill(Int_t i) const { return fSpill ; }
   inline Int_t GetTriggerType() const { return fTriggerType ; }
   inline Int_t GetLatency() const { return fLatency;}
   inline Int_t GetCalSel() const { return fCalSelection;}
   inline Int_t GetCalChannel() const { return fCalChannel;}
   inline Int_t GetPllFine() const { return fPllFine;}
   inline Int_t GetPllCoarse() const { return fPllCoarse;}
   inline Int_t GetOptoGain() const { return fOptoGain;}
   inline Int_t GetOptoBias() const { return fOptoBias;}
   inline Int_t GetProcess() const { return fProcess;}
   inline Int_t GetVpsp() const { return fVpsp;}
   inline Int_t GetVpspI2c() const { return fVpspI2c;}



   inline void setRun(Int_t valRun) { fRun=valRun;}
   inline void setEvent(Int_t valEvent) { fEvent=valEvent;}
   inline void setBx(Int_t valBx) { fBx=valBx;}
   inline void setInspill(Int_t valInspill) { fInspill=valInspill;}
   inline void setTime(Int_t valTime) { fTime=valTime;}
   inline void setTriggerType(Int_t valTriggerType) { fTriggerType=valTriggerType;}
   inline void setLatency(Int_t valLatency) { fLatency=valLatency;}
   inline void setCalSel(Int_t valCalSel) { fCalSelection=valCalSel;}
   inline void setCalChannel(Int_t valCalChannel) { fCalChannel=valCalChannel;}
   inline void setPllFine(Int_t valPllFine) { fPllFine=valPllFine;}
   inline void setPllCoarse(Int_t valPllCoarse) { fPllCoarse=valPllCoarse;}
   inline void setOptoGain(Int_t val) { fOptoGain=val;}
   inline void setOptoBias(Int_t val) { fOptoBias=val;}
   inline void setProcess(Int_t val) { fProcess=val;}
   inline void setVpsp(Int_t val) { fVpsp=val;}
   inline void setVpspI2c(Int_t val) { fVpspI2c=val;}


  ClassDef(TBEventHeader,2) // Test Beam Event header 
};

#endif
