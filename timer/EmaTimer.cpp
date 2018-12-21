/*
 * EmaTimer.cpp
 *
 *  Created on: Dec 8, 2018
 *      Author: linux
 */
#include <ctime>
#include <iostream>
#include <signal.h>

#include "EmaTimer.h"

using namespace std;

EmaTimer::EmaTimer(void (*fp)(union sigval arg))
{
    // Set the sigevent structure to cause the signal to be delivered by creating a new thread.
    se.sigev_notify            = SIGEV_THREAD;
    se.sigev_value.sival_ptr   = &timer_id;
    se.sigev_notify_function   = fp;
    se.sigev_notify_attributes = NULL;
    status = false;
}

bool EmaTimer::Create_Timer(int milli, int sec)
{
int status;
long int nsec;

    nsec   = 1000000 * milli;
    ts.it_value.tv_sec     = sec;
    ts.it_value.tv_nsec    = nsec;
    ts.it_interval.tv_sec  = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if(status == -1) { cout << "Error: Create timer" << endl; return false; }
    return true;
}

bool EmaTimer::Create_Itimer(int milli, int sec, int i_milli, int i_sec)
{
int status;
long int nsec,nsec_i;

    nsec_i = 1000000 * i_milli;
    nsec   = 1000000 * milli;
    ts.it_value.tv_sec     = sec;
    ts.it_value.tv_nsec    = nsec;
    ts.it_interval.tv_sec  = i_sec;
    ts.it_interval.tv_nsec = nsec_i;

    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if(status == -1) { cout << "Error: Create timer" << endl; return false; }
    return true;
}


bool EmaTimer::StartTimer(void)
{
    status = timer_settime(timer_id, 0, &ts, 0);
    if(status == -1) { cout << "Error: Set timer" << timer_id << endl; return false; }
    return true;
}

bool EmaTimer::StopTimer(void)
{
struct itimerspec tst;

    tst.it_value.tv_sec     = 0;
    tst.it_value.tv_nsec    = 0;
    tst.it_interval.tv_sec  = 0;
    tst.it_interval.tv_nsec = 0;

    status = timer_settime(timer_id, 0, &tst, 0);
    if(status == -1) { cout << "Error: Stopp timer" << timer_id << endl; return false; }
    return true;
}


EmaTimer::~EmaTimer()
{
    timer_delete(timer_id);
}
