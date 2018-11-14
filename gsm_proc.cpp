/*
 * gsm_proc.cpp
 *
 *  Created on: Nov 9, 2018
 *      Author: linux
 */
#include <pthread.h>
#include "gsm_proc.h"
#include "fona.h"
#include "alarmsys.h"

using namespace std;
using namespace BlackLib;

pthread_t gsmtask;

void *GsmTask(void *value)
{
static clock_t output_evt,tmeas_now;
static int livetimer   = 0;
static int rssitimer   = 0;
static int livefailcnt = 0;
static int rssifailcnt = 0;
FONA FONA1;


   if(!FONA1.Power_On()) {
       cout << "Fona startet nicht!" << endl;
       cout << "Alarmanlage nicht scharf schalten, kein Buzzer!" << endl;
       pthread_exit(NULL);
   }
   output_evt = 0;
   while(1) {
	   // second clock for time dependent functions
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          // check live unconditionally
          if(FONA1.poweredon) livetimer++;
          // check rssi only when live is true
          if(FONA1.fonalive)  rssitimer++;
       }
       if(sendsms) {
           // TODO: send email too

    	   // if live and rri send the email if not wait .....
           if(FONA1.fonarssi && FONA1.fonalive) {
              if(FONA1.SendSms()) FONA1.CreditCheck();
              else cout << "Fona Fehler beim Senden SMS!" << endl;
              sendsms = false;
           }
       }
       // check AT+OK every minute
       if(livetimer >= LIVE_TIMER) {
    	  livetimer = 0;
          if(!FONA1.LiveCheck()) {
        	  cout << "Fona liefert kein Acknowledge!" << endl;
        	  // if there is for five minutes no live in it => reset an email
        	  if(++livefailcnt > (MAX_DEAD_LIVETIME/LIVE_TIMER)) {
        		  FONA1.Power_On();
        		  // TODO: send only one email to admin
        		  livefailcnt = 0;
        	  }
          }
          else { livefailcnt = 0; cout << "Fona ist AT+OK!" << endl; }
       }
       // check network quality every minute
       if(rssitimer >= RSSI_TIMER) {
    	   rssitimer = 0;
           if(!FONA1.RxLevelCheck()) {
        	   cout << "Fona hat keinen GSM-Empfang!" << endl;
        	   // if there is for trhree minutes no live in it => reset an email
        	   if(++rssifailcnt > (MAX_DEAD_RSSITIME/RSSI_TIMER)) {
         		  FONA1.Power_On();
         		  // TODO: send only one email to admin
         		  rssifailcnt = 0;
        	   }
           }
           else { cout << "Fona Empfang!" << endl; }
       }
   }
   pthread_exit(NULL);
}



