#include "TBDefinition.h"
#ifndef ROOT_TBRuAna
#define ROOT_TBRuAna
using namespace std;

class TBRuAna : public TBBuilder {

public:
  // Files 
  Bool_t fIsOpen[MAX_FILES]; // Names of files
  TFile* fRootFile[MAX_FILES];
  Int_t iCurFile,iCurEvent;
  TFile* fCurFile;
  TTree* fTB; // TTree 
  TBranch* header_b;
  TBranch* det_b[MAX_DET];
  TBRuAna();
  ~TBRuAna();
  virtual void AddFile(Char_t *c);
  // virtual function to get event and Run
  virtual void  ReadFile(Int_t i);
  virtual Int_t GetNextEvent();
protected:
  // Initilisation and close of Zebra
  ClassDef(TBRuAna,1) //Semi-Abstract class to handle analysis

};
#endif
