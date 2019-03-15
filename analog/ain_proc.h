/*
 * ain_proc.h
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
#ifndef AIN_PROC_H_
#define AIN_PROC_H_

// NAMESPACES
using namespace std;

// DEFINES
#define AINVAL1 1
#define AINVAL2 2
#define AINVAL3 3
#define AINVAL4 4
#define VOLTPRODIGIT 0.439453
#define MAXLINE 4
#define UMAX 1.00
#define UMIN 0.30
#define MIDCNT  4
// microseconds
#define MIDTIME 5000
// microseconds
#define MEASUREINTERVAL 100
// EXTERN DECLARAIONS
extern void *AinTask(void *value);
extern pthread_t aintask;

#endif /* AIN_PROC_H_ */
