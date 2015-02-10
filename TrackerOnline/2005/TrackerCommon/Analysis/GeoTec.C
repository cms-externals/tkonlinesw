
#define buildModuleKey(fec_inst,fec_slot,ring_slot,ccu_slot,channel)( ((fec_inst&0x7F)<<25) | ((fec_slot&0xF)<<21) | ((ring_slot&0xF)<<17)|((ccu_slot&0XFF)<<9)| ((channel&0x1F)<<4) )

#define buildFedChannelKey(module_key,apv,opto) ((module_key&0xFFFFFFF0)|((apv&0x3)<<2)|(opto&0x3))

#define getModuleFecInstance(x) ((x>>25)&0x7F)
#define getModuleFecSlot(x) ((x>>21)&0xF)
#define getModuleFecRing(x) ((x>>17)&0xF)
#define getModuleFecCcu(x)  ((x>>9)&0xFF)
#define getModuleFecChannel(x)  ((x>>4)&0x1F)
#define getFedChannelApv1(x)  ( 0x20+(((x>>2)&0x3)-1)*2)
#define getFedChannelApv2(x)  ( 0x20+(((x>>2)&0x3)-1)*2+1)
#define getFedChannelOpto(x)  ( (x&0x3)-1 )


TBGeo* GetGeo(Int_t i)
{
  printf("i %d \n",i);
  TBGeo* g =new TBGeo();
  

  g->Set1st(1);
  g->SetLast(768);
      
    
  g->SetNChips(3);	
  if ( i == 1) 
  {
	  int idacq=buildModuleKey(0,0,0,11,19);
	  printf("%d %d %d\n",idacq,getModuleFecCcu(idacq),getModuleFecChannel(idacq));
   g->SetIdAcq(0,buildFedChannelKey(idacq,1,1));
   g->SetIdAcq(1,buildFedChannelKey(idacq,2,2));
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

