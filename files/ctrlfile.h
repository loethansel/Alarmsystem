#ifndef CTRLFILE_H_
#define CTRLFILE_H_

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "iniparser.h"

// xx SekundenCheck des GSM-Moduls
#define NUMBERFILE     "/home/debian/Alarmsystem/configfiles/gsm_number.txt"
#define MSGFILE        "/home/debian/Alarmsystem/configfiles/gsm_alarmmsg.txt"
#define ARMEDFILE      "/home/debian/Alarmsystem/configfiles/sys_armed.txt"
#define LINESFILE      "/home/debian/Alarmsystem/configfiles/sys_lines.txt"
#define INIFILENAME    "/home/debian/Alarmsystem/configfiles/config.ini"
#define MAX_NUM       5
#define MAX_MSG       5
#define MAXLINES     10
#define MAX_LINE_LEN  5
#define MAX_NUM_LEN  50
#define MAX_MSG_LEN  50
#define MAX_OUT       4
#define MAX_MAIL      2
// TYPEDEFS

// INISTRUCT
typedef struct {
    string logdebug;
} s_default;
typedef struct {
   string standort;
   string strasse;
   string hausnummer;
   string stadt;
   string plz;
} s_address;
typedef struct {
    string autoalarm;
    string autocnt;
    string alarmtime;
    string alarmtext;
} s_alarm;
typedef struct {
    string lineactive[MAXLINES];
    string lineumax[MAXLINES];
    string lineumin[MAXLINES];
    string linetext[MAXLINES];
    string cnt;
} s_lines;
typedef struct {
   string protocol;
} s_net;
typedef struct {
    string livetimer;
    string rssitimer;
    string livedeadtime;
    string rssideadtime;
    string creditwarnlevel;
} s_gsm;
typedef struct {
    string number[MAX_NUM];
    string name[MAX_NUM];
} s_num;
typedef struct {
    string outactive[MAX_OUT];
} s_out;
typedef struct {
    string alarmmail[MAX_MAIL];
    string servicemail[MAX_MAIL];
} s_mail;
// INISTRUCT
typedef struct {
   s_default LOGLEVEL;
   s_address ADDRESS;
   s_alarm   ALARM;
   s_lines   ALARM_LINE;
   s_net     NETWORK;
   s_gsm     GSM;
   s_num     TEL_NUM;
   s_out     OUT_ACTIVE;
   s_mail    EMAIL;
} s_initval;
/*
// ALARMNUMMERN
typedef struct {
    char number[MAX_NUM_LEN];
    int  numberlen;
    char name[MAX_NUM_LEN];
    int  namelen;
} s_numname;
typedef struct {
    s_numname numname[MAX_NUM];
    int       numcnt;
} s_alarmnum;
*/
/*
// ALARMTEXTE
typedef struct {
    char msgtxt[MAX_MSG_LEN];
    int  msgtxtlen;
    char msgid[MAX_MSG_LEN];
    int  msgidlen;
} s_msgtxtid;
typedef struct {
    s_msgtxtid msgid[MAX_MSG];
    int        msgcnt;
} s_msgtxt;
// LINES

typedef struct {
    bool   account;
    float  umin;
    float  umax;
    float  ucurr;
} s_linestxt;
typedef struct {
    s_linestxt l[MAXLINES];
    int    cnt;
} s_lines;
*/


// CLASSES
class ctrlfile : public INIParser
{
public:
   bool       armed_from_file;
//   s_msgtxt   msgtext;
//   s_alarmnum alarmnum;
//   s_lines    lines;
   s_initval  ini;
   ctrlfile();
   virtual ~ctrlfile();
   bool CreateDefaultIniFile(void);
   bool ReadFiles(void);
   bool WriteFiles(void);
   bool WriteSystemArmed(bool ctrl);
//   bool ReadAlarmNumbers(void);
//   bool ReadAlarmMsg(void);
   bool ReadSystemArmed(void);
//   bool ReadLines(void);
private:
   bool ReadInifile(void);
};

#endif //CTRLFILE_H_
