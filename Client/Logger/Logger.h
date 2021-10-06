#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>
#include <fstream>

class Logger
{
private:
    static constexpr uint16_t buffer_size = 4096;
    static std::ofstream logfile;
    static char* logfilename;
    static time_t rawtime;
    static char* buffer;
    static struct tm* timeinfo;

public:
    static void log(const char* buffer, const char* prefix = "", const char* logfilename = "log.txt");
    static void clear(const char* logfilename = "log.txt");
    static char* getlocaltime(const struct tm *timeptr);
};



#endif // LOGGER_H