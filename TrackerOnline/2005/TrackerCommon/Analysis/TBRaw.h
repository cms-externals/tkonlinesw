
#ifndef ROOT_TBRaw
#define ROOT_TBRaw
#include "TBDefinition.h"
using namespace std;


class TBRaw : public TNamed {
  
private:
  TBGeo* fGeo; //! Geometry 
  TH1S* fChip[3]; // Raw data
  UShort_t fApvAdr[6]; //! ApvAdr[6];
  UShort_t  fApvErr;
  TH1S* fADC;  //! Histogram of Raw Data
public:
  TBRaw(){fGeo = NULL; fADC=NULL;}
  TBRaw(TBGeo*g );
  ~TBRaw(){delete fADC;}
  inline void SetGeo(TBGeo* g) {fGeo =g;}
  inline TBGeo* GetGeo() const { return fGeo;}
  void Fill(Int_t c,UShort_t* data );
  void ReOrder();
  void ReOrder(Int_t process);
  inline Int_t  GetNstrip() const {return fADC->GetNbinsX();}
  inline UShort_t GetADC(Int_t i) const {return (UShort_t) 
                                    fADC->fArray[i+1];}
  inline void SetADC(Int_t i, UShort_t j) {fADC->fArray[i+1] = j;}
  inline TH1S* GetHisto() const { return fADC;}
  inline TH1S* GetHisto(Int_t c) const { return fChip[c];}
  inline UShort_t GetApvAddress(Int_t a){return fApvAdr[a];}
  inline UShort_t GetApvError(Int_t a) { return (fApvErr>>a)&0x1;}
  inline void Draw(Option_t* option) {fADC->Draw(option);}
  inline void DrawChip(Int_t c,Option_t* option) {fChip[c]->Draw(option);}
  ClassDef(TBRaw,1) // Test Beam Raw Data
       };
#endif
