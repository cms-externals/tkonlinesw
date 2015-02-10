//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBRaw.h"


#include "TBPed.h"
#include "TBCor.h"
#include <unistd.h>
ClassImp(TBPed)
// Internal histogram numbering
Int_t kNumHistPed=0;

/*   Pedestal structure */
/* Constructor */
TBPed::TBPed(TBGeo* g)
{
  // Build Pedestal progile and histo of flags
  // Use internal number to avoid double declaration
  fGeo = g; 
  Int_t fb = g->Get1st();
  Int_t lb = g->GetLast();
  Int_t nb = g->GetBonded();
  
  fIdAcq=g->GetIdAcq(0);


  char name[256],Title[256];
  char namef[256],Titlef[256];
  char namec[256],Titlec[256];

  sprintf(name,"Pedestal-Id%x-%d",g->GetIdLocal(),kNumHistPed);
  sprintf(namef,"PedestalFlag-Id%x-%d",g->GetIdLocal(),kNumHistPed);
  sprintf(namec,"CommonModeNoise_Id%x_%.4d",g->GetIdLocal(),kNumHistPed);
  kNumHistPed++;
  sprintf(Title,"Pedestal for detector with Id =%x",g->GetIdLocal());
  sprintf(Titlef,"Pedestal Flag for detector with Id =%x",g->GetIdLocal());
  sprintf(Titlec,"Common Mode Noise for detector with Id =%x",g->GetIdLocal());
  fPed=new TProfile(name,Title,nb,0.1,nb+0.1,"s");
  fPedF=new TH1S(namef,Titlef,nb,0.1,nb+0.1);
  fPedC=new TH1D(namec,Titlec,200,-50.,50.);

  sprintf(name,"RMS-%s-%d",fPed->GetName(),kNumHistPed);

  sprintf(Title,"RMS-%s",fPed->GetTitle());

  fPedE = new TH1F(name,Title,nb,0.1,nb+0.1);

  fCount=0;
}
void TBPed::SetName(const char* n)
{
  TNamed::SetName(n);
  Char_t name[256];
  memset(name,0,256);
  sprintf(name,"Profile_%s",n);
  fPed->SetName(name);
}
/* Fill  from raw data (Real pedestal ) */
void TBPed::Increment(TBRaw* r)
{
  Int_t i;
  Float_t dist;
  Int_t ns = fPed->GetNbinsX();
  if (r == NULL) {printf("raw null\n");return;}
  if (r->GetHisto() == NULL) {printf("hist null\n");return;}
  Short_t* rh = &r->GetHisto()->fArray[1];
  if (rh == NULL) {printf("rh null\n");return;}
  fCount++;
  // First 5 events fill all strips
  if (fCount<25)
    for (i=0;i<ns;i++) 
      {
	//      if (i==0) printf("%d %d \n",i,rh[i]);
	fPed->Fill((i+1)*1.,rh[i]*1.);
      }
  // Suppress big deviations
  else
    {
    for (i=0;i<ns;i++) 
      {
	//  if (i==0) printf("%d %d \n",i,rh[i]);
	if (rh[i] == 0) continue;
	dist = rh[i]-fPed->GetBinContent(i+1);
	if (dist<0) dist=-dist;
	if (dist<3000*fPed->GetBinError(i+1))
	  fPed->Fill((i+1)*1.,rh[i]*1.);
      }
    for (i=1;i<=ns;i++)
      {
	fPedE->SetBinContent(i,fPed->GetBinError(i));
      }

    }
  //printf("Fin de l increment\n");
  return;
}

/* Fill from TBCOR info */
void TBPed::Increment(TBCor* c)
{
  Int_t i;
  Float_t dist;
  Int_t ns = fPed->GetNbinsX();
  Float_t* ch= &c->GetHisto()->fArray[1];
 fCount++;
 // printf("Count is %d \n",fCount);
 if (fCount<25)
   for (i=0;i<ns;i++) 
     {
       if (this->IsDead(i) || this->IsNoisy(i) ) continue;
       fPed->Fill((i+1)*1.,c->GetChannel(i),1.);
     }
 else
   {
     for (i=0;i<ns;i++) 
       {
	 if (this->IsDead(i) || this->IsNoisy(i) ) continue;
	 dist = ch[i]-fPed->GetBinContent(i+1);
	 if (dist<0) dist=-dist;
	 if (dist<3*fPed->GetBinError(i+1))
	   fPed->Fill((i+1)*1.,c->GetChannel(i),1.);
       }
     for (i=1;i<=ns;i++)
       {
	 fPedE->SetBinContent(i,fPed->GetBinError(i));
       }

   }

     //Common mode noise
 Int_t nb = fGeo->GetBonded();
 Int_t nbc = nb/fGeo->GetNcm();
 for (Int_t j=1;j<=nbc;j++)
   fPedC->Fill(c->GetCmn(j));


}






/* Flag Pedestal from a previous one */
void TBPed::Flag(TBPed* p)
{
// Flag channels already dead
Int_t i;

Int_t nb = fPed->GetNbinsX();

for (i=1;i<=nb;i++)
  {
    if (p->IsDead(i)) this->FlagDead(i);
    if (p->IsNoisy(i)) this->FlagNoisy(i);
  }

}
void TBPed::Info()
{
// Flag channels already dead
Int_t i;
printf("Dump for Pedestal: %s \n \n",fPed->GetTitle());
Int_t nb = fPed->GetNbinsX();
Float_t meannoise=0;
Int_t nok=0;
for (i=1;i<=nb;i++)
  {
    if (this->IsDead(i)) printf("\t Strip %d is dead \n",i);
    if (this->IsNoisy(i)) printf("\t \t Strip %d is noisy \n",i);
    if (this->IsNoisy(i) ||this->IsDead(i)  ) continue;

    nok++;
    meannoise+=fPed->GetBinError(i);
  }
printf("Number of good channels : %d \n",nok);
if (nok > 0) printf("Mean Noise  : %f \n",meannoise/nok); 
}
/* Build Histograms of RMS */
TH1F* TBPed::GetHistoRMS()
{

  return fPedE;
// Flag channels already dead
Int_t i;

Int_t nb = fPed->GetNbinsX();
char name[256],Title[256];
sprintf(name,"RMS-%s-%d",fPed->GetName(),kNumHistPed);
kNumHistPed++;
sprintf(Title,"RMS-%s",fPed->GetTitle());

TH1F* r = new TH1F(name,Title,nb,0.1,nb+0.1);
for (i=1;i<=nb;i++)
  {
    r->SetBinContent(i,fPed->GetBinError(i));
  }

return r;
}

