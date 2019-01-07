/*
 * gsm_proc.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: Pandel
 *
 *       */
// INCLUDES
#include <sys/time.h>
#include "gsm_proc.h"
#include "../alarmsys.h"
#include "../logger/logger.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;
using namespace logger;
// GLOBAL VARS

// THREADS
pthread_t gsmtask;
//FONA FONA1;
Adafruit_FONA FONA1;

// Interval Timer Handler
void gsm_handler(void)
{
static int  livetimer   = 0;
static int  rssitimer   = 0;
static int  livefailcnt = 0;
static int  rssifailcnt = 0;
static int  seccnt      = 0;
static bool status      = false;
int numbercnt;
stringstream ss;
string        s;
int i;
char buff[40];

    // check live unconditionally
    if(FONA1.poweredon) livetimer++;
    // check rssi only when live is true
    if(FONA1.fonalive)  rssitimer++;
    // Output to Logfile every hour
    if(seccnt++ >= INFOTIME) {
        FONA1.getSIMCCID(buff);
        ss.str("");
        ss << "fona = "
 	       << "alive: "  << (FONA1.fonalive?"ok":"bad")  << "; "
 	       << "ccid:"    << buff <<"; "
           << "rx: "     << (FONA1.fonarssi?"ok":"bad")  << "; "
           << "net:"     << (FONA1.fonanet?"ok":"bad")   << "; "
		   << "rssi: "   << tostr(FONA1.rxpegel_numeric) << "; "
		   << "credit: " << FONA1.credit_aschar          << ";";
        s = ss.str();
        Logger::Write(Logger::INFO,s);
        seccnt = 0;
    }
    // armed-signal from mainthread, read files on every going "armed"
    if(armed && !status) {
        // read credit
        if(FONA1.fonarssi) {
            cout << "do first fona credit check after armed" << endl;
            Logger::Write(Logger::INFO,"do first fona credit check after armed");
            FONA1.CreditCheck();
            status = true;
        }
    }
    else if(!armed) status = false;
    // send-sms-signal from mainthread
    if(sendsms) {
        // if live and rri send the email if not wait .....
        if(FONA1.fonarssi && FONA1.fonalive) {
            numbercnt = stoi(ctrlfile->ini.TEL_NUM.numbercnt);
            for(i=0;i<numbercnt;i++) {
                ss.str("");
                ss.clear();
                ss << ctrlfile->ini.ALARM.alarmtext;
                s = ss.str();
                if(FONA1.SendSms(ctrlfile->ini.TEL_NUM.number[i],s)) {
                    FONA1.CreditCheck();
                    FONA1.deleteSMS_all();
                }
                else {
                    Logger::Write(Logger::ERROR,"SMS-error: fona error during send SMS!");
                    cout << "fona error during SMS sending!" << endl;
                }
            }
        }
        sendsms = false;
    }
    // check AT+OK every minute
    if(livetimer >= LIVE_TIMER) {
        livetimer = 0;
        if(!FONA1.LiveCheck()) {
            Logger::Write(Logger::ERROR,"fona did not acknowledge the LiveCheck!");
	        cout << "Fona liefert kein Acknowledge during LiveCheck!" << endl;
	        // if there is for five minutes no live in it => reset an email
	        if(++livefailcnt > (MAX_DEAD_LIVETIME/LIVE_TIMER)) {
		        FONA1.Power_On();
		        Logger::Write(Logger::ERROR,"Live-Check-Error: Fona musste reanimiert werden!");
		        livefailcnt = 0;
            }
        }
        else { livefailcnt = 0; cout << "fona is AT+OK!" << endl; }
    }
    // check network quality every minute
    if(rssitimer >= RSSI_TIMER) {
        rssitimer = 0;
        if(!FONA1.RxLevelCheck()) {
	        // cout << "Fona hat keinen GSM-Empfang!" << endl;
	        // if there is for trhree minutes no live in it => reset an email
	        if(++rssifailcnt > (MAX_DEAD_RSSITIME/RSSI_TIMER)) {
		        FONA1.Power_On();
                Logger::Write(Logger::ERROR,"RSSI-Error: Fona hat keinen GSM-Empfang!");
		        rssifailcnt = 0;
	        }
        }
        else { /*cout << "Fona Empfang!" << endl;*/ }
    }
}

// GSMTASK
void *GsmTask(void *value)
{

    // start fona on power up
   if(!FONA1.begin()) {
// if(!FONA1.Power_On()) {
       Logger::Write(Logger::ERROR,"poweron-Error: fona did not boot");
       cout << "poweron-error: Fona startet nicht!" << endl;
       Logger::Write(Logger::ERROR,"alarmsystem not armed, no buzzer");
       cout << "poweron-error: alarmsystem not armed, no buzzer" << endl;
   } else {
       Logger::Write(Logger::INFO,"fona powered on");
   }
   // LOOP
   while(1) {
       // INTERES SIGNAL PRGRAM END!
       if(program_end) break;
       gsm_handler();
       sleep(1);
   }
   pthread_exit(NULL);
}



