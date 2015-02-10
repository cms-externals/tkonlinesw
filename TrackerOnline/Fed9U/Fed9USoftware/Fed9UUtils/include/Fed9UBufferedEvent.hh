#ifndef H_Fed9UBufferedEvent
#define H_Fed9UBufferedEvent

#include "TypeDefs.hh"
#include "Fed9UDescription.hh"
#ifdef EVENT_STREAMLINE
#include "Fed9UEventStreamLine.hh"
#else
#include "Fed9UEvent.hh"
#endif
#include <iosfwd>
#include <vector>

namespace Fed9U {
  //using.*std::ostream;
  
  /**
   * \brief Designed to extend Fed9UEvent by adding an internal event buffer.
   * \class Fed9UBufferedEvent
   *
   * \author M. Noy
   * \version 0.0
   * \date 04-11-2003
   *
   * \todo - Add support for super DAQ mode.
   */
#ifdef EVENT_STREAMLINE
  class Fed9UBufferedEvent: public Fed9UEventStreamLine
#else
  class Fed9UBufferedEvent:public Fed9UEvent
#endif
  {
  public:

    /**
     * \name Public constructors.
     *
     * Class constructors available to Users.
     */

    //@{
    /**
     * \brief Default constructor.
     *
     * Sets _isOnline to true and _eventBufferLength to zero.
     */
    Fed9UBufferedEvent();

    /**
     * \brief Constructs a Fed9UBufferedEvent from an unsigned int array.
     * \param buffer Pointer to an unsigned long array containing the event buffer.
     * \param currentDescription Current FED settings that used to record the event buffer.
     * \param bufsize Size of the event buffer. Defaulted to 0.
     *
     * A buffered event is constructed from the data pointed to by arugment buffer. It does this by calling loadEvent.
     * _isOnline is set to true and _eventBufferLength to bufsize.
     */
    Fed9UBufferedEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize = 0);

    /**
     * \brief Constructs a Fed9UBufferedEvent from file.
     * \param input Reference to the input stream that contains the event data.
     * \param FedDescriptionAtTheTime FED settings that were used to record the event.
     *
     * Data is constructed from file, which should contain the buffer size and DAQ mode as the first two 32 bits words, then the data for buffer size worth
     * of 32 bits words containing the event data.
     *
     * \todo - check nothing funny with the file format as the described format is based on the code.
     */
    Fed9UBufferedEvent(std::istream & input, Fed9UDescription * FedDescriptionAtTheTime);    
    //@}

    /**
     * \brief Destructor.
     */
    ~Fed9UBufferedEvent();    
     
    
//      u32 getEventNumber() const;
//      u8  getEventType() const;
//      u16 getBunchCrossing() const;
//      u16 getSourceId() const;
//      u8  getFormatVersion() const;
//      u8  getMajorityPipeline(int feUnit) const;
//      u32 getTotalLength() const;
//      u16 getCrc() const;
//      u16 calcCrc() const;
    //void saveIgorFile(std::ostream & o) const;
    //void dump(std::ostream&) const;
  
    /**
     * \brief  Writes the event to file.
     * \param  output Output stream to write the data to.
     * \return unsigned long Returns 0 if the member function completed.
     *
     * The file is written with the event size as the first 32 bit word, DAQ mode as the second and then the subsequent event size worth of words
     * contain the event buffer.
     */
    u32 writeBufferedEventToFile(std::ostream & output);

    /**
     * \brief  Gives a pointer to the first element in the event buffer.
     * \return unsigned long* Pointer to the first element.
     */
    u32 * getEventBufferPointer();

    /**
     * \brief  Private member function to read the events from file.
     * \param  in Input stream containing the event file. First two 32 bit words should be the event length and DAQ mode respectively. Remaining data
     *         is the event buffer.
     * \param  desc Pointer to the description that contains the FED settings when the event was recorded.
     * \return unsigned long Upon completion of the member function a zero is returned.
     *
     * Once the file has been read the internal buffer is initialised with the file data.
     */
    inline u32 getBufferedEventFromFile(std::istream & in, Fed9UDescription * desc){return getEventFromFile(in, desc);}
    
    /**
     * \brief Loads an event buffer into the internal event buffer and initialises the appropriate class member variables.
     * \param buffer Pointer to the beginning of the unsigned long array that contains the event words.
     * \param currentDescription A Fed9UDescription object that contains the FED settings when the event was recorded.
     * \param bufsize Size of the event buffer pointed to by buffer.
     *
     * Should be used to create a new event in the existing Fed9UBufferedEvent object or to add one to a default constructed object.
     */
    void loadEvent(u32 * buffer, const Fed9UDescription * currentDescription, u32 bufsize);

    /**
     * \brief  Returns the data acquisition mode used to record the event.
     * \return Fed9UDaqMode An enumeration that gives the DAQ mode used to record the event. Further details of the allowed values can be
     *         found in the Fed9UDaqMode documentation.
     */
    Fed9UDaqMode getBufferedDaqMode() const {return _daqMode;}

  protected:
    
  private://member functions

    /**
     * \name Private constructors.
     *
     * Constructors used for either internal purposes or to prevent copying of Fed9UBufferedEvent objects.
     */
    //@{

    /**
     * \brief Copy constructor. Unimplemented.
     */
    Fed9UBufferedEvent(const Fed9UBufferedEvent &);

    /**
     * \brief Assignment operator. Unimplemented.
     */
    Fed9UBufferedEvent & operator = (const Fed9UBufferedEvent &);
    //@}

    /**
     * \brief  Private member function to read the events from file.
     * \param  in Input stream containing the event file. First two 32 bit words should be the event length and DAQ mode respectively. Remaining data
     *         is the event buffer.
     * \param  desc Pointer to the description that contains the FED settings when the event was recorded.
     * \return unsigned long Upon completion of the member function a zero is returned.
     *
     * Once the file has been read the internal buffer is initialised with the file data. Has been made private due to an accident with name convension.
     * getBufferedEventFromFile is the public member function that performs this task.
     */
    u32 getEventFromFile(std::istream & in, Fed9UDescription * desc);

    /**
     * \brief Copies an external buffer into the class internal buffer.
     * \param buf Pointer to an unsigned long array containing the event.
     * \param bufLen Size of the buffer begin copied.
     */
    void chargeEventBuffer(u32 * buf, u32 bufLen);   

  private://data members

    std::vector<u32> _eventBuffer; //!< Internal buffer used to store the event.

    u32 _eventBufferLength;        //!< Length of the event buffer.

    bool _isOnline;                //!< True if the buffer is "online" i.e. taken from a buffer that has not been saved for later analysis.
                                   //!< False is the buffer is "offline" i.e. loaded from a file or some other storage mechanism.

    Fed9UDaqMode _daqMode;         //!< Data acquision mode that the event was recorded in.
    

  };
}

#endif //H_Fed9UBufferedEvent
