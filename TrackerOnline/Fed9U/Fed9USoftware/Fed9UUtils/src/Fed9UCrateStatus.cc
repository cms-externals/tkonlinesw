#include "Fed9UCrateStatus.hh"
#ifdef TBHARDWARE
#include "Fed9UEntry.hh"
#endif


namespace Fed9U
{
  using namespace std;

  Fed9UCrateStatus::Fed9UCrateStatus():
    mFeds(),
    mFedIter(mFeds.begin())
  {
    // std::cout<<"Fed9UCrateStatus::Fed9UCrateStatus() "<<std::endl;
    init();
  }

  Fed9UCrateStatus::~Fed9UCrateStatus()
  {
    // std::cout<<"Fed9UCrateStatus::~Fed9UCrateStatus() Start"<<std::endl;
    purge();
    // std::cout<<"Fed9UCrateStatus::~Fed9UCrateStatus() End"<<std::endl;
  }
 
  
  bool Fed9UCrateStatus::getNextFed9UEntry(Fed9UEntry & f)
  {
    if(mFedIter!=mFeds.end()){
      while(!mFedIter->getTestFlag()){      
	mFedIter++;
	// std::cout<<"DBG: Fed9UCrateStatus::getNextFed9UEntry(Fed9UEntry & f): looping over the container"<<std::endl;
	if(mFedIter==mFeds.end()){
	  // std::cout<<"DBG: Fed9UCrateStatus::getNextFed9UEntry(Fed9UEntry & f): reached the end, returning false"<<std::endl;
	  return false;
	}
	
      }
      f=(*mFedIter);
      mFedIter++;
      return true;
    }
    return false;
  }


  void Fed9UCrateStatus::appendFed9UEntry(Fed9UEntry f)
  {
    //cout << "appending Fed entry" << endl;
    mFeds.push_back(f);
    //cout << "appended entry." << endl;
    resetIterator();
    //cout << "rested iterator" << endl;
  }
  

  void Fed9UCrateStatus::prependFed9UEntry(Fed9UEntry f)
  {
    mFeds.insert(mFeds.begin(), f);
    resetIterator();
  }
  

  void Fed9UCrateStatus::resetIterator()
  {
    mFedIter=mFeds.begin();
  }


  void Fed9UCrateStatus::display() const {
    
    for(std::vector<Fed9UEntry>::const_iterator i=mFeds.begin(); i!=mFeds.end(); i++)
      i->display();

  }

 
  bool Fed9UCrateStatus::areDuplicateSerialNumbers()
  {
    if(mFeds.size()<2) // 0 or 1 elements -> no duplicates
      return false;
    
    for(std::vector<Fed9UEntry>::const_iterator i=mFeds.begin(); i!=(mFeds.end()-1); i++)
      {
	for(std::vector<Fed9UEntry>::const_iterator j=i+1; j!=mFeds.end(); j++)
	  {
	    if(j==i)
	      continue;
	    if(i->getSerialNumber() == j->getSerialNumber() && j->getSerialNumber()!=UNSET_SN)
	      return true;
	  }
      }
    return false;
  }


  bool Fed9UCrateStatus::areUnsetSerialNumbers()
  {
     for(std::vector<Fed9UEntry>::const_iterator i=mFeds.begin(); i!=mFeds.end(); i++)
      {
	if(i->getSerialNumber()==UNSET_SN)
	  return true;
      }
     return false;
  }
  

  Fed9UEntry & Fed9UCrateStatus::getFed9UEntry(unsigned long pos)
  {
    if(pos>=mFeds.size()){
      std::cerr<<"getFed9UEntry(unsigned long) passed out of range index."<<pos<<std::endl;
      // throw... // when in the system.
      
    }
    return mFeds[pos];
  }

  
  void Fed9UCrateStatus::purge()
  {
    //
    // delete feds, clear contents and ensure that the iterator is sensible.
    //
    vector<Fed9UEntry>::iterator i;
    for(i = mFeds.begin() ; i != mFeds.end() ; ++i) {
      (*i).purge();
    }
    mFeds.clear();
    mFedIter=mFeds.begin();
  }


  void Fed9UCrateStatus::init()
  {
    //
    // populate this map like so, in case the way the base addresses are 
    // generated is changed.
    //
    mSlotBAMap.resize(22);
    unsigned long ba=0;
    for(std::vector<unsigned long>::iterator i=mSlotBAMap.begin(); 
	i!=mSlotBAMap.end(); i++, ba+=0x10000)
      {
	*i=ba;
	// std::cout<<std::hex<<"Base address = "<<ba<<std::endl;
      }
    // std::cout<<std::dec;
  }


  std::vector<unsigned long> Fed9UCrateStatus::mSlotBAMap;

} // namespace Fed9UTB
