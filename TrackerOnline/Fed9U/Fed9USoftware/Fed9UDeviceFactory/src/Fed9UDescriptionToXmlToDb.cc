//Jonathan Fulcher, 11/11/04
//Class which will write a Fed9UDescription to the database.
//This class uses the Xerces 2.3 library.*/

// don't bother building this class if the database is not defined
#ifdef DATABASE
#include <inttypes.h>
#include <stdint.h>
//Check if using Xerces version 1 
#if XERCES < 2
#warning "Using old version of XERCES, upgrade to 2 if you want to use Fed9UDescriptionToXml."
#else

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
//#pragma message("_XERCES_VERSION")
#if _XERCES_VERSION >= 20300

//Xerces headers
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/Base64.hpp>
#include "Fed9UStrX.hh"
#include "Fed9UXStr.hh"

//Fed9U headers
#include "Fed9UDescriptionToXmlToDb.hh"
#include "Fed9UXMLDescriptionException.hh"

//Standard includes
#include<iostream>
#include<fstream>
#include<sstream>

//Fec Exception Handler
#include "FecExceptionHandler.h"

#include <exception>


namespace Fed9U {

//   using xercesc::DOMImplementation;
//   using xercesc::DOMImplementationRegistry;
//   using xercesc::DOMImplementationLS;
//   using xercesc::DOMException;
//   using xercesc::XMLPlatformUtils;
//   using xercesc::XMLException;
//   using xercesc::XMLString;
//   using xercesc::XMLUni;
//   using xercesc::XMLFormatTarget;
//   using xercesc::LocalFileFormatTarget;
//   using xercesc::MemBufFormatTarget;
//   using xercesc::Base64;

//   using std::cerr;
//   using std::hex;
//   using std::dec;
//   using std::endl;
//   using std::cout;
//   using std::ostringstream;
//   using std::stringstream;
//   using std::ifstream;
//   using std::ofstream;

//This constructor takes the file name of the XML file to be written, and a reference to the 
//Fed9UDescription from which to make the XML file.
  Fed9UDescriptionToXmlToDb::Fed9UDescriptionToXmlToDb(Fed9UDescription &Fed9UDescriptionToWrite, 
						       DbFedAccess * dbFedAccess) throw (Fed9UXMLDescriptionException) : Fed9UDescriptionToXml(Fed9UDescriptionToWrite),
										    dbFedAccess_(dbFedAccess) {
    
  }

  
  Fed9UDescriptionToXmlToDb::Fed9UDescriptionToXmlToDb(std::vector<Fed9UDescription*> Fed9UDescriptionsToWrite, 
						       DbFedAccess * dbFedAccess ) throw (Fed9UXMLDescriptionException) : Fed9UDescriptionToXml( Fed9UDescriptionsToWrite ), 
															  dbFedAccess_(dbFedAccess) {
															  
    
  }

  void Fed9UDescriptionToXmlToDb::upload(u16 versionMinorId, u16 versionMajorId, std::string partition) {
    try {
      ICUTILS_VERIFY(partition != "null").msg("you must provide the partition!!!!").error();
      std::stringstream descriptionInXML;
      streamOutDescription(&descriptionInXML);
      std::cout << "The xml description created in buffer is being uploaded please wait...  " <<  std::endl;
      dbFedAccess_->setXMLClobWithVersion ( descriptionInXML.str(), partition, versionMajorId, versionMinorId);
      std::cout << "upload completed" << std::endl;
    } catch(FecExceptionHandler & exc) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!about to crash!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      std::cout << exc.what() << std::endl;
    } catch  (oracle::occi::SQLException & e) {
      std::cout << "!!!!!!!!!!!!!!!!!!!about to crash with occi::SQLException!!!!!!!!!!!" << std::endl;
      std::string errorMessage = e.what();
      std::cout << errorMessage << std::endl;
      if ((errorMessage.find("exit called from Java code with status 0",0))
	  != std::string::npos 
	  || (errorMessage.find("Java call terminated by uncaught Java exception",0)) 
	  != std::string::npos) {
	errorMessage = dbFedAccess_->getErrorMessage();
      }
      std::cout << errorMessage << std::endl;

    }  catch(...) {
      std::cout << "Oh Look there was an error in the Database Upload!!!" << std::endl;
    }
  }


  void Fed9UDescriptionToXmlToDb::upload(int updateVersion, std::string partition) {
    try {
      ICUTILS_VERIFY(partition != "null").msg("you must provide the partition!!!!").error();
      
#ifdef FED9U_DATABASE_FAST_UPLOAD
      
      std::string * fastFedDescBuffer = new std::string[8];
      streamOutFastDescription(fastFedDescBuffer);
      Fed9UMessage<Fed9UDebugLevel>(FED9U_DEBUG_LEVEL_DETAILED) << fastFedDescBuffer[0] << "\n\n\n" 
	   << fastFedDescBuffer[1] << "\n\n\n" 
	   << fastFedDescBuffer[2] << "\n\n\n" 
	   << fastFedDescBuffer[3] << "\n\n\n" 
	   << fastFedDescBuffer[4] << "\n\n\n"	  
	   << fastFedDescBuffer[5] << "\n\n\n" 
	   << fastFedDescBuffer[6] << "\n\n\n" 
	   << fastFedDescBuffer[7] << "\n\n\n" 
	   << std::endl; 

      std::cout << "FED9U_DATABASE_FAST_UPLOAD" << std::endl ;
      std::cout << "The xml description created in buffer is being uploaded please wait... 2 " << std::endl;
      std::cout << "partition Name = " << partition << " versionUpdate = " << updateVersion << std::endl;
      dbFedAccess_->setXMLClob ( fastFedDescBuffer, partition, (unsigned int)updateVersion );      
      std::cout << "setXMLClob (fastBuffer,partition,updateVersion) has just been called!!!" << std::endl;
      if (fastFedDescBuffer)
      	delete[] fastFedDescBuffer;

      

#else
      
      std::cout << "PROBLEM--------------> FED9U_DATABASE_FAST_UPLOAD" << std::endl ;
      std::stringstream descriptionInXML;
      streamOutDescription(&descriptionInXML);
      
// streamOutDescription(&cout);

 
      std::cout << "The xml description created in buffer is being uploaded please wait... 2 " << std::endl;
      std::cout << "partition Name = " << partition << " versionUpdate = " << updateVersion << std::endl;
      dbFedAccess_->setXMLClob ( descriptionInXML.str(), partition, (unsigned int)updateVersion);
      std::cout << "setXMLClob (buffer,partitino,updateVersion) has just been called!!!" << std::endl;
      
#endif

      std::cout << "upload completed" << std::endl;

    } catch(FecExceptionHandler & exc) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!about to crash!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      std::cout << exc.what() << std::endl;
    } catch  (oracle::occi::SQLException & e) {
      std::cout << "!!!!!!!!!!!!!!!!!!!about to crash with occi::SQLException!!!!!!!!!!!" << std::endl;
      std::string errorMessage = e.getMessage();
      if ((errorMessage.find("exit called from Java code with status 0",0))
	  != std::string::npos 
	  || (errorMessage.find("Java call terminated by uncaught Java exception",0)) 
	  != std::string::npos) {
	errorMessage = dbFedAccess_->getErrorMessage();
      }
      std::cout << errorMessage << std::endl;
    } catch(...) {
      std::cout << "Oh Look there was an error in the Database Upload!!!" << std::endl;
    }
  }

  void Fed9UDescriptionToXmlToDb::streamOutFastDescription(std::string * buf) throw(Fed9U::Fed9UXMLDescriptionException) {
    try {
      std::stringstream tempBuf0, tempBuf1,  tempBuf2, tempBuf3, tempBuf4, tempBuf5, tempBuf6, tempBuf7;
      
      // get the number of feds
      u32 numberOfFeds = (theFed9UDescriptionList.size() == 0) ? 1 : theFed9UDescriptionList.size();
      std::cout << "number Of feds in DescriptionToXmlToDb = " << numberOfFeds << std::endl;
      tempBuf0 << "<ROWSET>";
      tempBuf1 << "<ROWSET>";
      tempBuf2 << "<ROWSET>";
      tempBuf3 << "<ROWSET>";
      tempBuf4 << "<ROWSET>";
      tempBuf5 << "<ROWSET>";
      tempBuf6 << "<ROWSET>";
      tempBuf7 << "<ROWSET>";
      //loop over all feds    
      unsigned short i=0, j=0, k=0, m=0, ifeds=0;
     
      for ( ifeds = 0 ; ifeds < numberOfFeds ; ifeds ++ ) {
	if ( theFed9UDescriptionList.size() > 0 )
	  theFed9UDescription = *theFed9UDescriptionList[ifeds];

	//Set Fed9UAddress to read the BE FPGA parameters
	theFed9UAddress.setFedFeUnit(Fed9UAddress::BACKEND); 
	
	// First set the fed parameters...
	tempBuf0 << "<RAWFED><BEFIRMWAREVERSION>" << std::hex << theFed9UDescription.getBeFirmwareVersion() << std::dec
		 << "</BEFIRMWAREVERSION><CLOCKSOURCE>";
	
	Fed9UClockSource theFed9UClockSource = theFed9UDescription.getClock();
	if (theFed9UClockSource==FED9U_CLOCK_TTC) {
	  tempBuf0 << "TTC";
	}
	else if (theFed9UClockSource==FED9U_CLOCK_BACKPLANE) {
	  tempBuf0 << "BACKPLANE";
	}
	else if (theFed9UClockSource==FED9U_CLOCK_INTERNAL) {
	  tempBuf0 << "INTERNAL";
	}
	
	tempBuf0	 << "</CLOCKSOURCE><CRATE>" << theFed9UDescription.getCrateNumber()
			 << "</CRATE><VMECONTROLLERDAISYCHAINID>" << theFed9UDescription.getVmeControllerDaisyChainId() 
			 << "</VMECONTROLLERDAISYCHAINID><CRATESLOT>" << static_cast<unsigned long>((theFed9UDescription.getBaseAddress()>>16)&0x1f)
			 << "</CRATESLOT><DELAYFIRMWAREVERSION>" << std::hex << theFed9UDescription.getDelayFirmwareVersion() << std::dec
			 << "</DELAYFIRMWAREVERSION><EPROMVERSION>" << theFed9UDescription.getEpromVersion()
			 << "</EPROMVERSION><EVENTTYPE>" << theFed9UDescription.getDaqEventType()
			 << "</EVENTTYPE><FAKEEVENTFILE>" << theFed9UDescription.getFakeEventFile()
			 << "</FAKEEVENTFILE><FAKEEVENTTRIGGERDELAY>" << theFed9UDescription.getFakeEventTriggerDelay()
			 << "</FAKEEVENTTRIGGERDELAY><FEFIRMWAREVERSION>" << std::hex << theFed9UDescription.getFeFirmwareVersion() << std::dec
			 << "</FEFIRMWAREVERSION><VMEFIRMWAREVERSION>" << std::hex << theFed9UDescription.getVmeFirmwareVersion() << std::dec
			 << "</VMEFIRMWAREVERSION><FEDVERSION>" << theFed9UDescription.getFedVersion()
			 << "</FEDVERSION><FOV>" << theFed9UDescription.getDaqFov()
			 << "</FOV><GLOBALCLOCKCOARSESHIFT>" << theFed9UDescription.getGlobalCoarseSkew()
			 << "</GLOBALCLOCKCOARSESHIFT><BXOFFSET>" << theFed9UDescription.getBunchCrossingOffset()
			 << "</BXOFFSET><GLOBALCLOCKFINESHIFT>" << theFed9UDescription.getGlobalFineSkew()
			 << "</GLOBALCLOCKFINESHIFT><HALBUSADAPTOR>";
	
	Fed9UHalBusAdaptor theBusAdaptorType = theFed9UDescription.getBusAdaptorType();
	if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_SBS) {
	  tempBuf0 << "SBS";
	}
	else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_CAEN_PCI) {
	  tempBuf0 << "CAEN_PCI";
	}
	else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_CAEN_USB) {
	  tempBuf0 << "CAEN_USB";
	}
	else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_VXI) {
	  tempBuf0 << "VXI";
	}
	else if (theBusAdaptorType==FED9U_HAL_BUS_ADAPTOR_DUMMY) {
	  tempBuf0 << "DUMMY";
	}

	//Initialize a Fed9UTempContol object from the Fed9UDescription.
	Fed9UTempControl theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);

	tempBuf0 << "</HALBUSADAPTOR><HARDWAREID>" << theFed9UDescription.getFedHardwareId()
		 << "</HARDWAREID><HEADERTYPE>";
	
	Fed9UHeaderFormat theFed9UHeaderFormat = theFed9UDescription.getHeaderFormatType();
	if (theFed9UHeaderFormat==FED9U_HEADER_FULLDEBUG) {
	  tempBuf0 << "FULLDEBUG";
	}
	else if (theFed9UHeaderFormat==FED9U_HEADER_APVERROR) {
	  tempBuf0 << "APVERROR";
	}
	
	tempBuf0 << "</HEADERTYPE><ID>" << theFed9UDescription.getFedId()
		 << "</ID><MAXTEMPCRITICALBEFPGA>" << theFed9UTempControl.getCritical()
		 << "</MAXTEMPCRITICALBEFPGA><MAXTEMPFPGABEFPGA>" << theFed9UTempControl.getFpgaHigh()
		 << "</MAXTEMPFPGABEFPGA><MAXTEMPLM82BEFPGA>" << theFed9UTempControl.getLm82High()
		 << "</MAXTEMPLM82BEFPGA><FEDMODE>";
	
	Fed9UDaqMode theFed9UDaqMode = theFed9UDescription.getDaqMode();
	if (theFed9UDaqMode==FED9U_MODE_ZERO_SUPPRESSED) {
	  tempBuf0 <<  "ZERO_SUPPRESSION";
	}
	else if (theFed9UDaqMode==FED9U_MODE_PROCESSED_RAW) {
	  tempBuf0 <<  "PROCESSED_RAW";
	}
	else if (theFed9UDaqMode==FED9U_MODE_VIRGIN_RAW) {
	  tempBuf0 <<  "VIRGIN_RAW";
	}
	else if (theFed9UDaqMode==FED9U_MODE_SCOPE) {
	  tempBuf0 <<  "SCOPE";
	}
	
	tempBuf0 << "</FEDMODE><NAME>" << theFed9UDescription.getName()
		 << "</NAME><OPTORXRESISTOR>" << theFed9UDescription.getOptoRXResistor()
		 << "</OPTORXRESISTOR><READROUTE>" << (theFed9UDescription.getBeFpgaReadRoute() == FED9U_ROUTE_VME ? "VME" : "SLINK64" )
		 << "</READROUTE><SCOPELENGTH>" << theFed9UDescription.getScopeLength()
		 << "</SCOPELENGTH><STATE>" << (theFed9UDescription.getFedBeFpgaDisable() ? "DISABLE" : "ENABLE")
		 << "</STATE><SUPERMODE>";
	Fed9UDaqSuperMode theFed9UDaqSuperMode = theFed9UDescription.getDaqSuperMode();
	if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE) {
	  tempBuf0  << "FAKE";
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_ZERO_LITE) {
	  tempBuf0  << "ZERO_LITE";
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_FAKE_ZERO_LITE) {
	  tempBuf0  << "FAKE_ZERO_LITE";
	}
	else if (theFed9UDaqSuperMode==FED9U_SUPER_MODE_NORMAL) {
	  tempBuf0  << "NORMAL";
	}
	
	tempBuf0  << "</SUPERMODE><TESTREGISTER>" << theFed9UDescription.getTestRegister()
		  << "</TESTREGISTER><TRIGGERSOURCE>";
	
	Fed9UTrigSource theFed9UTrigSource = theFed9UDescription.getTriggerSource();
	if (theFed9UTrigSource==FED9U_TRIG_TTC) {
	  tempBuf0  << "TTC";
	}
	else if (theFed9UTrigSource==FED9U_TRIG_BACKPLANE) {
	  tempBuf0  << "BACKPLANE";
	}
	else if (theFed9UTrigSource==FED9U_TRIG_SOFTWARE) {
	  tempBuf0  << "SOFTWARE";
	}
	
	tempBuf0  << "</TRIGGERSOURCE></RAWFED>";
	
	
	//TTCrx
	
	//Initialize a Fed9UTtcrxDescription object from the Fed9UDescription.
	Fed9UTtcrxDescription theFed9UTtcrxDescription = theFed9UDescription.getTtcrx();

	tempBuf1 << "<RAWTTCRX><IDFED>" << theFed9UDescription.getFedId()
		 << "</IDFED><TTCRXBRCSTSTRTWOCOARSEDELAY>" << theFed9UTtcrxDescription.getBrcstStrTwoCoarseDelay()
		 << "</TTCRXBRCSTSTRTWOCOARSEDELAY><TTCRXBRCSTSTRTWOFINEDELAY>" << ( true == theFed9UTtcrxDescription.getDeskewedClock2Selected() ? theFed9UTtcrxDescription.getClockDesTwoFineDelay() : theFed9UTtcrxDescription.getClockDesOneFineDelay() )
		 << "</TTCRXBRCSTSTRTWOFINEDELAY><TTCRXBUNCHCOUNTER>" << (theFed9UTtcrxDescription.getCounterOperation() & FED9U_TTCRX_ENABLE_BUNCH_CTR?"ENABLE":"DISABLE")
		 << "</TTCRXBUNCHCOUNTER><TTCRXCLOCK40DESKWEDTWO>" << (theFed9UTtcrxDescription.getDeskewedClock2Selected() ? "ENABLE" : "DISABLE" )
		 << "</TTCRXCLOCK40DESKWEDTWO><TTCRXCLOCK40DESKWEDTWOOUTPUT>" << (theFed9UTtcrxDescription.getDeskewedClock2Disable() ? "DISABLE" : "ENABLE")
		 << "</TTCRXCLOCK40DESKWEDTWOOUTPUT><TTCRXCLOCKL1ACCEPTOUTPUT>" << (theFed9UTtcrxDescription.getClockL1AcceptDisable()? "DISABLE" : "ENABLE")
		 << "</TTCRXCLOCKL1ACCEPTOUTPUT><TTCRXDLLPUMPCURRENT>" << theFed9UTtcrxDescription.getDllPumpCurrent()
		 << "</TTCRXDLLPUMPCURRENT><TTCRXEVENTCOUNTER>" << (theFed9UTtcrxDescription.getCounterOperation() & FED9U_TTCRX_ENABLE_EVENT_CTR?"ENABLE":"DISABLE")
		 << "</TTCRXEVENTCOUNTER><TTCRXHAMMINGCHECKING>" << (theFed9UTtcrxDescription.getHammingCheckingDisable() ? "DISABLE" : "ENABLE")
		 << "</TTCRXHAMMINGCHECKING><TTCRXI2CID>" << theFed9UTtcrxDescription.getI2cId()
		 << "</TTCRXI2CID><TTCRXIACID>" << theFed9UTtcrxDescription.getIacId()
		 << "</TTCRXIACID><TTCRXL1ACCEPTCOARSEDELAY>" << theFed9UTtcrxDescription.getL1AcceptCoarseDelay()
		 << "</TTCRXL1ACCEPTCOARSEDELAY><TTCRXL1ACCEPTFINEDELAY>" << theFed9UTtcrxDescription.getClockDesOneFineDelay()
		 << "</TTCRXL1ACCEPTFINEDELAY><TTCRXNONDESKWEDCLOCK40OUTPUT>" << (theFed9UTtcrxDescription.getNonDeskewedClockDisable()?"DISABLE":"ENABLE")
		 << "</TTCRXNONDESKWEDCLOCK40OUTPUT><TTCRXPLLPUMPCURRENT>" << theFed9UTtcrxDescription.getPllPumpCurrent()
		 << "</TTCRXPLLPUMPCURRENT><TTCRXPARALLELOUTPUTBUS>" << (theFed9UTtcrxDescription.getParrallelOutputDisable() ? "DISABLE" : "ENABLE")
		 << "</TTCRXPARALLELOUTPUTBUS><TTCRXSERIALBOUTPUT>" << (theFed9UTtcrxDescription.getSerialBDisable() ? "DISABLE" : "ENABLE")
		 << "</TTCRXSERIALBOUTPUT></RAWTTCRX>";
	
	
	//Voltage Monitor
	
	//Initialize a Fed9UVoltageControl object from the Fed9UDescription.
	Fed9UVoltageControl theFed9UVoltageControl = theFed9UDescription.getVoltageMonitor();
	
	
	tempBuf2 << "<RAWVOLTAGEMONITOR><IDFED>" << theFed9UDescription.getFedId()
		 << "</IDFED><VOLMONMAX12V>" << theFed9UVoltageControl.get12VoltMax()
		 << "</VOLMONMAX12V><VOLMONMAX2POINT5V>" << theFed9UVoltageControl.get2Point5VoltMax()
		 << "</VOLMONMAX2POINT5V><VOLMONMAX3POINT3V>" << theFed9UVoltageControl.get3Point3VoltMax()
		 << "</VOLMONMAX3POINT3V><VOLMONMAX5V>" << theFed9UVoltageControl.get5VoltMax()
		 << "</VOLMONMAX5V><VOLMONMAXCOREV>" << theFed9UVoltageControl.getCoreVoltageMax()
		 << "</VOLMONMAXCOREV><VOLMONMAXEXTERNALTEMP>" << theFed9UVoltageControl.getExternalTempMax()
		 << "</VOLMONMAXEXTERNALTEMP><VOLMONMAXINTERNALTEMP>" << theFed9UVoltageControl.getInternalTempMax()
		 << "</VOLMONMAXINTERNALTEMP><VOLMONMAXSUPPLYV>" << theFed9UVoltageControl.getSupplyVoltageMax()
		 << "</VOLMONMAXSUPPLYV><VOLMONMIN12V>" << theFed9UVoltageControl.get12VoltMin()
		 << "</VOLMONMIN12V><VOLMONMIN2POINT5V>" << theFed9UVoltageControl.get2Point5VoltMin()
		 << "</VOLMONMIN2POINT5V><VOLMONMIN3POINT3V>" << theFed9UVoltageControl.get3Point3VoltMin()
		 << "</VOLMONMIN3POINT3V><VOLMONMIN5V>" << theFed9UVoltageControl.get5VoltMin()
		 << "</VOLMONMIN5V><VOLMONMINCOREV>" << theFed9UVoltageControl.getCoreVoltageMin()
		 << "</VOLMONMINCOREV><VOLMONMINEXTERNALTEMP>" << theFed9UVoltageControl.getExternalTempMin()
		 << "</VOLMONMINEXTERNALTEMP><VOLMONMININTERNALTEMP>" << theFed9UVoltageControl.getInternalTempMin()
		 << "</VOLMONMININTERNALTEMP><VOLMONMINSUPPLYV>" << theFed9UVoltageControl.getSupplyVoltageMin()
		 << "</VOLMONMINSUPPLYV><VOLMONOFFSETTEMP>" << theFed9UVoltageControl.getTempOffset()
		 << "</VOLMONOFFSETTEMP><VOLMONSTANDBY>" << (theFed9UVoltageControl.getStandBy() ? "ENABLE" : "DISABLE")
		 << "</VOLMONSTANDBY><VOLMONSTATUSREGISTERRESET>" << (theFed9UVoltageControl.getResetStatusRegister()?"ENABLE":"DISABLE")
		 << "</VOLMONSTATUSREGISTERRESET><VOLMONWHICHOFFSETTEMP>" << (theFed9UVoltageControl.getOffsetTempSelect()?"INTERNAL":"EXTERNAL")
		 << "</VOLMONWHICHOFFSETTEMP></RAWVOLTAGEMONITOR>";
	
	
      	for (i=0; i<=7; ++i) {
	  //set the fedAddress for first fe unit
	  theFed9UAddress.setFedFeUnit(i).setFeUnitChannel(0);
	  //Initialize a Fed9UTempControl object from the Fed9UDescription
	  Fed9UTempControl theFed9UTempControl = theFed9UDescription.getTempControl(theFed9UAddress);
	  
	  
	  tempBuf3 << "<RAWFEFPGA><IDFED>" << theFed9UDescription.getFedId()
		   << "</IDFED><ID>" << static_cast<u16>( theFed9UAddress.getExternalFedFeUnit())
		   << "</ID><MAXTEMPCRITICAL>" << theFed9UTempControl.getCritical()
		   << "</MAXTEMPCRITICAL><MAXTEMPFPGA>" << theFed9UTempControl.getFpgaHigh()
		   << "</MAXTEMPFPGA><MAXTEMPLM82>" << theFed9UTempControl.getLm82High()
		   << "</MAXTEMPLM82><OPTORXCAPACITOR>" << theFed9UDescription.getOptoRxCapacitor(theFed9UAddress)
		   << "</OPTORXCAPACITOR><OPTORXINPUTOFFSET>" << theFed9UDescription.getOptoRxInputOffset(theFed9UAddress)
		   << "</OPTORXINPUTOFFSET><OPTORXOUTPUTOFFSET>" << theFed9UDescription.getOptoRxOutputOffset(theFed9UAddress)
		   << "</OPTORXOUTPUTOFFSET><STATE>" << (theFed9UDescription.getFedFeUnitDisable(theFed9UAddress)?"DISABLE":"ENABLE")
		   << "</STATE></RAWFEFPGA>";
	  

	  for (m=0; m<6; m++) {

	    theFed9UAddress.setFeUnitChannelPair(m);
	      
	    tempBuf4 << "<RAWCHANNELPAIR><IDFED>" << theFed9UDescription.getFedId()
		     << "</IDFED><IDFEFPGA>" << static_cast<u16>( theFed9UAddress.getExternalFedFeUnit())
		     << "</IDFEFPGA><ID>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannelPair() )
		     << "</ID><FAKEEVENTRANDOMSEED>" << theFed9UDescription.getFakeEventRandomSeed(theFed9UAddress)
		     << "</FAKEEVENTRANDOMSEED><FAKEEVENTRANDOMMASK>" << theFed9UDescription.getFakeEventRandomMask(theFed9UAddress)
		     << "</FAKEEVENTRANDOMMASK></RAWCHANNELPAIR>";
	  

	    for (j=0; j<2; ++j) {

	      theFed9UAddress.setFeUnitChannel(2*m+j);
	      
	      tempBuf5 << "<RAWCHANNEL><IDFED>" << theFed9UDescription.getFedId()
		       << "</IDFED><IDFEFPGA>" << static_cast<u16>( theFed9UAddress.getExternalFedFeUnit())
		       << "</IDFEFPGA><IDCHANNELPAIR>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannelPair() )
		       << "</IDCHANNELPAIR><ADCINPUTRANGEMODE>";
	      //Set the  ADC input range mode type
	      Fed9UAdcControls theFed9UAdcControls = theFed9UDescription.getAdcControls(theFed9UAddress);
	      bool adcParam[4] = {false};
	      adcParam[0] = theFed9UAdcControls._dfsen;
	      adcParam[1] = theFed9UAdcControls._dfsval;
	      adcParam[2] = theFed9UAdcControls._s1;
	      adcParam[3] = theFed9UAdcControls._s2;
	      if ((adcParam[0]==true)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
		tempBuf5 << "1V"; 
	      }
	      else if ((adcParam[0]==false)&&(adcParam[1]==false)&&(adcParam[2]==true)&&(adcParam[3]==false)) {
		tempBuf5 << "2V"; 
	      }
	      else if ((adcParam[0]==false)&&(adcParam[1]==false)&&(adcParam[2]==false)&&(adcParam[3]==false)) {
		tempBuf5 << "DISABLE"; 
	      }

 
	      tempBuf5 << "</ADCINPUTRANGEMODE><COARSEDELAY>" << theFed9UDescription.getCoarseDelay(theFed9UAddress)
		       << "</COARSEDELAY><DATACOMPLEMENT>" << (theFed9UDescription.getComplement(theFed9UAddress)? "DISABLE" : "ENABLE")
		       << "</DATACOMPLEMENT><FINEDELAY>" << theFed9UDescription.getFineDelay(theFed9UAddress)
		       << "</FINEDELAY><ID>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannel() )
		       << "</ID><THRESHOLD>" << theFed9UDescription.getFrameThreshold(theFed9UAddress)
		       << "</THRESHOLD><TRIMDACOFFSET>" << theFed9UDescription.getTrimDacOffset(theFed9UAddress)
		       << "</TRIMDACOFFSET></RAWCHANNEL>";
	      
	      
	      for (k=0; k<=1; ++k) {

		theFed9UAddress.setChannelApv(k);

		tempBuf6 << "<RAWAPVFED><IDFED>" << theFed9UDescription.getFedId()
			 << "</IDFED><IDFEFPGA>" << static_cast<u16>( theFed9UAddress.getExternalFedFeUnit())
			 << "</IDFEFPGA><IDCHANNELPAIR>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannelPair() )
			 << "</IDCHANNELPAIR><IDCHANNEL>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannel() )
			 << "</IDCHANNEL><ID>" << static_cast<u16>( theFed9UAddress.getExternalChannelApv() )
			 << "</ID><MEDIANOVERRIDE>" << theFed9UDescription.getMedianOverride(theFed9UAddress)
			 << "</MEDIANOVERRIDE><STATE>" << (theFed9UDescription.getApvDisable(theFed9UAddress)? "DISABLE" : "ENABLE" )
			 << "</STATE></RAWAPVFED>";
		
		
		if (usingStrips_) {
		  
		  tempBuf7 << "<RAWSTRIP><IDFED>" << theFed9UDescription.getFedId()
			   << "</IDFED><IDFEFPGA>" << static_cast<u16>( theFed9UAddress.getExternalFedFeUnit())
			   << "</IDFEFPGA><IDCHANNELPAIR>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannelPair() )
			   << "</IDCHANNELPAIR><IDCHANNEL>" << static_cast<u16>( theFed9UAddress.getExternalFeUnitChannel() )
			   << "</IDCHANNEL><IDAPV>" << static_cast<u16>( theFed9UAddress.getExternalChannelApv() )
			   << "</IDAPV><DATA>";

		  char * stripsBuf = new char[STRIPS_PER_APV*4];
		  getStripDataBuffer(stripsBuf);
		  XMLSize_t outputlength;
		  XMLSize_t length = 512;
		  XMLByte* encodedXML;
		  
		  //std::cout << "Base 64 encoded strips string = " <<  reinterpret_cast<char*>(xercesc::Base64::encode(reinterpret_cast<const XMLByte*>(stripsBuf),length,&outputlength))  
		  //		<< "length = " << outputlength << std::endl;
		  tempBuf7 << reinterpret_cast<char*>(encodedXML=xercesc::Base64::encode(reinterpret_cast<const XMLByte*>(stripsBuf),length,&outputlength));
		  delete encodedXML;
		  delete[] stripsBuf;
		  tempBuf7   << "</DATA></RAWSTRIP>";
		}
	      }
	    }
	  }
	}
      }
      tempBuf0 << "</ROWSET>";
      tempBuf1 << "</ROWSET>";
      tempBuf2 << "</ROWSET>";
      tempBuf3 << "</ROWSET>";
      tempBuf4 << "</ROWSET>";
      tempBuf5 << "</ROWSET>";
      tempBuf6 << "</ROWSET>";
      tempBuf7 << "</ROWSET>";
      
      buf[0] = tempBuf0.str();
      buf[1] = tempBuf1.str();
      buf[2] = tempBuf2.str();
      buf[3] = tempBuf3.str();
      buf[4] = tempBuf4.str();
      buf[5] = tempBuf5.str();
      buf[6] = tempBuf6.str();
      buf[7] = tempBuf7.str();
	      

    } catch(std::exception & exc) {
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!about to crash!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      std::cout << exc.what() << std::endl;
    } catch(...) {
      std::cout << "Oh Look there was an error when streaming out the super fast db format !!!" << std::endl;
    }
  }

  /**Method which constructs the DOM document.*/
  void Fed9UDescriptionToXmlToDb::constuctDOMDocument(void) throw(Fed9U::Fed9UXMLDescriptionException){
       //Get a DOM Core implementation.
    xercesc::DOMImplementation* impl =  xercesc::DOMImplementationRegistry::getDOMImplementation(X("Core"));

    //Create a DOM document with root element using the DOMImplementation. 
    //The first argument of createDocumentis the root element namespace name, 
    //the second argument is the name of the root element and the third argument is DOMDocumentType.
    doc = impl->createDocument(X("http://fed9u.web.cern.ch/fed9u/configuration"),X("fed9U:CMSTrackerFeds"),0);
    //Add the schema statements to the root element
    xercesc::DOMElement* rootElement = doc->getDocumentElement();
    rootElement->setAttribute(X("xmlns:xsi"), X("http://www.w3.org/2001/XMLSchema-instance"));
    rootElement->setAttribute(X("xsi:schemaLocation"), X("http://fed9u.web.cern.ch/fed9u/configuration ConfigFed9U.xsd"));

    // TODO remove the supervisorIP from the xml for the DB this is no longer necessary.
    //Create a the tags specific to the database upload 
    // <supervisor supervisorIp="XXX.XXX.XXX.XXX">  
    // <xdaqInstance id="n" className="Fed9USupervisor">
    //   xercesc::DOMElement* supervisorChildElement = doc->createElement(X("supervisor"));
    //rootElement->appendChild(supervisorChildElement);
    //supervisorChildElement->setAttribute(X("supervisorIp"), X( supervisorIp_.c_str()));

    //xercesc::DOMElement* xdaqInstanceChildElement = doc->createElement(X("xdaqInstance"));
    //supervisorChildElement->appendChild(xdaqInstanceChildElement);
    std::ostringstream numberString;
    //numberString << instance_;
    //xdaqInstanceChildElement->setAttribute(X("id"), X((numberString.str()).c_str()));
    //xdaqInstanceChildElement->setAttribute(X("className"), X(className_.c_str()));
   

    //Add Fed9U elements using the Fed9UDescription to assign the attributes.
    addDOMElements(rootElement);

   //Now write the DOM object to an XML file. Start by setting up a DOMWriter.
    //Instantiate a DOMWriter
    XMLCh tempStr[100];
    xercesc::XMLString::transcode("LS", tempStr, 99);
    impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
    theDOMWriter = ((xercesc::DOMImplementationLS*)impl)->createLSSerializer();

    //Plug in error handler  
    theDOMWriter->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMErrorHandler,&errorHandler);
    //reset error count
    errorHandler.resetErrors();

    //Include whitespaces in the output
    if (theDOMWriter->getDomConfig()->canSetParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true)) {
      theDOMWriter->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
    }
  }

  //***************************************************************************************************
  //Method which adds elements to a DOMDocument using the Fed9UDescription to get the attribute values.
  // this method has been overridden so that we can write the strips in a different way for the database
  void Fed9UDescriptionToXmlToDb::addDOMElements(xercesc::DOMElement *rootElement) throw (Fed9UXMLDescriptionException)
  {
    //    xercesc::DOMElement* rootElement = doc->getDocumentElement();

    try {
      //Create a fed element and attach it to the root element. Then set the attributes from the Fed9UDescription.
      xercesc::DOMElement* fedChildElement = doc->createElement(X("fed"));
      rootElement->appendChild(fedChildElement);
      addFedAttributes(fedChildElement);
      //Now add child elements to fed
      unsigned short i=0, j=0, k=0, l=0, m=0;
      //Create feFpga child elements to fed. Then set attributes from Fed9UDescription.
      for (i=0; i<=7; ++i) {
	xercesc::DOMElement* feFpgaChildElement = doc->createElement(X("feFpga"));
	fedChildElement->appendChild(feFpgaChildElement);
	addFeFpgaAttributes(feFpgaChildElement,i);
	//Create channel child elements to feFpga. Then set attributes from Fed9UDescription.
	// Create channel pair child elements to feFpga, then set attribus from Fed9uDescription
	for (m=0; m<6; m++) {
	  xercesc::DOMElement* channelPairChildElement = doc->createElement(X("channelPair"));
	  feFpgaChildElement->appendChild(channelPairChildElement);
	  addChannelPairAttributes(channelPairChildElement, m);
	  
	  //Create channel child elements to channel pair. Then set attributes from Fed9UDescription.
	  for (j=0; j<2; ++j) {
	    xercesc::DOMElement* channelChildElement = doc->createElement(X("channel"));
	    channelPairChildElement->appendChild(channelChildElement);
	    addChannelAttributes(channelChildElement, 2*m+j);
	    //Create APV child elements to channel. Then set attributes from Fed9UDescription.
	    for (k=0; k<=1; ++k) {
	      xercesc::DOMElement* apvChildElement = doc->createElement(X("apv"));
	      channelChildElement->appendChild(apvChildElement);
	      addApvAttributes(apvChildElement, k);
	      // If usingStrips_ is true then we create strip child elements to APV. Then set attributes from Fed9UDescription.
	      if (usingStrips_) {
		// now rather than writing one tag for each strip, we loop over all strips and create one binary blob which we write to the 
		// <strips> tag, this is done within the method addStripsAttribute
		xercesc::DOMElement* stripChildElement = doc->createElement(X("strips"));
		apvChildElement->appendChild(stripChildElement);
		addStripsAttribute(stripChildElement);
	      }
	    }
	  }
	}
      }
    }
    catch (const xercesc::DOMException& e) {
      std::ostringstream code;
      code << e.code;
      std::string errMesg = "DOMException. Message: " + std::string(Fed9UStrX(e.msg).localForm()) + "  Code: " + code.str() + " (see DOMException.hpp).";
      //NOTE: Cannot use RETHROW here since DOMException does not inherit from std::exception                               
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_DOM, errMesg.c_str()));
    }
    catch (std::exception &e) {
      RETHROW(e, Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"std::exception."));
    }
    catch (...) {
      THROW(Fed9UXMLDescriptionException(Fed9UXMLDescriptionException::ERROR_UNKNOWN,"Unknown excpetion."));
    }
  }

}

#endif
#endif
#endif
