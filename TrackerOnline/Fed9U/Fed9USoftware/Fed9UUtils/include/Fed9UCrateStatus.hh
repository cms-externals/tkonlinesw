#ifndef Fed9UCrateStatus_HH
#define Fed9UCrateStatus_HH

#include <vector>
#include <iostream>
#include <iomanip>

#include "Fed9UEntry.hh"

#define UNSET_SN 0


#ifdef TBHARDWARE
#include "Fed9UABC.hh"
#endif

namespace Fed9U
{
//
// 18-05-2004
// public member prependEntry() added, to prepend a Fed9UEntry;
//

  /**
   * \brief  Fed9UCrateStatus: container class for the Fed9UEntry
   * \author Matt Noy
   * \date   04/2004
   *
   * Abstraction of a vector that should provide the ability to detect the crate, and iterate the testing sequence over the crate entries.
   */
  class Fed9UCrateStatus
  {
  public:

    /**
     * \brief Default constructor.
     *
     * Initialises the class to contain an empty set of Fed9UEntry's.
     */
    Fed9UCrateStatus();

    /**
     * \brief Destructor.
     *
     * Purges the FED entries.
     */
    ~Fed9UCrateStatus();
    

    /**
     * \brief  Each FED entry is stored in a vector element.
     * \param  pos Gives the position of the FED entry to be accessed.
     * \return Fed9UEntry Reference to the Fed9UEntry object stored in the given position.
     */
    Fed9UEntry & getFed9UEntry(unsigned long pos);

    /**
     * \brief  Allows a User to get the next Fed9UEntry in the list.
     * \param  f Reference to a Fed9UEntry object that the next entry is to be placed.
     * \return bool Boolean true if there is an entry to return, false if there is not.
     *         The reference to the Fed9UEntry will only be valid if this returns true.
     *
     * Fed9UEntry objects that have their test flag enabled will be skipped. See the
     * Fed9UEntry documentation for details on the test flag.
     */
    bool getNextFed9UEntry(Fed9UEntry & f);
    
    /**
     * \brief  Returns the number of Fed9UEntry objects in the sequence.
     * \return unsigned long Number of entries stored in the class.
     */
    inline unsigned long getNumberOfEntries(){return mFeds.size();}
   
    /**
     * \brief  Used to determine if the class contains any FED entries.
     * \return bool Boolean true if class is empty, false otherwise.
     */
    inline bool isEmpty(){return mFeds.empty();}
    
    /**
     * \brief Places the Fed9UEntry at the end of the sequence.
     * \param f Fed9UEntry to be placed at the end of the sequence.
     *
     * This function resets the internal iterator afterwards.
     */
    void appendFed9UEntry(Fed9UEntry f);

    /**
     * \brief Places the Fed9UEntry at the start of the sequence.
     * \param f Fed9UEntry to be placed at the end of the sequence.
     */
    void prependFed9UEntry(Fed9UEntry f);

    /**
     * \brief Resets the internal iterator to the start of the sequence
     */
    void resetIterator();

    /**
     * \brief Purges the contents of the object
     */
    void purge();

    /**
     * \brief calls Fed9UEntry::display on each element
     *
     * See Fed9UEntry documentation for more details on Fed9UEntry::display.
     */
    void display() const;
   
    /**
     * \brief  checks that each serial number is unique.
     * \return bool Returns true if any of the serial numbers duplicates, false if not.
     *
     * If an entry is unset then this will considered to be unique, even if there are multiple unset serial numbers.
     * areUnsetSerialNumbers should be consulted to check if this is the case.
     */
    bool areDuplicateSerialNumbers();

    /**
     * \brief  Checks if any feds in the crate have the serial number unset.
     * \return bool Boolean true if the serial numbers any serial numbers are unset.
     */
    bool areUnsetSerialNumbers();

   
  protected:

    /**
     * \brief Initialises the slot map.
     *
     * Slot map is defined by this class, so it can be modified if necessary and is used for accessing FED entries.
     */
    void init();

  protected:
    
    std::vector<Fed9UEntry> mFeds;                    //!< Holds each Fed9UEntry.
    std::vector<Fed9UEntry>::const_iterator mFedIter; //!< Used to iterate through the Fed9UEntry's in mFeds.

    static std::vector<unsigned long> mSlotBAMap;     //!< Map of the slot base addresses in a crate.

  }; // class Fed9UCrateStatus

} // namespace Fed9U

#endif // Fed9UCrateStatus_HH
