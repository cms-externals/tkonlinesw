#ifndef H_Fed9ULog
#define H_Fed9ULog

#include <fstream>
#include <string>

namespace Fed9U {

  //using.*std::string;

  /** 
   * \brief A class to log information to a file during a run of the program.
   * \author Emlyn Corrin
   * \version 0
   * \date 30 July 2003
   * 
   * Every time the program is run, instance of this class is automatically created,
   * called Fed9U::log, referring to the file Fed9U.log in the folder in which the
   * program was started. \n
   * On construction, the class looks for an existing log file of the specified name,
   * and if it exists, creates a backup by appending a tilde (~) to the file name,
   * before creating the new log file. The date and time are logged to the file
   * at startup, and at program termination if the program ends normally (e.g. not
   * via a core dump etc). \n
   * The class may be used like a standard ostream to send text, and values of
   * variables to the log file. \n
   * In addition, the log class provides a method stamp(), which inserts a timestamp
   * into the logfile, specifying the number of seconds, to the nearest millisecond
   * since the program started running. Note: this depends on the non-standard gettimeofday()
   * function available in linux, on other platforms this may need to be rewritten. \n
   * The output is unbuffered, so in the event of a crash all output already written
   * should be in the log file. \n
   * Example use:
\verbatim
#include "Fed9ULog.hh"

void somefunction() {
  log.stamp() << "Start of somefunction()" << endl;
  int x = something();
  log << "The value of x is " << x << endl;
}
\endverbatim
   * \n
   * There is a global instantiation of this class called log. It is declared at the end of this file
   * and the definition is in the file Fed9ULogTemplate.cc. Access to this log file is recommended to
   * be done through either a Fed9UMessage<Fed9ULog> or gFed9ULog object.
   */
  class Fed9ULog : public std::ofstream {

  public:

    /**
     * \brief Constructor.
     * \param filename Name of the log file.
     *
     * Opens the log file with the name that can be used for logging the output during the running of a program.
     */
    Fed9ULog(const char* filename);

    /**
     * \brief Destructor.
     *
     * Closes the log file, appending the date the file was closed.
     */
    ~Fed9ULog();

    /**
     * \brief  Writes the time to the file.
     * \return Self reference.
     */
    Fed9ULog& stamp();

    /**
     * \brief Called when an expected error causes the program to terminate.
     *
     * This write a mesage to the log and standard error out to indicate the program terminated unexpectedly.
     */
    static void new_unexpected();

    /**
     * \brief  Returns a string containing the time in seconds.
     * \return string
     */
    static std::string getTimeString();

    /**
     * \brief  Returns the date and time in the form day/month/year hour/minute/second.
     */
    static std::string getDateString();
  };

  //Can be removed as we now no longer require a file to be instantiated as it can be created on the fly.
//   /**
//    * \brief This is a global log file that is linked to the gFed9ULog object, which allows message levels to be set for the stream
//    *        and also will provide atomicity for its writes.
//    *
//    * Must be here in order to ensure that it is constructed before the gFed9ULog object.
//    */
//    extern Fed9ULog log;

}

#endif // H_Fed9ULog
