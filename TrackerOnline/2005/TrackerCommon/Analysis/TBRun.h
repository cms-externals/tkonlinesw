
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Run                                                                //
//                                                                      //
// Description of the Run and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "TBDefinition.h"
#ifndef ROOT_TBRun
#define ROOT_TBRun

using namespace std;


/*

Run definition 


    run number
    date
    number of event (if known)
    number of detectors
    Array of detectors Geometry

*/
class TBRun : public TNamed {
public:
  Int_t          fRun; // Run Number
  Int_t          fNbdet; // Number of detectors
  TObjArray* fGeo; // List of TGGeo of detectors
public:
  TBRun() {fGeo=NULL;}
  ~TBRun();
  // Adding a geometry
  inline void AddGeo(TBGeo* g)
  {if (!fGeo) fGeo=new TObjArray(MAX_DET);fGeo->Add(g);}
  // Setter
  inline void   SetRun(Int_t i) { fRun = i;}
  // Getters
  inline Int_t  GetRun() const { return fRun; }
  inline Int_t  GetNbdet() const { return fGeo->GetEntriesFast(); }
  inline TBGeo* GetGeo(Int_t i) const { return (TBGeo*) fGeo->At(i);}
  
  ClassDef(TBRun,1) //Test Beam Run Header
       };
#endif

