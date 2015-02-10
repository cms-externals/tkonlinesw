/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
/*
 *during the init method we only call setEnableTabs and set them to false
 *since no fed description object yet exists
 */



#define DESC_PTR fedAbcMap->getMap(static_cast<u16>(currentFedABC))[0]

void Fed9UDescriptionInterface::setCurrentFedAbc(int index) 
{
    cout << "ABC number " << currentFedABC << endl;
    currentFedABC = index;
}

int Fed9UDescriptionInterface::getCurrentFedAbc( )
{
    return currentFedABC;
}


void Fed9UDescriptionInterface::setAbcMap( Fed9U::Fed9UGuiAbcMap* guiMapPtr) 
{
    fedAbcMap = guiMapPtr;    
}

Fed9U::Fed9UGuiAbcMap* Fed9UDescriptionInterface::getAbcMap()
{
    return fedAbcMap;
}

void Fed9UDescriptionInterface::init()
{
    currentFedABC = -1;
    fedAbcMap = NULL;
    filename="";
    setEnableTabs(false);
    tabMainSettings->setCurrentPage(0);
}

void Fed9UDescriptionInterface::destroy()
{
}

void Fed9UDescriptionInterface::displayDescription()
{
    cout << "display main settings" << endl;
    displayMainSettings();
    cout << "display front End settings" << endl;
    displayFrontEndSettings();
}

void Fed9UDescriptionInterface::displayMainSettings()
{
    /***************************
      *Start by displaying the Main settings which generally apply to the BE FPGA
      ***************************/
    try {
 chkFedEnable->setChecked(!DESC_PTR->getFedBeFpgaDisable());
                crateNumber->setValue(DESC_PTR->getFed9UVMEDeviceDescription().getCrateNumber());
 slotNumber->setValue(DESC_PTR->getFed9UVMEDeviceDescription().getSlotNumber());
 testRegister->setValue(DESC_PTR->getTestRegister());
 fedID->setValue(DESC_PTR->getFedId());
 descriptionName->setText(QString(DESC_PTR->getFed9UVMEDeviceDescription().getName().c_str()));
 scopeLength->setValue(DESC_PTR->getScopeLength());
 Fed9U::Fed9UAddress addr;
 addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
 maxTempLM82->setValue( static_cast<int>( DESC_PTR->getTempControl(addr).getLm82High() ) );
 maxTempBeFpga->setValue( static_cast<int>( DESC_PTR->getTempControl(addr).getFpgaHigh() ) );
 maxTempCritical->setValue( static_cast<int>( DESC_PTR->getTempControl(addr).getCritical() ) );
 int clockSourceIndex(0);
 if (DESC_PTR->getClock() == Fed9U::FED9U_CLOCK_INTERNAL) 
     clockSourceIndex = 0;
 else if (DESC_PTR->getClock() == Fed9U::FED9U_CLOCK_BACKPLANE)
     clockSourceIndex = 1;
 else if (DESC_PTR->getClock() == Fed9U::FED9U_CLOCK_TTC)
     clockSourceIndex=2;
 else 
     QMessageBox::information(this, "Description Attribute Error", "Clock source value not valid!", 1);
 clockSource->setCurrentItem(clockSourceIndex);
    
 int daqModeIndex(0);
 if (DESC_PTR->getDaqMode() == Fed9U::FED9U_MODE_SCOPE) 
     daqModeIndex = 0;
 else if (DESC_PTR->getDaqMode() == Fed9U::FED9U_MODE_VIRGIN_RAW)
     daqModeIndex = 1;
 else if (DESC_PTR->getDaqMode() == Fed9U::FED9U_MODE_PROCESSED_RAW)
 daqModeIndex = 2;
 else if (DESC_PTR->getDaqMode() == Fed9U::FED9U_MODE_ZERO_SUPPRESSED)
     daqModeIndex = 3;
 else 
     QMessageBox::information(this, "Description Attribute Error", "Daq mode value not valid!", 1);
 daqMode->setCurrentItem(daqModeIndex);
    
 int trigSourceIndex(0);
 if (DESC_PTR->getTriggerSource() == Fed9U::FED9U_TRIG_SOFTWARE) 
     trigSourceIndex = 0;
 else if (DESC_PTR->getTriggerSource() == Fed9U::FED9U_TRIG_BACKPLANE)
     trigSourceIndex = 1;
 else if (DESC_PTR->getTriggerSource() == Fed9U::FED9U_TRIG_TTC)
     trigSourceIndex=2;
 else 
     QMessageBox::information(this, "Description Attribute Error", "Trigger source value not valid!", 1);
 triggerSource->setCurrentItem(trigSourceIndex);
    
 int readRouteIndex(0);
 if (DESC_PTR->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_NONE) 
     readRouteIndex = 0;
 else if (DESC_PTR->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_VME)
     readRouteIndex = 1;
 else if (DESC_PTR->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_SLINK64)
     readRouteIndex = 2;
 else 
     QMessageBox::information(this, "Description Attribute Error", "Read Route value not valid!", 1);
 readRoute->setCurrentItem(readRouteIndex);
    
 QString softwareVersion = QString("Utils Version: ") 
      + QString(getFed9UUtilsVersion().c_str());
//         + QString( "Lib Version: ")
  //       + QString(getFed9ULibVersion().c_str());
 softwareVersionDisplay->setText(softwareVersion);
    } catch ( exception & e ) {
 QMessageBox::information(this, "Value Get Error : ", e.what(), 1);
    }  
    
}

void Fed9UDescriptionInterface::displayFrontEndSettings(){
    /***************************
      * Display the Front End settings
      ***************************/
    try {
 Fed9U::Fed9UAddress currentFedAddress;
 currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
	feUnitEnable->setChecked(!DESC_PTR->getFedFeUnitDisable(currentFedAddress));
	maxTempFeCritical->setValue( static_cast<int>(DESC_PTR->getTempControl(currentFedAddress).getCritical() ) );
	maxTempFeFpga->setValue( static_cast<int>(DESC_PTR->getTempControl(currentFedAddress).getFpgaHigh() ) );
	maxTempFeLM82->setValue( static_cast<int>(DESC_PTR->getTempControl(currentFedAddress).getLm82High() ) );
	optoRXCapacitor->setValue( static_cast<int>( DESC_PTR->getOptoRxCapacitor(currentFedAddress) ) );
	optoRXInput->setValue( static_cast<int>( DESC_PTR->getOptoRxInputOffset(currentFedAddress) ) );
	optoRXOutput->setValue( static_cast<int>( DESC_PTR->getOptoRxOutputOffset(currentFedAddress) ) );
	bool overrideDisable;
	DESC_PTR->getCmMedianOverride(currentFedAddress,overrideDisable);	
	medianOverrideEnable->setChecked(!overrideDisable);
	displayChannelSettings(); 
    } catch ( exception & e ) {
	QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
    }		
}
    
void Fed9UDescriptionInterface::displayChannelSettings()
{
    /***************************
      * Display the Front End Channel settings
      ***************************/
    try {
	Fed9U::Fed9UAddress currentFedAddress;
	currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
	adcInputRangeMode->setCurrentItem(DESC_PTR->getAdcControls( currentFedAddress )._dfsen?0:1);
	coarseDelay->setValue( static_cast<int>( DESC_PTR->getCoarseDelay(currentFedAddress) ) );
	fineDelay->setValue( static_cast<int>( DESC_PTR->getFineDelay(currentFedAddress) ) );
	threshold->setValue( static_cast<int>( DESC_PTR->getFrameThreshold(currentFedAddress) ) );
	trimDacOffset->setValue( static_cast<int>( DESC_PTR->getTrimDacOffset(currentFedAddress) ) );	
	displayApvSettings();
    } catch ( exception & e ) {
	QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
    }		
}

void Fed9UDescriptionInterface::displayApvSettings()
{
    /***************************
      * Display the Apv settings
      ***************************/
    try {
	Fed9U::Fed9UAddress currentFedAddress;
	currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
	apvEnable->setChecked(!DESC_PTR->getApvDisable(currentFedAddress));
	bool overrideDisable;
	vector<u16> overrides = DESC_PTR->getCmMedianOverride(currentFedAddress,overrideDisable);	
	medianOverride->setValue( static_cast<int>( overrides[currentFedAddress.getFedApv()] ) );
	displayStripSettings();	
    } catch ( exception & e ) {
	QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
    }
}		

void Fed9UDescriptionInterface::displayStripSettings()
{
    /***************************
      * Display the Strip settings
      ***************************/
    try {
	Fed9U::Fed9UAddress currentFedAddress;
	currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
		setApvStrip( static_cast<u8>( apvStrip->value() ) );
	stripEnable->setChecked(!DESC_PTR->getFedStrips().getStrip(currentFedAddress).getDisable() );
	low->setValue( DESC_PTR->getFedStrips().getStrip(currentFedAddress).getLowThreshold() );
	high->setValue( DESC_PTR->getFedStrips().getStrip(currentFedAddress).getHighThreshold() );
	pedestal->setValue( DESC_PTR->getFedStrips().getStrip(currentFedAddress).getPedestal() );
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
}

void Fed9UDescriptionInterface::fileExit()
{
    exit(0);
}

/*
 *This method is used to toggle between enabling and disabling the main tabs.
 *since it makes no sense to be able to do anything with them until you have 
 *created a fed description object.
 */
void Fed9UDescriptionInterface::setEnableTabs(bool en)
{
    tabMainSettings->setTabEnabled(tabMainSettings->page(0),en);
    tabMainSettings->setTabEnabled(tabMainSettings->page(1),en);    
}



void Fed9UDescriptionInterface::Refresh_clicked()
{
    displayDescription();
}

/*************************************
  *Signal Handlers
  **************************************/
void Fed9UDescriptionInterface::crateNumber_valueChanged( int update)
{
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    // call this method on the description inside the device , since the fed has no register for this value
	    (*i)->getFed9UVMEDeviceDescription().setCrateNumber(static_cast<u16>(update));
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}
    }	 
}


void Fed9UDescriptionInterface::slotNumber_valueChanged( int update)
{
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    // call this method on the description inside the device , since the fed has no register for this value
	    (*i)->getFed9UVMEDeviceDescription().setSlotNumber(static_cast<u8>(update));
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    } 
}


void Fed9UDescriptionInterface::testRegister_valueChanged( int update )
{
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i) {	
	(*i)->setTestRegister(static_cast<u32>(update));
    }
}


void Fed9UDescriptionInterface::fedID_valueChanged( int update)
{
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	(*i)->setFedId(static_cast<u16>(update));
    }
}

void Fed9UDescriptionInterface::daqMode_activated( int update )
{
    Fed9U::Fed9UDaqMode daqMode(Fed9U::FED9U_MODE_SCOPE);
    if ( update == 0 )
	daqMode = Fed9U::FED9U_MODE_SCOPE;
    else if ( update == 1 )
	daqMode = Fed9U::FED9U_MODE_VIRGIN_RAW;
    else if ( update == 2 )
	daqMode = Fed9U::FED9U_MODE_PROCESSED_RAW;
    else if ( update == 3 )
	daqMode = Fed9U::FED9U_MODE_ZERO_SUPPRESSED;
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	(*i)->setDaqMode(daqMode);	
    }
}

void Fed9UDescriptionInterface::clockSource_activated( int update )
{
    Fed9U::Fed9UClockSource clockSource(Fed9U::FED9U_CLOCK_INTERNAL);
    if ( update == 0 )
	clockSource= Fed9U::FED9U_CLOCK_INTERNAL;
    else if ( update == 1 )
	clockSource= Fed9U::FED9U_CLOCK_BACKPLANE;
    else if ( update == 2 )
	clockSource= Fed9U::FED9U_CLOCK_TTC;
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    (*i)->setClock(clockSource);
    	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::triggerSource_activated( int update )
{
    Fed9U::Fed9UTrigSource trigSource(Fed9U::FED9U_TRIG_SOFTWARE);
    if ( update == 0 )
	trigSource = Fed9U::FED9U_TRIG_SOFTWARE;
    else if ( update == 1 )
	trigSource = Fed9U::FED9U_TRIG_BACKPLANE;
    else if ( update == 2 )
	trigSource = Fed9U::FED9U_TRIG_TTC;
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    (*i)->setTriggerSource(trigSource);
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::readRoute_activated( int update )
{
    Fed9U::Fed9UReadRoute readRoute(Fed9U::FED9U_ROUTE_NONE);
    if ( update == 0 )
	readRoute = Fed9U::FED9U_ROUTE_NONE;
    else if ( update == 1 )
	readRoute = Fed9U::FED9U_ROUTE_VME;
    else if ( update == 2 )
	readRoute = Fed9U::FED9U_ROUTE_SLINK64;
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    (*i)->setBeFpgaReadRoute(readRoute);
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::descriptionName_textChanged( const QString & update )
{
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    (*i)->getFed9UVMEDeviceDescription().setName(string(update.ascii()));
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::chkFedEnable_toggled( bool update )
{
    vector<Fed9U::Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	try {
	    (*i)->setFedBeFpgaDisable(!update);
	} catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::maxTempCritical_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
	vector<Fed9U::Fed9UABC*>::iterator i;
	for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ) {	
	    try {
		Fed9U::Fed9UTempControl temp = (*i)->getTempControl(addr);
		temp.setCritical(static_cast<u32>(update));
		(*i)->setTempControl(addr, temp);
	    } catch ( exception & e ) {
		QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	    }		
	}    
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}


void Fed9UDescriptionInterface::maxTempBeFpga_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
	vector<Fed9UABC*>::iterator i;
	for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	    try{
		Fed9U::Fed9UTempControl temp = (*i)->getTempControl(addr);
		temp.setFpgaHigh(static_cast<u32>(update));
		(*i)->setTempControl(addr, temp);
	    } catch ( exception & e ) {
		QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	    }		
	}
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}


void Fed9UDescriptionInterface::maxTempLM82_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND );
	vector<Fed9UABC*>::iterator i;
	for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	    try{
		Fed9U::Fed9UTempControl temp = (*i)->getTempControl(addr);
		temp.setLm82High(static_cast<u32>(update));
		(*i)->setTempControl(addr, temp);
	    } catch ( exception & e ) {
		QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	    }		
	}
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}

void Fed9UDescriptionInterface::scopeLength_valueChanged( int update )
{
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    cout << "Setting Scope Length to " << update << endl;
	    (*i)->setScopeLength(static_cast<u16>( update ));
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::frontEndUnit_activated( int )
{
    displayFrontEndSettings();
}


void Fed9UDescriptionInterface::feUnitChannel_activated( int )
{
    displayChannelSettings();
}


void Fed9UDescriptionInterface::channelApv_activated( int )
{
    displayApvSettings();
}


void Fed9UDescriptionInterface::apvStrip_valueChanged( int )
{
    displayStripSettings();
}


void Fed9UDescriptionInterface::feUnitEnable_toggled( bool update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setFedFeUnitDisable(currentFedAddress, !update );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}

void Fed9UDescriptionInterface::medianOverrideEnable_toggled( bool update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    bool overrideDisable;
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    vector<u16> overrides = (*i)->getCmMedianOverride(currentFedAddress,overrideDisable);
	    (*i)->setCmMedianOverride( currentFedAddress, !update,  overrides);	      
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}

void Fed9UDescriptionInterface::maxTempFeLM82_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    Fed9U::Fed9UTempControl temp = (*i)->getTempControl(currentFedAddress);
	    temp.setLm82High(static_cast<u32>(update));
	    (*i)->setTempControl(currentFedAddress, temp);
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::maxTempFeFpga_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    Fed9U::Fed9UTempControl temp = (*i)->getTempControl(currentFedAddress);
	    temp.setFpgaHigh(static_cast<u32>(update));
	    (*i)->setTempControl(currentFedAddress, temp);
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::maxTempFeCritical_valueChanged( int update)
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    Fed9U::Fed9UTempControl temp = (*i)->getTempControl(currentFedAddress);
	    temp.setCritical(static_cast<u32>(update));
	    (*i)->setTempControl(currentFedAddress, temp);
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::optoRXOutput_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setOptoRxOutputOffset( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::optoRXInput_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setOptoRxInputOffset( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::optoRXCapacitor_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setOptoRxCapacitor( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::coarseDelay_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setCoarseDelay( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::fineDelay_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setFineDelay( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::threshold_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setFrameThreshold( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::adcInputRangeMode_activated( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    Fed9U::Fed9UAdcControls adcControls = (*i)->getAdcControls(currentFedAddress);
	    adcControls._dfsen = (update == 0)?true:false;
	    (*i)->setAdcControls( currentFedAddress, adcControls );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::trimDacOffset_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setTrimDacOffset( currentFedAddress, static_cast<u16>(update) );
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
 }


void Fed9UDescriptionInterface::apvEnable_toggled( bool update )
{
   cout << "apvEnable_toggled"<<endl;
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
    vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
	try{
	    (*i)->setApvDisable(currentFedAddress,!update);
	} catch ( exception & e ) {
	    QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	}		
    }
}


void Fed9UDescriptionInterface::medianOverride_valueChanged( int update)
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
   bool overrideDisable;
   vector<Fed9UABC*>::iterator i;
   for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
       try{
	   vector<u16> overrides = (*i)->getCmMedianOverride(currentFedAddress,overrideDisable);
	   overrides[currentFedAddress.getFeUnitApv()] = static_cast<u16>(update);
	   (*i)->setCmMedianOverride( currentFedAddress, overrideDisable,  overrides);
       } catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
       }		
   }
}


void Fed9UDescriptionInterface::stripEnable_toggled( bool update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   vector<Fed9UABC*>::iterator i;
   for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
       try{
	   (*i)->getFedStrips().getStrip(currentFedAddress).setDisable(!update);
       } catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
       }		
   }
}


void Fed9UDescriptionInterface::low_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   vector<Fed9UABC*>::iterator i;
   for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
       try{
	   (*i)->getFedStrips().getStrip(currentFedAddress).setLowThreshold( static_cast<i16>(update) );
       } catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
       }		
   }
}


void Fed9UDescriptionInterface::high_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   vector<Fed9UABC*>::iterator i;
   for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
       try{
	   (*i)->getFedStrips().getStrip(currentFedAddress).setHighThreshold( static_cast<i16>(update) );
       } catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
       }		
   }
}


void Fed9UDescriptionInterface::pedestal_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   vector<Fed9UABC*>::iterator i;
   for (i = fedAbcMap->getMap(currentFedABC).begin() ; i != fedAbcMap->getMap(currentFedABC).end() ; ++i ){	
       try{
	   (*i)->getFedStrips().getStrip(currentFedAddress).setPedestal( static_cast<i16>(update) );
       } catch ( exception & e ) {
	   QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
       }		
   }
}


void Fed9UDescriptionInterface::applyToAllStrips_clicked()
{
    applyToAllStrips->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::STRIPS_PER_APV; i++){
	 currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		 setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		 setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
		 setApvStrip( static_cast<u8>( i ) );
	 vector<Fed9UABC*>::iterator it;
	 for (it = fedAbcMap->getMap(currentFedABC).begin() ; it != fedAbcMap->getMap(currentFedABC).end() ; ++it ){	
	     try{
		 if ( pedestalRadio->isChecked())
		     (*it)->getFedStrips().getStrip(currentFedAddress).setPedestal( static_cast<i16>(pedestal->value()) );
		 if ( highRadio->isChecked())
		     (*it)->getFedStrips().getStrip(currentFedAddress).setHighThreshold( static_cast<i16>(high->value()) );
		 if ( lowRadio->isChecked())
		     (*it)->getFedStrips().getStrip(currentFedAddress).setLowThreshold( static_cast<i16>(low->value()) );
		 if ( stripEnableRadio->isChecked())
		     (*it)->getFedStrips().getStrip(currentFedAddress).setDisable(!stripEnable->isChecked());
	     } catch ( exception & e ) {
		 QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	     }		
	 }
      }
    applyToAllStrips->setEnabled(true);
}


void Fed9UDescriptionInterface::applyToAllApvs_clicked()
{
    applyToAllApvs->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::APVS_PER_CHANNEL; i++){
	 currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		 setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		 setChannelApv( static_cast<u8>( i ) );
	 vector<Fed9UABC*>::iterator it;
	 for (it = fedAbcMap->getMap(currentFedABC).begin() ; it != fedAbcMap->getMap(currentFedABC).end() ; ++it ){	
	     try{
		 if ( medianOverrideRadio->isChecked()) {
		     bool overrideDisable;
		     vector<u16> overrides = (*it)->getCmMedianOverride(currentFedAddress,overrideDisable);
		     overrides[currentFedAddress.getFeUnitApv()] = static_cast<u16>(medianOverride->value());
		     (*it)->setCmMedianOverride( currentFedAddress, overrideDisable,  overrides);
		 }
		 if ( apvEnableRadio->isChecked())
		     (*it)->setApvDisable(currentFedAddress,!apvEnable->isChecked());
	  
		 if ( drillDownToStrips->isChecked()) {
		     channelApv->setCurrentItem(i);
		     applyToAllStrips_clicked();
		 }
	     } catch ( exception & e ) {
		 QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	     }		
	 }
      }
    applyToAllApvs->setEnabled(true);
}


void Fed9UDescriptionInterface::applyToAllChannels_clicked()
{
    applyToAllChannels->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::CHANNELS_PER_FEUNIT ; i++){
	currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		setFeUnitChannel( static_cast<u8>( i ) );
	 vector<Fed9UABC*>::iterator it;
	 for (it = fedAbcMap->getMap(currentFedABC).begin() ; it != fedAbcMap->getMap(currentFedABC).end() ; ++it){	
	     try{
		 if ( adcInputRangeRadio->isChecked())
		     (*it)->setAdcControls( currentFedAddress, 	
					   (*it)->getAdcControls( Fed9U::Fed9UAddress(0).setFedFeUnit(
						   static_cast<u8>( frontEndUnit->currentItem() )).setFeUnitChannel(
							   static_cast<u8>( feUnitChannel->currentItem() )) ));
		 if ( thresholdRadio->isChecked())
		     (*it)->setFrameThreshold( currentFedAddress, static_cast<u16>(threshold->value()) );
		 if ( trimDacOffsetRadio->isChecked())
		     (*it)->setTrimDacOffset( currentFedAddress, static_cast<u16>(trimDacOffset->value()) );
		 if ( fineDelayRadio->isChecked())
		     (*it)->setFineDelay( currentFedAddress, static_cast<u16>(fineDelay->value()) );
		 if ( coarseDelayRadio->isChecked())
		     (*it)->setCoarseDelay( currentFedAddress, static_cast<u16>(coarseDelay->value()) );
     
		 if ( drillDownToApvs->isChecked()) {
		     feUnitChannel->setCurrentItem(i);
		     applyToAllApvs_clicked();
		 }		
	     } catch ( exception & e ) {
		 QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	     }		
	 }
     }	
    applyToAllChannels->setEnabled(true);
}


void Fed9UDescriptionInterface::applyToAllFeUnits_clicked()
{
    applyToAllFeUnits->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::FEUNITS_PER_FED ; i++){
	currentFedAddress.setFedFeUnit(static_cast<u8>( i ) );
	 vector<Fed9UABC*>::iterator it;
	 for (it = fedAbcMap->getMap(currentFedABC).begin() ; it != fedAbcMap->getMap(currentFedABC).end() ; ++it ){	
	     try{
		 if ( tempControlRadio->isChecked())
		     (*it)->setTempControl(currentFedAddress, 
					  (*it)->getTempControl(
						  Fed9U::Fed9UAddress(0).setFedFeUnit(
							  static_cast<u8>(frontEndUnit->currentItem()))));
		 if ( feUnitEnableRadio->isChecked())
		     (*it)->setFedFeUnitDisable(currentFedAddress, !feUnitEnable->isChecked() );
		 if ( optoRxOutputRadio->isChecked())
		     (*it)->setOptoRxOutputOffset( currentFedAddress, static_cast<u16>(optoRXOutput->value()) );
		 if ( optoRxInputRadio->isChecked())
		     (*it)->setOptoRxInputOffset( currentFedAddress, static_cast<u16>(optoRXInput->value()) );
		 if ( optoRxCapacitorRadio->isChecked())
		     (*it)->setOptoRxCapacitor( currentFedAddress, static_cast<u16>(optoRXCapacitor->value()) );
		 if ( medianOverrideEnableRadio->isChecked()) {
		     bool overrideDisable;
		     vector<u16> overrides = (*it)->getCmMedianOverride(currentFedAddress,overrideDisable);
		     overrideDisable = !medianOverrideEnable->isChecked();
		     (*it)->setCmMedianOverride( currentFedAddress, overrideDisable,  overrides);
		 }	
	     } catch ( exception & e ) {
		 QMessageBox::information(this, "Value Set Error : ", e.what(), 1);
	     }		
	 }

	    
	if ( drillDownToChannels->isChecked()) {
	    frontEndUnit->setCurrentItem(i);
	    applyToAllChannels_clicked();
	}
    }	
    applyToAllFeUnits->setEnabled(true);
}
