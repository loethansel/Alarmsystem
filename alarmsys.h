/*
 * alarmsys.h
 *
 *  Created on: Nov 9, 2018
 *      Author: linux
 */
#ifndef ALARMSYS_H_
#define ALARMSYS_H_
// INCLUDE SYSTEM

// INCLUDE PROGRAM
#include "files/ctrlfile.h"
#include "socketclient/ThingSpeak.h"
#include "socketserver/DisplayServer.h"
#include "email/email.h"

using namespace std;
// DEFINES
#define TARGET

// GLOBAL VARS
extern bool  sendsms;
extern bool  armed;
extern bool  alarmactive;
extern bool  silentactive;
extern bool  program_end;
extern bool  contactopen;
extern bool  xbeetest;


// CLASSES EXTERN
extern CtrlFile      *ctrlfile;
extern ThingSpeak    *tspeak;
extern Email         *emailalarm;
extern DisplayServer *display;
// DEFINES
#define ON   true
#define OFF  false
#define SET  true
#define CLR  false
#define LOGFILENAME "/home/debian/Alarmsystem/files/alarm.log"
#define VERSION "V1.4"

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
