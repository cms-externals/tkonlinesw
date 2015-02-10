TBZana* MakeCluster(Int_t i,Int_t rphys,Int_t seq)
{

TBZana* ana = new TBZana();


TBGeo* g1 = GetGeo(1);
TBGeo* g2 = GetGeo(2);
TBGeo* g3 = GetGeo(3);
TBGeo* g4 = GetGeo(4);
TBGeo* g5 = GetGeo(5);
TBGeo* g6 = GetGeo(6);

ana->AddDet(g1);
ana->AddDet(g2);
ana->AddDet(g3);
ana->AddDet(g4);
ana->AddDet(g5);
ana->AddDet(g6);
printf("Detector Position 1 is added\n");
Char_t tmp[256];
 sprintf(tmp,"/datad/sir%.7d_000.dat",i);
ana->AddFile(tmp);
//return ana;
//ana->InitMapFile("/afs/in2p3.fr/group/delphi/mirabito/test.map");
//ana->SetFirstEvent(15000);
TStopwatch timer;
timer.Start();
ana->SetFirstEvent(1);
ana->SetLastEvent(20000000);
TObjArray* pedana = ana->CreatePed(i);
ana->FillPed(i,pedana);
ana->SetPedestal(pedana);
// return ana;
TObjArray* noiana = ana->CreateNoise(i);
ana->FillNoise(i,noiana);
ana->SetNoise(noiana);
ana->SetFirstEvent(0);
ana->SetLastEvent(3000000);



 for (Int_t j=0;j<=seq;j++)
   { 
     sprintf(tmp,"/datad/sir%.7d_%.3d.dat",rphys,j);
     ana->AddFile(tmp);
   }


ana->BuildCluster(rphys);


timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
return ana;

}

