// FindFilesWithExt
// Distribu� par http://www.developpez.com
// Auteur : Ricky81 ( http://ricky81.developpez.com )
//
//---------------------------------------------------------------------------
#ifndef FindFilesWithExtH
#define FindFilesWithExtH

#include <classes.hpp>
/*---------------------------------------------------------------------------
// Liste les fichiers d'extension sp�cifi�e, dans le r�pertoire indiqu�,
// et dans tous ses sous-r�pertoires si SousRep=true
---------------------------------------------------------------------------*/
void  __fastcall ListerFichiers(TStringList *liste, AnsiString NomRep,
   AnsiString extension, bool SousRep);

#endif
