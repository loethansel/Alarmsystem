 /* ALARM TO EMAIL */
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"
#include "blacklib/BlackGPIO/BlackGPIO.h"
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
// OUTPUTS
BlackLib::BlackGPIO  *out_1;
BlackLib::BlackGPIO  *out_2;
BlackLib::BlackGPIO  *out_3;
BlackLib::BlackGPIO  *out_4;
BlackLib::BlackGPIO  *out_5;
BlackLib::BlackGPIO  *out_6;
BlackLib::BlackGPIO  *out_7;
BlackLib::BlackGPIO  *out_8;
// INPUTS
BlackLib::BlackGPIO  *in_1;
BlackLib::BlackGPIO  *in_2;
BlackLib::BlackGPIO  *in_3;
BlackLib::BlackGPIO  *in_4;
BlackLib::BlackGPIO  *in_5;
BlackLib::BlackGPIO  *in_6;
BlackLib::BlackGPIO  *in_7;
BlackLib::BlackGPIO  *in_8;
BlackLib::BlackGPIO  *in_9;

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
   // free and unexport the output gpio's
   out_1->~BlackGPIO();
   out_2->~BlackGPIO();
   out_3->~BlackGPIO();
   out_4->~BlackGPIO();
   out_5->~BlackGPIO();
   out_6->~BlackGPIO();
   out_7->~BlackGPIO();
   out_8->~BlackGPIO();
   WriteLog("Main: Output GPIO's unexported",0,FALSE);
   // free and unexport the input gpio's
   in_1->~BlackGPIO();
   in_2->~BlackGPIO();
   in_3->~BlackGPIO();
   in_4->~BlackGPIO();
   in_5->~BlackGPIO();
   in_6->~BlackGPIO();
   in_7->~BlackGPIO();
   in_8->~BlackGPIO();
   in_9->~BlackGPIO();
   WriteLog("Main: Input GPIO's unexported",0,FALSE);
   WriteLog("Main: close logfile..........",sig,TRUE);

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

void init_hardware(void)
{   // Outputs
    WriteLog("Main: Output GPIO's exported to /sys/class/gpio",0,FALSE);
    out_1 = new BlackLib::BlackGPIO(BlackLib::GPIO_60 ,BlackLib::output,BlackLib::FastMode); // P9.12
    out_2 = new BlackLib::BlackGPIO(BlackLib::GPIO_50 ,BlackLib::output,BlackLib::FastMode); // P9.14
    out_3 = new BlackLib::BlackGPIO(BlackLib::GPIO_48 ,BlackLib::output,BlackLib::FastMode); // P9.15
    out_4 = new BlackLib::BlackGPIO(BlackLib::GPIO_51 ,BlackLib::output,BlackLib::FastMode); // P9.16
    out_5 = new BlackLib::BlackGPIO(BlackLib::GPIO_3  ,BlackLib::output,BlackLib::FastMode); // P9.21
    out_6 = new BlackLib::BlackGPIO(BlackLib::GPIO_2  ,BlackLib::output,BlackLib::FastMode); // P9.22
    out_7 = new BlackLib::BlackGPIO(BlackLib::GPIO_49 ,BlackLib::output,BlackLib::FastMode); // P9.23
    out_8 = new BlackLib::BlackGPIO(BlackLib::GPIO_117,BlackLib::output,BlackLib::FastMode); // P9.25
    // Inputs
    WriteLog("Main: Input GPIO's exported to /sys/class/gpio",0,FALSE);
    in_1 = new BlackLib::BlackGPIO(BlackLib::GPIO_115,BlackLib::input); // P9.27
    in_2 = new BlackLib::BlackGPIO(BlackLib::GPIO_66 ,BlackLib::input); // P8.07
    in_3 = new BlackLib::BlackGPIO(BlackLib::GPIO_67 ,BlackLib::input); // P8.08
    in_4 = new BlackLib::BlackGPIO(BlackLib::GPIO_69 ,BlackLib::input); // P8.09
    in_5 = new BlackLib::BlackGPIO(BlackLib::GPIO_68 ,BlackLib::input); // P8.10
    in_6 = new BlackLib::BlackGPIO(BlackLib::GPIO_45 ,BlackLib::input); // P8.11
    in_7 = new BlackLib::BlackGPIO(BlackLib::GPIO_44 ,BlackLib::input); // P8.12
    in_8 = new BlackLib::BlackGPIO(BlackLib::GPIO_23 ,BlackLib::input); // P8.13
    in_9 = new BlackLib::BlackGPIO(BlackLib::GPIO_26 ,BlackLib::input); // P8.14
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
    init_hardware();
    WriteLog("AL_main: Logfile Created!",0,FALSE);

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
        // Check gpio function on P9.12
        out_1->setValue(BlackLib::high);
        usleep(5000);
        out_1->setValue(BlackLib::low);
        usleep(5000);
        //-----------------------------------------------------------
        // Optokoppler Einlesen
        //-----------------------------------------------------------
/*
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
*/
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

