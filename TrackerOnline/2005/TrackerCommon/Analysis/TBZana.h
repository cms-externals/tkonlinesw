#include "TBDefinition.h"
#ifndef ROOT_TBZana
#define ROOT_TBZana

using namespace std;

#include "TBGeo.h"
#include "TBPed.h"
#include "TBEventHeader.h"
#include "TBRaw.h"
#include "TBCor.h"
#include "TBClu.h"
#include "TBRun.h"
#include "TBEvent.h"
#include "TBBuilder.h"


class TBZana : public TBBuilder {

private:
  // Files 
  Bool_t fIsOpen[MAX_FILES]; // Names of files
  Int_t fCurFile;
public:
  TBZana();
  ~TBZana();

  // virtual function to get event and Run
  virtual void  ReadFile(Int_t i);
  virtual Int_t GetNextEvent();
protected:
  // Initilisation and close of Zebra
  void InitZebra();
  void EndZebra();
  ClassDef(TBZana,1) //Semi-Abstract class to handle analysis

};
#endif
