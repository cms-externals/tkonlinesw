#include <TObject.h>
#include <TObjArray.h>
#include <TLine.h>
#include <TMarker.h>

#ifndef ROOT_TBPlane
#define ROOT_TBPlane
using namespace std;
#define MAX_PLANE 100

class TBHit : public TMarker {
public:
  TBHit(){;}

public:
  Float_t fVclu[2];
  Float_t fXclu[2];
  Float_t fVhclu[2];
  Float_t fNoiclu[2];
  Float_t fNoistr[2];
  Int_t fNsclu[2];
  Int_t fIstr[2];
  ClassDef(TBHit,1)
};

class TBPlane : public TLine  {
      
public:
  TBPlane(){;}
  TBPlane(Int_t nc){fHits= new TObjArray(nc);}
  ~TBPlane(){fHits->Delete();delete fHits;}
  void Clean(){fHits->Delete();}
  void SetType(Int_t id) {fType = id;}
  void SetIdx(Int_t id){fIdx=id;}
  void SetIdy(Int_t id){fIdy=id;}
  void SetX(Float_t x) {fX =x;}
  void SetY(Float_t y) {fY =y;}
  void SetZ(Float_t z) {fZ =z;}
  void SetDX(Float_t x) {fDX =x;}
  void SetDY(Float_t y) {fDY =y;}
  void SetDZ(Float_t z) {fDZ =z;}
  void SetPitchx(Float_t x) {fPitchx =x;}
  void SetPitchy(Float_t y) {fPitchy =y;}
  void SetNstripx(Int_t id){fNstripx=id;}
  void SetNstripy(Int_t id){fNstripy=id;}
  void SetReadoutx(Float_t x) {fReadoutx =x;}
  void SetReadouty(Float_t y) {fReadouty =y;}
  void SetCor(Float_t y) {fCor =y;}
  void SetDCor(Float_t y) {fDCor =y;}
  void AddHit(TBHit* h) {fHits->AddLast(h);}
  Int_t GetNhits() const { return fHits->GetEntriesFast();}
  TBHit* GetHit(Int_t i) const { return (TBHit*) fHits->At(i);}
  Float_t GetXPos(Int_t i);
  Float_t GetDXPos(Int_t i) const {return fDX;}
  Float_t GetYPos(Int_t i);
  Float_t GetDYPos(Int_t i) const {return fDY;}
  void Print() {fHits->Print();}

public:
  Int_t fType;
  Int_t fIdx,fIdy; // Id Local in X and Y
  Float_t  fX,fY,fZ; // Coordinate of center
  Float_t  fDX,fDY,fDZ; // Error
  Float_t  fPitchx,fPitchy;
  Int_t  fNstripx,fNstripy;
  Float_t  fReadoutx,fReadouty;
  Float_t  fCor,fDCor;
  TObjArray* fHits; // Array of hits

  ClassDef(TBPlane,1)

};
#endif
#ifndef ROOT_TBTrack
#define ROOT_TBTrack
class TBTrack : public TLine {
public:
  TBTrack(){;}
  TBTrack(Int_t np){fPlanes = new TObjArray(np);}
  ~TBTrack() {if (fPlanes) {fPlanes->Clear();delete fPlanes;} }
  void AddHit(TBPlane* p,UShort_t i); 
  void RemoveHit(UShort_t i); 
      void Clear() { fPlanes->Clear();}
  void Fit();

public:
  Float_t fax,fbx;
  Float_t fay,fby;
  Float_t fchi2x,fchi2y,fchi2;
  Int_t fndfx,fndfy,fndf;
  Float_t fCov[10];
  TObjArray* fPlanes;
  UShort_t fHitidx[MAX_PLANE];
  ClassDef(TBTrack,1)
};


#endif


