//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBPed.h"
#include "TBEventHeader.h"
#include "TBRaw.h"
#include "TBRU.h"
#include "TBCor.h"
#include "TBClu.h"
#include "TBRun.h"
#include "TBEvent.h"
#include "TBBuilder.h"
#include "TObjectTable.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TCanvas.h"
#include <unistd.h>
ClassImp(TBBuilder)
// Canvas for Display 
TCanvas* gRAWC=NULL;
TCanvas* gCHIP=NULL;
TPad *gCHIP1,*gCHIP2;
TCanvas* gCORC=NULL;
TCanvas* gDETC=NULL;
TPad*    gPADC[MAX_DET];
TCanvas* gFEDC=NULL;
TPad*    gFEDCH[MAX_DET][2];
TH1F*    gDETH[MAX_DET];

/* Analysis class */
TBBuilder::TBBuilder()
{
  fListOfDet = NULL;
  fListOfRaw = NULL;
  fListOfCor = NULL;
  fListOfPedestal = NULL;
  fListOfNoise = NULL;
  fListOfClu = NULL;
  //Line below added by BT
  fListOfCmn = NULL;
  fListOfRU=NULL;
  fCurEvent = NULL;
  fCurRun = NULL;
  fFirstEvent = 0;
  fLastEvent = 10000000;
  fmfile = NULL;
}

/* Destructor Close all files */
TBBuilder::~TBBuilder()
{
  printf("Deleting Builder %x %x \n",fCurRun,fCurEvent);
  if (fCurRun !=NULL) delete fCurRun;
  if (fCurEvent !=NULL) delete fCurEvent;
  if (fListOfDet !=NULL)
    {
      fListOfDet->Delete();
      delete fListOfDet;
    }
  if (fListOfRaw !=NULL)
    {
      fListOfRaw->Delete();
      delete fListOfRaw;
    }
  if (fListOfClu !=NULL)
    {
      fListOfClu->Delete();
      delete fListOfClu;
    }
  if (fListOfCor !=NULL)
    {
      fListOfCor->Delete();
      delete fListOfCor;
    }
  if (fListOfCmn !=NULL)
    {
      fListOfCmn->Delete();
      delete fListOfCmn;
    }

  if (fListOfPedestal !=NULL)
    {
      fListOfPedestal->Delete();
      delete fListOfPedestal;
    }
  if (fListOfNoise !=NULL)
    {
      fListOfNoise->Delete();
      delete fListOfNoise;
    }

  if (fListOfRU !=NULL)
    {
      fListOfRU->Delete();
      delete fListOfRU;
    }
}
/* Adding a new detector */
void TBBuilder::AddDet(TBGeo* g)
{
  Char_t name[256];
  if (fListOfDet == NULL)
    {
      fListOfDet = new TObjArray(MAX_DET);
      fListOfRaw = new TObjArray(MAX_DET);
      fListOfCor = new TObjArray(MAX_DET);
      fListOfPedestal = new TObjArray(MAX_DET);      
      fListOfNoise = new TObjArray(MAX_DET);
      fListOfClu = new TObjArray(MAX_CLU);
      //Line below added by BT
      fListOfCmn = new TObjArray(6*MAX_DET);
    }
  fListOfDet->Add(g);
  TBRaw* r = new TBRaw(g);
  fListOfRaw->Add(r);
  pair<int,TBRaw*> pr(g->GetIdAcq(0),r);
  mapRaw.insert(pr);
  TBCor* c = new TBCor(g);
  fListOfCor->Add(c);
  TBPed* p = new TBPed(g);
  p->SetGeo(g);
  sprintf(name,"ped-%x",g->GetIdLocal());
  p->SetName(name);
  fListOfPedestal->Add(p);
  TBPed* n = new TBPed(g);
  n->SetGeo(g);
  sprintf(name,"noi-%x",g->GetIdLocal());
  n->SetName(name);
  fListOfNoise->Add(n);

  //Part added by BT
  for (int i=0;i<6;i++){
    sprintf(name,"Cmn-%x-Apv%d",g->GetIdLocal(),i);
    TH1F* fCmn = new TH1F(name,name,400,-10.0,10.0);
    fListOfCmn->Add(fCmn);
    }
  //End of added part
}


/* Adding a new RU */
void TBBuilder::AddRU(Int_t instance)
{

  if (fListOfRU == NULL)
    {
      fListOfRU = new TObjArray(MAX_RU);
    }
  TBRU* r=  new TBRU(instance);
  fListOfRU->Add(r);
}


/* Pedestal creation */
TObjArray*  TBBuilder::CreatePed(Int_t nrun)
{ 
 Int_t i,j,k; 
 // Loop on files and find the run
// Create array of Pedestals
 Char_t name[256];
 TObjArray* allp = new TObjArray(fListOfDet->GetEntriesFast());
 for (j=0;j<fListOfDet->GetEntriesFast();j++)
   {
     TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
     TBPed *p = new TBPed(g);
     p->SetGeo(g);
     sprintf(name,"ped-%d-%x",nrun,g->GetIdLocal());
     p->SetName(name);
     allp->AddLast(p);
     if (fmfile !=NULL) fmfile->Add(p); 
   }
 printf("Pedestal profiles are created \n");
 return allp;


}
void TBBuilder::SavePedestal(Int_t nrun)
{
  /* Open Cluster Output file */
  Char_t fnout[256];
  if (nrun!=0)
    sprintf(fnout,"$SCRATCH/Ped%d.root",nrun);
  else
    sprintf(fnout,"$SCRATCH/Ped.root");
  TFile* fout = new TFile(fnout,"RECREATE");
  fout->cd();
/* Filling */
  fListOfPedestal->Write();
  fout->Close();
}

void TBBuilder::FillPed(Int_t nrun, TObjArray* allp)
{
  Int_t i,j,k,ier,nentries;
  // Loop on Files and found the run
  for (i=0;i<fNfiles;i++)
    {
     this->ReadFile(i); // Rewind file i
     ier= this->GetNextEvent();
     printf("FillPed ier %d \n",ier);
     if (ier == -1) continue;

     if ( fCurEvent->GetEventHeader()->GetRun()  != nrun) continue;
     break;
    }
  printf("curun %d %d %d \n",fCurRun,fCurEvent->GetEventHeader()->GetRun(),nrun);
  if (fCurRun == NULL) return;
  if ( fCurEvent->GetEventHeader()->GetRun()  != nrun) return;
  this->ReadFile(i); // Rewind File i
// Loop on all events up to 5000
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (fCurEvent->GetEventHeader()->GetEvent()%101 ==0)
	printf("DEBUG0 %d %d %d %d %d is read \n",ier, 
	       fCurEvent->GetEventHeader()->GetRun(), 
	       fCurEvent->GetEventHeader()->GetEvent(),fFirstEvent,fLastEvent ); 

      if (ier == -1) break;
      if (ier == -2) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  < fFirstEvent) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  > fLastEvent) continue;
      nentries++;
      //Line below modified by BT;we want a lot of entries!
      //if (nentries > 2000 ) break;
      if (nentries > 10000 ) break;
      if (nentries%101 == 0) printf("DEBUG1 %d %d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetInspill(), 
                                    fCurEvent->GetEventHeader()->GetEvent() ); 
      // Loop on all the detectors
      for (k=0;k<fListOfDet->GetEntriesFast();k++)
        {
          TBGeo* g =  (TBGeo*) fListOfDet->At(k);
          TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
          TBPed* p = (TBPed*) allp->At(k);
          p->Increment(r); // Update Pedestals
        }
      if (fmfile != NULL) {this->FillMapFile();}
      
//      printf("Fin de Fillped\n"); 
    }
     // Flag  Dead and Noisy
  for (k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBPed* p = (TBPed*) allp->At(k);
      Float_t low = g->GetPedDead();
      Float_t high = g->GetPedNoise();
      for (j=0;j<g->GetBonded();j++)
        { 
          if (p->GetRMS(j)< low) p->FlagDead(j);
          if (p->GetRMS(j)> high) p->FlagNoisy(j);
        }
    }
}



/* alternative to Flage Dead Pedestal in Standalone */
void TBBuilder::FlagDeadPedestal(TObjArray* allp)
{
Int_t i,j;
for (i=0;i<fListOfDet->GetEntriesFast();i++)
  {
    TBGeo* g =  (TBGeo*) fListOfDet->At(i);
    TBPed* p = (TBPed*) allp->At(i);
    Float_t low = g->GetPedDead();
    Float_t high = g->GetPedNoise();
    for (j=0;j<g->GetBonded();j++)
      { 
        if (p->GetRMS(j)< low) p->FlagDead(j);
        if (p->GetRMS(j)> high) p->FlagNoisy(j);
      }
  }
}
void TBBuilder::SetPedestal(TObjArray* allp)
{
  Int_t i;
  fListOfPedestal->Delete(); // Delete previous pedestal
  for (i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      TBPed* p = (TBPed*) allp->At(i);
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      p->SetGeo(g);
      fListOfPedestal->Add(p);
    }

}

/* Noise creation */
TObjArray*  TBBuilder::CreateNoise(Int_t nrun)
{ 
 Int_t i,j,k; 
 // Loop on files and find the run
// Create array of Pedestals
 Char_t name[256];
 TObjArray* allp = new TObjArray(fListOfDet->GetEntriesFast());
 for (j=0;j<fListOfDet->GetEntriesFast();j++)
   {
     TBGeo* g =  (TBGeo*)  fListOfDet->At(j);
     TBPed *p = new TBPed(g);
     p->SetGeo(g);
     sprintf(name,"noi-%d-%d",nrun,g->GetIdLocal());
     p->SetName(name);
     allp->AddLast(p);
     if (fmfile !=NULL) fmfile->Add(p); 
   }
 printf("Noise profiles are created \n");
 return allp;
}
/* Fill Noise */
void TBBuilder::FillNoise(Int_t nrun, TObjArray* alln)
{
 Int_t i,j,k,nentries,ier;

 // Loop on Files and found the run
 for (i=0;i<fNfiles;i++)
   {
     this->ReadFile(i); // Rewind file i
     ier= this->GetNextEvent();
     if (ier == -1) continue;
     if ( fCurEvent->GetEventHeader()->GetRun()  != nrun) continue;
     break;
   }
 if (fCurRun == NULL) return;
 if ( fCurEvent->GetEventHeader()->GetRun() != nrun) return;
 this->ReadFile(i); // Rewind File i

// Flag dead pedestal
 for (k=0;k<fListOfDet->GetEntriesFast();k++)
     {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBPed* p0 = (TBPed*) fListOfPedestal->At(k);
      TBPed* p = (TBPed*) alln->At(k);
      for (j=0;j<g->GetBonded();j++)
        {
          if (p0->IsDead(j)) p->FlagDead(j);
          if (p0->IsNoisy(j)) p->FlagNoisy(j);
        }
     }
 
// Loop on all events up to 5000
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      if (ier == -2) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  < fFirstEvent) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  > fLastEvent) continue;
      nentries++;
      //Line below modified by BT;we want a lot of entries!
      //if (nentries > 2000 ) break;
      if (nentries > 10000 ) break;
      if (nentries%101 == 0) printf("NOISE0 %d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() ); 
      // Loop on all the detectors
      for (k=0;k<fListOfDet->GetEntriesFast();k++)
        {
          TBGeo* g =  (TBGeo*) fListOfDet->At(k);
          TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
          TBPed* p = (TBPed*) fListOfPedestal->At(k);
          TBPed* n = (TBPed*) alln->At(k);
          TBCor *c = (TBCor*) fListOfCor->At(k); 
          c->Fill(r,p,p);
	  //Part added by BT-29/11/02
	  for (int kChip=0;kChip<6;kChip++){
	    TH1F* cmnH = (TH1F*) fListOfCmn->At(6*k+kChip);
	    int cmnFill = cmnH->Fill(c->GetCmn(kChip+1));
	  }
	  //End of added part
          n->Increment(c); 

        }
      if (fmfile != NULL) {this->FillMapFile();}
    }


     // Flag Dead and Noisy
  for (k=0;k<fListOfDet->GetEntriesFast();k++)
    {
      TBGeo* g =  (TBGeo*) fListOfDet->At(k);
      TBPed* p = (TBPed*) alln->At(k);
      Float_t low = g->GetNoiDead();
      Float_t high = g->GetNoiNoise();
      for (j=0;j<g->GetBonded();j++)
        {
          if (p->GetRMS(j)< low) p->FlagDead(j);
          if (p->GetRMS(j)> high) p->FlagNoisy(j);
        }
    }


}


/* Idem for Noise */
void TBBuilder::FlagDeadNoise(TObjArray* allp)
{
Int_t i,j;
for (i=0;i<fListOfDet->GetEntriesFast();i++)
  {
    TBGeo* g =  (TBGeo*) fListOfDet->At(i);
    TBPed* p = (TBPed*) allp->At(i);
    Float_t low = g->GetNoiDead();
    Float_t high = g->GetNoiNoise();
    for (j=0;j<g->GetBonded();j++)
      { 
        if (p->GetRMS(j)< low) p->FlagDead(j);
        if (p->GetRMS(j)> high) p->FlagNoisy(j);
      }
  }
}
void TBBuilder::SetNoise(TObjArray* allp)
{
  Int_t i;
  fListOfNoise->Delete(); // Delete previous pedestal
  for (i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      TBPed* p = (TBPed*) allp->At(i);
      TBGeo* g =  (TBGeo*) fListOfDet->At(i);
      p->SetGeo(g);
      fListOfNoise->Add(p);
    }

}

/* Cluster Builder */

void TBBuilder::BuildCluster(Int_t nrun)
{
  /* Open Cluster Output file */
  Char_t fnout[256];
  if (nrun!=0)
    sprintf(fnout,"$SCRATCH/Cluster%d.root",nrun);
  else
    sprintf(fnout,"$SCRATCH/Clusterall.root");
  TFile* fout = new TFile(fnout,"RECREATE");
  fout->cd();
  UShort_t ih,ns,kevt,ncl;
  UInt_t id;
  Float_t  vc,nc,vh,nh,xc;
 //	Build the Tree
  TTree *nt = new TTree("nt","An example of a ROOT tree");
  Int_t           Krun;
  Int_t           Kevt;
  Int_t           Nclus;
  UInt_t          Idet[MAX_CLU];
  Float_t         Vclu[MAX_CLU];
  Float_t         Noiclu[MAX_CLU];
  Float_t         Xclu[MAX_CLU];
  UInt_t          Nsclu[MAX_CLU];
  Float_t         Vhclu[MAX_CLU];
  Float_t         Vlclu[MAX_CLU];
  Float_t         Noistr[MAX_CLU];
  UInt_t          Istr[MAX_CLU];
  nt->Branch("Krun",&Krun,"Krun/I",25000);
  nt->Branch("Kevt",&Kevt,"Kevt/I",25000);
  nt->Branch("Nclus",&Nclus,"Nclus/i",25000);
  nt->Branch("Idet",Idet,"Idet[Nclus]/i",25000);
  nt->Branch("Vclu",Vclu,"Vclu[Nclus]/F",25000);
  nt->Branch("Noiclu",Noiclu,"Noiclu[Nclus]/F",25000);
  nt->Branch("Xclu",Xclu,"Xclu[Nclus]/F",25000);
  nt->Branch("Nsclu",Nsclu,"Nsclu[Nclus]/i",25000);
  nt->Branch("Vhclu",Vhclu,"Vhclu[Nclus]/F",25000);
  nt->Branch("Noistr",Noistr,"Noistr[Nclus]/F",25000);
  nt->Branch("Vlclu",Vlclu,"Vlclu[Nclus]/F",25000);
  nt->Branch("Istr",Istr,"Istr[Nclus]/i",25000);

  nt->Print();

  Int_t ier,i,j,k,l,nentries;
  Int_t RunFound=-1;
  // Loop on files and found the run
  Int_t ifr=-1,ilr=-1;
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      RunFound=i;
      ilr=RunFound;
      if (ifr==-1) ifr=ilr;
      //      break;
    }
  if (nrun!=0 && RunFound <0) return;

  if (nrun == 0)
    {
      ifr=0;
      ilr = fNfiles-1;
    }
  else
    {
      // ifr=RunFound;
      // ilr=RunFound;

    }
  for (i=ifr;i<=ilr;i++)
    {
  this->ReadFile(i);
      // Loop on all events 
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      if (ier == -2) continue;
      nentries++;
      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      if ( fCurEvent->GetEventHeader()->GetEvent()  < fFirstEvent) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  > fLastEvent) break;

      // Initialise entry 
      Krun =  fCurEvent->GetEventHeader()->GetRun() ;
      Kevt =  fCurEvent->GetEventHeader()->GetEvent() ;
      Nclus = 0;
      memset(Nsclu,0,MAX_CLU*sizeof(UInt_t));
      memset(Idet,0,MAX_CLU*sizeof(UInt_t));
      memset(Istr,0,MAX_CLU*sizeof(UInt_t));
      memset(Noistr,0,MAX_CLU*sizeof(Float_t));
      memset(Noiclu,0,MAX_CLU*sizeof(Float_t));
      memset(Vclu,0,MAX_CLU*sizeof(Float_t));
      memset(Vhclu,0,MAX_CLU*sizeof(Float_t));
      memset(Vlclu,0,MAX_CLU*sizeof(Float_t));
      memset(Xclu,0,MAX_CLU*sizeof(Float_t));
      fListOfClu->Delete();
      Int_t nclf =0;
      for (k=0;k<fListOfDet->GetEntriesFast();k++)
        {
          TBGeo* g =  (TBGeo*) fListOfDet->At(k);
          TBRaw* r = (TBRaw*) fListOfRaw->At(k); 
          TBPed* p = (TBPed*) fListOfPedestal->At(k);
          TBPed* n = (TBPed*) fListOfNoise->At(k);
          TBCor *c = (TBCor*) fListOfCor->At(k);
          c->Fill(r,p,n);
          id =g->GetIdLocal();
          c->BuildClusters(fListOfClu);
          ncl = fListOfClu->GetEntriesFast();
          // Add new clusters to the array
          for (l=nclf;l<ncl;l++)
            {
              if (Nclus >= MAX_CLU) break;
              TBClu* cl = (TBClu*) fListOfClu->At(l);
              
              Nsclu[Nclus] = cl->GetNstrip();
              Idet[Nclus] = id;
              Istr[Nclus] = cl->GetHighestStrip();
              Vhclu[Nclus] = cl->GetHighest();
              Noistr[Nclus] = n->GetRMS(cl->GetHighestStrip());
              Vclu[Nclus] = cl->GetV();
              Noiclu[Nclus] = cl->GetNoise();
              Xclu[Nclus] = cl->GetXLocal();
              Nclus++;
            }
          nclf =ncl; // Update end list
        }


      if (Nclus >0 ) nt->Fill();

    }
      
    }
  fout->Write();
  fout->Close();
  delete fout;
}

/* Display Utility */
void TBBuilder::DrawDet(Int_t nrun, Int_t nevt, Int_t id)

{
  Int_t ier;
  /* Build Canvas */
  if (gCHIP == NULL ) gCHIP =new TCanvas("CHIP"," RAW DATA "
                                         ,410,10,400,600);

  if (gRAWC == NULL ) gRAWC =new TCanvas("RAWC"," RAW DATA "
                                         ,10,10,800,400);
  if (gCORC == NULL ) gCORC =new TCanvas("CORC"," Corrected Data "
                                         ,10,400,800,400);
  // Loop on files and find the run/first evt
  Int_t i,j,k,l,nentries;
  // Loop on files and found the run
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      break;
    }
  this->ReadFile(i);
  // Loop on all events 
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      if (ier == -2) continue;

      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      nentries++;
      if (fCurEvent->GetEventHeader()->GetEvent() < nevt) continue; 
      if (fCurEvent->GetEventHeader()->GetEvent()%101 ==0)
	printf("%d %d is read  for %d \n \n ", 
	       fCurEvent->GetEventHeader()->GetRun(), 
	       fCurEvent->GetEventHeader()->GetEvent(),id);


      TBGeo* g =  (TBGeo*) fListOfDet->At(id);
      TBRaw* r = (TBRaw*)  fListOfRaw->At(id); 
      TBPed* p = (TBPed*) fListOfPedestal->At(id);
      // Now build corrected data and clusters
      TBPed* n = (TBPed*) fListOfNoise->At(id);
      TBCor *c = (TBCor*) fListOfCor->At(id); 
      c->Fill(r,p,n);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      //      if (fListOfClu->GetEntriesFast() <1) continue;
      // Draw Pedestal and Raw data
      TProfile* theped = p->GetProfile();
      theped->SetLineColor(3);
      theped->SetMinimum(1000.);
      gRAWC->cd();
      theped->Draw();
      r->Draw("same");
      gRAWC->Modified();
      gRAWC->Update();
      gCHIP->cd();
      r->DrawChip(0,"");
      r->DrawChip(1,"same");
      gCHIP->Modified();
      gCHIP->Update();


      // Draw Corrected
      gCORC->cd();
      c->Draw("");
      gCORC->Modified();
      gCORC->Update();
      TH1F* clh= new TH1F("clh","Clusters",g->GetBonded(),0.1,
                          g->GetBonded()+0.1);  
      for (l=0;l<3;l++)
	printf("Cmn %f \n",c->GetCmn(l));
      for (l=0;l<fListOfClu->GetEntriesFast();l++)
        {
          TBClu* cl = (TBClu*) fListOfClu->At(l);
          printf("Cluster: %d  V = %f X = %f Noise = %f \n",
                 l,cl->GetV(),cl->GetXLocal(),cl->GetNoise());
          for (Int_t m=cl->GetFirstStrip();m<=cl->GetLastStrip();m++)
            {clh->SetBinContent(m,c->GetChannel(m));}

        }
      clh->SetFillColor(2);
      clh->Draw("same");
      gCORC->Modified();
      gCORC->Update();

      printf("Continue ?");
      Char_t rep[256];
      gets(rep);
      if ( strcmp(rep,"q") == 0) break;
      delete clh;
      printf(" Return code %s \n",rep);

    }
}
/* Display Utility */
void TBBuilder::DrawHip(Int_t nrun, Float_t cut, Int_t id)

{
  Int_t ier,nevt=0,lastbx=-9999;
  /* Build Canvas */
  if (gCHIP == NULL ) gCHIP =new TCanvas("CHIP"," RAW DATA "
                                         ,410,10,400,600);
  if (gRAWC == NULL ) gRAWC =new TCanvas("RAWC"," RAW DATA "
                                         ,10,10,800,400);


  if (gCORC == NULL ) gCORC =new TCanvas("CORC"," Corrected Data "
                                         ,10,400,800,400);
  // Loop on files and find the run/first evt
  Int_t i,j,k,l,nentries;
  // Loop on files and found the run
  bool keep=false; 
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      //      break;
      // }
      // this->ReadFile(i);
  // Loop on all events 
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      if (ier == -2) continue;

      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      nentries++;
      if (fCurEvent->GetEventHeader()->GetEvent() < nevt) continue; 
      if (fCurEvent->GetEventHeader()->GetEvent()%101 ==0)
	printf("%d %d  %d is read  for %d \n \n ", 
	       fCurEvent->GetEventHeader()->GetRun(), 
	       fCurEvent->GetEventHeader()->GetEvent(),fCurEvent->GetEventHeader()->GetBx(),id);


      TBGeo* g =  (TBGeo*) fListOfDet->At(id);
      TBRaw* r = (TBRaw*)  fListOfRaw->At(id); 
      TBPed* p = (TBPed*) fListOfPedestal->At(id);
      // Now build corrected data and clusters
      TBPed* n = (TBPed*) fListOfNoise->At(id);
      TBCor *c = (TBCor*) fListOfCor->At(id); 
      c->Fill(r,p,n);
      fListOfClu->Delete();
      c->BuildClusters(fListOfClu);
      bool hip=false;
      for (int i=0;i<4;i++)
	if (TMath::Abs(c->GetCmn(i))>cut) hip=true;

      hip = (hip && (fCurEvent->GetEventHeader()->GetBx()-lastbx)>10);
      lastbx=fCurEvent->GetEventHeader()->GetBx();
	
      if (!hip && !keep) continue;
      for (int i=0;i<4;i++)
	printf("Cmn %d %f \n",i,c->GetCmn(i));

      //      if (fListOfClu->GetEntriesFast() <1) continue;
      // Draw Pedestal and Raw data
      TProfile* theped = p->GetProfile();
      theped->SetLineColor(3);
      theped->SetMinimum(1000.);
      gRAWC->cd();
      theped->Draw();
      r->Draw("same");
      gRAWC->Modified();
      gRAWC->Update();
      gCHIP->cd();
      r->DrawChip(0,"");
      r->DrawChip(1,"same");
      gCHIP->Modified();
      gCHIP->Update();

      // Draw Corrected
      gCORC->cd();
      c->Draw("");
      gCORC->Modified();
      gCORC->Update();
      TH1F* clh= new TH1F("clh","Clusters",g->GetBonded(),0.1,
                          g->GetBonded()+0.1);  

      for (l=0;l<fListOfClu->GetEntriesFast();l++)
        {
          TBClu* cl = (TBClu*) fListOfClu->At(l);
          printf("Cluster: %d  V = %f X = %f Noise = %f \n",
                 l,cl->GetV(),cl->GetXLocal(),cl->GetNoise());
          for (Int_t m=cl->GetFirstStrip();m<=cl->GetLastStrip();m++)
            {clh->SetBinContent(m,c->GetChannel(m));}

        }
      clh->SetFillColor(2);
      clh->Draw("same");
      gCORC->Modified();
      gCORC->Update();

      printf("Continue ?");
      Char_t rep[256];
      gets(rep);
      if ( strcmp(rep,"q") == 0) goto end;
      if ( strcmp(rep,"k") == 0) 
	keep=true;
      else
	keep=false;
      delete clh;
      printf(" Return code %s \n",rep);

    }
    }
 end:
  printf("On Sort\n");
}
/* Display Utility */
void TBBuilder::DrawEvent(Int_t nrun, Int_t nevt)

{
  Int_t ier;
  Int_t i,j,k,l,nentries;
  /* Build Canvas */
  if (gDETC == NULL ) 
    {
      gDETC =new TCanvas("DETC"," test Bench "
                                         ,10,10,800,800);
      gDETC->cd();
      Char_t name[256],title[256];
      Float_t psiz = 1./fListOfDet->GetEntriesFast();
      Float_t dpsiz = 0.1*psiz;
      for(i=0;i<fListOfDet->GetEntriesFast();i++)
        {
          sprintf(name,"Pad_%d",i);
          sprintf(title,"Detecteur %d",i);
          TPad* pd= new TPad(name,title,0.1,1-(i+1)*psiz+dpsiz,
                             0.9,1-i*psiz-dpsiz,0);
          gPADC[i] = pd;
          pd->SetBorderMode(0);
          pd->Draw();
          sprintf(name,"Cluster_%d",i);
          sprintf(title,"Cluster %d",i);
          TBGeo* g =  (TBGeo*) fListOfDet->At(i);
          gDETH[i] = new TH1F(name,title,g->GetBonded(),
                              0.1,g->GetBonded()+0.1);
          gDETH[i]->SetFillColor(2);

        }
    }
  // Loop on files and find the run/first evt

  // Loop on files and found the run
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      break;
    }
  this->ReadFile(i);
  // Loop on all events 
  nentries = 0;
  ier= 0;
  Bool_t fSTOP;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      fSTOP = kFALSE;
      if (ier == -2) continue;

      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      nentries++;
      if (fCurEvent->GetEventHeader()->GetEvent() < nevt) continue; 
      Int_t ncl=0;
      for(Int_t id=0;id<fListOfDet->GetEntriesFast();id++)
        {
          gDETH[id]->Reset();
	  if (fCurEvent->GetEventHeader()->GetEvent()%101 ==0)
	    printf("%d %d is read  for %d \n \n ", 
		   fCurEvent->GetEventHeader()->GetRun(), 
		   fCurEvent->GetEventHeader()->GetSpill(0),id);


          TBGeo* g =  (TBGeo*) fListOfDet->At(id);
          TBRaw* r = (TBRaw*)  fListOfRaw->At(id); 
          TBPed* p = (TBPed*) fListOfPedestal->At(id);
      // Now build corrected data and clusters
          TBPed* n = (TBPed*) fListOfNoise->At(id);
          TBCor *c = (TBCor*) fListOfCor->At(id); 
          c->Fill(r,p,n);
      // Draw Pedestal and Raw data
          /*
            TProfile* theped = p->GetProfile();
            theped->SetLineColor(3);
            gRAWC->cd();
            theped->Draw();
            r->Draw("same");
            gRAWC->Modified();
            gRAWC->Update();
            */
          fListOfClu->Delete();
          c->BuildClusters(fListOfClu);
          ncl+=fListOfClu->GetEntriesFast();
          if (fListOfClu->GetEntriesFast() <1) continue;

          for (l=0;l<fListOfClu->GetEntriesFast();l++)
            {
              TBClu* cl = (TBClu*) fListOfClu->At(l);
              printf("Cluster: %d  V = %f X = %f Noise = %f \n",
                 id,cl->GetV(),cl->GetXLocal(),cl->GetNoise());
              for (Int_t m=cl->GetFirstStrip();m<=cl->GetLastStrip();m++)
                {gDETH[id]->SetBinContent(m,c->GetChannel(m));}

            }
        }
      if (ncl >1)
        {
      // Draw Corrected

          gDETC->cd();

          for(Int_t id=0;id<fListOfDet->GetEntriesFast();id++)
            {

              gPADC[id]->cd();
              TBCor *c = (TBCor*) fListOfCor->At(id);

              c->Draw("");
              gDETH[id]->Draw("same");
              gPADC[id]->Modified();
              //              gPADC[id]->Update();
            }

          //gDETC->Modified();

          gDETC->Update();
          printf("5\n");
          printf("Continue ?");
          Char_t rep[256];
          gets(rep);
          if ( strcmp(rep,"q") == 0) fSTOP=kTRUE;

          printf(" Return code %s \n",rep);

        }
      if (fSTOP) break;
    }
}
void TBBuilder::DrawFedChannel(Int_t nrun, Int_t nevt)

{
  Int_t ier;
  Int_t i,j,k,l,nentries;
  /* Build Canvas */
  if (gFEDC == NULL ) 
    {
      gFEDC =new TCanvas("FEDC"," test Bench "
                                         ,10,10,800,800);
      gFEDC->cd();
      Char_t name[256],title[256];
      Float_t psiz = 1./fListOfDet->GetEntriesFast();
      Float_t dpsiz = 0.1*psiz;
      for(i=0;i<fListOfDet->GetEntriesFast();i++)
        {
          sprintf(name,"Pad0_%d",i);
          sprintf(title,"Detecteur %d 0",i);
          TPad* pd= new TPad(name,title,0.05,1-(i+1)*psiz+dpsiz,
                             0.45,1-i*psiz-dpsiz,0);
          gFEDCH[i][0] = pd;
          pd->SetBorderMode(0);
          pd->Draw();
          sprintf(name,"Pad1_%d",i);
          sprintf(title,"Detecteur %d 0",i);
          TPad* pd1= new TPad(name,title,0.55,1-(i+1)*psiz+dpsiz,
                             0.95,1-i*psiz-dpsiz,0);
          gFEDCH[i][1] = pd1;
          pd1->SetBorderMode(0);
          pd1->Draw();

        }
    }
  // Loop on files and find the run/first evt

  // Loop on files and found the run
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      break;
    }
  this->ReadFile(i);
  // Loop on all events 
  nentries = 0;
  ier= 0;
  Bool_t fSTOP;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      fSTOP = kFALSE;
      if (ier == -2) continue;

      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      nentries++;
      if (fCurEvent->GetEventHeader()->GetEvent() < nevt) continue; 
      Int_t ncl=0;
      for(Int_t id=0;id<fListOfDet->GetEntriesFast();id++)
        {
	  if (fCurEvent->GetEventHeader()->GetEvent()%101 ==0)
	    printf("%d %d is read  for %d \n \n ", 
		   fCurEvent->GetEventHeader()->GetRun(), 
		   fCurEvent->GetEventHeader()->GetSpill(0),id);


          TBRaw* r = (TBRaw*)  fListOfRaw->At(id); 
	  gFEDCH[id][0]->cd(); r->DrawChip(0,"");gFEDCH[id][0]->Modified();
	  gFEDCH[id][1]->cd(); r->DrawChip(0,"");gFEDCH[id][1]->Modified();


	}

          //gDETC->Modified();

          gFEDC->Update();
          printf("5\n");
          printf("Continue ?");
          Char_t rep[256];
          gets(rep);
          if ( strcmp(rep,"q") == 0) fSTOP=kTRUE;

          printf(" Return code %s \n",rep);

    
	  if (fSTOP) break;
    }

}


void TBBuilder::InitMapFile(const Char_t* cm)
{
  fmfile = TMapFile::Create(cm,"RECREATE", 10000000,"Test");
  Char_t name[256];
  for (Int_t j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g = (TBGeo*) fListOfDet->At(j);
      TBRaw* r = (TBRaw*) fListOfRaw->At(j);
      TBCor* c = (TBCor*) fListOfCor->At(j);
      TBPed* p = (TBPed*) fListOfPedestal->At(j);
      TBPed* n = (TBPed*) fListOfNoise->At(j);
      
      sprintf(name,"raw_%d",g->GetIdLocal());
      r->SetName(name);
      printf("%s is added to map file \n",name);
      
      fmfile->Add(r,name);
      
      sprintf(name,"cor_%d",g->GetIdLocal());
      c->SetName(name);
      printf("%s is added to map file \n",name);
      
      fmfile->Add(c,name);
      
      sprintf(name,"ped_%d",g->GetIdLocal());
      p->SetName(name);
      printf("%s is added to map file \n",name);
      
      fmfile->Add(p,name);
      
      sprintf(name,"noi_%d",g->GetIdLocal());
      n->SetName(name);
      printf("%s is added to map file \n",name);
      
      fmfile->Add(n,name);
  
    }

}

void TBBuilder::FillMapFile()
{
  for (Int_t j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g = (TBGeo*) fListOfDet->At(j);
      TBRaw* r = (TBRaw*) fListOfRaw->At(j);
      TBCor* c = (TBCor*) fListOfCor->At(j);
      TBPed* p = (TBPed*) fListOfPedestal->At(j);
      TBPed* n = (TBPed*) fListOfNoise->At(j);
      
      fmfile->Update(r);
      fmfile->Update(c);
      fmfile->Update(p);
      fmfile->Update(n);
    }
  fmfile->Update();
  sleep(1);
}

void TBBuilder::CloseMapFile()
{
  fmfile->Close();
}



void TBBuilder::FillOut(Int_t nrun)
{
  Int_t ier,i,j,k,l,nentries;
Char_t name[256];
//Line below modified by BT on 13/12
sprintf(name,"/tmp/sir%d.root",nrun);
fTOut = new TFile(name,"RECREATE","RAW DATA");
fTOut->SetCompressionLevel(9);
fT = new TTree("TB","Root Tree to Store Raw Data");
fT->SetAutoSave(1000000000); 
Int_t bufsize = 25600;
Int_t split=0;

TBRaw* R[MAX_DET];
 for (j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBGeo* g = (TBGeo*) fListOfDet->At(j);
      R[j] = (TBRaw*) fListOfRaw->At(j);
      sprintf(name,"raw_%d",g->GetIdAcq(0));
      fT->Branch(name, "TBRaw", &R[j], bufsize,split);
    }

fT->Print();
if (fT == NULL || fTOut == NULL) return;

  // Loop on files and found the run
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);

      ier = this->GetNextEvent();
      if (ier == -1) continue;

      printf("Reading file %d %d for %d wanted \n",i,
             fCurEvent->GetEventHeader()->GetRun() ,nrun);
      if ( fCurEvent->GetEventHeader()->GetRun()  != nrun 
           && nrun !=0 ) continue;
      break;
    }

  if (fCurRun == NULL) return;
  if ( fCurEvent->GetEventHeader()->GetRun() != nrun) return;
  TBEventHeader* head= fCurEvent->GetEventHeader();
  head->Dump();
  fT->Branch("Header","TBEventHeader",&head,bufsize,0);

  fT->Print();


  this->ReadFile(i);
      // Loop on all events 
  nentries = 0;
  ier= 0;
  while (ier !=-1)
    {     
      ier= this->GetNextEvent();
      if (ier == -1) break;
      if (ier == -2) continue;
      nentries++;
      if (nentries%101 == 0) printf("%d %d is read \n", 
                                    fCurEvent->GetEventHeader()->GetRun(), 
                                    fCurEvent->GetEventHeader()->GetEvent() );
      if ( fCurEvent->GetEventHeader()->GetEvent()  < fFirstEvent) continue;
      if ( fCurEvent->GetEventHeader()->GetEvent()  > fLastEvent) break;
      fT->Fill();
    }
  fTOut->cd();
  fT->Write();
  fTOut->Close();
}
