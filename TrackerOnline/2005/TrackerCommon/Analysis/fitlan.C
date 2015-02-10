//extern Double_t denlan(Double_t*);
#ifndef __CINT__
#include "TROOT.h"
#include "TMath.h"
#include "TBEvent.h"
#endif
Double_t fitlan(Double_t *x,Double_t *par)
{
    /* System generated locals */
    Float_t  r__1;
    Double_t ret_val;
    /* Local variables */

    ret_val = 0.;
    r__1 = (x[0] - TMath::Abs(par[1])) / par[2];
    //    printf("%f %f %f %f \n",x[0],par[0],r__1,denlan_(&r__1));
    ret_val = par[0] * (denlan_(&r__1));
    return ret_val;
} /* fitlan*/

