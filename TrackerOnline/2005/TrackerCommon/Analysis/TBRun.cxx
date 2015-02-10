//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include "TBRun.h"

ClassImp(TBRun)
/* Run */
TBRun::~TBRun()
{
  // Clean Geometry array
  /*
  if (fGeo) 
    {
      fGeo->Delete();
      delete  fGeo;
    }
  */
}
