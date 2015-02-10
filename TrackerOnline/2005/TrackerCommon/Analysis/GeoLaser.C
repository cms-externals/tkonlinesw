#define buildModuleKey(fec_inst,fec_slot,ring_slot,ccu_slot,channel)( ((fec_inst&0x7F)<<25) | ((fec_slot&0xF)<<21) | ((ring_slot&0xF)<<17)|((ccu_slot&0XFF)<<9)| ((channel&0x1F)<<4) )

#define buildFedChannelKey(module_key,apv,opto) ((module_key&0xFFFFFFF0)|((apv&0x3)<<2)|(opto&0x3))
TBGeo* GetGeo(Int_t i)
{
  printf("i %d \n",i);
  TBGeo* g =new TBGeo();
  

  g->Set1st(1);
  g->SetLast(512);
      
    
  g->SetNChips(2);	
  if ( i == 1) 
  {
	  int idacq=buildModuleKey(0,0,0,21,27);
	  printf("%d \n",idacq);
   g->SetIdAcq(0,buildFedChannelKey(idacq,1,1));
   g->SetIdAcq(1,buildFedChannelKey(idacq,3,3));
   
   } 
  g->SetPolarity (1);
  g->SetPitch (0.02);
  g->SetDirection (1);
  g->SetNcm (32);
  g->SetCluSeed(1.8);
  g->SetCluCut(4.);

  g->SetPedDead(0.1);
  g->SetPedNoise(17.);
  g->SetNoiDead(1.);
  g->SetNoiNoise(3.);

  return g;
}

