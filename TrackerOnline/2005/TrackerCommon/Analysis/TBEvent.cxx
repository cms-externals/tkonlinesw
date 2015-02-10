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

#include "TBEvent.h"

#include <unistd.h>
ClassImp(TBEvent)

/* main Event */
/*

Event constructor

1) Default

2) read TBRun Geometry list and build arrays accordingly


 */

TBEvent::TBEvent()
{
  fRun =NULL;
  fRaws = NULL;
  fCors = NULL;
  fClus=NULL;
  fEventHeader = NULL; 

}


TBEvent::TBEvent(TBRun* r)
{
  fRun =r;
  Int_t i;
  fRaws =new TObjArray(MAX_DET);
  fCors =new TObjArray(MAX_DET);
  fClus =new TObjArray(MAX_DET);
  printf("I will create a new event %d \n",r->GetNbdet());
  for (i=0;i<r->GetNbdet();i++)
    { 
    printf(" New det %d \n",i);
    TBRaw* raw = new TBRaw(r->GetGeo(i));fRaws->AddLast(raw);
    TBCor* cor = new TBCor(r->GetGeo(i));fCors->AddLast(cor);}
  fEventHeader = new TBEventHeader();
}

/*
  Destructeur
 */
TBEvent:: ~TBEvent()
{

  if (fRun!=NULL) delete fRun;
  if (fEventHeader!=NULL) delete fEventHeader;
  if (fRaws!=NULL)
    {
      fRaws->Delete();
      delete fRaws;
    }
  if (fCors!=NULL)
    {
      fCors->Delete();
      delete fCors;
    }
  if (fClus!=NULL)
    {
      fClus->Delete();
      delete fClus;
    }
}
