

#ifndef _DetGeo_H
#define _DetGeo_H


#include <ostream>
using namespace std;

// Detector Geometry and cuts



class DetGeo  {
private:
  int fFirstRun; //First run
  int fLastRun; //Last run
  int fIdAcq[3]; //ID Acquistion
  unsigned short fNChips; //Number of strips in acquisition
  unsigned short f1st; // First bonded strip
  unsigned short fLast; // Last bonded strip
  unsigned short fType; // Internal Type of detector (reserved)
  short fPolarity; // Signal Polarity
  short fDirection; // Readout direction
  float fPitch; // Readout Pitch
  float fDx; // X Distance to 0 
  float fDy; // Y Distance to 0 
  float fTilt; // X,Y angle
  float fDz; // Z position
  unsigned short fNcm; // Number of strips in Common mode calculation
  float fPedDead; // Cut on Pedestal rms to declare DEAD strips
  float fPedNoise;// Cut on Pedestal rms to declare Noisy strips
  float fNoiDead; // Cut on Ped-CMN rms to declare DEAD strips
  float fNoiNoise;// Cut on Ped-CMN rms to declare Noisy strips
  float fCluSeed; // Cut on noise for seed strips of cluster
  float fHighCut; // Cut on noise for Highest strips of cluster
  float fCluCut;  // Cut on cluster noise
  int   fMask[24]; // Mask
  int   fChipSize; //Chip Size (280 by default)
  unsigned int fDcuId;
public:
  DetGeo(){fChipSize=280;for (int i=0;i<24;i++) {fMask[i]=0xFFFFFFFF;}}

  // Setters
  void SetFirstRun (   int FirstRun ) {  fFirstRun = FirstRun ;} 
  void SetLastRun (   int LastRun ) {  fLastRun = LastRun ;} 
  void SetIdAcq (int chip,   int IdAcq ) {  fIdAcq[chip] = IdAcq ;} 
  void SetNChips (   unsigned short Nc     ) {  fNChips = Nc     ;} 
  void Set1st (   unsigned short first ) {  f1st = first ;} 
  void SetLast (   unsigned short Last ) {  fLast = Last ;} 
  void SetType (   unsigned short Type ) {  fType = Type ;} 
  void SetPolarity (   short Polarity ) {  fPolarity = Polarity ;} 
  void SetDirection (   short Direction ) {  fDirection = Direction ;} 
  void SetPitch (   float Pitch ) {  fPitch = Pitch ;} 
  void SetDx (   float Dx ) {  fDx = Dx ;} 
  void SetDy (   float Dy ) {  fDy = Dy ;} 
  void SetTilt (   float Tilt ) {  fTilt = Tilt ;} 
  void SetDz (   float Dz ) {  fDz = Dz ;} 
  void SetNcm (   unsigned short Ncm ) {  fNcm = Ncm ;} 
  void SetPedDead (   float PedDead ) {  fPedDead = PedDead ;} 
  void SetPedNoise (   float PedNoise ) {  fPedNoise = PedNoise ;} 
  void SetNoiDead (   float NoiDead ) {  fNoiDead = NoiDead ;} 
  void SetNoiNoise (   float NoiNoise ) {  fNoiNoise = NoiNoise ;} 
  void SetCluSeed (   float CluSeed ) {  fCluSeed = CluSeed ;} 
  void SetHighCut (   float HighCut ) {  fHighCut = HighCut ;}   
  void SetCluCut (   float CluCut ) {  fCluCut = CluCut ;}  
  void SetDead( int st) { fMask[st/32] &= ~(1<<(st%32));}
  void SetChipSize(int cs) {fChipSize=cs;}
  void SetDcuId(unsigned int di) {fDcuId=di;}
  // Getters
  inline int GetDead(int st){ return (fMask[st/32]>>(st%32))&0x1;}
  inline int GetFirstRun() const {  return fFirstRun ;} 
  inline int GetLastRun() const {  return fLastRun ;} 
  //  inline unsigned short GetIdLocal() const {  return (fIdAcq[0]>>16)& 0xffff ;} 
  inline int GetIdLocal() const {  int key= 
				       (GetModuleFecInstance(fIdAcq[0])<<28)|
				       (GetModuleFecSlot(fIdAcq[0])<<20)|
				       (GetModuleFecRing(fIdAcq[0])<<16)|
				       (GetModuleFecCcu(fIdAcq[0])<<8)| 
				        GetModuleFecChannel(fIdAcq[0]) ;
  // printf("IdAcq = %x IdLocal = %x \n", fIdAcq[0],key); 
  return key;} 
  inline int GetIdAcq(int nc)  const {  return fIdAcq[nc] ;} 
  inline unsigned short GetNChips() const {  return fNChips ;} 
  inline unsigned short Get1st() const {  return f1st ;} 
  inline unsigned short GetLast() const {  return fLast ;}
  inline unsigned short GetBonded() const {return fLast-f1st+1;} 
  inline unsigned short GetType() const {  return fType ;} 
  inline short GetPolarity() const {  return fPolarity ;} 
  inline short GetDirection() const {  return fDirection ;} 
  inline float GetPitch() const {  return fPitch ;} 
  inline float GetDx() const {  return fDx ;} 
  inline float GetDy() const {  return fDy ;} 
  inline float GetTilt() const {  return fTilt ;} 
  inline float GetDz() const {  return fDz ;} 
  inline unsigned short GetNcm() const {  return fNcm ;} 
  inline float GetPedDead() const {  return fPedDead ;} 
  inline float GetPedNoise() const {  return fPedNoise ;} 
  inline float GetNoiDead() const {  return fNoiDead ;} 
  inline float GetNoiNoise() const {  return fNoiNoise ;} 
  inline float GetCluSeed() const {  return fCluSeed ;} 
  inline float GetHighCut() const {  return fHighCut ;} 
  inline float GetCluCut() const {  return fCluCut ;} 
  inline int   GetChipSize() const { return fChipSize;}
  inline unsigned int GetDcuId() const { return fDcuId;}
  inline void toXML(ostream &os)
    {
      os << "<!-- "<<hex<<this->GetIdLocal() << dec <<" -->" << endl;
      os << "<DetGeo ";
      os << " IdAcq1 = \"" << this->GetIdAcq(0) << "\"";
      os << " IdAcq2 = \"" << this->GetIdAcq(1) << "\"";
      os << " IdAcq3 = \"" << this->GetIdAcq(2) << "\"";
      os << " NChips = \"" << this->GetNChips() << "\"";
      os << " ChipSize = \"" << this->GetChipSize() << "\"";
      os << " Type = \"" << this->GetType() << "\"";
      os << " Polarity = \"" << this->GetPolarity() << "\"";
      os << " Ncm = \"" << this->GetNcm() << "\"";
      os << " Direction = \"" << this->GetDirection() << "\"";
      os << " Pitch = \"" << this->GetPitch() << "\"";
      os << " Tilt = \"" << this->GetTilt() << "\"";
      os << " Dx = \"" << this->GetDx() << "\"";
      os << " Dy = \"" << this->GetDy() << "\"";
      os << " Dz = \"" << this->GetDz() << "\"";
      os << " CluSeed = \"" << this->GetCluSeed() << "\"";
     os << " HighCut = \"" << this->GetHighCut() << "\"";
      os << " CluCut = \"" << this->GetCluCut() << "\"";
      os << " PedDead = \"" << this->GetPedDead() << "\"";
      os << " PedNoisy = \"" << this->GetPedNoise() << "\"";
      os << " NoiDead = \"" << this->GetNoiDead() << "\"";
      os << " NoiNoisy = \"" << this->GetNoiNoise() << "\"";
      os << "/>" << endl;
    }

   static unsigned int BuildModuleKey( int fec_inst,int fec_slot,int ring_slot,int ccu_slot, int channel) {unsigned int key=( ((fec_inst&0x3F)<<26) | ((fec_slot&0x1F)<<21) | ((ring_slot&0xF)<<17)|((ccu_slot&0XFF)<<9)| ((channel&0x1F)<<4) );
//   printf("The builded key is %x \n",key);
   return key;
}

   static  unsigned int BuildFedChannelKey(int module_key,int apv,int opto){ return ((module_key&0xFFFFFFF0)|((apv&0x3)<<2)|(opto&0x3));}

   static unsigned int GetModuleFecInstance(int x) { return ((x>>26)&0x3F);}
   static unsigned int GetModuleFecSlot(int x) { return ((x>>21)&0x1F);}
   static unsigned int GetModuleFecRing(int x) { return ((x>>17)&0xF);}
   static unsigned int GetModuleFecCcu(int x) { return ((x>>9)&0xFF);}
   static unsigned int GetModuleFecChannel(int x) { return ((x>>4)&0x1F);}
   static unsigned int GetFedChannelApv1(int x) { return ( 0x20+(((x>>2)&0x3)-1)*2);}
   static unsigned int GetFedChannelApv2(int x) { return ( 0x20+(((x>>2)&0x3)-1)*2+1);}
   static unsigned int GetFedChannelOpto(int x) { return ( (x&0x3)-1 );}
    


};
#endif
