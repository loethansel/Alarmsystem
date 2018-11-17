/*
 * alarmsys.h
 *
 *  Created on: Nov 9, 2018
 *      Author: linux
 */
#ifndef ALARMSYS_H_
#define ALARMSYS_H_
// INCLUDE SYSTEM
#include <ctime>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
// INCLUDE PROGRAM
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"
#include "blacklib/BlackGPIO/BlackGPIO.h"
#include "fona.h"
#include "gsm_proc.h"
#include "serialrelais.h"
#include "infile.h"
#include "ain_proc.h"

using namespace std;
using namespace BlackLib;

// GLOBAL VARS
extern bool sendsms;
extern bool scharf;
extern bool alarmactive;
extern BlackGPIO  *BUZZER;
extern BlackGPIO  *LED;
// INPUTS
extern BlackGPIO  *SCHARF;
extern BlackGPIO  *UNSCHARF;



#endif /* ALARMSYS_H_ */
