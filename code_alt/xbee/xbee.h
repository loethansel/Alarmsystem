/*
 * xbee.h
 *
 *  Created on: 13.11.2018
 *      Author: linux
 */

#ifndef XBEE_H_
#define XBEE_H_
//#include "../blacklib/BlackLib.h"
//#include "../blacklib/BlackUART/BlackUART.h"


using namespace std;
using namespace BlackLib;


class xbee : public BlackUART
{
public:
	xbee();
    bool switch_xbee1(bool state);
    bool switch_xbee2(bool state);
    bool switch_xbee3(bool state);
	virtual ~xbee();
private:
    char  writeArr[255];
    char  readArr[255];
};

#endif /* XBEE_H_ */

// Halle Grau Steckdose: 84182600000FD57D

// --------------------------------------------------
// Configure Smart+
// Set all to default values
// press the button on the plugs till it clicks
// open XTCU and make the following changes
// KY ZS 2
// AP 1
// AO 3
// EE 1
// EO 2
// KY 5A6967426565416C6C69616E63653039
//
// TELEGRAM (Protocol ZIGBEE/API1)
// FrameType: 0x11
// FrameID  : 1
// Dest64   : MACID DEST
// Dest16   : 0xFFFE
// SE E8
// DE 01 GE
// DE 0A CREE
// DE 0B HUE
// DE 03 SMART+
// CI 0x0006 (payload for: 'on'/'off'/'toggle')
// on    : 0x01, 0x00, 0x01, 0x00, 0x10
// off   : 0x01, 0x00, 0x00, 0x00, 0x10
// toggle: 0x01, 0x00, 0x02, 0x00, 0x10
// CI 0x0008 (payload for: 0..255)
// value : 0x01, 0x00, 0x04, value, 0x10, 0x00, 0x10
// CI 0x0300 (payload for: 2700..255)
// value : 0x01, 0x00, 0x04, value, 0x10, 0x00, 0x10
// PI 0x0104
// Broadcast : 0
// Options   : 0
// PAYLOAD   : xx,xx,xx....
// Checksum
// ---
// Sources:
//https://github.com/ratmandu/node-red-contrib-zblight
//https://github.com/jimmo/home-automation
//https://github.com/bright-tools/xbeeapi
//https://github.com/andrewrapp/xbee-arduino
//https://www.digi.com/support/forum/61290/s2c-and-osram-lightify
//----------------------------------------------
// Configure XBee Remote AT (Alarmmelder FS)
// https://www.digi.com/blog/tag/xbee-pro/
// Switch    Coordinator to API-Mode AP=1
// Configure Remote D4=4
// Frame-Tool:
// MACID-Remote: 0013A200418259D5
// D4 AUS: 7E 00 10 17 01 00 13 A2 00 41 82 59 D5 FF FE 02 44 34 04 C6
// D4 AN : 7E 00 10 17 01 00 13 A2 00 41 82 59 D5 FF FE 02 44 34 04 C6
//----------------------------------------------
// Lampe (Hue): 0017880102FE942A
// AN : 7E 00 19 11 01 00 17 88 01 02 FE 94 2A FF FE E8 0B 00 06 01 04 00 00 01 00 01 00 10 82
// AUS: 7E 00 19 11 01 00 17 88 01 02 FE 94 2A FF FE E8 0B 00 06 01 04 00 00 01 00 00 00 10 83
//----------------------------------------------

