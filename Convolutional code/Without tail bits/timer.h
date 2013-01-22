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
  Timer(float timeToExpire = 0);

  // Return the string of the current date and time.
  // getCurrentTime() dynamically allocates a string and returns, be cautious
  char *getStartTime(void);
  char *getCurrentTime(void);
  int getElapsedTime(void);

  // Call hasExpired() to check if the timer expires.
  // Return true if the timer expires. Otherwise false is returned.
  bool hasExpired(void);

  // Restart the timer.
  void restart(void);

  // Change the time to expires.
  // Param : 
  //   timeToEpire : New expiration period.
  void resetPeriod(float timeToEpire);

private:

   // The expiration period.
   float _timeToExpire;

   char _startTimeString[32];
   
   // The time the timer was set.
   time_t _startTime;
};

#endif
