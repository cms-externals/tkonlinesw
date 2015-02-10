#ifndef _FedPmcXmlDescription_h_
#define _FedPmcXmlDescription_h_
#include <string>
#include <vector>
#include <sstream>
/**
   \class  FedPmcXmlDescription
   \author L.Mirabito
   \date November 2002
   \version 1.0
   
   \brief Description class to remap FedProcess TShare object


*/

class senderInstanceDescription {
 private:
  string name_;
  unsigned int instance_;
 public:
  inline string getName(){return name_;}
  inline unsigned int getInstance(){return instance_;}
  inline void setName(   string n) {name_=n;}
  inline void setInstance(   unsigned int instance) {instance_=instance;}


};


class FedChannelDescription {
 private:
  unsigned int number_;
  string fecSupervisor_;
  unsigned int instance_;
  unsigned int slot_;
  unsigned int ring_;
  unsigned int ccu_;
  unsigned int i2c_;
  unsigned int apv_;
  unsigned int opto_;
  unsigned int target_;
  unsigned int fiberdelay_;
 public:
  inline unsigned int getNumber(){return number_;}
  inline string getFecSupervisor(){return fecSupervisor_;}
  inline unsigned int getInstance(){return instance_;}
  inline unsigned int getSlot(){return slot_;}
  inline unsigned int getRing(){return ring_;}
  inline unsigned int getCcu(){return ccu_;}
  inline unsigned int getI2c(){return i2c_;}
  inline unsigned int getApv(){return apv_;}
  inline unsigned int getApv1(){return apv_;}
  inline unsigned int getApv2(){return (apv_+1);}
  inline unsigned int getOpto(){return opto_;}
  inline unsigned int getFiberDelay(){return fiberdelay_;}
  inline unsigned int getTargetAddress()  {return target_;}
  inline void setNumber(   unsigned int number) {number_=number;}
  inline void setFecSupervisor(   string fecSupervisor) {fecSupervisor_=fecSupervisor;}
  inline void setInstance(   unsigned int instance) {instance_=instance;}
  inline void setSlot(   unsigned int slot) {slot_=slot;}
  inline void setRing(   unsigned int ring) {ring_=ring;}
  inline void setCcu(   unsigned int ccu) {ccu_=ccu;}
  inline void setI2c(  unsigned int i2c){i2c_=i2c;}
  inline void setApv(   unsigned int apv) {apv_=apv;}
  inline void setOpto(   unsigned int opto) {opto_=opto;}
  inline void setFiberDelay(   unsigned int del) {fiberdelay_=del;}
  inline void setTargetAddress(   unsigned int target) {target_=target;}
  void toXML(ostream &os)
    {
      os << "<FedChannel "<<
	" channel=\"" << number_ << "\"" <<
	" fec =\"" << fecSupervisor_ << "\"" <<
	" fecInstance =\"" << instance_ << "\"" <<
	" fecSlot =\"" << slot_ << "\"" <<
	" fecRing =\"" << ring_ << "\"" <<
	" ccuAddress =\"" << ccu_ << "\"" <<
	" ccuChannel =\"" << i2c_ << "\"" <<
	" apv1 =\"" << apv_ << "\"" <<
	" apv2 =\"" << apv_+1 << "\"" <<
	" opto =\"" << opto_ << "\"" <<
	" fiberDelay =\"" << fiberdelay_ << "\"" <<
	"/>" << endl;
    }
};


class GenericFedDescription {
 protected:
  string name_;
  string description_;
  unsigned int instance_;
  unsigned int maxChannels_;
  unsigned int fTarget;
  unsigned int id_;
  vector<FedChannelDescription*> channels_;
  
 public:
  inline string getName(){return name_;}
  inline string getDescription(){return description_;}
  inline unsigned int getInstance(){return instance_;}
  inline void setName(   string& n) {name_=n;}
  inline void setDescription(   string& n) {description_=n;}
  inline void setInstance(   unsigned int instance) {instance_=instance;}
  inline void setMaxChannels(   unsigned int maxchannels) {maxChannels_=maxchannels;}
  inline void setFedId(   unsigned int id) {id_=id;}
  inline void addFedChannelDescription( FedChannelDescription* f) {channels_.push_back(f);}
  // Get total number of channels
  inline unsigned int getMaxChannels(){ return maxChannels_;}
  // Gte Number of channels connected
  inline int getNumberOfChannels(){return channels_.size();}
  /// Get Channel Description
  inline FedChannelDescription* getFedChannelDescription(int i) {return channels_[i];}
  /// Get Target address
  inline   unsigned int   getTargetAddress()  {return fTarget;}
  inline   unsigned int   getFedId()  {return id_;}
  inline void setTargetAddress(unsigned int i) {fTarget=i;}
  inline bool isFedPmc(){ return (name_=="FedPmcSupervisor")|| (name_ == "TrackerFedPmc");}

  void toXML(ostream &os)
    {
      os << "<GenericFed "<<
	" name =\"" << name_ << "\"" <<
	" instance =\"" << instance_ << "\"" <<
	" channels =\"" << maxChannels_ << "\"" <<
	" description =\"" << description_ << "\"" <<
	" fedid =\"" << id_ << "\"" <<
	">" << endl;
      for (unsigned int i=0;i<channels_.size();i++)
	channels_[i]->toXML(os);
      os << "</GenericFed>" <<endl;
    }


};



class FedPmcXmlDescription : public GenericFedDescription
{

private:
  int fDevice;
  int fNumber;
  bool fExternalClock;
  bool fExternalTrigger;
  bool fScopeMode;
  int fClockDelay;
  int fSample;
  int fLowThreshold;
  int fHighThreshold;
  bool fSoftTriggerControl;
  bool fTtcTriggerControl;
  bool fTscTriggerControl;
  bool fTscInhibitTriggerControl;
  bool fDioTriggerControl;

public:
  /// Get Fed Device number
  inline   int getDevice() const {return fDevice;}
  /// Get Fed Serial number
  inline   int getNumber() const {return fNumber;}
  /// Get External  Clok flag
  inline   bool getExternalClock() const {return fExternalClock;} 
  /// Get External trigger flag
  inline   bool getExternalTrigger() const {return fExternalTrigger;} 
  /// Get Scope Mode flag
  inline   bool getScopeMode() const {return fScopeMode;}
  /// Get clock Delay
  inline   int getClockDelay() const {return fClockDelay;}
  /// Get number of Sample
  inline   int getSample() const {return fSample;}
  /// Get Low threshold for Header finding
  inline   int getLowThreshold() const {return fLowThreshold;}
  /// Get High threshold for Header finding
  inline   int getHighThreshold() const {return fHighThreshold;}
  /// Get Soft Trigger control
  inline   bool getSoftTriggerControl() const {return fSoftTriggerControl;}
  /// Get Ttc Trigger control
  inline   bool getTtcTriggerControl() const {return fTtcTriggerControl;}
  /// Get Tsc Trigger control
  inline   bool getTscTriggerControl() const {return fTscTriggerControl;}
  /// Get Tsc Inhibit Trigger control
  inline   bool getTscInhibitTriggerControl() const {
    return fTscInhibitTriggerControl;}
  inline   bool getDioTriggerControl() const {
    return fDioTriggerControl;}
  /// Set Fed Device number
  inline void setDevice(   int a )  {fDevice=a;}
  /// Set Fed Serial number
  inline void setNumber(   int a )  {fNumber=a;}
  /// Set External  Clok flag
  inline void setExternalClock(   bool a )  {fExternalClock=a;}
  /// Set External trigger flag
  inline void setExternalTrigger(   bool a )  {fExternalTrigger=a;}
  /// Set Scope Mode flag
  inline void setScopeMode(   bool a )  {fScopeMode=a;}
  /// Set clock Delay
  inline void setClockDelay(   int a )  {fClockDelay=a;}
  /// Set number of Sample
  inline void setSample(   int a )  {fSample=a;}
  /// Set Low threshold for Header finding
  inline void setLowThreshold(   int a )  {fLowThreshold=a;}
  /// Set High threshold for Header finding
  inline void setHighThreshold(   int a )  {fHighThreshold=a;}
  /// Set Soft Trigger Control
  inline void setSoftTriggerControl(   bool a )  {fSoftTriggerControl=a;}
  /// Set TTC Trigger Control
  inline void setTtcTriggerControl(   bool a )  {fTtcTriggerControl=a;}
  /// Set TSC Trigger Control
  inline void setTscTriggerControl(   bool a )  {fTscTriggerControl=a;}
  /// Set TSC Inhibit Trigger Control
  inline void setTscInhibitTriggerControl(   bool a )  { 
    fTscInhibitTriggerControl=a;}
  /// Set DIO24H Trigger Control
  inline void setDioTriggerControl(   bool a )  { 
    fDioTriggerControl=a;}


  void toXML(ostream &os)
    {
      os << "<Fed "<<
	"supervisor =\"" <<getName() << "\"" <<
	" instance =\"" << getInstance() << "\"" <<
	" device =\"" << fDevice << "\"" <<
	" clockdelay =\"" << fClockDelay << "\"" <<
	" sample =\"" << fSample << "\"" <<
	" lowthreshold=\"" << fLowThreshold << "\"" <<
	" highthreshold =\"" << fHighThreshold << "\"";

      if (fExternalClock)
	os << " externalclock =\"true\"";
      else
	os << " externalclock =\"false\"";
      
      if (fExternalTrigger)
	os << " externaltrigger =\"true\"";
      else
	os << " externaltrigger =\"false\"";
      
      if (fSoftTriggerControl)
	os<< " softtriggercontrol =\"true\"";
      else
	os<< " softtriggercontrol =\"false\"";
      
      if (fTtcTriggerControl)
	os<< " ttctriggercontrol =\"true\"";
      else
	os<< " ttctriggercontrol =\"false\"";
      
      if (fTscTriggerControl)
	os<< " tsctriggercontrol =\"true\"";
      else
	os<< " tsctriggercontrol =\"false\"";
      
      if (fTscInhibitTriggerControl)
	os<< " tscinhibittriggercontrol =\"true\"";
      else
	os<< " tscinhibittriggercontrol =\"false\"";
      
      
      os << ">" << endl;
      for (unsigned int i=0;i<channels_.size();i++)
	channels_[i]->toXML(os);
      os << "</Fed>" <<endl;
    }

 };


class FedChannelConnectionDescription {
 private:
  unsigned int fedId_;
  unsigned int fedChannel_;
  char fecSupervisor_[100];
  char fecSupervisorIP_[100];
  unsigned int fecInstance_;
  unsigned int slot_;
  unsigned int ring_;
  unsigned int ccu_;
  unsigned int i2c_;
  unsigned int apv_;
  unsigned int dcuhid_;
  unsigned int detid_;
  unsigned int fiberlength_;
  unsigned int apvpairs_;
  bool enabled_ ;
 public:
  FedChannelConnectionDescription()
    {
  dcuhid_=0;
  enabled_ = true ; 
    }
  FedChannelConnectionDescription(FedChannelConnectionDescription &e)
    {
      fedId_ = e.getFedId();
      fedChannel_ = e.getFedChannel();
      this->setFecSupervisor(e.getFecSupervisor());
      this->setFecSupervisorIP(e.getFecSupervisorIP());
      fecInstance_=e.getFecInstance();
      enabled_ = e.getEnabled() ;

    }

  inline bool getEnabled() { return enabled_; }
  inline void setEnabled( bool enabled ) { enabled_ = enabled; }
  inline unsigned int getFedId(){return fedId_;}
  inline unsigned int getFedChannel(){return fedChannel_;}
  inline string getFecSupervisor(){return string(fecSupervisor_);}
  inline string getFecSupervisorIP(){return string(fecSupervisorIP_);}
  inline unsigned int getFecInstance(){return fecInstance_;}
  inline unsigned int getSlot(){return slot_;}
  inline unsigned int getRing(){return ring_;}
  inline unsigned int getCcu(){return ccu_;}
  inline unsigned int getI2c(){return i2c_;}
  inline unsigned int getApv(){return apv_;}
  inline unsigned int getDcuHardId(){return dcuhid_;}
  inline unsigned int getDetId(){return detid_;}
  inline unsigned int getFiberLength(){return fiberlength_;}
  inline unsigned int getApvPairs(){return apvpairs_;}
  inline void setFedId(   unsigned int number) {fedId_=number;}
  inline void setFedChannel(   unsigned int number) {fedChannel_=number;}
  inline void setFecSupervisor(   string fecSupervisor) {strncpy(fecSupervisor_,fecSupervisor.c_str(),100);}
  inline void setFecSupervisorIP(   string fecSupervisorIP) {strncpy(fecSupervisorIP_,fecSupervisorIP.c_str(),100);}
  inline void setFecInstance(   unsigned int instance) {fecInstance_=instance;}
  inline void setSlot(   unsigned int slot) {slot_=slot;}
  inline void setRing(   unsigned int ring) {ring_=ring;}
  inline void setCcu(   unsigned int ccu) {ccu_=ccu;}
  inline void setI2c(  unsigned int i2c){i2c_=i2c;}
  inline void setApv(   unsigned int apv) {apv_=apv;}
  inline void setDcuHardId(   unsigned int dcu) {dcuhid_=dcu;}
  inline void setDetId(   unsigned int d) {detid_=d;}
  inline void setFiberLength(   unsigned int l) {fiberlength_=l;}
  inline void setApvPairs(   unsigned int n) {apvpairs_=n;}
  void toXML(ostream &os)
    {
      os << "<FedChannelConnection "<<
	" fedId=\"" << fedId_ << "\"" <<
	" fedChannel=\"" << fedChannel_ << "\"" <<
	" fecSupervisorIp =\"" << fecSupervisorIP_ << "\"" <<
	" fecClassName =\"" << fecSupervisor_ << "\"" <<
	" fecInstance =\"" << fecInstance_ << "\"" <<
	" fecSlot =\"" << slot_ << "\"" <<
	" fecRing =\"" << ring_ << "\"" <<
	" ccuAddress =\"" << ccu_ << "\"" <<
	" ccuChannel =\"" << i2c_ << "\"" <<
	" apvAddress =\"" << apv_ << "\"" <<
	" dcuHardId =\"" << dcuhid_ << "\"" <<
	" detId =\"" << detid_ << "\"" <<
	" fiberLength =\"" << fiberlength_ << "\"" <<
	" apvPairs =\"" << apvpairs_ << "\"" <<
//	" enabled =\"" << enabled_ << "\"" <<
	"/>" << endl;
    }
};


#endif







