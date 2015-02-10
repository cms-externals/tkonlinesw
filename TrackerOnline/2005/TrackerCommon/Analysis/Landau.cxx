{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Mon Oct 19 11:47:48 1998 by ROOT version 2.00/12)
//   from TTree nt/An example of a ROOT tree
//   found on file: /scratch/mirabito/Cluster40473.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/scratch/mirabito/Cluster40473.root");
   if (!f) {
      f = new TFile("/scratch/mirabito/Cluster40473.root");
   }
   TTree *nt = (TTree*)gDirectory->Get("nt");

//Declaration of leaves types
   Int_t           Krun;
   Int_t           Kevt;
   UInt_t          Nclus;
   UInt_t          Idet[44];
   Float_t         Vclu[44];
   Float_t         Noiclu[44];
   Float_t         Xclu[44];
   UInt_t          Nsclu[44];
   Float_t         Vhclu[44];
   Float_t         Noistr[44];
   Float_t         Vlclu[44];
   UInt_t          Istr[44];

//Set branch addresses
   nt->SetBranchAddress("Krun",&Krun);
   nt->SetBranchAddress("Kevt",&Kevt);
   nt->SetBranchAddress("Nclus",&Nclus);
   nt->SetBranchAddress("Idet",Idet);
   nt->SetBranchAddress("Vclu",Vclu);
   nt->SetBranchAddress("Noiclu",Noiclu);
   nt->SetBranchAddress("Xclu",Xclu);
   nt->SetBranchAddress("Nsclu",Nsclu);
   nt->SetBranchAddress("Vhclu",Vhclu);
   nt->SetBranchAddress("Noistr",Noistr);
   nt->SetBranchAddress("Vlclu",Vlclu);
   nt->SetBranchAddress("Istr",Istr);

//     This is the loop skeleton
//       To read only selected branches, Insert statements like:
// nt->SetBranchStatus("*",0);  // disable all branches
// nt->SetBranchStatus("branchname",1);  // activate branchname

   Int_t nentries = nt->GetEntries();

   Int_t nbytes = 0;
//   for (Int_t i=0; i<nentries;i++) {
//      nbytes += nt->GetEvent(i);
//   }
}
