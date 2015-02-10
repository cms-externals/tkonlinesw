#include "Fed9UGuiAbcMap.hh"
#include "ICExImpl.hh"
namespace Fed9U {
  
  Fed9UGuiAbcMap::Fed9UGuiAbcMap(){}
  
  Fed9UGuiAbcMap::~Fed9UGuiAbcMap() {
    clear();
  }
  
  void Fed9UGuiAbcMap::addEntry() {
    std::vector <Fed9UABC*> tmp;
    fedAbcMap.push_back( tmp );
  }
  
  void Fed9UGuiAbcMap::deleteEntry(u32 index) {
    ICUTILS_VERIFY(index<fedAbcMap.size());

    std::vector< std::vector <Fed9UABC*> >::iterator i=fedAbcMap.begin();
    u32 ind=0;
    while(ind<index){
      ind++;
      i++;
    }
    fedAbcMap.erase(i);
  }
  
  void Fed9UGuiAbcMap::addMapping( u32 index, Fed9UABC* ptr ) {
    fedAbcMap[index].push_back( ptr );
  }
  
  void Fed9UGuiAbcMap::deleteMapping( u32 index, Fed9UABC* ptr ) {
    std::vector<Fed9UABC*>::iterator i;
    for (i = fedAbcMap[index].begin(); i!=fedAbcMap[index].end(); ++i) {
      if ( ( *i ) == ptr) {
        fedAbcMap[index].erase(i);
        break;
      }
    }
  }
  
  void Fed9UGuiAbcMap::clear() {
    fedAbcMap.clear();
  }

  std::vector<Fed9UABC*>& Fed9UGuiAbcMap::getMap(u32 index) {
    return fedAbcMap[index];
  }

}
