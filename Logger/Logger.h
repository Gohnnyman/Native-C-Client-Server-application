#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>
#include <fstream>

class Logger
{
private:
    static constexpr uint16_t buffer_size = 4096;
    std::ofstream logfile;
    char* logfilename;
    time_t rawtime;
    struct tm* timeinfo;
    char* buffer = new char[buffer_size];

public:
    Logger(char* logfilename);
    void log(const char* buffer);
    char* getlocaltime(const struct tm *timeptr) const;
    ~Logger();
};

#endif // LOGGER_H