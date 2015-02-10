#ifndef Fed9UMoFO_HH_
#define Fed9UMoFO_HH_
#include "Fed9UUtils.hh" 



//
// class Fed9UMoFO 
// Author: Matt Noy
// Date: June 2004
// Purpose: Provides a configurable monitoring 
//          handle that enables monitoring of the Fed. 
//  
//


namespace Fed9U
{
  
  class Fed9UDevice;
  
  class Fed9UMoFO
  {
  public:
    
   
    
    //
    // Hardware monitor bit field
    //
    enum MONITOR_FLAGS{COUNTERS=1, TEMPS=2, VOLTAGES=4, TTC=8, CLOCKSRC=16, BE_STATUS=32};


    //
    // constructor for the Fed9UMoFO class. 
    // Counters, Temperature, and Voltage are all 
    // monitored by default. The class doesn't assume
    // ownership of the Fed9UDevice it points to
    //    

//     Fed9UMoFO(Fed9UDevice * pFed, DATA_DESTINATION dest, bool MonCounters=true, 
// 		   bool MonTemp=true, bool MonVolt=true);

    Fed9UMoFO(Fed9UDevice * pFed,
	      u32 Flags=COUNTERS|TEMPS|VOLTAGES|TTC|CLOCKSRC|BE_STATUS);


    ~Fed9UMoFO(){}

    //
    // populates the containers with data from the fed passed to the class
    //
    void Load();

    //
    // 
    //
    void Send(std::ostream * os);

    // <NAC date="08/05/2007"> made all const
    //<JEC date=05-02-07> add get methods for private member variables for monitoring interface to Fed9USupervisor
    inline Fed9UCounters getCounterValue() const{return mFedCounters;}
    inline std::vector<Fed9UTempControlInfo> getTemperatureValues() const{return mTempInfo;}
    inline Fed9UVoltageControlInfo getVoltageValues() const{return mVoltageInfo;}
    inline Fed9UTtcrxDescriptionInfo getTTCRxInfo() const{return mTtcInfo;}
    inline Fed9UClockSource getClockSourceInfo() const{return mClkSource;}
    //</JEC>
    // </NAC>
    // <NAC date="07/05/2007"> added get method to get TempControlInfo for an FPGA by giving its address
    Fed9UTempControlInfo getTemperatureValues(const Fed9UAddress& fpga) const;
    // </NAC>

    //
    // get the monitor state
    //
    inline bool getCountersFlag()const{return mMonitorFlags&COUNTERS;}
    inline bool getTemperatureFlag()const{return mMonitorFlags&TEMPS;}
    inline bool getVoltageFlag()const{return mMonitorFlags&VOLTAGES;}    
    inline bool getTtcFlag()const{return mMonitorFlags&TTC;}
    inline bool getClockSrcFlag()const{return mMonitorFlags&CLOCKSRC;}
    
    //
    // get all the flags at once
    //
    inline u32 getMonitorFlags()const{return mMonitorFlags;}


    //
    // set the monitor state individually
    //
    inline void setCountersFlag(bool c){c?mMonitorFlags|=COUNTERS:mMonitorFlags&=~COUNTERS;}
    inline void setTemperatureFlag(bool t){t?mMonitorFlags|=TEMPS:mMonitorFlags&=~TEMPS;}
    inline void setVoltageFlag(bool v){v?mMonitorFlags|=VOLTAGES:mMonitorFlags&=~VOLTAGES;}
    inline void setTtcFlag(bool ttc){ttc?mMonitorFlags|=TTC:mMonitorFlags&=~TTC;}
    inline void setClockSrcFlag(bool cs){cs?mMonitorFlags|=CLOCKSRC:mMonitorFlags&=~CLOCKSRC;}
    
    //
    // set all flag at once
    //
    inline void setMonitorFlags(u32 Flags){mMonitorFlags=Flags;}    

  private:

    Fed9UMoFO(){}
    Fed9UMoFO(const Fed9UMoFO & m){}
    //operator=();

  private:


    //
    // configuration parameters
    //
    Fed9UDevice * mpFed;
    u32 mMonitorFlags;
    bool mHasNewData;
       

    //
    // data containers
    //
    Fed9UCounters mFedCounters;
    //Vector of Fed9UTempControlInfos. First FEUNITS_PER_FED are for FE units ordered by internal numbering. Next is BE them VME. (NAC 09/05/2007)
    std::vector<Fed9UTempControlInfo> mTempInfo;
    Fed9UVoltageControlInfo mVoltageInfo;
    Fed9UTtcrxDescriptionInfo mTtcInfo;
    Fed9UClockSource mClkSource;
    u32 mBeStatus;


  };

} // namespace Fed9U

#endif // Fed9UMoFO_HH_
