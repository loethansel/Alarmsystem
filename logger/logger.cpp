#include "logger.h"

#include <mutex>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>


using namespace std;
using namespace logger;


// --------------------------------------
// function implementations
// --------------------------------------
const string Logger::PRIORITY_NAMES[] =
{
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};
Logger* Logger::Instance()
{
    if (!instance){   // Only allow one instance of class to be generated.
        instance = new Logger;
    }
    return instance;
}
Logger* Logger::instance = Logger::Instance();

Logger::Logger() : active(false)
{
    if (instance)
    instance->minPriority = INFO;
}

void Logger::Start(Priority minPriority, const string& logFile)
{
    if (instance)
    instance->active = true;
    instance->minPriority = minPriority;
    if (logFile != "")
    {
        instance->fileStream.open(logFile.c_str(), ofstream::app);
    }
}

void Logger::Stop()
{
    if (instance)
    instance->active = false;
    if (instance->fileStream.is_open())
    {
        instance->fileStream.close();
    }
}
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string Logger::currentDateTime() {
    struct timeval tmnow;
    struct tm *tm;
    char buf[30], usec_buf[6];
    gettimeofday(&tmnow, NULL);
    int milli = (int)tmnow.tv_usec / 1000;
    tm = localtime(&tmnow.tv_sec);
    strftime(buf,30,"%Y-%m-%d %X", tm);
    strcat(buf,".");
    sprintf(usec_buf,"%03d",milli);
    strcat(buf,usec_buf);

    return buf;
}

void Logger::Write(Priority priority, const string& message, const char* str, const char* file )
{
    if (!instance){
        return;
    }
    if(instance->active && priority >= instance->minPriority)
    {
        mutex  m;
        stringstream ss;
        string filename(file);
        int    len, i;

        m.lock();
        ss.str(""); ss.clear();
        ss << currentDateTime()
           << " ["
           << PRIORITY_NAMES[priority]
           << "] ["
           << filename <<"::" << str << "]"
           << ":";
        ss.seekg(0, ios::end);
        len = ss.tellg();
        for(i=len;i<HEADERLEN;i++) ss << " ";
        ss << message << endl;
        // identify current output stream
        ostream& stream = instance->fileStream.is_open() ? instance->fileStream : cout;
        stream << ss.str();
        m.unlock();
    }
}
