// timer.cpp
// This file implements timer.h

#include "timer.h"

#include <iostream>
#include <cstring>

using namespace std;

Timer::Timer(double interval)
{
   // Save current time, give this value to _startTime
   time(&_startTime);

   strftime(_startTimeString, 32, "%Y-%m-%d %H:%M:%S", localtime(&_startTime));

   // Record our wished expiration period
   _timeToExpire = interval;
}

char *Timer::getStartTime(void)
{
   return _startTimeString;
}

char *Timer::getCurrentTime(void)
{
   time_t currentTime = time(0);
   
   char timeString[32];
   strftime(timeString, 32, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
   
   char *timeP = new char [32];
   strcpy(timeP,timeString);
   
   return timeP;
}

int Timer::getElapsedTime(void)
{
   time_t currentTime;

   time(&currentTime);
   
   return (currentTime - _startTime);
}

bool Timer::hasExpired(void)
{
   time_t currentTime;

   // Record the current time
   time(&currentTime);

   if((currentTime - _startTime) > _timeToExpire) { return true; }
   else { return false; }
}

void Timer::restart(void)
{
   // Change the value of _startTime so that it represents the
   // new time (usually the time right after the timer has expired)
   time(&_startTime);
}

void Timer::resetPeriod(double newPeriod)
{
   _timeToExpire = newPeriod;

   // SavedTime needs to be changed as well after someone has changed
   // expiration period.
   time(&_startTime);
}

