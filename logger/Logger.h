/*
 * Logger.h
 *
 *  Created on: Nov 17, 2018
 *      Author: Stephan
 */

#ifndef LOGGER_LOGGER_H_
#define LOGGER_LOGGER_H_
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
using namespace std;
class Logger
{
public:
    Logger();
    virtual ~Logger();
};
public:
   static TLogLevel& ReportingLevel();
   static Log();
   static Log(const string& sMessage);

protected:
   std::ostringstream os;
private:
   Log(const Log&);
   Log& operator =(const Log&);

   /**
   *   Log file name.
   **/
   static const std::string logfileName;
   /**
   *   Singleton logger class object pointer.
   **/
   static Logger* logger;
   /**
   *   Log file stream object.
   **/
   static ofstream logfile;
};

#endif /* LOGGER_LOGGER_H_ */
