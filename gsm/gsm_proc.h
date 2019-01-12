/*
 * gsm_proc.h
 *
 *  Created on: Nov 9, 2018
 *      Author: linux
 */

#ifndef GSM_PROC_H_
#define GSM_PROC_H_

// xx SekundenCheck des GSM-Moduls
#define GSMINTERVAL 1
//#define MAX_DEAD_LIVETIME 300
//#define MAX_DEAD_RSSITIME 300
//#define LIVE_TIMER 60
//#define RSSI_TIMER 60
// EXTERN DECLARAIONS
extern pthread_t gsmtask;
extern void *GsmTask(void *value);

#endif /* GSM_PROC_H_ */
