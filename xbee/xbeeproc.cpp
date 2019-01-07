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
void XbeeSetupSend(s_xbee *frame, bool setclr);
void XBeeSwitch(uint8_t device,bool setclr);
// CLASSES
XBee     xbee;
EmaTimer xbeeswitchontimer(XbeeSwitchOn_handler);
EmaTimer xbeeswitchofftimer(XbeeSwitchOff_handler);
EmaTimer xbeetimer(Xbee_handler);
XBeeAddress64 addr64;
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
bool timerstarted = false;
bool xbee_ok      = false;

//----------------------------------------------------------
// READ REMOTEAT STATUS RESPONSE
//----------------------------------------------------------
bool readRtAtRp(void)
{
    // read tx_status_response
    if(!xbee.readPacket(500)) goto noresponse;
    if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
      xbee.getResponse().getRemoteAtCommandResponse(racr_rp);
      if (racr_rp.isOk())
      {
          return true;
      }
      else {
          Logger::Write(Logger::ERROR,"xbee no remote at delivery success");
          cout << "xbee no delivery success" << endl;
          return false;
      }
    }
noresponse:
   Logger::Write(Logger::ERROR,"xbee no response");
   cout << "xbee no response" << endl;
   return false;
}

//----------------------------------------------------------
// READ TX STATUS RESPONSE
//----------------------------------------------------------
bool readTxRp(void)
{
    // read tx_status_response
    if(!xbee.readPacket(500)) goto noresponse;
    // got a response!
    if(xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
       xbee.getResponse().getZBTxStatusResponse(zbtx_rp);
       if(zbtx_rp.getDeliveryStatus() == SUCCESS)
       {
          return true;
       }
       else {
           Logger::Write(Logger::ERROR,"xbee no tx delivery success");
           cout << "xbee no tx delivery success" << endl;
           return false;
       }
    }
    return true;
noresponse:
    Logger::Write(Logger::ERROR,"xbee no response");
    cout << "xbee no response" << endl;
    return false;
}

//----------------------------------------------------------
// READ TXE STATUS RESPONSE
//----------------------------------------------------------
bool readTxeRp(void)
{
    // read tx_status_response
    if(!xbee.readPacket(500)) goto noresponse;
    // got a response!
    if(xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
       xbee.getResponse().getZBTxStatusResponse(zbtx_rp);
       if(zbtx_rp.getDeliveryStatus() == SUCCESS) {
          if(!xbee.readPacket(500)) goto noresponse;
          if(xbee.getResponse().getApiId() == ZB_EXPLICIT_RX_RESPONSE)
             {
                return true;
             }
       }
       else {
           Logger::Write(Logger::ERROR,"xbee no explicit delivery success");
           cout << "xbee no explicit delivery success" << endl;
           return false;
       }
    }
    return true;
noresponse:
    Logger::Write(Logger::ERROR,"xbee no response");
    cout << "xbee no response" << endl;
    return false;
}

//----------------------------------------------------------
// XBEESETUPSEND
//----------------------------------------------------------
void XbeeCkeckResponse(unsigned int frametype)
{
    switch(frametype) {
       // AT Command Request
       case AT_COMMAND_REQUEST:
           // TODO:
       break;
       // Tx Command Request
       case ZB_TX_REQUEST:
           readTxRp();
       break;
       // Explizit Addressing Command Frame
       case ZB_EXPLICIT_TX_REQUEST:
           // read tx_status_response
           readTxeRp();
       break;
       // Remote AT Command Request
       case REMOTE_AT_REQUEST:
           readRtAtRp();
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
string       s;
stringstream ss;

    // preparing frame data
    frametype = stoul(frame->framet,nullptr,16);
    addr_64   = stoull(frame->addr64,nullptr,16);
    if(addr_64 == 0x00) return;
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
    // preparing logmessage
    ss.str("");
    ss << "xbee tx:"  << frame->name    << "; "
       << "addr64:"   << frame->addr64  << "; "
       << "addr16:"   << frame->addr16  << "; "
       << "framet:"   << frame->framet  << "; "
       << "destend:"  << frame->destend << "; "
       << "cluster:"  << frame->cluster << "; "
       << "profile:"  << frame->profile << "; "
       << "payload:"  << ((setclr==SET)?frame->payload1:frame->payload2);
    s = ss.str();
    // send frame
    switch(frametype) {
       // AT Command Request
       case AT_COMMAND_REQUEST:
           // TODO:
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
         xbee.flush(BOTH);
         Logger::Write(Logger::INFO,s);
         xbee.send(zbetx);
         XbeeCkeckResponse(frametype);
       break;
       // Remote AT Command Request
       case REMOTE_AT_REQUEST:
         addr64 = XBeeAddress64(addr_64);
         racr.setRemoteAddress64(addr64);
         racr.setFrameId(frameid);
         racr.setRemoteAddress16(addr_16);
         racr.setCommand(&payload[0]);
         racr.setCommandValue(&payload[2]);
         racr.setCommandValueLength(loadlen-2);
         racr.setApplyChanges(true);
         xbee.flush(BOTH);
         Logger::Write(Logger::INFO,s);
         xbee.send(racr);
         XbeeCkeckResponse(frametype);
         racr.clearCommandValue();
       break;
    }
}

//----------------------------------------------------------
// XBEESWITCHOFF_HANDLER (1h cycl)
//----------------------------------------------------------
void XbeeSwitchOff_handler(union sigval arg)
{
    XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW1],CLR);
    usleep(50000);
    XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW2],CLR);
}

//----------------------------------------------------------
// XBEESWITCHON_HANDLER (1h cycl)
//----------------------------------------------------------
void XbeeSwitchOn_handler(union sigval arg)
{
    XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW1],SET);
    usleep(50000);
    XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW2],SET);
    if(timerstarted) xbeeswitchontimer.StartTimer();
    xbeeswitchofftimer.StartTimer();
}

//----------------------------------------------------------
// XBEE_HANDLER
//----------------------------------------------------------
void Xbee_handler(union sigval arg)
{
struct timeval tmnow;
struct tm *tm;
int onhour, offhour;

    // interval switch
    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    onhour  = stoi(ctrlfile->ini.TIMESW.onhour);
    offhour = stoi(ctrlfile->ini.TIMESW.offhour);
    if((tm->tm_hour >= onhour) || (tm->tm_hour < offhour)) {
       if(!timerstarted && armed) {
          timerstarted = true;
          xbeeswitchontimer.StartTimer();
       }
    } else if(timerstarted) {
        timerstarted = false;
        xbeeswitchontimer.StopTimer();
    }
    xbeetimer.StartTimer();
}

void XBeeSwitch(uint8_t device,bool setclr)
{
    if(!xbee_ok) return;
    switch(device) {
       case XBEEALARM:
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT2],setclr);
       break;
       case XBEETIME:
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW2],setclr);
       break;
       case XBEEONOFF:
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF2],setclr);
       break;
       case XBEEALL:
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT1],setclr);
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT2],setclr);
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW1],setclr);
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW2],setclr);
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF1],setclr);
          XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF2],setclr);
       break;
       default:
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ALROUT2],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_TIMESW2],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF1],setclr);
           XbeeSetupSend(&ctrlfile->ini.XBEE[XBEE_ON_OFF2],setclr);
       break;
    }

}


//----------------------------------------------------------
// XBeeTASK
//----------------------------------------------------------
void *XbeeTask(void *value)
{
int cyclesecs, onsecs;

   // start xbee on power up
   if(!xbee.begin()) {
       Logger::Write(Logger::ERROR,"poweron-Error: xbee uart open failure");
       cout << "poweron-error: xbee startet nicht!" << endl;
       xbee_ok     = false;
   } else {
       xbee_ok = true;
       Logger::Write(Logger::INFO,"xbee powered on");
       // switchon every hour
       cyclesecs  = stoi(ctrlfile->ini.TIMESW.cyclesecs);
       xbeeswitchontimer.Create_Timer(0x00,cyclesecs);
       // switch off after 900 sec
       onsecs     = stoi(ctrlfile->ini.TIMESW.onsecs);
       xbeeswitchofftimer.Create_Timer(0x00,onsecs);
       // xbee mainloop cycle
       xbeetimer.Create_Timer(0,1);
       xbeetimer.StartTimer();
       XBeeSwitch(XBEEALARM,CLR);
       XBeeSwitch(XBEETIME,CLR);
       XBeeSwitch(XBEEONOFF,SET);
   }
   // LOOP
   while(1) {
       // INTERES SIGNAL PRGRAM END!
       if(program_end) break;
       sleep(1);
   }
   pthread_exit(NULL);
}
