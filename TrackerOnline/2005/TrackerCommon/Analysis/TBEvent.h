#include "TBDefinition.h"
#ifndef ROOT_TBEvent
#define ROOT_TBEvent
using namespace std;
/*
Main Event Definition 


*/
class TBEvent : public TNamed {
  
private:
  TBEventHeader*  fEventHeader; // Event Header
  TBRun* fRun; //! reference to the run 
  TObjArray*          fRaws; // List of Raw data
  TObjArray*          fCors; // List of Corrected data
  TObjArray*          fClus; // List of Clusters
public:
  TBEvent();
  TBEvent(TBRun* r);
  ~TBEvent();
  inline void SetEventHeader(TBEventHeader* h) {fEventHeader = h;}
  inline void SetRun(TBRun* h) {fRun = h;}
  inline void SetCor(Int_t i,TBCor* c) { fCors->AddAt(c,i);}
  inline TBEventHeader*  GetEventHeader() const { return fEventHeader; }
  inline TBRun*  GetRun() const { return fRun; }
  inline TObjArray*        GetRaws() const { return fRaws; }
  inline TObjArray*        GetCors() const { return fCors; }
  inline TBRaw*  GetRaw(Int_t i) const { return (TBRaw*) fRaws->At(i);}
  inline TBCor*  GetCor(Int_t i) const { return (TBCor*) fCors->At(i);}
  ClassDef(TBEvent,1) // Test Beam Event structure
};

#endif
