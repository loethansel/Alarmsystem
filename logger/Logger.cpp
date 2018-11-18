/*
 * Logger.cpp
 *
 *  Created on: Nov 17, 2018
 *      Author: linux
 */

#include "Logger.h"
#include "Logger.h"
#include "Logger.h"
#include"Utilities.h"
using namespace std;
namespace Logging
{
const string logfileName = "Log.txt";
Logger* logger = NULL;
ofstream Logger::;

Logger* Logger::Logger()
{
    if (logger == NULL)
    {
        logger = new Logger();
        logfile.open(logfileName.c_str(), ios::out | ios::app);
    }
    return logger;
}

void Logger::Log(const char * format, ...)
{
    char* sMessage = NULL;
    int nLength = 0;
    va_list args;
    va_start(args, format);
    nLength = _vscprintf(format, args) + 1;
    sMessage = new char[nLength];
    vsprintf_s(sMessage, nLength, format, args);
    logfile << Util::CurrentDateTime() << ":\t";
    logfile << sMessage << "\n";
    va_end(args);

    delete[] sMessage;
}

void Logger::Log(const string& sMessage)
{
    logfile << Util::CurrentDateTime() << ":\t";
    logfile << sMessage << "\n";
}

Logger& Logger::operator<<(const string& sMessage)
{
    logfile << "\n" << Util::CurrentDateTime() << ":\t";
    logfile << sMessage << "\n";
    return *this;
}

Logger::~Logger()
{
    // TODO Auto-generated destructor stub
}

}
