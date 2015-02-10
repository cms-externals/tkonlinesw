//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBRaw.h"
#include <unistd.h>
ClassImp(TBRaw)
Int_t kNumRaw=0;
/*    Raw Data */
/* 

Constructor 
Only store the physical part
*/
TBRaw::TBRaw(TBGeo* g)
{
  fGeo = g; 
  Int_t fb = g->Get1st();
  Int_t lb = g->GetLast();
  Int_t nb = g->GetBonded();
  
  Int_t nc=g->GetNChips();
  Int_t id=g->GetIdAcq(0);
  Int_t ccu = id>>24 &0XFF;
  Int_t ch = id>>16 &0XFF;
  Int_t cs = g->GetChipSize();
  char name[256],Title[256];
  sprintf(name,"ccu%d-%d;%d",ccu,ch,kNumRaw);
  kNumRaw++;
  sprintf(Title,"Raw data for detector CCU %d Channel %d ",ccu,ch);
  fADC=new TH1S(name,Title,nb,0.1,nb+0.1);
  for (Int_t j=0;j<3;j++)
    {
      if (j<nc)
	{
	  id=g->GetIdAcq(j);
	  Int_t apvs=id & 0xFFFF;
	  sprintf(name,"ccu%d-%d-0x%x;%d",ccu,ch,apvs,kNumRaw);
	  kNumRaw++;
	  sprintf(Title,"Reorder data for detector CCU %d Channel %d APVS 0x%x ",ccu,ch,apvs);
	  fChip[j]=new TH1S(name,Title,cs,0.1,cs+0.1);
	}
      else
	fChip[j]=NULL;
    }
}

/* Fill from fortran */
void TBRaw::Fill(Int_t c,UShort_t* data)
{
  Int_t shift=1;
  fChip[c]->Reset();
  for (int i=0;i<fGeo->GetChipSize();i++)
    {
      //LM 8/10/02      fChip[c]->SetBinContent(i+1,data[i]);
      fChip[c]->SetBinContent(shift+i,data[i]);
    }
}


void TBRaw::ReOrder()
{
  this->ReOrder(0);
}
void TBRaw::ReOrder(Int_t process)
{
  if (fGeo->GetChipSize()!=280) return;
  Int_t shift=1;
  fApvErr=0;
  for (Int_t c=0;c<fGeo->GetNChips();c++)
    {
      UShort_t fr1[128],fr2[128];
      for (int j=0;j<256;j++)
	{
	  // LM 8/10/02
	  //	  if (j%2==0)
	  //  fr1[j/2]= fChip[c]->fArray[1+j+24];
	  // else
	  //  fr2[j/2]= fChip[c]->fArray[1+j+24];
	  if (process<2)
	    {
	      if (j%2==0)
		fr1[j/2]= fChip[c]->fArray[shift+j+24];
	      else
		fr2[j/2]= fChip[c]->fArray[shift+j+24];
	    }



	}
      //      printf("%d 1\n",c);
      // getchar();
      for (Int_t j=0;j<128;j++)
	{
	  Int_t l=j;
	  if (process<2) 
	    {
	      l = 32*(j%4)+8*(j/4)-31*(j/16);
	      fADC->SetBinContent(shift+c*256+l,fr1[j]);
	      fADC->SetBinContent(shift+c*256+l+128,fr2[j]);
	    }
	  else
	    {
	      fADC->SetBinContent(shift+c*256+l,fChip[c]->fArray[shift+j+24]);
	      fADC->SetBinContent(shift+c*256+l+128,fChip[c]->fArray[shift+j+24+128]);
	    }
	}

      // Address
      // 1st Find the base and tick
      float base=99999;
      float tick=-99999;
      for (int k=0;k<24;k++)
	{
	  int adb=k/2;
	  int iad =(k%2==0)?c*2:c*2+1;
	  if (fChip[c]->fArray[shift+k]>tick) 
	    tick=fChip[c]->fArray[shift+k];
	  if (fChip[c]->fArray[shift+k]<base) 
	    base=fChip[c]->fArray[shift+k];
	}
      
      // Find the adress
      float thresh=256;
      if (tick-base>10) thresh= (base+tick)/2.;
      //      printf("%f %f %f \n",base,tick,thresh);
      fApvAdr[c*2] =0;
      fApvAdr[c*2+1] =0;
      for (int k=6;k<22;k++)
	{
	  int adb=(k-6)/2;
	  int iad =(k%2==0)?c*2:c*2+1;
	  if (fChip[c]->fArray[shift+k]>thresh) fApvAdr[iad]|=1<<adb;
	}
      //Error bit
      if (fChip[c]->fArray[shift+22]>thresh) fApvErr |=1<<(c*2);
      if (fChip[c]->fArray[shift+23]>thresh) fApvErr |=1<<(c*2+1);
      //   printf("%d %d \n",fChip[c]->fArray[shift+22],fChip[c]->fArray[shift+23]);
    }
  //  printf("%d %d \n",fADC->fArray[1],fADC->fArray[fGeo->GetNChips()*256]);
}
