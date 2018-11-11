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
FONA FONA1;


   if(!FONA1.Power_On()) {
       cout << "Fona startet nicht!" << endl;
       pthread_exit(NULL);
   }
   output_evt = 0;
   while(1) {
       if(sendsms) {
           sendsms = false;
           if(FONA1.SendSms()) FONA1.CreditCheck();
           else cout << "Fona Fehler beim Senden SMS!" << endl;
       }
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt + LIVE_TIMER) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          if(!FONA1.LiveCheck()) { cout << "Fona liefert kein Acknowledge!" << endl; }
          else { cout << "Fona ist AT+OK!" << endl; }
          if(!FONA1.RxLevelCheck()) { cout << "Fona hat keinen GSM-Empfang!" << endl; }
          else { cout << "Fona Empfang!" << endl; }
       }
       // if(FONA1.IsRunning()) FONA1.Power_Off();
       //else FONA1.Power_On();
   }
   pthread_exit(NULL);
}



