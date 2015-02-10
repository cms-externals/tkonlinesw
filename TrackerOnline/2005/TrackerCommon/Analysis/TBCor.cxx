//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBPed.h"
#include "TBRaw.h"

#include "TBCor.h"
#include "TBClu.h"
#include "TBEventHeader.h"
#include <algorithm>
#include <numeric>
#include <unistd.h>

Int_t kNumHist;
ClassImp(TBCor)

/* Corrected raw data */

/* Constructor */
TBCor::TBCor(TBGeo* g)
{
fGeo = g;
Int_t fb = g->Get1st();
Int_t lb = g->GetLast();
Int_t nb = g->GetBonded();
Int_t nbc = nb/g->GetNcm();
fIdAcq=g->GetIdAcq(0);
 fCor=NULL;

if (fCor == NULL) 
  {
    char name[256],Title[256];
    sprintf(name,"Corrected-Id%x;%d",g->GetIdAcq(0),kNumHist);
    sprintf(Title,"Corrected Data for detector with Id =%x",
            g->GetIdAcq(0));
  kNumHist++;
    fCor=new TH1F(name,Title,nb,0.1,nb+0.1);
    sprintf(name,"CMN-Id%x;%d",g->GetIdAcq(0),kNumHist);
    sprintf(Title,"Common Mode Noise for detector with Id =%x",
            g->GetIdAcq(0));
    fCmn =new TH1F(name,Title,nbc,0.1,nbc+0.1);
    }
else
  {
    fCor->Reset();
    fCmn->Reset();
  }

}
/* Fill from raw data && Pedestal */
void TBCor::Fill(TBRaw* raw, TBPed* ped, TBPed* noi,TBEventHeader* h)
{
  Int_t i,j,is,it;
  Int_t process =0;
  if (h!=NULL) process =h->GetProcess();
  //  printf("Process %x %d \n",h,process);
  fNoise= noi;
  UShort_t ns=fCor->GetNbinsX();

  TBGeo* g= fGeo;
  Float_t pol= g->GetPolarity();
  UShort_t ncm = g->GetNcm();
  Float_t vcopy[4096],cxmask[4096],vcalc[4096];
  Int_t idxsor[4096];
  // Initialize array
  Float_t noise,mean=0,median=0;
  Int_t nok=ns;
  fCmn->Reset();


  // Fill mask array and vch array
  Float_t* vch = &fCor->fArray[1];
  for (i=0;i<ns;i++)
    {
      if (noi->IsDead(i) || noi->IsNoisy(i)) 
        {cxmask[i]=0;vch[i]=0;}
      else
        {
        cxmask[i]=1.0;
	if (process<TBEventHeader::PROCESS)
	  vch[i] = (raw->GetADC(i)-ped->GetMean(i))*pol;
	else
	  vch[i] = raw->GetADC(i);
        }
    }
  if (process == TBEventHeader::CLUSTER) return;
  //First calculate on chip basis
  Int_t nchip=g->GetNcm();
  for (i=1;i<=ns/nchip;i++)
    {
      Float_t cxmask_chip[nchip];
      copy(&cxmask[(i-1)*nchip],&cxmask[i*nchip],&cxmask_chip[0]);


      // sort the chip and skip 10% higher and lower
      Int_t id1=1,id2=1,id3=0;
      copy(&vch[(i-1)*nchip],&vch[i*nchip],&vcopy[0]);
      sort(&vcopy[0],&vcopy[nchip]);
      median=vcopy[nchip/2];
      float ncx=nchip;
      for (Int_t il=0;il<nchip/10;il++)
        {
	  vcopy[il]=0;
	  vcopy[nchip-il-1] = 0;
	  ncx-=2;
        }

      // Count living strips
      for (Int_t il=0;il<nchip;il++)
	if (cxmask_chip[il]==0) ncx-=1;
      mean  = accumulate(&vcopy[0],&vcopy[nchip],(float)0.0)/ncx;

      for (int j =0;j<ncm;j++) vch[(i-1)*ncm+j] -=(mean*cxmask[(i-1)*ncm+j]);

      fCmn->Fill((i+1)*1.,mean);
    }

  fCor->SetMinimum(-100.);
  fCor->SetMaximum(100.);


}

/* Build an array of TBClu */
void TBCor::BuildClusters(TObjArray* f)

{
TBGeo* g = fGeo; 
TBPed* noi= fNoise;
Int_t i,j;
UShort_t nstrips = g->GetBonded();
// Build pre-cluster
Int_t* low = new Int_t[nstrips];
Int_t* high = new Int_t[nstrips];
Int_t nclus = 0;
// if (g->GetIdLocal() == 0x7716)
//   printf("%d NSTRIP-========================== %d \n",g->GetIdLocal());
for (i=0;i<nstrips;i++)
  {
    // if (g->GetIdLocal() == 0x7716)
    //  printf("%d %d %d %f %f \n",i,noi->IsDead(i), noi->IsNoisy(i),fCor->GetBinContent(i+1),noi->GetRMS(i));
    if (noi->IsDead(i) || noi->IsNoisy(i)) 
      {
	//	printf("Dead or noisy noise %d %f \n",i,noi->GetRMS(i));
	continue;
      }

    if (noi->GetRMS(i) ==0)
      {
	//	printf("no noise %d %f \n",i,noi->GetRMS(i));
	continue;
      }
    if (fCor->GetBinContent(i+1) < g->GetCluSeed()*noi->GetRMS(i)) continue;
    Bool_t Append = kFALSE;
    
    // printf("j=%d,%f %f %f  \n",i,fCor->GetBinContent(i),
    //       g->GetCluSeed(),noi->GetRMS(i));
    for (j=0;j<nclus;j++)
      {
        if (i-high[j] == 1) { high[j] = i;Append =kTRUE; break;}
      }
    if ( !Append) 
      {
        low[nclus] = i;
        high[nclus] =i;
        nclus++;
      }
  }
// Calculate total charge and make second cuts
#ifdef DEBUG_CLUSTER
 printf("\t Det %x %d seed %f %f \n",g->GetIdLocal(),nclus,g->GetCluCut(),g->GetHighCut());
#endif
UShort_t nc=0;
for (i=0;i<nclus;i++)
  {
    Float_t clv=0;
    Float_t clnoi=0;
    Float_t clx=0;
    UShort_t ivh =0;
    Float_t vm =0;
    for (j=low[i];j<=high[i];j++)
      {
        Float_t v=fCor->GetBinContent(j+1)-noi->GetMean(j);
        clv +=v;
        clnoi +=noi->GetRMS(j)*noi->GetRMS(j);
        clx +=v*(j*1.);
       if (v>vm)
          { vm=v;ivh=j;}
      }
    clnoi =sqrt(clnoi);
    clx /=clv;
    //    printf ("%d %f %f %f \n",i,clv,clnoi,clx);
    if (clv < g->GetCluCut()*clnoi) continue;
    if (vm < g->GetHighCut()*noi->GetRMS(ivh)) continue;
    TBClu* cl= new TBClu(this,low[i],high[i],ivh,clv,clnoi,clx);
#ifdef DEBUG_CLUSTER
    printf("New cluster %f %f %f %d \n",cl->GetV(),vm,clv,high[i]-low[i]+1);
    Char_t iii[100];
    scanf("%s",&iii);
#endif
    f->Add(cl);
    nc++;
  }
#ifdef DEBUG_CLUSTER
 printf("\t Det %x %d cluster %d  \n",g->GetIdLocal(),nc,f->GetEntries());
#endif
delete low;
delete high;
}

