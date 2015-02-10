#ifndef TBMONITOR_H
#define TBMONITOR_H
#include <map>
#include <string>
using namespace std;
#include "TBGeo.h"
#include "TBPed.h"
#include "TBEventHeader.h"
#include "TBRaw.h"
#include "TBCor.h"
#include "TBClu.h"
#include "TBRun.h"
#include "TBRU.h"
#include "TBEvent.h"
#include "TBBuilder.h"


#define DCU_DEPTH 4096
class DCUHisto : public TNamed 
{
 private:

  TH1S* fDcuChannel[8];
  TH1D* fWaferTemperature;
  TH1D* f250Bias;
  TH1D* f125Bias;
  TH1D* fWaferCurrent;
  TH1D* fHybridTemperature;
  TH1D* fDcuTemperature;
  Int_t fCount;
 public:
  DCUHisto(TBGeo* g);
  ~DCUHisto();
  void update(UShort_t* dcu);
  inline TH1S* getDcuChannel(int i){return fDcuChannel[i];}
  inline TH1D* getWaferTemperature(){return fWaferTemperature;}
  inline TH1D* get250Bias(){return f250Bias;}
  inline TH1D* get125Bias(){return f125Bias;}
  inline TH1D* getWaferCurrent(){return fWaferCurrent;}
  inline TH1D* getHybridTemperature(){return fHybridTemperature;}
  inline TH1D* getDcuTemperature(){return fDcuTemperature;}
  void reset();
  static Float_t TemperatureWafer(int adcu);
  static Float_t TemperatureHybrid(int adcu);
  static Float_t TemperatureDcu(int adcu);
  static Float_t V250Bias(int adcu);
  static Float_t V125Bias(int adcu);
  ClassDef(DCUHisto,1)
};

class TBClusterHisto : public TNamed 
{
 public:
  TBClusterHisto(TBGeo* g);
  ~TBClusterHisto();
  void update(TObjArray* clusters,float thr=0.);
  inline TH1S* getNumberOfStrip(){return fNumberOfStrip;}
  inline TH1D* getClusterNoise(){return fClusterNoise;}
  inline TH1D* getClusterSignal(){return fClusterSignal;}
  inline TH1D* getClusterSignalHigh(){return fClusterSignalHigh;}
  inline TH1D* getClusterPosition(){return fClusterPosition;}
  inline TH1D* getSeedNoise(){return fSeedNoise;}
  inline TH1D* getSeedSignal(){return fSeedSignal;}
  inline TH1D* getSeedPosition(){return fSeedPosition;}
  void reset();
 private:

  TH1S* fNumberOfStrip;
  TH1D* fClusterNoise;
  TH1D* fClusterSignal;
  TH1D* fClusterSignalHigh;
  TH1D* fClusterPosition;
  TH1D* fSeedNoise;
  TH1D* fSeedSignal;
  TH1D* fSeedPosition;
  ClassDef(TBClusterHisto,1)
};
class TBCalibrationHisto : public TNamed 
{
 public:
  TBCalibrationHisto(TBGeo* g);
  ~TBCalibrationHisto();
  void update(TBEventHeader* h, TBRaw* r);
  void update(TBEventHeader* h, TBRaw* r,TBPed* p);
  inline TProfile* getCalibrationStrip(Int_t i) { return fStrip[i];}
  void reset();
 private:
  TProfile* fStrip[768];
  ClassDef(TBCalibrationHisto,1)
};
class TBTimingHisto : public TNamed 
{
 public:
  TBTimingHisto(TBGeo* g);
  ~TBTimingHisto();
  void update(TBEventHeader* h, TBRaw* r);
  inline TH1D* getTimingTick(Int_t chip){ return fTick[chip];}
  inline TH1D* getTimingTickFed(Int_t chip){ return fTickFed[chip];}
  void reset();
 private:
  TH1D* fTick[3];
  TH1D* fTickFed[3];
  ClassDef(TBTimingHisto,1)
};
class TBOptoHisto : public TNamed 
{
 public:
  TBOptoHisto(TBGeo* g);
  ~TBOptoHisto();
  void update(TBEventHeader* h, TBRaw* r);
  inline TProfile* getOptoTick(Int_t chip,Int_t gain){ return fTick[chip][gain];}
  inline TProfile* getOptoBase(Int_t chip,Int_t gain){ return fBase[chip][gain];}
  inline TH1D* getOptoDiff(Int_t chip,Int_t gain){ return fDiff[chip][gain];}
  void reset();
 private:
  enum {ngain=4,nlaser=3};
  TProfile* fTick[nlaser][ngain];
  TProfile* fBase[nlaser][ngain];
  TH1D* fDiff[nlaser][ngain];
  ClassDef(TBOptoHisto,1)
};
class TBLatencyHisto : public TNamed 
{
 public:
  TBLatencyHisto(TBGeo* g);
  ~TBLatencyHisto();
  void update(TBEventHeader* h,TObjArray* clusters);
  inline TH1D* getLatencyNumberOfCluster(){ return fNumberOfCluster;}
  inline TProfile* getLatencyClusterSignal(){ return fClusterSignal;}
  void reset();
 private:
  TH1D* fNumberOfCluster;
  TProfile* fClusterSignal;
  ClassDef(TBLatencyHisto,1)
};
class TBDelayHisto : public TNamed 
{
 public:
  TBDelayHisto(TBGeo* g);
  ~TBDelayHisto ();
  void update(TBEventHeader* h,TObjArray* clusters);
  inline TProfile* getDelayClusterSignal(){ return fClusterSignal;}
  inline TH1D* getDelayNumberOfCluster(){ return fNumberOfCluster;}
  void reset();
 private:
  TH1D* fNumberOfCluster;
  TProfile* fClusterSignal;
  ClassDef(TBDelayHisto,1)

};

#define MAXVPSP 60
class TBVpspHisto : public TNamed
{
 public:
  TBVpspHisto(TBGeo* g);
  ~TBVpspHisto ();
  void update(TBEventHeader* h,TBRaw* r);
#ifdef VPSPPED
  inline TProfile* getMeanNoise(int j){ return fMeanNoise[j];}
#endif
  inline TProfile* getMeanMean(int j){ return fMeanMean[j];}
  void reset();
 private:
  int fvpsp;
  int napv;
#ifdef VPSPPED
  TBPed* fPed[MAXVPSP];
  TProfile* fMeanNoise[6];
#endif
  TProfile* fMeanMean[6];
  ClassDef(TBVpspHisto,1)

};




// Hash table for the APV
typedef map<unsigned long, TH1* , less<unsigned long> > mapTH1 ;
typedef pair<unsigned long, TH1*> pairTH1 ;

typedef map<string, unsigned long , less<string> > mapString ;
typedef pair<string, unsigned long> pairString ;

class TBMonitor :  public TBBuilder
{
 public:
  TBMonitor(bool calib=false);
  ~TBMonitor();
  void newEvent(Int_t *pilot);
  void newDCUChannel(Int_t idloc,UShort_t* data);
  void newFedChannel(Int_t idacq,UShort_t* data);
  void newReadoutUnit(Int_t inst,Int_t* data,Int_t size);
  void reorder(Int_t process=0);
  void updatePedestal();
  void updateNoise();
  void clearPedestal();
  int loadPedestal(const char* fn);
  void updateClusters();
  void updateCalibration();
  void updateTimingAdjustement();
  void updateOptoAdjustement();
  void updateLatencyCurve();
  void updateDelayCurve();
  Int_t updateConnection();
  void updateVpspScan();
  void toFile(Int_t nr,Int_t type);
  void savePedestal(const char* fn);
void saveCMSSWHistos();
  void deleteHistos(int i);
  void deleteHistos();
  void initHistos(int ir);
  void initHistos();
  void initDCUHistos();
  void saveDCUHistos();
  inline DCUHisto* getDCUHisto(int k) {return fDCUHisto[k];}
  inline TBClusterHisto* getClusterHisto(int k) {return fClusterHisto[k];}
  inline TBCalibrationHisto* getCalibrationHisto(int k) {return fCalibrationHisto[k];}
  inline TBTimingHisto* getTimingHisto(int k) {return fTimingHisto[k];}
  inline TBOptoHisto* getOptoHisto(int k) {return fOptoHisto[k];}
  inline TBLatencyHisto* getLatencyHisto(int k) {return fLatencyHisto[k];}
  inline TBDelayHisto* getDelayHisto(int k) {return fDelayHisto[k];}
  inline TBVpspHisto* getVpspHisto(int k) {return fVpspHisto[k];}
  inline TBEventHeader* GetEventHeader(){return header_;}
  void initTree(Int_t nr);
  void initTree(Char_t *fdir,Int_t nr,Int_t fseq);
  void fillTree();
  void initRUTree(Char_t *fdir,Int_t nr,Int_t fseq);
  void fillRUTree();
  void closeRUTree();
  int getRUTreeSize() { return fTRUOut->GetSize();}
  void Close();
  void initMap(const Char_t* m);
  void updateMap();
  inline int getHistoListSize(){return fHistoList->size();}
  void postAnalysis();
  void FitOptoTick(int gain,int imod, int jtick);

  TH1* getHistoById(unsigned long idx) 
    {
      mapTH1::iterator histmap = fHistoList->find(idx) ;
      if (histmap == fHistoList->end()) 
	return NULL; 
      else 
	return histmap->second;
    }

  TH1* getHistoByName(string s) 
    {
      int idx;
      mapString::iterator stmap = fHistoNames->find(s) ;
      if (stmap == fHistoNames->end()) 
	return NULL; 
      else 
	idx= stmap->second;
      return this->getHistoById(idx); 
    }
  mapString* getHistoNamesMap(){return fHistoNames;}
  void updateMaps();

 private:
  DCUHisto* fDCUHisto[MAX_DET];
  TBClusterHisto* fClusterHisto[MAX_DET];
  TBClusterHisto* fClusterAll;
  TBCalibrationHisto* fCalibrationHisto[MAX_DET];
  TBTimingHisto* fTimingHisto[MAX_DET];
  TBOptoHisto* fOptoHisto[MAX_DET];
  TBLatencyHisto* fLatencyHisto[MAX_DET];
  TBLatencyHisto* fLatencyAll;
  TBDelayHisto* fDelayHisto[MAX_DET];
  TBVpspHisto* fVpspHisto[MAX_DET];
  TBEventHeader* header_;
  TFile* fHistoOut;
  mapTH1* fHistoList;
  mapString* fHistoNames;

  // Analysis storage

  Float_t fTickDelay[3][MAX_DET];
  Float_t fTickBase[3][MAX_DET];
  Float_t fTickTop[3][MAX_DET];
  int fOptoBias[4][3][MAX_DET];
  int fOptoBaseValue[4][3][MAX_DET];
  int fOptoTickValue[4][3][MAX_DET];
  Float_t fOptoBaseDeriv[4][3][MAX_DET];
  Float_t fOptoTickDeriv[4][3][MAX_DET];
  int fOptoStart[4][3][MAX_DET];
  Float_t fOptoGain[4][3][MAX_DET];
  int fVpsp[6][MAX_DET];
 /** output file  */
  TFile*    fTRUOut; // Output RU file
  /** Output Tree */
  TTree*    fTRU;  // OutPut RU Tree
  /** Tag */
  bool fFilled[MAX_DET];
  bool useCalib_;
  bool move2Directory(char* name);
 public:
  Float_t getTickDelay(int idet,int itick){ return fTickDelay[itick][idet];}
  Float_t getTickBase(int idet,int itick){ return fTickBase[itick][idet];}
  Float_t getTickTop(int idet,int itick){ return fTickTop[itick][idet];}
  Int_t getOptoBias(int idet,int itick, int igain){ return fOptoBias[igain][itick][idet];}
  Int_t getOptoTickValue(int idet,int itick, int igain){ return fOptoTickValue[igain][itick][idet];}
  Int_t getOptoBaseValue(int idet,int itick, int igain){ return fOptoBaseValue[igain][itick][idet];}
  Float_t getOptoBaseDeriv(int idet,int itick, int igain){ return fOptoBaseDeriv[igain][itick][idet];}
  Float_t getOptoTickDeriv(int idet,int itick, int igain){ return fOptoTickDeriv[igain][itick][idet];}
  Int_t getOptoStart(int idet,int itick, int igain){ return fOptoStart[igain][itick][idet];}
  Float_t getOptoGain(int idet,int itick, int igain){ return fOptoGain[igain][itick][idet];}
 
  Int_t getVpsp(int idet,int iapv){ return fVpsp[iapv][idet];}
  bool isFilled(int idet) { return fFilled[idet];}
  void setFilled(int idet) {fFilled[idet]=true;}
  void clearFilled(){for (int j=0;j<MAX_DET;j++) fFilled[j]=false;}
  map<TBRaw*,int> fRaw2Idet;
  ClassDef(TBMonitor,1)
};
#endif
