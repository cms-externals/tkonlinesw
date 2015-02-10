#include <stdint.h>
#include <inttypes.h>

#include "Fed9UAddress.hh"

#include "ICAssert.hh"
#include <iostream>
#include <cmath>
#include <cstdlib>

namespace Fed9U {


  /******************************************************************************************
   * Constructors.
   *
   *
   ******************************************************************************************/

  Fed9UAddress::Fed9UAddress() :
    _femod(UNSPECIFIED), 
    _fechan(UNSPECIFIED), 
    _apv(UNSPECIFIED), 
    _strip(UNSPECIFIED),
    _countFromTop(FED9U_ADDRESS_COUNT_FROM_TOP) , 
    // <NAC date=28/03/2007>
    _countBase(FED9U_ADDRESS_COUNT_BASE),
    _broadcastDelayChip(false)
    // </NAC>
  { ICUTILS_VERIFY( abs(_countBase) <= 1 )(_countBase).error().msg("The count base for external counting must be 0 or 1!!!"); }

  Fed9UAddress::Fed9UAddress(u8 fedchan, u8 apv, u8 strip) :
    _femod(fedchan/CHANNELS_PER_FEUNIT), 
    _fechan(fedchan%CHANNELS_PER_FEUNIT), 
    _apv(apv), 
    _strip(strip), 
    _countFromTop(FED9U_ADDRESS_COUNT_FROM_TOP), 
    // <NAC date=28/03/2007>
    _countBase(FED9U_ADDRESS_COUNT_BASE),
    _broadcastDelayChip(false)
    // </NAC>
  { ICUTILS_VERIFY( abs(_countBase) <= 1 )(_countBase).error().msg("The count base for external counting must be 0 or 1!!!"); }

  Fed9UAddress::Fed9UAddress(bool fromTop, u16 base) : 
    _femod(UNSPECIFIED), 
    _fechan(UNSPECIFIED), 
    _apv(UNSPECIFIED), 
    _strip(UNSPECIFIED),
    _countFromTop(fromTop),
    // <NAC date=28/03/2007>
    _countBase(base),
    _broadcastDelayChip(false)
    // </NAC>
  { ICUTILS_VERIFY( abs(_countBase) <= 1 )(_countBase).error().msg("The count base for external counting must be 0 or 1!!!"); }


  /******************************************************************************************
   * Internal numbering Getters.
   *
   *
   ******************************************************************************************/


  u8  Fed9UAddress::getFedFpga() const {
    return _femod;
  }

  u8  Fed9UAddress::getFedFeUnit() const {
    ICUTILS_VERIFY(_femod < FEUNITS_PER_FED || _femod == FEBROADCAST || _femod == BACKEND)((int)_femod).error().msg("NOTE:!!! This is an internal feUnit Number!!![0-7]");
    return _femod;
  }

  u8  Fed9UAddress::getFeUnitChannel() const {
    ICUTILS_VERIFY(_femod != FEBROADCAST && _femod != BACKEND && _femod != VME)
      .msg("Address is BACKEND, VME or FEBROADCAST, can't get channel number").error();
    ICUTILS_VERIFY(_fechan < CHANNELS_PER_FEUNIT)((int)_fechan).error().msg("NOTE:!!! This is an internal channel Number!!![0-12]");;
    return _fechan;
  }
  // <NAC date=28/03/2007>
  u8  Fed9UAddress::getFeUnitDelayChip() const {
    ICUTILS_VERIFY(_femod != BACKEND && _femod != VME)
      .msg("Address is BACKEND or VME, can't get delay chip number").error();
    ICUTILS_VERIFY(_femod != FEBROADCAST || _broadcastDelayChip).msg("Address is FEBROADCAST and delayChip is not broadcast, can't get delay chip number").error();
    if (_broadcastDelayChip) { return 3; }
    else { return static_cast<int>(static_cast<float>(_fechan)/4.0); }
  }
  u8  Fed9UAddress::getFedDelayChip() const {
    ICUTILS_VERIFY(_femod != FEBROADCAST && _femod != BACKEND && _femod != VME)
      .msg("Address is BACKEND, VME or FEBROADCAST, can't get delay chip number").error();
    ICUTILS_VERIFY(!_broadcastDelayChip)
      .msg("Address broadcast delay chip, can't get FED delay chip number").error();
    return _femod*3+static_cast<int>(static_cast<float>(_fechan)/4.0);
  }
  // </NAC>
  u8  Fed9UAddress::getChannelApv() const {
    ICUTILS_VERIFY(_femod != FEBROADCAST && _femod != BACKEND && _femod != VME)
      .msg("Address is BACKEND, VME or FEBROADCAST, can't get apv number").error();
    ICUTILS_VERIFY(_apv < APVS_PER_CHANNEL)((int)_apv).error().msg("NOTE:!!! This is an internal APV Number!!![0-1]");;
    return _apv;
  }
  u8  Fed9UAddress::getApvStrip() const {
    ICUTILS_VERIFY(_femod != FEBROADCAST && _femod != BACKEND && _femod != VME)
      .msg("Address is BACKEND, VME or FEBROADCAST, can't get strip number").error();
    ICUTILS_VERIFY(_strip < STRIPS_PER_APV)((int)_strip).error().msg("NOTE:!!! This is an internal strip Number!!![0-127]");;
    return _strip;
  }

  /*******************************************************************/

  u8  Fed9UAddress::getFedChannel()            const { return getFedFeUnit()    * CHANNELS_PER_FEUNIT + getFeUnitChannel(); }
  u8  Fed9UAddress::getFedApv()                const { return getFedChannel()   * APVS_PER_CHANNEL    + getChannelApv();    }
  u16 Fed9UAddress::getFedStrip()              const { return getFedApv() * static_cast<u16>(STRIPS_PER_APV) + getApvStrip();  }
  u8  Fed9UAddress::getFeUnitApv()             const { return getFeUnitChannel() * APVS_PER_CHANNEL + getChannelApv(); }

  u8  Fed9UAddress::getFedChannelPair()        const { return getFedChannel() / 2; }
  u8  Fed9UAddress::getFeUnitChannelPair()        const { return getFeUnitChannel() / 2; }
  u8  Fed9UAddress::getChannelPairChannel()        const { return getFeUnitChannel() % 2; }
  




  /******************************************************************************************
   * External numbering Getters.
   * 
   * external getters, return external numberings depending on 
   * the base and direction of numbering chosen by the user
   ******************************************************************************************/

  u8  Fed9UAddress::getExternalFedFeUnit()     const { 
    if ( _femod == FEBROADCAST || _femod == BACKEND ) 
      return getFedFeUnit();
    else
      return _countFromTop ? _countBase + _femod : FEUNITS_PER_FED - _femod + _countBase - 1; 
  }
  u8  Fed9UAddress::getExternalFeUnitChannel() const { 
    return _countFromTop ? ( getFedChannel() % CHANNELS_PER_FEUNIT ) + _countBase 
      : ( _countBase + CHANNELS_PER_FEUNIT - ( getFedChannel() % CHANNELS_PER_FEUNIT ) - 1 );
  }

  u8  Fed9UAddress::getExternalFeUnitChannelPair() const { 
    return _countFromTop ? ( ( getFedChannel() % CHANNELS_PER_FEUNIT ) / 2 ) + _countBase 
      : ( _countBase +( CHANNELS_PER_FEUNIT / 2 ) - ( ( getFedChannel() % CHANNELS_PER_FEUNIT) / 2 ) - 1 );
  }


  
  // note that the order of counting does not affect the APV order or the strip order, 
  // since these are set on the module and not on the front pannel of the FED.
  // however, to be consistent accross the board, we use the same count base as for the other numbering
  u8  Fed9UAddress::getExternalChannelApv() const { 
    return _countBase + getChannelApv();
  }
  u8  Fed9UAddress::getExternalApvStrip() const {
    return _countBase + getApvStrip();
  }
 
  /*******************************************************************/


  u8  Fed9UAddress::getExternalFedChannel()    const { 
    return _countFromTop ? getFedChannel() + _countBase :  ( _countBase + CHANNELS_PER_FED - getFedChannel() - 1 );
  }

  u8  Fed9UAddress::getExternalChannelPairChannel()    const { 
    return _countFromTop ? ( getFedChannel() % 2 ) + _countBase :  ( _countBase + (( CHANNELS_PER_FED - getFedChannel() ) % 2) - 1 );
  }

  u8  Fed9UAddress::getExternalFedChannelPair()    const { 
    return _countFromTop ? ( getFedChannel() / 2 ) + _countBase :  ( _countBase + ( CHANNELS_PER_FED / 2) - ( getFedChannel()/2 ) - 1 );
  }

  u16  Fed9UAddress::getExternalFedApv()     const { 
    // remember that the apv count order does not depend on the Front end and channel count order! so we don't depend on _countFromTop.
    return getExternalFedChannel() * APVS_PER_CHANNEL + ( getChannelApv()-(_countBase * (APVS_PER_CHANNEL-1) ) );
  }
  u16  Fed9UAddress::getExternalFedStrip()   const { 
    return getExternalFedApv() * static_cast<u16>(STRIPS_PER_APV) + ( getApvStrip() - (_countBase*(static_cast<u16>(STRIPS_PER_APV)-1)));
  }
  u8  Fed9UAddress::getExternalFeUnitApv() const  {
    // remember that the apv count order does not depend on the Front end and channel count order! so we don't depend on _countFromTop.
    return getExternalFeUnitChannel() * APVS_PER_CHANNEL + ( getChannelApv()-(_countBase * (APVS_PER_CHANNEL-1) ) );
  }


  
  /******************************************************************************************
   * Count base and number order for external numbering Getters.
   * 
   ******************************************************************************************/
  
  u16 Fed9UAddress::getCountBase() const {
    return _countBase;
  }
  bool Fed9UAddress::getCountFromTop() const {
    return _countFromTop;
  }
 
  
  /******************************************************************************************
   * Internal numbering Setters.
   * 
   * Internal getters, return Internal numberings independent of 
   * the base and direction of numbering chosen by the user. This numering is the internal
   * representation used in the Fed9UAddress class to store the address
   ******************************************************************************************/
  
  Fed9UAddress& Fed9UAddress::setAddress(u8 fedFeUnit, u8 feUnitChannel,
					 u8 channelApv, u8 apvStrip) {
    return setFedFeUnit(fedFeUnit).setFeUnitChannel(feUnitChannel).setChannelApv(channelApv).setApvStrip(apvStrip);
  }

  Fed9UAddress& Fed9UAddress::setFedFpga(u8 fedFpga) {
    if (fedFpga == FEBROADCAST || fedFpga == BACKEND || fedFpga == VME) _fechan = _apv = _strip = UNSPECIFIED;
    else ICUTILS_VERIFY((fedFpga<FEUNITS_PER_FED)||(fedFpga==VME)||(fedFpga==BACKEND) || fedFpga == UNSPECIFIED)((int)fedFpga).error().msg("NOTE:!!! This is an internal feUnit Number!!![0-7]");;
    _femod = fedFpga;
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setFedFeUnit(u8 fedmod) {
    if (fedmod == FEBROADCAST || fedmod == BACKEND) _fechan = _apv = _strip = UNSPECIFIED;
    else ICUTILS_VERIFY(fedmod < FEUNITS_PER_FED || fedmod == UNSPECIFIED)((int)fedmod).error().msg("NOTE:!!! This is an internal feUnit Number!!![0-7]");;
    _femod = fedmod;
    return *this;
  }
  Fed9UAddress& Fed9UAddress::setFeUnitChannel(u8 fechan) {
    ICUTILS_VERIFY(fechan < CHANNELS_PER_FEUNIT || fechan == UNSPECIFIED)((int)fechan).error().msg("NOTE:!!! This is an internal channel Number!!![0-11]");;
    _fechan = fechan;
    return *this;
  }
  // <NAC date=28/03/2007>
  Fed9UAddress& Fed9UAddress::setFeUnitDelayChip(u8 fedelay) {
    //check it is a delaychip(0-2) or broadcast(3)
    ICUTILS_VERIFY(fedelay < CHANNELS_PER_FEUNIT/4+1)((int)fedelay).error().msg("NOTE:!!! This is a delay chip Number!!![0-2 or 3 for broadcast]");;
    //if it is broadcast then set _broadcastDelayChip
    if (fedelay==3) { _broadcastDelayChip=true; return *this; }
    else { _fechan = fedelay*4; _broadcastDelayChip=false; return *this; }
  }
  Fed9UAddress& Fed9UAddress::setFedDelayChip(u8 fedDelay) {
    setFedFeUnit(fedDelay / 3);
    setFeUnitDelayChip(fedDelay % 3);
    return *this;
  }
  // </NAC>
  Fed9UAddress& Fed9UAddress::setFeUnitChannelPair(u8 fechan) {
    ICUTILS_VERIFY(fechan < (CHANNELS_PER_FEUNIT/2) || fechan == UNSPECIFIED)((int)fechan).error().msg("NOTE:!!! This is an internal channel pair Number!!![0-5]");;
    u8 chanPairChan = 0;
    try {
      chanPairChan = getChannelPairChannel();
    }catch (...){
      chanPairChan = 0; // do nothing and we use chanPairChan = 0 since channel has not previously been set
    }
    _fechan = (fechan * 2) + chanPairChan;
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setChannelApv(u8 chanapv) {
    ICUTILS_VERIFY(chanapv < APVS_PER_CHANNEL || chanapv == UNSPECIFIED)((int)chanapv).error().msg("NOTE:!!! This is an internal apv Number!!![0-1]");;
    _apv = chanapv;
    return *this;
  }
  Fed9UAddress& Fed9UAddress::setApvStrip(u8 apvstrip) {
    ICUTILS_VERIFY(apvstrip < STRIPS_PER_APV || apvstrip == UNSPECIFIED)((int)apvstrip).error().msg("NOTE:!!! This is an internal strip Number!!![0-127]");;
    _strip = apvstrip;
    return *this;
  }


  /*******************************************************************/

  Fed9UAddress& Fed9UAddress::setFedChannel(u8 fedChannel) {
    setFedFeUnit(fedChannel / CHANNELS_PER_FEUNIT);
    setFeUnitChannel(fedChannel % CHANNELS_PER_FEUNIT);
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setFedChannelPair(u8 fedchanpair) {
    ICUTILS_VERIFY(fedchanpair < (CHANNELS_PER_FED/2) || fedchanpair == UNSPECIFIED)((int)fedchanpair).error().msg("NOTE:!!! This is an internal channel pair Number!!![0-47]");
    setFedFeUnit(fedchanpair / (CHANNELS_PER_FEUNIT/2));
    setFeUnitChannelPair(  fedchanpair % ( CHANNELS_PER_FEUNIT/2)  );

    return *this;
  }

  Fed9UAddress& Fed9UAddress::setChannelPairChannel(u8 chanpairchan) {
    ICUTILS_VERIFY(chanpairchan < 2 )((int)chanpairchan).error().msg("NOTE:!!! This is an internal channel pair channel Number!!![0-1]");;
    setFeUnitChannel( ( getFeUnitChannelPair() * 2 ) + chanpairchan  );
    return *this;
  }


  Fed9UAddress& Fed9UAddress::setFedApv(u8 fedApv) {
    setFedChannel(fedApv / APVS_PER_CHANNEL);
    setChannelApv(fedApv % APVS_PER_CHANNEL);
    return *this;
  }
  Fed9UAddress& Fed9UAddress::setFedStrip(u16 fedStrip) {
    setFedApv(fedStrip / STRIPS_PER_APV);
    setApvStrip(fedStrip % STRIPS_PER_APV);
    return *this;
  }
  Fed9UAddress& Fed9UAddress::setFeUnitApv(u8 feUnitApv) {
    setFeUnitChannel(feUnitApv / APVS_PER_CHANNEL);
    setChannelApv(feUnitApv % APVS_PER_CHANNEL);
    return *this;
  }






   /******************************************************************************************
   * External numbering Setters.
   * 
   * external setters set external numberings depending on 
   * the base and direction of numbering chosen by the user
   ******************************************************************************************/

  Fed9UAddress& Fed9UAddress::setExternalFedFeUnit(u8 feUnit) {
    if (feUnit == FEBROADCAST || feUnit == BACKEND) { 
      _fechan = _apv = _strip = UNSPECIFIED;
      setFedFeUnit(feUnit);
    } else {
      ICUTILS_VERIFY ( ( feUnit - _countBase ) < FEUNITS_PER_FED || feUnit == UNSPECIFIED)(_countBase)((int)feUnit).msg("ExternalFeUnit Out of Range").error();
      _femod = _countFromTop ? feUnit - _countBase : ( FEUNITS_PER_FED - ( feUnit - _countBase ) ) - 1 ;
    }
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setExternalFeUnitChannel(u8 channel) {
    ICUTILS_VERIFY ( ( channel - _countBase ) < CHANNELS_PER_FEUNIT )(_countBase)(channel).msg("External channel Out of Range").error();
    _fechan = _countFromTop ? channel - _countBase : ( CHANNELS_PER_FEUNIT - ( channel - _countBase) ) - 1 ;
    return *this;
  } 

  Fed9UAddress& Fed9UAddress::setExternalFeUnitChannelPair(u8 channelPair) {
    ICUTILS_VERIFY ( ( channelPair - _countBase ) < ( CHANNELS_PER_FEUNIT / 2) )(_countBase)(channelPair).msg("External channel pair Out of Range").error();
    u8 chanpairchan = 0;
    try {
      chanpairchan = getChannelPairChannel();
    } catch (...) {
      chanpairchan=0;// ignore this error since it just means that chanpairchan is unset.. so we use 0
    }

    _fechan = _countFromTop ? ( channelPair * 2 ) - _countBase + chanpairchan : ( CHANNELS_PER_FEUNIT - ( (channelPair*2) - _countBase) ) - 1 + chanpairchan ;
    return *this;
  } 
  
  Fed9UAddress& Fed9UAddress::setExternalChannelPairChannel(u8 chanpairchan) {
    ICUTILS_VERIFY(chanpairchan - _countBase < 2 )((int)chanpairchan).error().msg("NOTE:!!! This is an external channel pair channel Number!!");;
    setChannelPairChannel( _countFromTop ? chanpairchan - _countBase : (1 - (chanpairchan - _countBase) ) );
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setExternalChannelApv(u8 chanApv) {
    // remember that apv order is not affected by _countFromTop since it's not changed by FED front pannel numbering, it's defined by the module
    setChannelApv(chanApv - _countBase);
    return * this;
  }

  Fed9UAddress& Fed9UAddress::setExternalApvStrip(u8 apvStrip) {
    // remember that strip order is not affected by _countFromTop since it's not changed by FED front pannel numbering, it's defined by the module
    setApvStrip( apvStrip - _countBase );
    return * this;
  }

  


  /*******************************************************************/



  Fed9UAddress& Fed9UAddress::setExternalFedChannel(u8 channel) {
    ICUTILS_VERIFY( ( channel - _countBase ) < CHANNELS_PER_FED )(_countBase)(channel).msg("External Channel Out of Range").error(); 
    setFedChannel( _countFromTop ? channel - _countBase : ( CHANNELS_PER_FED - ( channel  - _countBase ) ) - 1 );
    return *this;
  }

  Fed9UAddress& Fed9UAddress::setExternalFedChannelPair(u8 channelPair) {
    ICUTILS_VERIFY( ( channelPair - _countBase ) < ( CHANNELS_PER_FED/2) )(_countBase)(channelPair).msg("External Channel Pair Out of Range").error(); 
    setFedChannelPair( _countFromTop ? channelPair - _countBase : ( (CHANNELS_PER_FED/2) - ( channelPair  - _countBase ) ) - 1 );
    return *this;
  }


  Fed9UAddress& Fed9UAddress::setExternalFedApv(u16 fedApv) {
    // remember that strip order is not affected by _countFromTop since it's not changed by FED front pannel numbering, it's defined by the module
    setExternalFedChannel(  (fedApv + _countBase) / APVS_PER_CHANNEL );
    setChannelApv(  (fedApv + _countBase) % APVS_PER_CHANNEL  );
    return *this;
  }
  
  Fed9UAddress& Fed9UAddress::setExternalFedStrip(u16 fedStrip) {
   // remember that apv order is not affected by _countFromTop since it's not changed by FED front pannel numbering, it's defined by the module
    setExternalFedApv( (fedStrip + _countBase*( static_cast<u16>(STRIPS_PER_APV)-1 ) ) / static_cast<u16>(STRIPS_PER_APV));
    setApvStrip( ( fedStrip + _countBase*( static_cast<u16>(STRIPS_PER_APV)-1 ) ) % static_cast<u16>(STRIPS_PER_APV) );
    return *this;
  }
    
  Fed9UAddress& Fed9UAddress::setExternalFeUnitApv(u8 feUnitApv) {
     // remember that strip order is not affected by _countFromTop since it's not changed by FED front pannel numbering, it's defined by the module
    setExternalFeUnitChannel(  (feUnitApv + _countBase) / APVS_PER_CHANNEL );
    setChannelApv(  (feUnitApv + _countBase) % APVS_PER_CHANNEL  );
    return *this;
  }

  /** This method allows you to set the feUnit number bassed upon the feFirmware Address (which happens to run from 1 throuh 8 and 15)
   *  rather than from 0 through 7 as in software internal or 8 through 1 in external numbering
   *  This is used inside the Fed9UVmeBase and below. It should
   *  be used when passing fpga numbers to Fed9UVmeBase.
   ************************************/
  Fed9UAddress& Fed9UAddress::setFirmwareFedFeUnit(u8 feFpga) {
    _femod = feFpga - 1;
    return *this;
  }

   /** This method allows you to get the feUnit number bassed upon the feFirmware Address (which happens to run from 1 throuh 8 and 15)
   *  rather than from 0 through 7 as in software internal or 8 through 1 in external numbering
   *  This is used inside the Fed9UVmeBase and below. It should
   *  be used when passing fpga numbers to Fed9UVmeBase.
   ************************************/
  u8 Fed9UAddress::getFirmwareFedFeUnit() const {
    return _femod + 1;
  }

}
