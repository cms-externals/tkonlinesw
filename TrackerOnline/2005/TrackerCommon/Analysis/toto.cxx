#include "TBMonitor.h"
#include "TKey.h"
#include <math.h>

ClassImp(TBClusterHisto)
ClassImp(TBCalibrationHisto)
ClassImp(TBTimingHisto)
ClassImp(TBOptoHisto)
ClassImp(TBLatencyHisto)
ClassImp(TBDelayHisto)
ClassImp(TBMonitor)
TBClusterHisto::TBClusterHisto(TBGeo* g)
{
  Int_t ns= g->GetLast() - g->Get1st()+1;
  Int_t id= g->GetIdLocal();
  char name[256];

  memset(name,0,256);
  sprintf(name,"number_of_strip_0x%.4x",id);
  fNumberOfStrip=  new TH1S(name,name,20,0.1,20.1);

  memset(name,0,256);
  sprintf(name,"cluster_noise_0x%.4x",id);
  fClusterNoise = new TH1D(name,name,100,0.,20.);

  memset(name,0,256);
  sprintf(name,"cluster_signal_0x%.4x",id);
  fClusterSignal = new TH1D(name,name,200,0.,50.);

  memset(name,0,256);
  sprintf(name,"cluster_position_0x%.4x",id);
  fClusterPosition = new TH1D(name,name,ns,0.1,ns+0.1);

  memset(name,0,256);
  sprintf(name,"seed_noise_0x%.4x",id);
  fSeedNoise = new TH1D(name,name,100,0.,20.);

  memset(name,0,256);
  sprintf(name,"seed_signal_0x%.4x",id);
  fSeedSignal = new TH1D(name,name,200,0.,50.);

  memset(name,0,256);
  sprintf(name,"seed_position_0x%.4x",id);
  fSeedPosition = new TH1D(name,name,ns,0.1,ns+0.1);

}  
TBClusterHisto::~TBClusterHisto()
{
  delete fNumberOfStrip;
  delete fClusterNoise;
  delete fClusterSignal;
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
  fClusterPosition->Reset();
  fSeedNoise->Reset();
  fSeedSignal->Reset();
  fSeedPosition->Reset();
  return;
}


void TBClusterHisto::update(TObjArray* clusters)
{
  for (Int_t l=0;l<clusters->GetEntriesFast();l++)
    {
      TBClu* cl = (TBClu*) clusters->At(l);
      fNumberOfStrip->Fill(cl->GetNstrip()*1.);
      fSeedPosition->Fill(cl->GetHighestStrip()*1.);
      fSeedSignal->Fill(cl->GetHighest()/cl->GetSeedNoise());
      fClusterSignal->Fill(cl->GetV()/cl->GetSeedNoise());
      fClusterNoise->Fill(cl->GetNoise());
      fClusterPosition->Fill(cl->GetXLocal());
    }
}

TBCalibrationHisto::TBCalibrationHisto(TBGeo* g)
{
 Int_t ns= g->GetLast()-g->Get1st()+1;
  Int_t id= g->GetIdLocal();
  char name[256];
  for (Int_t i=ns;i<768;i++)
    fStrip[i]=NULL;
  for (Int_t i=0;i<ns;i++)
    {
      memset(name,0,256);
      sprintf(name,"calibration_strip_0x%.4x_%.3d",id,i);
      fStrip[i]=  new TProfile(name,name,100,0.1,100.1,-512.,512.," ");
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
	  vped+=r->GetADC(j*128+k);
	  k+=8;
	}
      vped/=16.;
      Float_t vcal;
      for (int k=ical;k<128;)
	{
	  vcal=r->GetADC(j*128+k);
	  fStrip[j*128+k]->Fill(t,(vcal-vped)*sign);
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
	  fStrip[j*128+k]->Fill(t,(vcal-vped)*sign);
	  //	  if (k == ical)
	  //  printf("%f diff %f %f %f \n",t,vcal,vped,vcal-vped);
	  k+=8;
	}
    }
}
TBTimingHisto::TBTimingHisto(TBGeo* g)
{
 
  Int_t id= g->GetIdLocal();
  char name[256];
  fTick[0]=NULL;fTick[1]=NULL;fTick[2]=NULL;
  for (Int_t i=0;i<g->GetNChips();i++)
    {
      memset(name,0,256);
      sprintf(name,"tick_chip_0x%.4x-%.3d",id,i);
      fTick[i]=  new TProfile(name,name,40*24,0.1,(40*25)+.1,0.,512.," ");
    }
}
TBTimingHisto::~TBTimingHisto()
{
  for (Int_t i=0;i<3;i++)
    if (fTick[i]!=NULL) delete fTick[i];
}
void TBTimingHisto::reset()
{
  for (Int_t i=0;i<3;i++)
    if (fTick[i]!=NULL)  fTick[i]->Reset();
}

void TBTimingHisto::update(TBEventHeader* h, TBRaw* r)
{      
 Float_t t;

 TBGeo* g = r->GetGeo();

  for (int j=0;j<g->GetNChips();j++)
    {
      
      Float_t vped=0;
      for (int is=0;is<40;is++)
	{
	   t = (is+1)*25-(h->GetPllFine()*25./24.);
	   vped=r->GetHisto(j)->fArray[is+1];
	   fTick[j]->Fill(t,vped);
	}  

    }
}
TBOptoHisto::TBOptoHisto(TBGeo* g)
{
 
  Int_t id= g->GetIdLocal();
  char name[256];
  for (Int_t ga=0;ga<4;ga++)
    {
      for (Int_t i=0;i<3;i++)
	{
	  fTick[i][ga]=NULL;
	  fBase[i][ga]=NULL;
	  fDiff[i][ga]=NULL;
	}
    for (Int_t i=0;i<g->GetNChips();i++)
      {
	memset(name,0,256);
	sprintf(name,"tick_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fTick[i][ga]=  new TProfile(name,name,63,0.1,63+.1,0.,512.," ");
	memset(name,0,256);
	sprintf(name,"base_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fBase[i][ga]=  new TProfile(name,name,63,0.1,63+.1,0.,512.," ");
	memset(name,0,256);
	sprintf(name,"diff_chip_0x%.4x_%.3d_gain%.1d",id,i,ga);
	fDiff[i][ga]=  new TH1D(name,name,63,0.1,63+.1);
      }
    }
}
TBOptoHisto::~TBOptoHisto()
{
  for (Int_t g=0;g<3;g++)
    for (Int_t i=0;i<3;i++)
      {
	if (fTick[i][g]!=NULL) delete fTick[i][g];
	if (fBase[i][g]!=NULL) delete fBase[i][g];
	if (fDiff[i][g]!=NULL) delete fDiff[i][g];
      }
}
void TBOptoHisto::reset()
{
  for (Int_t g=0;g<3;g++)
    for (Int_t i=0;i<3;i++)
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
     
     Float_t vmin=512,vmax=0;;
     for (int is=0;is<128;is++)
       {
	 float v=r->GetHisto(j)->fArray[is+1];
	 vmin= (v<vmin)?v:vmin;
	 vmax= (v>vmax)?v:vmax;
       }
     float med=(vmin+vmax)/2.;
     for (int is=0;is<128;is++)
       {
	 float v=r->GetHisto(j)->fArray[is+1];;
	 if (v<med) 
	   fBase[j][gain]->Fill(bias,v,1.);
	 else
	   fTick[j][gain]->Fill(bias,v,1.);
       }

 
     for (int i=0;i<63;i++)
       fDiff[j][gain]->SetBinContent(i+1,fTick[j][gain]->GetBinContent(i)-fBase[j][gain]->GetBinContent(i));
   }
}
TBLatencyHisto::TBLatencyHisto(TBGeo* g)
{
 
  Int_t id= g->GetIdLocal();
  char name[256];

  memset(name,0,256);
  sprintf(name,"latency_scan_signal_0x%.4x",id);
  fClusterSignal=  new TProfile(name,name,200,0.1,200.1,7.,50.," ");
  memset(name,0,256);
  sprintf(name,"latency_scan_ncluster_0x%.4x",id);
  fNumberOfCluster=  new TH1D(name,name,200,0.1,200.1);
  
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
 if (clusters->GetEntriesFast()>0) fNumberOfCluster->Fill(t,clusters->GetEntriesFast()*1.);
  for (Int_t l=0;l<clusters->GetEntriesFast();l++)
    {
      TBClu* cl = (TBClu*) clusters->At(l);
      fClusterSignal->Fill(t,cl->GetV()/cl->GetSeedNoise(),1.);
    }
}
TBDelayHisto::TBDelayHisto(TBGeo* g)
{
 
  Int_t id= g->GetIdLocal();
  char name[256];

  memset(name,0,256);
  sprintf(name,"delay_scan_0x%.4x",id);
  fClusterSignal=  new TProfile(name,name,200,0.1,200*25./24.+.1,8.,50.," ");

  memset(name,0,256);
  sprintf(name,"delay_scan_ncluster_0x%.4x",id);
  fNumberOfCluster=  new TH1D(name,name,200,0.1,200*25./24.+0.1);

  
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

TBMonitor::TBMonitor()
{
  for (int i=0;i<MAX_DET;i++)
    {
      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
    }
  header_ = new TBEventHeader();

  fTOut=NULL;
  fT=NULL;
  fmfile=NULL;

  fHistoList = new mapTH1();
  fHistoNames = new mapString();
}
TBMonitor::~TBMonitor()
{
  delete header_;

  for (int i=0;i<MAX_DET;i++)
    {
     
      
      if (fClusterHisto[i]!=NULL) delete fClusterHisto[i];
      if (fCalibrationHisto[i]!=NULL) delete fCalibrationHisto[i];
      if (fTimingHisto[i]!=NULL) delete fTimingHisto[i];
      if (fOptoHisto[i]!=NULL) delete fOptoHisto[i];
      if (fLatencyHisto[i]!=NULL) delete fLatencyHisto[i];
      if (fDelayHisto[i]!=NULL) delete fDelayHisto[i];
      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;
    }

  printf("Deleting TBMonitor \n");
}
void TBMonitor::newEvent(Int_t *pilot)
{


  header_->Update(pilot);
  header_->setLatency(pilot[51]);
  header_->setCalSel(pilot[52]);
  header_->setCalChannel(pilot[53]);

  header_->setPllCoarse(pilot[81]);
  header_->setPllFine(pilot[82]);

  header_->setOptoGain(pilot[91]);
  header_->setOptoBias(pilot[92]);

}


void TBMonitor::newFedChannel(Int_t idacq,UShort_t* data)
{
  for (int i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      TBGeo* g=(TBGeo*) fListOfDet->At(i);
      TBRaw* r = (TBRaw*) fListOfRaw->At(i); 
      for (int j=0;j<g->GetNChips();j++)
	{
	  if (idacq!=g->GetIdAcq(j)) continue;
	  r->Fill(j,data);
	}
    }
}
void TBMonitor::reorder()
{
  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {

      TBRaw* r = (TBRaw*) fListOfRaw->At(k);
      r->ReOrder();
      //     printf("%d is reordered\n",k);
    }
}

void TBMonitor::updatePedestal()
{
  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
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
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,n);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      fClusterHisto[k]->update(fListOfClu);
    }

}
void TBMonitor::updateCalibration()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      

      fCalibrationHisto[k]->update(header_,r,p);
    }


}

void TBMonitor::updateTimingAdjustement()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      fTimingHisto[k]->update(header_,r);
    }

}

void TBMonitor::updateOptoAdjustement()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      fOptoHisto[k]->update(header_,r);
    }

}

void TBMonitor::updateLatencyCurve()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,n);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      fLatencyHisto[k]->update(header_,fListOfClu);
    }

}

void TBMonitor::updateDelayCurve()
{

  for (int k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
      TBPed* p = (TBPed*) fListOfPedestal->At(k);
      TBPed* n = (TBPed*) fListOfNoise->At(k);
      TBCor *c = (TBCor*) fListOfCor->At(k); 
      c->Fill(r,p,n);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      fDelayHisto[k]->update(header_,fListOfClu);
    }

}
void TBMonitor::deleteHistos()
{
  deleteHistos(0);
}

void TBMonitor::deleteHistos(int iseq)
{


  Char_t fnout[256];
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    sprintf(fnout,"/tmp/TBMonitor%.7d_%.3d.root",header_->GetRun(),iseq);
  else
    sprintf(fnout,"%s/TBMonitor%.7d_%.3d.root",fscr,header_->GetRun(),iseq);
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
      if (fCalibrationHisto[i]!=NULL)  
	
	{
	  printf("\t calibration ");
	  TDirectory *cald = new TDirectory("Calibration","Calibration","");
	  cald->cd();
	  
	  for (int j=0;j<768;j++)
	    if (fCalibrationHisto[i]->getCalibrationStrip(j)!=NULL)
	      {
		fCalibrationHisto[i]->getCalibrationStrip(j)->Write();

		
	      }

	  printf("Done \n");
	}
      //      getchar();
      det->cd();

      if (fClusterHisto[i]!=NULL)
	{
	  printf("\t cluster ");
	  TDirectory *clud = new TDirectory("Cluster","Cluster","");
	  clud->cd();

	  fClusterHisto[i]->getNumberOfStrip()->Write();
	  fClusterHisto[i]->getClusterNoise()->Write();
	  fClusterHisto[i]->getClusterSignal()->Write();
	  fClusterHisto[i]->getClusterPosition()->Write();
	  fClusterHisto[i]->getSeedNoise()->Write();
	  fClusterHisto[i]->getSeedSignal()->Write();
	  fClusterHisto[i]->getSeedPosition()->Write();
	  printf("Done \n");
	}
      det->cd();
      if (fTimingHisto[i]!=NULL)
	{
	  printf("\t timing ");
	  TDirectory *timd = new TDirectory("Timing","Timing","");
	  timd->cd();

	  for (int j=0;j<3;j++)
	    if (fTimingHisto[i]->getTimingTick(j)!=NULL)
	      fTimingHisto[i]->getTimingTick(j)->Write();
	  printf("Done \n");
	}
      det->cd();
      if (fDelayHisto[i]!=NULL)
	{
	  TDirectory *deld = new TDirectory("Delay","Delay","");
	  deld->cd();

	  fDelayHisto[i]->getDelayClusterSignal()->Write();
	  fDelayHisto[i]->getDelayNumberOfCluster()->Write();

	  
	}
      det->cd();
      if (fOptoHisto[i]!=NULL)
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
	}


      
    }
  cout <<" closing TBMon " << endl;
  fout->Close();

  cout <<" clearing " << endl;
  /*
  if (header_->GetTriggerType() == TBEventHeader::PEDESTAL)
    this->toFile(header_->GetRun(),header_->GetTriggerType());
  */
  for (int i=0;i<MAX_DET;i++)
    {
     
      
      if (fClusterHisto[i]!=NULL) fClusterHisto[i]->reset();
      if (fCalibrationHisto[i]!=NULL) fCalibrationHisto[i]->reset();
      if (fTimingHisto[i]!=NULL) fTimingHisto[i]->reset();
      if (fOptoHisto[i]!=NULL) fOptoHisto[i]->reset();
      if (fLatencyHisto[i]!=NULL) fLatencyHisto[i]->reset();
      if (fDelayHisto[i]!=NULL) fDelayHisto[i]->reset();
    }
#ifdef DELETEHISTO
  for (int i=0;i<MAX_DET;i++)
    {
     
      
      if (fClusterHisto[i]!=NULL) delete fClusterHisto[i];
      if (fCalibrationHisto[i]!=NULL) delete fCalibrationHisto[i];
      if (fTimingHisto[i]!=NULL) delete fTimingHisto[i];
      if (fOptoHisto[i]!=NULL) delete fOptoHisto[i];
      if (fLatencyHisto[i]!=NULL) delete fLatencyHisto[i];
      if (fDelayHisto[i]!=NULL) delete fDelayHisto[i];

      fClusterHisto[i]=NULL;
      fCalibrationHisto[i]=NULL;
      fTimingHisto[i]=NULL;
      fOptoHisto[i]=NULL;
      fLatencyHisto[i]=NULL;
      fDelayHisto[i]=NULL;

    }
#endif
  fHistoList->clear();
  fHistoNames->clear();
}

void TBMonitor::updateMaps()
{
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
	  sprintf(hname,"|BeamTest|Det%.2d|Raw data|Chip%.2d",i,j);
	  TH1* h = r->GetHisto(j);
	  fHistoList->insert(pairTH1(key,h));
	  fHistoNames->insert(pairString(hname,key));
	  key++;
	}
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|Raw data|All",i);
      TH1* h = r->GetHisto();
      fHistoList->insert(pairTH1(key,h));
      fHistoNames->insert(pairString(hname,key));
      key++;


      TBPed* p = (TBPed*) fListOfPedestal->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|Pedestal|Position",i);
      TH1* hpp  = p->GetProfile(); 
      fHistoList->insert(pairTH1(key,hpp));
      fHistoNames->insert(pairString(hname,key));
      key++;
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|Pedestal|Noise",i);
      TH1* hpn  = p->GetHistoRMS(); 
      fHistoList->insert(pairTH1(key,hpn));
      fHistoNames->insert(pairString(hname,key));
      key++;

      TBPed* n = (TBPed*) fListOfNoise->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|PedestalAfterCmn|Position",i);
      TH1* hnp  = n->GetProfile(); 
      fHistoList->insert(pairTH1(key,hnp));
      fHistoNames->insert(pairString(hname,key));
      key++;
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|PedestalAfterCmn|Noise",i);
      TH1* hnn  = n->GetHistoRMS(); 
      fHistoList->insert(pairTH1(key,hnn));
      fHistoNames->insert(pairString(hname,key));
      key++;

      TBCor* c = (TBCor*) fListOfCor->At(i);
      memset(hname,0,256);
      sprintf(hname,"|BeamTest|Det%.2d|Corrected|Raw",i);
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
		  sprintf(hname,"|BeamTest|Det%.2d|Calibration|Strip%.3d",i,j);
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
		  sprintf(hname,"|BeamTest|Det%.2d|Timing|Tick%.1d",i,j);
		  fHistoList->insert(pairTH1(key,hs));
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
		    sprintf(hname,"|BeamTest|Det%.2d|Opto|Gain%.1d|Tick%.1d",i,g,j);
		    fHistoList->insert(pairTH1(key,hs));
		    fHistoNames->insert(pairString(hname,key));
		    key++;

		  }
		TH1* hb = fOptoHisto[i]->getOptoBase(j,g);
		if (hb!=NULL)
		  {
		    memset(hname,0,256);
		    sprintf(hname,"|BeamTest|Det%.2d|Opto|Gain%.1d|Base%.1d",i,g,j);
		    fHistoList->insert(pairTH1(key,hb));
		    fHistoNames->insert(pairString(hname,key));
		    key++;

		  }
		TH1* hd = fOptoHisto[i]->getOptoDiff(j,g);
		if (hd!=NULL)
		  {
		    memset(hname,0,256);
		    sprintf(hname,"|BeamTest|Det%.2d|Opto|Gain%.1d|Diff%.1d",i,g,j);
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
	      sprintf(hname,"|BeamTest|Det%.2d|Latency|NClus",i);
	      fHistoList->insert(pairTH1(key,hn));
	      fHistoNames->insert(pairString(hname,key));
	      key++;


	      TH1* hs = fLatencyHisto[i]->getLatencyClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Latency|Signal",i);
	      fHistoList->insert(pairTH1(key,hs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	}


      if (fClusterHisto[i]!=NULL)
	{
	      TH1* hns = fClusterHisto[i]->getNumberOfStrip();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Cluster|Nstrip",i);
	      fHistoList->insert(pairTH1(key,hns));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcs = fClusterHisto[i]->getClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Cluster|Signal",i);
	      fHistoList->insert(pairTH1(key,hcs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hcp = fClusterHisto[i]->getClusterPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Cluster|Position",i);
	      fHistoList->insert(pairTH1(key,hcp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	      TH1* hss = fClusterHisto[i]->getSeedSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Cluster|SeedSignal",i);
	      fHistoList->insert(pairTH1(key,hss));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	      TH1* hsp = fClusterHisto[i]->getSeedPosition();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Cluster|SeedPosition",i);
	      fHistoList->insert(pairTH1(key,hsp));
	      fHistoNames->insert(pairString(hname,key));
	      key++;
	}

      if (fDelayHisto[i]!=NULL)
	{


	      TH1* hn = fDelayHisto[i]->getDelayNumberOfCluster();

	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Delay|NClus",i);
	      fHistoList->insert(pairTH1(key,hn));
	      fHistoNames->insert(pairString(hname,key));
	      key++;



	      TH1* hs = fDelayHisto[i]->getDelayClusterSignal();
	      memset(hname,0,256);
	      sprintf(hname,"|BeamTest|Det%.2d|Delay|Signal",i);
	      fHistoList->insert(pairTH1(key,hs));
	      fHistoNames->insert(pairString(hname,key));
	      key++;

	}

    
    }

}
void TBMonitor::initHistos()
{
  initHistos(0);
}
void TBMonitor::initHistos(int ir)
{
  TFile* fHistoOut = new TFile("/tmp/tot","RECREATE");
  fHistoOut->cd();
  for(int i=0;i<fListOfDet->GetEntries();i++)
    { 
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      if (fClusterHisto[i]==NULL)
	fClusterHisto[i] = new TBClusterHisto(g);
      if (fCalibrationHisto[i]==NULL)
	fCalibrationHisto[i] = new TBCalibrationHisto(g);
      if (fTimingHisto[i]==NULL)
	fTimingHisto[i] = new TBTimingHisto(g);
      if (fOptoHisto[i]==NULL)
	fOptoHisto[i] = new TBOptoHisto(g);
      if (fLatencyHisto[i]==NULL)
	fLatencyHisto[i] = new TBLatencyHisto(g);
      if (fDelayHisto[i]==NULL)
	      fDelayHisto[i] = new TBDelayHisto(g);
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
  int j;Char_t name[256];
  TObjArray* allp = new TObjArray(fListOfDet->GetEntriesFast());
  for (j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
      memset(name,0,256);
      sprintf(name,"ped-%x",g->GetIdLocal());
      TBPed *p = (TBPed*) f->Get(name);
      allp->AddLast(p);
   }
  TObjArray* alln = new TObjArray(fListOfDet->GetEntriesFast());
  for (j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
      memset(name,0,256);
      sprintf(name,"noi-%x",g->GetIdLocal());
      TBPed *p = (TBPed*) f->Get(name);
      alln->AddLast(p);
   }
  SetPedestal(allp);
  FlagDeadPedestal(allp);
  SetNoise(alln);
  FlagDeadNoise(alln);
  updateMaps();
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
void TBMonitor::initTree(Int_t nrun)
{
  char* fscr=getenv("SCRATCH");
  if (fscr==NULL)
    this->initTree("/tmp",nrun,0);
  else
    this->initTree(fscr,nrun,0);
}

void TBMonitor::initTree(Char_t* fdir,Int_t nrun,Int_t fseq)
{
  if (fTOut!=NULL) return;
  Int_t ier,i,j,k,l,nentries;
Char_t name[256];
sprintf(name,"%s/sir%.7d_%.3d.root",fdir,nrun,fseq);
fTOut = new TFile(name,"RECREATE","RAW DATA");
 fTOut->SetCompressionLevel(2);
fT = new TTree("TB","Root Tree to Store Raw Data");
 fTOut->SetCompressionLevel(2);
fT->SetAutoSave(1000000000); 
Int_t bufsize = 120000;
Int_t split=0;


 for (j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g = (TBGeo*) fListOfDet->At(j);
      R[j] = (TBRaw*) fListOfRaw->At(j);
      sprintf(name,"raw_%x",g->GetIdLocal());
      TBranch* branch=fT->Branch(name, "TBRaw", &R[j], bufsize,split);
      branch->SetAutoDelete(kFALSE);
    }


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
  char name[255];
  if (GetEventHeader()->GetTriggerType() == TBEventHeader::TIMING)
    {
      for (Int_t i=0;i<fListOfDet->GetEntriesFast();i++)
	{
	  
	  TBGeo* g =  (TBGeo*)  fListOfDet->At(i);
	  memset(name,0,255);
	  sprintf(name,"Local-%x",g->GetIdLocal());
	  if (fTimingHisto[i]!=NULL)
	    {
	      for (int j=0;j<3;j++)
		if (fTimingHisto[i]->getTimingTick(j)!=NULL)
		  {
		    TH1D* h= (TH1D*) fTimingHisto[i]->getTimingTick(j);
		    float maxdev=-999;
		    int ideriv;
		    for (int is=2;is<1000;is++)
		      {
			float deriv = (h->GetBinContent(is)-h->GetBinContent(is-1));
			if (deriv>maxdev)
			  {
			    maxdev=deriv;
			    ideriv=is;
			  }
		      }
		    int goodsample=int( rint(ideriv*25./24.+15.))%25;
		    printf("Detector %d %s tick %d max %f %f %f %d \n",i,name,j,ideriv*25./24.,maxdev,ideriv*25./24.+15.,goodsample);
		  }
	    }
	}
    }
  if (GetEventHeader()->GetTriggerType() == TBEventHeader::GAINSCAN)
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
		  for (int gain=1;gain<4;gain++)
		    {
		      if (fOptoHisto[i]->getOptoDiff(j,gain)!=NULL)
			{
			  TH1D* h= (TH1D*) fOptoHisto[i]->getOptoDiff(j,gain);
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

			  int plateauWidth=0;
			  for (int k2=1;k2<63;k2++)
			    {
			      if (h->GetBinContent(k2)>0.97*hiTickH)
				{
				  plateauWidth++;
				  if (plateauWidth<=2) optOptoBias=k2;
				}
			    }
			  printf("Detector %d %s Laser %d Gain %d Bias %d Width %d\n",i,name,j,gain,optOptoBias,plateauWidth);
			}
		    }
		}
	    }
	  //End of added lines
	}
    }

  return;
}
