/*
 * ain_proc.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include <fstream>
#include <string>
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
//---------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------
#define CLOCKID CLOCK_REALTIME
#define SIG     SIGRTMIN
//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;
// GLOBALVARS
pthread_t aintask;
// Analg Input Declaration
BlackADC analog0(BlackLib::AIN0 );
BlackADC analog1(BlackLib::AIN1 );
BlackADC analog2(BlackLib::AIN2 );
BlackADC analog3(BlackLib::AIN3 );

// Interval Timer Handler
//void measure_handler(union sigval arg)
void measure_handler(void)
{
static int seccnt = 0;
float        valueFloat[4];
stringstream ss;
string       s;
int          i;
bool         lineactive[MAXLINE];
bool         linethreshold[MAXLINE];
bool         prealert[4];
float        umin, umax;

    // read analo values
    valueFloat[0] = analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[1] = analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[2] = analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[3] = analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    // Check if line is active
    for(i=0;i<MAXLINE;i++) {
        if(CTRLFILE->ini.ALARM_LINE.lineactv[i] == "true") lineactive[i] = true;
        else lineactive[i] = false;
        linethreshold[i]   = false;
    }
    // log the analog values every hour
    if(seccnt++ >= INFOTIME) {
       for(i=0;i<MAXLINE;i++) {
           if(lineactive[i]) {
               ss.str("");
               ss.clear();
               ss << "value LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
               s = ss.str();
               Logger::Write(Logger::DEBUG,s);
           }
       }
       seccnt = 0;
    }
    // first read all the lines to maxline
    for(i=0;i<MAXLINE;i++) {
        // Values to standard out
    	//!!
        // cout << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
        // check if value is off-limit
        prealert[i] = false;
        umin = stof(CTRLFILE->ini.ALARM_LINE.lineumin[i]);
        umax = stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]);
        if((valueFloat[i] >= umax) || (valueFloat[i] <= umin)) {
            // set alarm if line is permitted
            if(lineactive[i]) { prealert[i] = true; linethreshold[i] = true; }
        }
    }
    // wait 100ms to eleminate spikes and fail alerts
    usleep(100000);
    // reread the values
    valueFloat[0] = analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[1] = analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[2] = analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    valueFloat[3] = analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
    // second read all the lines to maxline
    for(i=0;i<MAXLINE;i++) {
        // check if value is off-limit
        umin = stof(CTRLFILE->ini.ALARM_LINE.lineumin[i]);
        umax = stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]);
        if((valueFloat[i] >= umax) || (valueFloat[i] <= umin)) {
            if(armed && (prealert[i] == true)) {
                alarmactive = true;
                if(CTRLFILE->ini.ALARM_LINE.linelog == "true") {
                    ss.str("");
                    ss.clear();
                    ss << "alarm LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
                    s = ss.str();
                    Logger::Write(Logger::INFO,s);
                }
            }
        }
    }
    // Check if all active lines are idle
    if((linethreshold[0] == 0) && (linethreshold[1] == 0) && (linethreshold[2] == 0) && (linethreshold[3] == 0)) contactopen = false;
    else contactopen = true;
}

void create_itimer_ainproc(int i)
{
timer_t timer_id;
int status;
struct itimerspec ts;
struct sigevent se;
long long nanosecs = 1000000 * 100 * i * i;

    // Set the sigevent structure to cause the signal to be delivered by creating a new thread.
    se.sigev_notify            = SIGEV_THREAD;
    se.sigev_value.sival_ptr   = &timer_id;
    // se.sigev_notify_function   = measure_handler;
    se.sigev_notify_attributes = NULL;

    ts.it_value.tv_sec  = nanosecs / 1000000000;
    ts.it_value.tv_nsec = nanosecs % 1000000000;
    ts.it_interval.tv_sec  = 1;
    ts.it_interval.tv_nsec = 200000;

    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if (status == -1) cout << "Create timer" << endl;
    // TODO maybe we'll need to have an array of itimerspec
    status = timer_settime(timer_id, 0, &ts, 0);
    if (status == -1) cout << "Set timer" << endl;
}

//---------------------------------------------------------------------------
// AINTASK
//---------------------------------------------------------------------------
void *AinTask(void *value)
{
    //create_itimer_ainproc(10000);
    while(1) {
        // INTERES SIGNAL PROGRAM END!!
        if(program_end) break;
        measure_handler();
    	sleep(1);
    }
    pthread_exit(NULL);
}





