#include "TBDefinition.h"
#ifndef ROOT_TBClu
#define ROOT_TBClu
using namespace std;

class TBClu : public TNamed {

private:
  TBCor* fCor; //! Pointer to the Corrected data structure 
  UShort_t fFirstStrip; // First Strip 
  UShort_t fLastStrip; // Last strip
  UShort_t fHighest; // Highest strip
  Float_t  fV; // Signal
  Float_t  fNoise; //  Noise
  Float_t  fXLocal; // Barycenter of position (unit=strip)
public:
  TBClu(){fCor=NULL;}
  TBClu(TBCor* c, UShort_t f, UShort_t l,UShort_t h, 
        Float_t V, Float_t N,Float_t x);
  ~TBClu(){;}
  UShort_t  GetNstrip() const {return fLastStrip-fFirstStrip+1;}
  UShort_t  GetFirstStrip() const {return fFirstStrip;}
  UShort_t  GetLastStrip() const {return fLastStrip;}
  UShort_t  GetHighestStrip() const {return fHighest;}
  Float_t GetHighest() const {return fCor->GetChannel(fHighest);}
  Float_t GetV() const {return fV;}
  Float_t GetNoise() const {return fNoise;}
  Float_t GetSeedNoise() const {return fCor->GetNoise(fHighest);}
  Float_t GetXLocal() const {return fXLocal;}
  inline TBCor*  GetCor() const { return fCor;}

  void Draw(){;}
  ClassDef(TBClu,1) // Test Beam Cluster 
       };

#endif
