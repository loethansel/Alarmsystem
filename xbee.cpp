/*
 * xbee.cpp
 *
 *  Created on: 13.11.2018
 *      Author: linux
 */

#include "xbee.h"

using namespace std;
using namespace BlackLib;


XBEE::XBEE() : BlackUART(UART4,Baud38400,ParityNo,StopOne,Char8)
{
	// TODO Auto-generated constructor stub

}

XBEE::~XBEE() {
	// TODO Auto-generated destructor stub
}

