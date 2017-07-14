/*************************************************************************
 * Author: C. Delaere                                                    *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/

#ifndef XSI_CONNECTION_H
#define XSI_CONNECTION_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <unistd.h>

#include "xdaq/Object.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/URLEndpoint.h"
//#include "xoap/SOAPConnection.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPElement.h"
#include "xoap/SOAPMessage.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPBodyElement.h"
#include "xoap/SOAPName.h"
#include "xoap/AttachmentPart.h"
#include "xoap/SOAPAllocator.h"
#include "xoap/exception/Exception.h"
#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationContextImpl.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#define NS_PVSS_PREFIX "psx"
#define NS_SMI_PREFIX "smi"
#define PSX_PVSS_NS_URI "http://xdaq.cern.ch/xdaq/xsd/2006/psx-pvss-10.xsd"
#define PSX_SMI_NS_URI "http://xdaq.web.cern.ch/xdaq/xsd/2006/psx-smi-10.xsd"
#define PVSS_APPLICATION_DESCRIPTOR "psx"
#define PVSS_APPLICATION_INSTANCE 0
#define SMI_APPLICATION_DESCRIPTOR "psx"
#define SMI_APPLICATION_INSTANCE 0
#define SOAPACTIONSTRING "urn:xdaq-application:service=psx"

//! Connect to PVSS through PSX (PVSS to SOAP interface)
/*!
    XPditor is a class that implement SOAP messaging with PVSS, 
    following the PSX specifications. 
    It is designed to handle both synchronous and asynchronous communication 
    for raw datapoint access and fsm access.
*/
class XPditor
{
  public:
    //! constructor
    /*!
      Establish the SOAP connection. Parameters are the PVSS host and connection port, as well as the originator.
      Default behaviour is to connect to port 43210 of localhost, as cms.
      \param application descriptor
      \param originator transaction originator
      \param service (psx)
      \param instance of the psx (0)
      \param string that contains the service to be set ("urn:xdaq-application:service=psx") for the head (setHeader("SOAPAction", soapActionString))
      * 
    */
    XPditor(xdaq::Application* application, 
	    const std::string originator = "StripTrackerPsx", 
	    const std::string pvssClassName = PVSS_APPLICATION_DESCRIPTOR, const unsigned int instance = PVSS_APPLICATION_INSTANCE,
	    const std::string soapActionString = SOAPACTIONSTRING ) ;

    //! destructor
    /*!
      Deletes the SOAP connection, and unsubscribes pending asynchronous transactions.
    */
    virtual ~XPditor();
    // inline setters and getters for the authentification parameters
    //! set the login
    inline void setLogin(const std::string login = "") { login_ = login; }
    //! get the login 
    inline std::string getLogin() const { return login_; }
    //! set the password
    inline void setPassword(const std::string password = "") { password_ = password; }
    //! get the password
    inline std::string getPassword() const { return password_; }
    // ownership
    /* no ownership anymore in PSX
    */
    virtual bool takeOwnership(const std::string domain="PSX_RAW_ACCESS");
    //! release the ownership of a domain.
    /* no ownership anymore in PSX
    */
    virtual bool releaseOwnership(const std::string domain="PSX_RAW_ACCESS");
    // raw synchronous dp access
    //! set the value of a datapoint
    virtual void dpSet(const std::string dp, const std::string value, bool crossCheck = false);
    //! set the value of many datapoints
    virtual void dpSet(std::vector<std::string> dp, std::vector<std::string> value, bool crossCheck = false);
    //! read the value of a datapoint
    virtual std::string dpGetSync(const std::string dp);
    //! read the value of many datapoints
    virtual std::map<std::string,std::string> dpGetSync(std::vector<std::string> dps, bool crossCheck = false);
    //! obtain the list of available datapoints
    virtual std::vector<std::string> dpGetNames(const std::string dp);
    // raw asynchronous dp access
    //! subscribe a datapoint
    virtual std::string dpSubscribe(const std::string dp, int* id=NULL, const std::string context="");
    //! subscribe several datapoints as a whole
    virtual std::map<std::string,std::string> dpSubscribe(std::vector<std::string> dps, int* id=NULL, const std::string context="");
    //! unsubscribed a datapoint
    /*! if the datapoint has been subscribed as part of a global transaction, all dps will be unsubscribed. */
    virtual bool dpUnsubscribe(const std::string dp);
    //! unsubscribed a (set of) datapoint(s) via the subscription id.
    virtual bool dpUnsubscribe(const int id);
    //! read the value of a datapoint in asynchronous mode
    virtual std::string dpGetAsync(const std::string dp);
    // fsm tree ownership
    //! take the ownership of a fsm tree
    virtual bool fsmTakeTree(const std::string domain, const bool exclusive=true);
    //! release the ownership of a fsm tree
    virtual bool fsmReleaseTree(const std::string domain, const bool all=true);
    //! share the ownership of a fsm tree
    virtual bool fsmShareTree(const std::string domain);
    // fsm synchronous access
    //! send a command to some FSM 
    virtual bool fsmSendCommand(const std::string domain, const std::string command);
    //! read the state of a FSM (synchronous mode)
    virtual std::string fsmGetStateSync(const std::string domain);
    // fsm asynchronous access
    //! connect to the state of a FSM, to be notified of changes asynchronously
    virtual bool fsmConnectState(const std::string domain, const std::string context="");
    //! disconnect a FSM 
    virtual bool fsmDisconnectState(const std::string domain);

  protected:
    std::vector<std::string> domainOwnership_; //!< list of owned domains
    std::vector<std::string> domainHasTree_;   //!< list of connected fsm tree 
    std::map<std::string,std::string> domainsRegistered_; //!< list of registered domains
    std::map<std::string,std::string> dpRegistered; //!< list of registered datapoints
  private:
    //! private method to prepare the SOAP messages
    xoap::SOAPElement preparePVSSBody(xoap::SOAPEnvelope& envelope, const std::string command);
    xoap::SOAPElement prepareSMIBody(xoap::SOAPEnvelope& envelope, const std::string command);
    std::string login_;                 //!< login
    std::string password_;              //!< password
    std::string originator_;            //!< originator
    std::string meOffCourse_;           //!< local url
    std::string myURN_;                 //!< local urn
    std::string soapActionString_;      //!< SOAP action string for the header
    xdaq::ApplicationDescriptor* pvssAD_;//!< application descriptor for the PVSS messenger
    xdaq::ApplicationDescriptor* smiAD_; //!< application descriptor for the SMI messenger
    xdaq::ApplicationContext* myContext_;//!< application context to send SOAP messages
    xdaq::Application* myApplication_ ; //!< the application descriptor that wants to send command
};

/** Create an access to PSX
 */
XPditor::XPditor(xdaq::Application* application, 
		 const std::string originator, 
		 const std::string pvssClassName, const unsigned int instance, 
		 const std::string soapActionString ): originator_(originator), soapActionString_(soapActionString),myApplication_(application) {

  myContext_ = application->getApplicationContext();
  meOffCourse_ = myContext_->getContextDescriptor()->getURL();
  myURN_ = application->getApplicationDescriptor()->getURN();
  pvssAD_=myContext_->getDefaultZone()->getApplicationDescriptor(pvssClassName,instance);
  smiAD_=myContext_->getDefaultZone()->getApplicationDescriptor(SMI_APPLICATION_DESCRIPTOR,SMI_APPLICATION_INSTANCE);

// This part run other the group in order to find a service, not used since we set the soapActionString that defines the destination
//   unsigned int several = 0 ;
//   pvssAD_ = NULL ;
//   // get all groups
//   std::set< xdaq::ApplicationGroup * > mygroups = myContext_->getDefaultZone()->getGroups();
//   if (mygroups.empty()) XCEPT_RAISE(xcept::Exception,"No service " + pvssClassName + " available (no group: myContext_->getDefaultZone()->getGroups() is empty)") ;
//   else {
//     std::cout<< "------------------------>  On a trouve " << mygroups.size() << " groups" << std::endl;
//     // try to find the service on all groups found
//     for (std::set<xdaq::ApplicationGroup*>::iterator ig = mygroups.begin(); ig != mygroups.end() ; ig++) {
//       xdaq::ApplicationGroup* mygroup = dynamic_cast<xdaq::ApplicationGroup*> (*ig);
//       if (mygroup != NULL) {
// 	std::cout<< "------------------------> " << mygroup->getName() <<std::endl;
// 	std::set< xdaq::ApplicationDescriptor*> mydescs= mygroup->getApplicationDescriptors("service",pvssClassName);
// 	std::cout<< "------------------------>  On a trouve " << mydescs.size() << " " << pvssClassName << " services" << std::endl;
// 	if (mydescs.size() == 1) {	  
// 	  std::set< xdaq::ApplicationDescriptor*>::iterator id=mydescs.begin();
// 	  pvssAD_ = dynamic_cast<xdaq::ApplicationDescriptor*> (*id);
// 	  std::cout << "------------------------> Found the service" << std::endl ;
// 	}
// 	else {
// 	  std::cout << "------------------------> Found several services on group " << mygroup->getName() << std::endl ;
// 	  several ++ ;
// 	}
//       }
//     }
//   }
//   // Service found ?
//   if (pvssAD_ == NULL) {
//     if (several) XCEPT_RAISE(xcept::Exception,"Found too much services for " + pvssClassName) ;
//     else XCEPT_RAISE(xcept::Exception,"No service " + pvssClassName + " available") ;
//   }
//   std::cout << pvssAD_->getURN() << " | " << pvssAD_->getContextDescriptor()->getURL() << " | " << pvssAD_->getLocalId() << std::endl ;
}

/**
 */
XPditor::~XPditor() 
{ 
  std::vector<std::string> tmp;
  //unregister dp
  for(std::map<std::string,std::string>::iterator dp=dpRegistered.begin();dp!=dpRegistered.end();dp++)
    tmp.push_back(dp->first);
  for(std::vector<std::string>::iterator dp=tmp.begin();dp<tmp.end();dp++)
    dpUnsubscribe(*dp);
  //unregister states
  for(std::map<std::string,std::string>::iterator dom=domainsRegistered_.begin();dom!=domainsRegistered_.end();dom++)
    tmp.push_back(dom->first);
  for(std::vector<std::string>::iterator dom=tmp.begin();dom<tmp.end();dom++)
    fsmDisconnectState(*dom);
  //release all domains, and release the ownership
  tmp.clear();
  for(std::vector<std::string>::iterator dom=domainHasTree_.begin();dom<domainHasTree_.end();dom++)
    tmp.push_back(*dom);
  for(std::vector<std::string>::iterator dom=tmp.begin();dom<tmp.end();dom++)
    fsmReleaseTree(*dom);
  tmp.clear();
  //release the ownership
  for(std::vector<std::string>::iterator dom=domainOwnership_.begin();dom<domainOwnership_.end();dom++)
    tmp.push_back(*dom);
  for(std::vector<std::string>::iterator dom=tmp.begin();dom<tmp.end();dom++)
    releaseOwnership();
}

/** Not implemented
 */
bool XPditor::takeOwnership(const std::string domain)
{
  // FAKE: no ownership
  // first check we don't already have the ownership
  std::vector<std::string>::iterator result = find(domainOwnership_.begin(), domainOwnership_.end(), domain);
  if(result!=domainOwnership_.end()) { 
     XCEPT_RAISE(xcept::Exception,"XPditor::takeOwnership::existingOwnership");
     return false;
  }
  domainOwnership_.push_back(domain);
  return true;
}

/** Not implemented
 */
bool XPditor::releaseOwnership(const std::string domain)
{
  // FAKE: no ownership
  //first check that we have ownership
  std::vector<std::string>::iterator result = find(domainOwnership_.begin(), domainOwnership_.end(), domain);
  if(result==domainOwnership_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::releaseOwnership::noOwnership");
     return false;
  }
  domainOwnership_.erase(result);
  return true;
}

/** Set a vector of datapoints
 */
void XPditor::dpSet(std::vector<std::string> dps, std::vector<std::string> values, bool crossCheck)
{
  //--- compose SOAP request
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = preparePVSSBody(envelope,"dpSet");
  for (std::vector<std::string>::const_iterator dp = dps.begin(), value = values.begin() ; (dp < dps.end()) && (value < values.end()); dp ++, value++) {
    xoap::SOAPName key = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
    xoap::SOAPElement keyElement = bodyElement.addChildElement(key);
    xoap::SOAPName datapoint = envelope.createName("name");
    keyElement.addAttribute(datapoint, dp->c_str());
    keyElement.addTextNode(value->c_str());
  }

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = pvssAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);

  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  if (body.hasFault()) {

    xoap::SOAPFault fault = body.getFault() ;
    XCEPT_RAISE(xcept::Exception,fault.getFaultString()) ;
  }
  /* -------------------------------------------------------------------------------------------------
  xoap::SOAPName response = envelope.createName("dpSetResponse",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement == bodyElements.end() ){
  XCEPT_RAISE(xcept::Exception,"XPditor::dpSet::noResponse");
  return;
  }
  ----------------------------------------------------------------------------------- */
  // if requested, cross-check the values. It must match what has just been set.
  if(crossCheck) {
    std::map<std::string,std::string> newVals = dpGetSync(dps,true);
    bool valid = (newVals.size()==dps.size());
    for (std::vector<std::string>::const_iterator dp = dps.begin(), value = values.begin() ; (dp < dps.end()) && (value < values.end()); dp ++, value++) {
      valid &= (newVals[std::string(dp->c_str())]==std::string(value->c_str()));
    }
    if(!valid) XCEPT_RAISE(xcept::Exception,"XPditor::dpSet::value not set correctly");
  }
  return;
}

/**
 */
void XPditor::dpSet(const std::string dp, const std::string value, bool crossCheck)
{
  std::vector<std::string> vdp;
  vdp.push_back(dp);
  std::vector<std::string> vval;
  vval.push_back(value);
  dpSet(vdp,vval,crossCheck);
  return;
}

/**
 */
std::map<std::string,std::string> XPditor::dpGetSync(std::vector<std::string> dps, bool crossCheck)
{
  std::map<std::string,std::string> result;

  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPElement bodyElement = preparePVSSBody(envelope,"dpGet");
  for (std::vector<std::string>::const_iterator dp = dps.begin(); dp<dps.end(); dp ++) {
    xoap::SOAPName key = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
    xoap::SOAPElement keyElement = bodyElement.addChildElement(key);
    xoap::SOAPName datapoint = envelope.createName("name");
    keyElement.addAttribute(datapoint, dp->c_str());
  }

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = pvssAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();

  if (body.hasFault()) {

    xoap::SOAPFault fault = body.getFault() ;
    XCEPT_RAISE(xcept::Exception,fault.getFaultString()) ;
    //throw std::string(fault.getFaultString()) ;
  }

  xoap::SOAPName response = envelope.createName("dpGetResponse",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  for(std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();p_bodyElement<bodyElements.end();p_bodyElement++) {
    xoap::SOAPName dpValue = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
    xoap::SOAPName dpName = envelope.createName("name");
    std::vector< xoap::SOAPElement > keyElements = p_bodyElement->getChildElements(dpValue);
    for(std::vector< xoap::SOAPElement >::iterator p_keyElement = keyElements.begin();p_keyElement<keyElements.end();p_keyElement++) {
      result[p_keyElement->getAttributeValue(dpName)] = p_keyElement->getValue();
    }
  }
  // what to do when some dps are not returned ???
  // for the time being let the responsability to the user.
  // the above choice has implications on the check done in XPditor::dpGetSync(const std::string dp)
  // a deep check is only performed if the second parameter is true.
  if(crossCheck) {
    bool valid = (result.size()==dps.size());
    for (std::vector<std::string>::const_iterator dp = dps.begin(); dp<dps.end(); dp ++) {
      valid &= (result.count(std::string(dp->c_str()))==1);
    }
    if(!valid) XCEPT_RAISE(xcept::Exception,"XPditor::dpGetSync::returned values does not match query");
  }
  return result;
}

/**
 */
std::string XPditor::dpGetSync(const std::string dp)
{
  std::vector<std::string> dpv;
  dpv.push_back(dp);
  std::map<std::string,std::string> responses = dpGetSync(dpv);

  //std::cout << dp << std::endl ;
  //std::cout << "Receive " << responses.size() << " elements" << std::endl ;
  //for (std::map<std::string,std::string>::iterator itStr = responses.begin() ; itStr != responses.end() ; itStr ++) {
    //std::cout << dp << "/" << itStr->first << " => " << itStr->second << std::endl ;
    //if (itStr->first.find_first_of(dp) != string::npos) std::cout << "FOUND" << std::endl;
    //else std::cout << "Not equals" << std::endl ;
  //}

  if(!responses.count(dp)) {

    XCEPT_RAISE(xcept::Exception,"XPditor::dpGetSync::no value returned for the datapoint " + dp);
  }
  return responses[dp];
}

/**
 */
std::vector<std::string> XPditor::dpGetNames(const std::string dp)
{
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPElement bodyElement = preparePVSSBody(envelope,"dpGetNames");
  bodyElement.addTextNode(dp.c_str());

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = pvssAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("dpGetNamesResponse",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector<std::string> result;
  for(std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();p_bodyElement<bodyElements.end();p_bodyElement++) {
      xoap::SOAPName dpValue = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
      xoap::SOAPName dpName = envelope.createName("name");
      std::vector< xoap::SOAPElement > keyElements = p_bodyElement->getChildElements(dpValue);
      for(std::vector< xoap::SOAPElement >::iterator p_keyElement = keyElements.begin();p_keyElement<keyElements.end();p_keyElement++) {
	result.push_back(p_keyElement->getAttributeValue(dpName));
      }
  }
  return result;
}

/**
 */
bool XPditor::fsmSendCommand(const std::string domain, const std::string command)
{
  //--- compose SOAP request
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"send");
  xoap::SOAPName fsm   = envelope.createName("object");
  xoap::SOAPName cmd   = envelope.createName("command");
  xoap::SOAPName owner = envelope.createName("owner");
  bodyElement.addAttribute(fsm, domain.c_str());
  bodyElement.addAttribute(cmd, command.c_str());
  bodyElement.addAttribute(owner, originator_.c_str());

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("sendResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
       return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmSendCommand::noResponse");
  return false;
}

/**
 */
std::string XPditor::fsmGetStateSync(const std::string domain)
{
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"getState");
  xoap::SOAPName fsm   = envelope.createName("object");
  xoap::SOAPName owner = envelope.createName("owner");
  bodyElement.addAttribute(fsm, domain.c_str());
  bodyElement.addAttribute(owner, originator_.c_str());

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("getStateResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
    return p_bodyElement->getValue();
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmGetStateSync::noResponse");
  return std::string("");
}

/**
 */
bool XPditor::fsmTakeTree(const std::string domain, const bool exclusive)
{
  // first check we don't already have the tree
  std::vector<std::string>::iterator result = find(domainHasTree_.begin(), domainHasTree_.end(), domain);
  if(result!=domainHasTree_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::fsmTakeTree::existingOwnership");
     return false ;
  }
  //--- compose SOAP request
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"take");
  xoap::SOAPName fsm   = envelope.createName("object");
  xoap::SOAPName owner = envelope.createName("owner");
  xoap::SOAPName mode  = envelope.createName("exclusive");
  bodyElement.addAttribute(fsm, domain.c_str());
  bodyElement.addAttribute(owner, originator_.c_str());
  bodyElement.addAttribute(mode, exclusive?"true":"false");

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("takeResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
      domainHasTree_.push_back(domain);
      return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmTakeTree::noResponse");
  return false;
}

/**
 */
bool XPditor::fsmReleaseTree(const std::string domain, const bool all)
{
  //first check that we have the tree
  std::vector<std::string>::iterator result = find(domainHasTree_.begin(), domainHasTree_.end(), domain);
  if(result==domainOwnership_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::fsmReleaseTree::noOwnership");
     return false;
  }
  //--- compose SOAP request
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"release");
  xoap::SOAPName fsm   = envelope.createName("object");
  xoap::SOAPName owner = envelope.createName("owner");
  xoap::SOAPName mode  = envelope.createName("all");
  bodyElement.addAttribute(fsm, domain.c_str());
  bodyElement.addAttribute(owner, originator_.c_str());
  bodyElement.addAttribute(mode, all?"true":"false");

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("releaseResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
      domainHasTree_.erase(result);
      return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmReleaseTree::noResponse");
  return false;
}

/**
 */
bool XPditor::fsmShareTree(const std::string domain)
{
  //first check that we have the tree
  std::vector<std::string>::iterator result = find(domainHasTree_.begin(), domainHasTree_.end(), domain);
  if(result==domainOwnership_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::fsmShareTree::noOwnership");
     return false;
  }
  return fsmTakeTree(domain, false);
}

/**
 */
bool XPditor::fsmConnectState(const std::string domain, const std::string context)
{
  // check that we are not already connected to that state
  std::map<std::string,std::string>::iterator result = domainsRegistered_.find(domain);
  if(result != domainsRegistered_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::fsmConnectState::alreadyConnected");
     return false ;
  }
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"connect");
  xoap::SOAPName replyTo = envelope.createName("url");
  xoap::SOAPName fsm   = envelope.createName("object");
  xoap::SOAPName owner = envelope.createName("owner");
  xoap::SOAPName contextn = envelope.createName("context");
  xoap::SOAPName actionn  = envelope.createName("action");
  bodyElement.addAttribute(fsm, domain.c_str());
  bodyElement.addAttribute(owner, originator_.c_str());
  bodyElement.addAttribute(replyTo,meOffCourse_);
  bodyElement.addAttribute(contextn,context);
  bodyElement.addAttribute(actionn,myURN_);

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("connectResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
      xoap::SOAPName context = envelope.createName("id");
      domainsRegistered_[domain] = p_bodyElement->getAttributeValue(context);
      return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmConnectState::noResponse");
  return false;
}

/**
 */
bool XPditor::fsmDisconnectState(const std::string domain)
{
  // check that we are connected to that state
  std::map<std::string,std::string>::iterator result = domainsRegistered_.find(domain);
  if(result == domainsRegistered_.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::fsmDisconnectState::notConnected");
     return false;
  }
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = prepareSMIBody(envelope,"disconnect");
  xoap::SOAPName context = envelope.createName("id");
  bodyElement.addAttribute(context, domainsRegistered_[domain]);

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = smiAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("disconnectResponse",NS_SMI_PREFIX,PSX_SMI_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
    domainsRegistered_.erase(result);
    return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::fsmDisconnectState::noResponse");
  return false;
}

std::map<std::string,std::string> XPditor::dpSubscribe(std::vector<std::string> dps, int* id, const std::string context)
{
  std::map<std::string,std::string> result;
  // check that we didn't already subscribed the dps
  for(std::vector<std::string>::iterator dp = dps.begin(); dp<dps.end(); dp++) {
    std::map<std::string,std::string>::iterator res = dpRegistered.find(*dp);
    if(res != dpRegistered.end()) {
       XCEPT_RAISE(xcept::Exception,"XPditor::dpSubscribe::alreadySubscribed");
       return result;
    }
  }
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = preparePVSSBody(envelope,"dpConnect");
  xoap::SOAPName replyTo  = envelope.createName("url");
  xoap::SOAPName contextn = envelope.createName("context");
  xoap::SOAPName actionn  = envelope.createName("action");
  bodyElement.addAttribute(replyTo,meOffCourse_);
  bodyElement.addAttribute(contextn,context);
  bodyElement.addAttribute(actionn,myURN_);
  xoap::SOAPName name = envelope.createName("name");
  for(std::vector<std::string>::iterator dp = dps.begin(); dp<dps.end(); dp++) {
    xoap::SOAPName key = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
    xoap::SOAPElement keyElement = bodyElement.addChildElement(key);
    keyElement.addAttribute(name, dp->c_str());
  }

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = pvssAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response  = envelope.createName("dpConnectResponse");
  xoap::SOAPName datapoint = envelope.createName("dp",NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ){
      xoap::SOAPName context = envelope.createName("id");
      std::string theId = p_bodyElement->getAttributeValue(context);
      if(id) *id = atoi(theId.c_str());
      xoap::SOAPName dpName = envelope.createName("name");
      std::vector< xoap::SOAPElement > keyElements = p_bodyElement->getChildElements(dpName);
      for(std::vector< xoap::SOAPElement >::iterator p_keyElement = keyElements.begin();p_keyElement<keyElements.end();p_keyElement++) {
        result[p_keyElement->getAttributeValue(dpName)] = p_keyElement->getValue();
        dpRegistered[p_keyElement->getAttributeValue(dpName)] = theId; 
      }
      return result;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::dpSubscribe::noResponse");
  return result;
}

/**
 */
std::string XPditor::dpSubscribe(const std::string dp, int* id, const std::string context)
{
  std::vector<std::string> dpv;
  dpv.push_back(dp);
  std::map<std::string,std::string> res = dpSubscribe(dpv,id,context);
  if(!res.count(dp)) {
	XCEPT_RAISE(xcept::Exception,"XPditor::dpSubscribe::no subscription id returned.");
  }
  return res[dp];
}

/**
 */
bool XPditor::dpUnsubscribe(const int id)
{
  // put id in a string
  std::stringstream ss;
  ss << id;
  std::string ids(ss.str());
  // check that we know that subscription id 
  std::vector<std::string> concernedDps;
  for(std::map<std::string,std::string>::iterator p_dpm = dpRegistered.begin(); p_dpm != dpRegistered.end(); p_dpm++) {
     if (p_dpm->second == std::string(ids)) concernedDps.push_back(p_dpm->first);
  }
  if(!concernedDps.size()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::dpUnsubscribe::notSubscribed");
     return false;
  }
  //--- compose SOAP message to be send to PVSS to SOAP interface
  xoap::MessageReference request = xoap::createMessage();
  xoap::SOAPEnvelope envelope = request->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPElement bodyElement = preparePVSSBody(envelope,"dpDisconnect");
  xoap::SOAPName psiContext = envelope.createName("id");
  bodyElement.addAttribute(psiContext, std::string(ids));

  // add a header to the message for external service
  if (soapActionString_ != "") request->getMimeHeaders()->setHeader("SOAPAction", soapActionString_);

  //--- send SOAP request and wait for SOAP reply
  xdaq::ApplicationDescriptor * d = pvssAD_;
  xoap::MessageReference reply = myContext_->postSOAP(request, *myApplication_->getApplicationDescriptor(), *d);
  //xoap::MessageReference reply = myContext_->postSOAP(request, d);
  //--- decode SOAP reply
  envelope = reply->getSOAPPart().getEnvelope();
  body = envelope.getBody();
  xoap::SOAPName response = envelope.createName("dpDisconnectResponse");
  std::vector< xoap::SOAPElement > bodyElements = body.getChildElements(response);
  std::vector< xoap::SOAPElement >::iterator p_bodyElement = bodyElements.begin();
  if ( p_bodyElement != bodyElements.end() ) {
     for(std::vector<std::string>::iterator p_concernedDps=concernedDps.begin(); p_concernedDps<concernedDps.end(); p_concernedDps++) {
    	dpRegistered.erase(dpRegistered.find(*p_concernedDps));
     }
     return true;
  }
  XCEPT_RAISE(xcept::Exception,"XPditor::dpUnsubscribe::noResponse");
  return false;
}

/**
 */
bool XPditor::dpUnsubscribe(const std::string dp)
{
  // check that we have subscribed that dp
  std::map<std::string,std::string>::iterator result = dpRegistered.find(dp);
  if(result == dpRegistered.end()) {
     XCEPT_RAISE(xcept::Exception,"XPditor::dpUnsubscribe::notSubscribed");
     return false;
  }
  return dpUnsubscribe(atoi((result->second).c_str())); 
}

/**
 */
std::string XPditor::dpGetAsync(const std::string dp)
{
  // No dpGetAsync in PSX
  return std::string("");
}

/**
 */
xoap::SOAPElement XPditor::preparePVSSBody(xoap::SOAPEnvelope& envelope, const std::string command)
{
  //envelope.addNamespaceDeclaration ("xsd", "http://www.w3.org/2001/XMLSchema");
  //envelope.addNamespaceDeclaration ("xsi", "http://www.w3.org/2001/XMLSchema-instance");
  //enveloppe.setEncodingStyle("http://schemas.xmlsoap.org/soap/encoding/");
  //envelope.addNamespaceDeclaration ("SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/");
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName cmd = envelope.createName(command.c_str(),NS_PVSS_PREFIX,PSX_PVSS_NS_URI);
  xoap::SOAPElement bodyElement = body.addBodyElement(cmd);
/*
  if((login_ != "") && (password_ != "")) {
    xoap::SOAPName username = envelope.createName("username");
    bodyElement.addAttribute(username, login_.c_str());
    xoap::SOAPName password = envelope.createName("password");
    bodyElement.addAttribute(password, password_.c_str());
  }
*/
  return bodyElement;
}

/**
 */
xoap::SOAPElement XPditor::prepareSMIBody(xoap::SOAPEnvelope& envelope, const std::string command)
{
  //envelope.addNamespaceDeclaration ("xsd", "http://www.w3.org/2001/XMLSchema");
  //envelope.addNamespaceDeclaration ("xsi", "http://www.w3.org/2001/XMLSchema-instance");
  //enveloppe.setEncodingStyle("http://schemas.xmlsoap.org/soap/encoding/");
  //envelope.addNamespaceDeclaration ("SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/");
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName cmd = envelope.createName(command.c_str(),NS_SMI_PREFIX,PSX_SMI_NS_URI);
  xoap::SOAPElement bodyElement = body.addBodyElement(cmd);
/*
  if((login_ != "") && (password_ != "")) {
    xoap::SOAPName username = envelope.createName("username");
    bodyElement.addAttribute(username, login_.c_str());
    xoap::SOAPName password = envelope.createName("password");
    bodyElement.addAttribute(password, password_.c_str());
  }
*/
  return bodyElement;
}

#endif
