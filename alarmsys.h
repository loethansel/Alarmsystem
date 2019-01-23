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
#include "socketclient/ThingSpeak.h"
#include "email/email.h"

using namespace std;
using namespace BlackLib;
// DEFINES
#define TARGET

// GLOBAL VARS
extern bool sendsms;
extern bool armed;
extern bool alarmactive;
extern bool silentactive;
extern bool program_end;
extern bool contactopen;

// CLASSES EXTERN
extern CtrlFile   *ctrlfile;
extern ThingSpeak *tspeak;
extern Email      *emailalarm;
// DEFINES
#define ON   true
#define OFF  false
#define SET  true
#define CLR  false
#define LOGFILENAME "/home/debian/Alarmsystem/files/alarm.log"
#define VERSION "V1.2"

// MAIN CLASS
class Alert
{
public:
    Alert();
    bool file_work(void);
    void init_system(void);
    bool init_tasks(void);
    bool switch_relais(bool onoff);
    void set_armed(void);
    void set_unarmed(void);
    void main_handler(void);
    void create_itimer_mainproc(int i);
    virtual ~Alert();
private:
};

#endif /* ALARMSYS_H_ */
