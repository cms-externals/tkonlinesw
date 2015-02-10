#include "TBDefinition.h"
#ifndef ROOT_TBPed
#define ROOT_TBPed
using namespace std;
class TBRaw;
class TBCor;
class TBPed : public TNamed {
  
private:
  TBGeo* fGeo; // geometry
  Int_t fIdAcq;
  Int_t fCount;
  TProfile* fPed; // TProfile of pedestal with error = sigma     
  TH1S* fPedF;    // Histogram of flags  
  TH1F* fPedE;    // Histogram of Error
  TH1D* fPedC;    // Histogram of CMN

public:
  TBPed(){fGeo=NULL;fPed=NULL;fCount=0;fPedF=NULL;fPedE=NULL;fPedC=NULL;}
  TBPed(TBGeo *g);
  ~TBPed(){if (fPed!=NULL){delete fPed;delete fPedF; delete fPedE; delete fPedC;}}
  inline void Clear(){fCount=0;fPed->Reset();fPedF->Reset();}
  inline Int_t GetStatistic() {return fCount;}
  inline void SetGeo(TBGeo* g) {fGeo =g;fIdAcq=g->GetIdAcq(0);}
  inline TBGeo* GetGeo() const { return fGeo;}
  void Increment(TBRaw* r);
  void Increment(TBCor* c);
  virtual void SetName(const char *n);
  inline Int_t  GetNstrip() const {return fPed->GetNbinsX();}
  inline Float_t GetMean(Int_t i) const {return fPed->GetBinContent(i+1);}
  inline Float_t GetRMS(Int_t i) const {return fPed->GetBinError(i+1);}
  inline void PrintStrip(Int_t i) {printf("Strip %d : %f % f \n",i,(float) fPed->GetBinContent(i),(float) fPed->GetBinError(i));}
  void Flag(TBPed* p);
  virtual void Info();
  TH1F* GetHistoRMS();
  inline void FlagDead( Int_t i) { fPedF->SetBinContent(i,1);}
  inline void FlagNoisy( Int_t i) { fPedF->SetBinContent(i,2);}
  inline Bool_t IsDead(Int_t i) const {return (fPedF->GetBinContent(i) == 1);}
  inline Bool_t IsNoisy(Int_t i) const {return (fPedF->GetBinContent(i) == 2);}
  inline void Draw(Option_t* option) {fPed->Draw(option);}
  inline TProfile* GetProfile() const {return fPed;}
  inline TH1D* GetCommonModeNoise() const {return fPedC;}

  
  ClassDef(TBPed,1) // Test Beam Pedestal
       };
#endif
