/*
 * ThingSpeak.cpp
 *
 *  Created on: 08.01.2019
 *      Author: Pandel
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "ThingSpeak.h"
#include "SocketClient.h"

using namespace std;

SocketClient sc("thingspeak.com",80);

// write: DR31KS856DPF1VJP
// read:  GK5NGDORRBPSOTYE

ThingSpeak::ThingSpeak()
{

}

void ThingSpeak::pushout(uint16_t feld, float value)
{
ostringstream ts_head, ts_data;
stringstream ss;

   cout << "Starting EBB Thing Speak Example" << endl;

   ss << "field" << dec << feld << "=" << value << endl;
   cout << ss.str() << endl;
//   ts_data << "field" << dec << feld << "=" << fixed << setprecision(3) << value << endl;
   ts_data << "field" << dec << feld << "=" << value << endl;
   ts_head << "POST /update HTTP/1.1\n"
           << "Host: api.thingspeak.com\n"
           << "Connection: close\n"
           << "X-THINGSPEAKAPIKEY: DR31KS856DPF1VJP\n"
           << "Content-Type: application/x-www-form-urlencoded\n"
           << "Content-Length:" << string(ts_data.str()).length() << "\n\n";
   sc.connectToServer();
   sc.send(string(ts_head.str()));
   sc.send(string(ts_data.str()));
   string rec = sc.receive(1024);
   cout << "[" << rec << "]" << endl;
   cout << "End of EBB Thing Speak Example" << endl;
}

ThingSpeak::~ThingSpeak()
{
    // TODO Auto-generated destructor stub
}

