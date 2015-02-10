#ifndef H_Fed9UWait
#define H_Fed9UWait

namespace Fed9U {

  /**
   * \brief Waits for a number of microseconds before returning.
   * \param microsec Number of microseconds to waits for. Defaulted to zero.
   */
  void fed9Uwait(unsigned long microsec = 0);

  /**
   * \brief Waits for a time period specified in seconds and microseconds.
   * \param seconds Number of seconds to wait for.
   * \param microsec Number of microseconds to wait for.
   */
  void fed9Uwait(unsigned long seconds, unsigned long microsec);
  
}

#endif // H_Fed9UWait
