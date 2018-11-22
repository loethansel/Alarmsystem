/*
 * ain_proc.h
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */

#ifndef AIN_PROC_H_
#define AIN_PROC_H_

// INCLUDE SYSTEM
#include <pthread.h>
#include <fstream>
#include <string>
#include <iostream>
// INCLUDE PROGRAM
#include "alarmsys.h"
#include "files/ctrlfile.h"
#include "blacklib/BlackADC/BlackADC.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;

// typedef float (*fptr_t) (digitAfterPoint mode);

// DEFINES
#define AINVAL1 1
#define AINVAL2 2
#define AINVAL3 3
#define AINVAL4 4
#define VOLTPRODIGIT 0.439453
#define MAXLINE 4
#define UMAX 1.00
#define UMIN 0.30
// EXTERN DECLARAIONS
extern void *AinTask(void *value);
extern pthread_t aintask;

#endif /* AIN_PROC_H_ */
