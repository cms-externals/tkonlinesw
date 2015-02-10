#include "Fed9ULogTemplate.hh"
#include "Fed9ULog.hh"

#include <sstream>
#include <string>
#include <errno.h>

namespace Fed9U {


  Fed9UStream::Fed9UStream(std::ostream* const os)
    : std::ostream( os->rdbuf() ), mpFileLog(NULL), mMutexIsInitialised(false), mErrorLocking(0), mErrorUnlocking(0) {
    //Initialise the pthread mutex that will be used to ensure that operations on the class
    //member functions are atomic.
    //Don't really need to check the return arugment, nothing can be done here.
    //std::cout << "Initialised." << std::endl;
    initMutex();

  }//Fed9Stream::Fed9UStream

  //<GJR date=26/11/2006>
  Fed9UStream::Fed9UStream(const std::string& filename)
    //Compiler enforces the initialisation of the class its inherited from first, hence we have to initialise it with
    //a buffer that can be guarantied to be there, std::cout, and then move it after to our desired buffer.
    : std::ostream( std::cout.rdbuf() ), mpFileLog(NULL), mMutexIsInitialised(false), mErrorLocking(0), mErrorUnlocking(0) {
    //Initialise the pthread mutex that will be used to ensure that operations on the class
    //member functions are atomic.
    //Don't really need to check the return arugment, nothing can be done here.
    //    std::cout << "Do I reach here?" << std::endl;
    initMutex();

    //Now we set up the log file to point at the appropriate file, unless of course it is a NULL string, in which case
    //we can defer the initialisation of the stream till later.
    if ("" != filename) {
      //Create the log object.
      mpFileLog = new Fed9ULog( filename.c_str() );
      //Now move our buffer to point at it.
      setNewOstream( mpFileLog );
    }

  }//Fed9Stream::Fed9UStream
  //</GJR>

  Fed9UStream::~Fed9UStream() {
    //Delete the log file object if one was ever created, and set to NULL for good practice.
    if (NULL != mpFileLog) {
      delete mpFileLog;
      mpFileLog = NULL;
    }
  }


  bool Fed9UStream::initMutex() {

    //A bool to inform the member function if it was successful in setting the mutex attributes.
    //At present it is not that useful, as we will just initialise the mutex with defaults anyway if it fails.
    //May become useful if more complex things are done to the mutex to initialise it.
    bool lMutexAttrSuccess(false);

    //Initialise the mutex attributes to default...
    int lErrAtInit( pthread_mutexattr_init(&mWriteMutexAttr) );
    if (0 == lErrAtInit) {
      //...change the attribute 'type' so that it has error checking...
      int lErrAtCheck( pthread_mutexattr_settype(&mWriteMutexAttr, PTHREAD_MUTEX_ERRORCHECK) );
      if (0 == lErrAtCheck) {
	//...successfully managed to set all the attribute types, so mark this as having been done...
	lMutexAttrSuccess = true;
      } else {
	//...failure to update the attributes, so mark this as a failure...
	lMutexAttrSuccess = false;
	setMutexErrorNumber(lErrAtCheck);
      }//if/else (0==lErrAtCheck)
    } else {
      //...failed to initialise the attributes, so mark this as a failure...
      lMutexAttrSuccess = false;
      setMutexErrorNumber(lErrAtInit);
    }//if/else (0==lErrAtInit)

    //Will hold any error number that is returned from the mutex initialisation.
    int lErrMuInit(0);
    if (lMutexAttrSuccess) {
      //We have successfully initialised the mutex attributes to those we want and so initialise with those values...
      lErrMuInit = pthread_mutex_init(&mWriteMutex, &mWriteMutexAttr);
      //...check for errors...
      if (0 == lErrMuInit) {
	//...woot! success in the initialisation, better tell everyone...
	mMutexIsInitialised = true;
      } else {
	//...not particularly good, failed to init the mutex...
	mMutexIsInitialised = false;
	setMutexErrorNumber(lErrMuInit);
      }//if/else (0==lErrMuInit)
    } else {
      //Weren't successful in initialising the attributes, initialise the mutex with defaults it shouldn't matter anyway...
      lErrMuInit = pthread_mutex_init(&mWriteMutex, NULL);
      if (0 == lErrMuInit) {
	//...woot! success in the initialisation, better tell everyone...
	mMutexIsInitialised = true;	  
      } else {
	//...not particularly good, failed to init the mutex...
	mMutexIsInitialised = false;
	setMutexErrorNumber(lErrMuInit);
      }//if/else (0==lErrMuInit)
    }//if/else (lErrMuInit)

    if (mMutexIsInitialised) {
      //*this << "Log stream initialised." << std::endl;
    } else {
      //We failed to initialise the mutex, so it cannot be used thus making the class no longer thread safe.
      //Output to the stream can still be performed, but it is no longer guarantied to be atomic and so garbled
      //messages may (will) result when threads are used. Still the object will limp on with reduced functionality.
      //Send a message to the to warn people about this.
      std::ostringstream errMsg;
      errMsg << "\n\nFailed to initialise the mutex to use for protecting writes to the stream.\n"
	     << "Log is still available, however it will NO LONGER BE THREAD SAFE.\n"
	     << "Mutex initialisation returned the error code:\n";
      if (EINVAL == lErrMuInit) {
	errMsg << "EINVAL: The value specified for the argument is not correct.";
      } else if (ENOMEM) {
	errMsg << "ENOMEM: The system cannot allocate the resources required to create the mutex.";
      } else {
	errMsg << lErrMuInit << ": Unknown error code.";
      }

      if (!lMutexAttrSuccess) {
	errMsg << "The mutex attributes also failed to initialised and returned the error code:\n";
	if (EINVAL == lErrAtInit) {
	  errMsg << "EINVAL: The value specified for the argument is not correct.";
	} else {
	  errMsg << lErrAtInit << ": Unknown error code.";
	}
      }
 
      errMsg << "\n" << std::endl;
      *this << errMsg.str();

    }

    return mMutexIsInitialised;
  }//Fed9UStream::initMutex()


  bool Fed9UStream::getMutexIsInitialised() const {
    return mMutexIsInitialised;
  }//getMutexIsInitialised()


  Fed9UStream& Fed9UStream::stamp() {
    (*this) << "Timestamp: " << Fed9ULog::getTimeString() << std::endl;
    return *this;
  }


  Fed9UStream& Fed9UStream::setNewOstream(std::ostream* const os) {
    //In order to point the underlying stream buffer to a new location, we will go through the member functions
    //implemented in the base class std::ostream. Using our write mutex we must ensure that no one else can
    //access the buffer while it is being moved!

    //Just check the given stream is not NULL, we can't do anything if it is...
    if (NULL == os)
      return *this;

    //First we acquire the mutex, if it is initialised...
    if ( mMutexIsInitialised ) {
      //...requesting a lock on the mutex...
      mErrorLocking = pthread_mutex_lock(&mWriteMutex);
      if (0 == mErrorLocking) {
	//...we were successful in getting the mutex,
	//delete any existing file that we may be writing to.
	if (NULL != mpFileLog) {
	  delete mpFileLog;
	  mpFileLog = NULL;
	}
	//take the given std::ostream streambuf and connect it to our own (using base class member functions)...
	dynamic_cast<std::ostream*>(this)->rdbuf( os->rdbuf() );

	//...now we are finished with our move we can unlock the mutex...
	mErrorUnlocking = pthread_mutex_unlock(&mWriteMutex);
	if (0 != mErrorUnlocking) {
	  //...we can't unlock the mutex, the program will probably hang but there is little that can be done
	  //about it. Write the unlock error to the stream and hope we fix the bug that caused it...
	  getUnlockErrorNumber();
	}
      } else {
	//...the lock failed for some reason. We are now basically screwed and thread safety is out of
	//the window. Write the error to file and hope that it is not too garbled to read,
	//also that it was the error number from the calling thread...
	getLockErrorNumber();
      }
    } else {
      //...don't have a mutex to use, so will have to work in a none thread safe manor,
      //delete any existing file that we may be writing to.
      if (NULL != mpFileLog) {
	delete mpFileLog;
	mpFileLog = NULL;
      }
      //we take the given std::ostream streambuf and connect it to our own (using base class member functions)...
      dynamic_cast<std::ostream*>(this)->rdbuf( os->rdbuf() );
    }
    //...and we are done.
    return *this;
  }//setNewOstream(std::ostream* const os)


  Fed9UStream& Fed9UStream::setNewOstream(const std::string& filename) {
    //In order to point the underlying stream buffer to a new location, we will go through the member functions
    //implemented in the base class std::ostream. Using our write mutex we must ensure that no one else can
    //access the buffer while it is being moved!

    //First we acquire the mutex, if it is initialised...
    if ( mMutexIsInitialised ) {
      //...requesting a lock on the mutex...
      mErrorLocking = pthread_mutex_lock(&mWriteMutex);
      if (0 == mErrorLocking) {
	//...we were successful in getting the mutex,
	//so we need to delete the old file object if it existed and update it with a new one.
	if (NULL != mpFileLog ){
	  delete mpFileLog;
	  mpFileLog = NULL;
	}
	//Now decide where to set it to.
	if ("" != filename) {
	  //We have a valid file name so create a Fed9ULog object to write to.
	  mpFileLog = new Fed9ULog( filename.c_str() );
	  //and update our buffer to point to it.
	  dynamic_cast<std::ostream*>(this)->rdbuf( mpFileLog->rdbuf() );
	} else {
	  //an empty string was passed so set to std::cout's buffer.
	  dynamic_cast<std::ostream*>(this)->rdbuf( std::cout.rdbuf() );
	}

	//...now we are finished with our move we can unlock the mutex...
	mErrorUnlocking = pthread_mutex_unlock(&mWriteMutex);
	if (0 != mErrorUnlocking) {
	  //...we can't unlock the mutex, the program will probably hang but there is little that can be done
	  //about it. Write the unlock error to the stream and hope we fix the bug that caused it...
	  getUnlockErrorNumber();
	}
      } else {
	//...the lock failed for some reason. We are now basically screwed and thread safety is out of
	//the window. Write the error to file and hope that it is not too garbled to read,
	//also that it was the error number from the calling thread...
	getLockErrorNumber();
      }
    } else {
      //...don't have a mutex to use, so will have to work in a none thread safe manor,
      //so we need to delete the old file object if it existed and update it with a new one.
      if (NULL != mpFileLog ){
	delete mpFileLog;
	mpFileLog = NULL;
      }
      //Now decide where to set it to.
      if ("" != filename) {
	//We have a valid file name so create a Fed9ULog object to write to.
	mpFileLog = new Fed9ULog( filename.c_str() );
	//and update our buffer to point to it.
	//we take the given std::ostream streambuf and connect it to our own (using base class member functions)...
	dynamic_cast<std::ostream*>(this)->rdbuf( mpFileLog->rdbuf() );
      } else {
	//an empty string was passed so set to std::cout's buffer.
	//we take the given std::ostream streambuf and connect it to our own (using base class member functions)...
	dynamic_cast<std::ostream*>(this)->rdbuf( std::cout.rdbuf() );
      }
    }
    //...and we are done.
    return *this;
  }//setNewOstream(std::ostream* const os)


  int Fed9UStream::getLockErrorNumber() {
    //Perform a none thread safe write to the ostream.
    //Casts back to the inherited stream operator<< definition and hence bypasses the
    //class overloaded version of the operator<<, which provides the thread safety.
    //Neccessary as it is used to report errors in the mutex locking.
    *(dynamic_cast<std::ostream*>(this)) << "Encountered the following error while locking the mutex:\n";
    switch (mErrorLocking){
    case 0:
      *(dynamic_cast<std::ostream*>(this)) << "No error" << std::endl;
      break;
    case EINVAL:
      *(dynamic_cast<std::ostream*>(this)) << "EINVAL" << std::endl;
      break;
    case EDEADLK:
      *(dynamic_cast<std::ostream*>(this)) << "EDEADLK" << std::endl;
      break;
    default:
      *(dynamic_cast<std::ostream*>(this)) << "Unknown " << mErrorLocking << std::endl;
      break;
    }//switch (mErrorLocking)

    return mErrorLocking;
  }//Fed9UStream::getLockErrorNumber()


  int Fed9UStream::getUnlockErrorNumber() {
    //Perform a none thread safe write to the ostream.
    //Casts back to the inherited stream operator<< definition and hence bypasses the
    //class overloaded version of the operator<<, which provides the thread safety.
    //Neccessary as it is used to report errors in the mutex unlocking.
    *(dynamic_cast<std::ostream*>(this)) << "Encountered the following error while unlocking the mutex:\n";
    switch (mErrorUnlocking){
    case 0:
      *(dynamic_cast<std::ostream*>(this)) << "No error" << std::endl;
      break;
    case EINVAL:
      *(dynamic_cast<std::ostream*>(this)) << "EINVAL" << std::endl;
      break;
    case EPERM:
      *(dynamic_cast<std::ostream*>(this)) << "EPERM" << std::endl;
      break;
    default:
      *(dynamic_cast<std::ostream*>(this)) << "Unknown " << mErrorUnlocking << std::endl;
      break;
    }//switch (mErrorUnlocking)

    return mErrorUnlocking;
  }//Fed9UStream::getUnlockErrorNumber()


  Fed9UStream& Fed9UStream::setMutexErrorNumber(int errNum) {
    //Perform a none thread safe write to the ostream.
    //Casts back to the inherited stream operator<< definition and hence bypasses the
    //class overloaded version of the operator<<, which provides the thread safety.
    //Neccessary as it is used to report errors in the mutex.
    *(dynamic_cast<std::ostream*>(this)) << "The following pthread mutex error was sent to stream:\n";
    switch (errNum){
    case 0:
      *(dynamic_cast<std::ostream*>(this)) << "No error" << std::endl;
      break;
    case EINVAL:
      *(dynamic_cast<std::ostream*>(this)) << "EINVAL" << std::endl;
      break;
    case EDEADLK:
      *(dynamic_cast<std::ostream*>(this)) << "EDEADLK" << std::endl;
      break;
    case EBUSY:
      *(dynamic_cast<std::ostream*>(this)) << "EBUSY" << std::endl;
      break;
    case ENOMEM:
      *(dynamic_cast<std::ostream*>(this)) << "ENOMEM" << std::endl;
      break;
    default:
      *(dynamic_cast<std::ostream*>(this)) << "Unknown " << mErrorLocking << std::endl;
      break;
    }//switch (mErrorLocking)

    return *this;
  }//Fed9UStream::setMutexErrorNumber


  Fed9UStream& Fed9UStream::operator<<(std::ostream& (*pf)(std::ostream&)) {
    //std::cout << FED9U_FUNCTION << std::endl;
    *(dynamic_cast<std::ostream*>(this)) << pf;
    return *this;
  }//operator<<(std::ostream& (*pf)(std::ostream&))


  Fed9UStream& Fed9UStream::operator<<(std::ios_base& (&pf)(std::ios_base&)) {
    //std::cout << FED9U_FUNCTION << std::endl;
    *(dynamic_cast<std::ostream*>(this)) << pf;
    return *this;
  }//operator<<(std::ios_base& (*pf)(std::ios_base&))

  //Initialise the debug, error and log level.
  template<> Fed9UDebugLevel Fed9U::Fed9UMessage<Fed9UDebugLevel>::smLevel( FED9U_DEBUG_LEVEL_MINIMAL );
  template<> Fed9UErrorLevel Fed9U::Fed9UMessage<Fed9UErrorLevel>::smLevel( FED9U_ERROR_LEVEL_WARNING );
  template<> Fed9ULogLevel Fed9U::Fed9UMessage<Fed9ULogLevel>::smLevel( FED9U_LOG_LEVEL_NONE );

  //<GJR date=26/11/2006>
  //Don't need this any more as we create the log file on the fly, when it is needed.
//   //Create the log.
//    Fed9ULog log("Fed9U.log");
  //</GJR>
  //Create the gFed9UOut stream, point it to cout.
  Fed9UStream gFed9UOut(&std::cout);

  //Create the gFed9UErr stream, point it to cerr.
  Fed9UStream gFed9UErr(&std::cerr);

  //<GJR date=26/11/2006>
  //Create the Fed9U log file with an empty string, hence no file is created and it is just pointed at std::cerr.
  Fed9UStream gFed9ULog(std::string(""));

}//namespace Fed9U
