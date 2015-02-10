//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBPed.h"
#include "TBCor.h"
#include "TBClu.h"
ClassImp(TBClu)

/*Clusters */
/* Fill it */
TBClu::TBClu(TBCor* c, UShort_t f, UShort_t l,UShort_t h, 
        Float_t V, Float_t N,Float_t x)
{
  fCor =c;
  fFirstStrip=f;
  fLastStrip =l;
  fHighest =h;
  fV = V;
  fNoise = N;
  fXLocal = x;
}

