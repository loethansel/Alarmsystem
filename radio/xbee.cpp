/*
 * xbee.cpp
 *
 *  Created on: 13.11.2018
 *      Author: Ralf Pandel
 */
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
char writeArrOff[] = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x82, 0x59, 0xD5, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x04, 0xC6 };
char writeArrOn[]  = { 0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x13, 0xA2, 0x00, 0x41, 0x82, 0x59, 0xD5, 0xFF, 0xFE, 0x02, 0x44, 0x34, 0x05, 0xC5 };

    flush(bothDirection);
    length = 20;
    if(state == true) {
        if(!write(writeArrOn,length))  return false;
    } else {
        if(!write(writeArrOff,length)) return false;
    }
   return true;
}


xbee::~xbee() {
    // Close Uart4
	close();
}

