 /*

 ####################################################################################
 #  BlackLib Library controls Beaglebone Black's inputs and outputs.                #
 #  Copyright (C) 2013-2015 by Yigit YUCE                                           #
 ####################################################################################
 #                                                                                  #
 #  This file is part of BlackLib library.                                          #
 #                                                                                  #
 #  BlackLib library is free software: you can redistribute it and/or modify        #
 #  it under the terms of the GNU Lesser General Public License as published by     #
 #  the Free Software Foundation, either version 3 of the License, or               #
 #  (at your option) any later version.                                             #
 #                                                                                  #
 #  BlackLib library is distributed in the hope that it will be useful,             #
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of                  #
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   #
 #  GNU Lesser General Public License for more details.                             #
 #                                                                                  #
 #  You should have received a copy of the GNU Lesser General Public License        #
 #  along with this program.  If not, see <http://www.gnu.org/licenses/>.           #
 #                                                                                  #
 #  For any comment or suggestion please contact the creator of BlackLib Library    #
 #  at ygtyce@gmail.com                                                             #
 #                                                                                  #
 ####################################################################################

 */
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"
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
        if(outok) {
            WriteLog("AL_main: Alarm!!!",0,FALSE);
            execvp("./mailer.sh", NULL);
        }


        //-----------------------------------------------------------
        // Optokoppler Einlesen
        //-----------------------------------------------------------
        if(Opto1.getNumericValue() == FALSE) {
            if(!barrier && outok) {
                WriteLog("AL_main: Alarm!!!",0,FALSE);
                execvp("./mailer.sh", NULL);
            }
            barrier = TRUE;
        }
        else {
            barrier = FALSE;
        }

    }



/*
    string to("kontakt@flugsport-berlin.de");
    string subject("Hallo Norbert");
    string body("Alarm, Alarm, Alarm...");
    stringstream command;
    command << "sudo echo \"" << body << "\" | mail -s \"" << subject << "\" " << to;
    */
/*
    stringstream ss;
    ss << "-s \"Testmail \" kontakt@flugsport-berlin.de";
    char *const arr[] = {"|", "mail", "-s","Testmail","kontakt@flugsport-berlin.de",NULL};
    char *const ar[] = {"ls","-la"};
    //int ret = execv("/usr/bin/",ar);
*/

//    int result = system(command.str().c_str());
//    system("clear");
//    cout << "Command: " << command.str() << endl;
    //    cout << "The return value was " << result << endl;
//    cout << "The return value was " << ret << endl;

//    example_GPIO();
//    example_ADC();
//    example_PWM();
//    example_SPI();
//    example_UART();
//    example_I2C();
//    example_directory();
//    example_threadAndMutex();
//    example_time();

    return 0;
}
