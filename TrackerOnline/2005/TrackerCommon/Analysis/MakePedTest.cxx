TBRana* MakePed()
{

TBRana* ana = new TBRana();


TBGeo* g1 = GetGeo(1);

ana->AddDet(g1);
printf("Detector Position 1 is added\n");

Char_t tmp[256];
 int i = 23714;
 ana->AddFile("/home/pipo/scratch/sir0020778_000.root");


printf("%s \n",tmp);
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
ana->SetLastEvent(2000000);
//ana->BuildCluster(30053);


timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

return ana;

}

