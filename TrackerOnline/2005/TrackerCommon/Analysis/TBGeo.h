
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Run                                                                //
//                                                                      //
// Description of the Run and track parameters                        //
//                
// Version 2: Add ChipSize Value                                        //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TBGeo
#define ROOT_TBGeo
#include "TBDefinition.h"

#include <ostream>
using namespace std;
#define buildModuleKey(fec_inst,fec_slot,ring_slot,ccu_slot,channel)( ((fec_inst&0x3F)<<26) | ((fec_slot&0x1F)<<21) | ((ring_slot&0xF)<<17)|((ccu_slot&0XFF)<<9)| ((channel&0x1F)<<4) )

#define buildFedChannelKey(module_key,apv,opto) ((module_key&0xFFFFFFF0)|((apv&0x3)<<2)|(opto&0x3))

#define getModuleFecInstance(x) ((x>>26)&0x3F)
#define getModuleFecSlot(x) ((x>>21)&0x1F)
#define getModuleFecRing(x) ((x>>17)&0xF)
#define getModuleFecCcu(x)  ((x>>9)&0xFF)
#define getModuleFecChannel(x)  ((x>>4)&0x1F)
#define getFedChannelApv1(x)  ( 0x20+(((x>>2)&0x3)-1)*2)
#define getFedChannelApv2(x)  ( 0x20+(((x>>2)&0x3)-1)*2+1)
#define getFedChannelOpto(x)  ( (x&0x3)-1 )

// Detector Geometry and cuts



class TBGeo : public TNamed {
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
  unsigned int fDetId;
  unsigned int fFecCrate;
  unsigned int fFecSlot;
  unsigned int fFecRing;
  unsigned int fFecCcu;
  unsigned int fFecChannel;
  unsigned int fFedId;
  unsigned int fFedChannel[3];
public:
  TBGeo();
  ~TBGeo();
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
  void SetDetId(unsigned int di) {fDetId=di;}
  void SetFecCrate(unsigned int di) {fFecCrate=di;}
  void SetFecSlot(unsigned int di) {fFecSlot=di;}
  void SetFecRing(unsigned int di) {fFecRing=di;}
  void SetFecCcu(unsigned int di) {fFecCcu=di;}
  void SetFecChannel(unsigned int di) {fFecChannel=di;}
  void SetFedId(unsigned int di) {fFedId=di;}
  void SetFedChannel (int chip,   int di ) {  fFedChannel[chip] = di ;} 
  // Getters  void 

  inline int GetDead(int st){ return (fMask[st/32]>>(st%32))&0x1;}
  inline int GetFirstRun() const {  return fFirstRun ;} 
  inline int GetLastRun() const {  return fLastRun ;} 
  //  inline unsigned short GetIdLocal() const {  return (fIdAcq[0]>>16)& 0xffff ;} 
  inline int GetIdLocal() const {  int key= 
				       (getModuleFecInstance(fIdAcq[0])<<28)|
				       (getModuleFecSlot(fIdAcq[0])<<20)|
				       (getModuleFecRing(fIdAcq[0])<<16)|
				       (getModuleFecCcu(fIdAcq[0])<<8)| 
				        getModuleFecChannel(fIdAcq[0]) ;
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
  inline unsigned int GetDetId() const { return fDetId;}
  inline unsigned int GetFecCrate() const { return fFecCrate;}
  inline unsigned int GetFecSlot() const { return fFecSlot;}
  inline unsigned int GetFecRing() const { return fFecRing;}
  inline unsigned int GetFecCcu() const { return fFecCcu;}
  inline unsigned int GetFecChannel() const { return fFecChannel;}
  inline unsigned int GetFedId() const { return fFedId;}
  inline int GetFedChannel(int nc)  const {  return fFedChannel[nc] ;} 
  inline void toXML(ostream &os)
    {
      os << "<!-- "<<hex<<this->GetIdLocal() << dec <<" -->" << endl;
      os << "<TBGeo ";
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
    

  ClassDef(TBGeo,2) //Test Beam Geometry 
};
#endif
