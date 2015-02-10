//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBPed.h"
#include "TBEventHeader.h"
#include "TBRU.h"
#include "TBRaw.h"
#include "TBCor.h"
#include "TBClu.h"
#include "TBRun.h"
#include "TBEvent.h"
#include "TBBuilder.h"
#include "TBRana.h"
#include "TObjectTable.h"
#include "TNtuple.h"
#include "TMath.h"
#include <unistd.h>

ClassImp(TBRana)

TBRana::TBRana()
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

TBRana::~TBRana()
{
  if (fCurFile) fCurFile->Close();
  Int_t i,lun;
  for (i=0;i<fNfiles;i++)
    {
      fIsOpen[i] = kFALSE;	
    }
  if(fCurRun!=NULL) delete fCurRun;
}
void TBRana::AddFile(Char_t *c)
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
void TBRana::ReadFile(Int_t i)
{
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
  
  fTB = (TTree*) fRootFile[i]->Get("TB");
  fTB->Clear();
  fTB->Print();
  
  printf("Number of Entries %f \n",fTB->GetEntries());
  if (fListOfDet!=NULL)
    {
      printf("Number of Detector %d \n",fListOfDet->GetEntriesFast());
      
      Char_t name[256];
      for (Int_t j=0;j<fListOfDet->GetEntriesFast();j++)
	{
	  TBGeo* g = (TBGeo*)fListOfDet->At(j);
	  sprintf(name,"raw_%x",g->GetIdLocal());
	  TBRaw* r = (TBRaw*) fListOfRaw->At(j);
	  fTB->SetBranchAddress(name,&r);
	  printf("%d %x %s \n",j,r,name);
	}
    }
  fCurEvent = new TBEvent();
  head = new TBEventHeader();
  fCurEvent->SetEventHeader(head);

  fTB->SetBranchAddress("Header",&head);

  return;
}
#define TEST
static Int_t RUBUFFER[MAX_RU*96*512];

Int_t TBRana::GetNextEvent()
{
  Int_t i,j,ier;
  Char_t name[256];
  if (!fIsOpen[iCurFile]) return -1;
//  printf("get next event %x \n",fCurRun);
  fRootFile[iCurFile]->cd();
#ifdef TEST
   TBranch *bheader = fTB->GetBranch("Header");
   //   printf("%x header branch \n",bheader);
   bheader->SetAddress(&head);
   
   bheader->GetEvent(iCurEvent);
   printf("Header %d %d %d \n",iCurEvent,head->GetRun(),head->GetEvent());
   if (fListOfDet!=NULL)
     {
       TBranch* rawb[MAX_DET];
       for (i=0;i<fListOfDet->GetEntriesFast();i++)
	 {
	   TBGeo* g = (TBGeo*)fListOfDet->At(i);
	   TBRaw* r = (TBRaw*) fListOfRaw->At(i);
	   memset(name,0,256);
	   sprintf(name,"raw_%x",g->GetIdLocal());
	   rawb[i] = fTB->GetBranch(name);
	   //   printf("Branch name %s %x \n",name,rawb[i]);
	   rawb[i]->SetAddress(&r);
	   //      printf("Branch  %d %d %x \n",iCurEvent,i,r);
	   ier = rawb[i]->GetEvent(iCurEvent);
	 }
     }
   if (fListOfRU!=NULL)
     {
       TBranch* rub[MAX_RU];
       for (i=0;i<fListOfRU->GetEntriesFast();i++)
	 {
	   TBRU* r = (TBRU*) fListOfRU->At(i);
	   //r->setBuffer(&RUBUFFER[i*96*512]);
	   //	   r->setBufferSize(96*512);
	   memset(name,0,256);
	   sprintf(name,"RU_%d",r->getInstance());
	   rub[i] = fTB->GetBranch(name);
	   printf("Branch name %s %x \n",name,rub[i]);
	   rub[i]->SetAddress(&r);
	   printf("Branch  %d %d %x \n",iCurEvent,i,r);
	   ier = rub[i]->GetEvent(iCurEvent);
	 }
       
     }
   /*
  fTB->SetBranchAddress("Header",&head);
  for (i=0;i<fListOfDet->GetEntriesFast();i++)
    {
      TBGeo* g = (TBGeo*)fListOfDet->At(i);
      sprintf(name,"raw_%x",g->GetIdLocal());
      fTB->SetBranchAddress(name,&R[i]);
      printf("Branch %d %x \n",i,R[i]);
    }
  */
#endif
  //  if (iCurEvent%100 == 1) printf("lets Go ! %d \n",iCurEvent);
  //    fTB->Print();


  //  ier = fTB->GetEvent(iCurEvent);
  // getchar();

  //  printf("Go ! %d \n",ier);
  if(ier<=0)
   {
        printf("End of file \n");
//       fCurFile->Close();
//        delete fCurFile;
//        fIsOpen[iCurFile] =kFALSE;
        return -1;
   }
  else
    if (fListOfDet!=NULL)
      for (i=0;i<fListOfDet->GetEntriesFast();i++)
	{
	  TBRaw* r = (TBRaw*)fListOfRaw->At(i);
	  r->ReOrder();
	}
  
  fCurRun->SetRun(head->GetRun());

  iCurEvent++;
  //  printf("C est fini \n");
  return ier;
}
TBRU* TBRana::getRU(Int_t nrun, Int_t nevt, Int_t id)

{
  Int_t ier;
  // Loop on files and find the run/first evt
  Int_t i,j,k,l,nentries;
  // Loop on files and found the run
  for (i=0;i<fNfiles;i++)
    {
      printf("Openning new file %d \n",i);
      this->ReadFile(i);
      printf("Getting one event \n");
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
      printf("On y vacsgrthryjyto            jjjjjj \n");
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


  
      TBRU* r = (TBRU*)  fListOfRU->At(id); 
      return r;
    }
}
