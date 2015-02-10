#include <stdint.h>
#include <inttypes.h>

#include "Fed9UDescriptionCreator.hh"
#include <string>
#include "TypeDefs.hh"
#include "ICExImpl.hh"
#include <cstdlib>

//using namespace std;


namespace Fed9U {

  Fed9UDescriptionCreator::Fed9UDescriptionCreator(Fed9UDescription* ptr_desc):
    _ptr_desc(ptr_desc){
  }
  
  void  Fed9UDescriptionCreator::menu(){
    char choice;
    bool again(true);
    try {
      do {
	std::cout << "You are currently editing description: " << _ptr_desc->getName() << "\n"
	     << "########################################################################################\n"
	     << "#--------------------------------------------------------------------------------------#\n"
	     << "#----------------------- FED Description Creator Menu ---------------------------------#\n"
	     << "#--------------------------------------------------------------------------------------#\n"
	     << "########################################################################################\n"
	     << "# Please enter a choice:                                                               #\n"
	     << "#                                                                                      #\n"
             << "#  READ OPTIONS:                               FRONT-END SETTINGS:                     #\n"
             << "#                                                                                      #\n"
	     << "#  0  Read Description Version                 E  Set Delays                           #\n"
	     << "#  1  Read Main Settings                       F  Set Trim Dac Offset                  #\n"
	     << "#  2  Read Front End or Back End Settings      G  Set Frame Thresholds                 #\n"
	     << "#  3  Read Channel Settings                    H  Set ADC controls                     #\n"
             << "#  4  Read Strip Settings                      I  Set Disable Apv flags                #\n"
             << "#                                              J  Set Opto RX Input Offset             #\n"
             << "#  MAIN SETTINGS:                              K  Set Opto RX Output Offset            #\n"
             << "#                                              L  Set Opto RX Capacitors               #\n"
	     << "#  5  Set VME Base Address                     M  Set Median Overrides                 #\n"
	     << "#  6  Set Test Register                        N  Set Median Override Disables         #\n"
	     << "#  7  Set Disable Fed                          O  Set Front End Unit Disables          #\n"
	     << "#  8  Set Fed ID                               P  Set Front End Temp Controls          #\n"
	     << "#  9  Set Trigger Source                       S  Set Strips                           #\n"
	     << "#  A  Set Clock Source                         U  Set APV Fake Event Disable           #\n"
	     << "#  B  Set Read Route                                                                   #\n"
	     << "#  C  Set Daq and Super Daq Mode               OTHER SETTINGS (BACK-END, POWER, ETC.)  #\n"
	     << "#  D  Set Scope Length                                                                 #\n"
             << "#  T  Set Description Name                      R  Set Back End Temp Controls          #\n"
             << "#                                                                                      #\n"
	     << "#  Q  Quit                                                                             #\n"
	     << "#                                                                                      #\n"
	     << "########################################################################################" << std::endl;
	std::cin >> std::dec >> choice;
	
	switch (tolower(choice)) {
	//Read Description Version
	case '0' : {
	  std::cout << "\n\n ###" << std::endl; //_ptr_desc->getVersion() << "\n\n" << std::endl;
	  break;
	}
	//Read Main Settings
	case '1' : {
	  std::string trigger, route, clock, mode;
	  if(_ptr_desc->getTriggerSource() == FED9U_TRIG_SOFTWARE){
	    trigger = "Software";
	  } else if (_ptr_desc->getTriggerSource() == FED9U_TRIG_BACKPLANE){
	    trigger = "Backplane";
	  } else {
	    trigger = "TTC";
	  }
	  
	  if(_ptr_desc->getBeFpgaReadRoute() == FED9U_ROUTE_VME){
	    route = "VME";
	  } else {
	    route = "S-Link";
	  }
	  
	  if(_ptr_desc->getClock() == FED9U_CLOCK_INTERNAL){
	    clock = "Internal";
	  } else if (_ptr_desc->getClock() == FED9U_CLOCK_BACKPLANE){
	    clock = "Backplane";
	  } else {
	    clock = "TTC";
	  }

	  if(_ptr_desc->getDaqMode() == FED9U_MODE_SCOPE){
	    mode = "Scope";
	  } else if (_ptr_desc->getDaqMode() == FED9U_MODE_VIRGIN_RAW){
	    mode = "Virgin Raw";
	  } else if (_ptr_desc->getDaqMode() == FED9U_MODE_ZERO_SUPPRESSED){
	    mode = "Zero Suppressed";
	  } else {
	    mode = "Processed Raw";
	  }
	  
	  std::cout << "\n\n####################################################################\n"
	       << "# Main Settings read back from description, all values are in Hex\n"
	       << "#  Base Address:   " << std::hex << _ptr_desc->getBaseAddress() << "\n"
	       << "#  Test Register:  " << _ptr_desc->getTestRegister() << "\n"
	       << "#  Disable FED:    " << _ptr_desc->getFedBeFpgaDisable()   << "\n"
	       << "#  Fed ID:         " << _ptr_desc->getFedId()        << "\n"
	       << "#  Trigger Source: " << trigger                      << "\n"
	       << "#  Clock Source:   " << clock                        << "\n"
	       << "#  Read Route:     " << route                        << "\n"
	       << "#  Daq Mode:       " << mode                         << "\n"
	       << "#  Scope Length:   " << _ptr_desc->getScopeLength()  << "\n"
	       << "\n\n###############################################################\n" << std::endl;
	  break;
	}
	//Read Front End or Back End Settings
	case '2' : {
          std::cout << "Type in the front end unit you want to read (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << ", or 10 for all front ends, or 11 for back end)?" << std::endl;
          u16 unit = 0;
          std::cin >> std::dec >> unit;
	  
	  if (unit<=Fed9UAddress().getCountBase()+7) {
	    displayFeSettings(Fed9UAddress().setExternalFedFeUnit(unit).getFedFeUnit());
	  }
	  else if (unit==10) {
	    for (u16 fe=0; fe<=7; ++fe) {
	      displayFeSettings(fe);
	    }
	  }
          else if (unit==11) {
	    displayBeSettings();
	  }
	  break;
	}
	//Read Channel Settings
	case '3' : {
	  std::cout << "Type in the channel you want to read (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << ", or 100 for all channels)?" << std::endl;
          u16 channel = 0;
          std::cin >> std::dec >> channel;
          if (channel<=Fed9UAddress().getCountBase()+95) {
	    displayChannelSettings(Fed9UAddress().setExternalFedChannel(channel).getFedChannel());
	  }
          else if (channel==100) {
            for (u16 ch=0; ch<=95; ++ch) {
	      displayChannelSettings(ch);
	    }
	  }
	  break;
	}
	//Read strip settings
	case '4' : {
          std::cout << "Type in the strip you want to read (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+24575 << ", or 25000 for all strips)?" << std::endl;
          u16 strip = 0;
          std::cin >> std::dec >> strip;
          if (strip<=Fed9UAddress().getCountBase()+24575) {
	    displayStripSettings(Fed9UAddress().setExternalFedStrip(strip).getFedStrip());
	  }
          else if (strip==25000) {
            for (u16 st=0; st<=24575; ++st) {
	      displayStripSettings(st);
	    }
	  }
	  break;
	}
        //Set VME Base Address
	case '5' : {
	  std::cout << "\nType in the VME base addres (in hex format):" << std::endl;
          unsigned long value = 0x0;
          std::cin >> std::hex >> value;
          _ptr_desc->setBaseAddress(value);
	  break;
	}
	//Set Test Register
	case '6' : {
          std::cout << "\nType in a value for the test register (in hex format):" << std::endl;
          unsigned long value = 0x0;
          std::cin >> std::hex >> value;
	  _ptr_desc->setTestRegister(value);
	  break;
	}
	//Set Disable Fed
	case '7' : {
          std::cout << "Type in:" << std::endl;
          std::cout << "   0 to enable the FED" << std::endl;
          std::cout << "   1 to disable the FED" << std::endl;
          char value;
          std::cin >> value;
          _ptr_desc->setFedBeFpgaDisable(atoi(&value)); 
	  break;
	}
	//Set Fed ID
	case '8' : {
	  std::cout << "Type in a FED ID number (in hex format): " << std::endl;
          unsigned long value = 0x0;
          std::cin >> std::hex >> value;
	  _ptr_desc->setFedId(value);
	  break;
	}
	//Set Trigger Source  ****************************************************
	case '9' : {  
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set Trigger Source:                                              #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# 1 = Software                                                     #\n"
	       <<     "# 2 = Backplane                                                    #\n"
	       <<     "# 3 = TTC                                                          #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	    std::cin >> choice;
	  switch (choice){
	  case '1':{
	    _ptr_desc->setTriggerSource(FED9U_TRIG_SOFTWARE);
	    break;
	  }
	  case '2':{
	    _ptr_desc->setTriggerSource(FED9U_TRIG_BACKPLANE);
	    break;
	  }
	  case '3':{
	    _ptr_desc->setTriggerSource(FED9U_TRIG_TTC);
	    break;
	  }
	  default:{
	    break;
	  }
	  }
	  //End of setting triggers source ****************************************************
	  break;
	}
	//Set Clock Source *********************************************************
	case 'a' : {
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set Clock Source:                                                #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# 1 = Internal                                                     #\n"
	       <<     "# 2 = Backplane                                                    #\n"
	       <<     "# 3 = TTC                                                          #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	    std::cin >> choice;
	  switch (choice){
	  case '1':{
	    _ptr_desc->setClock(FED9U_CLOCK_INTERNAL);
	    break;
	  }
	  case '2':{
	    _ptr_desc->setClock(FED9U_CLOCK_BACKPLANE);
	    break;
	  }
	  case '3':{
	    _ptr_desc->setClock(FED9U_CLOCK_TTC);
	    break;
	  }
	  default:{
	    break;
	  }
	  }
	  //End of setting clock source ****************************************************
	  break;
	}
	//Set Read Route
	case 'b' : {
           std::cout << "\n\n####################################################################\n"
	       <<     "# Set Read Route:                                                  #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# 1 = VME                                                          #\n"
	       <<     "# 2 = S-Link64                                                     #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	    std::cin >> choice;
	  switch (choice){
          case '1': {
            _ptr_desc->setBeFpgaReadRoute(FED9U_ROUTE_VME);
	    break;
	  }
          case '2': {
            _ptr_desc->setBeFpgaReadRoute(FED9U_ROUTE_SLINK64);
	    break;
	  }
	  default: {
	    break;
	  }
	  }
	  break;
	}
	//Set Daq Mode *********************************************************
	case 'c' : {
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set DAQ Mode                                                     #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# 1 = Scope                                                        #\n"
	       <<     "# 2 = Virgin Raw                                                   #\n"
	       <<     "# 3 = Processed Raw                                                #\n"
	       <<     "# 4 = Zero Suppressed                                              #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	    std::cin >> choice;
	  switch (choice){
	  case '1':{
	    _ptr_desc->setDaqMode(FED9U_MODE_SCOPE);
	    break;
	  }
	  case '2':{
	    _ptr_desc->setDaqMode(FED9U_MODE_VIRGIN_RAW);
	    break;
	  }
	  case '3':{
	    _ptr_desc->setDaqMode(FED9U_MODE_PROCESSED_RAW);
	    break;
	  }
	  case '4':{
	    _ptr_desc->setDaqMode(FED9U_MODE_ZERO_SUPPRESSED);
	    break;
	  }
	  default:{
	    break;
	  }
	  }
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set Super DAQ Mode                                               #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# 1 = Normal                                                       #\n"
	       <<     "# 2 = Fake                                                         #\n"
	       <<     "# 3 = Zero suppressed lite normal                                  #\n"
	       <<     "# 4 = Zero Suppressed lite fake                                    #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	  std::cin >> choice;
	  switch (choice){
	  case '1':{
	    _ptr_desc->setDaqSuperMode(FED9U_SUPER_MODE_NORMAL);
	    break;
	  }
	  case '2':{
	    _ptr_desc->setDaqSuperMode(FED9U_SUPER_MODE_FAKE);
	    break;
	  }
	  case '3':{
	    _ptr_desc->setDaqSuperMode(FED9U_SUPER_MODE_ZERO_LITE);
	    break;
	  }
	  case '4':{
	    _ptr_desc->setDaqSuperMode(FED9U_SUPER_MODE_FAKE_ZERO_LITE);
	    break;
	  }
	  default:{
	    break;
	  }
	  }

	  //End of setting DAQ & SUPER DAQ Mode ****************************************************
	  break;
	}
	//Set Scope Length
	case 'd' : {
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set Scope Length                                                 #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# Enter a value in std::decimal:                                        #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	  std::string value;
	  std::cin >> value;
	  _ptr_desc->setScopeLength(atoi(value.c_str()));
	  break;
	}
	//Set Delays
	case 'e' : {
          bool done(false);
          while(!done) {
          std::cout << "\n\n####################################################################\n"
	       <<     "# Set coarse and fine delays                                       #\n"
	       <<     "#                                                                  #\n"	 
               <<     "# Q = Return                                                       #\n"
	       <<     "# Enter the channel number to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "):                          #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
	  std::cout << "Type in a coarse delay (in std::decimal): " << std::endl;
          u16 coarse = 0x0;
          std::cin >> std::dec >> coarse;
          std::cout << "Type in a fine delay (in std::decimal): " << std::endl;
          u16 fine = 0x0;
          std::cin >> std::dec >> fine;
	  _ptr_desc->setDelay(Fed9UAddress().setExternalFedChannel(static_cast<u8>(atoi(channel.c_str()))), coarse, fine);
	  }
	  break;
	}
	//Set Trim Dac Offset
	case 'f' : {
          bool done(false);
          while(!done) {
          std::cout << "\n\n####################################################################\n"
	       <<     "# Set TrimDAC offset                                               #\n"
	       <<     "#                                                                  #\n"
               <<     "# Q = Return                                                       #\n"
	       <<     "# Enter the channel number to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "):                          #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
          std::cout << "Type in a value for the offset (0-255): " << std::endl;
          u16 value = 0;
          std::cin >> std::dec >> value;
	  _ptr_desc->setTrimDacOffset(Fed9UAddress().setExternalFedChannel(static_cast<u8>(atoi(channel.c_str()))), value);
	  }
	  break;
	}
	//Set Frame Thresholds
	case 'g' : {
	  bool done(false);
	  while(!done){
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Frame Thresholds:                                                  #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the numer of the channel to set:                           #\n"
		 <<     "# " << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "                                                           #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string value;
	    std::cin >> value;
	    if (tolower(value.c_str()[0]) == 'q') break;
	    if (atoi(value.c_str()) < 0 || atoi(value.c_str()) > 95) {
	      std::cout << "invalid channel" << std::endl;
	      continue;
	    }
	    u16 channel = atoi(value.c_str());
	    std::cout << " Enter threshold, 0 - 1023: " << std::endl;
	    std::cin >> value;
	    if (atoi(value.c_str()) < 0 || atoi(value.c_str()) > 1023) {
	      std::cout << "invalid threshold" << std::endl;
	      continue;
	    }
	   _ptr_desc->setFrameThreshold(Fed9UAddress().setExternalFedChannel(static_cast<u8>(channel)),atoi(value.c_str()));
	  }
	  break;
	}
	//Set ADC controls
	case 'h' : {
	  bool done(false);
          while(!done) {
           std::cout << "\n\n####################################################################\n"
	       <<     "# Set the ADC controls                                             #\n"
	       <<     "#                                                                  #\n"	
       	       <<     "# Q = Return                                                       #\n"
	       <<     "# Enter the channel number to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "):                          #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
          std::cout << "Type in 1 to set 1V mode, or 2 for 2V mode:" << std::endl;
          u16 choice = 0;
          std::cin >> std::dec >> choice;
          switch (choice) {
	  case '1' : {
            _ptr_desc->setAdcControls(Fed9UAddress().setExternalFedChannel(static_cast<u8>(atoi(channel.c_str()))), Fed9UAdcControls(true, false, true, false));
	    break;
	  }
	  case '2': {
            _ptr_desc->setAdcControls(Fed9UAddress().setExternalFedChannel(static_cast<u8>(atoi(channel.c_str()))), Fed9UAdcControls(false, false, true, false));
	    break;
	  }
	  default: {
	    break;
	  }
	  }
	  }
	  break;
	}
	//Set Disable Apv flags
	case 'i' : {
	  bool done(false);
          while(!done) {
	   std::cout << "\n\n####################################################################\n"
	       <<     "# Set the APV disable flags                                        #\n"
	       <<     "#                                                                  #\n"	
	       <<     "# Q = Return                                                       #\n" 
	       <<     "# First enter the channel number (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "):                           #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
          Fed9UAddress theFed9UAddress;
	  theFed9UAddress.setExternalFedChannel(static_cast<u8>(atoi(channel.c_str())));
          //Set APV0
          theFed9UAddress.setChannelApv(0);
          setApvDisableFlag(theFed9UAddress);
          //Set APV1
          theFed9UAddress.setChannelApv(1);
          setApvDisableFlag(theFed9UAddress);
	  }
	  break;
	}
        //Set Opto RX Input Offset
	case 'j' : {
          bool done(false);
	  while(!done) {
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Opto Rx input offset:                                        #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the number of the front end to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                  #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string feUnit;
            std::cin >> feUnit;
	    if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
	    Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
            std::cout << "Type in the value of the offset:";
            u16 offset;
            std::cin >> offset;
	    _ptr_desc->setOptoRxInputOffset(theFed9UAddress, offset);
	  }
	  break;
	}
        //Set Opto RX Output Offset
	case 'k' : {
	  bool done(false);
	  while(!done) {
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Opto Rx output offset:                                       #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the number of the front end to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                  #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string feUnit;
            std::cin >> feUnit;
	    if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
	    Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
            std::cout << "Type in the value of the offset:";
            u16 offset;
            std::cin >> offset;
	    _ptr_desc->setOptoRxOutputOffset(theFed9UAddress, offset);
	  }
	  break;
	}
        //Set Opto RX Capacitors
	case 'l' : {
	  bool done(false);
	  while(!done) {
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Opto Rx capacitor:                                           #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the number of the front end to set (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                  #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string feUnit;
            std::cin >> feUnit;
	    if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
            Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
            std::cout << "Type in the value:";
            u16 offset;
            std::cin >> offset;
	    _ptr_desc->setOptoRxCapacitor(theFed9UAddress, offset);
	  }
	  break;
	}
        //Set Median Overrides
	case 'm' : {
	  bool done(false);
          while(!done) {
	   std::cout << "\n\n####################################################################\n"
	       <<     "# Set the common mode median override values:                      #\n"
	       <<     "# (remember to enable to override in order to use this)            #\n"
	       <<     "#                                                                  #\n"	
	       <<     "# Q = Return                                                       #\n" 
	       <<     "# First enter the channel number (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+95 << "):                           #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
          Fed9UAddress theFed9UAddress;
	  theFed9UAddress.setExternalFedChannel(static_cast<u8>(atoi(channel.c_str())));
          std::cout << "Now type in the APV (0 or 1):";
          u16 value = 0;
          std::cin >> value;
          theFed9UAddress.setChannelApv(value);
          std::cout << "Now type in the value to set:";
          std::cin >> value;
          _ptr_desc->setMedianOverride(theFed9UAddress, value);
	  }
	break;
	}
        //Set Median Override Disables
        case 'n' : {
	  bool done(false);
	  while(!done) {
	    std::cout << "\n\n##################################################################\n"
                 << "# Set the median override:                                           #\n"
                 << "# (this will set the median override function for a whole front end) #\n"
                 << "#                                                                    #\n"
                 << "# Q = Return                                                         #\n"
                 << "# Enter the front end number (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                                  #\n"
                 << "#                                                                    #\n"
                 << "######################################################################\n"
                 << std::endl;
            std::string feUnit;
            std::cin >> feUnit;
            if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
            Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
            std::cout << "Type in: " << std::endl;
            std::cout << "    0 to enable the override" << std::endl;
            std::cout << "    1 to disable the override" << std::endl;
            char value;
            std::cin >> value;
            _ptr_desc->setMedianOverrideDisable(theFed9UAddress, atoi(&value));
	  }
	break;
	}
        //Set Front End Unit Disables
	case 'o' : {
	  bool done(false);
	  while(!done){
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Front End Unit Disables:                                     #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the front end number (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                                #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string feUnit;
            std::cin >> feUnit;
            if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
            Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
	    std::cout << "Type in: " << std::endl;
            std::cout << "    0 to enable the front end" << std::endl;
            std::cout << "    1 to disable the front end" << std::endl;
            char value;
            std::cin >> value;
            _ptr_desc->setFedFeUnitDisable(theFed9UAddress, atoi(&value));
	  }
	  break;
	}
        //Set Front End Temp Controls
	case 'p' : {
	  bool done(false);
	  while(!done){
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set Front End Unit Temperature Parameters:                       #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "# Q = Return                                                       #\n"
		 <<     "# Enter the front end number (" << Fed9UAddress().getCountBase() << "-" 
	       << Fed9UAddress().getCountBase()+7 << "):                                #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
	    std::string feUnit;
            std::cin >> feUnit;
            if (tolower(feUnit.c_str()[0]) == 'q') {
	      break;
	    }
            Fed9UAddress theFed9UAddress;
	    theFed9UAddress.setExternalFedFeUnit(static_cast<u8>(atoi(feUnit.c_str())));
             //Make a Fed9UTempControl to store the temperature parameters.
            Fed9UTempControl theFed9UTempControl;
            std::cout << "Type in the maximum temperature for the LM82:";
            u16 temp;
            std::cin >> temp;
            theFed9UTempControl.setLm82High(temp);
            std::cout << "Type in the maximum temperature for the front end FPGA:";
            std::cin >> temp;
            theFed9UTempControl.setFpgaHigh(temp);
	    std::cout << "Type in the critical temperature:";
            std::cin >> temp;
            theFed9UTempControl.setCritical(temp);
            //Pass the Fed9UTempControl into the Fed9UDescription. 
            _ptr_desc->setTempControl(theFed9UAddress, theFed9UTempControl);
	  }
	  break;
	}
        //Set Back End Temp Controls
	case 'r' : {
          std::cout << "\n\n####################################################################\n"
               <<     "# Set Back End Unit Temperature Parameters:                        #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	  Fed9UAddress theFed9UAddress;
	  theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND);
          //Make a Fed9UTempControl to store the temperature parameters.
          Fed9UTempControl theFed9UTempControl;
          std::cout << "Type in the maximum temperature for the LM82:";
          u16 temp;
          std::cin >> temp;
          theFed9UTempControl.setLm82High(temp);
          std::cout << "Type in the maximum temperature for the back end FPGA:";
          std::cin >> temp;
          theFed9UTempControl.setFpgaHigh(temp);
	  std::cout << "Type in the critical temperature:";
          std::cin >> temp;
          theFed9UTempControl.setCritical(temp);
          //Pass the Fed9UTempControl into the Fed9UDescription. 
          _ptr_desc->setTempControl(theFed9UAddress, theFed9UTempControl);
	  break;
	}
        //Set Strips
	case 's' : {
          bool done(false);
          while(!done) {
 	    std::cout << "\n######################################\n"
                 << "# Set Strips                         #\n"
                 << "# 1 = Set a single strip             #\n"
                 << "# 2 = Set all strips on an APV       #\n"
                 << "# 3 = Set all strips for a front end #\n"
                 << "# 4 = Set all strips for a FED       #\n"
                 << "# Q = Quit Menu                      #\n"
                 << "######################################\n" << std::endl;
            char option;
            std::cin >> option;
            if (tolower(option) == 'q') {
	      break;
	    }
            Fed9UAddress theFed9UAddress;
            std::vector<u16> stripParameters;
            switch(tolower(option)) {
	    case '1': {
              std::cout << "Type in the strip number (" << theFed9UAddress.getCountBase() << "-" << (theFed9UAddress.getCountBase()+24575) << "): " << std::endl;
              u16 strip;
              std::cin >> strip;
              theFed9UAddress.setExternalFedStrip(strip); 
              askForStripParameters(stripParameters);
              setStripParameters(stripParameters, theFed9UAddress);
	      break;
	    }
	    case '2': {
              std::cout << "Type in the APV (" << theFed9UAddress.getCountBase() << "-" << theFed9UAddress.getCountBase()+191 << "): ";
              u16 apv;
              std::cin >> apv;
              theFed9UAddress.setExternalFedApv(apv);
              askForStripParameters(stripParameters);
              for (int i=0; i<=(STRIPS_PER_APV-1); ++i) { //Loop over each strip for this APV.
                theFed9UAddress.setApvStrip(i);
                setStripParameters(stripParameters, theFed9UAddress);
	      }
	      break;
	    }
	    case '3': {
              std::cout << "Type in the front end (" << theFed9UAddress.getCountBase() << "-" << theFed9UAddress.getCountBase()+7 << "): ";
              u16 feUnit;
              std::cin >> feUnit;
              theFed9UAddress.setExternalFedFeUnit(feUnit);
	      askForStripParameters(stripParameters);
              for (int i=0; i<=((APVS_PER_CHANNEL*CHANNELS_PER_FEUNIT)-1); ++i) {  //Loop over each APV for this FE unit.
                theFed9UAddress.setFeUnitApv(i);
                for (int j=0; j<=(STRIPS_PER_APV-1); ++j) {
                  theFed9UAddress.setApvStrip(j);
                  setStripParameters(stripParameters, theFed9UAddress);
		}
	      }
	      break;
	    }
	    case '4': {
              askForStripParameters(stripParameters);
              for (int i=0; i<=(STRIPS_PER_FED-1); ++i) {
                theFed9UAddress.setFedStrip(i);
                setStripParameters(stripParameters, theFed9UAddress);
	      }
	      break;
	    }
	    default: {
	      break;
	    }
	    }
          }        
	  break;
	}
	  //Set APV fake event disable.
	case 'u' : {
	  bool done(false);
          while(!done) {
	    std::cout << "\n\n####################################################################\n"
		 <<     "# Set the APV fake event disable flags                             #\n"
		 <<     "#                                                                  #\n"	
		 <<     "# Q = Return                                                       #\n" 
		 <<     "# First enter the channel number (" << Fed9UAddress().getCountBase() << "-" 
		 << Fed9UAddress().getCountBase()+95 << "):                           #\n"
		 <<     "#                                                                  #\n"	       
		 <<     "####################################################################\n"
		 << std::endl;
          std::string channel;
          std::cin >> channel;
          if (tolower(channel.c_str()[0]) == 'q') {
	    break;
	  }
          Fed9UAddress theFed9UAddress;
	  theFed9UAddress.setExternalFedChannel(static_cast<u8>(atoi(channel.c_str())));
          //Set APV0
          theFed9UAddress.setChannelApv(0);
          setApvFakeEventDisableFlag(theFed9UAddress);
          //Set APV1
          theFed9UAddress.setChannelApv(1);
          setApvFakeEventDisableFlag(theFed9UAddress);
	  }
	  break;
	}
        //Set Description Name
	case 't' : {
	  std::cout << "\n\n####################################################################\n"
	       <<     "# Set Name                                                         #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "# Enter a name:                                                    #\n"
	       <<     "#                                                                  #\n"	       
	       <<     "####################################################################\n"
	       << std::endl;
	  std::string name;
	  std::cin >> name;
	  _ptr_desc->setName(name);
	  break;
	}
	//Quit
	case 'q' : {
	  again = false;
	  break;
	}
	default:{
	  break;
	}
	}
      } while(again);
    }catch(ICUtils::ICException & e) {
      std::cout << e.what() << std::endl;
    }
  }


  void Fed9UDescriptionCreator::displayFeSettings(u16 feUnit)
  {
    // feUnit contains the internal representation of the feUnit number
    //Set the FE address.
     Fed9UAddress theFed9UAddress;
     theFed9UAddress.setFedFeUnit(feUnit);
     //Initialize a Fed9UTempControl object from the Fed9UDescription.
     Fed9UTempControl theFed9UTempControl = _ptr_desc->getTempControl(theFed9UAddress);
     std::cout << "\n\n###################################################################\n"
          << "# Settings for FE FPGA unit " << feUnit << ".\n"
          << "# Disable:             " << std::dec << _ptr_desc->getFedFeUnitDisable(theFed9UAddress)   << "\n" 
          << "# OptoRx input offset: " << _ptr_desc->getOptoRxInputOffset(theFed9UAddress)         << "\n"
          << "# OptoRx output offset: " << _ptr_desc->getOptoRxOutputOffset(theFed9UAddress)       << "\n"
          << "# OptoRx capacitor: " << _ptr_desc->getOptoRxCapacitor(theFed9UAddress)              << "\n"
          << "# Max temp for the LM82 chip: " << theFed9UTempControl.getLm82High()                 << "\n"
          << "# Max temp for the FE FPGA: " << theFed9UTempControl.getFpgaHigh()                   << "\n"
          << "# Critical temp for the FE FPGA: " << theFed9UTempControl.getCritical()              << "\n" 
          << "# Median override disable: " << _ptr_desc->getMedianOverrideDisable(theFed9UAddress) << "\n"
          << "#######################################################################\n" << std::endl;
  }


  void Fed9UDescriptionCreator::displayBeSettings(void)
  {
    //Set the BE unit address
    Fed9UAddress theFed9UAddress;
    theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND);
    //Initialize a Fed9UTempControl object from the Fed9UDescription.
    Fed9UTempControl theFed9UTempControl = _ptr_desc->getTempControl(theFed9UAddress);
    std::cout << "\n\n###################################################################\n"
         << "# Settings for BE FPGA.\n"
         << "# Max temp for the LM82 chip: " << std::dec << theFed9UTempControl.getLm82High()    << "\n"
         << "# Max temp for the FE FPGA: " << theFed9UTempControl.getFpgaHigh()      << "\n"
         << "# Critical temp for the FE FPGA: " << theFed9UTempControl.getCritical() << "\n"
         << "#######################################################################\n" << std::endl; 
  }


  void Fed9UDescriptionCreator::displayChannelSettings(u16 channel)
  {
    // channel is the internal representation of the Fed Channel number
    //Set the channel address.
    Fed9UAddress theFed9UAddress;
    theFed9UAddress.setFedChannel(static_cast<u8>(channel));
    //Intialize a Fed9UAdcControls object from the Fed9UDescription.
    Fed9UAdcControls theFed9UAdcControls = _ptr_desc->getAdcControls(theFed9UAddress);
    std::cout << "\n\n###################################################################\n"
         << "# Settings for channel " << channel << ".\n"
         << "# Coarse delay:        " << std::dec << _ptr_desc->getCoarseDelay(theFed9UAddress) << "\n"
	 << "# Fine delay:          " << _ptr_desc->getFineDelay(theFed9UAddress)          << "\n"
         << "# Frame threshold:     " << _ptr_desc->getFrameThreshold(theFed9UAddress)     << "\n"
         << "# TrimDAC offset:      " << _ptr_desc->getTrimDacOffset(theFed9UAddress)      << "\n"
         << "# Data complement:     " << _ptr_desc->getComplement(theFed9UAddress)         << std::endl;
    bool adcParam[4] = {false};
    adcParam[0] = theFed9UAdcControls._dfsen;
    adcParam[1] = theFed9UAdcControls._dfsval;
    adcParam[2] = theFed9UAdcControls._s1;
    adcParam[3] = theFed9UAdcControls._s2;
    if ((adcParam[0]==true)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
      std::cout << "# ADC mode:           1V" << std::endl;
    }
    else if ((adcParam[0]==false)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
      std::cout << "# ADC mode:           2V" << std::endl;
    }
    //Set the APV address
    theFed9UAddress.setChannelApv(0);
    std::cout << "# APV0 disable: " << _ptr_desc->getApvDisable(theFed9UAddress) << "  median override value: " << _ptr_desc->getMedianOverride(theFed9UAddress) << std::endl;
    theFed9UAddress.setChannelApv(1);
    std::cout << "# APV1 disable: " << _ptr_desc->getApvDisable(theFed9UAddress) << "  median override value: " << _ptr_desc->getMedianOverride(theFed9UAddress) << std::endl;
    std::cout << "#######################################################################\n" << std::endl;
  }


  void Fed9UDescriptionCreator::displayStripSettings(u16 strip)
  {
    //Set the strip address
    Fed9UAddress theFed9UAddress;
    Fed9UChannelInterface theFed9UChannelInterface;
    theFed9UAddress.setFedStrip(strip);
    //Initialize a Fed9UStripDescription from the Fed9UDescription
    const Fed9UStripDescription theFed9UStripDescription = (_ptr_desc->getFedStrips()).getStrip(theFed9UAddress);
    std::cout << "\n\n#######################################################################\n"
         << "# Settings for strip " << strip << ".\n"
         << "# Disable:                " << std::dec << theFed9UStripDescription.getDisable()    << "\n"
         << "# Pedestal:               " << theFed9UStripDescription.getPedestal()          << "\n"
         << "# Low cluster threshold:  " << theFed9UStripDescription.getLowThreshold()      << "\n"
         << "# High cluster threshold: " << theFed9UStripDescription.getHighThreshold()     << "\n"
         << "##########################################################################\n" << std::endl;
  }


  void Fed9UDescriptionCreator::setApvDisableFlag(const Fed9UAddress &theFed9UAddress)
  {
    if (theFed9UAddress.getChannelApv()==0) {
      std::cout << "For APV0 type in: " << std::endl;
    }
    else if (theFed9UAddress.getChannelApv()==1) {
      std::cout << "For APV1 type in: " << std::endl;
    }
    std::cout << "    0 to enable" << std::endl;
    std::cout << "    1 to disable " << std::endl;
    char apv;
    std::cin >> apv;
    _ptr_desc->setApvDisable(theFed9UAddress, atoi(&apv));
  }

  void Fed9UDescriptionCreator::setApvFakeEventDisableFlag(const Fed9UAddress &theFed9UAddress)
  {
    if (theFed9UAddress.getChannelApv()==0) {
      std::cout << "For APV0 type in: " << std::endl;
    }
    else if (theFed9UAddress.getChannelApv()==1) {
      std::cout << "For APV1 type in: " << std::endl;
    }
    std::cout << "    0 to enable" << std::endl;
    std::cout << "    1 to disable " << std::endl;
    char apv;
    std::cin >> apv;
    _ptr_desc->setApvFakeEventDisable(theFed9UAddress, atoi(&apv));
  }


  void Fed9UDescriptionCreator::askForStripParameters(std::vector<u16> &stripParameters)
  {
    //Clear vector
    if (stripParameters.size()!=0) {
      stripParameters.clear();
    }
    //Make four elements
    for (int i=0; i<=3; ++i) {
      stripParameters.push_back(0);
    }
    //Ask user to input strip parameters
    std::cout << "Type in:" << std::endl;
    std::cout << "   0 to enable" << std::endl;
    std::cout << "   1 to disable" << std::endl;
    std::cin >> stripParameters[0];
    //Skip the rest of the parameters if the user chooses to disable this strip.
    if (stripParameters[0]) {
      std::cout << "Other parameters will be set to zero." << std::endl;
    }
    else {
      std::cout << "Type in the pedestal: ";
      std::cin >> stripParameters[1];
      std::cout << "Type in the high cluster threshold: ";
      std::cin >> stripParameters[2];
      std::cout << "Type in the low cluster threshold: ";
      std::cin >> stripParameters[3];
    }
  }


  void Fed9UDescriptionCreator::setStripParameters(const std::vector<u16> &stripParameters, const Fed9UAddress &theFed9UAddress)
  {
    //Instantiate a Fed9UStripDescription
    Fed9UStripDescription theFed9UStripDescription = (_ptr_desc->getFedStrips()).getStrip(theFed9UAddress);
    //Set strip parameters
    theFed9UStripDescription.setDisable(stripParameters[0]);
    theFed9UStripDescription.setPedestal(stripParameters[1]);
    theFed9UStripDescription.setHighThreshold(stripParameters[2]);
    theFed9UStripDescription.setLowThreshold(stripParameters[3]);
    //Load strip description back into Fed9UDescription
    (_ptr_desc->getFedStrips()).setStrip(theFed9UAddress, theFed9UStripDescription);
  }

  


} //End of namespace Fed9U
