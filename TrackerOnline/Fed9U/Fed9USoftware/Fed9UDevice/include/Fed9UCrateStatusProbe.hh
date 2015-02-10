#ifndef Fed9UCrateStatusProbe_HH
#define Fed9UCrateStatusProbe_HH

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Fed9UCrateStatus.hh"
#include "Fed9UDescription.hh"

#define UNSET_SN 0

#ifdef TBHARDWARE
#include "Fed9UABC.hh"
#endif

//class std::ostringstream;
  
namespace Fed9U
{

  /** 
   * \brief Fed9UCrateStatus: container class for the Fed9UEntry
   * \author Jonathan Fulcher
   * \author Matt Noy
   * \version 0
   * \date 1 April 2004
   *
   * Fed9UCrateStatus: container class for the Fed9UEntry
   *
   *abstraction of a vector that should
   *provide the ability to detect the crate,
   *and iterate the testing sequence over 
   *the crate entries.
   *
   *
   *
   *18-05-2004
   *public member prependEntry() added, to prepend a Fed9UEntry;
   **/
  class Fed9UCrateStatusProbe : public Fed9UCrateStatus
  {
    
  public:

    /**
     * \brief Default constructor.
     */
    //Fed9UCrateStatusProbe();

    /**
     * \brief Destructor
     */
    ~Fed9UCrateStatusProbe();

    /**
     * \brief Probes the crate for FEDs adding a Fed9UEntry to the sequence for each one found.
     * \param pDesc Pointer to the Fed9UDescription object used to construct Fed9UDevice objects to communicate with the FEDs.
     *        Should have the hardware ID set to 0 or 998 to run the scan. See Fed9UDescription::setHardwareId for more details. Default null.
     * \param start First slot number to be probed. Default 1.
     * \param end Last slot number to be probed. Default 21.
     * \param KeepFeds If boolean true places a valid pointer to the Fed9UDevice into the FedEvent (which then assumes ownership).
     *        If boolean false FED objects are deteleted once crate probe is complete. Default false.
     * \param KeepDescriptions If boolean true the description pointed to by pDesc is left for calling function to delete. If boolean false
     *        it will be deleted at the end of probe crate and the pointer assigned to NULL. Default false
     * \param showTemps If boolean true then the temperatures will be read back from the FED and values displayed if the temperatures are out of range. Default false.
     * \param useThreads Each slot can be probed in its own thread if threading is supported on the system. Default true.
     */
    void probeCrate(Fed9UDescription * pDesc = NULL, unsigned long start=1, unsigned long end=21, bool KeepFeds=false, bool KeepDescriptions=false, bool showTemps=true, bool useThreads=false, bool adcsOff=true );

  private:
    /**
     * \brief  Probes a specific slot in a crate to determine if a FED is present.
     * \param  param A void pointer to the parameter list. The parameters it expects:
     *         \li 0: Pointer to the description being used.
     *         \li 1: Pointer to an area of memory where the FED object will be stored, should be NULL as it will deleted if not.
     *                It will deleted if parameter three is set false, once the member function is complete.
     *         \li 2: Slot number that is to be probed.
     *         \li 3: If boolean true any FED object created is kept and can be accessed through pointer given in parameter 1. 
     *                If boolean false FED objects are deleted once crate probe is complete and pointer given in parameter 1 is set to NULL.
     *         \li 4: If boolean true then the temperatures will be read back from the FED and values displayed if the temperatures are out of range.
     *         \li 5: Counter that is incremented each time a new instance of this static member function is created and decremeneted when it finishes.
     *         \li 6: A pointer to a boolean that if true means that all the threads have finished their initialisation routine and it is safe
     *                to continue with the thread.
     *         \li 7: Pointer to an ostringstream that all the thread output is written to. As cout is not thread safe the main thread must write to it.
     * \return void* Returns null on all returns from the program. It has no means, it is required to be a void* for thread creation.
     *
     * This member function has been written in mind for being spwaned in multiple threads, allow each slot in a crate to be probed almost
     * simulataneously.
     */
    static void* probeSlot(void* param);

    static u32 mRunningProbe; //!< Used to keep track of the number of slots being probed.

 }; // class Fed9UCrateStatus

} // namespace Fed9U

#endif // Fed9UCrateStatusProbe_HH
