#include <ctime>
#include <cerrno>
#include <iostream>

#include "Fed9UWait.hh"

namespace Fed9U {

  /** wait
   * wait takes number of microseconds to wait for.
   *
   * matt noy, 05/2002
   * Emlyn Corrin 09/2003
   */
  
  // modified by m.noy on 11-02-2003 to remove the printf()
  // in order to use with c++ under rh linux 7.2
  
  // modified by m.noy 12-02-2003 to use microseconds instead
  // and the arguments changed to type unsigned long

  // modified by E. Corrin 18/09/2003 to remove the seconds
  // argument and just take microseconds, to return
  // immediately if a wait of zero is requested, and
  // to continue the wait if it is interrupted before
  // completing.
  void fed9Uwait(unsigned long microsec) {
    if (microsec == 0) return;
    timespec requested;
    timespec remaining;
    requested.tv_sec  =  microsec / 1000000; // set the time structure
    requested.tv_nsec = (microsec % 1000000) * 1000;
    remaining.tv_sec = 1;
    remaining.tv_nsec = 2;
    if (nanosleep(&requested, &remaining) == -1) {
      std::cerr << "!! fed9Uwait(" << microsec << "), s="
		<< (microsec / 1000000) << ", ns=" << ((microsec % 1000000) * 1000)
		<< (errno == EINTR ? ", Interrupted. " : ", Error. ")
		<< "rem = " << remaining.tv_sec << ", " << remaining.tv_nsec << std::endl;
      //      if (errno == EINTR) {
      //requested = remaining;
      //} else {
      //break;
      //}
    }
  }

  void fed9Uwait(unsigned long seconds, unsigned long microsec) {
    fed9Uwait(seconds*1000000+microsec);
  }

}

