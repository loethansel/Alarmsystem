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
// NAMESPACES
using namespace std;
using namespace BlackLib;
// DEFINES
#define AINVAL1 1
#define AINVAL2 2
#define AINVAL3 3
#define AINVAL4 4
// EXTERN DECLARAIONS
extern void *AinTask(void *value);
extern pthread_t aintask;

#endif /* AIN_PROC_H_ */
