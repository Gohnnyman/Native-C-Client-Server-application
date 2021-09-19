#include"Logger.h"
#include<fstream>
#include<iostream>
#include<time.h>
#include<cstring>
#include <mutex>

std::mutex mtx;


Logger::Logger(char* logfilename)
{
    this->logfilename = logfilename;
    logfile.open(logfilename, std::fstream::app);    
}

Logger::~Logger()
{
    logfile.close();
}


void Logger::log(const char* buffer)
{
    mtx.lock();
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strcpy(this->buffer, "[");
    sprintf(this->buffer, "[%s]: %s", getlocaltime(timeinfo), buffer);
    logfile << this->buffer;
    logfile.flush();
    mtx.unlock();
}

char* Logger::getlocaltime(const struct tm *timeptr) const
{
  static const char wday_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  static char result[26];
  sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
    wday_name[timeptr->tm_wday],
    mon_name[timeptr->tm_mon],
    timeptr->tm_mday, timeptr->tm_hour,
    timeptr->tm_min, timeptr->tm_sec,
    1900 + timeptr->tm_year);
  return result;
}