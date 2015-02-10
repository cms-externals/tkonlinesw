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
void Fed9UGUIConfig::init()
{
    ptr=NULL;
    filename="";
    setEnableTabs(false);
    tabMainSettings->setCurrentPage(0);

}

void Fed9UGUIConfig::destroy()
{
    if(ptr!=NULL)
	delete ptr;
}

void Fed9UGUIConfig::displayDescription()
{
    displayMainSettings();
    displayFrontEndSettings();
}

void Fed9UGUIConfig::displayMainSettings()
{
    /***************************
      *Start by displaying the Main settings which generally apply to the BE FPGA
      ***************************/
    chkFedEnable->setChecked(!ptr->getFedBeFpgaDisable());
    crateNumber->setValue(ptr->getCrateNumber());
    slotNumber->setValue(ptr->getSlotNumber());
    testRegister->setValue(ptr->getTestRegister());
    fedID->setValue(ptr->getFedId());
    descriptionName->setText(QString(ptr->getName().c_str()));
    Fed9U::Fed9UAddress addr;
    addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
    maxTempLM82->setValue( static_cast<int>( ptr->getTempControl(addr).getLm82High() ) );
    maxTempBeFpga->setValue( static_cast<int>( ptr->getTempControl(addr).getFpgaHigh() ) );
    maxTempCritical->setValue( static_cast<int>( ptr->getTempControl(addr).getCritical() ) );
    int clockSourceIndex(0);
    if (ptr->getClock() == Fed9U::FED9U_CLOCK_INTERNAL) 
	clockSourceIndex = 0;
    else if (ptr->getClock() == Fed9U::FED9U_CLOCK_BACKPLANE)
	clockSourceIndex = 1;
    else if (ptr->getClock() == Fed9U::FED9U_CLOCK_TTC)
	clockSourceIndex=2;
    else 
	QMessageBox::information(this, "Description Attribute Error", "Clock source value not valid!", 1);
    clockSource->setCurrentItem(clockSourceIndex);
    
    int daqModeIndex(0);
    if (ptr->getDaqMode() == Fed9U::FED9U_MODE_SCOPE) 
	daqModeIndex = 0;
    else if (ptr->getDaqMode() == Fed9U::FED9U_MODE_VIRGIN_RAW)
	daqModeIndex = 1;
    else if (ptr->getDaqMode() == Fed9U::FED9U_MODE_PROCESSED_RAW)
	daqModeIndex = 2;
    else if (ptr->getDaqMode() == Fed9U::FED9U_MODE_ZERO_SUPPRESSED)
	daqModeIndex = 3;
    else 
	QMessageBox::information(this, "Description Attribute Error", "Daq mode value not valid!", 1);
    daqMode->setCurrentItem(daqModeIndex);
    
    int trigSourceIndex(0);
    if (ptr->getTriggerSource() == Fed9U::FED9U_TRIG_SOFTWARE) 
	trigSourceIndex = 0;
    else if (ptr->getTriggerSource() == Fed9U::FED9U_TRIG_BACKPLANE)
	trigSourceIndex = 1;
    else if (ptr->getTriggerSource() == Fed9U::FED9U_TRIG_TTC)
	trigSourceIndex=2;
    else 
	QMessageBox::information(this, "Description Attribute Error", "Trigger source value not valid!", 1);
    triggerSource->setCurrentItem(trigSourceIndex);
    
    int readRouteIndex(0);
    if (ptr->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_NONE) 
	readRouteIndex = 0;
    else if (ptr->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_VME)
	readRouteIndex = 1;
    else if (ptr->getBeFpgaReadRoute() == Fed9U::FED9U_ROUTE_SLINK64)
	readRouteIndex = 2;
    else 
	QMessageBox::information(this, "Description Attribute Error", "Read Route value not valid!", 1);
    readRoute->setCurrentItem(readRouteIndex);
    
    QString softwareVersion = QString("Utils Version: ") 
			      + QString(getFed9UUtilsVersion().c_str());
//			      + QString( "Lib Version: ")
		//	      + QString(getFed9ULibVersion().c_str());
    softwareVersionDisplay->setText(softwareVersion);
}

void Fed9UGUIConfig::displayFrontEndSettings(){
    /***************************
      * Display the Front End settings
      ***************************/
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    feUnitEnable->setChecked(!ptr->getFedFeUnitDisable(currentFedAddress));
    maxTempFeCritical->setValue( static_cast<int>(ptr->getTempControl(currentFedAddress).getCritical() ) );
    maxTempFeFpga->setValue( static_cast<int>(ptr->getTempControl(currentFedAddress).getFpgaHigh() ) );
    maxTempFeLM82->setValue( static_cast<int>(ptr->getTempControl(currentFedAddress).getLm82High() ) );
    optoRXCapacitor->setValue( static_cast<int>( ptr->getOptoRxCapacitor(currentFedAddress) ) );
    optoRXInput->setValue( static_cast<int>( ptr->getOptoRxInputOffset(currentFedAddress) ) );
    optoRXOutput->setValue( static_cast<int>( ptr->getOptoRxOutputOffset(currentFedAddress) ) );
    displayChannelSettings(); 
}

void Fed9UGUIConfig::displayChannelSettings()
{
    /***************************
      * Display the Front End Channel settings
      ***************************/
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    dataComplement->setChecked(!ptr->getComplement( currentFedAddress));
    adcInputRangeMode->setCurrentItem(ptr->getAdcControls( currentFedAddress )._s1?(ptr->getAdcControls( currentFedAddress )._dfsen?0:1):2);
    coarseDelay->setValue( static_cast<int>( ptr->getCoarseDelay(currentFedAddress) ) );
    fineDelay->setValue( static_cast<int>( ptr->getFineDelay(currentFedAddress) ) );
    threshold->setValue( static_cast<int>( ptr->getFrameThreshold(currentFedAddress) ) );
    trimDacOffset->setValue( static_cast<int>( ptr->getTrimDacOffset(currentFedAddress) ) );	
    displayApvSettings();
}

void Fed9UGUIConfig::displayApvSettings()
{
    /***************************
      * Display the Apv settings
      ***************************/
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
    apvEnable->setChecked(!ptr->getApvDisable(currentFedAddress));
    medianOverride->setValue( static_cast<int>( ptr->getMedianOverride(currentFedAddress) ) );   
    cmOverrideEnable->setChecked(!ptr->getMedianOverrideDisable(currentFedAddress));
    displayStripSettings();
}

void Fed9UGUIConfig::displayStripSettings()
{
    /***************************
      * Display the Strip settings
      ***************************/
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   stripEnable->setChecked(!ptr->getFedStrips().getStrip(currentFedAddress).getDisable() );
   low->setValue( ptr->getFedStrips().getStrip(currentFedAddress).getLowThreshold() );
   high->setValue( ptr->getFedStrips().getStrip(currentFedAddress).getHighThreshold() );
   pedestal->setValue( ptr->getFedStrips().getStrip(currentFedAddress).getPedestal() );
}

void Fed9UGUIConfig::fileNew()
{
    if(ptr!=NULL)
	delete ptr;
    try {
	ptr=new Fed9U::Fed9UDescription;
	ptr->loadDefaultDescription();
    } catch ( exception & e ) {
	QMessageBox::information(this, "New file error : ", e.what(), 1);
    }	
    setEnableTabs(true);
    displayDescription();
}

void Fed9UGUIConfig::fileOpen()
{
    if(ptr!=NULL)
	delete ptr;
    //using std::istream;
    QString s =  QFileDialog::getOpenFileName("","Fed Description Files (*.xml *.fed)",this,"open file dialog","Choose a file" );
    if(s=="")
	 return;
    ptr=new Fed9U::Fed9UDescription;
    try{
	if(s.endsWith(".fed")) {
	    std::ifstream input(s.ascii());
	    if(!input.is_open())
		return;
	    filename=s;
	    ptr->loadDescription(static_cast<istream &>(input));
	}
	else if(s.endsWith(".xml")) {
	    ptr->loadDefaultDescription();
	    Fed9U::Fed9UXMLDescription xDesc(s.ascii(),*ptr);
	    xDesc.makeNewFed9UDescription();
	    filename=s;
	}	
	else {	
	    //the file is unrecognisable, so we popup an alert error to inform the user
	    QMessageBox::information(this, "File Input Error", "Unable to open file because it has the wrong extension", 1);
	    return;
	}
    }
    catch(exception & exc){
	// we must handle this error here... pop up an error dialogue
	QMessageBox::information(this, "File Input Error : "+s, exc.what(), 1);
	return;
    }
    catch(exception & exc ){
	QMessageBox::information(this, "UnknownError", exc.what(), 1);
	return;
    }
    catch(...){
	QMessageBox::information(this, "UnknownError", "", 1);
	return;
    }
    setEnableTabs(true);
    displayDescription();
}

void Fed9UGUIConfig::saveXMLFile()
{	
    try{
	Fed9U::Fed9UDescriptionToXml pob(string(filename.ascii()),*ptr);
	pob.writeXmlFile();
    }catch(exception & exc){
	QMessageBox::information(this, "Save Error : "+filename, exc.what(), 1);    	    
    }
}

void Fed9UGUIConfig::saveFedFile()
{
    try{
	ofstream fileo(filename);
	ICUTILS_ASSERT( fileo.is_open() ).msg("cannot open file for writing");
	ptr->saveDescription(fileo);
    }catch(exception & exc){
	QMessageBox::information(this, "Save Error : "+filename, exc.what(), 1);    	    
    }
}

void Fed9UGUIConfig::fileSave()
{
    if(ptr!=NULL)
    {
	if( filename.isEmpty() ) {
	    fileSaveAs();
	    return;	 
	}	
	if (filename.endsWith(QString(".fed"))) {
	    saveFedFile();
	}
	else if (filename.endsWith(QString(".xml")) ) {
	    saveXMLFile();	
	}
	else {
	    QMessageBox::information(this, "File name must end with .xml or .fed : "+filename, "", 1);
	}
    }
}

void Fed9UGUIConfig::fileSaveAs()
{  
    
    QFileDialog* fd = new QFileDialog( this, "file dialog", TRUE );
    fd->setMode( QFileDialog::AnyFile );
    fd->setFilter( "Fed Descriptions (*.xml)" );
    QString fileName;
    if ( fd->exec() == QDialog::Accepted )
	fileName = fd->selectedFile();
    delete fd;
    if(fileName.isEmpty())	
	return;
    filename=fileName;
    if(!filename.endsWith(".xml") && !filename.endsWith(".fed"))
	filename+=".xml";	
    fileSave();
	
}

void Fed9UGUIConfig::filePrint()
{

}

void Fed9UGUIConfig::fileExit()
{
    exit(0);
}

/*
 *This method is used to toggle between enabling and disabling the main tabs.
 *since it makes no sense to be able to do anything with them until you have 
 *created a fed description object.
 */
void Fed9UGUIConfig::setEnableTabs(bool en)
{
    tabMainSettings->setTabEnabled(tabMainSettings->page(0),en);
    tabMainSettings->setTabEnabled(tabMainSettings->page(1),en);    
}



void Fed9UGUIConfig::Refresh_clicked()
{
    displayDescription();
}

/*************************************
  *Signal Handlers
  **************************************/
void Fed9UGUIConfig::crateNumber_valueChanged( int update)
{
    ptr->setCrateNumber(static_cast<u16>(update));
}


void Fed9UGUIConfig::slotNumber_valueChanged( int update)
{
    ptr->setSlotNumber(static_cast<u8>(update));
}


void Fed9UGUIConfig::testRegister_valueChanged( int update )
{
    ptr->setTestRegister(static_cast<u32>(update));
}


void Fed9UGUIConfig::fedID_valueChanged( int update)
{
    ptr->setFedId(static_cast<u16>(update));
}


void Fed9UGUIConfig::DaqMode_activated( int update )
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
    ptr->setDaqMode(daqMode);
}


void Fed9UGUIConfig::clockSource_activated( int update )
{
    Fed9U::Fed9UClockSource clockSource(Fed9U::FED9U_CLOCK_INTERNAL);
    if ( update == 0 )
	clockSource= Fed9U::FED9U_CLOCK_INTERNAL;
    else if ( update == 1 )
	clockSource= Fed9U::FED9U_CLOCK_BACKPLANE;
    else if ( update == 2 )
	clockSource= Fed9U::FED9U_CLOCK_TTC;
    ptr->setClock(clockSource);
}


void Fed9UGUIConfig::triggerSource_activated( int update )
{
    Fed9U::Fed9UTrigSource trigSource(Fed9U::FED9U_TRIG_SOFTWARE);
    if ( update == 0 )
	trigSource = Fed9U::FED9U_TRIG_SOFTWARE;
    else if ( update == 1 )
	trigSource = Fed9U::FED9U_TRIG_BACKPLANE;
    else if ( update == 2 )
	trigSource = Fed9U::FED9U_TRIG_TTC;
    ptr->setTriggerSource(trigSource);
}


void Fed9UGUIConfig::readRoute_activated( int update )
{
    Fed9U::Fed9UReadRoute readRoute(Fed9U::FED9U_ROUTE_NONE);
    if ( update == 0 )
	readRoute = Fed9U::FED9U_ROUTE_NONE;
    else if ( update == 1 )
	readRoute = Fed9U::FED9U_ROUTE_VME;
    else if ( update == 2 )
	readRoute = Fed9U::FED9U_ROUTE_SLINK64;
    ptr->setBeFpgaReadRoute(readRoute);
}


void Fed9UGUIConfig::descriptionName_textChanged( const QString & update )
{
    ptr->setName(string(update.ascii()));
}


void Fed9UGUIConfig::chkFedEnable_toggled( bool update )
{
    ptr->setFedBeFpgaDisable(!update);
}


void Fed9UGUIConfig::maxTempCritical_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
	Fed9U::Fed9UTempControl temp = ptr->getTempControl(addr);
	temp.setCritical(static_cast<u32>(update));
	ptr->setTempControl(addr, temp);
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}


void Fed9UGUIConfig::maxTempBeFpga_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND);
	Fed9U::Fed9UTempControl temp = ptr->getTempControl(addr);
	temp.setFpgaHigh(static_cast<u32>(update));
	ptr->setTempControl(addr, temp);
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}


void Fed9UGUIConfig::maxTempLM82_valueChanged( int update )
{
    try {
	Fed9U::Fed9UAddress addr;
	addr.setFedFeUnit( Fed9U::Fed9UAddress::BACKEND );
	Fed9U::Fed9UTempControl temp = ptr->getTempControl(addr);
	temp.setLm82High(static_cast<u32>(update));
	ptr->setTempControl(addr, temp);
    }catch( exception & exc){
		    QMessageBox::information(this, "Utils Error ", exc.what(), 1);    	
    }
}

void Fed9UGUIConfig::scopeLength_valueChanged( int update )
{
    ptr->setScopeLength(static_cast<u16>( update ));
}


void Fed9UGUIConfig::frontEndUnit_activated( int )
{
    displayFrontEndSettings();
}


void Fed9UGUIConfig::feUnitChannel_activated( int )
{
    displayChannelSettings();
}


void Fed9UGUIConfig::channelApv_activated( int )
{
    displayApvSettings();
}


void Fed9UGUIConfig::apvStrip_valueChanged( int )
{
    displayStripSettings();
}


void Fed9UGUIConfig::feUnitEnable_toggled( bool update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    ptr->setFedFeUnitDisable(currentFedAddress, !update );    
}


void Fed9UGUIConfig::maxTempFeLM82_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    Fed9U::Fed9UTempControl temp = ptr->getTempControl(currentFedAddress);
    temp.setLm82High(static_cast<u32>(update));
    ptr->setTempControl(currentFedAddress, temp);
}


void Fed9UGUIConfig::maxTempFeFpga_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    Fed9U::Fed9UTempControl temp = ptr->getTempControl(currentFedAddress);
    temp.setFpgaHigh(static_cast<u32>(update));
    ptr->setTempControl(currentFedAddress, temp);
}


void Fed9UGUIConfig::maxTempFeCritical_valueChanged( int update)
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    Fed9U::Fed9UTempControl temp = ptr->getTempControl(currentFedAddress);
    temp.setCritical(static_cast<u32>(update));
    ptr->setTempControl(currentFedAddress, temp);
}


void Fed9UGUIConfig::optoRXOutput_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    ptr->setOptoRxOutputOffset( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::optoRXInput_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    ptr->setOptoRxInputOffset( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::optoRXCapacitor_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ));
    ptr->setOptoRxCapacitor( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::coarseDelay_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    ptr->setCoarseDelay( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::fineDelay_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    ptr->setFineDelay( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::threshold_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    ptr->setFrameThreshold( currentFedAddress, static_cast<u16>(update) );
}


void Fed9UGUIConfig::adcInputRangeMode_activated( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    Fed9U::Fed9UAdcControls adcControls = ptr->getAdcControls(currentFedAddress);
    adcControls._s1 = (update != 2)? true:false;
    adcControls._dfsen = (update == 0)?true:false;
    ptr->setAdcControls( currentFedAddress, adcControls );
}

void Fed9UGUIConfig::dataComplement_toggled( bool update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    ptr->setComplement( currentFedAddress, !update );
}


void Fed9UGUIConfig::trimDacOffset_valueChanged( int update )
{
    Fed9U::Fed9UAddress currentFedAddress;
    currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) );
    ptr->setTrimDacOffset( currentFedAddress, static_cast<u16>(update) );
 }


void Fed9UGUIConfig::apvEnable_toggled( bool update )
{
   cout << "apvEnable_toggled"<<endl;
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
   ptr->setApvDisable(currentFedAddress,!update);
}


void Fed9UGUIConfig::medianOverride_valueChanged( int update)
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) );
   ptr->setMedianOverride( currentFedAddress, static_cast<u16>(update) );
   cmOverrideEnable->setChecked((medianOverride->value() == 0)?false:true);   
}


void Fed9UGUIConfig::stripEnable_toggled( bool update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   ptr->getFedStrips().getStrip(currentFedAddress).setDisable(!update);
}


void Fed9UGUIConfig::low_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   ptr->getFedStrips().getStrip(currentFedAddress).setLowThreshold( static_cast<i16>(update) );
}


void Fed9UGUIConfig::high_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   ptr->getFedStrips().getStrip(currentFedAddress).setHighThreshold( static_cast<i16>(update) );
}


void Fed9UGUIConfig::pedestal_valueChanged( int update )
{
   Fed9U::Fed9UAddress currentFedAddress;
   currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
	    setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
	    setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
	    setApvStrip( static_cast<u8>( apvStrip->value() ) );
   ptr->getFedStrips().getStrip(currentFedAddress).setPedestal( static_cast<i16>(update) );
}


void Fed9UGUIConfig::applyToAllStrips_clicked()
{
    applyToAllStrips->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::STRIPS_PER_APV; i++){
	 currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		 setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		 setChannelApv( static_cast<u8>( channelApv->currentItem() ) ).
		 setApvStrip( static_cast<u8>( i ) );
	 if ( pedestalRadio->isChecked())
	     ptr->getFedStrips().getStrip(currentFedAddress).setPedestal( static_cast<i16>(pedestal->value()) );
	 if ( highRadio->isChecked())
	     ptr->getFedStrips().getStrip(currentFedAddress).setHighThreshold( static_cast<i16>(high->value()) );
	 if ( lowRadio->isChecked())
	     ptr->getFedStrips().getStrip(currentFedAddress).setLowThreshold( static_cast<i16>(low->value()) );
	 if ( stripEnableRadio->isChecked())
	     ptr->getFedStrips().getStrip(currentFedAddress).setDisable(!stripEnable->isChecked());
      }
    applyToAllStrips->setEnabled(true);
}


void Fed9UGUIConfig::applyToAllApvs_clicked()
{
    applyToAllApvs->setEnabled(false);
     Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::APVS_PER_CHANNEL; i++){
	 currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		 setFeUnitChannel( static_cast<u8>( feUnitChannel->currentItem() ) ).
		 setChannelApv( static_cast<u8>( i ) );
	 if ( medianOverrideRadio->isChecked()) {
	     ptr->setMedianOverride( currentFedAddress, static_cast<u16>(medianOverride->value()) );
	     cmOverrideEnable->setChecked((medianOverride->value() == 0)?false:true);   
	 }
	 if ( apvEnableRadio->isChecked())
	     ptr->setApvDisable(currentFedAddress,!apvEnable->isChecked());
	 
	 if ( drillDownToStrips->isChecked()) {
	     channelApv->setCurrentItem(i);
	     applyToAllStrips_clicked();
	 }
      }
    applyToAllApvs->setEnabled(true);
}


void Fed9UGUIConfig::applyToAllChannels_clicked()
{
    applyToAllChannels->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::CHANNELS_PER_FEUNIT ; i++){
	currentFedAddress.setFedFeUnit(static_cast<u8>( frontEndUnit->currentItem() ) ).
		setFeUnitChannel( static_cast<u8>( i ) );
	if ( dataComplementRadio->isChecked())
	    ptr->setComplement( currentFedAddress, !dataComplement->isChecked() );
	if ( adcInputRangeRadio->isChecked())
	    ptr->setAdcControls( currentFedAddress, 	
				 ptr->getAdcControls( Fed9U::Fed9UAddress(0).setFedFeUnit(
					 static_cast<u8>( frontEndUnit->currentItem() )).setFeUnitChannel(
						 static_cast<u8>( feUnitChannel->currentItem() )) ));
	if ( thresholdRadio->isChecked())
	    ptr->setFrameThreshold( currentFedAddress, static_cast<u16>(threshold->value()) );
	if ( trimDacOffsetRadio->isChecked())
	    ptr->setTrimDacOffset( currentFedAddress, static_cast<u16>(trimDacOffset->value()) );
	if ( fineDelayRadio->isChecked())
	    ptr->setFineDelay( currentFedAddress, static_cast<u16>(fineDelay->value()) );
	if ( coarseDelayRadio->isChecked())
	     ptr->setCoarseDelay( currentFedAddress, static_cast<u16>(coarseDelay->value()) );
     
	 if ( drillDownToApvs->isChecked()) {
	     feUnitChannel->setCurrentItem(i);
	     applyToAllApvs_clicked();
	 }
     }	
    applyToAllChannels->setEnabled(true);
}


void Fed9UGUIConfig::applyToAllFeUnits_clicked()
{
    applyToAllFeUnits->setEnabled(false);
    Fed9U::Fed9UAddress currentFedAddress;
    for(int i = 0 ; i<Fed9U::FEUNITS_PER_FED ; i++){
	currentFedAddress.setFedFeUnit(static_cast<u8>( i ) );
	if ( tempControlRadio->isChecked())
	    ptr->setTempControl(currentFedAddress, 
			    ptr->getTempControl(
				    Fed9U::Fed9UAddress(0).setFedFeUnit(
					    static_cast<u8>(frontEndUnit->currentItem()))));
	if ( feUnitEnableRadio->isChecked())
	    ptr->setFedFeUnitDisable(currentFedAddress, !feUnitEnable->isChecked() );
	if ( optoRxOutputRadio->isChecked())
	    ptr->setOptoRxOutputOffset( currentFedAddress, static_cast<u16>(optoRXOutput->value()) );
	if ( optoRxInputRadio->isChecked())
	    ptr->setOptoRxInputOffset( currentFedAddress, static_cast<u16>(optoRXInput->value()) );
	if ( optoRxCapacitorRadio->isChecked())
	    ptr->setOptoRxCapacitor( currentFedAddress, static_cast<u16>(optoRXCapacitor->value()) );
     
	if ( drillDownToChannels->isChecked()) {
	    frontEndUnit->setCurrentItem(i);
	    applyToAllChannels_clicked();
	}
    }	
    applyToAllFeUnits->setEnabled(true);
}


