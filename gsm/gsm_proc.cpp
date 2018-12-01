/*
 * gsm_proc.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: Pandel
 *
 *       */
// INCLUDES
#include "gsm_proc.h"
#include "../alarmsys.h"
#include "../logger/logger.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;
using namespace logger;
// THREADS
pthread_t gsmtask;

// GSMTASK
void *GsmTask(void *value)
{
static clock_t output_evt,tmeas_now;
static int  livetimer   = 0;
static int  rssitimer   = 0;
static int  livefailcnt = 0;
static int  rssifailcnt = 0;
static bool status      = false;
int numbercnt;
stringstream ss;
string        s;
int i;
FONA FONA1;

   // start fona on power up
   if(!FONA1.Power_On()) {
       Logger::Write(Logger::ERROR,"poweron-Error: fona did not boot");
       cout << "poweron-error: Fona startet nicht!" << endl;
       Logger::Write(Logger::INFO,"alarmsystem not armed, no buzzer");
       cout << "poweron-error: alarmsystem not armed, no buzzer" << endl;
   }
   output_evt = 0;
   // LOOP
   while(1) {
       // INTERES SIGNAL PRGRAM END!
       if(program_end) break;
	   // second clock for time dependent functions
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt+1) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          // check live unconditionally
          if(FONA1.poweredon) livetimer++;
          // check rssi only when live is true
          if(FONA1.fonalive)  rssitimer++;
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
              numbercnt = stoi(CTRLFILE->ini.TEL_NUM.numbercnt);
              for(i=0;i<numbercnt;i++) {
                 ss.clear();
                 ss << CTRLFILE->ini.ALARM.alarmtext;
                 s.clear();
                 s = ss.str();
                 if(FONA1.SendSms(CTRLFILE->ini.TEL_NUM.number[i],s)) {
                     FONA1.CreditCheck();
                     FONA1.deleteSMS_all();
                 }
                 else {
                     Logger::Write(Logger::ERROR,"SMS-error: fona error during send SMS!");
                     cout << "fona error during SMS sending!" << endl;
                 }
              }
              sendsms = false;
           }
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
        	   cout << "Fona hat keinen GSM-Empfang!" << endl;
        	   // if there is for trhree minutes no live in it => reset an email
        	   if(++rssifailcnt > (MAX_DEAD_RSSITIME/RSSI_TIMER)) {
         		  FONA1.Power_On();
                  Logger::Write(Logger::ERROR,"RSSI-Error: Fona hat keinen GSM-Empfang!");
         		  rssifailcnt = 0;
        	   }
           }
           else {
               cout << "Fona Empfang!" << endl;
           }
       }
   }
   pthread_exit(NULL);
}



