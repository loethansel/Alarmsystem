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

bool xbee::switch_xbee(bool state)
{
size_t length;
string rbuff;
int i;
static char writeArrOff[20] = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x82, 0x59, 0xD5, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x04, 0xC6 };
static char writeArrOn[20]  = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x82, 0x59, 0xD5, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x05, 0xC5 };

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
    //close();
    return true;
error:
  close();
   //mtx.unlock();
  return false;
}


xbee::~xbee() {
    // Close Uart4
	close();
}

