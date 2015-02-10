#include "TBDefinition.h"
#include <map>
#ifndef ROOT_TBBuilder
#define ROOT_TBBuilder
using namespace std;
/**
   \class TBBuilder
    \author L.Mirabito
    \version 0.1
    \date November 2002
    \brief Main Analysis class

    <p>
  */

class TBBuilder : public TNamed {

public:
  // Files 
  /** Number of files */
  UShort_t fNfiles; //! Number of files
  /** File names */
  Char_t fListOfFiles[MAX_FILES][256]; //! Names of files
  // database
  /** List of Detector TBGeo */
  TObjArray*  fListOfDet; // List of Geometry to be studied
  // Run dependant quantities
  /** current run (obsolete) */
  TBRun* fCurRun; // Current Run
  /** List of Pedestal objects TBPed */ 
  TObjArray*  fListOfPedestal; // List of TBPed 
  /** List of Noise objects TBPed */ 
  TObjArray*  fListOfNoise; // List of TBPed
  // Current event
  /** Current event (obsolete) */ 
  TBEvent* fCurEvent;
  /** List of TBRaw data */ 
  TObjArray*  fListOfRaw; // List of TBRaw
  /** List of TBCor data */ 
  TObjArray*  fListOfCor; // List of TBCor
  /** List of TBClu data */ 
  TObjArray*  fListOfClu; // List of TBClu
//2 lines below added by BT
  /** List of TH1F data */ 
  TObjArray*  fListOfCmn; // List of TH1F

  /** List of TBRU data */ 
  TObjArray*  fListOfRU; // List of Readout unit

  // Control
  Int_t fRunStudied; // Selected run
  Int_t fFirstEvent; // First event
  Int_t fLastEvent; // LastEvent
  // Share memory
  TMapFile* fmfile; // ShareMemory
  // Output Tree
  /** output file  */
  TFile*    fTOut; // Output file
  /** Output Tree */
  TTree*    fT;  // OutPut Tree
public:
  /** Default Constructor */
  TBBuilder();
  ~TBBuilder();
  // insert New files and ew Geometry
  virtual void AddFile(Char_t *c) {strcpy(&fListOfFiles[fNfiles][0],c);
  printf ("%s is added\n",&fListOfFiles[fNfiles][0]);fNfiles++;} 
  void AddDet(TBGeo* g);

  void AddRU(Int_t instance);
  // virtual function to get event and Run
  virtual void  ReadFile(Int_t i){;}
  virtual Int_t GetNextEvent(){return 0;}
  // Create Pedestals
  TObjArray* CreatePed(Int_t nrun);
  virtual void SavePedestal(Int_t nrun);
  virtual void ReadPedestal(Int_t nrun){;}
  // Fill Pedestal
  void FillPed(Int_t nrun,TObjArray* lped );
  void FlagDeadPedestal(TObjArray* lped );
  void SetPedestal(TObjArray* lped );
  // Fiil Noise
  TObjArray* CreateNoise(Int_t nrun);
  virtual void SaveNoise(Int_t nrun){;}
  virtual void ReadNoise(Int_t nrun){;}
  void FillNoise(Int_t nrun,TObjArray* lnoi );
  void FlagDeadNoise(TObjArray* lnoi );
  void SetNoise(TObjArray* lnoi );
  // Build Clusters
  void BuildCluster(Int_t nrun);
  // Utilities
  TBRun* GetCurRun() const { return fCurRun;}
  TBEvent* GetCurEvent() const { return fCurEvent;}
  UShort_t GetNdet() const { return fListOfDet->GetEntriesFast();}

  void DrawDet(Int_t nrun, Int_t nevt, Int_t ndet);
  void DrawHip(Int_t nrun, Float_t cut, Int_t ndet);
  void DrawEvent(Int_t nrun, Int_t nevt);
  void DrawFedChannel(Int_t nrun, Int_t nevt);
  virtual void Loop(){;}
  inline void SetFirstEvent(Int_t i) { fFirstEvent = i;}
  inline void SetLastEvent(Int_t i) { fLastEvent = i;}
  // Share Memory
  void InitMapFile(const Char_t* cm);
  void FillMapFile();
  void CloseMapFile();
  // Output Tree
  void FillOut(Int_t nrun);

  map<int,TBRaw*> mapRaw;
  ClassDef(TBBuilder,1) //Semi-Abstract class to handle analysis
       };
#endif
