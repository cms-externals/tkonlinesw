///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                     
//
// RMain                                                               
//
//                                                                     
//
// Main program used to create ROOT application.                       
//
//                                                                     
//
//////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TRint.h"


extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

TROOT root("Rint","The ROOT Interactive Interface", initfuncs);

//______________________________________________________________________________
int main(int argc, char **argv)
{
   TRint *theApp = new TRint("Root_Rint", &argc, argv, 0, 0);
 //      TApplication *theApp= new TApplication("App", &argc, argv);
   // Init Intrinsics, build all windows, and enter event loop
   theApp->Run();

   delete theApp;

   return(0);
}

