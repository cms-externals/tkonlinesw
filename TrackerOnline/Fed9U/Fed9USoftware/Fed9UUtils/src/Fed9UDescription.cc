#include <stdint.h>
#include <inttypes.h>
#include "Fed9UDescription.hh"
#include "ICAssert.hh"
#include "TypeDefs.hh"
#include "Fed9ULogTemplate.hh"
// <NAC date="26/06/2007"> needed to check that address table exists
#include <fstream>
// </NAC>

#include <iostream>
#include <cstdlib>

// This version number must be incremented whenever
// a change is made to the description storage format:
/*#define DESCRIPTION_VERSION 6//This is in the header.*/

/*#define VERSION_TEXT(name) ("Fed9U" #name "Version" STRINGIZE(DESCRIPTION_VERSION))//This is in the header*/

namespace Fed9U {

  //<JEC date=27/9/2007>
  //  std::string Fed9U::Fed9UDescription::_fed9UAddressTable = std::string("Fed9UAddressTable.dat");
  //</JEC>

  using std::endl;

  Fed9UDescription::Fed9UDescription():_epromDescription(2) {
    memset ( &(_name[0]),0,200);
    memset ( &(_fakeEventFile[0]),0,200);
    //<JEC date=26/9/2007> changed _fed9UAddressTable to a char array
    memset ( &(_fed9UAddressTable[0]),0,200);
    //</JEC>
    
    // <NAC date="26/07/2007"> fixed to check ENV_CMS_TK_DAQ and to check that the file exists and throw if not
    char* tkDaqBaseDir = getenv ("ENV_TRACKER_DAQ");
    char* fedBaseDir = getenv ("ENV_CMS_TK_FED9U_ROOT");
    char* baseDir;
    if (tkDaqBaseDir != NULL) baseDir = tkDaqBaseDir;
    else baseDir = fedBaseDir;
    //std::cout << "ENV_TRACKER_DAQ " << tkDaqBaseDir << std::endl;
    //std::cout << "ENV_CMS_TK_FED9U_ROOT " << fedBaseDir << std::endl;
    //std::cout << "Looking for Fed9UAddressTable.dat in " << baseDir << std::endl;
    //<JEC date=26/9/2007>
    //    if (baseDir != NULL) _fed9UAddressTable = std::string(baseDir)+std::string("/config/Fed9UAddressTable.dat");
    if (baseDir != NULL) {
      std::string tmpStr = std::string(baseDir)+std::string("/config/Fed9UAddressTable.dat");
      memcpy ( &(_fed9UAddressTable[0]),tmpStr.c_str(),(tmpStr.size()>200)?200:tmpStr.size());
    }
    //</JEC>
    else { 
      //<JEC date=26/9/2007>
      //      _fed9UAddressTable = std::string("Fed9UAddressTable.dat");
      std::string tmpStr = "Fed9UAddressTable.dat";
      memcpy ( &(_fed9UAddressTable[0]),tmpStr.c_str(),(tmpStr.size()>200)?200:tmpStr.size());
      //</JEC>
      
      std::stringstream msg;
      msg << "Could not find environmental variable ENV_TRACKER_DAQ or ENV_CMS_TK_FED9U_ROOT, defaulting fed HAL address table to Fed9UAddressTable.dat, in your local directory, if you do not have this file present, the fed software will not work!!!" << std::endl;
      Fed9UMessage<Fed9UErrorLevel>(FED9U_ERROR_LEVEL_UNEXPECTED_RECOVERED) << msg.str();
      std::cout << msg.str();	//remove this. it is to ensure you see it when using libraries from outside Fed9U
    }
    std::stringstream msg;
    msg << "Using Address table " << _fed9UAddressTable << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << msg.str();
    std::cout << msg.str();	//remove this. it is to ensure you see it when using libraries from outside Fed9U
    
    //<JRF date="28/11/2007"> 
    // The following code has been moved to within the Fed9UVMEBase class so that this exception is never raised when only dealing with descriptions and not using hardware
    
    //std::ifstream addressTableFile(_fed9UAddressTable);
    //ICUTILS_VERIFYX(addressTableFile,Fed9UDescriptionException)(_fed9UAddressTable)(tkDaqBaseDir)(fedBaseDir).msg("Failed to open Address table").error().code(Fed9UDescriptionException::ERROR_NO_ADDRESS_TABLE);
    
    // </JRF>

    // </NAC>

    this->loadDefaultDescription();
  }

  Fed9UDescription::~Fed9UDescription() {
  }

  Fed9UDescription* Fed9UDescription::clone ( ) {
    Fed9UDescription* description =  new Fed9UDescription ( *this ) ;
    return description;
  }
  
  /**************************************************************************************************
   *  setters
   *************************************************************************************************/

  Fed9UDescription & Fed9UDescription::setHalAddressTable(const std::string val) {
    //<JEC date=27/9/2007>
    //    _fed9UAddressTable = val;
    //    strcpy(_fed9UAddressTable,val.c_str());
    char deft[22] = "Fed9UAddressTable.dat";
    if (val.size() == 0) {
      memcpy ( &(_fed9UAddressTable[0]),&(deft[0]),22);
    } else {
      std::string temp;
      for(u32 i=0; i < val.size(); i++){
	if (val[i] != ' ') 
	  temp.push_back(val[i]);
      }
      memcpy ( &(_fed9UAddressTable[0]),temp.c_str(),(temp.size()>200)?200:temp.size());
    } 
    //</JEC>
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setBusAdaptorType(Fed9UHalBusAdaptor type) {
    ICUTILS_VERIFY(type == FED9U_HAL_BUS_ADAPTOR_SBS || 
	   type == FED9U_HAL_BUS_ADAPTOR_CAEN_PCI ||
	   type == FED9U_HAL_BUS_ADAPTOR_CAEN_USB ||
	   type == FED9U_HAL_BUS_ADAPTOR_VXI ||
	   type == FED9U_HAL_BUS_ADAPTOR_DUMMY )(type).error().msg("You have not set a good bus adaptor Type");
    _busAdaptorType = type;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFeFirmwareVersion(u32 version) {
   _feFirmwareVersion=version; 
   return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setBeFirmwareVersion(u32 version) {
    _beFirmwareVersion=version;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setVmeFirmwareVersion(u32 version) {
    _vmeFirmwareVersion=version;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setDelayFirmwareVersion(u32 version) {
    _delayFirmwareVersion=version;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setFedVersion(u32 version) {
    _fedVersion=version;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setEpromVersion(u32 version) {
    _epromVersion=version;
    return *this;
  }
  
  Fed9UHalBusAdaptor Fed9UDescription::getBusAdaptorType() const {
    ICUTILS_VERIFY(_busAdaptorType == FED9U_HAL_BUS_ADAPTOR_SBS || 
	   _busAdaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_PCI ||
	   _busAdaptorType == FED9U_HAL_BUS_ADAPTOR_CAEN_USB ||
	   _busAdaptorType == FED9U_HAL_BUS_ADAPTOR_VXI ||
	   _busAdaptorType == FED9U_HAL_BUS_ADAPTOR_DUMMY )(_busAdaptorType).error().msg("You have not set a good bus adaptor Type");
    return _busAdaptorType;
  }
  
  u32 Fed9UDescription::getEpromVersion()const {
    return _epromVersion;
  }

  u32 Fed9UDescription::getFedVersion()const {
    return _fedVersion;
  }
  
  u32 Fed9UDescription::getFeFirmwareVersion()const {
    return _feFirmwareVersion; 
  }
  
  u32 Fed9UDescription::getBeFirmwareVersion()const {
     return _beFirmwareVersion; 
 }

  u32 Fed9UDescription::getVmeFirmwareVersion()const {
    return _vmeFirmwareVersion; 
  }

  u32 Fed9UDescription::getDelayFirmwareVersion()const {
    return _delayFirmwareVersion; 
  }
  
  Fed9UDescription& Fed9UDescription::setDelay(const Fed9UAddress& fedChannel, u16 coarseDelay, u16 fineDelay) {
    setFineDelay(fedChannel, fineDelay);
    setCoarseDelay(fedChannel, coarseDelay);
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFineDelay(const Fed9UAddress& fedChannel, u16 value) {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedChannel.getFedFeUnit()]._fineDelay[fedChannel.getFeUnitChannel()] = value;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setCoarseDelay(const Fed9UAddress& fedChannel, u16 value) {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedChannel.getFedFeUnit()]._coarseDelay[fedChannel.getFeUnitChannel()] = value;    
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setTrimDacOffset(const Fed9UAddress& fedChannel, u16 value) {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedChannel.getFedFeUnit()]._trimDacOffset[fedChannel.getFeUnitChannel()] = value;    
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setOptoRxOffset(const Fed9UAddress& fedFeUnit, u16 value) {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    ICUTILS_VERIFY(value < 64)(value).msg("Value out of range 0 - 63").error();
    if (fedFeUnit.getFedFeUnit() == Fed9UAddress::FEBROADCAST) doLoop(&Fed9UDescription::setOptoRxOffset, value);
    _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset = value;    
    return *this;
  }  

  Fed9UDescription& Fed9UDescription::setOptoRxInputOffset(const Fed9UAddress& fedFeUnit, u16 value) {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    ICUTILS_VERIFY(value < 16)(value).msg("Value out of range 0 - 15").error();
    if (fedFeUnit.getFedFeUnit() == Fed9UAddress::FEBROADCAST) doLoop(&Fed9UDescription::setOptoRxInputOffset, value);
    _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset = ( _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset & 0x30 ) | ( value & 0x0F );    
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setOptoRxOutputOffset(const Fed9UAddress& fedFeUnit, u16 value) {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    ICUTILS_VERIFY(value < 4)(value).msg("Value out of range 0 - 3").error();
    if (fedFeUnit.getFedFeUnit() == Fed9UAddress::FEBROADCAST) doLoop(&Fed9UDescription::setOptoRxOutputOffset, value);
    _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset = ( _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset & 0x0F ) | ( (value << 4) & 0x30 ); 
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setOptoRxCapacitor(const Fed9UAddress& fedFeUnit, u16 value) {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    if (fedFeUnit.getFedFeUnit() == Fed9UAddress::FEBROADCAST) doLoop(&Fed9UDescription::setOptoRxCapacitor, value);
    _feParams[fedFeUnit.getFedFeUnit()]._optoRxCapacitor = value;    
    return *this;
  }  
  
  Fed9UDescription& Fed9UDescription::setApvDisable(const Fed9UAddress& fedApv, bool disable) {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedApv.getFedFeUnit()]._apvDisable[fedApv.getFeUnitApv()] = disable;
    return *this;
  }

  //<GR date=27/07/2006> Added APV fake event disable
  Fed9UDescription& Fed9UDescription::setApvFakeEventDisable(Fed9UAddress fedApv, bool disable) {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedApv.getFedFeUnit()]._apvFakeEventDisable[fedApv.getFeUnitApv()] = disable;
    return *this;
  }
  //</GR>

  Fed9UDescription& Fed9UDescription::setMedianOverride(const Fed9UAddress& fedApv, u16 medianOverride) {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedApv.getFedFeUnit()]._medianOverride[fedApv.getFeUnitApv()] = medianOverride;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setMedianOverrideDisable(const Fed9UAddress& fedFeUnit, bool medianOverrideDisable){
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    if (fedFeUnit.getFedFeUnit() == Fed9UAddress::FEBROADCAST) doLoop(&Fed9UDescription::setMedianOverrideDisable, medianOverrideDisable);
    _feParams[fedFeUnit.getFedFeUnit()]._medianOverrideDisable = medianOverrideDisable;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setCmMedianOverride( Fed9UAddress fedFeUnit, bool medianOverrideDisable, std::vector<u16> medianOverride) {
    for (u8 i=0; i<APVS_PER_FEUNIT; i++){
      ICUTILS_VERIFY(medianOverride[i] < 1024)(medianOverride[i]).msg("Median Override Value too high. Max = 1023").error();
      setMedianOverride( fedFeUnit.setFeUnitChannel(i>>1).setChannelApv(i%2), medianOverride[i] );
    }
    setMedianOverrideDisable ( fedFeUnit, medianOverrideDisable );
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setScopeLength(u16 numberOfWords) {
    _scopeLength = numberOfWords;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setAdcOffset(const Fed9UAddress& channel, u16 offset){
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::BACKEND && channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[channel.getFedFeUnit()]._trimDacOffset[channel.getFeUnitChannel()] = offset;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setTempControl(const Fed9UAddress& fedFpga, const Fed9UTempControl& tempControl) {
    if (fedFpga.getFedFpga() == Fed9UAddress::BACKEND) {
      _beTempControl = tempControl;
    } else if(fedFpga.getFedFpga() == Fed9UAddress::VME){
      _vmeTempControl = tempControl;
    } else if(fedFpga.getFedFpga() == Fed9UAddress::FEBROADCAST){
      doLoop(&Fed9UDescription::setTempControl, tempControl);
    } else {
      _feParams[fedFpga.getFedFpga()]._tempControl = tempControl;
    }
    return * this;
  }

  Fed9UDescription& Fed9UDescription::setFrameThreshold(const Fed9UAddress& channel, u16 channelThreshold) {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::BACKEND && channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << "We are about to set the threshold to :" << channelThreshold << std::endl;
    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << "Value Set was!!! :" <<_feParams[channel.getFedFeUnit()]._channelThreshold[channel.getFeUnitChannel()] << std::endl;
    _feParams[channel.getFedFeUnit()]._channelThreshold[channel.getFeUnitChannel()] = (channelThreshold & 0xFFE0) + ( ((channelThreshold&0x1F) > 0xF) ? 0x20 : 0x0) ; // we have to force the value to be multiple of 32 use usual rounding

    Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_SUPER_DETAILED) << "Value Set was!!! :" <<_feParams[channel.getFedFeUnit()]._channelThreshold[channel.getFeUnitChannel()] << std::endl;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFedBeUnit(u16 beUnit) {
    _beUnit=beUnit;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setTriggerSource(Fed9UTrigSource triggerSource) {
    _triggerSource = triggerSource;
    return *this;
  }
  
  Fed9UDescription& Fed9UDescription::setTestRegister(u32 apvFrameEnable) {
    _testRegister = apvFrameEnable;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFedFeUnitDisable(const Fed9UAddress& fedFeUnit, bool fedFeUnitDisable) {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    _feParams[fedFeUnit.getFedFeUnit()]._feUnitDisable = fedFeUnitDisable;
    return *this;
  }  

  Fed9UDescription& Fed9UDescription::setFedBeFpgaDisable(bool fedDisable) {
    _fedDisable=fedDisable;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFedId(u16 fedId) {
    _fedId = fedId;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setFedHardwareId(u32 fedHardId) {
    _fedHardwareId = fedHardId;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setBeFpgaReadRoute(Fed9UReadRoute readRoute) {
    _readRoute=readRoute;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setChannelBufferOccupancy(const Fed9UAddress& channel, u16 channelOccupancy) {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST && channel.getFedFeUnit() != Fed9UAddress::BACKEND)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[channel.getFedFeUnit()]._channelBufferOccupancy[channel.getFeUnitChannel()]=channelOccupancy;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setAdcControls(const Fed9UAddress& channel, bool dfsen, bool dfsval, bool s1, bool s2) {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST && channel.getFedFeUnit() != Fed9UAddress::BACKEND)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    // note that setting the ADCControls for one channel will also set the same values for the channel pair in the following order
    // | vector location  |   channel pair  |
    // |       0          |      (0,3)      |
    // |       1          |      (1,2)      |
    // |       2          |      (4,7)      |
    // |       3          |      (5,6)      |
    // |       4          |      (8,11)     |
    // |       5          |      (9,10)     |

    // use the following vector to do the mapping
    const u16 channelPairMap[12] = {0, 1, 1, 0, 2, 3, 3, 2, 4, 5, 5, 4};
    _feParams[channel.getFedFeUnit()]._adcControls[channelPairMap[channel.getFeUnitChannel()]]=Fed9UAdcControls( dfsen, dfsval, s1, s2);
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setAdcControls(const Fed9UAddress& channel, const Fed9UAdcControls& adcControls) {
    return setAdcControls(channel, adcControls._dfsen, adcControls._dfsval, adcControls._s1, adcControls._s2 );
  }

  Fed9UDescription& Fed9UDescription::setDaqMode(Fed9UDaqMode fed9UDaqMode) {
    _fed9UDaqMode = fed9UDaqMode;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setDaqSuperMode(Fed9UDaqSuperMode fed9UDaqSuperMode) {
    _fed9UDaqSuperMode = fed9UDaqSuperMode;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setClock(Fed9UClockSource clockSelect) {
    _clockMode = clockSelect;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setTtcrx(const Fed9UTtcrxDescription& ttcrxDescription) {
    _ttcrxDescription = ttcrxDescription;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setEprom(const Fed9UEpromDescription& epromDescription) {
    _epromDescription = epromDescription;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setVoltageMonitor(const Fed9UVoltageControl& voltageController) {
    _voltageController = voltageController;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setComplement(const Fed9UAddress& channel, bool disable) {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST && channel.getFedFeUnit() != Fed9UAddress::BACKEND)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    _feParams[channel.getFedFeUnit()]._complement[channel.getFeUnitChannel()] = disable;
    return *this;
  }

    Fed9UDescription& Fed9UDescription::setName(std::string name) {
      char deft[8]= {'d','e','f','a','u','l','t'};
      if (name.size() == 0) {
	memcpy ( &(_name[0]),&(deft[0]),8);
      }
      else {
	std::string temp;
	for(u32 i=0; i < name.size(); i++){
	  if (name[i] != ' ') 
	  temp.push_back(name[i]);
	}
	memcpy ( &(_name[0]),temp.c_str(),(temp.size()>200)?200:temp.size());
	//_name = temp.c_str();
      } 
      //std::cout << __PRETTY_FUNCTION__ << " name being set to " << name << " and the array holds" << _name << std::endl;
      
      return *this;
    }

  Fed9UDescription& Fed9UDescription::setCrateNumber(u16 crateNumber) {
    _crateNumber = crateNumber;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setVmeControllerDaisyChainId(u16 daisyChainId) {
    _vmeControllerDaisyChainId = daisyChainId;
    return *this;
  }

  Fed9UDescription& Fed9UDescription::setSlotNumber(u8 slotNumber) {
    _baseAddress = slotNumber << 16;
    return *this;
  }

/********************************************************************************************
 * getters
 ********************************************************************************************/

  //<JEC date=26/9/2007>
  //const std::string & Fed9UDescription::getHalAddressTable() const {
  std::string Fed9UDescription::getHalAddressTable() const {
    //    return _fed9UAddressTable;
    return std::string(_fed9UAddressTable);
    //</JEC>
  }
  
  u16 Fed9UDescription::getFineDelay(const Fed9UAddress& fedChannel) const {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedChannel.getFedFeUnit()]._fineDelay[fedChannel.getFeUnitChannel()];    
  }

  u16 Fed9UDescription::getCoarseDelay(const Fed9UAddress& fedChannel) const {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedChannel.getFedFeUnit()]._coarseDelay[fedChannel.getFeUnitChannel()];    
  }

  u16 Fed9UDescription::getTrimDacOffset(const Fed9UAddress& fedChannel) const {
    ICUTILS_VERIFY(fedChannel.getFedFeUnit() != Fed9UAddress::BACKEND && fedChannel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedChannel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedChannel.getFedFeUnit()]._trimDacOffset[fedChannel.getFeUnitChannel()];    
  }

  u16 Fed9UDescription::getOptoRxOffset(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset;    
  }

  u16 Fed9UDescription::getOptoRxInputOffset(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.    getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return ( _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset & 0x0F );    
  }

  u16 Fed9UDescription::getOptoRxOutputOffset(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return ( ( _feParams[fedFeUnit.getFedFeUnit()]._optoRxOffset & 0x30 ) >> 4 ) ;    
  }

  u16 Fed9UDescription::getOptoRxCapacitor(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedFeUnit.getFedFeUnit()]._optoRxCapacitor;    
  }

  bool Fed9UDescription::getApvDisable(const Fed9UAddress& fedApv) const {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedApv.getFedFeUnit()]._apvDisable[fedApv.getFeUnitApv()];
  }
  
  //<GR date=27/07/2006> Added APV fake event disable
  bool Fed9UDescription::getApvFakeEventDisable(Fed9UAddress fedApv) const {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedApv.getFedFeUnit()]._apvFakeEventDisable[fedApv.getFeUnitApv()];
  }
  //</GR>

  u32 Fed9UDescription::getAllApvDisables(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    u32 apvDisables = 0;
    for (u8 i=0;i<APVS_PER_FEUNIT;i++){
      apvDisables |= static_cast<u32>(_feParams[fedFeUnit.getFedFeUnit()]._apvDisable[i]) <<  ((APVS_PER_FEUNIT-1)-i);
    }
    return apvDisables&0xFFFFFF;
  }
  u16  Fed9UDescription::getFakeEventRandomSeed(const Fed9UAddress& fedChannelPair) const {
    return _feParams[fedChannelPair.getFedFeUnit()]._fakeEventRandomSeed[fedChannelPair.getFeUnitChannelPair()];
  }
  
  u16  Fed9UDescription::getFakeEventRandomMask(const Fed9UAddress& fedChannelPair) const {
    return _feParams[fedChannelPair.getFedFeUnit()]._fakeEventRandomMask[fedChannelPair.getFeUnitChannelPair()];
  }

  Fed9UDescription & Fed9UDescription::setFakeEventRandomSeed(Fed9UAddress& fedChannelPair, u16 value) {
    _feParams[fedChannelPair.getFedFeUnit()]._fakeEventRandomSeed[fedChannelPair.getFeUnitChannelPair()] = value;
    return * this;
  }

  Fed9UDescription & Fed9UDescription::setFakeEventRandomMask(Fed9UAddress& fedChannelPair, u16 value) {
    _feParams[fedChannelPair.getFedFeUnit()]._fakeEventRandomMask[fedChannelPair.getFeUnitChannelPair()] = value;
    return * this;
  }


  u16 Fed9UDescription::getMedianOverride(const Fed9UAddress& fedApv) const {
    ICUTILS_VERIFY(fedApv.getFedFeUnit() != Fed9UAddress::BACKEND && fedApv.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedApv.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedApv.getFedFeUnit()]._medianOverride[fedApv.getFeUnitApv()];
  }
  
  bool Fed9UDescription::getMedianOverrideDisable(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedFeUnit.getFedFeUnit()]._medianOverrideDisable;
  }

  std::vector<u16> Fed9UDescription::getCmMedianOverride(Fed9UAddress fedFeUnit, bool& medianOverrideDisable) const {
    std::vector<u16> medianOverrides;
    medianOverrideDisable = getMedianOverrideDisable(fedFeUnit);
    for(u8 i = 0 ; i < APVS_PER_FEUNIT ; i++ ){
      medianOverrides.push_back(getMedianOverride(fedFeUnit.setFeUnitChannel(i>>1).setChannelApv(i%2)));
    }
    return medianOverrides;
  }
  
  u16 Fed9UDescription::getScopeLength() const {
    return _scopeLength;
  }
    
  u16 Fed9UDescription::getAdcOffset(const Fed9UAddress& channel) const {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::BACKEND && channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[channel.getFedFeUnit()]._trimDacOffset[channel.getFeUnitChannel()];
  }

  u16 Fed9UDescription::getFrameThreshold(const Fed9UAddress& channel) const {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::BACKEND && channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[channel.getFedFeUnit()]._channelThreshold[channel.getFeUnitChannel()];
  }  
  
  Fed9UTrigSource Fed9UDescription::getTriggerSource() const {return _triggerSource;}
  
  u32 Fed9UDescription::getTestRegister() const {return _testRegister;}

  bool Fed9UDescription::getFedFeUnitDisable(const Fed9UAddress& fedFeUnit) const {
    ICUTILS_VERIFY(fedFeUnit.getFedFeUnit() != Fed9UAddress::BACKEND && fedFeUnit.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (fedFeUnit.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[fedFeUnit.getFedFeUnit()]._feUnitDisable;
  }
  
  u32 Fed9UDescription::getFedBeUnit() const { return _beUnit;}
    
  bool Fed9UDescription::getFedBeFpgaDisable() const {return _fedDisable;}
  
  u16 Fed9UDescription::getFedId() const {return _fedId;}

  u32 Fed9UDescription::getFedHardwareId() const {return _fedHardwareId;}
  
  Fed9UReadRoute Fed9UDescription::getBeFpgaReadRoute() const {return _readRoute;}
  
  u16 Fed9UDescription::getChannelBufferOccupancy(const Fed9UAddress& channel) const {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST && channel.getFedFeUnit() != Fed9UAddress::BACKEND)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[channel.getFedFeUnit()]._channelBufferOccupancy[channel.getFeUnitChannel()];
  }
  
  Fed9UAdcControls Fed9UDescription::getAdcControls(const Fed9UAddress& channel) const {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST && channel.getFedFeUnit() != Fed9UAddress::BACKEND)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    // note here we get the value for a pair of channels, so we can get one value for two incoming addresses. 
    // the way these addresses are mapped to the channel pair in the vector are as follows (note channel pair is with internal numbering here):
    // | vector location  |   channel pair  |
    // |       0          |      (0,3)      |
    // |       1          |      (1,2)      |
    // |       2          |      (4,7)      |
    // |       3          |      (5,6)      |
    // |       4          |      (8,11)     |
    // |       5          |      (9,10)     |
   
    // use the following vector to do the mapping
    const u16 channelPairMap[12] = {0, 1, 1, 0, 2, 3, 3, 2, 4, 5, 5, 4};
    return _feParams[channel.getFedFeUnit()]._adcControls[channelPairMap[channel.getFeUnitChannel()]];
  }

  Fed9UDaqMode Fed9UDescription::getDaqMode() const{
    return _fed9UDaqMode;
  }

  Fed9UDaqSuperMode Fed9UDescription::getDaqSuperMode() const{
    return _fed9UDaqSuperMode;
  }

  Fed9UClockSource Fed9UDescription::getClock() const {
    return _clockMode;
  }
 
  Fed9UTempControl Fed9UDescription::getTempControl(const Fed9UAddress& fedFpga) const {
    ICUTILS_VERIFY(fedFpga.getFedFpga() != Fed9UAddress::FEBROADCAST)(fedFpga.getFedFpga()).msg("Address out of range").error();
    if (fedFpga.getFedFpga() == Fed9UAddress::BACKEND ) {
      return _beTempControl;
    } else if (fedFpga.getFedFpga() == Fed9UAddress::VME ) {
      return _vmeTempControl;
    } else {
      return _feParams[fedFpga.getFedFpga()]._tempControl;
    }
  }

  bool Fed9UDescription::getComplement(const Fed9UAddress& channel) const {
    ICUTILS_VERIFY(channel.getFedFeUnit() != Fed9UAddress::BACKEND && channel.getFedFeUnit() != Fed9UAddress::FEBROADCAST)
      (channel.getFedFeUnit()).msg("Address out of range").error();
    return _feParams[channel.getFedFeUnit()]._complement[channel.getFeUnitChannel()];
  }

  Fed9UTtcrxDescription Fed9UDescription::getTtcrx() const {
    return _ttcrxDescription;
  }

  Fed9UEpromDescription Fed9UDescription::getEprom() const {
    return _epromDescription;
  }

  Fed9UVoltageControl Fed9UDescription::getVoltageMonitor() const {
    return _voltageController;
  }

  u16 Fed9UDescription::getCrateNumber() const {
    return _crateNumber;
  }

  u16 Fed9UDescription::getVmeControllerDaisyChainId() const {
    return _vmeControllerDaisyChainId;
  }

  u8 Fed9UDescription::getSlotNumber() const {
    return (_baseAddress >> 16) & 0xFF;
  }

  //<JEC date=08/12/2005>
  //Added eventType to description
  Fed9UDescription& Fed9UDescription::setDaqEventType(u16 eventType) {
    _eventType = eventType;
    return *this;
  }

  u16 Fed9UDescription::getDaqEventType(void) const {
    return _eventType;
  }
  //</JEC>

  //<JEC date=08/12/2005>
  //Added DaqFov to description
  Fed9UDescription& Fed9UDescription::setDaqFov(u16 fov) {
    _fov = fov;
    return *this;
  }

  u16 Fed9UDescription::getDaqFov(void) const {
    return _fov;
  }
  //</JEC>

  //<JEC date=09/01/2006>
  //Added tracker header type
  Fed9UHeaderFormat Fed9UDescription::getHeaderFormatType(void) const {
    return _headerType;
  }

  Fed9UDescription& Fed9UDescription::setHeaderFormatType(Fed9UHeaderFormat headerType) {
    _headerType = headerType;
    return *this;
  }
  //</JEC>

  //<JEC date=23/02/06>
  //Added BunchCrossingOffset to description
  Fed9UDescription& Fed9UDescription::setBunchCrossingOffset(u16 bxOffset) {
    _bxOffset = bxOffset;
    return *this;
  }

  u16 Fed9UDescription::getBunchCrossingOffset(void) const {
    return _bxOffset;
  }
  //</JEC>


  /************************************************************************************************
   * methods for creating and loading and saving descriptions and subcomponents.
   ***********************************************************************************************/
  void Fed9UDescription::loadDescription(std::istream & is) {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    static const std::string expectedVersion = VERSION_TEXT(Description);
    std::string actualVersion;
    is >> actualVersion;
    is >> _name;
    ICUTILS_VERIFY(actualVersion == expectedVersion)(actualVersion)(expectedVersion).error()
      .msg("Wrong version number in settings, they may need to be updated");
    loadSettings(is);
    loadStrips(is);
    is >> _epromDescription;
  }

  void Fed9UDescription::loadSettings(std::istream& is) {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    static const std::string expectedVersion = VERSION_TEXT(Settings);
    std::string actualVersion;
    is >> actualVersion;
    ICUTILS_VERIFY(actualVersion == expectedVersion)(actualVersion)(expectedVersion).error()
      .msg("Wrong version number in description file, it may need to be updated");
    is >> _busAdaptorType;
    is >> _feFirmwareVersion;
    is >> _beFirmwareVersion;
    is >> _vmeFirmwareVersion;
    is >> _fedVersion;
    is >> _epromVersion;
    is >> _baseAddress;
    is >> _fed9UDaqMode;
    is >> _fed9UDaqSuperMode;
    is >> _scopeLength;
    is >> _triggerSource;
    is >> _testRegister;
    is >> _beUnit;
    is >> _fedDisable;
    is >> _fedId;
    is >> _fedHardwareId;
    is >> _readRoute;
    is >> _clockMode;
    is >> _beTempControl;
    is >> _vmeTempControl;
    is >> _crateNumber;
    is >> _vmeControllerDaisyChainId;
    is >> _ttcrxDescription;
    is >> _voltageController;
    is >> _globalFineSkew;
    is >> _globalCoarseSkew;
    is >> _optoRXResistor;
    is >> _fakeEventFile;
    is >> _fakeEventTriggerDelay;
    is >> _eventType;
    is >> _fov;
    is >> _headerType;
    is >> _bxOffset;
    for(int i=0; i<8; i++) {
      is >> _feParams[i];
    }
    
    //
    // global fine skew initialised here. temporary bodge to avoid .fed file conflicts...
    // should be overridden by the xml load.
    //
    _globalFineSkew=0;
    _globalCoarseSkew=0;
    
  }

  void Fed9UDescription::loadStrips(std::istream& is) {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    static const std::string expectedVersion = VERSION_TEXT(Strips);
    std::string actualVersion;
    is >> actualVersion;
    ICUTILS_VERIFY(actualVersion == expectedVersion)(actualVersion)(expectedVersion).error()
      .msg("Wrong version number in strips, they may need to be updated");
    is >> _strips;
  }

  void Fed9UDescription::saveDescription(std::ostream & os) const {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    os << VERSION_TEXT(Description) << endl;
    os << _name << endl; 
    saveSettings(os);
    saveStrips(os);
    os << _epromDescription << endl;
  }

  void Fed9UDescription::saveSettings(std::ostream& os) const {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    os << VERSION_TEXT(Settings) << endl;
    os << _busAdaptorType << endl;
    os << _feFirmwareVersion << endl;
    os << _beFirmwareVersion << endl;
    os << _vmeFirmwareVersion << endl;
    os << _fedVersion << endl;
    os << _epromVersion << endl;
    os << _baseAddress << endl;
    os << _fed9UDaqMode << endl;
    os << _fed9UDaqSuperMode << endl;
    os << _scopeLength << endl;
    os << _triggerSource << endl;
    os << _testRegister << endl;
    os << _beUnit << endl;
    os << _fedDisable << endl;
    os << _fedId << endl;
    os << _fedHardwareId << endl;
    os << _readRoute << endl;
    os << _clockMode << endl;
    os << _beTempControl << endl;
    os << _vmeTempControl << endl;
    os << _crateNumber << endl;
    os << _vmeControllerDaisyChainId << endl;
    os << _ttcrxDescription << endl;
    os << _voltageController << endl;
    os << _globalFineSkew << endl;
    os << _globalCoarseSkew << endl;
    os << _optoRXResistor << endl;
    os << _fakeEventFile << endl;
    os << _fakeEventTriggerDelay << endl;
    os << _eventType << endl;
    os << _fov << endl;
    os << _headerType << endl;
    os << _bxOffset << endl;
    for(int i=0; i<8; i++) {
      os << _feParams[i] << endl;
    }
  }

  void Fed9UDescription::saveStrips(std::ostream& os) const {
    // NOTE: if any changes are made to the description file,
    // the DESCRIPTION_VERSION define at the top of this file
    // must be incremented.
    os << VERSION_TEXT(Strips) << endl;
    os << _strips;
  }

  void Fed9UDescription::loadDefaultSettings() {
    _crateNumber = 0;
    _vmeControllerDaisyChainId = 0;
    //_name=std::string("default");
    char deft[8]= {'d','e','f','a','u','l','t'};
    memcpy ( &(_name[0]),&(deft[0]),8);
    _busAdaptorType=FED9U_HAL_BUS_ADAPTOR_CAEN_PCI;
    _feFirmwareVersion=0;
    _beFirmwareVersion=0;
    _vmeFirmwareVersion=0;
    _fedVersion=0;
    _epromVersion=2;
    _baseAddress=0;
    _fed9UDaqMode=FED9U_MODE_SCOPE;
    _fed9UDaqSuperMode=FED9U_SUPER_MODE_NORMAL;
    _scopeLength=32;
    _triggerSource=FED9U_TRIG_SOFTWARE;
    _testRegister=0;
    //_beFpgaMode=0;
    _beUnit=0;
    _fedDisable=true;
    _fedId=0;
    _fedHardwareId=0;
    _readRoute=FED9U_ROUTE_VME;
    _clockMode=FED9U_CLOCK_INTERNAL;
    _beTempControl = Fed9UTempControl();
    _vmeTempControl = Fed9UTempControl();
    _ttcrxDescription = Fed9UTtcrxDescription();
    _voltageController = Fed9UVoltageControl();
    _epromDescription = Fed9UEpromDescription(_epromVersion);
    _optoRXResistor = 60;
    char deft2[13]= {'f','a','k','e','E','v','e','n','t','.','f','e','v'};
    memcpy ( &(_name[0]),&(deft2[0]),13);
    //    _fakeEventFile = "fakeEvent.fev";
    _fakeEventTriggerDelay = 0;
    _eventType = 0;
    _fov = 0;
    _headerType = FED9U_HEADER_APVERROR;
    _bxOffset = 1;
    for(int i=0; i<8; i++) {
      _feParams[i] = Fed9UFrontEndDescription(0x8, 0xf, 0x1f, 0x6);
    }
    
    //
    // global fine skew initialised here...
    //
    _globalFineSkew=0;
    _globalCoarseSkew=0;
  }

  void Fed9UDescription::loadDefaultStrips() {
    _strips.loadDefaultStrips();
  }

  //<JF date=18/03/2004>
  //Add get required buffer size.
  u32 Fed9UDescription::getRequiredBufferSize() const {
    u32 estimate=0; // will return number of u32s in data buffer
    u32 daqHeader = 8; // number of bytes in daq Header
    u32 trackerSpecial = 8; //Number of bytes in tracker special header //Matthew Pearson Jan 05
    u32 daqTrailer = 8; //  number of bytes in daq Trailer
    u32 trackerHeader = 8 << 4; // number of bytes in tracker Header
    u32 rubbish = 4; // occasionally there is one extra 32 bit word of rubbish

    u32 noFEsEnabled=0;
    for( int i=0; i < FEUNITS_PER_FED; i++ ){
      if(!_feParams[i]._feUnitDisable)
        noFEsEnabled++;  
    }
    u32 payload = 0;
    if ( _fed9UDaqMode == FED9U_MODE_SCOPE )
      payload= ( (( _scopeLength << 1 ) + 3) * 12 );
    else
      payload= ( ((140 << 2)+3) * 12 );

    payload = (payload + 7) & ~7;
    payload *= noFEsEnabled;
    //
    // create the estimate by summing the payload length with the length of the
    // header and the trailer
    // 
    estimate = ( payload + daqHeader + trackerSpecial + daqTrailer + trackerHeader + rubbish) >> 2;
    //Matthew Pearson Jan 05 - modifed above line to take into account extra tracker special header word.
    
    return estimate;
  }
  // <NAC date="24/04/2007"> operator to compare descriptions
  bool operator == (const Fed9UDescription& l, const Fed9UDescription& r) {
    Fed9UAddress addr;
    if (l.getEpromVersion() != r.getEpromVersion()) return false;
    if (l.getBusAdaptorType() != r.getBusAdaptorType()) return false;
    if (l.getFedVersion() != r.getFedVersion()) return false;
    if (l.getFeFirmwareVersion() != r.getFeFirmwareVersion()) return false;
    if (l.getBeFirmwareVersion() != r.getBeFirmwareVersion()) return false;
    if (l.getVmeFirmwareVersion() != r.getVmeFirmwareVersion()) return false;
    if (l.getDelayFirmwareVersion() != r.getDelayFirmwareVersion()) return false;
    if (l.getBaseAddress() != r.getBaseAddress()) return false;
    if (l.getScopeLength() != r.getScopeLength()) return false;
    if (l.getTriggerSource() != r.getTriggerSource()) return false;
    if (l.getTestRegister() != r.getTestRegister()) return false;
    if (l.getFedBeUnit() != r.getFedBeUnit()) return false;
    if (l.getFedBeFpgaDisable() != r.getFedBeFpgaDisable()) return false;
    if (l.getFedId() != r.getFedId()) return false;
    if (l.getFedHardwareId() != r.getFedHardwareId()) return false;
    if (l.getBeFpgaReadRoute() != r.getBeFpgaReadRoute()) return false;
    if (l.getDaqMode() != r.getDaqMode()) return false;
    if (l.getDaqSuperMode() != r.getDaqSuperMode()) return false;
    if (l.getClock() != r.getClock()) return false;
    if (l.getCrateNumber() != r.getCrateNumber()) return false;
    if (l.getVmeControllerDaisyChainId() != r.getVmeControllerDaisyChainId()) return false;
    if (l.getSlotNumber() != r.getSlotNumber()) return false; //required?
    if (l.getName() != r.getName()) return false;
    if (l.getHalAddressTable() != r.getHalAddressTable()) return false;
    if (l.getGlobalFineSkew() != r.getGlobalFineSkew()) return false;
    if (l.getGlobalCoarseSkew() != r.getGlobalCoarseSkew()) return false;
    if (l.getOptoRXResistor() != r.getOptoRXResistor()) return false;
    if (l.getFakeEventFile() != r.getFakeEventFile()) return false;
    if (l.getFakeEventTriggerDelay() != r.getFakeEventTriggerDelay()) return false;
    if (l.getDaqEventType() != r.getDaqEventType()) return false;
    if (l.getDaqFov() != r.getDaqFov()) return false;
    if (l.getHeaderFormatType() != r.getHeaderFormatType()) return false;
    if (l.getBunchCrossingOffset() != r.getBunchCrossingOffset()) return false;
    addr.setFedFpga(Fed9UAddress::BACKEND);
    if (l.getTempControl(addr) != r.getTempControl(addr)) return false;
    addr.setFedFpga(Fed9UAddress::VME);
    if (l.getTempControl(addr) != r.getTempControl(addr)) return false;
    if (l.getTtcrx() != r.getTtcrx()) return false;
    if (l.getEprom() != r.getEprom()) return false;
    if (l.getVoltageMonitor() != r.getVoltageMonitor()) return false;
    for (int f=0; f<FEUNITS_PER_FED; f++) {
      addr.setFedFeUnit(f);
      if (l.getFrontEndDescription(addr) != r.getFrontEndDescription(addr)) return false;
    }
    if (l.getFedStrips() != r.getFedStrips()) return false;
    return true;
  }
  // </NAC>
  // <NAC date="25/04/2007>
  Fed9UFrontEndDescription Fed9UDescription::getFrontEndDescription(const Fed9UAddress& addr) const
  {
    return _feParams[addr.getFedFeUnit()];
  }
  Fed9UDescription& Fed9UDescription::setFrontEndDescription(const Fed9UAddress& addr, const Fed9UFrontEndDescription& theNewFrontEndDescription)
  {
    _feParams[addr.getFedFeUnit()] = theNewFrontEndDescription;
    return *this;
  }
  // </NAC>
}
//</JF>      
