#ifndef H_Fed9UGuiAbcMap
#define H_Fed9UGuiAbcMap


#include "TypeDefs.hh"
#include <vector>

/////////////////////////////////////////////////////////////////
//                                                             //
//   Fed 9U Abstract Base Class   : Fed9UABC                   //
//                                                             //
//  Author: Matt Noy, Jonathan Fulcher                IC       //
//  date: 14/05/2004                                           //
//  notes:                                                     //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////

namespace Fed9U {
  //using.*std::vector;
  class Fed9UABC;

  /**
   * \brief  Map class to map from an index to an array of abc pointers.
   * \author Matt Noy, Jonathan Fulcher, IC
   * \date   10/05/2004
   *
   * Each vector of Fed9UABC pointers is stored in it own element in the container. Entries can be added to the container
   * or to the vectors stored in the container.
   */
  class Fed9UGuiAbcMap {

  public:

    /**
     * \brief Constructor.
     */
    Fed9UGuiAbcMap();

    /**
     * \brief Destructor.
     *
     * Removes all entries from the array.
     */
    ~Fed9UGuiAbcMap() ;

    /**
     * \brief Adds an empty mapping of Fed9UABC pointers to the end of the container.
     */
    void addEntry() ;

    /**
     * \brief Removes a specific mapping of Fed9UABC pointers from the container.
     * \param index Index of the Fed9UABC pointer vector in the container to be deleted.
     */
    void deleteEntry(u32 index) ;

    /**
     * \brief Adds a pointer to a Fed9UABC class to the end of a mapping stored in the container.
     * \param index Index of the vector to be accessed within the container.
     * \param ptr Pointer to the Fed9UABC class to be added to the vector.
     */
    void addMapping( u32 index, Fed9UABC* ptr ) ;

    /**
     * \brief Deletes the Fed9UABC pointer given from the mapping stored in the container.
     * \param index Index of the vector that is to be accessed from the container.
     * \param ptr Pointer that is to be erased from the mapping.
     */
    void deleteMapping( u32 index, Fed9UABC* ptr ) ;

    /**
     * \brief Removes all data from the container.
     */
    void clear() ;

    /**
     * \brief  Returns a mapping that is stored in the container.
     * \param  index Index of the mapping that is to be returned from the container.
     * \return vector<Fed9UABC*> Reference to the mapping that is being returned from the container.
     */
    std::vector<Fed9UABC*>& getMap(u32 index) ;

    /**
     * \brief Returns the number of mappings that are stored within the container.
     * \return unsigned long.
     */
    u32 size() { return fedAbcMap.size(); }

  private:

    std::vector< std::vector <Fed9UABC*> > fedAbcMap; //!< Container to hold the mapping.

  };
}
#endif // H_Fed9UGuiAbcMap
