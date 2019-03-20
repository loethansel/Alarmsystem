/*
 * ThingSpeak.cpp
 *
 *  Created on: 08.01.2019
 *      Author: Pandel
 */
// INCLUDES
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <stdlib.h>
#include "ThingSpeak.h"
#include "SocketClient.h"
#include "../alarmsys.h"
#include "../files/ctrlfile.h"
#include "../logger/logger.h"
// NAMESPACES
using namespace std;
using namespace logger;
// CLASSES
SocketClient sc("api.thingspeak.com",80);


ThingSpeak::ThingSpeak()
{
stringstream  ss;
int port;

    for(int i=0;i<8;i++) fieldval[i] = 0.00;
    this->writekey = ctrlfile->ini.TSPEAK.writekey;
    this->readkey  = ctrlfile->ini.TSPEAK.readkey;
    ss << "tspeak: host=" << ctrlfile->ini.TSPEAK.hostname
       << "; port=" << ctrlfile->ini.TSPEAK.port
       << "; wkey=" << this->writekey
       << "; rkey=" << this->readkey;
    ss.str(""); ss.clear();
    Logger::Write(Logger::INFO,ss.str());
    try { port = stoi(ctrlfile->ini.TSPEAK.port); }
    catch(const exception& e) { cout << "catched exception thingspeak: " << e.what() << endl; }
    sc.setServer(ctrlfile->ini.TSPEAK.hostname,port);
}

void ThingSpeak::setval(uint8_t field,float value)
{
    if((field < 1) || (field > 8)) return;
    fieldval[field-1] = value;
}

void ThingSpeak::pushall(void)
{
ostringstream ts_head, ts_data;
int len;
int retval;

   ts_data << "field1=" << fixed << setprecision(2) << fieldval[0] << "&"
           << "field2=" << fixed << setprecision(2) << fieldval[1] << "&"
           << "field3=" << fixed << setprecision(2) << fieldval[2] << "&"
           << "field4=" << fixed << setprecision(2) << fieldval[3] << "&"
           << "field5=" << fixed << setprecision(2) << fieldval[4] << "&"
           << "field6=" << fixed << setprecision(2) << fieldval[5] << "&"
           << "field7=" << fixed << setprecision(2) << fieldval[6] << "&"
           << "field8=" << fixed << setprecision(2) << fieldval[7] << endl;
#ifdef TSTEST
   stringstream ss;
   string s;
   char harr[255];
   ss      << "field1=" << fixed << setprecision(2) << fieldval[0] << "&"
           << "field2=" << fixed << setprecision(2) << fieldval[1] << "&"
           << "field3=" << fixed << setprecision(2) << fieldval[2] << "&"
           << "field4=" << fixed << setprecision(2) << fieldval[3] << "&"
           << "field5=" << fixed << setprecision(2) << fieldval[4] << "&"
           << "field6=" << fixed << setprecision(2) << fieldval[5] << "&"
           << "field7=" << fixed << setprecision(2) << fieldval[6] << "&"
           << "field8=" << fixed << setprecision(2) << fieldval[7] << endl;
   s = ss.str();
   s.copy(harr,s.length(),0);
#endif
   len = string(ts_data.str()).length();
   ts_head << "POST /update HTTP/1.1\n"
           << "Host:api.thingspeak.com\n"
           << "Connection: close\n"
           << "X-THINGSPEAKAPIKEY:"<< this->writekey << "\n"
           << "Content-Type: application/x-www-form-urlencoded\n"
           << "Content-Length:" << len << "\n\n";
   sc.connectToServer();
   sc.send(string(ts_head.str()));
   sc.send(string(ts_data.str()));
   string rec = sc.receive(1024);
   retval = rec.find("OK");
   if(retval != -1) {
       display->SetVal(SOCKSTATE,"datatransfer");
       Logger::Write(Logger::INFO,"thingspeak send data successful");
   }
   else {
       display->SetVal(SOCKSTATE,"error");
       Logger::Write(Logger::ERROR,"thingspeak send data fail");
   }
   sc.disconnectFromServer();
}


void ThingSpeak::pushout(uint16_t feld, float value)
{
ostringstream ts_head, ts_data;
stringstream ss;
string s;
int len;
int retval;

   setval(feld,value);
   ts_data << "field" << dec << feld << "=" << fixed << setprecision(2) << value << endl;
   len = string(ts_data.str()).length();
   ts_head << "POST /update HTTP/1.1\n"
           << "Host:api.thingspeak.com\n"
           << "Connection: close\n"
           << "X-THINGSPEAKAPIKEY:"<< this->writekey << "\n"
           << "Content-Type: application/x-www-form-urlencoded\n"
           << "Content-Length:" << len << "\n\n";
   sc.connectToServer();
   sc.send(string(ts_head.str()));
   sc.send(string(ts_data.str()));
   string rec = sc.receive(1024);
   retval = rec.find("OK");
   if(retval != -1) Logger::Write(Logger::INFO,"thingspeak send data successful");
   else Logger::Write(Logger::ERROR,"thingspeak send data fail");
   sc.disconnectFromServer();
}

ThingSpeak::~ThingSpeak()
{
    // TODO Auto-generated destructor stub
}

