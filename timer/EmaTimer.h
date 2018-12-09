/*
 * EmaTimer.h
 *
 *  Created on: Dec 8, 2018
 *      Author: Pandel
 */

#ifndef TIMER_EMATIMER_H_
#define TIMER_EMATIMER_H_

namespace std
{

class EmaTimer
{
public:
    EmaTimer(void (*fp)(union sigval arg));
    bool Create_Timer(int milli, int sec);
    bool Create_Itimer(int milli, int sec, int i_milli, int i_sec);
    bool StartTimer(void);
    bool StopTimer(void);
    virtual ~EmaTimer();
private:
    timer_t timer_id;
    int     status;
    struct itimerspec ts;
    struct sigevent   se;
};

} /* namespace std */

#endif /* TIMER_EMATIMER_H_ */
