#include "logger.h"

#include <mutex>

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
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

void Logger::Write(Priority priority, const string& message, const char* str, const char* file )
{
    if (!instance){
        return;
    }
    if(instance->active && priority >= instance->minPriority)
    {
        mutex m;
        string filename(file);

        m.lock();
        // identify current output stream
        ostream& stream = instance->fileStream.is_open() ? instance->fileStream : cout;

        stream  << currentDateTime() 
                << " [" 
                << PRIORITY_NAMES[priority]
                << "] ["
                << filename <<"::" << str << "]"
                << ":\t"
                << message
                << endl;
        cout  << currentDateTime()
                        << " ["
                        << PRIORITY_NAMES[priority]
                        << "] ["
                        << filename <<"::" << str << "]"
                        << ":\t"
                        << message
                        << endl;
        m.unlock();
    }
}
