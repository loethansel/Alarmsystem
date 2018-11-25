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
Logger* Logger::instance = NULL;
Logger* Logger::Instance()
{
    if (!instance)   // Only allow one instance of class to be generated.
        instance = new Logger;
    return instance;
}
Logger::Logger() : active(false)
{
    instance->minPriority = INFO;
}

void Logger::Start(Priority minPriority, const string& logFile)
{
    instance->active = true;
    instance->minPriority = minPriority;
    if (logFile != "")
    {
        instance->fileStream.open(logFile.c_str(), ofstream::trunc);
    }
}

void Logger::Stop()
{
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

void Logger::Write(Priority priority, const string& message)
{
    if(instance->active && priority >= instance->minPriority)
    {
        mutex m;

        m.lock();
        // identify current output stream
        ostream& stream = instance->fileStream.is_open() ? instance->fileStream : cout;

        stream  << currentDateTime() 
                << " [" 
                << PRIORITY_NAMES[priority]
                << "]" 
                << ":\t"
                << message
                << endl;

        m.unlock();
    }
}
