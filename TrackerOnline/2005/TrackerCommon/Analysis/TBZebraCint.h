/********************************************************************
* TBZebraCint.h
********************************************************************/
#ifdef __CINT__
#error TBZebraCint.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableTBZebraCint();
extern void G__cpp_setup_inheritanceTBZebraCint();
extern void G__cpp_setup_typetableTBZebraCint();
extern void G__cpp_setup_memvarTBZebraCint();
extern void G__cpp_setup_globalTBZebraCint();
extern void G__cpp_setup_memfuncTBZebraCint();
extern void G__cpp_setup_funcTBZebraCint();
extern void G__set_cpp_environmentTBZebraCint();
}


#include "TROOT.h"
#include "TMemberInspector.h"
#include "TBZana.h"
#include "TBLinkZebra.h"

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__TBZebraCintLN_TClass;
extern G__linked_taginfo G__TBZebraCintLN_TObject;
extern G__linked_taginfo G__TBZebraCintLN_TNamed;
extern G__linked_taginfo G__TBZebraCintLN_TBBuilder;
extern G__linked_taginfo G__TBZebraCintLN_TBZana;

/* STUB derived class for protected member access */
