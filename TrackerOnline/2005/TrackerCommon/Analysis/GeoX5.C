TBGeo* GetGeo(Int_t i)
{
  printf("i %d \n",i);
  TBGeo* g =new TBGeo();
  

  g->Set1st(1);
  g->SetLast(512);
      
    
  g->SetNChips(2);	
  if ( i == 1) 
  {
   g->SetIdAcq(1,0x24014042);
   g->SetIdAcq(0,0x2401484A);
   } 
   if ( i == 2) 
  {
   g->SetIdAcq(0,0x24024042);
   g->SetIdAcq(1,0x2402484A);
   }
    if ( i == 3) 
  {
   g->SetIdAcq(0,0x24034042);
   g->SetIdAcq(1,0x2403484A);
   }
    if ( i == 4) 
  {
   g->SetIdAcq(0,0x24044042);
   g->SetIdAcq(1,0x2404484A);
   }
    if ( i == 5) 
  {
   g->SetIdAcq(0,0x24054042);
   g->SetIdAcq(1,0x2405484A);
   }
    if ( i == 6) 
  {
   g->SetIdAcq(0,0x24064042);
   g->SetIdAcq(1,0x2406484A);
   }
    if ( i == 8) 
  {
   g->SetIdAcq(0,0x761C4042);
   g->SetIdAcq(1,0x761C484A);
   }
  g->SetPolarity (1);
  g->SetPitch (0.02);
  g->SetDirection (1);
  g->SetNcm (128);
  g->SetCluSeed(1.8);
  g->SetCluCut(4.);

  g->SetPedDead(0.1);
  g->SetPedNoise(17.);
  g->SetNoiDead(1.);
  g->SetNoiNoise(3.);

  return g;
}

