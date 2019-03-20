/*
 * DiplayServer.cpp
 *
 *  Created on: 18.03.2019
 *      Author: Pandel
 */
#include "DisplayServer.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../alarmsys.h"
#include "../timer/EmaTimer.h"
#include "../logger/logger.h"
#include "../alarmsys.h"
#include "SocketServer.h"

using namespace std;
using namespace logger;


// FOREWARD
void display_handler(union sigval arg);
// THREADS
pthread_t displaytask;
// CLASSES
EmaTimer displaytimer(display_handler);
SocketServer sserver(PIPENAME,00);

void display_handler(union sigval arg)
{
int erg;
stringstream ss_data;
string  valstring;
string  cmdstring[5];
char    chr[10];

    if(!sserver.isConnected) return;
    // receive command
    erg = recv(sserver.clientsocketfd, &chr, 9, 0);
   // chr[8] = 0;
    cmdstring[0] = "getvalue";
    cmdstring[1] = "xbeetest";
    // send data
    if(erg == -1) { } //     cout << "not_received_data_from_client" << endl;
    else if(erg == 0) {
        sserver.disconnectFromClient();
        Logger::Write(Logger::INFO,"client_shutdowned");
        cout << "client_shutdowned" << endl;
    }
    else {
        // cout << "received: "  << chr << endl;
        // get values
        if(cmdstring[0].compare(chr) == 0) {
            ss_data << "field1="  << display->val.systemstate  << "&"
                    << "field2="  << display->val.gsmstate << "&"
                    << "field3="  << display->val.bme680state << "&"
                    << "field4="  << display->val.socketclientstate << "&"
                    << "field5="  << display->val.serialrelaisstate << "&"
                    << "field6="  << display->val.xbeestate << "&"
                    << "field7="  << display->val.alarmstate << "&"
                    << "field8="  << display->val.credit << "&"
                    << "field9="  << display->val.rssi << "&"
                    << "field10=" << display->val.temperature << "&"
                    << "field11=" << display->val.pressure << "&"
                    << "field12=" << display->val.humidity << "&"
                    << "field13=" << display->val.line1 << "&"
                    << "field14=" << display->val.line2 << "&"
                    << "field15=" << display->val.line3 << "&"
                    << "field16=" << display->val.line4 << "&" << endl;
            int len = ss_data.str().length();
            // TODO: muss noch geändert werden in Class-function!!
            if(send(sserver.clientsocketfd,ss_data.str().c_str(),len, 0) == -1) sserver.disconnectFromClient();
        }
        // set armed
        else if(cmdstring[1].compare(chr) == 0) {
            xbeetest = true;
        }
    }
    displaytimer.StartTimer();
}

void DisplayServer::SetVal(int field, string value)
{
    if((field < 1) || (field > 16)) return;
    switch(field) {
       case 1:  val.systemstate       = value; break;
       case 2:  val.gsmstate          = value; break;
       case 3:  val.bme680state       = value; break;
       case 4:  val.socketclientstate = value; break;
       case 5:  val.serialrelaisstate = value; break;
       case 6:  val.xbeestate         = value; break;
       case 7:  val.alarmstate        = value; break;
       case 8:  val.credit            = value; break;
       case 9:  val.rssi              = value; break;
       case 10: val.temperature       = value; break;
       case 11: val.pressure          = value; break;
       case 12: val.humidity          = value; break;
       case 13: val.line1             = value; break;
       case 14: val.line2             = value; break;
       case 15: val.line3             = value; break;
       case 16: val.line4             = value; break;
       default: break;
    }
}


DisplayServer::DisplayServer()
{
    val.systemstate       = "--";
    val.gsmstate          = "--";
    val.bme680state       = "--";
    val.socketclientstate = "--";
    val.serialrelaisstate = "--";
    val.xbeestate         = "--";
    val.alarmstate        = "--";
    val.credit            = "0";
    val.rssi              = "0";
    val.temperature       = "0.00";
    val.pressure          = "0.00";
    val.humidity          = "0.00";
    val.line1             = "0.0000";
    val.line2             = "0.0000";
    val.line3             = "0.0000";
    val.line4             = "0.0000";
}

DisplayServer::~DisplayServer()
{
    // TODO Auto-generated destructor stub
}


//---------------------------------------------------------------------------
// DISPLAY
//---------------------------------------------------------------------------
void *DisplayTask(void *value)
{

   displaytimer.Create_Timer(0,1);
   displaytimer.StartTimer();

   while(1) {
        // INTERES SIGNAL PROGRAM END!!
        if(program_end) break;
        // wait blocking for client connection
        if(!sserver.isConnected) {
            sserver.connectToClient();
            displaytimer.StartTimer();
        }
        //display_handler();
        usleep(200);
    }
    sserver.disconnectFromClient();
    pthread_exit(NULL);
}
