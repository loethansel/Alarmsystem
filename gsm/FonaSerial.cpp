/*
 * FonaSerial.cpp
 *
 *  Created on: Jan 3, 2019
 *      Author: Pandel
 *
 */

#include <ctime>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "FonaSerial.h"
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackUART/BlackUART.h"
#include "../files/ctrlfile.h"
#include "../alarmsys.h"
#include "../logger/logger.h"
#include "../blacklib/BlackErr.h"


using namespace std;
using namespace BlackLib;
#ifndef TARGET
BlackUART  serialfona(UART2,Baud115200,ParityNo,StopOne,Char8);
#else
BlackUART  serialfona(UART5,Baud115200,ParityNo,StopOne,Char8);
#endif

FonaSerial::FonaSerial()
{
    // TODO Auto-generated constructor stub
}

bool FonaSerial::isopen()
{
    return serialfona.isOpen();
}


bool FonaSerial::serialopen()
{
    bool isOpened = serialfona.open(BlackLib::ReadWrite | BlackLib::NonBlock);
    // check the open succeed
    if(!isOpened ) cout << "fona monster on " << serialfona.getPortName() << " can\'t open!" << endl;
    else {
        cout << "fona monster on " << serialfona.getPortName() << " is open.." << endl;
        return true;
    }
    return false;
}

bool FonaSerial::serialclose()
{
    serialfona.close();
    return true;
}

void FonaSerial::flush()
{
    serialfona.flush(bothDirection);
}

char FonaSerial::read()
{
static char readval;

    serialfona.read(&readval,1);
    return readval;
}

bool FonaSerial::read(char *readbuff, int len)
{
    if(serialfona.read(readbuff,len)) return true;
    else return false;
}


bool FonaSerial::write(uint8_t val)
{
char writeval;

    writeval = static_cast<char>(val);
    return serialfona.write(&writeval,1);
}

bool FonaSerial::write(char *text, uint8_t len)
{
char hchar;
uint16_t i;

    serialfona.flush(bothDirection);
    for(i=0;i<len;i++) {
       hchar = static_cast<char>(*text++);
       serialfona.write(&hchar,1);
    }
    return true;
}

bool FonaSerial::write(const uint8_t *val, uint16_t len)
{
char hchar;
uint16_t i;

    serialfona.flush(bothDirection);
    for(i=0;i<len;i++) {
       hchar = static_cast<char>(*val++);
       serialfona.write(&hchar,1);
    }
    return true;
}

bool FonaSerial::write(const char *text, uint8_t len)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << text;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,len)) return true;
    else return false;
}

bool FonaSerial::print(const char *text)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << text;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length)) return true;
    else return false;
}

bool FonaSerial::print(char *text)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << text;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length)) return true;
    else return false;
}

bool FonaSerial::print(uint16_t val)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << std::dec << val;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length)) return true;
    else return false;
}

bool FonaSerial::print(int32_t val)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << std::dec << val;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length)) return true;
    else return false;
}

bool FonaSerial::print(uint32_t val)
{
stringstream ss;
string s;
size_t length;

    ss.clear(); ss.str("");
    ss << std::dec << val;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length)) return true;
    else return false;
}

bool FonaSerial::println(int32_t val,uint8_t dechex)
{
stringstream ss;
string s;
size_t length;
char ret[] = "\r";

    ss.clear(); ss.str("");
    if      (dechex == 10) ss << std::dec << val;
    else if (dechex == 16) ss << std::hex << val;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length) && serialfona.write(ret,1)) return true;
    else return false;
}

bool FonaSerial::println(const char *text)
{
stringstream ss;
string s;
size_t length;
char ret[] = "\r";

    ss.clear(); ss.str("");
    ss << text;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length) && serialfona.write(ret,1)) return true;
    else return false;
}

bool FonaSerial::println(char *text)
{
stringstream ss;
string s;
size_t length;
char ret[] = "\r";

    ss.clear(); ss.str("");
    ss << text;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length) && serialfona.write(ret,1)) return true;
    else return false;
}

bool FonaSerial::println(uint8_t val)
{
stringstream ss;
string s;
size_t length;
char ret[] = "\r";

    ss.clear(); ss.str("");
    ss << std::dec << val;
    s = ss.str();
    length = s.copy(writearr,s.size(),0);
    writearr[length] = '\0';
    serialfona.flush(bothDirection);
    if(serialfona.write(writearr,length) && serialfona.write(ret,1)) return true;
    else return false;
}

bool FonaSerial::println()
{
char ret[] = "\r";

    if(serialfona.write(ret,1)) return true;
    else return false;
}

bool FonaSerial::available()
{
    if(serialfona.fail(BlackLib::BlackUART::readErr)) return false;
    else return true;
}

bool FonaSerial::peek()
{
return true;
}

FonaSerial::~FonaSerial()
{
    // TODO Auto-generated destructor stub
}

