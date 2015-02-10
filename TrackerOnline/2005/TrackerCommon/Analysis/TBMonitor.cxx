#include "TBMonitor.h"
#include "TKey.h"
#include <math.h>
#include <time.h>
ClassImp(DCUHisto)
ClassImp(TBClusterHisto)
ClassImp(TBCalibrationHisto)
ClassImp(TBTimingHisto)
ClassImp(TBOptoHisto)
ClassImp(TBLatencyHisto)
ClassImp(TBDelayHisto)
ClassImp(TBMonitor)
#include <stdio.h>

DCUHisto::DCUHisto(TBGeo* g)
{
  char name[256];
  Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  for (Int_t j=0;j<8;j++)
    {
      memset(name,0,256);
      sprintf(name,"dcu_channel_0x%.4x_%.1d",id,j);
      fDcuChannel[j]=  new TH1S(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);
    }

  
  memset(name,0,256);
  sprintf(name,"wafer_temperature_0x%.4x",id);
  fWaferTemperature = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);

  memset(name,0,256);
  sprintf(name,"v250_bias_0x%.4x",id);
  f250Bias = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);

  memset(name,0,256);
  sprintf(name,"v125_bias_0x%.4x",id);
  f125Bias = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);

  memset(name,0,256);
  sprintf(name,"wafer_current_0x%.4x",id);
  fWaferCurrent = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);

  memset(name,0,256);
  sprintf(name,"hybrid_temperature_0x%.4x",id);
  fHybridTemperature = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);

  memset(name,0,256);
  sprintf(name,"dcu_temperature_0x%.4x",id);
  fDcuTemperature = new TH1D(name,name,DCU_DEPTH,0.1,DCU_DEPTH+0.1);
  fCount=0;
}  
DCUHisto::~DCUHisto()
{
  for (int j=0;j<8;j++)
    delete fDcuChannel[j];
  delete fWaferTemperature;
  delete f250Bias;
  delete f125Bias;
  delete fWaferCurrent;
  delete fHybridTemperature;
  delete fDcuTemperature;
}
void DCUHisto::reset()
{
  fCount=0;
 for (int j=0;j<8;j++)
     fDcuChannel[j]->Reset();
   fWaferTemperature->Reset();
   f250Bias->Reset();
   f125Bias->Reset();
   fWaferCurrent->Reset();
   fHybridTemperature->Reset();
   fDcuTemperature->Reset();
  return;
}

Float_t DCUHisto::TemperatureWafer(int adcu)
{
  Float_t B=3090.;
  Float_t invt=1./(273.+24.7)+log(adcu/201.)/B;
  return (1./invt)-273.;
}
Float_t  DCUHisto::TemperatureHybrid(int adcu)
{
  Float_t B=3090.;
  Float_t invt=1./(273.+25.0)+log(adcu/220.)/B;
  return (1./invt)-273.;
}

Float_t  DCUHisto::TemperatureDcu(int adcu)
{
  Float_t t=(adcu-2632)/9.22+(273+24.0);
  return t-273.;
}
Float_t  DCUHisto::V250Bias(int adcu){ return adcu/2230.*2.;}
Float_t  DCUHisto::V125Bias(int adcu){return adcu/2230.*2.;}

void DCUHisto::update(UShort_t* dcu)
{

  fCount++;

  for (Int_t l=0;l<8;l++)
    {
      fDcuChannel[l]->SetBinContent(fCount%DCU_DEPTH,dcu[l]*1.);
    }
  fWaferTemperature->SetBinContent(fCount%DCU_DEPTH,TemperatureWafer(dcu[0]));
  f250Bias->SetBinContent(fCount%DCU_DEPTH,V250Bias(dcu[1]));
  f125Bias->SetBinContent(fCount%DCU_DEPTH,V125Bias(dcu[2]));
  fWaferCurrent->SetBinContent(fCount%DCU_DEPTH,dcu[3]*10.);
  fHybridTemperature->SetBinContent(fCount%DCU_DEPTH,TemperatureHybrid(dcu[4]));
  fDcuTemperature->SetBinContent(fCount%DCU_DEPTH,TemperatureDcu(dcu[7]));
}

TBClusterHisto::TBClusterHisto(TBGeo* g)
{
  Int_t ns=768;
  Int_t id=0xffff;
  if (g!=NULL)
{
   ns= g->GetLast() - g->Get1st()+1;
   id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
printf("Cluster Histo %x First %d Last %d \n",id,g->Get1st(),g->GetLast(),ns);
}

  

  char name[256];

  memset(name,0,256);
  sprintf(name,"number_of_strip_0x%.4x",id);
  fNumberOfStrip=  new TH1S(name,name,20,0.1,20.1);
  fNumberOfStrip->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"cluster_noise_0x%.4x",id);
  fClusterNoise = new TH1D(name,name,100,0.,20.);
  fClusterNoise->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"cluster_signal_0x%.4x",id);
  fClusterSignal = new TH1D(name,name,200,0.,50.);
  fClusterSignal->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"highest_cluster_signal_0x%.4x",id);
  fClusterSignalHigh = new TH1D(name,name,200,0.,50.);
  fClusterSignalHigh->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"cluster_position_0x%.4x",id);
  fClusterPosition = new TH1D(name,name,ns,0.1,ns+0.1);
  fClusterPosition->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"seed_noise_0x%.4x",id);
  fSeedNoise = new TH1D(name,name,100,0.,20.);
  fSeedNoise->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"seed_signal_0x%.4x",id);
  fSeedSignal = new TH1D(name,name,200,0.,50.);
  fSeedSignal->SetDirectory(0);

  memset(name,0,256);
  sprintf(name,"seed_position_0x%.4x",id);
  fSeedPosition = new TH1D(name,name,ns,0.1,ns+0.1);
  fSeedPosition->SetDirectory(0);  

}  
TBClusterHisto::~TBClusterHisto()
{
  delete fNumberOfStrip;
  delete fClusterNoise;
  delete fClusterSignal;
  delete fClusterSignalHigh;
  delete fClusterPosition;
  delete fSeedNoise;
  delete fSeedSignal;
  delete fSeedPosition;
}
void TBClusterHisto::reset()
{
  fNumberOfStrip->Reset();
  fClusterNoise->Reset();
  fClusterSignal->Reset();
  fClusterSignalHigh->Reset();
  fClusterPosition->Reset();
  fSeedNoise->Reset();
  fSeedSignal->Reset();
  fSeedPosition->Reset();
  return;
}


void TBClusterHisto::update(TObjArray* clusters,float thr)
{
  float v=-9999;
  for (Int_t l=0;l<clusters->GetEntries();l++)
    {
      TBClu* cl = (TBClu*) clusters->At(l);
      if ((cl->GetHighest()/cl->GetSeedNoise())<thr) continue;
      fNumberOfStrip->Fill(cl->GetNstrip()*1.);
      fSeedPosition->Fill(cl->GetHighestStrip()*1.);
      fSeedSignal->Fill(cl->GetHighest()/cl->GetSeedNoise());
      fClusterSignal->Fill(cl->GetV()/cl->GetSeedNoise());
      if (cl->GetV()/cl->GetSeedNoise()>v) v=cl->GetV()/cl->GetSeedNoise();
      fClusterNoise->Fill(cl->GetNoise());
      fClusterPosition->Fill(cl->GetXLocal());
    }
  if (v>0) fClusterSignalHigh->Fill(v);
}

TBCalibrationHisto::TBCalibrationHisto(TBGeo* g)
{
 Int_t ns= g->GetLast()-g->Get1st()+1;
 Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];
  for (Int_t i=0;i<768;i++)
    fStrip[i]=NULL;
  for (Int_t i=0;i<ns;i++) // 1 per module
    //  for (Int_t i=0;i<1;i++)
    {
      memset(name,0,256);
      sprintf(name,"calibration_strip_0x%.4x_%.3d",id,i);
      fStrip[i]=  new TProfile(name,name,100,0.1,100.1,-1024.,1024.," ");
      fStrip[i]->SetDirectory(0);
    }
}
TBCalibrationHisto::~TBCalibrationHisto()
{
  for (Int_t i=0;i<768;i++)
    if (fStrip[i]!=NULL) delete fStrip[i];
}

void TBCalibrationHisto::reset()
{
  for (Int_t i=0;i<768;i++)
    if (fStrip[i]!=NULL) fStrip[i]->Reset();
}


void TBCalibrationHisto::update(TBEventHeader* h, TBRaw* r)
{      
  Int_t isub,ical = h->GetCalChannel();
  Float_t t;
  if (ical<4) 
    isub = ical+4;
  else
    isub= ical-4;
  //  printf("cal %d %d %d %d \n",h->GetCalChannel(),h->GetCalSel(),ical,isub);
  t = (100-h->GetLatency())*8+(7-h->GetCalSel())+0.5;  
  if (h->GetTriggerType()==33) 
    t = (102-h->GetLatency())*8+(7-h->GetCalSel())+0.5;
  TBGeo* g = r->GetGeo();
  Int_t sign=g->GetPolarity();

  for (int j=0;j<2*g->GetNChips();j++)
    {
      Float_t vped=0;
      for (int k=isub;k<128;)
	{
	  vped+=r->GetADC(j*128+k);
	  k+=8;
	}
      vped/=16.;
      Float_t vcal;
      for (int k=ical;k<128;)
	{
	  vcal=r->GetADC(j*128+k);
	  fStrip[j*128+k]->Fill(t,(vcal-vped)*sign); // 1 histo
	  //fStrip[0]->Fill(t,(vcal-vped)*sign); 

	  //	  if (k == ical)
	  //  printf("%f diff %f %f %f \n",t,vcal,vped,vcal-vped);
	  k+=8;
	}
    }
}
void TBCalibrationHisto::update(TBEventHeader* h, TBRaw* r, TBPed* p)
{      
  Int_t isub,ical = h->GetCalChannel();
  Float_t t;
  if (ical<=4) 
    isub = ical+4;
  else
    isub= ical-4;
  //  printf("%d %d \n",ical,isub);
  t = (100-h->GetLatency())*8+(7-h->GetCalSel())+0.5;  
  if (h->GetTriggerType()==33) 
    t = (102-h->GetLatency())*8+(7-h->GetCalSel())+0.5;
  TBGeo* g = r->GetGeo();
  Int_t sign=g->GetPolarity();

  for (int j=0;j<2*g->GetNChips();j++)
    {
      Float_t vped=0;
      for (int k=isub;k<128;)
	{
	  vped+=r->GetADC(j*128+k)-p->GetMean(k);
	  k+=8;
	}
      vped/=16.;
      Float_t vcal;
      for (int k=ical;k<128;)
	{
	  vcal=r->GetADC(j*128+k)-p->GetMean(k);
	  fStrip[j*128+k]->Fill(t,(vcal-vped)*sign); // 1 histo
	  //fStrip[0]->Fill(t,(vcal-vped)*sign); // 1 histo
	  //	  if (k == ical)
	  //  printf("%f diff %f %f %f \n",t,vcal,vped,vcal-vped);
	  k+=8;
	}
    }
}
TBTimingHisto::TBTimingHisto(TBGeo* g)
{
 
  Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];
  fTick[0]=NULL;fTick[1]=NULL;fTick[2]=NULL;
  fTickFed[0]=NULL;fTickFed[1]=NULL;fTickFed[2]=NULL;
  for (Int_t i=0;i<g->GetNChips();i++)
    {
      memset(name,0,256);
      sprintf(name,"tick_chip_0x%.4x-%.3d",id,i);
      fTick[i]=  new TProfile(name,name,40*24,0.1,(40*25)+.1,0.,1050.," ");
      fTick[i]->SetDirectory(0);
      memset(name,0,256);
      sprintf(name,"tickfed_chip_0x%.4x-%.3d",id,i);
      fTickFed[i]=  new TProfile(name,name,40*25,0.1,(40*25)+.1,0.,1050.," ");
      fTickFed[i]->SetDirectory(0);
    }
}
TBTimingHisto::~TBTimingHisto()
{
  for (Int_t i=0;i<3;i++)
    {
      if (fTick[i]!=NULL) delete fTick[i];
      if (fTickFed[i]!=NULL) delete fTickFed[i];
    }
}
void TBTimingHisto::reset()
{
  for (Int_t i=0;i<3;i++)
    {
      if (fTick[i]!=NULL)  fTick[i]->Reset();
      if (fTickFed[i]!=NULL)  fTickFed[i]->Reset();
    }
}

void TBTimingHisto::update(TBEventHeader* h, TBRaw* r)
{      
 Float_t t;

 TBGeo* g = r->GetGeo();
 // Reinitialize for TIMING_FED

 int type=(h->GetTriggerType() == TBEventHeader::TIMING)?0:1;
 // printf("Id %x Time %d  %d %d\n",g->GetIdLocal(),h->GetPllFine(),r->GetHisto(0)->fArray[51],type);
  for (int j=0;j<g->GetNChips();j++)
    {
      
      Float_t vped=0;
      for (int is=0;is<40;is++)
	{
	   t = (type==0)?(is+1)*25-(h->GetPllFine()*25./24.):(is+1)*25-(h->GetPllFine()*1.);
	   vped=r->GetHisto(j)->fArray[is+1];
	   if (type == 0) 
	     fTick[j]->Fill(t,vped);
	   else
	     fTickFed[j]->Fill(t,vped);
	}  

    }
}




TBOptoHisto::TBOptoHisto(TBGeo* g)
{
 
 Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];
  for (Int_t ga=0;ga<ngain;ga++)
    {
      for (Int_t i=0;i<nlaser;i++)
	{
	  fTick[i][ga]=NULL;
	  fBase[i][ga]=NULL;
	  fDiff[i][ga]=NULL;
	}
    for (Int_t i=0;i<g->GetNChips();i++)
      {
	memset(name,0,256);
	sprintf(name,"tick_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fTick[i][ga]=  new TProfile(name,name,63,0.1,63+.1,0.,1026.," ");
	fTick[i][ga]->SetDirectory(0);
	memset(name,0,256);
	sprintf(name,"base_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fBase[i][ga]=  new TProfile(name,name,63,0.1,63+.1,0.,1026.," ");
	fBase[i][ga]->SetDirectory(0);
	memset(name,0,256);
	sprintf(name,"diff_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fDiff[i][ga]=  new TH1D(name,name,63,0.1,63+.1);
	fDiff[i][ga]->SetDirectory(0);


      }
    }
}
TBOptoHisto::~TBOptoHisto()
{
  for (Int_t g=0;g<ngain;g++)
    for (Int_t i=0;i<nlaser;i++)
      {
	if (fTick[i][g]!=NULL) delete fTick[i][g];
	if (fBase[i][g]!=NULL) delete fBase[i][g];
	if (fDiff[i][g]!=NULL) delete fDiff[i][g];
      }
}
void TBOptoHisto::reset()
{
  for (Int_t g=0;g<ngain;g++)
    for (Int_t i=0;i<nlaser;i++)
      {
	if (fTick[i][g]!=NULL)  fTick[i][g]->Reset();
	if (fBase[i][g]!=NULL)  fBase[i][g]->Reset();
	if (fDiff[i][g]!=NULL)  fDiff[i][g]->Reset();
      }
}

void TBOptoHisto::update(TBEventHeader* h, TBRaw* r)
{      
 Float_t t;

 TBGeo* g = r->GetGeo();
 int gain = h->GetOptoGain();
 float bias = h->GetOptoBias()*1.0;
 for (int j=0;j<g->GetNChips();j++)
   {
     
     Float_t vmin=9999.,vmax=0;
     for (int is=0;is<128;is++)
       {
	 float v=r->GetHisto(j)->fArray[is+1];
	 vmin= (v<vmin)?v:vmin;
	 vmax= (v>vmax)?v:vmax;
       }
     float med=(vmin+vmax)/2.;
     float amp = (vmax-vmin);
     for (int is=0;is<128;is++)
       {
	 float v=r->GetHisto(j)->fArray[is+1];;
	 if (amp>1)
	   {
	     if (v<med) 
	       fBase[j][gain]->Fill(bias,v,1.);
	     else
	       fTick[j][gain]->Fill(bias,v,1.);
	   }
	 else
	   {
	     // NO more tick
	     fBase[j][gain]->Fill(bias,v,1.);
	     fTick[j][gain]->Fill(bias,v,1.);
	   }
       }

 
     for (int i=0;i<63;i++)
       fDiff[j][gain]->SetBinContent(i+1,fTick[j][gain]->GetBinContent(i)-fBase[j][gain]->GetBinContent(i));
   }
}
TBLatencyHisto::TBLatencyHisto(TBGeo* g)
{
 
  Int_t id=0xFFFF;
  if (g!=0)  id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];

  memset(name,0,256);
  sprintf(name,"latency_scan_signal_0x%.4x",id);
  fClusterSignal=  new TProfile(name,name,200,0.1,200.1,10.,50.," ");
  fClusterSignal->SetDirectory(0);
  memset(name,0,256);
  sprintf(name,"latency_scan_ncluster_0x%.4x",id);
  fNumberOfCluster=  new TH1D(name,name,200,0.1,200.1);
  fNumberOfCluster->SetDirectory(0);
}
TBLatencyHisto::~TBLatencyHisto()
{
  delete fClusterSignal;
  delete fNumberOfCluster;
}
void TBLatencyHisto::reset()
{
   fClusterSignal->Reset();
   fNumberOfCluster->Reset();
}

void TBLatencyHisto::update(TBEventHeader* h, TObjArray* clusters)
{      
 Float_t t=h->GetLatency();
 //if (clusters->GetEntriesFast()>0) fNumberOfCluster->Fill(t,clusters->GetEntriesFast()*1.);
  float xnclu=0.;
  for (Int_t l=0;l<clusters->GetEntriesFast();l++)
    {
      TBClu* cl = (TBClu*) clusters->At(l);
      fClusterSignal->Fill(t,cl->GetV()/cl->GetSeedNoise(),1.);
	if (cl->GetV()/cl->GetSeedNoise()>10.) xnclu +=1.;
    }
    fNumberOfCluster->Fill(t,xnclu);
}
TBDelayHisto::TBDelayHisto(TBGeo* g)
{
 
  Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];

  memset(name,0,256);
  sprintf(name,"delay_scan_0x%.4x",id);
  fClusterSignal=  new TProfile(name,name,200,0.1,200*25./24.+.1,8.,50.," ");
  fClusterSignal->SetDirectory(0);
  memset(name,0,256);
  sprintf(name,"delay_scan_ncluster_0x%.4x",id);
  fNumberOfCluster=  new TH1D(name,name,200,0.1,200*25./24.+0.1);
  fNumberOfCluster->SetDirectory(0);
  
}
TBDelayHisto::~TBDelayHisto()
{
  delete fClusterSignal;
  delete fNumberOfCluster;
}


void TBDelayHisto::reset()
{
  fClusterSignal->Reset();
  fNumberOfCluster->Reset();
}

void TBDelayHisto::update(TBEventHeader* h, TObjArray* clusters)
{
 Float_t t=h->GetPllCoarse()*25.+h->GetPllFine()*25./24.;
 if (clusters->GetEntriesFast()>0) fNumberOfCluster->Fill(t,clusters->GetEntriesFast()*1.);

  for (Int_t l=0;l<clusters->GetEntriesFast();l++)
    {
      TBClu* cl = (TBClu*) clusters->At(l);
      // fClusterSignal->Fill(t,cl->GetV()/cl->GetSeedNoise(),1.);
      // Better alignement on highest strip from efficiency point of view
      fClusterSignal->Fill(t,cl->GetHighest()/cl->GetSeedNoise(),1.);
    }
}






TBVpspHisto::TBVpspHisto(TBGeo* g)
{

 Int_t id=(g->GetDcuId()!=0)?g->GetDcuId():g->GetIdLocal();
  char name[256];
  napv = g->GetNChips()*2; 
#ifdef VPSPPED
  for (Int_t j=0;j<MAXVPSP;j++)
    fPed[j] = new TBPed(g);
#endif

  for (int j=0;j<6;j++)
    {
#ifdef VPSPPED
      fMeanNoise[j]= NULL;
#endif
      fMeanMean[j]= NULL;
    }
  for (Int_t j=0;j<napv;j++)
    {
#ifdef VPSPPED
      memset(name,0,256);
      sprintf(name,"vpsp_noise_0x%.4x_%.1d",id,j);
      fMeanNoise[j]=  new TProfile(name,name,60,0.1,60.1,0., 20.," ");
#endif
      memset(name,0,256);
      sprintf(name,"vpsp_mean_0x%.4x_%.1d",id,j);
      fMeanMean[j]=  new TProfile(name,name,60,0.1,60.1,0., 1025.," ");
      fMeanMean[j]->SetDirectory(0);

    }
  fvpsp=0;
}
TBVpspHisto::~TBVpspHisto()
{
  for (Int_t j=0;j<napv;j++)
    {
#ifdef VPSPPED
        delete fMeanNoise[j];
#endif
        delete fMeanMean[j];
    }
#ifdef VPSPPED
  for (Int_t i=0;i<MAXVPSP;i++)
    {
      if (fPed[i]!=NULL) delete fPed[i];
    }
#endif
}
void TBVpspHisto::reset()
{
  for (Int_t j=0;j<napv;j++)
    {
#ifdef VPSPPED
        fMeanNoise[j]->Reset();
#endif
        fMeanMean[j]->Reset();
    }
#ifdef VPSPPED
  for (Int_t i=0;i<MAXVPSP;i++)
    {
      if (fPed[i]!=NULL)  fPed[i]->Clear();
    }
#endif
  fvpsp=0;
}

void TBVpspHisto::update(TBEventHeader* h, TBRaw* r)
{
 Float_t t;
 TBGeo* g = r->GetGeo();
 if (getModuleFecChannel(g->GetIdAcq(0))!=h->GetVpspI2c()) return;
 // Reinitialize for TIMING_FED
#ifdef VPSPPED
 int type=(h->GetTriggerType() == TBEventHeader::VPSPSCAN)?0:1;
 if (h->GetVpsp()> fvpsp )
   {
     if (fvpsp>0)
       {
	 TBPed* p= fPed[fvpsp];
	 for (int j=0;j<p->GetNstrip()/128;j++)
	   for (int l=0;l<128;l++)
	     {
	       fMeanNoise[j]->Fill(fvpsp*1.0,p->GetRMS(j*128+l));
	       fMeanMean[j]->Fill(fvpsp*1.0,p->GetMean(j*128+l));
	     }
       }
     fvpsp = h->GetVpsp();
   }
 // printf("Id %x Time %d  %d %d\n",g->GetIdLocal(),h->GetPllFine(),r->GetHisto(0)->fArray[51],type);
 TBPed* p= fPed[h->GetVpsp()];
 p->Increment(r);
#else
 fvpsp = h->GetVpsp();
 for (int j=0;j<napv;j++)
   for (int l=0;l<128;l++)
     {
       fMeanMean[j]->Fill(fvpsp*1.0,r->GetADC(j*128+l));
     }
#endif

}





TBMonitor::TBMonitor(bool calib)
{
  useCalib_ = calib;
  for (int i=0;i<MAX_DET;i++)
    {
      fDCUHisto[i]=NULL;
      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
      fVpspHisto[i]=NULL;
    }
  fLatencyAll=NULL;
  fClusterAll = NULL;
  header_ = new TBEventHeader();

  fTOut=NULL;
  fT=NULL;
  fmfile=NULL;

  fHistoList = new mapTH1();
  fHistoNames = new mapString();

  fTRUOut=NULL;
  fTRU=NULL;
  fRaw2Idet.clear();
}
TBMonitor::~TBMonitor()
{
  delete header_;

  for (int i=0;i<MAX_DET;i++)
    {
     
      
      if (fDCUHisto[i]!=NULL) delete fDCUHisto[i];
      if (fClusterHisto[i]!=NULL) delete fClusterHisto[i];
      if (fCalibrationHisto[i]!=NULL) delete fCalibrationHisto[i];
      if (fTimingHisto[i]!=NULL) delete fTimingHisto[i];
      if (fOptoHisto[i]!=NULL) delete fOptoHisto[i];
      if (fLatencyHisto[i]!=NULL) delete fLatencyHisto[i];
      if (fDelayHisto[i]!=NULL) delete fDelayHisto[i];
      if (fVpspHisto[i]!=NULL) delete fVpspHisto[i];
      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
      fVpspHisto[i]=NULL;
    }
  if (fLatencyAll!=NULL) delete fLatencyAll;
  fLatencyAll =NULL;
  if (fClusterAll!=NULL) delete fClusterAll;
  fClusterAll = NULL;
  printf("Deleting TBMonitor \n");
}
void TBMonitor::newEvent(Int_t *pilot)
{


  header_->Update(pilot);
  header_->setLatency(pilot[4051]);
  header_->setCalSel(pilot[4052]);
  header_->setCalChannel(pilot[4053]);

  header_->setPllCoarse(pilot[4081]);
  header_->setPllFine(pilot[4082]);

  header_->setOptoGain(pilot[4091]);
  header_->setOptoBias(pilot[4092]);

  if (header_->GetTriggerType() == TBEventHeader::CONNECTION)
    header_->setCalChannel(pilot[4093]);

  header_->setProcess(pilot[4094]);
  header_->setVpsp(pilot[4095]);
  header_->setVpspI2c(pilot[4096]);
  clearFilled();
}


void TBMonitor::newDCUChannel(Int_t idloc,UShort_t* data)
{
  for (int i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      TBGeo* g=(TBGeo*) fListOfDet->At(i);
      if (idloc!=g->GetIdLocal()) continue;
      fDCUHisto[i]->update(data);
    }
}

void TBMonitor::newFedChannel(Int_t idacq,UShort_t* data)
{
  if (fRaw2Idet.size() == 0)
    {
      cout << "Filling Raw2Idet map " << endl;
      for (int i=0;i<fListOfDet->GetEntries();i++)
	{
	  
	  TBRaw* r = (TBRaw*) fListOfRaw->At(i); 
	  pair<TBRaw*,int> pr(r,i);
	  fRaw2Idet.insert(pr);
	}
    }

  int idacq0=buildFedChannelKey(idacq&0xFFFFFFF0,1,1);
  TBRaw* r0 = mapRaw[idacq0];
  if (r0 == NULL)
    {
      cout << " No TBRaw assoicated to " << hex <<idacq0 << dec <<endl;
      return;
    }
  TBGeo* g =  r0->GetGeo();
  if (g== NULL)
    {
        cout << " No TBGeo assoicated to " << hex <<idacq0 << dec <<endl;
      return;
    }
  for (int j=0;j<g->GetNChips();j++)
    {
      if (idacq!=g->GetIdAcq(j)) continue;
      
      r0->Fill(j,data);
    }
  //  cout << "r0 filled " <<endl;
  // cout << fRaw2Idet[r0] << endl;
  int idet = fRaw2Idet[r0];
  if (idet>-1 && idet<MAX_DET)
    setFilled(idet);
  else
    cout << "Wriong IDet " << idet <<endl;

//   int idet=-1;
//   for (int i=0;i<fListOfDet->GetEntriesFast();i++)
//     {
//       TBGeo* g=(TBGeo*) fListOfDet->At(i);
//       TBRaw* r = (TBRaw*) fListOfRaw->At(i); 
//       for (int j=0;j<g->GetNChips();j++)
// 	{
// 	  if (idacq!=g->GetIdAcq(j)) continue;
// 	  idet=i;
// 	  r->Fill(j,data);
// 	}
//     }
//   if (idet!=-1) 
//     { 
//       //printf("Det %d is filled\n",idet);
//       setFilled(idet); }
}

void TBMonitor::newReadoutUnit(Int_t inst,Int_t* data,Int_t si)
{
  if (fListOfRU == NULL) return;
  for (int i=0;i<fListOfRU->GetEntries();i++)
    {
      TBRU* r = (TBRU*) fListOfRU->At(i); 
      if (inst!=r->getInstance()) continue;
      r->copyBuffer(data,si);
    }
}


void TBMonitor::reorder(Int_t process)
{
  //  printf("Process Mode %d \n",process);
  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBRaw* r = (TBRaw*) fListOfRaw->At(k);
      r->ReOrder(process);
      //     printf("%d is reordered\n",k);
    }
}

void TBMonitor::updatePedestal()
{
  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k);
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      p->Increment(r); // Update Pedestals
    }
}


void TBMonitor::updateNoise()
{
  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,p);
      
      n->Increment(c); 
      
    }
}

void TBMonitor::updateClusters()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fClusterHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fClusterHisto[k]= new TBClusterHisto(g);
	}
      if (fClusterAll == NULL)
	{
	  fHistoOut->cd();
	  fClusterAll = new TBClusterHisto(0);
	}

      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k);

      c->Fill(r,p,n,header_);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);

      fClusterHisto[k]->update(fListOfClu);
      fClusterAll->update(fListOfClu,8); // Fill it only with cluster with S/N > 8

    }

}
void TBMonitor::updateCalibration()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fCalibrationHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fCalibrationHisto[k]= new TBCalibrationHisto(g);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      
      if (fCalibrationHisto[k]!=NULL)
	fCalibrationHisto[k]->update(header_,r,p);
    }


}

void TBMonitor::updateTimingAdjustement()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {

      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fTimingHisto[k] == NULL)
	{
	  printf("Det %d  is timing created \n",k);
	  //	  getchar();
	  fHistoOut->cd();
	  // getchar();
	  fTimingHisto[k]= new TBTimingHisto(g);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      if (fTimingHisto[k]!=NULL)
	fTimingHisto[k]->update(header_,r);
    }

}

void TBMonitor::updateOptoAdjustement()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fOptoHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fOptoHisto[k]= new TBOptoHisto(g);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      fOptoHisto[k]->update(header_,r);
    }

}

void TBMonitor::updateLatencyCurve()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fLatencyHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fLatencyHisto[k]= new TBLatencyHisto(g);
	}
      if (fLatencyAll == NULL)
	{
	  fHistoOut->cd();
	  fLatencyAll= new TBLatencyHisto(0);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,n,header_);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      fLatencyHisto[k]->update(header_,fListOfClu);
      fLatencyAll->update(header_,fListOfClu);
    }

}

void TBMonitor::updateDelayCurve()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fDelayHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fDelayHisto[k]= new TBDelayHisto(g);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,n,header_);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      fDelayHisto[k]->update(header_,fListOfClu);
    }

}


void TBMonitor::updateVpspScan()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      if(!isFilled(k)) continue;
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      if (fVpspHisto[k] == NULL)
	{
	  fHistoOut->cd();
	  fVpspHisto[k]= new TBVpspHisto(g);
	}
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      fVpspHisto[k]->update(header_,r);
    }

}


Int_t TBMonitor::updateConnection()
{


  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      //      fOptoHisto[k]->update(header_,r);

      
      int idOn = header_->GetCalChannel();

      for (int j=0;j<g->GetNChips();j++)
	{
	  
	  Float_t vmin=1025,vmax=-1;;
	  for (int is=0;is<280;is++)
	    {
	      float v=r->GetHisto(j)->fArray[is+1];
	      vmin= (v<vmin)?v:vmin;
	      vmax= (v>vmax)?v:vmax;
	    }
	  float maxdif=(vmax-vmin);
	  
	  // printf("%x %x %f %f %f \n",idOn,g->GetIdAcq(j),vmin,vmax,maxdif);

	  if (maxdif>70 || vmax>800) // Channel is ON
	    {
	      printf("Maximum %f %f %f \n",vmin,vmax,maxdif);
	      if (g->GetIdAcq(j) == idOn)
		{
		  printf("Channel found OK========> %x \n",idOn);
		  return 0;
		}
	      else
		{
// 		  printf("Error in channel association \n");
// 		  printf("\t Expected %x \n",idOn);
// 		  printf("\t Seen  %x \n",g->GetIdAcq(j));
		  return g->GetIdAcq(j);
		}

	    }

	}






    }

  return -1;

}


void TBMonitor::saveDCUHistos()
{

  time_t tout=time(0);
  Char_t fnout[256];
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    sprintf(fnout,"/tmp/TBMonitor_DCU_%.8x.root",tout);
  else
    sprintf(fnout,"%s/TBMonitor_DCU_%.8x.root",fscr,tout);
  TFile* fout = new TFile(fnout,"RECREATE");
  fout->cd();
  Char_t name[256];
  for(int i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      printf("Writing histo %d \n",i);
      fout->cd();
      memset(name,0,256);sprintf(name,"Detector%.2d",i);
      TDirectory *det = new TDirectory(name,name,"");
      printf("Writing histo %d %s \n",i,name);
      det->cd();
      if (fDCUHisto[i]!=NULL)  
	
	{
	  for (int j=0;j<8;j++)
	    if (fDCUHisto[i]->getDcuChannel(j)!=NULL)
	      fDCUHisto[i]->getDcuChannel(j)->Write();

	  fDCUHisto[i]->getWaferTemperature()->Write();
	  fDCUHisto[i]->get250Bias()->Write();
	  fDCUHisto[i]->get125Bias()->Write();
	  fDCUHisto[i]->getWaferCurrent()->Write();
	  fDCUHisto[i]->getHybridTemperature()->Write();
	  fDCUHisto[i]->getDcuTemperature()->Write();
	      

	  printf("Done \n");
	}
      //      getchar();
      det->cd();
    }
  fout->Close();
}


void TBMonitor::deleteHistos()
{
  deleteHistos(0);
}

void TBMonitor::deleteHistos(int iseq)
{

  // Exit if no detectors defined
  if (fListOfDet == NULL) return;
  if (fListOfDet->GetEntries()<=0) return;
  cout << " Jentre dans deleteHistos " << endl;
  cout << fListOfDet->GetEntries() << " Are found " << endl;
  Char_t fnout[256];
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    sprintf(fnout,"/tmp/TBMonitor%.7d_%.3d.root",header_->GetRun(),iseq);
  else
    sprintf(fnout,"%s/TBMonitor%.7d_%.3d.root",fscr,header_->GetRun(),iseq);
  TFile* fout = new TFile(fnout,"RECREATE");
  fout->SetCompressionLevel(0);
  fout->cd();
  Char_t name[256];
  cout << " Writing histograms " <<__LINE__ << endl;

  // Writing summary histograms
  memset(name,0,256);sprintf(name,"Summary");
  TDirectory *det = new TDirectory(name,name,"");
  det->cd();
  if (fLatencyAll!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY)  )
    {
      TDirectory *latd = new TDirectory("Latency","Latency","");
      latd->cd();
      fLatencyAll->getLatencyNumberOfCluster()->Write();
      fLatencyAll->getLatencyClusterSignal()->Write();
      latd->Write();
    }
   det->cd();
   if (fClusterAll!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::PHYSIC ||GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY  ) )
	{
	  printf("\t cluster ");
	  TDirectory *clud = new TDirectory("Cluster","Cluster","");
	  clud->cd();

	  fClusterAll->getNumberOfStrip()->Write();
	  fClusterAll->getClusterNoise()->Write();
	  fClusterAll->getClusterSignal()->Write();
	  fClusterAll->getClusterSignalHigh()->Write();
	  fClusterAll->getClusterPosition()->Write();
	  fClusterAll->getSeedNoise()->Write();
	  fClusterAll->getSeedSignal()->Write();
	  fClusterAll->getSeedPosition()->Write();
	  printf("Done \n");
	  clud->Write();
	}
  cout << " Looping on detectors " <<__LINE__ << endl;
  for(int i=0;i<fListOfDet->GetEntries();i++)
    {
      cout << " Writing histograms " <<i << "  at line " <<__LINE__ << endl;
      fout->cd();
      memset(name,0,256);sprintf(name,"Detector%.2d",i);
      TDirectory *det = new TDirectory(name,name,"");
      printf("Writing histo %d %s \n",i,name);
      det->cd();
      if ((GetEventHeader()->GetTriggerType() == TBEventHeader::PEDESTAL) )  
	
	{
	  printf("\t Pedestal ");
	 
	  TDirectory *pedd = new TDirectory("Pedestal","Pedestal","");
	  pedd->cd();
	  TBPed* p= (TBPed*) fListOfPedestal->At(i);
	  TBPed* n= (TBPed*) fListOfNoise->At(i);
	  p->GetProfile()->Write();
	  n->GetProfile()->Write();
	  p->GetHistoRMS()->Write();
	  n->GetHistoRMS()->Write();
	  n->GetCommonModeNoise()->Write();
	  pedd->Write();
	  printf("Done \n");
	}


      det->cd();
      if (fCalibrationHisto[i]!=NULL &&  (GetEventHeader()->GetTriggerType() == TBEventHeader::CALIBRATION ||GetEventHeader()->GetTriggerType() == 33  ) )  
	
	{

	  cout << __LINE__ << "Calibration " << endl;
	 
	  TDirectory *cald = new TDirectory("Calibration","Calibration","");
	  cald->cd();
	  
	  for (int j=0;j<768;j++) 
	    if (fCalibrationHisto[i]!=NULL)
	      {
		if (fCalibrationHisto[i]->getCalibrationStrip(j)!=NULL)
		  {
		    
		    fCalibrationHisto[i]->getCalibrationStrip(j)->Write();
		    
		
		  }
	      }
	  cald->Write();
	  printf("Done \n");
	}
      //      getchar();
      det->cd();

      if (fClusterHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::PHYSIC ||GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY  ) )
	{
	  printf("\t cluster ");
	  TDirectory *clud = new TDirectory("Cluster","Cluster","");
	  clud->cd();

	  fClusterHisto[i]->getNumberOfStrip()->Write();
	  fClusterHisto[i]->getClusterNoise()->Write();
	  fClusterHisto[i]->getClusterSignal()->Write();
	  fClusterHisto[i]->getClusterSignalHigh()->Write();
	  fClusterHisto[i]->getClusterPosition()->Write();
	  fClusterHisto[i]->getSeedNoise()->Write();
	  fClusterHisto[i]->getSeedSignal()->Write();
	  fClusterHisto[i]->getSeedPosition()->Write();
	  printf("Done \n");
	  clud->Write();
	}
      det->cd();
      if (fTimingHisto[i]!=NULL && ((GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING) || (GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING_FED))  )
	{
	  printf("\t timing ");
	  TDirectory *timd = new TDirectory("Timing","Timing","");
	  timd->cd();

	  for (int j=0;j<3;j++)
	    if (fTimingHisto[i]->getTimingTick(j)!=NULL)
	      fTimingHisto[i]->getTimingTick(j)->Write();
	  for (int j=0;j<3;j++)
	    if (fTimingHisto[i]->getTimingTickFed(j)!=NULL)
	      fTimingHisto[i]->getTimingTickFed(j)->Write();

	  timd->Write();
	  printf("Done \n");
	}
      det->cd();
      if (fDelayHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::DELAY || GetEventHeader()->GetTriggerType() == TBEventHeader::DELAY_TTC  )  )
	{
	  TDirectory *deld = new TDirectory("Delay","Delay","");
	  deld->cd();

	  fDelayHisto[i]->getDelayClusterSignal()->Write();
	  fDelayHisto[i]->getDelayNumberOfCluster()->Write();

	  deld->Write();
	}
      det->cd();
      if (fOptoHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::GAINSCAN)  )
	{
	  TDirectory *optod = new TDirectory("Opto","Opto","");
	  optod->cd();
	  for (int g=0;g<4;g++)
	    for (int j=0;j<3;j++)
	      {
		//		printf("hsit %d %d \n",g,j);
		if (fOptoHisto[i]->getOptoTick(j,g)!=NULL)
		  fOptoHisto[i]->getOptoTick(j,g)->Write();
		//printf("Base hsit %d %d \n",g,j);
		if (fOptoHisto[i]->getOptoBase(j,g)!=NULL)
		  fOptoHisto[i]->getOptoBase(j,g)->Write();
		if (fOptoHisto[i]->getOptoDiff(j,g)!=NULL)
		  fOptoHisto[i]->getOptoDiff(j,g)->Write();
	      }
	  optod->Write();
	}

      det->cd();
      if (fVpspHisto[i]!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::VPSPSCAN)  )
	{
	  TDirectory *vpspd = new TDirectory("Vpsp","Vpsp","");
	  vpspd->cd();
	  for (int j=0;j<6;j++)
	    if (fVpspHisto[i]->getMeanMean(j)!=NULL)
	      {
#ifdef VPSPPED
		fVpspHisto[i]->getMeanNoise(j)->Write();
#endif
		fVpspHisto[i]->getMeanMean(j)->Write();
	      }
	  vpspd->Write();
	}

     

      det->cd();
      if (fLatencyHisto[i]!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY)  )
	{
	  TDirectory *latd = new TDirectory("Latency","Latency","");
	  latd->cd();
	  fLatencyHisto[i]->getLatencyNumberOfCluster()->Write();
	  fLatencyHisto[i]->getLatencyClusterSignal()->Write();
	  latd->Write();
	}

      

      det->Write();
      
    }
  cout <<" closing TBMon " << endl;
  fout->Close();

  cout <<" clearing " << endl;
  /*
  if (header_->GetTriggerType() == TBEventHeader::PEDESTAL)
    this->toFile(header_->GetRun(),header_->GetTriggerType());
  */
#undef DELETEHISTO
#ifndef DELETEHISTO
  for (int i=0;i<MAX_DET;i++)
    {
      //cout << " resetting histo " <<i<< endl;
      
      if (fClusterHisto[i]!=NULL) fClusterHisto[i]->reset();
      if (fCalibrationHisto[i]!=NULL) fCalibrationHisto[i]->reset();
      if (fTimingHisto[i]!=NULL) fTimingHisto[i]->reset();
      if (fOptoHisto[i]!=NULL) fOptoHisto[i]->reset();
      if (fLatencyHisto[i]!=NULL) fLatencyHisto[i]->reset();
      if (fDelayHisto[i]!=NULL) fDelayHisto[i]->reset();
      if (fVpspHisto[i]!=NULL) fVpspHisto[i]->reset();
    }
#else
  for (int i=0;i<MAX_DET;i++)
    {
      //cout << "deleting histo " << i << endl;
      
      if (fClusterHisto[i]!=NULL) delete fClusterHisto[i];
      if (fCalibrationHisto[i]!=NULL) delete fCalibrationHisto[i];
      if (fTimingHisto[i]!=NULL) delete fTimingHisto[i];
      if (fOptoHisto[i]!=NULL) delete fOptoHisto[i];
      if (fLatencyHisto[i]!=NULL) delete fLatencyHisto[i];
      if (fDelayHisto[i]!=NULL) delete fDelayHisto[i];
      if (fVpspHisto[i]!=NULL) delete fVpspHisto[i];

      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
      fVpspHisto[i]=NULL;

    }
#endif
  if (fLatencyAll!=NULL) delete fLatencyAll; fLatencyAll=NULL;
  if (fClusterAll!=NULL) delete fClusterAll; fClusterAll=NULL;

  fHistoList->clear();
  fHistoNames->clear();
}

bool TBMonitor::move2Directory(char* name)
{
  TIter next(gDirectory->GetListOfKeys());
  bool found;
  while (TKey *obj = (TKey*)next()) 
    {
      TObject* o = obj->ReadObj();
      // if it's a directory, enter
      if (strcmp(o->IsA()->GetName(),"TDirectory")==0) {
	if (strcmp(o->GetName(),name)==0) 
	  {
	    gDirectory->cd(name);
	    found = true;
	    return true;
	  }
	    }
    }
  if (!found)
    {
       TDirectory *ndir = new TDirectory(name,name,"");
       ndir->cd();
       return true;
    }

}


void TBMonitor::saveCMSSWHistos()
{

  // Exit if no detectors defined
  if (fListOfDet == NULL) return;
  if (fListOfDet->GetEntries()<=0) return;
  cout << " Jentre dans deleteHistos " << endl;
  cout << fListOfDet->GetEntries() << " Are found " << endl;
  Char_t fnout[256];
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    sprintf(fnout,"/tmp/CWMonitor%.7d.root",header_->GetRun());
  else
    sprintf(fnout,"%s/CWMonitor%.7d.root",fscr,header_->GetRun());
  TFile* fout = new TFile(fnout,"RECREATE");
  fout->SetCompressionLevel(0);
  fout->cd();
  Char_t name[256];
  cout << " Writing histograms " <<__LINE__ << endl;

  // Writing summary histograms
  memset(name,0,256);sprintf(name,"Summary");
  TDirectory *det = new TDirectory(name,name,"");
  det->cd();
  if (fLatencyAll!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY)  )
    {
      TDirectory *latd = new TDirectory("Latency","Latency","");
      latd->cd();
      fLatencyAll->getLatencyNumberOfCluster()->Write();
      fLatencyAll->getLatencyClusterSignal()->Write();
      latd->Write();
    }
   det->cd();
   if (fClusterAll!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::PHYSIC ||GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY  ) )
	{
	  printf("\t cluster ");
	  TDirectory *clud = new TDirectory("Cluster","Cluster","");
	  clud->cd();

	  fClusterAll->getNumberOfStrip()->Write();
	  fClusterAll->getClusterNoise()->Write();
	  fClusterAll->getClusterSignal()->Write();
	  fClusterAll->getClusterSignalHigh()->Write();
	  fClusterAll->getClusterPosition()->Write();
	  fClusterAll->getSeedNoise()->Write();
	  fClusterAll->getSeedSignal()->Write();
	  fClusterAll->getSeedPosition()->Write();
	  printf("Done \n");
	  clud->Write();
	}
  cout << " Looping on detectors " <<__LINE__ << endl;

  fout->cd();
  memset(name,0,256);sprintf(name,"DQMData");
  TDirectory *dqmd = new TDirectory(name,name,"");
  dqmd->cd();
  memset(name,0,256);sprintf(name,"SiStrip");
  TDirectory *sistripd = new TDirectory(name,name,"");
  sistripd->cd();
  memset(name,0,256);sprintf(name,"ControlView");
  TDirectory *controlviewd = new TDirectory(name,name,"");
  controlviewd->cd();
  char feccraten[256],fecslotn[256],fecringn[256],ccuaddrn[256],ccuchann[256];
 for(int i=0;i<fListOfDet->GetEntries();i++)
    {
	TBGeo* g = (TBGeo*) fListOfDet->At(i);
	controlviewd->cd();
	memset(feccraten,0,256);
	memset(fecslotn,0,256);
	memset(fecringn,0,256);
	memset(ccuaddrn,0,256);
	memset(ccuchann,0,256);
	sprintf(feccraten,"FecCrate%d",g->GetFecCrate());
	sprintf(fecslotn,"FecSlot%d",g->GetFecSlot());
	sprintf(fecringn,"FecRing%d",g->GetFecRing());
	sprintf(ccuaddrn,"CcuAddr%d",g->GetFecCcu());
	sprintf(ccuchann,"CcuChan%d",g->GetFecChannel());
	printf("Writing %s/%s/%s/%s/%s \n",feccraten,fecslotn,fecringn,ccuaddrn,ccuchann);
	if (!move2Directory(feccraten)) continue;
	if (!move2Directory(fecslotn)) continue;
	if (!move2Directory(fecringn)) continue;
	if (!move2Directory(ccuaddrn)) continue;
	if (!move2Directory(ccuchann)) continue;
	if ((GetEventHeader()->GetTriggerType() == TBEventHeader::PEDESTAL) )  
	  {
	    printf("\t Pedestal ");
	    
	    TBPed* p= (TBPed*) fListOfPedestal->At(i);
	    TBPed* n= (TBPed*) fListOfNoise->At(i);
	    p->GetProfile()->Write();
	    n->GetProfile()->Write();
	    p->GetHistoRMS()->Write();
	    n->GetHistoRMS()->Write();
	    n->GetCommonModeNoise()->Write();
	    gDirectory->Write();
	    printf("Done \n");
	  }


      if (fCalibrationHisto[i]!=NULL &&  (GetEventHeader()->GetTriggerType() == TBEventHeader::CALIBRATION ||GetEventHeader()->GetTriggerType() == 33  ) )  
	
	{

	  cout << __LINE__ << "Calibration " << endl;
	 
	  
	  for (int j=0;j<768;j++) 
	    if (fCalibrationHisto[i]!=NULL)
	      {
		if (fCalibrationHisto[i]->getCalibrationStrip(j)!=NULL)
		  {
		    
		    fCalibrationHisto[i]->getCalibrationStrip(j)->Write();
		    
		
		  }
	      }
	  gDirectory->Write();
	  printf("Done \n");
	}
      if (fClusterHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::PHYSIC ||GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY  ) )
	{
	  printf("\t cluster ");
	  fClusterHisto[i]->getNumberOfStrip()->Write();
	  fClusterHisto[i]->getClusterNoise()->Write();
	  fClusterHisto[i]->getClusterSignal()->Write();
	  fClusterHisto[i]->getClusterSignalHigh()->Write();
	  fClusterHisto[i]->getClusterPosition()->Write();
	  fClusterHisto[i]->getSeedNoise()->Write();
	  fClusterHisto[i]->getSeedSignal()->Write();
	  fClusterHisto[i]->getSeedPosition()->Write();
	  printf("Done \n");
	  gDirectory->Write();
	}

      if (fTimingHisto[i]!=NULL && ((GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING) || (GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING_FED))  )
	{
	  printf("\t timing ");
	  
	  for (int j=0;j<3;j++)
	    if (fTimingHisto[i]->getTimingTick(j)!=NULL)
	      fTimingHisto[i]->getTimingTick(j)->Write();
	  for (int j=0;j<3;j++)
	    if (fTimingHisto[i]->getTimingTickFed(j)!=NULL)
	      fTimingHisto[i]->getTimingTickFed(j)->Write();

	  gDirectory->Write();
	  printf("Done \n");
	}

      if (fDelayHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::DELAY || GetEventHeader()->GetTriggerType() == TBEventHeader::DELAY_TTC  )  )
	{
	  printf("\t Delay ");
	  fDelayHisto[i]->getDelayClusterSignal()->Write();
	  fDelayHisto[i]->getDelayNumberOfCluster()->Write();

	  gDirectory->Write();
	  printf("Done \n");
	}

      if (fOptoHisto[i]!=NULL && (GetEventHeader()->GetTriggerType() == TBEventHeader::GAINSCAN)  )
	{
	  printf("\t Opto ");
	  for (int g=0;g<4;g++)
	    for (int j=0;j<3;j++)
	      {
		//		printf("hsit %d %d \n",g,j);
		if (fOptoHisto[i]->getOptoTick(j,g)!=NULL)
		  fOptoHisto[i]->getOptoTick(j,g)->Write();
		//printf("Base hsit %d %d \n",g,j);
		if (fOptoHisto[i]->getOptoBase(j,g)!=NULL)
		  fOptoHisto[i]->getOptoBase(j,g)->Write();
		if (fOptoHisto[i]->getOptoDiff(j,g)!=NULL)
		  fOptoHisto[i]->getOptoDiff(j,g)->Write();
	      }
	  gDirectory->Write();
	  printf("Done \n");
	}


      if (fVpspHisto[i]!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::VPSPSCAN)  )
	{
	  printf("\t Vpsp ");

	  for (int j=0;j<6;j++)
	    if (fVpspHisto[i]->getMeanMean(j)!=NULL)
	      {
#ifdef VPSPPED
		fVpspHisto[i]->getMeanNoise(j)->Write();
#endif
		fVpspHisto[i]->getMeanMean(j)->Write();
	      }
	  gDirectory->Write();
	  printf("Done \n");

	}

     


      if (fLatencyHisto[i]!=NULL&& (GetEventHeader()->GetTriggerType() == TBEventHeader::LATENCY)  )
	{
	  printf("\t Latency ");
	  fLatencyHisto[i]->getLatencyNumberOfCluster()->Write();
	  fLatencyHisto[i]->getLatencyClusterSignal()->Write();
	  gDirectory->Write();
	  printf("Done \n");
	}


    }
  cout <<" closing TBMon " << endl;
  fout->Close();

  cout <<" clearing " << endl;
  /*
  if (header_->GetTriggerType() == TBEventHeader::PEDESTAL)
    this->toFile(header_->GetRun(),header_->GetTriggerType());
  */
#undef DELETEHISTO
#ifndef DELETEHISTO
  for (int i=0;i<MAX_DET;i++)
    {
      cout << " resetting histo " <<i<< endl;
      
      if (fClusterHisto[i]!=NULL) fClusterHisto[i]->reset();
      if (fCalibrationHisto[i]!=NULL) fCalibrationHisto[i]->reset();
      if (fTimingHisto[i]!=NULL) fTimingHisto[i]->reset();
      if (fOptoHisto[i]!=NULL) fOptoHisto[i]->reset();
      if (fLatencyHisto[i]!=NULL) fLatencyHisto[i]->reset();
      if (fDelayHisto[i]!=NULL) fDelayHisto[i]->reset();
      if (fVpspHisto[i]!=NULL) fVpspHisto[i]->reset();
    }
#else
  for (int i=0;i<MAX_DET;i++)
    {
      //cout << "deleting histo " << i << endl;
      
      if (fClusterHisto[i]!=NULL) delete fClusterHisto[i];
      if (fCalibrationHisto[i]!=NULL) delete fCalibrationHisto[i];
      if (fTimingHisto[i]!=NULL) delete fTimingHisto[i];
      if (fOptoHisto[i]!=NULL) delete fOptoHisto[i];
      if (fLatencyHisto[i]!=NULL) delete fLatencyHisto[i];
      if (fDelayHisto[i]!=NULL) delete fDelayHisto[i];
      if (fVpspHisto[i]!=NULL) delete fVpspHisto[i];

      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
      fVpspHisto[i]=NULL;

    }
#endif
  if (fLatencyAll!=NULL) delete fLatencyAll; fLatencyAll=NULL;
  if (fClusterAll!=NULL) delete fClusterAll; fClusterAll=NULL;

  fHistoList->clear();
  fHistoNames->clear();
}

void TBMonitor::updateMaps()
{
  cout << " Entering updateMaps()"<< endl;
  Char_t hname[256];
  fHistoList->clear();
  fHistoNames->clear();
  Int_t key=0;
  for(int i=0;i<fListOfDet->GetEntries();i++)
    { 
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      TBRaw* r= (TBRaw*) fListOfRaw->At(i); 
      for (int j=0;j<g->GetNChips();j++)
	{
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|Raw data|Chip%.2d",i,j);
	  TH1* h = r->GetHisto(j);
	  fHistoList->insert(pairTH1(key,h));
	  fHistoNames->insert(pairString(hname,key));
	  key++;
	}
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|Raw data|All",i);
      TH1* h = r->GetHisto();
      fHistoList->insert(pairTH1(key,h));
      fHistoNames->insert(pairString(hname,key));
      key++;


      TBPed* p = (TBPed*) fListOfPedestal->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|Pedestal|Position",i);
      TH1* hpp  = p->GetProfile(); 
      fHistoList->insert(pairTH1(key,hpp));
      fHistoNames->insert(pairString(hname,key));
      key++;
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|Pedestal|Noise",i);
      TH1* hpn  = p->GetHistoRMS(); 
      fHistoList->insert(pairTH1(key,hpn));
      fHistoNames->insert(pairString(hname,key));
      key++;

      TBPed* n = (TBPed*) fListOfNoise->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|PedestalAfterCmn|Position",i);
      TH1* hnp  = n->GetProfile(); 
      fHistoList->insert(pairTH1(key,hnp));
      fHistoNames->insert(pairString(hname,key));
      key++;
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|PedestalAfterCmn|Noise",i);
      TH1* hnn  = n->GetHistoRMS(); 
      fHistoList->insert(pairTH1(key,hnn));
      fHistoNames->insert(pairString(hname,key));
      key++;

      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|PedestalAfterCmn|CommonModeNoise",i);
      TH1* hcmn  = n->GetCommonModeNoise(); 
      fHistoList->insert(pairTH1(key,hcmn));
      fHistoNames->insert(pairString(hname,key));
      key++;

      TBCor* c = (TBCor*) fListOfCor->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.4d|Corrected|Raw",i);
      TH1* hc  = c->GetHisto(); 
      fHistoList->insert(pairTH1(key,hc));
      fHistoNames->insert(pairString(hname,key));
      key++;

      if (fCalibrationHisto[i]!=NULL)
	{
	  for (int j=0;j<768;j++)
	    {
	      TH1* hs = fCalibrationHisto[i]->getCalibrationStrip(j);
	      if (hs!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|Calibration|Strip%.3d",i,j);
		  fHistoList->insert(pairTH1(key,hs));
		  fHistoNames->insert(pairString(hname,key));
		  key++;

		}
	    }
	}
      if (fTimingHisto[i]!=NULL)
	{
	  for (int j=0;j<3;j++)
	    {
	      TH1* hs = fTimingHisto[i]->getTimingTick(j);
	      if (hs!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|Timing|Tick%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hs));
		  fHistoNames->insert(pairString(hname,key));
		  key++;

		}

	      TH1* hsf = fTimingHisto[i]->getTimingTickFed(j);
	      if (hsf!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|TimingFed|Tick%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hsf));
		  fHistoNames->insert(pairString(hname,key));
		  key++;

		}

	    }
	}
      if (fOptoHisto[i]!=NULL)
	{
	  for (int g=0;g<4;g++)
	    for (int j=0;j<3;j++)
	      {
		TH1* hs = fOptoHisto[i]->getOptoTick(j,g);
		if (hs!=NULL)
		  {
		    memset(hname,0,256);
		    sprintf(hname,"|BeamTest|Det%.4d|Opto|Gain%.1d|Tick%.1d",i,g,j);
		    fHistoList->insert(pairTH1(key,hs));
		    fHistoNames->insert(pairString(hname,key));
		    key++;

		  }
		TH1* hb = fOptoHisto[i]->getOptoBase(j,g);
		if (hb!=NULL)
		  {
		    memset(hname,0,256);
		    sprintf(hname,"|BeamTest|Det%.4d|Opto|Gain%.1d|Base%.1d",i,g,j);
		    fHistoList->insert(pairTH1(key,hb));
		    fHistoNames->insert(pairString(hname,key));
		    key++;

		  }
		TH1* hd = fOptoHisto[i]->getOptoDiff(j,g);
		if (hd!=NULL)
		  {
		    memset(hname,0,256);
		    sprintf(hname,"|BeamTest|Det%.4d|Opto|Gain%.1d|Diff%.1d",i,g,j);
		    fHistoList->insert(pairTH1(key,hd));
		    fHistoNames->insert(pairString(hname,key));
		    key++;

		  }
	    }
	}

      if (fLatencyHisto[i]!=NULL)
	{

	      TH1* hn = fLatencyHisto[i]->getLatencyNumberOfCluster();

	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Latency|NClus",i);
	      fHistoList->insert(pairTH1(key,hn));
	      fHistoNames->insert(pairString(hname,key));
	      key++;


	      TH1* hs = fLatencyHisto[i]->getLatencyClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Latency|Signal",i);
	      fHistoList->insert(pairTH1(key,hs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	}


      if (fClusterHisto[i]!=NULL)
	{
	      TH1* hns = fClusterHisto[i]->getNumberOfStrip();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|Nstrip",i);
	      fHistoList->insert(pairTH1(key,hns));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcs = fClusterHisto[i]->getClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|Signal",i);
	      fHistoList->insert(pairTH1(key,hcs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcsh = fClusterHisto[i]->getClusterSignalHigh();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|HighestSignal",i);
	      fHistoList->insert(pairTH1(key,hcsh));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcp = fClusterHisto[i]->getClusterPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|Position",i);
	      fHistoList->insert(pairTH1(key,hcp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	      TH1* hss = fClusterHisto[i]->getSeedSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|SeedSignal",i);
	      fHistoList->insert(pairTH1(key,hss));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hsp = fClusterHisto[i]->getSeedPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Cluster|SeedPosition",i);
	      fHistoList->insert(pairTH1(key,hsp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	}

      if (fDelayHisto[i]!=NULL)
	{


	      TH1* hn = fDelayHisto[i]->getDelayNumberOfCluster();

	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Delay|NClus",i);
	      fHistoList->insert(pairTH1(key,hn));
	      fHistoNames->insert(pairString(hname,key));
	      key++;



	      TH1* hs = fDelayHisto[i]->getDelayClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.4d|Delay|Signal",i);
	      fHistoList->insert(pairTH1(key,hs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	}

      if (fVpspHisto[i]!=NULL)
	{

	  for (int j=0;j<6;j++)
	    {
	      
#ifdef VPSPPED
	      TH1* hn = fVpspHisto[i]->getMeanNoise(j);
	      if (hn!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|VPSP|Noise|APV%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hn));
		  fHistoNames->insert(pairString(hname,key));
		  key++;
		}
#endif

	      TH1* hm = fVpspHisto[i]->getMeanMean(j);
	      if (hm!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|VPSP|Mean|APV%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hm));
		  fHistoNames->insert(pairString(hname,key));
		  key++;
		}


	    }
	}




            if (fDCUHisto[i]!=NULL)
	{
	  for (int j=0;j<8;j++)
	    {
	      TH1* hs = fDCUHisto[i]->getDcuChannel(j);
	      if (hs!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|DCU|Channel%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hs));
		  fHistoNames->insert(pairString(hname,key));
		  key++;
		  
		}
	    }

	  TH1* hs = fDCUHisto[i]->getWaferTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureWafer",i);
	  fHistoList->insert(pairTH1(key,hs));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs1 = fDCUHisto[i]->getHybridTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureHybrid",i);
	  fHistoList->insert(pairTH1(key,hs1));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs2 = fDCUHisto[i]->getDcuTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureDcu",i);
	  fHistoList->insert(pairTH1(key,hs2));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs3 = fDCUHisto[i]->get250Bias();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|Bias250",i);
	  fHistoList->insert(pairTH1(key,hs3));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs4 = fDCUHisto[i]->get125Bias();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|Bias125",i);
	  fHistoList->insert(pairTH1(key,hs4));
	  fHistoNames->insert(pairString(hname,key));
	  key++;


	  TH1* hs5 = fDCUHisto[i]->getWaferCurrent();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|BiasCurrent",i);
	  fHistoList->insert(pairTH1(key,hs5));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	}





    }

  // Summary
      if (fLatencyAll!=NULL)
	{

	      TH1* hn = fLatencyAll->getLatencyNumberOfCluster();

	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Latency|NClus");
	      fHistoList->insert(pairTH1(key,hn));
	      fHistoNames->insert(pairString(hname,key));
	      key++;


	      TH1* hs = fLatencyAll->getLatencyClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Latency|Signal");
	      fHistoList->insert(pairTH1(key,hs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	}

if (fClusterAll!=NULL)
	{
	      TH1* hns = fClusterAll->getNumberOfStrip();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|Nstrip");
	      fHistoList->insert(pairTH1(key,hns));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcs = fClusterAll->getClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|Signal");
	      fHistoList->insert(pairTH1(key,hcs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcsh = fClusterAll->getClusterSignalHigh();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|HighestSignal");
	      fHistoList->insert(pairTH1(key,hcsh));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcp = fClusterAll->getClusterPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|Position");
	      fHistoList->insert(pairTH1(key,hcp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	      TH1* hss = fClusterAll->getSeedSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|SeedSignal");
	      fHistoList->insert(pairTH1(key,hss));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hsp = fClusterAll->getSeedPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Summary|Cluster|SeedPosition");
	      fHistoList->insert(pairTH1(key,hsp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	}


}
void TBMonitor::initHistos()
{
  initHistos(0);
}
void TBMonitor::initDCUHistos()
{

  Int_t key=1000000;
  Char_t hname[256];
  fHistoOut = new TFile("/tmp/dcuh","RECREATE");
  fHistoOut->cd();
  for(int i=0;i<fListOfDet->GetEntries();i++)
    { 
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      if (fDCUHisto[i]==NULL)
	fDCUHisto[i] = new DCUHisto(g);
      else
	fDCUHisto[i]->reset();


      if (fDCUHisto[i]!=NULL)
	{
	  cout << "Initializing Histo " << i << endl;
	  for (int j=0;j<8;j++)
	    {
	      TH1* hs = fDCUHisto[i]->getDcuChannel(j);
	      if (hs!=NULL)
		{
		  memset(hname,0,256);
		  sprintf(hname,"|BeamTest|Det%.4d|DCU|Channel%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hs));
		  fHistoNames->insert(pairString(hname,key));
		  key++;
		  
		}
	    }

	  TH1* hs = fDCUHisto[i]->getWaferTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureWafer",i);
	  fHistoList->insert(pairTH1(key,hs));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs1 = fDCUHisto[i]->getHybridTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureHybrid",i);
	  fHistoList->insert(pairTH1(key,hs1));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs2 = fDCUHisto[i]->getDcuTemperature();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|TemperatureDcu",i);
	  fHistoList->insert(pairTH1(key,hs2));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs3 = fDCUHisto[i]->get250Bias();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|Bias250",i);
	  fHistoList->insert(pairTH1(key,hs3));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	  TH1* hs4 = fDCUHisto[i]->get125Bias();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|Bias125",i);
	  fHistoList->insert(pairTH1(key,hs4));
	  fHistoNames->insert(pairString(hname,key));
	  key++;


	  TH1* hs5 = fDCUHisto[i]->getWaferCurrent();
	  memset(hname,0,256);
	  sprintf(hname,"|BeamTest|Det%.4d|DCU|BiasCurrent",i);
	  fHistoList->insert(pairTH1(key,hs5));
	  fHistoNames->insert(pairString(hname,key));
	  key++;

	}




    }
}
#define FED9U
void TBMonitor::initHistos(int ir)
{


  //  TFile* fHistoOut = new TFile("tot","RECREATE");
  fHistoOut = new TFile("tot","RECREATE");
  fHistoOut->cd();
  return;
  for(int i=0;i<fListOfDet->GetEntries();i++)
    { 
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      printf("Initialize histos for detector %d adrr %x \n",i,g->GetIdLocal());
      //      printf("cluster \n");
      if (fClusterHisto[i]==NULL    )
	fClusterHisto[i] = new TBClusterHisto(g);
//#ifndef FED9U 
      //  printf("calibration \n");
      if (useCalib_  )
	{
	  if (fCalibrationHisto[i]==NULL)
	    fCalibrationHisto[i] = new TBCalibrationHisto(g);
	}
//#endif
      // printf("timing \n");
      if (fTimingHisto[i]==NULL  )
	fTimingHisto[i] = new TBTimingHisto(g);
      // printf("opto \n");
      if (fOptoHisto[i]==NULL  )
	fOptoHisto[i] = new TBOptoHisto(g);
      //  printf("latency \n");
      if (fLatencyHisto[i]==NULL )
	fLatencyHisto[i] = new TBLatencyHisto(g);
      // printf("delay \n");
      if (fDelayHisto[i]==NULL  )
	      fDelayHisto[i] = new TBDelayHisto(g);
      // printf("vpsp \n");
      if (fVpspHisto[i]==NULL)
	      fVpspHisto[i] = new TBVpspHisto(g);
    }
  
  updateMaps();
}


void TBMonitor::toFile(int nrun,int type)
{
  /* Open Cluster Output file */
  printf("Enter %d %d \n",nrun,type);
  if (type==3)
    {
      Char_t fnout[256];
      if (nrun!=0)
	//Destination directory changed by BT on 1/12/02
	{
	  char* fscr=getenv("SCRATCH");
	  if (fscr==NULL)
	    sprintf(fnout,"/tmp/daqcalib%.7d.root",nrun);
	  else
	    sprintf(fnout,"%s/daqcalib%.7d.root",fscr,nrun);

	}
      else
	sprintf(fnout,"/tmp/daqdialog.root");
      TFile* fout = new TFile(fnout,"RECREATE");
      fout->cd();
      printf("I have create %s \n",fnout);
	 
      /* Filling */
      
      for(int i=0;i<fListOfDet->GetEntriesFast();i++)
	{
	  for (int j=0;j<768;j++)
	    if (fCalibrationHisto[i]->getCalibrationStrip(j)!=NULL)
	      {

	      fCalibrationHisto[i]->getCalibrationStrip(j)->Write();

	      }
	}
      fout->Close();
    }
  if (type==2)
    {
      Char_t fnout[256];

      if (nrun!=0)
	//Destination directory changed by BT on 1/12/02
	{
	  char* fscr=getenv("SCRATCH");
	  if (fscr==NULL)
	    sprintf(fnout,"/tmp/daqped%.7d.root",nrun);
	  else
	    sprintf(fnout,"%s/daqped%.7d.root",fscr,nrun);

	}
      else
	sprintf(fnout,"/tmp/daqdialog.root");





      TFile* fout = new TFile(fnout,"RECREATE");
      fout->cd();
      /* Filling */
      
      for(int i=0;i<fListOfPedestal->GetEntriesFast();i++)
	{

	  TBPed* p= (TBPed*)fListOfPedestal->At(i);
	  TBPed* n= (TBPed*)fListOfNoise->At(i);
	  p->Write();
	  n->Write();
	}
      fout->Close();
    }

}

void TBMonitor::savePedestal(const char* fnout)
{
  // RB (02/07/04)
  for (int i=0;i<fListOfPedestal->GetEntriesFast();i++) {
    
    TBPed* p = (TBPed*)fListOfPedestal->At(i);
    TProfile* h = p->GetProfile();
    int bin = h->GetNbinsX() / 2;
    double bak=h->GetBinContent(bin); 
    double val = bak - 120.; // subtract ~1.5 MIP
   // h->SetBinContent(bin,val);
    cout<<"doctoring pedestals: "<<i<<"  "<<bak<<endl;
    cout<<"doctoring pedestals: "<<i<<"  "<<h->GetBinContent(bin)<<endl;
  } 
  printf("Saving pedestal\n");
  TFile* f = new TFile(fnout,"RECREATE");
  f->cd();
  fListOfPedestal->Write();
  fListOfNoise->Write();
  f->Close();
  return;
}

int TBMonitor::loadPedestal(const char *fnin)
{
  //  clearPedestal();
  fListOfPedestal->Delete();
  fListOfNoise->Delete();
  TFile* f = new TFile(fnin);
  printf("Pedestal file %s is zombie ? %d\n",fnin,f->IsZombie());
  if (f->IsZombie()) return -1;
  f->cd();
  int j;char name[256];
  cout <<"Number of Det found " << fListOfDet->GetEntries()<<endl;
  TObjArray* allp = new TObjArray(fListOfDet->GetEntries());
  TBPed* ped=NULL;
  for (j=0;j<fListOfDet->GetEntries();j++)
    {
      f->cd();
      TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
      memset(name,0,256);
      sprintf(name,"ped-%x;1",g->GetIdLocal());
      //      cout << "Det " << j <<" "<< name << endl;

      ped=NULL;

      ped = (TBPed*) f->Get(name);
      //    cout << "Ptr " << ped << endl;
      if (ped!=NULL)
	allp->AddLast(ped);
   }
  TObjArray* alln = new TObjArray(fListOfDet->GetEntries());
  for (j=0;j<fListOfDet->GetEntries();j++)
    {
      f->cd();
      TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
      memset(name,0,256);
      sprintf(name,"noi-%x",g->GetIdLocal());
      //      cout << "Det " << j <<" "<< name << endl;
      ped=NULL;
      ped = (TBPed*) f->Get(name);
      //  cout << "Ptr " << ped << endl;
      if (ped!=NULL)
	alln->AddLast(ped);
   }
  SetPedestal(allp);
  FlagDeadPedestal(allp);
  SetNoise(alln);
  FlagDeadNoise(alln);
  printf("The pedestals are loaded and the monitor is updated\n");
  //  updateMaps();
  printf("The monitoring maps are updated\n");
  return 0;

}

void TBMonitor::clearPedestal()
{

  for(int i=0;i<fListOfDet->GetEntriesFast();i++)
    {

      TBPed* p =  (TBPed*)  fListOfPedestal->At(i);
      TBPed* n =  (TBPed*)  fListOfNoise->At(i);
      p->Clear();
      n->Clear();
    }

}
static TBRaw* R[MAX_DET];
static TBRU* RU[MAX_RU];
void TBMonitor::initTree(Int_t nrun)
{
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    {
      this->initTree("/tmp",nrun,0);
       }
  else
    {
      this->initTree(fscr,nrun,0);
     }
}

void TBMonitor::initTree(Char_t* fdir,Int_t nrun,Int_t fseq)
{
  if (fTOut!=NULL) return;
  Int_t ier,i,j,k,l,nentries;
Char_t name[256];
sprintf(name,"%s/sir%.7d_%.3d.root",fdir,nrun,fseq);
fTOut = new TFile(name,"RECREATE","RAW DATA");
 fTOut->SetCompressionLevel(0);
fT = new TTree("TB","Root Tree to Store Raw Data");
 fTOut->SetCompressionLevel(1);
fT->SetAutoSave(140000000); 
Int_t bufsize = 220000;
Int_t split=0;

 if (fListOfDet!=NULL)
   for (j=0;j<fListOfDet->GetEntriesFast();j++)
     {
       TBGeo* g = (TBGeo*) fListOfDet->At(j);
       R[j] = (TBRaw*) fListOfRaw->At(j);
       sprintf(name,"raw_%x",g->GetIdLocal());
       TBranch* branch=fT->Branch(name, "TBRaw", &R[j], bufsize,split);
       branch->SetAutoDelete(kFALSE);
     }
#ifdef SAMEFILE
 if (fListOfRU!=NULL)
   {
     for (j=0;j<fListOfRU->GetEntries();j++)
       {
	 RU[j] = (TBRU*) fListOfRU->At(j);
// 	 sprintf(name,"RU_%x",RU[j]->getInstance());
// 	 TBranch* branch=fT->Branch(name, "TBRU", &RU[j], bufsize,split);
// 	 branch->SetAutoDelete(kFALSE);
         char sizename[256];
         char arrayname[256];
         char sizetype[256];
         char arraytype[256];
         sprintf(sizename,"size_RU_%x",RU[j]->getInstance());
         sprintf(sizetype,"size_RU_%x/I",RU[j]->getInstance());

         fT->Branch(sizename, &RU[j]->fSize,sizetype);
	 printf("====> Branch %s %s %d\n",sizename,sizetype,RU[j]->getInstance());

         sprintf(arraytype,"RU_%x[size_RU_%x]/I",RU[j]->getInstance(),RU[j]->getInstance());
         sprintf(arrayname,"RU_%x",RU[j]->getInstance());

         fT->Branch(arrayname, RU[j]->fBuffer,arraytype);
	 printf("====> Branch %s %s %d\n",arrayname,arraytype,RU[j]->getInstance());



       }
   }
#endif
if (fT == NULL || fTOut == NULL) return;

  fT->Branch("Header","TBEventHeader",&header_,bufsize,split);

  fT->Print();

  

}

void TBMonitor::fillTree()
{
  Int_t ier,i,j,k,l,nentries;Char_t name[256];
  fTOut->cd();
  fT->Fill();

}
void TBMonitor::Close()
{
  if (fTOut == NULL) return;
  fTOut->cd();
  fT->Write();
  fTOut->Close();

  printf("Now I stop!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  fTOut=NULL;


  fT=NULL;

}
void TBMonitor::initMap(const Char_t* cm)
{


  int siz= fListOfDet->GetEntriesFast()*5000000;
  fmfile = TMapFile::Create(cm,"RECREATE", siz,"Test");
  printf("New Map file %x \n",fmfile);
  Char_t name[256];
  for (Int_t j=0;j<fListOfDet->GetEntriesFast();j++)
    {

      // Pedestal
      TBPed* p = (TBPed*) fListOfPedestal->At(j);

      fmfile->Add(p,p->GetName());

      // Noise
      TBPed* n = (TBPed*) fListOfNoise->At(j);

      fmfile->Add(n,n->GetName());


      // Clusters
      TBClusterHisto* clh= fClusterHisto[j];

      TH1S* hnbs = clh->getNumberOfStrip();
      fmfile->Add(hnbs,hnbs->GetName());
      TH1D* hclnoi=clh->getClusterNoise();
      fmfile->Add(hclnoi,hclnoi->GetName());
      TH1D* hclsig= clh->getClusterSignal();
      fmfile->Add(hclsig,hclsig->GetName());
      TH1D* hclsigh= clh->getClusterSignalHigh();
      fmfile->Add(hclsigh,hclsigh->GetName());
      TH1D* hclpos= clh->getClusterPosition();
      fmfile->Add(hclpos,hclpos->GetName());

      TH1D* hseednoi=clh->getSeedNoise();
      fmfile->Add(hseednoi,hseednoi->GetName());
      TH1D* hseedsig= clh->getSeedSignal();
      fmfile->Add(hseedsig,hseedsig->GetName());
      TH1D* hseedpos= clh->getSeedPosition();
      fmfile->Add(hseedpos,hseedpos->GetName());


      // Calibration
      TBCalibrationHisto* calh= fCalibrationHisto[j];

      int  totsiz=0;
      
      for (Int_t is=0;is<768;is++)
	{
	  TProfile* ps = calh->getCalibrationStrip(is);
	  if (ps!=NULL)
	    {
	      fmfile->Add(ps,ps->GetName());
	    }
	}
    }

}


void TBMonitor::updateMap()
{
  //  return;
  fmfile->cd();
  fmfile->Update();

  //  fmfile->ls();
}
void TBMonitor::postAnalysis()
{

  FILE* fdebug=fopen("/tmp/debug","w");

 
 char name[255];
  if (GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING || GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING_FED)
    {
      float tcor = (GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING)?25./24:1.;
      for (Int_t i=0;i<fListOfDet->GetEntriesFast();i++)
	{
	  
	  TBGeo* g =  (TBGeo*)  fListOfDet->At(i);
	  memset(name,0,255);
	  sprintf(name,"Local-%x",g->GetIdLocal());
	  if (fTimingHisto[i]!=NULL)
	    {
	      for (int j=0;j<3;j++)
		{fTickDelay[j][i] = -1;
		if (fTimingHisto[i]->getTimingTick(j)!=NULL)
		  {
		    TH1D* h = (TH1D*) fTimingHisto[i]->getTimingTick(j);
		    if (tcor==1.)
		      h= (TH1D*) fTimingHisto[i]->getTimingTickFed(j);
		    float maxdev=-9999;
		    float mindev=9999;
		    int ideriv;
		    int idevmin;
		    for (int is=10;is<h->GetNbinsX()-10;is++)
		      {
			float deriv = (h->GetBinContent(is+1)-h->GetBinContent(is-1));
			if (deriv>maxdev)
			  {
			    maxdev=deriv;
			    ideriv=is;
			  }
			if (deriv<mindev)
			  {
			    mindev=deriv;
			    idevmin=is;
			  }


		      }

		    if (maxdev<10.) continue;
		    fTickDelay[j][i] = ideriv*tcor;
		    fTickBase[j][i] = h->GetBinContent(ideriv-10);
		    fTickTop[j][i] = h->GetBinContent(ideriv+10);
		    int goodsample=int( rint(ideriv*tcor+15.))%25;
		    int goodsample2=int( rint(idevmin*tcor-35.))%25;
		    printf("Detector %d %s tick %d max %f %f %f %d falling %f second %d \n",i,name,j,ideriv*tcor,maxdev,ideriv*tcor+15.,goodsample,idevmin*tcor,goodsample2);
		  }
		}
	    }
	}
    }
  if (GetEventHeader()->GetTriggerType() == TBEventHeader::GAINSCAN)
    {
      for (int gain=0;gain<4;gain++)
	{

	  for (Int_t i=0;i<fListOfDet->GetEntriesFast();i++)
	    {
	      TBGeo* g =  (TBGeo*)  fListOfDet->At(i);
	      memset(name,0,255);
	      sprintf(name,"Local-%x",g->GetIdLocal());
	      if (fOptoHisto[i]!=NULL)
		{
		  for (int j=0;j<3;j++)
		    {

		      if (fOptoHisto[i]->getOptoDiff(j,gain)!=NULL)
			{
#ifdef OLDGAINANALYSIS
			  TH1D* h= (TH1D*) fOptoHisto[i]->getOptoDiff(j,gain);
			  TH1D* hb= (TH1D*) fOptoHisto[i]->getOptoBase(j,gain);
			  TH1D* ht= (TH1D*) fOptoHisto[i]->getOptoTick(j,gain);
			  //Look for highest tick height
			  float hiTickH=0.;
			  int optOptoBias=0;
			  for (int k=1;k<63;k++)
			    {
			      if (h->GetBinContent(k)>hiTickH)
				hiTickH=h->GetBinContent(k);
			    }
			  //Compute plateau width(defined as height>0.97*max)
			  //Choose optimal working point at the beginning of
			  //the plateau
			  float basederiv[64];
			  float tickderiv[64];

			  int plateauWidth=0;
			  float maxbasederiv=-1;int ibdm=-1;
			  float maxtickderiv=-1;int itdm=-1;
			  for (int k2=2;k2<63;k2++)
			    {
			      if (h->GetBinContent(k2)>0.97*hiTickH)
				{
				  plateauWidth++;
				  if (plateauWidth<=2) optOptoBias=k2;
				}

			      basederiv[k2]=hb->GetBinContent(k2)-hb->GetBinContent(k2-1);
			      tickderiv[k2]=ht->GetBinContent(k2)-ht->GetBinContent(k2-1);
			      if ((basederiv[k2]>maxbasederiv) &&
			      	(hb->GetBinContent(k2)> 200) &&
				(hb->GetBinContent(k2)<400))
				{
				  maxbasederiv = basederiv[k2];
				  ibdm=k2;
				}
			      if ((tickderiv[k2]>maxtickderiv) &&
			      	(ht->GetBinContent(k2)> 200) &&
				(ht->GetBinContent(k2)<400))
				{
				  maxtickderiv = tickderiv[k2];
				  itdm=k2;
				}
			    }
			    
			    
			    
			    // Construct the line
			    float ab=maxbasederiv;
			    float bb=hb->GetBinContent(ibdm)-ab*ibdm;
			    float b300 = (300.-bb)/ab;
			    //			    cout << i <<":"<<j<<" Base : " <<ab <<" " << bb << " " << b300 << endl;
			    float at=maxtickderiv;
			    float bt=ht->GetBinContent(itdm)-at*itdm;
			    float t300 = (300.-bt)/at;
			    // cout << i <<":"<<j<<" Tick : " <<at <<" " << bt << " " << t300 << endl;			    
			    float tgain= (b300-t300)*at/800.;
			    int ibasestart=-1;
			    for (int k=2;k<63;k++)
			    {
			      if (hb->GetBinContent(k)-hb->GetBinContent(k-1)>0.9*maxbasederiv)
				{ibasestart=k;break;}
			    }
			    
			    printf("Gain %f  start %d \n",tgain,ibasestart);


			    fprintf(fdebug,"%d %s  %d Base %d %f %f Tick %d %f %f Slope Base %f  x+ %f Tick %f x + %f Gain %f Bias %d \n",i,name,j,
				    ibdm ,maxbasederiv,hb->GetBinContent(ibdm),
				    itdm,maxtickderiv,ht->GetBinContent(itdm),
				    ab,bb,at,bt,tgain,ibasestart);

			  int baseval=hb->GetBinContent(optOptoBias);
			  int tickval=ht->GetBinContent(optOptoBias);
			  fOptoBias[gain][j][i]= optOptoBias;
			  fOptoBaseValue[gain][j][i]= baseval;
			  fOptoTickValue[gain][j][i]= tickval;
			  fOptoBaseDeriv[gain][j][i]= basederiv[optOptoBias];
			  fOptoTickDeriv[gain][j][i]= tickderiv[optOptoBias];
			  fOptoGain[gain][j][i] = tgain;
			  fOptoStart[gain][j][i] = ibasestart;
			  
			  printf("Detector %d %s Laser %d Gain %d Bias %d Width %d base val %d  deriv %f Tick base %d deriv %f Tick Height %d\n",i,name,j,gain,optOptoBias,plateauWidth,baseval,basederiv[optOptoBias],tickval,tickderiv[optOptoBias],tickval-baseval);
#else
			  FitOptoTick(gain,i,j);
#endif
			}
		    }
		}
	    }
	  //End of added lines
	}
    }


 if (GetEventHeader()->GetTriggerType() == TBEventHeader::VPSPSCAN)
    {
      memset(fVpsp,0,MAX_DET*6*sizeof(int));
      for (Int_t i=0;i<fListOfDet->GetEntriesFast();i++)
	{
	  TBGeo* g =  (TBGeo*)  fListOfDet->At(i);
	  memset(name,0,255);
	  sprintf(name,"Local-%x",g->GetIdLocal());
	  if (fVpspHisto[i]!=NULL)
		{
		  for (int j=0;j<6;j++) //Loop on APV
		    {
		      
		      if (fVpspHisto[i]->getMeanMean(j)!=NULL)
			{
#ifdef VPSPPED
			  TH1D* h= (TH1D*) fVpspHisto[i]->getMeanNoise(j);
#endif
			  TH1D* hm= (TH1D*) fVpspHisto[i]->getMeanMean(j);
			  //Look for highest tick height

#ifdef NOISEANALYSIS
			  float hiTickH=0.;
			  int optVpsp=0;
			  for (int k=5;k<55;k++)
			    {
			      if (h->GetBinContent(k)>hiTickH)
				hiTickH=h->GetBinContent(k);
			    }
			  //Compute plateau width(defined as height>0.97*max)
			  //Choose optimal working point at the beginning of
			  //the plateau

			  int plateauWidth=0;
			  int startPlateau=5;
			  for (int k2=5;k2<55;k2++)
			    {
			      if (h->GetBinContent(k2)>0.95*hiTickH)
				{
				  if (plateauWidth == 0) startPlateau=k2;
				  plateauWidth++;
				  if ((hm->GetBinContent(startPlateau)-
				      hm->GetBinContent(k2))>255) break;
				  optVpsp=k2;
				}

			    }

			  fVpsp[j][i]= optVpsp;

			  
			  printf("Detector %d %s APV %d : Start Plateau %d %f (N) %f (M) - Optimum %d %f (N) %f (M)\n",i,name,j,startPlateau,h->GetBinContent(startPlateau),hm->GetBinContent(startPlateau),optVpsp,h->GetBinContent(optVpsp),hm->GetBinContent(optVpsp));

#else
			  float top=0.;
			  float bottom=1025.;
			  int optVpsp=0;
			  for (int k=5;k<55;k++)
			    {
			      if (hm->GetBinContent(k) == 0) continue;
			      if (hm->GetBinContent(k)>top)
				top=hm->GetBinContent(k);
			      if (hm->GetBinContent(k)<bottom)
				bottom=hm->GetBinContent(k);

			    }
			  float optimum = bottom+1./3.*(top-bottom);

			  int k2;
			  for ( k2=5;k2<55;k2++)
			    {
			      if (hm->GetBinContent(k2)<optimum) break;

			    }

			  fVpsp[j][i]= k2;

			  
			  printf("Detector %d %s APV %d : Bottom %f Top %f Optimum %f Setting %d \n",i,name,j,bottom,top,optimum,fVpsp[j][i]);

#endif
			}
		    }
		}
	}
	  //End of added lines
	
    }



  return;
}
void TBMonitor::initRUTree(Char_t* fdir,Int_t nrun,Int_t fseq)
{
  if (fTRUOut!=NULL) return;
  Int_t ier,i,j,k,l,nentries;
Char_t name[256];
sprintf(name,"%s/RU%.7d_%.3d.root",fdir,nrun,fseq);
 printf("File name of the tree %s \n",name);
fTRUOut = new TFile(name,"RECREATE","RAW DATA");
 fTRUOut->SetCompressionLevel(0);
fTRU = new TTree("TRU","Root Tree to Store Raw Data");
 fTRUOut->SetCompressionLevel(0);
fTRU->SetAutoSave(140000000); 
Int_t bufsize = 220000;
Int_t split=0;

 if (fListOfRU!=NULL)
   {
     for (j=0;j<fListOfRU->GetEntries();j++)
       {
	 RU[j] = (TBRU*) fListOfRU->At(j);
	 std::cout << " New RU branch " << j <<" @ " << RU[j] << std::endl;
         char sizename[256];
         char arrayname[256];
         char sizetype[256];
         char arraytype[256];
         sprintf(sizename,"size_RU_%x",RU[j]->getInstance());
         sprintf(sizetype,"size_RU_%x/I",RU[j]->getInstance());

         fTRU->Branch(sizename, &RU[j]->fSize,sizetype);
	 printf("====> Branch %s %s %d\n",sizename,sizetype,RU[j]->getInstance());

         sprintf(arraytype,"RU_%x[size_RU_%x]/I",RU[j]->getInstance(),RU[j]->getInstance());
         sprintf(arrayname,"RU_%x",RU[j]->getInstance());

         fTRU->Branch(arrayname, RU[j]->fBuffer,arraytype);
	 printf("====> Branch %s %s %d\n",arrayname,arraytype,RU[j]->getInstance());



       }
   }


  fTRU->Print();
}

void TBMonitor::fillRUTree()
{
  if (fTRU == NULL) return;
  fTRUOut->cd();
  fTRU->Fill();

}
void TBMonitor::closeRUTree()
{
  if (fTRUOut == NULL) return;

  fTRUOut->cd();
  fTRU->Write();
  fTRUOut->Close();

  printf("Now I stop!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  fTRUOut=NULL;


  fTRU=NULL;
}

void TBMonitor::FitOptoTick(int gain,int imod,int jtick)
{
  TH1D* hb= (TH1D*) fOptoHisto[imod]->getOptoBase(jtick,gain);
  TH1D* ht= (TH1D*) fOptoHisto[imod]->getOptoTick(jtick,gain);
  //Look for minimum and maximum of the base
  float basemin=9999.; float basemax=-9999.;
  float tickmin=9999.; float tickmax=-9999.;
  for (int k=2;k<63;k++)
    {
      if (hb->GetBinContent(k)<basemin) basemin = hb->GetBinContent(k);
      if (hb->GetBinContent(k)>basemax) basemax = hb->GetBinContent(k);
      if (ht->GetBinContent(k)<tickmin) tickmin = hb->GetBinContent(k);
      if (ht->GetBinContent(k)>tickmax) tickmax = hb->GetBinContent(k);

    }
 
  // Find 10-90 % data of the base
  float low=basemin+0.3*(basemax-basemin);
  float high=basemax-0.3*(basemax-basemin);
  double xsum=0,ysum=0,x2sum=0,y2sum=0,xysum=0,np=0;
   for (int k=2;k<63;k++)
    {
      if (hb->GetBinContent(k)<low) continue;
      if (hb->GetBinContent(k)>high) continue;
      np+=1;
      xsum+=k;
      x2sum+=k*k;
      ysum+=hb->GetBinContent(k);
      y2sum+=hb->GetBinContent(k)*hb->GetBinContent(k);
      xysum+=k*hb->GetBinContent(k);

    }
  
   double vx2= 1./np*x2sum - (xsum*xsum/np/np);
   double vxy= 1./np*xysum - (ysum*xsum/np/np);

   double ab = vxy/vx2;
   double bb = ysum/np - ab * xsum/np;
   double b300  =(500. - bb)/ab;

   //cout <<" base : " <<ab <<" " << bb << " " << b300 << " with " <<np<< endl;

  // Find 10-90 % data of the tick
   low=tickmin+0.3*(tickmax-tickmin);
   high=tickmax-0.3*(tickmax-tickmin);
    xsum=0;ysum=0;x2sum=0;y2sum=0;xysum=0;np=0;
   for (int k=2;k<63;k++)
    {
      if (ht->GetBinContent(k)<low) continue;
      if (ht->GetBinContent(k)>high) continue;
      np+=1;
      xsum+=k;
      x2sum+=k*k;
      ysum+=ht->GetBinContent(k);
      y2sum+=ht->GetBinContent(k)*ht->GetBinContent(k);
      xysum+=k*ht->GetBinContent(k);

    }
  
    vx2= 1./np*x2sum - (xsum*xsum/np/np);
    vxy= 1./np*xysum - (ysum*xsum/np/np);

   double at = vxy/vx2;
   double bt = ysum/np - at * xsum/np;
   double t300  =(500. - bt)/at;
   
  // cout <<" Tick : " <<at <<" " << bt << " " << t300 <<" with " <<np<< endl;

  float tgain= (b300-t300)*ab/800.;

  //cout << "And the gain is " << tgain << " or " << ((bt-bb))/800. <<" Tick height " << tgain*800.<< endl;
	int ibasestart=-1;
	 for (int k=2;k<63;k++)
		 {
		if ((hb->GetBinContent(k)-hb->GetBinContent(k-1))>0.9*ab)
		   {ibasestart=k;break;}	
	         }
  // Now get the optimal bias setting
  int baseval=hb->GetBinContent(ibasestart);
			  int tickval=ht->GetBinContent(ibasestart);
// cout <<"Deriv "<< hb->GetBinContent(ibasestart)-hb->GetBinContent(ibasestart-1) <<endl;
			  fOptoBias[gain][jtick][imod]= ibasestart;
			  fOptoBaseValue[gain][jtick][imod]= baseval;
			  fOptoTickValue[gain][jtick][imod]= tickval;
			  fOptoBaseDeriv[gain][jtick][imod]= ab;
			  fOptoTickDeriv[gain][jtick][imod]= at;
			  fOptoGain[gain][jtick][imod] = tgain;
			  fOptoStart[gain][jtick][imod] = ibasestart;
			  
			  printf("Detector %d Laser %d Gain %d Bias %d base val %d  deriv %f Tick val %d deriv %f Tick Height %d calculated %f \n",imod,jtick,gain,ibasestart,baseval,ab,tickval,at,tickval-baseval,tgain*800.);
  return;
}
