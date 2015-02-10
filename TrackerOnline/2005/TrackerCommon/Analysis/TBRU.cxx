//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "TMath.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TProfile.h"
#include "TMapFile.h"

#include <iostream>


#include "TBRU.h"
#include <unistd.h>
ClassImp(TBRU)

/*    Raw Data */
/* 

Constructor 
Only store the physical part
*/



TBRU::TBRU()
{
  fInstance = -1; 
  fSize=0;

}

TBRU::TBRU(Int_t instance)
{
  fInstance = instance; 
  fSize= MAX_RU_SIZE;

}

