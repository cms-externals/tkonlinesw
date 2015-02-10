TBGeo* GetGeo(Int_t pos)
{
  printf("Position %d \n",pos);
  TBGeo* g =new TBGeo();
  //  g->Dump();
  switch (pos)
    {
    case 4: // Pisa GG
      g->SetIdAcq(3010108);
      g->SetNstrip(512);
      g->Set1st(1);
      g->SetLast(512);
      g->SetNcm(16);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(15.);
      g->SetNoiDead(4.);
      g->SetNoiNoise(50.);
      break;

    case 5: // IBM1 + GEM (3mm)
      g->SetIdAcq(9010110);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(7.);
      g->SetPedNoise(25.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(9.);
      break;

    case 6: // IBM6 + GEM (1mm)
      g->SetIdAcq(9010612);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(40.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(9.);
      break;
      break;
    case 7: // IMEC 3
      g->SetIdAcq(4013314);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(4.);
      g->SetPedNoise(7.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(8.);
      break;

    case 8: // REOSC 4
      g->SetIdAcq(4014416);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(15.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(10.);
      break;

    case 9: // TTE 3 
      g->SetIdAcq(4012318);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(15.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(7.);
      break;

    case 10: // TTE 2
      g->SetIdAcq(4012220);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(10.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(7.);
      break;

    case 11: // TTE 6
      g->SetIdAcq(4012622);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(25.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(7.);
      break;

    case 12: // TTE 1
      g->SetIdAcq(4012124);
      g->SetNstrip(256);
      g->Set1st(1);
      g->SetLast(256);
      g->SetNcm(8);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(5.);
      g->SetPedNoise(10.);
      g->SetNoiDead(3.);
      g->SetNoiNoise(7.);
      break;

    case 14: // Mulhouse Micromegas + Gem
      g->SetIdAcq(7010128);
      g->SetNstrip(512);
      g->Set1st(1);
      g->SetLast(512);
      g->SetNcm(16);
      g->SetPolarity (-1);
      g->SetPitch (0.02);
      g->SetCluSeed(2.);
      g->SetHighCut(3.);
      g->SetCluCut(4.);
      g->SetPedDead(6.);
      g->SetPedNoise(15.);
      g->SetNoiDead(5.);
      g->SetNoiNoise(12.);
      break;

    default:
      break;
      
    }
  g->SetIdLocal(pos);
  return g;
}



