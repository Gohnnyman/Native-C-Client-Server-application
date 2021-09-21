#include "Logger.h"
#include <fstream>
#include <iostream>
#include <time.h>
#include <cstring>
#include <mutex>

std::mutex mtx;
std::ofstream Logger::logfile;
char* Logger::logfilename;
time_t Logger::rawtime;
struct tm* Logger::timeinfo;
char* Logger::buffer = new char[Logger::buffer_size];



void Logger::clear(const char* logfilename)
{
  mtx.lock();
  logfile.open(logfilename, std::ios::out | std::ios::trunc);
  logfile.close();
  mtx.unlock();
}

void Logger::log(const char* buffer, const char* prefix, const char* logfilename)
{
  mtx.lock();
  logfile.open(logfilename, std::fstream::app);

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  sprintf(Logger::buffer, "[%s]: %s%s", getlocaltime(timeinfo), prefix, buffer);

  logfile << Logger::buffer << '\n';
  logfile.flush();
  logfile.close();
  mtx.unlock();
}

char* Logger::getlocaltime(const struct tm *timeptr)
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