#ifndef _TrackerParser_h_
#define _TrackerParser_h_



#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/util/TranscodingException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <string>
#include <stdlib.h>


#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMInputSource.hpp>
using namespace std;
using namespace XERCES_CPP_NAMESPACE;
#include <string>
#include <memory>

#include "FedPmcXmlDescription.h"
#include "TscXmlDescription.h"
#include <vector>
#include "DetGeo.h"
#include <map>


/**
\class TrackerParser
\author L.Mirabito
\date March 2001
\version 1.00

\brief daq.dtd XML parser based on Xerces library

It Parse a given file, create and fill corresponding description.
\version 1.1
\date October 2001
\brief add Nim 470 support
\date December 2001 
\brief add Nhq201 and TRHX support W.Beaumont   
*/

class TrackerParser
{
private:
vector <FedChannelConnectionDescription*> *fedChan_;
multimap <int,FedChannelConnectionDescription*> *mapFedIdChannel_;
map <int,FedChannelConnectionDescription*> *mapDcuIdChannel_;
vector<GenericFedDescription*> *genFed_;
TscXmlDescription* tscd_;
string* pedestalFile_;
vector<DetGeo*>  *module_;
vector<senderInstanceDescription*> *sender_;
XercesDOMParser* parser_;
bool debugOutput_;
public:
string* getPedestalFile(){return pedestalFile_;}
void setPedestalFile(string s) {pedestalFile_ =new string(s.c_str());}
TscXmlDescription* getTscXmlDescription(){return tscd_;}
void addTscXmlDescription(TscXmlDescription*t ) { tscd_=t;}

int getNumberOfModule(){ return module_->size();}
DetGeo* getDetGeo(int i){ return (*module_)[i];}
void addDetGeo(DetGeo* g){module_->push_back(g);}

int getNumberOfsenderInstance(){ return sender_->size();}
senderInstanceDescription* getsenderInstanceDescription(int i) {return (*sender_)[i];}

int getNumberOfGenericFed(){ return genFed_->size();}
GenericFedDescription* getGenericFedDescription(int i) {return (*genFed_)[i];}
void addGenericFedDescription(GenericFedDescription* g){ genFed_->push_back(g);}


int getNumberOfFedChannel(){ return fedChan_->size();}
FedChannelConnectionDescription* getFedChannelConnection(int i) { return (*fedChan_)[i];}
FedChannelConnectionDescription* getFedChannelConnectionByDcu(int dcu) { return (*mapDcuIdChannel_)[dcu];}
FedChannelConnectionDescription* getFedChannelConnection(int fedid,unsigned int fedch) {

// Iterators
multimap<int, FedChannelConnectionDescription*>::iterator iter;
multimap<int, FedChannelConnectionDescription*>::iterator lower;
multimap<int, FedChannelConnectionDescription*>::iterator upper;


lower = (*mapFedIdChannel_).lower_bound(fedid);
upper = (*mapFedIdChannel_).upper_bound(fedid);
for (iter = lower; iter != upper; iter++) {
FedChannelConnectionDescription* cur=iter->second;
if (cur->getFedChannel() == fedch) 	  return cur;
}

return NULL;

}


void addFedChannelConnection(FedChannelConnectionDescription* ch){
fedChan_->push_back(ch);
pair<int,FedChannelConnectionDescription*> pdcu(ch->getDcuHardId(),ch);
mapDcuIdChannel_->insert(pdcu);
pair<int,FedChannelConnectionDescription*> pfed(ch->getFedId(),ch);
mapFedIdChannel_->insert(pfed);

}

/// Parse Monitor



public:
TrackerParser()
{
fedChan_ = new vector<FedChannelConnectionDescription*>();
mapFedIdChannel_ = new multimap<int,FedChannelConnectionDescription*>();
mapDcuIdChannel_ = new map<int,FedChannelConnectionDescription*>();
genFed_ = new vector<GenericFedDescription*>();
module_ = new vector<DetGeo*>();
pedestalFile_=NULL;
tscd_=NULL;
sender_ = new vector<senderInstanceDescription*>();
parser_=NULL;
debugOutput_=false;
}


~TrackerParser()
{
if (tscd_!=NULL) delete tscd_;
for (unsigned int i=0;i<module_->size();i++) delete (*module_)[i];
delete module_;
for (unsigned int i=0;i<sender_->size();i++) delete (*sender_)[i];
delete sender_;
for (unsigned int i=0;i<genFed_->size();i++) delete (*genFed_)[i];
delete genFed_;
for (unsigned int i=0;i<fedChan_->size();i++) delete (*fedChan_)[i];
delete fedChan_;
delete mapFedIdChannel_;
delete mapDcuIdChannel_;
if (parser_!=NULL)
{
  delete parser_;
  XMLPlatformUtils::Terminate();
}
}

void clear()
{
module_->clear();
sender_->clear();
genFed_->clear();
fedChan_->clear();
mapFedIdChannel_->clear();
mapDcuIdChannel_->clear();
} 

void purge()
{
for (unsigned int i=0;i<module_->size();i++) delete (*module_)[i];
for (unsigned int i=0;i<sender_->size();i++) delete (*sender_)[i];
for (unsigned int i=0;i<genFed_->size();i++) delete (*genFed_)[i];
for (unsigned int i=0;i<fedChan_->size();i++) delete (*fedChan_)[i];
if (tscd_!=NULL)
{
  delete tscd_;
  tscd_=NULL;
}
this->clear();

}

string XMLCh2String (const XMLCh* ch)
{
if (ch == 0) return "";

auto_ptr<char> v(XMLString::transcode (ch));
return string(v.get());
}


/** Parse the file
@param name File Name
*/
void parseFile(string name)
{
// Clear previous setting

//  sender_->clear();
//       module_->clear();
//       if (tscd_!=NULL)
// 	{
// 	  delete tscd_;
// 	  tscd_=NULL;
// 	}
//       genFed_->clear();


if (parser_ == NULL)
{
  try
    {
      XMLPlatformUtils::Initialize();
      parser_ = new XercesDOMParser;

      std::cout << "parser_->setValidationScheme(XercesDOMParser::Val_Never) ;//Val_Auto);" << std::endl ;

      parser_->setValidationScheme(XercesDOMParser::Val_Auto);
      parser_->setDoNamespaces(false);
      parser_->setDoSchema(false);
      parser_->setValidationSchemaFullChecking(false);
      parser_->setCreateEntityReferenceNodes(false);
      //	      DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
      // parser_->setErrorHandler(errReporter);





    }
  catch(const XMLException& toCatch)
    {
      cerr << "*** Error during Xerces-c Initialization.\n"
	   << "  Exception message:"
	   << XMLCh2String( toCatch.getMessage() ) << endl;
      return;
    }

}
else
parser_->resetDocumentPool();

//
//  Parse the XML file, catching any XML exceptions that might propogate
//  out of it.
//
bool errorsOccured = false;

try
{
  
  parser_->parse((char*) name.c_str());
 
}

catch (const XMLException& e)
{
  cerr << "An error occured during parsing\n   Message: "
       <<XMLCh2String(e.getMessage()) << endl;
  errorsOccured = true;
}

catch (...)
{
  cerr << "An error occured during parsing\n " << endl;
  errorsOccured = true;
}
// If the parse was successful, output the document data from the DOM tree


DOMDocument* doc=parser_->getDocument();


if (!errorsOccured && doc!=NULL) {
parse(doc);
}


}



/** Parse the source
@param source InputSource
*/
void parseSource(InputSource &source)
{
  // Clear previous setting
  
  sender_->clear();
  module_->clear();
  if (tscd_!=NULL)
    {
      delete tscd_;
      tscd_=NULL;
    }
  genFed_->clear();

  
  if (parser_ == NULL)
    {
      try
	{
	  XMLPlatformUtils::Initialize();
	  parser_ = new XercesDOMParser;
	  
	  std::cout << "parser_->setValidationScheme(XercesDOMParser::Val_Never) ;//Val_Auto);" << std::endl ;

	  parser_->setValidationScheme(XercesDOMParser::Val_Auto);
	  parser_->setDoNamespaces(false);
	  parser_->setDoSchema(false);
	  parser_->setValidationSchemaFullChecking(false);
	  parser_->setCreateEntityReferenceNodes(false);
	  //	      DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
	  // parser_->setErrorHandler(errReporter);
	}
      catch(const XMLException& toCatch)
	{
	  cerr << "*** Error during Xerces-c Initialization.\n"
	       << "  Exception message:"
	       << XMLCh2String( toCatch.getMessage() ) << endl;
	  return;
	}
      
    }
  else
    parser_->resetDocumentPool();
  
  //
  //  Parse the XML file, catching any XML exceptions that might propogate
  //  out of it.
  //
  bool errorsOccured = false;

  try
    {
      
      parser_->parse(source);
 
    }

  catch (const XMLException& e)
    {
      cerr << "An error occured during parsing\n   Message: "
	   <<XMLCh2String(e.getMessage()) << endl;
      errorsOccured = true;
    }

  catch (...)
    {
      cerr << "An error occured during parsing\n " << endl;
      errorsOccured = true;
    }
  // If the parse was successful, output the document data from the DOM tree
  
  
  DOMDocument* doc=parser_->getDocument();

  
  if (!errorsOccured && doc!=NULL) {
    parse(doc);
  }


}


/// Parse the document
void parse(DOMDocument* doc)
{

DOMNode*  n1 = doc->getFirstChild();
if (n1!=NULL) 
 if (debugOutput_) cout << "doc first child = " << XMLCh2String(n1->getNodeName()) << endl;
else
 if (debugOutput_) cout << " Document had no childs " << endl;
while (n1!=NULL) {
 if (debugOutput_) cout << "next node = " << XMLCh2String(n1->getNodeName()) << endl;
  if (n1->getNodeType() == DOMNode::ELEMENT_NODE) break;
  n1 = n1->getNextSibling();
}

DOMNode*  n = n1->getFirstChild();
if (n!=NULL)
 if (debugOutput_) cout << "n1 first child = " << XMLCh2String(n->getNodeName()) << endl;


while(n!=NULL ) {
  // the node was really an element.
 if (debugOutput_) cout << "Tag = " << XMLCh2String(n->getNodeName()) << endl;
  if (XMLCh2String(n->getNodeName())=="Sender")
   SenderTagParser(n);
  if (XMLCh2String(n->getNodeName())=="GenericFed")
    GenericFedTagParser(n);

  if (XMLCh2String(n->getNodeName())=="Fed")
    FedTagParser(n);
  if (XMLCh2String(n->getNodeName())=="Tsc")
    TscTagParser(n);
  if (XMLCh2String(n->getNodeName())=="Monitor")
    MonitorTagParser(n);


  if (XMLCh2String(n->getNodeName())=="FedChannelList")
    FedChannelListTagParser(n);

    
  n = n->getNextSibling();
}



}


/// Parse Fed tag 
void FedTagParser(DOMNode* n)
{

if (debugOutput_) printf ("I will create a new Fed Description \n");
FedPmcXmlDescription* a= new FedPmcXmlDescription();
a->setMaxChannels(8);
// All attributes
DOMNamedNodeMap*  attributes = n->getAttributes();
//      int attrCount = attributes->getLength();
//Device
int device;
DOMNode*  e = attributes->getNamedItem(XMLString::transcode("device"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&device);
a->setDevice(device);
if (debugOutput_) printf("\t FedParser:: Device %d\n",a->getDevice());


// externalclock
bool externalclock;
e = attributes->getNamedItem(XMLString::transcode("externalclock"));
externalclock = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setExternalClock(externalclock);
if (debugOutput_) printf("\t FedParser:: ExternalClock %d\n",a->getExternalClock());
// externaltrigger
bool externaltrigger;
e = attributes->getNamedItem(XMLString::transcode("externaltrigger"));
externaltrigger = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setExternalTrigger(externaltrigger);
if (debugOutput_) printf("\t FedParser:: Externaltrigger %d\n",a->getExternalTrigger());
// ScopeMode
bool ScopeMode;
e = attributes->getNamedItem(XMLString::transcode("scopemode"));
ScopeMode = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setScopeMode(ScopeMode);
if (debugOutput_) printf("\t FedParser:: ScopeMode %d\n",a->getScopeMode());
  
//ClockDelay
int ClockDelay;
e = attributes->getNamedItem(XMLString::transcode("clockdelay"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ClockDelay);
a->setClockDelay(ClockDelay);
if (debugOutput_) printf("\t FedParser:: ClockDelay %d\n",a->getClockDelay());

//Sample
int Sample;
e = attributes->getNamedItem(XMLString::transcode("sample"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&Sample);
a->setSample(Sample);
if (debugOutput_) printf("\t FedParser:: Sample %d\n",a->getSample());
//LowThreshold
int LowThreshold;
e = attributes->getNamedItem(XMLString::transcode("lowthreshold"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&LowThreshold);
a->setLowThreshold(LowThreshold);
if (debugOutput_) printf("\t FedParser:: LowThreshold %d\n",a->getLowThreshold());
//HighThreshold
int HighThreshold;
e = attributes->getNamedItem(XMLString::transcode("highthreshold"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&HighThreshold);
a->setHighThreshold(HighThreshold);
if (debugOutput_) printf("\t FedParser:: HighThreshold %d\n",a->getHighThreshold());
// Soft Trigger Control
bool SoftTriggerControl;
e = attributes->getNamedItem(XMLString::transcode("softtriggercontrol"));
SoftTriggerControl = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setSoftTriggerControl(SoftTriggerControl);
if (debugOutput_) printf("\t FedParser:: SoftTriggerControl %d\n",a->getSoftTriggerControl());

// Ttc Trigger Control
bool TtcTriggerControl;
e = attributes->getNamedItem(XMLString::transcode("ttctriggercontrol"));
TtcTriggerControl = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setTtcTriggerControl(TtcTriggerControl);
if (debugOutput_) printf("\t FedParser:: TtcTriggerControl %d\n",a->getTtcTriggerControl());

// Tsc Trigger Control
bool TscTriggerControl;
e = attributes->getNamedItem(XMLString::transcode("tsctriggercontrol"));
TscTriggerControl = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setTscTriggerControl(TscTriggerControl);
if (debugOutput_) printf("\t FedParser:: TscTriggerControl %d\n",a->getTscTriggerControl());

// Tsc Inhibit Trigger Control
bool TscInhibitTriggerControl;
e = attributes->getNamedItem(XMLString::transcode("tscinhibittriggercontrol"));
TscInhibitTriggerControl = strcmp(XMLCh2String(e->getNodeValue()).c_str(),"true")==0;
a->setTscInhibitTriggerControl(TscInhibitTriggerControl);
if (debugOutput_) printf("\t FedParser:: TscInhibitTriggerControl %d\n",a->getTscInhibitTriggerControl());

int num;
e = attributes->getNamedItem(XMLString::transcode("instance"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&num);

a->setInstance(num);
char cmd[256];
e = attributes->getNamedItem(XMLString::transcode("supervisor"));
memset(cmd,0,256);
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
string scmd1(cmd);
a->setName(scmd1);

if (debugOutput_) printf("\t FedParser::%s %d\n",cmd,num);
DOMNode*  n2 =n->getFirstChild();
while (n2!=NULL)
{
  if (XMLCh2String(n2->getNodeName())=="FedChannel")
    {
      if (debugOutput_) printf("FedChannel found \n");
      FedChannelDescription *fch=new FedChannelDescription();
      DOMNamedNodeMap*  attributes = n2->getAttributes();
      //	      int attrCount = attributes->getLength();
      int fedch; char cmd[80];
      DOMNode*  e = attributes->getNamedItem(XMLString::transcode("channel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fedch);
      int fec,slot,ring,ccu,ch,apv1,apv2,optoch;
      e = attributes->getNamedItem(XMLString::transcode("fecInstance"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fec);
      e = attributes->getNamedItem(XMLString::transcode("fecSlot"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&slot);
      e = attributes->getNamedItem(XMLString::transcode("fecRing"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ring);
      e = attributes->getNamedItem(XMLString::transcode("ccuAddress"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ccu);
      e = attributes->getNamedItem(XMLString::transcode("ccuChannel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ch);
      e = attributes->getNamedItem(XMLString::transcode("apv1"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apv1);
      e = attributes->getNamedItem(XMLString::transcode("apv2"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apv2);
      e = attributes->getNamedItem(XMLString::transcode("opto"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&optoch);
      
      e = attributes->getNamedItem(XMLString::transcode("fec"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
      string scmd2(cmd);
      fch->setFecSupervisor(scmd2);
      fch->setInstance(fec);
      fch->setNumber(fedch);
      fch->setSlot(slot);
      fch->setRing(ring);
      fch->setCcu(ccu);
      fch->setI2c(ch);
      fch->setApv(apv1);
      fch->setOpto(optoch);
      a->addFedChannelDescription(fch);
    }
  n2 = n2->getNextSibling();        
}

genFed_->push_back(a);



}

/// Parse GenericFed tag 
void GenericFedTagParser(DOMNode* n)
{

if (debugOutput_) printf ("I will create a new Fed Description \n");
GenericFedDescription* a= new GenericFedDescription();

// All attributes
DOMNamedNodeMap*  attributes = n->getAttributes();
//      int attrCount = attributes->getLength();
int num;
DOMNode*  e = attributes->getNamedItem(XMLString::transcode("instance"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&num);

a->setInstance(num);

e = attributes->getNamedItem(XMLString::transcode("name"));
char cmd[256];
memset(cmd,0,256);
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
string scmd1(cmd);
a->setName(scmd1);

e = attributes->getNamedItem(XMLString::transcode("description"));
memset(cmd,0,256);
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
string scmd2(cmd);
a->setDescription(scmd2);



int mch;
e = attributes->getNamedItem(XMLString::transcode("channels"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&mch);

a->setMaxChannels(mch);

int fid;
e = attributes->getNamedItem(XMLString::transcode("fedid"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fid);

a->setFedId(fid);



if (debugOutput_) printf("\t GenericFedParser::%s %d\n",a->getName().c_str(),num);
DOMNode*  n2 =n->getFirstChild();
while (n2!=NULL)
{
  if (XMLCh2String(n2->getNodeName())=="FedChannel")
    {
      if (debugOutput_) printf("FedChannel found \n");
      FedChannelDescription *fch=new FedChannelDescription();
      DOMNamedNodeMap*  attributes = n2->getAttributes();
      //	      int attrCount = attributes->getLength();
      int fedch; char cmd[80];
      DOMNode*  e = attributes->getNamedItem(XMLString::transcode("channel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fedch);
      int fec,slot,ring,ccu,ch,apv1,apv2,optoch,delay;
      e = attributes->getNamedItem(XMLString::transcode("fecInstance"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fec);
      e = attributes->getNamedItem(XMLString::transcode("fecSlot"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&slot);
      e = attributes->getNamedItem(XMLString::transcode("fecRing"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ring);
      e = attributes->getNamedItem(XMLString::transcode("ccuAddress"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ccu);
      e = attributes->getNamedItem(XMLString::transcode("ccuChannel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ch);
      e = attributes->getNamedItem(XMLString::transcode("apv1"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apv1);
      e = attributes->getNamedItem(XMLString::transcode("apv2"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apv2);
      e = attributes->getNamedItem(XMLString::transcode("opto"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&optoch);
      e = attributes->getNamedItem(XMLString::transcode("fiberDelay"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&delay);
      
      
      e = attributes->getNamedItem(XMLString::transcode("fec"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
      string scmd2(cmd);
      fch->setFecSupervisor(scmd2);
      fch->setInstance(fec);
      fch->setNumber(fedch);
      fch->setSlot(slot);
      fch->setRing(ring);
      fch->setCcu(ccu);
      fch->setI2c(ch);
      fch->setApv(apv1);
      fch->setOpto(optoch);
      fch->setFiberDelay(delay);
      a->addFedChannelDescription(fch);
    }
  n2 = n2->getNextSibling();        
}

genFed_->push_back(a);



}




/// Parse FedChannelList tag
void FedChannelListTagParser(DOMNode* n)
{




DOMNode*  n2 =n->getFirstChild();
while (n2!=NULL)
{
  if (XMLCh2String(n2->getNodeName())=="FedChannelConnection")
    {
      if (debugOutput_) printf("FedChannel found \n");
      FedChannelConnectionDescription *fch=new FedChannelConnectionDescription();
      DOMNamedNodeMap*  attributes = n2->getAttributes();
      //	      int attrCount = attributes->getLength();
      int fedch,fedid; char cmd[80];
      int fec,slot,ring,ccu,ch,apv;
      int dcuid,detid,fiberlength,apvpairs;
      DOMNode*  e = attributes->getNamedItem(XMLString::transcode("fedChannel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fedch);
      e = attributes->getNamedItem(XMLString::transcode("fedId"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fedid);

      if (debugOutput_) printf("FED %d %d  \n",fedch,fedid);
      e = attributes->getNamedItem(XMLString::transcode("fecInstance"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fec);
      e = attributes->getNamedItem(XMLString::transcode("fecSlot"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&slot);
      e = attributes->getNamedItem(XMLString::transcode("fecRing"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ring);
      e = attributes->getNamedItem(XMLString::transcode("ccuAddress"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ccu);
      e = attributes->getNamedItem(XMLString::transcode("ccuChannel"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ch);
      e = attributes->getNamedItem(XMLString::transcode("apvAddress"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apv);

      e = attributes->getNamedItem(XMLString::transcode("dcuHardId"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&dcuid);
      e = attributes->getNamedItem(XMLString::transcode("detId"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&detid);
      e = attributes->getNamedItem(XMLString::transcode("fiberLength"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&fiberlength);
      e = attributes->getNamedItem(XMLString::transcode("apvPairs"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&apvpairs);

      if (debugOutput_) printf("I2C %d %d %d %d %d %d \n",fec,slot,ring,ccu,ch,apv);



      e = attributes->getNamedItem(XMLString::transcode("fecClassName"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);

      if (debugOutput_) printf("Sup %s\n",cmd);
      string scmd2(cmd);
      fch->setFecSupervisor(scmd2);
      e = attributes->getNamedItem(XMLString::transcode("fecSupervisorIp"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
      if (debugOutput_) printf("SupIP %s\n",cmd);
      string scmd3(cmd);
      fch->setFecSupervisorIP(scmd3);

      fch->setFecInstance(fec);
      fch->setFedChannel(fedch);
      fch->setFedId(fedid);
      fch->setSlot(slot);
      fch->setRing(ring);
      fch->setCcu(ccu);
      fch->setI2c(ch);
      fch->setApv(apv);
      fch->setDcuHardId(dcuid);
      fch->setDetId(detid);
      fch->setFiberLength(fiberlength);
      fch->setApvPairs(apvpairs);
      if (debugOutput_) printf("Adding channel \n");
      fedChan_->push_back(fch);
    }
  n2 = n2->getNextSibling();        
}





}





/// Parse Fed tag
void SenderTagParser(DOMNode* n)
{

if (debugOutput_) printf ("I will create a new Fed Description \n");
senderInstanceDescription* a= new senderInstanceDescription();

// All attributes
DOMNamedNodeMap*  attributes = n->getAttributes();
//      int attrCount = attributes->getLength();

int num;
DOMNode*  e = attributes->getNamedItem(XMLString::transcode("instance"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&num);

a->setInstance(num);
char cmd[256];
e = attributes->getNamedItem(XMLString::transcode("name"));
memset(cmd,0,256);
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",cmd);
string scmd1(cmd);
a->setName(scmd1);

sender_->push_back(a);



}



/// Parse Tsc
void TscTagParser(DOMNode* ei)
{
if (debugOutput_) printf("\t TscTagparser  \n");

TscXmlDescription* a= new TscXmlDescription();

// All attributes
DOMNamedNodeMap*  attributes = ei->getAttributes();
//      int attrCount = attributes->getLength();
//SeqsiDevice
int TscDevice;
DOMNode*  e = attributes->getNamedItem(XMLString::transcode("TscDevice"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&TscDevice);
a->setNumber(TscDevice);

char cmd[80];
memset(cmd,0,80);
sprintf(cmd,"tscdrv%.2d",TscDevice);
string scmd(cmd);
a->setDeviceName(scmd);

if (debugOutput_) printf("\t TscParser:: TscDevice %s\n",a->getDeviceName().c_str());
int ia;
e = attributes->getNamedItem(XMLString::transcode("resetlatency"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setResetLatency(ia);

e = attributes->getNamedItem(XMLString::transcode("clockmin"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setClockMin(ia);

e = attributes->getNamedItem(XMLString::transcode("triggercount"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setTriggerCount(ia);

e = attributes->getNamedItem(XMLString::transcode("triggerregister"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setTriggerRegister(ia);

e = attributes->getNamedItem(XMLString::transcode("triggerfrequency"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setTriggerFrequency(ia);

e = attributes->getNamedItem(XMLString::transcode("fedlatency"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setFedLatency(ia);

e = attributes->getNamedItem(XMLString::transcode("apvlatency"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setApvLatency(ia);

e = attributes->getNamedItem(XMLString::transcode("caliblatency"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setCalibrationLatency(ia);

e = attributes->getNamedItem(XMLString::transcode("feddelayclock"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setFedDelayClock(ia);

e = attributes->getNamedItem(XMLString::transcode("feddelaytrigger"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setFedDelayTrigger(ia);

e = attributes->getNamedItem(XMLString::transcode("gateposition"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setGatePositionDelay(ia);

e = attributes->getNamedItem(XMLString::transcode("gatewidth"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setGateWidthDelay(ia);

e = attributes->getNamedItem(XMLString::transcode("pulserdelay"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ia);
a->setPulserDelay(ia);

DOMNode*  n2 =ei->getFirstChild();
while (n2!=NULL)
{
  if (XMLCh2String(n2->getNodeName())=="TscRegister")
    {

      TscXmlRegister *fch=new TscXmlRegister();
      DOMNamedNodeMap*  attributes = n2->getAttributes();
      //	      int attrCount = attributes->getLength();
      int offset,value;
      DOMNode*  e = attributes->getNamedItem(XMLString::transcode("offset"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&offset);
      e = attributes->getNamedItem(XMLString::transcode("value"));
      sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&value);
      a->addTscXmlRegister(fch);
    }
  n2 = n2->getNextSibling();        
}
tscd_=a;

}

void MonitorTagParser(DOMNode* ei)
{
if (debugOutput_) printf("Monitor\n");
DOMNode*  n2 = ei->getFirstChild();
if (n2 == 0) if (debugOutput_) printf("problem\n");
while( n2!=NULL ) {
if (XMLCh2String(n2->getNodeName())=="Pedfile")
  {
    char pname[256];
    memset(pname,0,256);
    
    DOMNamedNodeMap*  attributes = n2->getAttributes();
    DOMNode*  e = attributes->getNamedItem(XMLString::transcode("Name"));
    sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%s",pname);
    pedestalFile_ = new string(pname);
   if (debugOutput_) cout << "---> I enter the FileNameParser " <<pname << endl;
  }

if (XMLCh2String(n2->getNodeName())=="Detector") 
  ModuleTagParser(n2);
if (XMLCh2String(n2->getNodeName())=="TBGeo") 
  DetGeoTagParser(n2);	

if (XMLCh2String(n2->getNodeName())=="DetGeo") 
  DetGeoTagParser(n2);	

n2 = n2->getNextSibling();

}
return;
}
void ModuleTagParser(DOMNode* ei) {
if (debugOutput_) cout << "---> I enter the ModuleTagParser" << endl;
DOMNamedNodeMap*  attributes = ei->getAttributes();
// float a;
int b;

int num,napv,ccu,ch,opto1,opto2,opto3;

DOMNode*  e = attributes->getNamedItem(XMLString::transcode("Number"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&num);
// cout << "Detector Number\t" << num << endl;


e = attributes->getNamedItem(XMLString::transcode("Number_of_apv"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&napv);
//cout << "Number_of_apv\t" << napv << endl;
int nc= napv/2;

int instance,ring,slot;
e = attributes->getNamedItem(XMLString::transcode("instance"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&instance);
//cout << "Ccu_number\t" << instance << endl;

e = attributes->getNamedItem(XMLString::transcode("ring"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ring);
//cout << "Ccu_number\t" << ring << endl;

e = attributes->getNamedItem(XMLString::transcode("slot"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&slot);
//cout << "Ccu_number\t" << slot << endl;

e = attributes->getNamedItem(XMLString::transcode("Ccu_number"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ccu);
// cout << "Ccu_number\t" << ccu << endl;


e = attributes->getNamedItem(XMLString::transcode("Ccu_channel"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ch);
//cout << "Ccu_channel\t" << ch << endl;

e = attributes->getNamedItem(XMLString::transcode("Opto1"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&opto1);
//cout << "opto1\t" <<opto1 << endl;

e = attributes->getNamedItem(XMLString::transcode("Opto2"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&opto2);
//cout << "opto2\t" << opto2 << endl;

e = attributes->getNamedItem(XMLString::transcode("Opto3"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&opto3);
//cout << "opto3\t" << opto3 << endl;

int pol;
e = attributes->getNamedItem(XMLString::transcode("Polarity"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&pol);
// cout << "Polarity\t" << pol << endl;
int ncm;
e = attributes->getNamedItem(XMLString::transcode("NCmn"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ncm);
// cout << "NCmn\t" << ncm << endl;

int ncs;
e = attributes->getNamedItem(XMLString::transcode("ChipSize"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&ncs);
//cout << "ChipSize\t" << ncs << endl;

DetGeo* g =  new DetGeo();

unsigned long key = g->BuildModuleKey(instance,slot,ring,ccu,ch);
if (debugOutput_) printf("key is %lx \n",key);
if (ncs != 280)
g->SetChipSize(ncs);
g->Set1st(1);
g->SetLast(nc*256);


g->SetNChips(nc);     
if (nc ==2)
{
int id1= g->BuildFedChannelKey(key,1,opto1);
int id3= g->BuildFedChannelKey(key,3,opto3);
g->SetIdAcq(0,id1);
g->SetIdAcq(1,id3);
}
else
{
int id1= g->BuildFedChannelKey(key,1,opto1);
int id2= g->BuildFedChannelKey(key,2,opto2);
int id3= g->BuildFedChannelKey(key,3,opto3);

g->SetIdAcq(0,id1);
g->SetIdAcq(1,id2);
g->SetIdAcq(2,id3);

}                


float seed,neigh,deadc;

e = attributes->getNamedItem(XMLString::transcode("Seed_cut"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&seed);
//cout << "Seed_cut\t" << seed << endl;


e = attributes->getNamedItem(XMLString::transcode("Neighbour_cut"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&neigh);
//cout << "Neighbour_cut\t" << neigh << endl;


e = attributes->getNamedItem(XMLString::transcode("Dead_strip_cut"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&deadc);
// cout << "Dead_strip_cut\t" << deadc << endl;
g->SetPolarity (pol);
g->SetPitch (0.0180);
g->SetDirection (1);
g->SetNcm (ncm);
g->SetCluSeed(seed);
g->SetCluCut(neigh);
g->SetHighCut(3.);

g->SetPedDead(0.1);
g->SetPedNoise(deadc);

g->SetNoiDead(0.1);
g->SetNoiNoise(deadc);

DOMNodeList* dead = ei->getChildNodes();

for ( unsigned int i=0; i<dead->getLength(); ++i ) {
DOMNode*  onedead = dead->item(i);
if (XMLCh2String(onedead->getNodeName())=="Dead_Strip" ) {
attributes = onedead->getAttributes(); 
e = attributes->getNamedItem(XMLString::transcode("Channel"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
if (debugOutput_) cout << "New dead strip found : " << b << endl;
g->SetDead(b);
}
}

module_->push_back(g);
return;
}


void DetGeoTagParser(DOMNode* ei) {
if (debugOutput_) cout << "---> I enter the DetGeoTagParser now" << endl;

float a;
int b,nch;

DetGeo* g =  new DetGeo();


DOMNamedNodeMap*  attributes = ei->getAttributes();

DOMNode*  e = attributes->getNamedItem(XMLString::transcode("FirstRun"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetFirstRun(b);

e = attributes->getNamedItem(XMLString::transcode("LastRun"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetLastRun(b);

e = attributes->getNamedItem(XMLString::transcode("NChips"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&nch);
g->SetNChips(nch);
g->Set1st(1);
g->SetLast(nch*256);
if (nch>= 1)
{
e = attributes->getNamedItem(XMLString::transcode("IdAcq1"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetIdAcq(0,b);

}
if (nch>=2)
{
e = attributes->getNamedItem(XMLString::transcode("IdAcq2"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetIdAcq(1,b);

}
if (nch==3)
{
e = attributes->getNamedItem(XMLString::transcode("IdAcq3"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetIdAcq(2,b);

}
if (debugOutput_) cout << " IdL "<<hex<< g->GetIdLocal() <<dec <<endl;

e = attributes->getNamedItem(XMLString::transcode("ChipSize"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetChipSize(b);
e = attributes->getNamedItem(XMLString::transcode("Polarity"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetPolarity(b);
e = attributes->getNamedItem(XMLString::transcode("Ncm"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetNcm(b);
e = attributes->getNamedItem(XMLString::transcode("Direction"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
g->SetDirection(b);

e = attributes->getNamedItem(XMLString::transcode("Pitch"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetPitch(a);
e = attributes->getNamedItem(XMLString::transcode("Tilt"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetTilt(a);
e = attributes->getNamedItem(XMLString::transcode("Dx"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetDx(a);
e = attributes->getNamedItem(XMLString::transcode("Dy"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetDy(a);
e = attributes->getNamedItem(XMLString::transcode("Dz"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetDz(a);

e = attributes->getNamedItem(XMLString::transcode("CluSeed"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetCluSeed(a);
e = attributes->getNamedItem(XMLString::transcode("CluCut"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetCluCut(a);
e = attributes->getNamedItem(XMLString::transcode("HighCut"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetHighCut(a);

e = attributes->getNamedItem(XMLString::transcode("PedDead"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetPedDead(a);
e = attributes->getNamedItem(XMLString::transcode("PedNoisy"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetPedNoise(a);
e = attributes->getNamedItem(XMLString::transcode("NoiDead"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetNoiDead(a);
e = attributes->getNamedItem(XMLString::transcode("NoiNoisy"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%f",&a);
g->SetNoiNoise(a);



DOMNodeList* dead = ei->getChildNodes();

for ( unsigned int i=0; i<dead->getLength(); ++i ) {
DOMNode*  onedead = dead->item(i);
if (XMLCh2String(onedead->getNodeName())=="Dead_Strip" ) {
attributes = onedead->getAttributes(); 
e = attributes->getNamedItem(XMLString::transcode("Channel"));
sscanf(XMLCh2String(e->getNodeValue()).c_str(),"%d",&b);
if (debugOutput_) cout << "New dead strip found : " << b << endl;
g->SetDead(b);
}
}

module_->push_back(g);
return;
}

static void splitInVector(std::string ENV,vector<string> &v)
{
 int idx1=0,idx2;
 size_t pos;
  do 
    {
      pos = ENV.find(":",idx1);
      if (pos!=string::npos)
        {
          idx2= pos-1;
        }
      else
        idx2= ENV.length();
      //cout << "adding " << ENV.substr(idx1,idx2-idx1+1) <<endl;
      v.push_back(ENV.substr(idx1,idx2-idx1+1));
      idx1 =pos+1;
    } while (pos!=string::npos);

}




static void getDbConfiguration(string &name,string &passwd,string &path)
{
char* cpath=getenv("CONFDB");
if (cpath == NULL) return;
string confdb(cpath);
int ipass = confdb.find("/");
int ipath = confdb.find("@");
name.clear();
name=confdb.substr(0,ipass); 
passwd.clear();
passwd=confdb.substr(ipass+1,ipath-ipass-1); 
path.clear();
path=confdb.substr(ipath+1,confdb.size()-ipath); 


}


void buildModuleList(float low_noise=0.1,float high_noise=100.)
{
module_->clear();
unsigned char used[64000];
memset(used,0,64000*sizeof(unsigned char));

      for (unsigned int i=0;i<fedChan_->size();i++)
	{
	  if (used[i]!=0) continue;
	  used[i]=1;
	  int napv = 4;

	  if ((*fedChan_)[i]->getApv() == 34 ) napv=6;


	  for (unsigned int j=i+1;j<fedChan_->size();j++)
	    {
	      if (used[j]!=0) continue;
	      if ((*fedChan_)[i]->getFecInstance() != (*fedChan_)[j]->getFecInstance()) continue;
	      if ((*fedChan_)[i]->getSlot() != (*fedChan_)[j]->getSlot()) continue;
	      if ((*fedChan_)[i]->getRing() != (*fedChan_)[j]->getRing()) continue;
	      if ((*fedChan_)[i]->getCcu() != (*fedChan_)[j]->getCcu()) continue;
	      if ((*fedChan_)[i]->getI2c() != (*fedChan_)[j]->getI2c()) continue;

	      if ((*fedChan_)[j]->getApv() == 34 ) napv=6;
	      used[j]=1;
	    }
	  
	  // Build the module
	  DetGeo* g =  new DetGeo();
	  
	  unsigned long key = DetGeo::BuildModuleKey((*fedChan_)[i]->getFecInstance(),
						     (*fedChan_)[i]->getSlot(),
						     (*fedChan_)[i]->getRing(),
						     (*fedChan_)[i]->getCcu(),
						     (*fedChan_)[i]->getI2c());
//	  if (debugOutput_) printf("The new key is %d from %d %d %d %x %x  \n",key,
//		 (*fedChan_)[i]->getFecInstance(),
//		 (*fedChan_)[i]->getSlot(),
//		 (*fedChan_)[i]->getRing(),
//		 (*fedChan_)[i]->getCcu(),
//		 (*fedChan_)[i]->getI2c());
	  
	  g->SetChipSize(280);
	  g->Set1st(1);
	  g->SetLast(napv*128);
	  g->SetType(1);
	  g->SetTilt(0);
	  g->SetDx(0);
	  g->SetDy(0);
	  g->SetDz(0);
	  
	  g->SetNChips(napv/2);     
	  if (napv ==4)
	    {
	      int id1= DetGeo::BuildFedChannelKey(key,1,1);
	      int id3= DetGeo::BuildFedChannelKey(key,3,3);
	      g->SetIdAcq(0,id1);
	      g->SetIdAcq(1,id3);
	      g->SetIdAcq(2,0);
	      if (debugOutput_) printf("key is %lx %x %x \n",key,id1,id3);
	    }
	  else
	    {
	      int id1= DetGeo::BuildFedChannelKey(key,1,1);
	      int id2= DetGeo::BuildFedChannelKey(key,2,2);
	      int id3= DetGeo::BuildFedChannelKey(key,3,3);
	      
	      g->SetIdAcq(0,id1);
	      g->SetIdAcq(1,id2);
	      g->SetIdAcq(2,id3);
	      
	    }                
	  
	  
	  
	  
	  g->SetPolarity (1);
	  g->SetPitch (0.0180);
	  g->SetDirection (1);
	  g->SetNcm (128);
	  g->SetCluSeed(4.);
	  g->SetCluCut(2.);
	  g->SetHighCut(3.);
	  
	  g->SetPedDead(0.1);
	  g->SetPedNoise(100.);

	  g->SetNoiDead(0.1);
	  g->SetNoiNoise(100.);
	  g->SetDcuId(0); // Set it to 0 to keep local id as tag in TBMonitor (LM 19/07/07)
	  addDetGeo(g);



	}

   if (debugOutput_) cout << "Number Of Module is "<< this->getNumberOfModule() <<endl;
   }

 friend ostream & operator << (ostream &  os, const TrackerParser &t)
   {

     TrackerParser* tp = (TrackerParser*) &t;
     // File creation
     os <<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<endl;
     os <<"<!DOCTYPE Tracker SYSTEM \"http://cmsdoc.cern.ch/cms/cmt/System_aspects/Daq/dtd/trackerdescription.dtd\">" <<endl;
      os <<"<TrackerDescription>" << endl;

      // Loop on FEDs
      for (int ifed=0;ifed<tp->getNumberOfGenericFed();ifed++)
	{
	  GenericFedDescription* fed= tp->getGenericFedDescription(ifed);
	  if (fed->getName() == "FedPmcSupervisor") continue;
	  fed->toXML(os); // GenericFed

	}


      // TSC
      if (tp->getTscXmlDescription()!=NULL)
	tp->getTscXmlDescription()->toXML(os);


      // FedChannelList
      // printf("I found %d channel \n",tp->getNumberOfFedChannel());
      os << "<FedChannelList>"<<endl;
      for (int ich=0;ich<tp->getNumberOfFedChannel();ich++)
	{
	  FedChannelConnectionDescription* fedch= tp->getFedChannelConnection(ich);
	  printf("Address of Fed Channel %x \n",(long) fedch);
	  fedch->toXML(os);
	}
      os << "</FedChannelList>"<<endl;





      // Modules

      os << "<Monitor>"<<endl;
      if (tp->getPedestalFile()!=NULL)
      os << "<Pedfile Name=\"" << tp->getPedestalFile() <<"\"/>" << endl;

      for (int i=0;i<tp->getNumberOfModule();i++)
	{
	  tp->getDetGeo(i)->toXML(os);
	}
      
      os <<"</Monitor>" << endl;
      os <<"</TrackerDescription>" << endl;
     
      return os;
   }

  /** Sort the connection vector by FEC slot, ring slot, CCU address, i2c channel and i2c address
   */
  static bool sortByFec ( FedChannelConnectionDescription *r1, FedChannelConnectionDescription *r2 ) {

    if (r1->getSlot() < r2->getSlot()) return true ;
    else if (r1->getSlot() == r2->getSlot()) {
      if (r1->getRing() < r2->getRing()) return true ;
      else if (r1->getRing() == r2->getRing()) {
	if (r1->getCcu() < r2->getCcu()) return true ;
	else if (r1->getCcu() == r2->getCcu()) {
	  if (r1->getI2c() < r2->getI2c()) return true ;
	  else if (r1->getI2c() == r2->getI2c()) {
	    if (r1->getApv() < r2->getApv()) return true ;
	    else if (r1->getApv() == r2->getApv()) {
	      return true ;
	    }
	    else return false ;
	  }
	  else return false ;
	}
	else return false ;
      }
      else return false ;
    }
    else return false ;

  }
};

#endif


