//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBZana.h"
#include "TObjectTable.h"
#include "TNtuple.h"
#include "TMath.h"

#include <unistd.h>
ClassImp(TBZana)

extern "C" void zbini_();
extern "C" void zbclose_(Int_t&);
extern "C" void zbopen_(Int_t&,const Char_t *,Int_t);
extern "C" void zbgetbuffer_(Int_t&,Int_t *,Int_t&);

extern "C" void mzend_();

extern "C" Float_t rvsum_(Int_t&,Float_t*,Float_t*,Float_t*,Float_t*);
extern "C" Float_t rvmpy_(Int_t&,Float_t*,Float_t*,Float_t*,Float_t*);
extern "C" void rvmul_(Int_t&,Float_t*,Float_t*,Float_t*,Float_t*,Float_t*,Float_t*);
extern "C" void vbias_(Float_t*,Float_t&,Float_t*,Int_t&);

// Zebra case

TBZana::TBZana()
{
  zbini_();
}

TBZana::~TBZana()
{
  Int_t i,lun;
  for (i=0;i<fNfiles;i++)
    {
      lun = 22+i;
      zbclose_(lun);
      fIsOpen[i] = kFALSE;
    }
  mzend_();
}
void TBZana::ReadFile(Int_t i)
{
  Int_t lun=22+i;
  if (fIsOpen[i])
    zbclose_(lun);

  zbopen_(lun,&fListOfFiles[i][0],strlen(&fListOfFiles[i][0]));
  if (lun == 0) fIsOpen[i] = kTRUE;
  fCurFile = i;
}
Int_t TBZana::GetNextEvent()
{
  static int lastrun=-1,nevt=0;
  Int_t i,j,ier;
  if (!fIsOpen[fCurFile]) return -1;
  if  ( fCurRun==NULL)
    {
      fCurRun = new TBRun();
      
      for (i=0;i<fListOfDet->GetEntriesFast();i++)
        {
          TBGeo* g = (TBGeo*)fListOfDet->At(i);
          fCurRun->AddGeo(g);
        }
      fCurEvent = new TBEvent(fCurRun);
    }    


  Int_t lun = 22+fCurFile;
  Int_t len,buf[MAX_DET*MAX_STRIPS];
  zbgetbuffer_(lun,buf,len);
  ier = lun;
  if (ier > 2 ) return -1;
  Int_t lpil = buf[0];
  Int_t* pil = &buf[1];
  Int_t lend = len-lpil-1;
  Int_t* bufd = &buf[1+lpil];
  if (pil[0]!=lastrun)
    {
      lastrun=pil[0];
      nevt=0;
      printf("New Run %d %d\n",lastrun,pil[0]);
    }
 
  fCurRun->SetRun(pil[0]);
  if (ier !=0 ) return pil[0];
 pil[4]=nevt++; 
 fCurEvent->GetEventHeader()->Update(pil);
 //  printf("Will read %d %d %d %d %d %d \n",pil[0],pil[1],pil[2],pil[3],pil[4],nevt);

  // Now Fill the vents and the arrays
  Int_t ptr=0;
  Int_t ndet=0;
  while (ptr <lend-1)
    {
      Int_t wc= bufd[ptr];
      if (wc > 4096 || ptr+wc > len ) 
        { 
          printf("Zbread wrong Word Count %d at %d for len %d \n",wc,ptr,len);
          break;
        }
      Int_t wcl = bufd[ptr+wc-1] ^ -1;
      if (wcl != wc)
        { 
          printf("Zbread wrong Word Count Last %d , %d \n",wc,wcl);
          break;
        }
      //      printf("Found det %x \n",bufd[ptr+1]);
      //  printf("Header  %d \n",fCurEvent->GetEventHeader()->GetSpill(0));
       if (fCurEvent->GetEventHeader()->GetSpill(0) > 0)
         {
       for (j=0;j<fListOfDet->GetEntriesFast();j++)
         {
           TBGeo* g = (TBGeo*) fListOfDet->At(j);
	   //           printf(" I Look for %d %d \n",g->GetIdAcq(0),g->GetNChips());
	   for (int k=0;k<g->GetNChips();k++)
	     {
	       //	       printf("%d %d \n",k,g->GetIdAcq(k));
	       if (g->GetIdAcq(k) == bufd[ptr+1] )
		 {
		   ndet++;
		   TBRaw* r =fCurEvent->GetRaw(j);

		   r->Fill(k,(UShort_t*) &bufd[ptr+5]);

		   r= (TBRaw*) fListOfRaw->At(j); 

		   r->Fill(k,(UShort_t*) &bufd[ptr+5]);

		 }
	     }

	  
         }
         }
       else // Slow Control
         {
           switch (bufd[ptr+1]/1000000)
            {
             case 12: //Nano CAEN
                     break;
             case 13: //Nano LEcroy
                     break;
             case 14: //Scaler CAEN
                     break;
             case 15: //Scaler Struck
                     break;
             case 16: //Lecroy PM
                     break;
             default:
                     break;
            }
         }

       ptr +=wc;
    }
  for (j=0;j<fListOfDet->GetEntriesFast();j++)
    {
      TBRaw* r= (TBRaw*) fListOfRaw->At(j); 
      r->ReOrder();
      //      getchar();
    }
  if (ndet>0 && pil[0] !=0)
    return pil[1];
  else
    return -2;
}


