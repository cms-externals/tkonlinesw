#include "TypeDefs.hh"
#include "Fed9UChannelInterface.hh"
#include "ICExImpl.hh"
#include <iostream>

namespace Fed9U {

  Fed9UChannelInterface::Fed9UChannelInterface() : _channel(0), _base(FED9U_CHANNEL_INTERFACE_COUNT_BASE_DEFAULT), _fromTop(FED9U_CHANNEL_INTERFACE_COUNT_FROM_TOP_DEFAULT) {} 
  
  Fed9UChannelInterface::Fed9UChannelInterface(u8 fedChannel, u8 countingBase, bool fromTop) : 
    _base(countingBase), _fromTop(fromTop) {
    setChannel(fedChannel); 
  }

  u8 Fed9UChannelInterface::getChannel() const {
    return _fromTop ? _channel + _base :  ( _base + CHANNELS_PER_FED  - _channel  - 1 );
  }
 
  u8 Fed9UChannelInterface::getFeUnit() const {
    return getChannel() / CHANNELS_PER_FEUNIT;
  }
 
  u8 Fed9UChannelInterface::getFeUnitChannel() const {
    return _fromTop ? ( _channel % CHANNELS_PER_FEUNIT ) + _base 
      : ( _base + CHANNELS_PER_FEUNIT - ( _channel % CHANNELS_PER_FEUNIT ) - 1 );
  }
  
  Fed9UChannelInterface & Fed9UChannelInterface::setChannel(u8 channel) {
    ICUTILS_VERIFY( ( channel - _base ) < CHANNELS_PER_FED )((u16)_base)(channel).msg("Channel Out of Range").error(); 
    _channel = _fromTop ? channel - _base : ( CHANNELS_PER_FED - ( channel  - _base ) ) - 1;
    return *this;
  }
  
  Fed9UChannelInterface &  Fed9UChannelInterface::setFeUnit(u8 feUnit) {
    setChannel( feUnit * CHANNELS_PER_FEUNIT + getFeUnitChannel() );
    return *this;
  }

  Fed9UChannelInterface &  Fed9UChannelInterface::setFeUnitChannel(u8 feUnitChannel) {
    setChannel(  getFeUnit() * CHANNELS_PER_FEUNIT   + feUnitChannel ); 
    return *this;
  }
  
  Fed9UChannelInterface & Fed9UChannelInterface::setBase(u8 base) {
    ICUTILS_VERIFY( base < (0xFF - CHANNELS_PER_FED) )(base).msg("base is out of range").error(); 
    _base=base;
    return *this;
  }

  u8 Fed9UChannelInterface::getBase() {
    return _base;
  }
  
  Fed9UChannelInterface & Fed9UChannelInterface::setCountFromTop(bool fromTop) {
    _fromTop=fromTop;
    return *this;
  }

  bool Fed9UChannelInterface::getCountFromTop() {
    return _fromTop;
  }
  
  u8 Fed9UChannelInterface::getChannelInternal() const {
    return _channel;
  }
  
  Fed9UChannelInterface & Fed9UChannelInterface::setChannelInternal(u8 channel){
    ICUTILS_VERIFY( channel < CHANNELS_PER_FED )(channel).msg("Channel out of range").error();
    _channel = channel;
    return *this;
  }
  
  
  
}
