#include "TBDefinition.h"
#ifndef ROOT_TBRana
#define ROOT_TBRana
using namespace std;

class TBRana : public TBBuilder {

public:
  // Files 
  Bool_t fIsOpen[MAX_FILES]; // Names of files
  TFile* fRootFile[MAX_FILES];
  Int_t iCurFile,iCurEvent;
  TFile* fCurFile;
  TTree* fTB; // TTree 
  TBranch* header_b;
  TBranch* det_b[MAX_DET];
  TBRana();
  ~TBRana();
  virtual void AddFile(Char_t *c);
  // virtual function to get event and Run
  virtual void  ReadFile(Int_t i);
  virtual Int_t GetNextEvent();

  TBRU* getRU(Int_t nrun, Int_t nevt, Int_t id);
protected:
  // Initilisation and close of Zebra
  ClassDef(TBRana,1) //Semi-Abstract class to handle analysis

};
#endif
