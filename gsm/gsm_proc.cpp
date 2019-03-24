/*
 * gsm_proc.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: Pandel
 *
 *       */
// INCLUDE SYSTEM
#include <iomanip>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
// INCLUDE PROGRAM
#include "fona.h"
#include "Adafruit_FONA.h"
#include "../alarmsys.h"
#include "../files/ctrlfile.h"
#include "gsm_proc.h"
#include "../timer/EmaTimer.h"
#include "../alarmsys.h"
#include "../logger/logger.h"
#include "../email/email.h"
#include "../socketclient/ThingSpeak.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;
using namespace logger;
// FUNCTION DECLARATIONS
// HANDLER
void rssi_handler(union sigval arg);
void live_handler(union sigval arg);
void info_handler(union sigval arg);
void gsm_handler(union sigval arg);
// GLOBAL VARS
// THREADS
pthread_t gsmtask;
// CLASSES
Adafruit_FONA FONA1;
EmaTimer  rssitimer(rssi_handler);
EmaTimer  livetimer(live_handler);
EmaTimer  gsmtimer(gsm_handler);
EmaTimer  gsminfotimer(info_handler);


void GetCompCredit(void)
{
stringstream ss;
string s;
int i;
int len;
string hstr1, hstr2;
bool retval;

    retval = FONA1.CreditCheck();
    if(!retval) {
        Logger::Write(Logger::ERROR,"fona get and compare credit check failed");
        return;
    }
    display->SetVal(CREDITSTATE,string(FONA1.credit_aschar));
    tspeak->setval(CREDITFIELD,FONA1.credit_numeric);
    ss.str(""); ss.clear();
    ss << FONA1.credit_aschar;
    // get length
    hstr1 = ss.str();
    hstr2 = ctrlfile->ini.GSM.creditwarnlevel;
    len = static_cast<int>(hstr1.size());
    // trim string to 00.00
    for(i=0;i<len;i++) {
        if(!((hstr1[i] >= '0' && hstr1[i] <= '9') || (hstr1[i] == '.'))) hstr1[i] = '0';
        if(!((hstr2[i] >= '0' && hstr2[i] <= '9') || (hstr2[i] == '.'))) hstr2[i] = '0';
    }
    // compare credit level
    try {
       if(stof(hstr1) < stof(hstr2)) {
           Logger::Write(Logger::INFO,"fona credit is low level, please charge your card");
           ss.str(""); ss.clear();
           ss << "card credit is low: " << hstr1 << endl;
           emailalarm->send(SERVICEMAIL,ss.str());
       }
    } catch(const exception& e) { cout << "catched exception creditcomp: " << e.what() << endl; }
}

// Interval Timer Handler (usually every hour)
void info_handler(union sigval arg)
{
stringstream ss;
char buff[255];

    FONA1.getSIMCCID(buff);
    ss.str(""); ss.clear();
    ss << "fona = "
       << "alive: "  << (FONA1.fonalive?"ok":"bad")  << "; "
       << "ccid:"    << buff <<"; "
       << "rx: "     << (FONA1.fonarssi?"ok":"bad")  << "; "
       << "net:"     << (FONA1.fonanet?"ok":"bad")   << "; "
       << "rssi: "   << tostr(FONA1.rxpegel_numeric) << "; "
       << "credit: " << FONA1.credit_aschar          << ";";
    Logger::Write(Logger::INFO,ss.str());
    gsminfotimer.StartTimer();
}


// Interval Timer Handler (usually every hour)
void rssi_handler(union sigval arg)
{
static int rssifailcnt = 0;
bool retval;
stringstream ss;

   retval = FONA1.RxLevelCheck();
   if(!retval) {
       // cout << "Fona hat keinen GSM-Empfang!" << endl;
       // if there is for trhree minutes no live in it => reset an email
       if(++rssifailcnt > stoi(ctrlfile->ini.GSM.rssideadtime)) {
           FONA1.begin();
           Logger::Write(Logger::ERROR,"rssi-error: fona can't get network signal!");
           display->SetVal(RSSISTATE,"0");
           rssifailcnt = 0;
       }
   }
   else {
       ss.str(""); ss.clear();
       ss << FONA1.rxpegel_aschar;
       display->SetVal(RSSISTATE,ss.str());
       tspeak->setval(RSSIFIELD,static_cast<float>(FONA1.rxpegel_numeric));
   }
   rssitimer.StartTimer();
}

// Interval LiveTimer Handler
// check AT+OK every minute
void live_handler(union sigval arg)
{
bool retval;
static int  livefailcnt = 0;
static bool firstcheck = false;

   retval = FONA1.LiveCheck(1000);
   if(!retval) {
       Logger::Write(Logger::ERROR,"fona did not acknowledge the livecheck!");
       cout << "Fona liefert kein Acknowledge during LiveCheck!" << endl;
       // if there is for five minutes no live in it => reset an email
       if(++livefailcnt > stoi(ctrlfile->ini.GSM.livedeadtime)) {
           FONA1.begin();
           Logger::Write(Logger::ERROR,"livecheck-error: fona restarted!");
           livefailcnt = 0;
           // message to display
           display->SetVal(GSMSTATE,"liveerror");
       }
   }
   else {
       livefailcnt = 0;
       if(!firstcheck) {
          Logger::Write(Logger::INFO,"fona first credit check after power up");
          GetCompCredit();
          gsminfotimer.StartTimer();
          firstcheck = true;
       }
       cout << "fona is AT+OK!" << endl;
       // message to display
       display->SetVal(GSMSTATE,"running");
   }
   livetimer.StartTimer();
}


// Interval Timer Handler
void gsm_handler(union sigval arg)
{
static bool status      = false;
int numbercnt;
stringstream ss;
string        s;
int i;

    // armed-signal from mainthread, read files on every going "armed"
    if(armed && !status) {
        // read credit
        if(FONA1.fonarssi) {
            cout << "do first fona credit check after armed" << endl;
            Logger::Write(Logger::INFO,"fona credit check after armed");
            GetCompCredit();
            status = true;
        }
    }
    else if(!armed) status = false;
    // send-sms-signal from mainthread
    if(sendsms) {
        // if live and rri send the email if not wait .....
        if(FONA1.fonarssi && FONA1.fonalive) {
            try { numbercnt = stoi(ctrlfile->ini.TEL_NUM.numbercnt); }
            catch(const exception& e) { cout << "catched exception gsmhandler: " << e.what() << endl; }
            for(i=0;i<numbercnt;i++) {
                ss.str("");
                ss.clear();
                ss << ctrlfile->ini.ALARM.alarmtext;
                s = ss.str();
                if(FONA1.SendSms(ctrlfile->ini.TEL_NUM.number[i],s)) {
                    GetCompCredit();
                    FONA1.deleteSMS_all();
                }
                else {
                    Logger::Write(Logger::ERROR,"SMS-error: fona error during send sms!");
                    cout << "fona error during SMS sending!" << endl;
                }
            }
        }
        sendsms = false;
    }
    gsmtimer.StartTimer();
}

// GSMTASK
void *GsmTask(void *value)
{
static bool simlocked = true;
char simbuff[5];
char imeibuff[20];
stringstream ss;

    // start fona on power up
   if(!FONA1.begin()) {
       Logger::Write(Logger::ERROR,"poweron-Error: fona did not boot");
       cout << "poweron-error: Fona startet nicht!" << endl;
       Logger::Write(Logger::ERROR,"alarmsystem not armed, no buzzer");
       cout << "poweron-error: alarmsystem not armed, no buzzer" << endl;
   } else {
       // if simpin is set try to unlock
       if(!FONA1.isSIMunlocked()) {
           ctrlfile->ini.GSM.simpin.copy(simbuff,5,0);
           if(FONA1.unlockSIM(&simbuff[0])) {
               simlocked = false;
               Logger::Write(Logger::INFO,"fona sim unlocked");
           }
           else {
               simlocked = true;
               Logger::Write(Logger::ERROR,"fona can't unlock sim");
           }
       } else {
           simlocked = false;
           Logger::Write(Logger::INFO,"fona sim unlocked");       }
       // start timer if sim is unlocked
       if(!simlocked) {
          Logger::Write(Logger::INFO,"fona powered on");
          FONA1.getIMEI(&imeibuff[0]);
          ss << "fona imei: " << imeibuff;
          Logger::Write(Logger::INFO,ss.str());
          try {
          rssitimer.Create_Timer(0,stoi(ctrlfile->ini.GSM.rssitimer)*60);
          rssitimer.StartTimer();
          livetimer.Create_Timer(0,stoi(ctrlfile->ini.GSM.livetimer)*60);
          livetimer.StartTimer();
          gsmtimer.Create_Timer(0,GSMINTERVAL);
          gsmtimer.StartTimer();
          gsminfotimer.Create_Timer(0,stoi(ctrlfile->ini.ALARM.infotime)*60);
          } catch(const exception& e) { cout << "catched exception gsmproc: " << e.what() << endl; }
          // message to display
          display->SetVal(GSMSTATE,"running");
          Logger::Write(Logger::INFO,"fona timer init");
       }
   }
   // LOOP
   while(1) {
       // INTERES SIGNAL PRGRAM END!
       if(program_end) break;
       sleep(1);
   }
   pthread_exit(NULL);
}



