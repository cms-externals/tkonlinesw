#ifndef Fed9UEntry_HH
#define Fed9UEntry_HH

#include <vector>
#include <iostream>
#include <iomanip>

#define UNSET_SN 0
#define TBHARDWARE
#include "TypeDefs.hh"
#ifdef TBHARDWARE
#include "Fed9UABC.hh"
#endif

namespace Fed9U
{

  /**
   * \brief  Fed9UEntry: class to represent and identify a FED in the crate, imple container for the serial number and base address.
   * \author Matt Noy, IC
   * \date:  04/2004
   *
   * 18-05-2004: Fed9UABC pointer added to enable the container to keep the pointers to the Fed9UABC representing real hardware.
   */
  class Fed9UEntry
  {
  public:

    /**
     * \brief Default constructor.
     *
     * Creates an entry with an UNSET serial number, zero base addres, slot number and sets the test me flag to true. If the variable TBHARDWARE
     * is defined then the FED pointer is set to null.
     */
    Fed9UEntry():
      mSerialNo(UNSET_SN),
      mBA(0),
      mSlotNumber(0),
      mTestMe(true)
#ifdef TBHARDWARE
	      ,
      mpFed(NULL)
#endif
    {}

    /**
     * \brief Constructor.
     * \param SN Serial number of the entry.
     * \param BA Base address of the FED entry.
     * \param slot Slot number of the entry.
     * \param f Pointer to a Fed9UABC, which will be used for the entry. Only present if TBHARDWARE is defined.
     *
     * The test me setting is also enabled.
     */
    Fed9UEntry(unsigned long SN, unsigned long BA, unsigned long slot
#ifdef TBHARDWARE 
, Fed9U::Fed9UABC * f=NULL):
#else 
      ):
#endif
      mSerialNo(SN),
      mBA(BA),
      mSlotNumber(slot),
      mTestMe(true)
#ifdef TBHARDWARE
    ,
      mpFed(f)
#endif
    {}

  /**
   * \brief Destructor.
   */
  ~Fed9UEntry()
  {
  }
  
  /**
   * \brief  The base address of a FED object defines the start of it memory address and also its physical location in the crate.
   * \return unsigned long Base address of the FED entry.
   */
  inline unsigned long getBaseAddress() const {return mBA;}

  /**
   * \brief  Each FED has a serial number, which defines it uniquely.
   * \return unsigned long Serial number of the FED entry.
   */
  inline unsigned long getSerialNumber() const {return mSerialNo;}

  /**
   * \brief  The physical location of the FED in a crate is defined by its slot number.
   * \return unsigned long Returns the slot number of the entry.
   */
  inline unsigned long getSlotNumber() const {return mSlotNumber;}    

  /**
   * \todo - find out what this does.
   */
  inline bool getTestFlag() const {return mTestMe;}    

  /**
   * \brief  Each FPGA on the FED has a temperature control monitor, that will cause the power to the FED to be cut in event of the FPGA over heating.
   * \return unsigned long Maximum temperature that the LM82 will allow the FPGA to reach before cutting power to the board.
   */
  //<GR date=14/08/2006>
  //Changed the getMaxLM82Temp to return an unsigned long instead of a bool as that is what it is stored as and set as.
  inline unsigned long getMaxLM82Temp() const {return mMaxLM82Temp;}
  //inline bool getMaxLM82Temp() const {return mMaxLM82Temp;}
  //</GR>

  /**
   * \brief The base address of a FED object defines the start of it memory address and also its physical location in the crate.
   * \param ba Sets the base address of the FED entry.
   */
  inline void setBaseAddress(unsigned long ba){mBA=ba;}

  /**
   * \brief Each FED has a serial number, which defines it uniquely.
   * \param sn Sets the serial number of the FED entry.
   */
  inline void setSerialNumber(unsigned long sn){mSerialNo=sn;}

  /**
   * \todo - find out what this does.
   */
  inline void setTestFlag(bool test){mTestMe=test;}

  /**
   * \brief The physical location of the FED in a crate is defined by its slot number.
   * \param slot Sets the slot number of the entry.
   */
  inline void setSlotNumber(unsigned long slot){mSlotNumber=slot;}    

  /**
   * \brief Each FPGA on the FED has a temperature control monitor, that will cause the power to the FED to be cut in event of the FPGA over heating.
   * \param temp Sets the maximum temperature that the LM82 will allow the FPGA to reach before cutting power to the board.
   */
  inline void setMaxLM82Temp(unsigned long temp){mMaxLM82Temp=temp;}    

#ifdef TBHARDWARE
  /**
   * \brief  The FED entry can be a real FED or software decription that is derived from Fed9UABC if TBHARDWARE is defined.
   * \return Fed9UABC Returns a pointer to the Fed9UABC entry in the class.
   */
  inline Fed9U::Fed9UABC * getFed9UABC(){return mpFed;}

  /**
   * \brief The FED entry can be a real FED or software decription that is derived from Fed9UABC if TBHARDWARE is defined.
   * \param f Sets the pointer to the Fed9UABC class that is used to access the FED object.
   */
  inline void setFed9UABC(Fed9U::Fed9UABC * f){mpFed=f;}

  /**
   * \brief Deletes the Fed9UABC object stored in the Fed9UABC pointer.
   */
  inline void purge() { delete mpFed; }
#endif

  /**
   * \brief  Checks the FED entry to see if a serial number has been set.
   * \return bool Boolean true if the serial number has been set.
   */
  inline bool isSerialNumberSet(){return mSerialNo!=0;} //Assumes a serial number of zero will never exist...

  /**
   * \brief Prints the serial number, base address and test to standard output, cout, along with some explanatory text.
   */
  inline void display() const {
    std::cout<<"Serial No = "<<std::hex<<mSerialNo
	     <<", BA = "<<std::hex<<mBA
	     <<", test = "<<std::dec<<mTestMe
	     <<std::endl;
  }

private:
  unsigned long mSerialNo;   //!< Stores the local copy of the serial number.
  unsigned long mBA;         //!< Stores the local copy of the base address.
  unsigned long mSlotNumber; //!< Stores the local copy of the slot number.
  bool mTestMe;              //!< \todo - find out what this does.
  u32 mMaxLM82Temp;          //!< Stores the local copy of the maximum temperature the LM82 will allow a FPGA or itself to reach.
#ifdef TBHARDWARE
  Fed9UABC * mpFed;          //!< If TBHARDWARE is defined, then this will store a pointer to the FED that is being used.
#endif

  };

}

#endif // Fed9UEntry_HH
