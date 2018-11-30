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
#include "gsm/fona.h"
#include "gsm/gsm_proc.h"
#include "relais/serialrelais.h"
#include "analog/ain_proc.h"
#include "files/ctrlfile.h"
#include "files/iniparser.h"
#include "radio/xbee.h"
#include "email/email.h"

using namespace std;
using namespace BlackLib;

// GLOBAL VARS
extern bool sendsms;
extern bool armed;
extern bool alarmactive;
extern bool program_end;
extern bool contactopen;

extern BlackGPIO    *OUT_BUZZER;
extern BlackGPIO    *OUT_LED;
// INPUTS
extern BlackGPIO    *IN_SCHARF;
extern BlackGPIO    *IN_UNSCHARF;
// FILES
extern ctrlfile     *CTRLFILE;
// RELAIS
extern serialrelais *RELAIS;
// I2C-RELAIS
extern xbee         *RADIORELAIS;
// EMAIL
extern email        *EMAILALARM;


#define ON  true
#define OFF false
// Alarmtime in minutes
#define ALARMTIME 1
#define LOGFILENAME "/home/debian/Alarmsystem/files/alarm.log"

#endif /* ALARMSYS_H_ */
