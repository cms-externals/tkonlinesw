#include "Fed9UFrontEndDescription.hh"

#include <iostream>

namespace Fed9U {

  //using std::endl;

  Fed9UFrontEndDescription::Fed9UFrontEndDescription(u16 fine, 
						     u16 coarse, 
						     u16 trim, 
						     u16 opto, 
						     u16 cap, 
						     bool apvDisable, 
						     //<GR date=27/07/2006> Added APV fake event disable
						     bool apvFakeEventDisable,
						     //</GR>
						     u16 median, 
						     bool medianDisable,
						     u16 channelThreshold,
						     u16 channelBufferOccupancy,
						     const Fed9UAdcControls & adcControls,
						     bool feUnitDisable,
                                                     const Fed9UTempControl & tempControl,
                                                     bool complement,
						     u16 fakeEventRandomSeed,
						     u16 fakeEventRandomMask
                                                     ) :
    _optoRxOffset(opto), _optoRxCapacitor(cap), _medianOverrideDisable(medianDisable),
    _feUnitDisable(feUnitDisable), _tempControl(tempControl)
    {
    for(int i=0; i<CHANNELS_PER_FEUNIT; i++) {
      _fineDelay[i] = fine;
      _coarseDelay[i] = coarse;
      _trimDacOffset[i] = trim;
      _channelThreshold[i] = channelThreshold;
      _channelBufferOccupancy[i] = channelBufferOccupancy;
      _adcControls[i] = adcControls;
      _complement[i] = complement;
    }
    for(int i=0; i<APVS_PER_FEUNIT; i++) {  
      _apvDisable[i] = apvDisable;
      //<GR date=27/07/2006> Added APV fake event disable
      _apvFakeEventDisable[i] = apvFakeEventDisable;
      //</GR>
      _medianOverride[i] = median;
    }  
    for (int i=0; i<CHANNELS_PER_FEUNIT/2; i++) {
      _fakeEventRandomSeed[i] = fakeEventRandomSeed;
      _fakeEventRandomMask[i] = fakeEventRandomMask;
    }
  }

  // <NAC date="24/04/2007"> added const
  bool Fed9UAdcControls::operator==(const Fed9UAdcControls& A ) const {
  // </NAC>
    return ( (A._dfsval == _dfsval) &&  (A._dfsen == _dfsen) && (A._s1 == _s1) && (A._s2 == _s2) );
  }

  std::ostream& operator<<(std::ostream& os, const Fed9UFrontEndDescription& desc) {
    os << desc._optoRxOffset << '\t' 
       << desc._optoRxCapacitor << '\t' 
       << desc._medianOverrideDisable << '\t' 
       << desc._feUnitDisable<< std::endl
       << desc._tempControl << std::endl;
    for (int i = 0; i < CHANNELS_PER_FEUNIT; i++) {
      os << desc._fineDelay[i] 
	 << '\t' 
	 << desc._coarseDelay[i] 
	 << '\t' 
	 << desc._trimDacOffset[i] 
	 << '\t' 
	 << desc._channelThreshold[i] 
	 << '\t' 
	 << desc._channelBufferOccupancy[i]
	 << '\t' 
	 << desc._adcControls[i]
         << '\t'
         << desc._complement[i]
	 << std::endl;
    }
    for (int i = 0; i < APVS_PER_FEUNIT; i++) {
      os << desc._apvDisable[i] 
	 << '\t' 
	//<GR date=27/07/2006> Added APV fake event disable
	 << desc._apvFakeEventDisable[i]
	//</GR>
	 << '\t'
	 << desc._medianOverride[i] 
	 << '\t' 
	 << std::endl;
    }
    for (int i = 0; i < CHANNELS_PER_FEUNIT/2; i++) {
      os << desc._fakeEventRandomSeed[i]
	 << '\t'
	 << desc._fakeEventRandomMask[i]
	 << std::endl;
    }
    return os;
  }

  std::istream& operator>>(std::istream& is, Fed9UFrontEndDescription& desc) {
    is >> desc._optoRxOffset 
       >> desc._optoRxCapacitor 
       >> desc._medianOverrideDisable 
       >> desc._feUnitDisable
       >> desc._tempControl;
    for (int i = 0; i < CHANNELS_PER_FEUNIT; i++) {
      is >> desc._fineDelay[i] 
	 >> desc._coarseDelay[i] 
	 >> desc._trimDacOffset[i] 
	 >> desc._channelThreshold[i]
	 >> desc._channelBufferOccupancy[i]
	 >> desc._adcControls[i]
         >> desc._complement[i];
    }
    for (int i = 0; i < APVS_PER_FEUNIT; i++) {
      is >> desc._apvDisable[i] 
	//<GR date=27/07/2006> Added APV fake event disable
	 >> desc._apvFakeEventDisable[i]
	//</GR>
	 >> desc._medianOverride[i];
    }
    for (int i = 0; i < CHANNELS_PER_FEUNIT/2; i++) {
      is >> desc._fakeEventRandomSeed[i]
	 >> desc._fakeEventRandomMask[i];
    }
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const Fed9UAdcControls& cont){
    return  os << cont._dfsen
	       << '\t'
	       << cont._dfsval
	       << '\t'
	       << cont._s1
	       << '\t'
	       << cont._s2
	       << std::endl;
  }
  std::istream& operator>>(std::istream& is, Fed9UAdcControls& cont){
    return is >> cont._dfsen
	      >> cont._dfsval
	      >> cont._s1
	      >> cont._s2;
  }
  
  // <NAC date="24/04/2007"> operator to compare front end descriptions
  bool operator == (const Fed9UFrontEndDescription& l, const Fed9UFrontEndDescription& r)
  {
    if (l._optoRxOffset != r._optoRxOffset) return false;
    if (l._optoRxCapacitor != r._optoRxCapacitor) return false;
    if (l._medianOverrideDisable != r._medianOverrideDisable) return false;
    if (l._feUnitDisable != r._feUnitDisable) return false;
    if (l._tempControl != r._tempControl) return false;
    for (int c=0; c<CHANNELS_PER_FEUNIT; c++) {
      if (l._fineDelay[c] != r._fineDelay[c]) return false;
      if (l._coarseDelay[c] != r._coarseDelay[c]) return false;
      if (l._trimDacOffset[c] != r._trimDacOffset[c]) return false;
      if (l._channelThreshold[c] != r._channelThreshold[c]) return false;
      if (l._channelBufferOccupancy[c] != r._channelBufferOccupancy[c]) return false;
      if (l._complement[c] != r._complement[c]) return false;
      if (l._adcControls[c] != r._adcControls[c]) return false;
    }
    for (int cp=0; cp<CHANNELS_PER_FEUNIT/2; cp++) {
      if (l._fakeEventRandomSeed[cp] != r._fakeEventRandomSeed[cp]) return false;
      if (l._fakeEventRandomMask[cp] != r._fakeEventRandomMask[cp]) return false;
    }
    for (int a=0; a<APVS_PER_FEUNIT; a++) {
      if (l._apvDisable[a] != r._apvDisable[a]) return false;
      if (l._medianOverride[a] != r._medianOverride[a]) return false;
    }
    return true;
  }
  // </NAC>

}
