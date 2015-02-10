//*CMZ :  0.90/13 29/01/97  14.26.37  by  Rene Brun
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include <stdio.h>
#include "TBGeo.h"
#include <unistd.h>
ClassImp(TBGeo)
/* Run */
TBGeo::TBGeo() {fChipSize=280;for (int i=0;i<24;i++) {fMask[i]=0xFFFFFFFF;}}
TBGeo::~TBGeo(){;}
