/*
 * xbeeproc.cpp
 *
 *  Created on: Dec 22, 2018
 *      Author: Pandel
 */
// INCLUDES
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#include "../logger/logger.h"
#include "../timer/EmaTimer.h"
#include "../alarmsys.h"
#include "../files/ctrlfile.h"
#include "XBee.h"
#include "xbeeproc.h"
// NAMESPACES
using namespace std;
using namespace logger;
// FOREWARD
void Xbee_handler(union sigval arg);
void XbeeSwitchOn_handler(union sigval arg);
void XbeeSwitchOff_handler(union sigval arg);
void XbeeAlarm(uint8_t nr);
void XbeeSetupSend(s_xbee *frame, bool setclr);
// CLASSES
//XBee  xbee = XBee();
XBee     xbee;
EmaTimer xbeeswitchontimer(XbeeSwitchOn_handler);
EmaTimer xbeeswitchofftimer(XbeeSwitchOff_handler);
EmaTimer xbeetimer(Xbee_handler);
// SH + SL Address of receiving XBee
// Steckdose
XBeeAddress64 addr64; // = XBeeAddress64(0x7CB03EAA, 0x00B239EA);
// Lampe
//XBeeAddress64 addr642 = XBeeAddress64(0x00178801, 0x02FE942A);
// request / response classes
ZBExplicitTxRequest     zbetx;
ZBExplicitRxResponse    zbetx_rp;
ZBTxRequest             zbtx;
ZBTxStatusResponse      zbtx_rp;
AtCommandRequest        acr;
AtCommandResponse       arc_sp;
RemoteAtCommandRequest  racr;
RemoteAtCommandResponse racr_rp;

// TASKS
pthread_t xbeetask;
// GLOBALS
bool xbeeblocked;
uint8_t payload_on[]  = { 0x01, 0x00, 0x01, 0x00, 0x10 };
uint8_t payload_off[] = { 0x01, 0x00, 0x00, 0x00, 0x10 };


//ZBExplicitTxRequest zbTx = ZBExplicitTxRequest(addr642, 0xFFFE,0x00,0x00,payload_on , sizeof(payload_on),  0x01, 0xE8, 0x0B, 0x0006, 0x0104);
//ZBExplicitTxRequest zbTx4 = ZBExplicitTxRequest(addr642, 0xFFFE,0x00,0x00,payload_off, sizeof(payload_off), 0x01, 0xE8, 0x0B, 0x0006, 0x0104);
//ZBTxRequest   zbTx1   = ZBTxRequest(addr64, payload_on, sizeof(payload_on));
//ZBTxRequest   zbTx2   = ZBTxRequest(addr64, payload_off, sizeof(payload_off));
//ZBTxStatusResponse txStatus = ZBTxStatusResponse();

ZBExplicitRxResponse txStatus = ZBExplicitRxResponse();


//----------------------------------------------------------
// XBEESWITCHOFF_HANDLER (1h cycl)
//----------------------------------------------------------
void XbeeSwitchOff_handler(union sigval arg)
{
    /*
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].name     = "steckdose";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].addr64   = "7CB03EAA00B239EA";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].addr16   = "FFFE";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].framet   = "11";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].destend  = "03";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].cluster  = "0006";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].profile  = "0104";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].payload1 = "0100010010";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].payload2 = "0100000010";
*/
    XbeeSetupSend(&CTRLFILE->ini.XBEE[XBEE_TIMESW1],CLR);
}

//----------------------------------------------------------
// XBEESWITCHON_HANDLER (1h cycl)
//----------------------------------------------------------
void XbeeSwitchOn_handler(union sigval arg)
{
/*
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].name     = "steckdose";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].addr64   = "7CB03EAA00B239EA";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].addr16   = "FFFE";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].framet   = "11";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].destend  = "03";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].cluster  = "0006";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].profile  = "0104";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].payload1 = "0100010010";
    CTRLFILE->ini.XBEE[XBEE_TIMESW1].payload2 = "0100000010";
*/
    XbeeSetupSend(&CTRLFILE->ini.XBEE[XBEE_TIMESW1],SET);
    xbeeswitchontimer.StartTimer();
    xbeeswitchofftimer.StartTimer();
}

//----------------------------------------------------------
// XBEEALARM
//----------------------------------------------------------
void XbeeAlarm(uint8_t nr)
{
}

//----------------------------------------------------------
// XBEE_HANDLER
//----------------------------------------------------------
void Xbee_handler(union sigval arg)
{
   // TODO: Check Time an start stop intervaltimer
    if(alarmactive && armed) {
        if(!xbeeblocked) {
            XbeeAlarm(0);
            xbeeblocked = true;
        }
    } else {
        XbeeAlarm(1);
        xbeeblocked = false;
    }
}

//----------------------------------------------------------
// XBEESETUPSEND
//----------------------------------------------------------
void XbeeCkeckResponse(unsigned int frametype)
{
    switch(frametype) {
       // AT Command Request
       case AT_COMMAND_REQUEST:
       break;
       // Tx Command Request
       case ZB_TX_REQUEST:
       break;
       // Explizit Addressing Command Frame
       case ZB_EXPLICIT_TX_REQUEST:
           // response
           if(xbee.readPacket(500)) {
              // got a response!
              // should be a znet tx status
              // should be a znet tx status
              if(xbee.getResponse().getApiId() == ZB_EXPLICIT_RX_RESPONSE) {
                 xbee.getResponse().getZBExplicitRxResponse(txStatus);
                 // get the delivery status, the fifth byte
                 if(txStatus.isError() == SUCCESS) {
                     Logger::Write(Logger::INFO,"xbee send on frame success");
                     cout << "XBEE: send on frame" << endl;
                 }
                 else {
                     Logger::Write(Logger::ERROR,"xbee error send on frame");
                     cout << "XBEE: error sending..." << endl;
                 }
               } else {
                   Logger::Write(Logger::ERROR,"xbee no response");
                   cout << "XBEE: getting no response" << endl;
               }
           }
       break;
       // Remote AT Command Request
       case REMOTE_AT_REQUEST:
       break;
    }
}


//----------------------------------------------------------
// XBEESETUPSEND
//----------------------------------------------------------
void XbeeSetupSend(s_xbee *frame, bool setclr)
{
unsigned int frametype;
uint64_t     addr_64;
uint16_t     addr_16;
uint8_t      broadcast = 0x00;
uint8_t      option    = 0x00;
uint8_t      frameid   = 0x01;
uint8_t      src_end   = 0xE8;
uint8_t      dst_end   = 0x03;
uint16_t     cluster   = 0x0006;
uint16_t     profile   = 0x0104;
char         hexbuf[2];
char         charbuff[255];
uint8_t      payload[255];
uint8_t      *loadptr;
uint8_t      loadlen;
uint8_t      i,j;


    frametype = stoul(frame->framet,nullptr,16);
    addr_64   = stoull(frame->addr64,nullptr,16);
    addr_16   = stoull(frame->addr16,nullptr,16);
    dst_end   = stoul(frame->destend,nullptr,16);
    cluster   = stoul(frame->cluster,nullptr,16);
    profile   = stoul(frame->profile,nullptr,16);
    loadlen   = (setclr==SET)?frame->payload1.length():frame->payload2.length();
    (setclr==SET)?frame->payload1.copy(charbuff,loadlen):frame->payload2.copy(charbuff,loadlen);
    loadlen /= 2;
    j = 0;
    for(i=0;i<loadlen;i++) {
        hexbuf[0] = charbuff[j++];
        hexbuf[1] = charbuff[j++];
        payload[i] = stoi(hexbuf,nullptr,16);
    }
    loadptr   = payload;
    switch(frametype) {
       // AT Command Request
       case AT_COMMAND_REQUEST:
       break;
       // Tx Command Request
       case ZB_TX_REQUEST:
       break;
       // Explizit Addressing Command Frame
       case ZB_EXPLICIT_TX_REQUEST:
         zbetx.setAddress64(XBeeAddress64(addr_64));
         zbetx.setAddress16(addr_16);
         zbetx.setBroadcastRadius(broadcast);
         zbetx.setFrameId(frameid);
         zbetx.setOption(option);
         zbetx.setSrcEndpoint(src_end);
         zbetx.setDstEndpoint(dst_end);
         zbetx.setClusterId(cluster);
         zbetx.setProfileId(profile);
         zbetx.setPayload(loadptr);
         zbetx.setPayloadLength(loadlen);
         xbee.send(zbetx);
         XbeeCkeckResponse(frametype);
       break;
       // Remote AT Command Request
       case REMOTE_AT_REQUEST:
       break;
    }
}

//----------------------------------------------------------
// XBeeTASK
//----------------------------------------------------------
void *XbeeTask(void *value)
{
   // start xbee on power up
   if(!xbee.begin()) {
       Logger::Write(Logger::ERROR,"poweron-Error: xbee uart open failure");
       cout << "poweron-error: xbee startet nicht!" << endl;
       xbeeblocked = true;
   } else {
       Logger::Write(Logger::INFO,"xbee powered on");
       xbeeswitchontimer.Create_Timer(0x00,5);
       xbeeswitchontimer.StartTimer();
       xbeeswitchofftimer.Create_Timer(0x00,2);
       // xbee mainloop cycle
       xbeetimer.Create_Timer(100,0x00);
       xbeetimer.StartTimer();
   }
   // blockerinit
   xbeeblocked = false;
   // LOOP
   while(1) {
       // INTERES SIGNAL PRGRAM END!
       if(program_end) break;
       sleep(1);
   }
   pthread_exit(NULL);
}
