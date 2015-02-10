//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
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
#include "TBRuAna.h"
#include "TObjectTable.h"
#include "TNtuple.h"
#include "TMath.h"
#include <unistd.h>

ClassImp(TBRuAna)

TBRuAna::TBRuAna()
{
 // Init
 Int_t i;
 fCurRun =new TBRun();
 fCurEvent =new TBEvent();
 fCurEvent =NULL;
 fCurFile =NULL;
 fNfiles=0;
  for (i=0;i<MAX_FILES;i++)
    {
      fIsOpen[i] = kFALSE;
      fRootFile[i] = NULL;
    }
}

TBRuAna::~TBRuAna()
{
  if (fCurFile) fCurFile->Close();
  Int_t i,lun;
  for (i=0;i<fNfiles;i++)
    {
      fIsOpen[i] = kFALSE;	
    }
  if(fCurRun!=NULL) delete fCurRun;
}
void TBRuAna::AddFile(Char_t *c)
{
  strcpy(&fListOfFiles[fNfiles][0],c);
  fRootFile[fNfiles] = new TFile(c);
  fRootFile[fNfiles]->cd();
  fRootFile[fNfiles]->ls();
  fIsOpen[fNfiles] = kTRUE;
  fNfiles++;
}

static TBRaw* R[MAX_DET];
static TBEventHeader* head;
static TTree* gTBTree;
void TBRuAna::ReadFile(Int_t i)
{
  if (fListOfRU == NULL) return;
 printf("I will open %s \n",fListOfFiles[i]);
  if (fIsOpen[i])
   {
    fRootFile[i]->cd();
    iCurFile = i;
    iCurEvent=0;
   }
  else
    {
      return;
    }

  //
  // if(fCurRun!=NULL) delete fCurRun;
  //  if(fCurEvent!=NULL) delete fCurEvent;
  
  fTB = (TTree*) fRootFile[i]->Get("TRU");
  fTB->Clear();
  fTB->Print();
  
  printf("Number of Entries %f \n",fTB->GetEntries());


  Char_t name[256];
  for (Int_t j=0;j<fListOfRU->GetEntriesFast();j++)
    {
      TBRU* r = (TBRU*) fListOfRU->At(j);
      sprintf(name,"RU_%x",r->getInstance());

      fTB->SetBranchAddress(name,&r);
      printf("%d %x %s \n",j,r,name);
    }

  fCurEvent = new TBEvent();
  head = new TBEventHeader();
  fCurEvent->SetEventHeader(head);

 // fTB->SetBranchAddress("Header",&head);

  return;
}

Int_t TBRuAna::GetNextEvent()
{
  if (fListOfRU == NULL) return -1;
  Int_t i,j,ier;
  Char_t name[256];
  if (!fIsOpen[iCurFile]) return -1;
//  printf("get next event %x \n",fCurRun);
  fRootFile[iCurFile]->cd();

//   TBranch *bheader = fTB->GetBranch("Header");
   //   printf("%x header branch \n",bheader);
//   bheader->SetAddress(&head);
   
//   bheader->GetEntry(iCurEvent);

   TBranch* rub[MAX_RU];
   TBranch* rubs[MAX_RU];
  for (i=0;i<fListOfRU->GetEntriesFast();i++)
    {
      TBRU* r = (TBRU*) fListOfRU->At(i);



      char sizename[256];
      char arrayname[256];
      char sizetype[256];
      char arraytype[256];
      sprintf(sizename,"size_RU_%x",r->getInstance());

      
      sprintf(arrayname,"RU_%x",r->getInstance());
      


      rubs[i] = fTB->GetBranch(sizename);
      rubs[i]->SetAddress(&r->fSize);
      ier = rubs[i]->GetEntry(iCurEvent);

      rub[i] = fTB->GetBranch(arrayname);
      rub[i]->SetAddress(r->fBuffer);

      ier = rub[i]->GetEntry(iCurEvent);

    }



  if(ier<=0)
   {
        printf("End of file \n");
   }
  else
    {;}
  fCurRun->SetRun(head->GetRun());

  iCurEvent++;

  return ier;
}
