/*
 * gsm_proc.h
 *
 *  Created on: Nov 9, 2018
 *      Author: linux
 */

#ifndef GSM_PROC_H_
#define GSM_PROC_H_

// xx SekundenCheck des GSM-Moduls
#define LIVE_TIMER 10

extern pthread_t gsmtask;
extern void *GsmTask(void *value);

#endif /* GSM_PROC_H_ */
