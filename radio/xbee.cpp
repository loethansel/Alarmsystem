/*
 * xbee.cpp
 *
 *  Created on: 13.11.2018
 *      Author: Ralf Pandel
 */
#include <mutex>
#include "xbee.h"
#include "../logger/logger.h"
//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;


xbee::xbee() : BlackUART(UART4,Baud9600,ParityNo,StopOne,Char8)
{
    bool isOpened = open( BlackLib::ReadWrite | BlackLib::NonBlock );
    // check the open succeed
    if(!isOpened ) cout << "XBEE on UART4 can\'t open!" << endl;
    else {
        cout << "XBEE on UART4 is open...." << endl;
    }
}

// MACID: 0013A200418259D5
bool xbee::switch_xbee1(bool state)
{
size_t length;
string rbuff;
int i;
static char writeArrOn[20]   = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x83, 0xDF, 0xB6, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x05, 0x5D };
static char writeArrOff[20]  = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x83, 0xDF, 0xB6, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x04, 0x5E };

    close();
    open( BlackLib::ReadWrite | BlackLib::NonBlock );
    flush(bothDirection);
    length = 20;
    if(state == true) {
        if(!write(writeArrOn,length))  { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    } else {
        if(!write(writeArrOff,length)) { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    }
    sleep(1);
    return true;
error:
  close();
  return false;
}

// MACID: 0x7CB03EAA00B08A30 (Lampe-Wozi)
bool xbee::switch_xbee2(bool state)
{
size_t length;
string rbuff;
int i;
static char writeArrOn[29]  = { 0x7E, 0x00, 0x19, 0x11, 0x01, 0x7C, 0xB0, 0x3E, 0xAA, 0x00, 0xB0, 0x8A, 0x30, 0xFF, 0xFE, 0xE8, 0x03, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x6A };
static char writeArrOff[29] = { 0x7E, 0x00, 0x19, 0x11, 0x01, 0x7C, 0xB0, 0x3E, 0xAA, 0x00, 0xB0, 0x8A, 0x30, 0xFF, 0xFE, 0xE8, 0x03, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x6B };

    close();
    open( BlackLib::ReadWrite | BlackLib::NonBlock );
    flush(bothDirection);
    length = 29;
    if(state == true) {
        if(!write(writeArrOn,length))  { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    } else {
        if(!write(writeArrOff,length)) { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    }
    sleep(1);
    return true;
error:
  close();
  return false;
}

// MACID: 0x7CB03EAA00B07F57 (Lampe-WC)
bool xbee::switch_xbee3(bool state)
{
size_t length;
string rbuff;
int i;
static char writeArrOn[29]  = { 0x7E, 0x00, 0x19, 0x11, 0x01, 0x7C, 0xB0, 0x3E, 0xAA, 0x00, 0xB0, 0x7F, 0x57, 0xFF, 0xFE, 0xE8, 0x03, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x4E };
static char writeArrOff[29] = { 0x7E, 0x00, 0x19, 0x11, 0x01, 0x7C, 0xB0, 0x3E, 0xAA, 0x00, 0xB0, 0x7F, 0x57, 0xFF, 0xFE, 0xE8, 0x03, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x4F };

    close();
    open( BlackLib::ReadWrite | BlackLib::NonBlock );
    flush(bothDirection);
    length = 29;
    if(state == true) {
        if(!write(writeArrOn,length))  { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    } else {
        if(!write(writeArrOff,length)) { goto error; }
        usleep(40000);
        if(!read(readArr,length))      { goto error; }
        rbuff = readArr;
    }
    sleep(1);
    //close();
    return true;
error:
  close();
   //mtx.unlock();
  return false;
}

// MACID: 0x7CB03EAA00B0AE7A (Lampe-Buero)



xbee::~xbee() {
    // Close Uart4
	close();
}

