#include <inttypes.h>
#include <stdint.h>
#include "Fed9UMoFO.hh"
#include "Fed9UDevice.hh"
#include "TypeDefs.hh"

// #include "log4cplus/logger.hh"

namespace Fed9U
{
  using std::hex;

// 				 bool MonTemp=true, bool MonVolt=true):
  Fed9UMoFO::Fed9UMoFO(Fed9UDevice * pFed, 
		       u32 Flags):
    mpFed(pFed),
    mMonitorFlags(Flags),
    mHasNewData(false),
    mBeStatus(0)
  {
    mTempInfo.reserve(FEUNITS_PER_FED+2);
  }


  //
  // Downloads the status as requested by the flags from the fed
  // pointed to by mpFed.
  //
  void Fed9UMoFO::Load()
  {
    if(!mMonitorFlags) // return if nothing is to be done
      return;
        
    //
    // readout the fed counters
    //
    //cout << "About to getCounters in MoFo.Load()" << endl;
    if(mMonitorFlags&COUNTERS){ 
      mFedCounters=mpFed->getCounters();
    }
    
    //
    // readout the temperatures from all possible places
    // probably don't want any finer control than this. 
    //
    if(mMonitorFlags&TEMPS){ // then read the temperature 
      Fed9UAddress addr;
      // <NAC date="07/05/2007"> Changed so that only current temperature is stored, not all previously loaded temperatures
      // Also used constants from Fed9UAddress for BE and VME FPGA numbers
      std::vector<Fed9UTempControlInfo> newTempInfo;
      newTempInfo.reserve(FEUNITS_PER_FED+2);
      for(unsigned short fpga=0;fpga<FEUNITS_PER_FED; fpga++)
	{
	  addr.setFedFpga(fpga);
      //cout << "About to TempControlInfo for Fpga " << (u16)fpga << " in MoFo.Load()" << endl;
	  newTempInfo.push_back(mpFed->getTempControlInfo(addr));
	}
      
      addr.setFedFpga(Fed9UAddress::BACKEND); // BE fpga
      //cout << "About to getTempControlInfo for BACKEND in MoFo.Load()" << endl;
      newTempInfo.push_back(mpFed->getTempControlInfo(addr));
      
      addr.setFedFpga(Fed9UAddress::VME); // VME fpga
      //cout << "About to getTempControlInfo for VME in MoFo.Load()" << endl;
      newTempInfo.push_back(mpFed->getTempControlInfo(addr));
      
      mTempInfo = newTempInfo;
      // </NAC>

    }
    
    //
    // readout the fed voltage monitors
    //
    if(mMonitorFlags&VOLTAGES){
      //cout << "About to getVoltageMonitorInfo in MoFo.Load()" << endl;
      mVoltageInfo=mpFed->getVoltageMonitorInfo();      
    }

    //
    // readout the TTCrx chip status 
    //
    if(mMonitorFlags&TTC){
      //cout << "About to getTtcrxInfo in MoFo.Load()" << endl;
      if((mClkSource=mpFed->getClock())==FED9U_CLOCK_TTC){
	    mTtcInfo=mpFed->getTtcrxInfo();      
      }      
    }

    //
    // if ttc monitoring not required, but clock src integrity is, 
    // require a read from the device, otherwise, this is done anyway
    // by the TTC monitoring. 
    //
    if(mMonitorFlags&CLOCKSRC){
      //cout << "About to getClock() in MoFo.Load()" << endl;
      mClkSource=mpFed->getClock();
    }


    //
    // 
    // 
    if(mMonitorFlags&BE_STATUS){
      //cout << "About to getBeStatusRegister() in MoFo.Load()" << endl;
      mBeStatus=mpFed->getBeStatusRegister();
      
    }
 
    mHasNewData=true;
  }
  
  // <NAC date="07/05/2007"> added get method to get TempControlInfo for an FPGA by giving its address
  Fed9UTempControlInfo Fed9UMoFO::getTemperatureValues(const Fed9UAddress& fpga) const
  {
    if (fpga.getFedFpga() == Fed9UAddress::VME) return mTempInfo[FEUNITS_PER_FED+1];
    else if (fpga.getFedFpga() == Fed9UAddress::BACKEND) return mTempInfo[FEUNITS_PER_FED];
    else return mTempInfo[fpga.getFedFeUnit()];
  }
  // </NAC>

  void Fed9UMoFO::Send(ostream * os)
  {
    //
    // if Send is call and no new data as been added, then return.
    //
    if(!mHasNewData){
      return;
    }

    
    //
    // prepare the message, based on the flags set in the configuration. 
    //

    std::ostringstream oss;
    if(mMonitorFlags&COUNTERS){
      oss<<"-------- Counters --------\n";
      oss<<mFedCounters<<"\n";
    }
    if(mMonitorFlags&TEMPS){
      oss<<"------ Temperatures -------\n";
      for(std::vector<Fed9UTempControlInfo>::const_iterator i=mTempInfo.begin(); i!=mTempInfo.end(); i++){
	oss<<(*i)<<"\n";
      }
    }
    if(mMonitorFlags&VOLTAGES){
      oss<<"-------- Voltages --------\n";
      oss<<mVoltageInfo<<"\n";
    }

    if(mMonitorFlags&TTC){
      oss<<"-------- TTC --------\n";
      if(mpFed->getClock()==FED9U_CLOCK_TTC){
	oss<<mTtcInfo<<"\n";
      }else{
	oss<<"TTC clock not selected\n";
      }
    }

    
    if(mMonitorFlags&BE_STATUS){
      oss<<"---- BE Status Register ----\n";
      oss<<hex<<mBeStatus<<dec<<"\n";	
    }
    
    if(mMonitorFlags&CLOCKSRC){
      if(mClkSource!=mpFed->getFed9UDescription().getClock()){
	
	std::cerr<<" *** ATTENTION: Clock Source Mismatch, FED may need to be reinitialised *** "
		 <<std::endl;
	

      }
    }
    


    // send the message to the selected output
    //
    (*os)<<oss.str();
    
    mHasNewData=false;
  }
  

} // namespace Fed9U
