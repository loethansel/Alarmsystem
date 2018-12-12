/*
 * xbee.h
 *
 *  Created on: 13.11.2018
 *      Author: linux
 */

#ifndef XBEE_H_
#define XBEE_H_
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackUART/BlackUART.h"


using namespace std;
using namespace BlackLib;

/*
 https://www.digi.com/blog/tag/xbee-pro/
 Switch Base to API-Mode AP=1
 Configure Remote D4=4
 Frame-Tool:
 MACID-Remote: 0013A200418259D5
 D4 AUS: 7E 00 10 17 01 00 13 A2 00 41 82 59 D5 FF FE 02 44 34 04 C6
 D4 AN : 7E 00 10 17 01 00 13 A2 00 41 82 59 D5 FF FE 02 44 34 04 C6
 */

class xbee : public BlackUART
{
public:
	xbee();
	bool switch_xbee(bool state);
	virtual ~xbee();
private:
    char  writeArr[255];
    char  readArr[255];
};

#endif /* XBEE_H_ */
//https://github.com/ratmandu/node-red-contrib-zblight
//https://github.com/jimmo/home-automation
//https://github.com/bright-tools/xbeeapi
//https://github.com/andrewrapp/xbee-arduino
//https://www.digi.com/support/forum/61290/s2c-and-osram-lightify
