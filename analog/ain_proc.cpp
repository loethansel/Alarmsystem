/*
 * ain_proc.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

#include "ain_proc.h"
#include "../logger/logger.h"
#include "../timer/EmaTimer.h"
#include "../socketclient/ThingSpeak.h"

//---------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;
// CLASSES
// FOREWARD DECLARATIONS
void linelog_handler(union sigval arg);
void measure_handler(union sigval arg);
// GLOBALVARS
bool      silentalarm[4];
float     valueFloat[4];
pthread_t aintask;
EmaTimer  linelogtimer(linelog_handler);
EmaTimer  measuretimer(measure_handler);
// Analog Input Declaration
BlackADC analog0(BlackLib::AIN0 );
BlackADC analog1(BlackLib::AIN1 );
BlackADC analog2(BlackLib::AIN2 );
BlackADC analog3(BlackLib::AIN3 );

//----------------------------------------------------------
// LINETOLOGGER
//----------------------------------------------------------
void linetologger(unsigned int line)
{
stringstream ss;
string       s;

    ss.str(""); ss.clear();
    ss << ctrlfile->ini.ALARM_LINE.linetext[line] << ": " << fixed << setprecision(3) << valueFloat[line];
    s = ss.str();
    Logger::Write(Logger::INFO,s);
}

//----------------------------------------------------------
// LINELOG_HANDLER
//----------------------------------------------------------
void linelog_handler(union sigval arg)
{
int i;
    // log only if linelog is chosen
    if(ctrlfile->ini.ALARM_LINE.linelog == "true") {
        // log the analog values every hour
        for(i=0;i<MAXLINE;i++) {
            // print only active lines
            if(ctrlfile->ini.ALARM_LINE.lineactv[i] == "true") linetologger(i);
        }
    }
    linelogtimer.StartTimer();
}

//----------------------------------------------------------
// READ_ANALOGVALUES
//----------------------------------------------------------
bool read_analogvalues(unsigned int line)
{
unsigned int i;
double       tmpfloatval[4];
float        umin, umax;
bool         result;

    // return true for lines that should not be checked
    if(ctrlfile->ini.ALARM_LINE.lineactv[line] == "false") return true;
    // read min/max thresholds
    try {
    umin = stof(ctrlfile->ini.ALARM_LINE.lineumin[line]);
    umax = stof(ctrlfile->ini.ALARM_LINE.lineumax[line]);
    } catch(const exception& e) { cout << "catched exception analog umin/umax: " << e.what() << endl; }
    // read analog values
    switch(line) {
        case 0:
            tmpfloatval[0] = 0.0;
            for(i=0;i<MIDCNT;i++) { tmpfloatval[0] += analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT; usleep(MIDTIME); }
            valueFloat[0] = tmpfloatval[0] / MIDCNT;
            tspeak->setval(LINE1FIELD,valueFloat[0]);
            if((valueFloat[0] >= umax) || (valueFloat[0] <= umin)) result = false;
            else result = true;
        break;
        case 1:
            tmpfloatval[1] = 0.0;
            for(i=0;i<MIDCNT;i++) { tmpfloatval[1] += analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT; usleep(MIDTIME); }
            valueFloat[1] = tmpfloatval[1] / MIDCNT;
            tspeak->setval(LINE2FIELD,valueFloat[1]);
            if((valueFloat[1] >= umax) || (valueFloat[1] <= umin)) result = false;
            else result = true;
        break;
        case 2:
            tmpfloatval[2] = 0.0;
            for(i=0;i<MIDCNT;i++) { tmpfloatval[2] += analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT; usleep(MIDTIME); }
            valueFloat[2] = tmpfloatval[2] / MIDCNT;
            tspeak->setval(LINE3FIELD,valueFloat[2]);
            if((valueFloat[2] >= umax) || (valueFloat[2] <= umin)) result = false;
            else result = true;
        break;
        case 3:
            tmpfloatval[3] = 0.0;
            for(i=0;i<MIDCNT;i++) { tmpfloatval[3] += analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT; usleep(MIDTIME); }
            valueFloat[3] = tmpfloatval[3] / MIDCNT;
            tspeak->setval(LINE4FIELD,valueFloat[3]);
            if((valueFloat[3] >= umax) || (valueFloat[3] <= umin)) result = false;
            else result = true;
        break;
        default:
            result = false;
        break;
    }
    return result;
}

//----------------------------------------------------------
// MEASURE_HANDLER (100ms)
//----------------------------------------------------------
void measure_handler(union sigval arg)
{
bool linethreshold[MAXLINE];
int  i;

    // check all lines
    for(i=0;i<MAXLINE;i++) {
        // read all lines
        if(!read_analogvalues(i)) {
            linethreshold[i] = true;
            if(armed) {
                if(ctrlfile->ini.ALARM_LINE.linecalm[i] == "true") silentactive = true;
                else alarmactive = true;
                linetologger(i);
            }
        } else linethreshold[i] = false;
    }
    // Check if all active lines are idle
    if((linethreshold[0] == 0) && (linethreshold[1] == 0) && (linethreshold[2] == 0) && (linethreshold[3] == 0)) {
        contactopen  = false;
        silentactive = false;
    } else contactopen = true;
    measuretimer.StartTimer();
}

//---------------------------------------------------------------------------
// AINTASK
//---------------------------------------------------------------------------
void *AinTask(void *value)
{
    // cyclic log the voltage value of each line
    try {
    linelogtimer.Create_Timer(0x00,stoi(ctrlfile->ini.ALARM.infotime)*60);
    } catch(const exception& e) { cout << "catched exception analog infotime: " << e.what() << endl; }
    linelogtimer.StartTimer();

    measuretimer.Create_Timer(MEASUREINTERVAL,0x00);
    measuretimer.StartTimer();
    while(1) {
        // INTERES SIGNAL PROGRAM END!!
        if(program_end) break;
    	usleep(200);
    }
    pthread_exit(NULL);
}





