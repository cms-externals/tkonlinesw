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
void Fed9UDebug::init()
{
    fed9UDescriptionInterface->setAbcMap(&fedAbcMap);
}

void Fed9UDebug::destroy()
{
}

void Fed9UDebug::fileNew()
{
    int lastElement = fedAbcMap.size();
    try {
 fedAbcMap.addEntry();
 fedAbcMap.addMapping(lastElement, reinterpret_cast<Fed9U::Fed9UABC*>(new Fed9U::Fed9UDescription));
    fedAbcMap.getMap(lastElement)[0]->loadDefaultDescription();
 fed9UDescriptionInterface->setCurrentFedAbc(lastElement);
 activeFedAbc->insertItem(QString("untitiled[") + QString("").setNum(lastElement).append("]"));
 activeFedAbc->setCurrentItem(lastElement);
    } catch ( exception & e ) {
 QMessageBox::information(this, "New file error : ", e.what(), 1);
    }     
    fed9UDescriptionInterface->setEnableTabs(true);
    fed9UDescriptionInterface->displayDescription();
}
/*
void Fed9UDebug::fileOpen()
{
    fileNew();
    QString s =  QFileDialog::getOpenFileName("","Fed Description Files (*.xml *.fed)",this,"open file dialog","Choose a file" );
    if(s=="")
  return;
    try{
 if(s.endsWith(".fed")) {
     std::ifstream input(s.ascii());
     if(!input.is_open())
  return;
     filename=s;
     DESC_PTR->loadDescription(static_cast<istream &>(input));
 }
 else if(s.endsWith(".xml")) {
     DESC_PTR->loadDefaultDescription();
     Fed9U::Fed9UXMLDescription xDesc(s.ascii(),*DESC_PTR);
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

void Fed9UDebug::saveXMLFile()
{ 
    try{
 Fed9U::Fed9UDescriptionToXml pob(string(filename.ascii()),*DESC_PTR);
 pob.writeXmlFile();
    }catch(exception & exc){
 QMessageBox::information(this, "Save Error : "+filename, exc.what(), 1);         
    }
}

void Fed9UDebug::saveFedFile()
{
    try{
 ofstream fileo(filename);
 ICUTILS_ASSERT( fileo.is_open() ).msg("cannot open file for writing");
 DESC_PTR->saveDescription(fileo);
    }catch(exception & exc){
 QMessageBox::information(this, "Save Error : "+filename, exc.what(), 1);         
    }
}

void Fed9UDebug::fileSave()
{
    if(DESC_PTR!=NULL)
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

void Fed9UDebug::fileSaveAs()
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
*/
void Fed9UDebug::filePrint()
{
}

void Fed9UDebug::fileExit()
{
    exit(0);
}


void Fed9UDebug::activeFedAbc_activated( int update )
{
    fed9UDescriptionInterface->setCurrentFedAbc(update);
    cout << " fed map at 0 " << fedAbcMap.getMap(0)[0] << endl;
    fed9UDescriptionInterface->displayDescription();
}


void Fed9UDebug::detectFeds_clicked()
{
    // first we clear all lists and maps
    crateStatus.purge();
    crateStatus.probeCrate(0,1,21,true,false);
    Fed9U::Fed9UEntry fe;
    int lastElement;
    //TODO do not add mappings for those that exist already.
    while(crateStatus.getNextFed9UEntry(fe)) {
 //for (fedIter=fedAbcMap.getMap().begin() ; 
 //fedAbcMap.getMap()
 /*if () {*/
  lastElement = fedAbcMap.size();
  fedAbcMap.addEntry();
  cout << "added map entry, size is now " << fedAbcMap.size() << endl;
  cout << " adding abc ptr " << fe.getFed9UABC() << endl;
  fedAbcMap.addMapping(lastElement, fe.getFed9UABC());
                 cout  << "last element = " << lastElement << " fed map at 0 " << fedAbcMap.getMap(0)[0] << endl;
                 activeFedAbc->insertItem(QString("Slot ") + QString("").setNum(fe.getSlotNumber()));
                 availableFeds->insertItem(QString("Slot") + QString("").setNum(fe.getSlotNumber()));
    
      }
    //} 
    if(fedAbcMap.size() && fed9UDescriptionInterface->getCurrentFedAbc() == -1) {
 fed9UDescriptionInterface->setCurrentFedAbc(0);
 fed9UDescriptionInterface->setEnableTabs(true);
 fed9UDescriptionInterface->displayDescription();
    }
}


void Fed9UDebug::addFed_clicked()
{

}


void Fed9UDebug::removeFed_clicked()
{

}
