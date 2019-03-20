/*
 * DiplayServer.h
 *
 *  Created on: 18.03.2019
 *      Author: Pandel
 */

#ifndef SOCKETSERVER_DISPLAYSERVER_H_
#define SOCKETSERVER_DISPLAYSERVER_H_
// INCLUDE SYSTEM
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
// INCLUDE PROGRAM

// namespace
using namespace std;
// defines
#define PIPENAME "/tmp/alarmpipe"
// externals
extern void *DisplayTask(void *value);
// threads
extern pthread_t displaytask;
// typedefs
typedef struct {
   string systemstate;
   string gsmstate;
   string bme680state;
   string socketclientstate;
   string serialrelaisstate;
   string xbeestate;
   string alarmstate;
   string credit;
   string rssi;
   string temperature;
   string pressure;
   string humidity;
   string line1;
   string line2;
   string line3;
   string line4;
} s_displayvalue;

#define SYSSTATE     1
#define GSMSTATE     2
#define BMESTATE     3
#define SOCKSTATE    4
#define RELAISSTATE  5
#define XBEESTATE    6
#define ALARMSTATE   7
#define CREDITSTATE  8
#define RSSISTATE    9
#define TEMPSTATE    10
#define PRESSTATE    11
#define HUMISTATE    12
#define LINE1STATE   13
#define LINE2STATE   14
#define LINE3STATE   15
#define LINE4STATE   16

//classes
class DisplayServer
{
public:
    s_displayvalue val;
    void SetVal(int field,string value);
    DisplayServer();
    virtual ~DisplayServer();
private:

};

#endif /* SOCKETSERVER_DISPLAYSERVER_H_ */
