 /* ALARM TO EMAIL */
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"
#include <ctime>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
/*
#include "blacklib/examples/example_GPIO.h"
#include "blacklib/examples/example_ADC.h"
#include "blacklib/examples/example_PWM.h"
#include "blacklib/examples/example_SPI.h"
#include "blacklib/examples/example_UART.h"
#include "blacklib/examples/example_I2C.h"
#include "blacklib/examples/example_directory.h"
#include "blacklib/examples/example_threadAndMutex.h"
#include "blacklib/examples/example_time.h"
*/

//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
// GLOBAL Defines
//---------------------------------------------------------------------------
#define TRUE  1
#define FALSE 0
#define SEND_BLOCKTIME_SEC 60
#define DEBUG 1
//---------------------------------------------------------------------------
// GLOBAL Declarations
//---------------------------------------------------------------------------
ofstream    ofs;
bool program_end = FALSE;

//---------------------------------------------------------------------------
// function: DebugOut
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void DebugOut(const char *string,unsigned int value,unsigned char valout)
{
#if(DEBUG == 1)
   if(valout) {
       std::cout << string << std::hex << value << std::endl;
   } else {
       std::cout << string << std::endl;
   }
#endif
}

//---------------------------------------------------------------------------
// function: WriteLog
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void WriteLog(const char *string,unsigned int value,unsigned char valout)
{
// specifies the position of colon from the left
char         timestamp[22];
int          pos1;
time_t       now;
stringstream s,s1;
std::string  str;
#define DISTFROMLEFT 8

   now = time(0);
   strftime(timestamp, 22, "%d.%m.%Y-%H:%M:%S", localtime(&now));
   // formatting string "file   :"
   s << string;
   str = s.str();
   pos1 = str.find(':');
   if((pos1 < DISTFROMLEFT) && (pos1 > 0)) {
       str.insert(pos1,"        ",DISTFROMLEFT-pos1);
   }
   // Output to Logfile
   if(valout) {
       std::clog << timestamp << "  " << str << std::hex << value << std::endl;
   } else {
       std::clog << timestamp << "  " << str << std::endl;
   }
   // Output to Console
   s1 << timestamp << "  " << str;
   const std::string tmp = s1.str();
   const char* cstr      = tmp.c_str();
   DebugOut(cstr,value,valout);
}


//---------------------------------------------------------------------------
// EXIT Function
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void termination_handler(int sig)
{
   WriteLog("Main: Caught Signal: ",sig,TRUE);
   WriteLog("Main: End of Communication",0,FALSE);
   ofs.close();
   exit(0);
}

//---------------------------------------------------------------------------
// create_logfile Function
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void create_logfile(void)
{
char     timestamp[22];
time_t   now;
stringstream s;
#define LOGFILENAME "alrlog_"

   now = time(0);
   strftime(timestamp, 22, "%Y%m%d%H%M%S", localtime(&now));
   s << LOGFILENAME << timestamp << ".txt";
   ofs.open (string(s.str()));
   // Logfile Output umleiten
   clog.rdbuf(ofs.rdbuf());
}

//---------------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
int main()
{
struct sigaction action;
static clock_t output_evt,tmeas_now;
bool   outok;
bool   barrier = FALSE;
FILE   *fp = NULL;

    program_end = FALSE;
    // Set Termination Handler
    action.sa_handler = termination_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_NODEFER;
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGINT,  &action, NULL);
    // Logfile
    create_logfile();
    BlackLib::BlackGPIO   Opto1(BlackLib::GPIO_60,BlackLib::input);
    while(1) {
        //-----------------------------------------------------------
        // Sendesperre z.B. nicht mehr als einen Alarm/min. melden
        //-----------------------------------------------------------
        outok = FALSE;
        tmeas_now = clock() / CLOCKS_PER_SEC;
        if(tmeas_now >= (output_evt + SEND_BLOCKTIME_SEC) ) {
           output_evt = clock() / CLOCKS_PER_SEC;
           outok = TRUE;
        }
        /* Test
        if(outok) {
            WriteLog("AL_main: Alarm sending email!",0,FALSE);
            fp = popen("./mailer.sh","w");
            if (fp == NULL) cout << "Failed sending email" << endl;
            else            cout << "Successs sending email" << endl;
            pclose(fp);
        }
        */
        //-----------------------------------------------------------
        // Optokoppler Einlesen
        //-----------------------------------------------------------
        if(Opto1.getNumericValue() == FALSE) {
            if(!barrier && outok) {
                WriteLog("AL_main: Alarm!!!",0,FALSE);
                fp = popen("./mailer.sh","w");
                if (fp == NULL) cout << "Failed sending email" << endl;
                else            cout << "Successs sending email" << endl;
                pclose(fp);
            }
            barrier = TRUE;
        }
        else barrier = FALSE;
    }
    return 0;
}

/*

  /etc/ssmtp/ssmtp.conf


# echo "Test" | mail -s "Test" ralf@pandel.de
#--------------------------------------------------
# Config file for sSMTP sendmail
#
# The person who gets all mail for userids < 1000
# Make this empty to disable rewriting.
root=ralf@pandel.de

# The place where the mail goes. The actual machine name is required no
# MX records are consulted. Commonly mailhosts are named mail.domain.com
mailhub=smtp.strato.de:587
AuthUser=ralf@pandel.de
AuthPass=
UserTLS=YES
UseSTARTTLS=YES
AuthLogin=YES


# Where will the mail seem to come from?
rewriteDomain=pandel.de

# The full hostname
hostname=ralf@pandel.de

# Are users allowed to set their own From: address?
# YES - Allow the user to specify their own From: address
# NO - Use the system generated From: address
FromLineOverride=YES

--------------------------------------------------------
Script mailer.sh

#!/bin/bash
echo "Alarm" | mail -s "Alarm" ralf@pandel.de

*/

