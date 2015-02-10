#include "TBDefinition.h"
#ifndef ROOT_TBCor
#define ROOT_TBCor
using namespace std;

class TBCor : public TNamed {

private:
  TBGeo* fGeo; //! TB Geometry
  Int_t fIdAcq;
  TBPed* fNoise; //!TB Noise
  TH1F*  fCor; // Histogram of Raw-Data - Pedestal -CMN
  TH1F*  fCmn; // Histogram of CMN


public:
  TBCor(){fGeo=NULL;fNoise=NULL;fCor=NULL;fCmn=NULL;}
  TBCor(TBGeo* g);
  ~TBCor(){delete fCor; delete fCmn;}
  inline void SetGeo(TBGeo* g) {fGeo =g;fIdAcq=g->GetIdAcq(0);}
  inline TBGeo* GetGeo() const { return fGeo;}
  inline void SetNoi(TBPed* p) {fNoise =p;}
  inline TBPed* GetNoi() const { return fNoise;}
  void Fill(TBRaw* r, TBPed *p, TBPed* n,TBEventHeader* h=NULL);
  inline Int_t  GetNstrip() {return fCor->GetNbinsX();}
  Float_t GetChannel(Int_t i) {return fCor->fArray[i+1];}
  inline Float_t GetNoise(Int_t i) {return fNoise->GetRMS(i);}
  inline Float_t GetCmn(Int_t i) {return fCmn->fArray[i+1];}
  inline void Draw(Option_t* option) { fCor->Draw(option);}
  inline void DrawCmn(Option_t* option){ fCmn->Draw(option);}
  void BuildClusters(TObjArray* f);
  inline TH1F* GetHisto() const { return fCor;}
  ClassDef(TBCor,1) // Test Beam Corrected Data
       };

#endif
