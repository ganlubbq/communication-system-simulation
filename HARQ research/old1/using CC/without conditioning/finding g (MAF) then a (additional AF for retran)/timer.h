#ifndef TIMER_H
#define TIMER_H

/*
 * This file defines the timer used in proxy.
 *
 */

#include <ctime>

class Timer
{
public:

  // Construct the Timer. 
  Timer(double timeToExpire = 0);

  // Print out the current date and time on the screen
  void printStartTime(void);
  void printCurrentTime(void);
  void printElapsedTime(void);

  // Return the string of the current date and time.
  // Remember to call delete [] after use
  char *getStartTime(void);
  char *getCurrentTime(void);
  char *getElapsedTime(void);

  // Call hasExpired() to check if the timer expires.
  // Return true if the timer expires. Otherwise false is returned.
  bool hasExpired(void);

  // Restart the timer.
  void restart(void);

  // Change the time to expires.
  // Param : 
  //   timeToEpire : New expiration period.
  void resetPeriod(double timeToEpire);

private:
   void printTime(time_t time);

   char *getTheTime(time_t time);

   // The expiration period.
   double _timeToExpire;

   // The time the timer was set.
   time_t _startTime;
};

#endif
