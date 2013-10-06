// timer.cpp
// This file implements timer.h

#include "timer.h"

#include <iostream>
#include <ctime>
#include <string>
#include <cstring>

using namespace std;

Timer::Timer(double interval)
{
   // Save current time, give this value to _startTime
   time(&_startTime);

   // Record our wished expiration period
   _timeToExpire = interval;
}

void Timer::printTime(time_t _Time)
{ 
    // Get the current date and time using the "tm" structure.
    struct tm *TimeP = localtime(&_Time);
    
    int year = (TimeP->tm_year) + 1900;
    int mon = (TimeP->tm_mon) + 1;
    int day = (TimeP->tm_mday);
    int hour = (TimeP->tm_hour);
    int min = (TimeP->tm_min);
    int sec = (TimeP->tm_sec);
    
    cout << year << "/" << mon << "/" << day << " ";
    cout << hour << ":" << min << ":" << sec;
}

char *Timer::getTheTime(time_t _Time)
{
    // Get the current date and time using the "tm" structure.
    struct tm *TimeP = localtime(&_Time);
    
    int year = (TimeP->tm_year) + 1900;
    int mon = (TimeP->tm_mon) + 1;
    int day = (TimeP->tm_mday);
    int hour = (TimeP->tm_hour);
    int min = (TimeP->tm_min);
    int sec = (TimeP->tm_sec);

    char yearStr[10] = "";
    char monStr[10] = "";
    char dayStr[10] = "";
    char hourStr[10] = "";
    char minStr[10] = "";
    char secStr[10] = "";

    itoa(year,yearStr,10);
    itoa(mon,monStr,10);
    itoa(day,dayStr,10);
    itoa(hour,hourStr,10);
    itoa(min,minStr,10);
    itoa(sec,secStr,10);
    
    // Stupid~~
    string temp = "";
    temp.append(yearStr);
    temp.append("/");
    temp.append(monStr);
    temp.append("/");
    temp.append(dayStr);
    temp.append(" ");
    temp.append(hourStr);
    temp.append(":");
    temp.append(minStr);
    temp.append(":");
    temp.append(secStr);
    
    char *timeStr = new char [60];
    strcpy(timeStr,temp.c_str());
    
    return timeStr;
}


void Timer::printStartTime(void)
{
   printTime(_startTime);
}

void Timer::printCurrentTime(void)
{
   time_t currentTime;

   time(&currentTime);
   
   printTime(currentTime);
}

void Timer::printElapsedTime(void)
{
   time_t currentTime;

   time(&currentTime);
   
   cout << currentTime - _startTime << " sec";
}


char *Timer::getStartTime(void)
{
   getTheTime(_startTime);
}

char *Timer::getCurrentTime(void)
{
   time_t currentTime;

   time(&currentTime);
   
   getTheTime(currentTime);
}

char *Timer::getElapsedTime(void)
{
   time_t currentTime;

   time(&currentTime);
   
   int diff = currentTime - _startTime;
   
   char diffStr[10] = "";
   itoa(diff,diffStr,10);
   string temp = "";
   temp.append(diffStr);
   temp.append(" sec");
   
    char *timeStr = new char [10];
    strcpy(timeStr,temp.c_str());
    
   return timeStr;
}


bool Timer::hasExpired(void)
{
   time_t currentTime;

   // Record the current time
   time(&currentTime);

   if((currentTime - _startTime) > _timeToExpire)
   {
      return true;
   }
   else
   {
      return false;
   }
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

