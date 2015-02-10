#ifndef _Fed9ULogTemplate_H_
#define _Fed9ULogTemplate_H_

#include "TypeDefs.hh"

#include <pthread.h>

#include <iomanip>
//#include <ostream>
#include <sstream>
#include <typeinfo>

namespace Fed9U {

  template<typename T> class Fed9UMessage;
  class Fed9ULog;

  /**
   * \brief  A template class that once instantiated creates a wrapper for an output stream, making each write to that stream
   *         via the operator<< atomic through mutexes.
   * \author Gareth Rogers, Jonathan Fulcher.
   * \date   2006/09/01
   *
   * The stream handles its own resource locking, thus providing thread safety. It is possible that it will be unable to construct
   * and initialise the mutex it uses for resource locking. In this case the class will still construct, however it will no longer
   * be thread safe and a write or function calls can no longer be guarantied to be atomic. A check to ensure the class is initialised
   * fully is provided though a public member function, it also prints a message to the stream warning of this failure. This should
   * almost never happen.\n\n
   *
   * There are two member functions that are not thread safe, as these are used for reporting errors that occured while attempting
   * to achieve resource locks to the stream, getLockErrorNumber and getUnlockErrorNumber. As these report errors that happen while
   * thread locking or unlocking is in progress it is not possible to provide atomicity for these functions. If errors occur while
   * messages are being written to the stream then the data will be written anyway in a non thread safe way.
   *
   * The stream supports the writing of all data types that can be written to a std::ostream, as well as all the manipulators.
   */
  class Fed9UStream : public std::ostream {

  public:

    /**
     * \brief Constructor.
     * \param os A pointer to the output stream that the messages are to be sent to.
     *
     * The constructor also initialises the mutex that will protect the writes to the underlying buffer and provide
     * thread safety for the buffer writes. If mutex initialisation fails then the class will construct successfully
     * and it should be checked that it is thread safe by checking the appropriate public member function.
     */
    explicit Fed9UStream(std::ostream* const os);

    //<GJR date=26/11/2006>
    /**
     * \brief Constructor.
     * \param filename Name of the file that is to be used to write log messages to, if an empty string is passed std::cout
     *        will be used and no file is created.
     *
     * This will initialise the log stream with a file as its underlying buffer, whose name is provided upon construction.
     * The constructor also initialises the mutex that will protect the writes to the underlying buffer and provide
     * thread safety for the buffer writes. If mutex initialisation fails then the class will construct successfully
     * and it should be checked that it is thread safe by checking the appropriate public member function.
     */
    explicit Fed9UStream(const std::string& fileName);
    //</GJR>

    /**
     * \brief Destructor.
     *
     * Release any new'd memory.
     */
    ~Fed9UStream();

    /**
     * \brief  Returns the state of the class mutex.
     * \return boolean. True if the mutex has been correctly initialised, false if it has not.
     *
     * If a false is return the operator<< is no longer guarantied to be atomic and will behave
     * for example like std::cout in a threaded environment.
     */
    bool getMutexIsInitialised() const;

    /**
     * \brief  Writes the time to the file.
     * \return Self reference.
     */
    Fed9UStream& stamp();

    /**
     * \brief  Allows the std::ostream object that the Fed9UStream writes to be moved to a new stream.
     * \param  os Pointer to the std::ostream whose buffer we wish to use.
     * \return Self reference.
     *
     * This is an atomic operation, provided the mutex has been initialised, however moving a buffer within multiple threads
     * will cause difficult to follow behaviour as the order in which the threads move the buffer can never be gaurantied.
     * Never the less one thread can safely change the output buffer while other threads may be trying to write to it.
     * The User will have to ensure that it is done in a safe order.
     */
    Fed9UStream& setNewOstream(std::ostream* const os);

    /**
     * \brief  Allows the std::ostream object that the Fed9UStream writes to be moved to a new file.
     * \param  filename Name of the file that is to be used for the log, if an empty string is passed then it will be redirected
     *         to std::cout.
     * \return Self reference.
     *
     * This is an atomic operation, provided the mutex has been initialised, however moving a buffer within multiple threads
     * will cause difficult to follow behaviour as the order in which the threads move the buffer can never be gaurantied.
     * Never the less one thread can safely change the output buffer while other threads may be trying to write to it.
     * The User will have to ensure that it is done in a safe order.
     */
    Fed9UStream& setNewOstream(const std::string& filename);

   private:

    /**
     * \brief  Writes the current error returned (can be success) from the most recent lock attempt and returns the error
     *         number itself.
     * \return int Error number that occured. It can be decoded as follows using the following defines by the pthread library:
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct. 
     *         \li EDEADLK A thread attempted to relock an already held mutex, and the mutex is an errorcheck mutex type.
     *
     * The class guards against most of these errors and will prevent most from occuring, even if abused. This is not thread
     * safe.
     */
    int getLockErrorNumber();

    /**
     * \brief  Writes the current error returned (can be success) from the most recent unlock attempt and returns the error
     *         number itself.
     * \return int Error number that occured. It can be decoded as follows using the following defines by the pthread library:
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct.
     *         \li EPERM The mutex is not currently held by the caller.
     *
     * The class guards against most of these errors and will prevent most from occuring, even if abused. This is not thread
     * safe.
     */
    int getUnlockErrorNumber();

    /**
     * \brief  Initialises the class mutex, used to protect write operation within the member functions.
     * \return bool Returns true of the initialisation of the mutex was successful.
     */
    bool initMutex();

    /**
     * \brief  Writes an error number from any mutex operation to the stream. It takes the error number to be written and is
     *         thus re-enterant.
     * \param  errNum Error number that occured.
     * \return Self reference.
     *
     * The following errors will be written to the stream as text, any other will just have their number written.
     *         \li 0 Operation was successful.
     *         \li EINVAL The value specified for the argument is not correct. 
     *         \li EDEADLK A thread attempted to relock an already held mutex, and the mutex is an errorcheck mutex type.
     *         \li EBUSY The mutex is currently owned by another thread.
     *         \li ENOMEM The system cannot allocate the resources required to create the mutex. 
     */
    Fed9UStream& setMutexErrorNumber(int errNum);

  public:

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UStream, such as std::endl;
     * \param  std::ostream& (*pf)(std::ostream&) A pointer to a function that takes a std::ostream& as an arugment and returns a std::ostream&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::ios_base as arguments, any manipulator can be written to the Fed9UStream objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UStream& operator<<(std::ostream& (*pf)(std::ostream&));

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UStream, such as std::endl;
     * \param  std::ios_base& (*pf)(std::ios_base&) A pointer to a function that takes a std::ios_base& as an arugment and returns a std::ios_base&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::iostream as arguments, any manipulator can be written to the Fed9UStream objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UStream& operator<<(std::ios_base& (&pf)(std::ios_base&));

    /**
     * \brief  This overload of the operator<< allows a Fed9UMessage<T> to be set for a Fed9UStream object.
     * \param  level The level that is to be written.
     * \return const Fed9UMessage<T>& A reference to the level that was written.
     *
     * The template parameter T should be an enum that defines the three different types of log that are available for the Fed9U
     * project. Fed9UDebugLevel, Fed9UErrorLevel or Fed9ULogLevel defined in the file TypeDefs.hh are the three that can be used.
     * When the Fed9UMessage<T> is sent to the stream it is not written to the stream, rather it is returned and all subsequent
     * writes will be to the Fed9UDebugLevel object. An overloaded operator is provided which writes the object
     * writes to the appropriate stream will be handled by the Fed9UMessage<T> class and its overloaded operator<<. Only those
     * level that exceed the environmental level will be written. Thus control is provided over what detail of message is provided.
     */
    template<typename T>
    Fed9UMessage<T> operator<<(const Fed9UMessage<T>& level) {
      //std::cout << FED9U_FUNCTION << std::endl;
      return level;
    }//operator<<(Fed9UMessage<T>& level)

    /**
     * \brief  An overload of the operator<< which allows any data type that can be written to an ostream to be written to any instantiation of the
     *         template class Fed9ULogTemplate.
     * \param  data Templated data type that is to be written to the stream. The data type specified by U must have an overload version of the
     *         operator<< for an std::ostream object for it to be written to the log.
     * \return Fed9UStream& Self reference.
     */
    template<typename U>
    Fed9UStream& operator<<(const U& data) {
      //std::cout << FED9U_FUNCTION << std::endl;
      //If have successfully initialised our mutex then we can ask it for a lock to ensure
      //that will behave in a nice thread safe manor, otherwise will just have to send it straight to the ostream.
      if ( mMutexIsInitialised ) {
	//Request a lock on the mutex...
	mErrorLocking = pthread_mutex_lock(&mWriteMutex);
	if (0 == mErrorLocking) {
	  //We were successful in getting the mutex.
	  //This bit of the code actually does the write.
	  //We first cast the template object back to its base class std::ostream and then write the data to the stream.
	  //This allows any data type that has an overloaded operator<< for writing to std::ostreams.
	  *(dynamic_cast<std::ostream*>(this)) << data;

	  //Now we are finished with our writing we can unlock the mutex...
	  mErrorUnlocking = pthread_mutex_unlock(&mWriteMutex);
	  if (0 != mErrorUnlocking) {
	    //...we can't unlock the mutex, the program will probably hang but there is little that can be done
	    //about it. Write the unlock error to the stream and hope we fix the bug that caused it.
	    getUnlockErrorNumber();
	  }
	} else {
	  //The lock failed for some reason. We are now basically screwed and thread safety is out of
	  //the window. Write the error to file and hope that it is not too garbled to read,
	  //also that it was the error number from the calling thread.
	  getLockErrorNumber();
 	}
      } else {
	//Don't have a mutex to use, so will have to work in a none thread safe manor.
	//Hence just write the data straight to the stream.
	*(dynamic_cast<std::ostream*>(this)) << data;
      }

      //...we are now done, so return a reference to the input stream thus allowing stream commands to be chained together.
      return *this;
    }//operator<<(U)


  private:

    Fed9ULog* mpFileLog;  //!< If a file log is required then this will point to the file object.
    pthread_mutexattr_t mWriteMutexAttr; //!< These are the attributes of the mutex used. Controls 
    pthread_mutex_t mWriteMutex;         //!< It is this mutex that must be aquired before writes to the buffer can be performed.
    bool mMutexIsInitialised;            //!< If the mutex fails to get initialised and is in an unusable state, then this data member tells the class about it.
    u32 mErrorLocking;   //!< Stores the last error number to be recorded when a lock was attempted. Should be zero and hence error free.
    u32 mErrorUnlocking; //!< Stores the last error number to be recorded when an unlock was attempted. Should be zero and hence error free.

  };//class Fed9UStream


  /**
   * \brief This is a global Fed9UStream object that is used to write debug messages during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9UOut;

  /**
   * \brief This is a global Fed9UStream object that is used to write error messages during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9UErr;

  /**
   * \brief This is a global Fed9UStream object that is used to log messages to file during function excution in an atomic mannar.
   */
  extern Fed9UStream gFed9ULog;


  /**
   * \brief  Used to set the level of a message that is to be written to one of the global streams provided by the Fed9U library.
   * \author Gareth Rogers, Jonathan Fulcher
   * \date   2006/09/07
   *
   * The template parameter T should refer to on of the message level enums (Fed9UDebugLevel, Fed9UErrorLevel or Fed9ULogLevel) and the
   * behaviour of the class is undefined if it is constructed with any other type. The class supports the streaming of any data type
   * that has an overloaded operator<< for use with a std::ostream, plus all the manipulators for use with std::ostreams.
   * The message level written will persit until either a new level is written or a semi colon, ending the line of code, is encountered.
   * The message level can only be changed within the same enum level. You could not for example write one message with a Fed9UDebugLevel
   * and then another at a Fed9UErrorLevel within the same line of code.
   */
  template<typename T>
  class Fed9UMessage {

  public:

    /**
     * \brief Constructor
     * \param level The message level that should be set for any subsequent write to the stream. The level will persit in the stream until
     *        either a new level is written or a semi colon ends the line of code.
     *
     * During construction the class also identifies which stream (gFed9UOut, gFed9UErr or gFed9ULog) and which global message level (Fed9U::Fed9UMessage<Fed9UDebugLevel>::smLevel,
     * Fed9U::Fed9UMessage<Fed9UErrorLevel>::smLevel or Fed9U::Fed9UMessage<Fed9ULogLevel>::smLevel) should be used when deciding whether the message is of sufficient level to be written to the appropriate stream.
     */
    Fed9UMessage(const T& level)
      : mLevel(level), mGlobalLevel(NULL), mGlobalStream(NULL) {

      if ( typeid(T) == typeid(Fed9UDebugLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9UDebugLevel>::smLevel);
	mGlobalStream = &gFed9UOut;
      } else if ( typeid(T) == typeid(Fed9UErrorLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9UErrorLevel>::smLevel);
	mGlobalStream = &gFed9UErr;
      } else if ( typeid(T) == typeid(Fed9ULogLevel) ) {
	mGlobalLevel  = reinterpret_cast<T*>(&Fed9U::Fed9UMessage<Fed9ULogLevel>::smLevel);
	mGlobalStream = &gFed9ULog;
        // <NAC date="16/05/2007"> stamp log
        mGlobalStream->stamp();
        // </NAC>
      } else {
	//Do nothing everything remains null.
      }

    }//Fed9UMessage(const T& level)

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UMessage, such as std::endl;
     * \param  std::ostream& (*pf)(std::ostream&) A pointer to a function that takes a std::ostream& as an arugment and returns a std::ostream&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::ios_base as arguments, any manipulator can be written to the Fed9UMessage objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UMessage<T> operator<<(std::ostream& (*pf)(std::ostream&)) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << pf;

      return *this;
    }//operator<<(std::ostream& (*pf)(std::ostream&))

    /**
     * \brief  Provides a wrapper that allows the manipulators with the given function prototype to be written to a Fed9UMessage, such as std::endl;
     * \param  std::ios_base& (*pf)(std::ios_base&) A pointer to a function that takes a std::ios_base& as an arugment and returns a std::ios_base&.
     *         The manipulators such as std::endl, std::hex have this signature.
     *
     * With the wrapper function and the overload with std::iostream as arguments, any manipulator can be written to the Fed9UMessage objects and have
     * the effects they would on an ostream or derived classes.
     */
    Fed9UMessage<T> operator<<(std::ios_base& (&pf)(std::ios_base&)) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << pf;

      return *this;
    }//operator<<(std::ios_base& (*pf)(std::ios_base&))

    /**
     * \brief  This allows a new message level to be set for subsequent writes to the stream.
     * \param  newLevel The new message level to be written at.
     * \return Self reference.
     *
     * Message levels can only be changed within the same message level type (i.e. FED9U_DEBUG_LEVEL_DETAILED to FED9U_DEBUG_LEVEL_INTERMEDIATE,
     * but not FED9U_DEBUG_LEVEL_DETAILED to FED9U_ERROR_LEVEL_CRITICAL). The new level persists for the same length of time the old level
     * would have persisted for. Until a new level is written or a semi colon indicates the end of the line of code.
     */
    Fed9UMessage<T> operator<<(const Fed9UMessage<T>& newLevel) {
      //std::cout << FED9U_FUNCTION << std::endl;
      return newLevel;
    }//operator<<(const Fed9UMessage<T>& newLevel)

    /**
     * \brief  This will write any type that can be written to a std::ostream to the appropriate stream, provided the level is greater
     *         than or equal to the environmental level.
     * \param  data Having a templated type U, it can be any object or built in type. Provided it can be written to an std::ostream.
     * \return Self reference.
     *
     * By returning a self reference it ensures that chained stream calls always are written to the message level.
     */
    template<typename U>
    Fed9UMessage<T> operator<<(const U& data) {
      //std::cout << FED9U_FUNCTION << std::endl;
      if (mLevel <= *mGlobalLevel || NULL == mGlobalLevel)
	(*mGlobalStream) << data;

      return *this;
    }//operator<<(const U& data)
    
    //template<typename U>
    //friend const Fed9UMessage<Fed9UDebugLevel>& operator<<(const Fed9UMessage<Fed9UDebugLevel>& level, U data);

  /**
   * \brief A static variable to define the level of debug printout that is sent to the debug stream.
   *
   * This is initialised to the FED9U_DEBUG_LEVEL_MINIMAL upon program start up (in Fed9ULogTemplate.cc), which means no messages will be sent to the debug
   * stream. It can be set to any level at run time, which will immediately cause debug messages of that level be sent to the
   * debug stream.
   */
  static T smLevel;

  /**
   * \brief A static variable to define the level of error printout that is sent to the error stream.
   *
   * This is initialised to the FED9U_ERROR_LEVEL_CRITICAL upon program start up (in Fed9ULogTemplate.cc), which means only erorr messages defined as critical
   * will be sent to the debug stream. It can be set to any level at run time, which will immediately cause debug messages of that
   * level be sent to the error stream.
   */
  //static Fed9UErrorLevel sgErrorLevel;

  /**
   * \brief A static variable the defines what level of log messages are to be sent to the Fed9U file log.
   *
   * This is initialised to FED9U_LOG_LEVEL_NONE upon program start up (in Fed9ULogTemplate.cc), which means no log file is created until the
   * log level is set to a higher value and the first message is sent to the log. It can be changed to any level at
   * run time, which will cause the log messages of that level to be immediately written to the stream.
   */
  //static Fed9ULogLevel sgLogLevel;


  private:

    const T& mLevel;
    const T* mGlobalLevel;
    Fed9UStream* mGlobalStream;

  };//class Fed9ULogLevelTemplate

}//namespace Fed9U

#endif//_Fed9ULogTemplate_H_
