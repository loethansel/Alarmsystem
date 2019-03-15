#ifndef CTRLFILE_H_
#define CTRLFILE_H_
// INCLUDES
#include "iniparser.h"
// NAMESPACES
using namespace std;
// DEFINES
// xx SekundenCheck des GSM-Moduls
#define NUMBERFILE     "/home/debian/Alarmsystem/files/gsm_number.txt"
#define MSGFILE        "/home/debian/Alarmsystem/files/gsm_alarmmsg.txt"
#define ARMEDFILE      "/home/debian/Alarmsystem/files/sys_armed.txt"
#define LINESFILE      "/home/debian/Alarmsystem/files/sys_lines.txt"
#define INIFILENAME    "/home/debian/Alarmsystem/files/config.ini"
#define MAX_NUM       5
#define MAX_MSG       5
#define MAXLINES      4
#define MAX_LINE_LEN  5
#define MAX_NUM_LEN  50
#define MAX_MSG_LEN  50
#define MAX_OUT       4
#define MAX_MAIL      2
#define MAX_XBEE      8
// MAX_XBEE defines
#define XBEE_ALROUT1 0
#define XBEE_ALROUT2 1
#define XBEE_ALR_IN1 2
#define XBEE_ALR_IN2 3
#define XBEE_TIMESW1 4
#define XBEE_TIMESW2 5
#define XBEE_ON_OFF1 6
#define XBEE_ON_OFF2 7

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
    string autotime;
    string autocnt;
    string alarmtime;
    string alarmtext;
    string infotime;
} s_alarm;
typedef struct {
    string lineactv[MAXLINES];
    string lineumax[MAXLINES];
    string lineumin[MAXLINES];
    string linetext[MAXLINES];
    string linecalm[MAXLINES];
    string linelog;
} s_lines;
typedef struct {
    string protocol;
    string address;
} s_net;
typedef struct {
    string livetimer;
    string rssitimer;
    string livedeadtime;
    string rssideadtime;
    string creditwarnlevel;
    string simpin;
} s_gsm;
typedef struct {
    string number[MAX_NUM];
    string name[MAX_NUM];
    string numbercnt;
} s_num;
typedef struct {
    string out[MAX_OUT];
} s_out;
typedef struct {
    string alarmmail[MAX_MAIL];
    string servicemail;
} s_mail;
typedef struct {
    string hostname;
    string port;
    string channelid;
    string writekey;
    string readkey;
    string interval;
} s_tspeak;
typedef struct {
    string name;
    string addr64;
    string addr16;
    string framet;
    string destend;
    string cluster;
    string profile;
    string payload1;
    string payload2;
} s_xbee;
typedef struct {
    string onhour;
    string offhour;
    string cyclesecs;
    string onsecs;
} s_timesw;
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
   s_tspeak  TSPEAK;
   s_timesw  TIMESW;
   s_xbee    XBEE[MAX_XBEE];
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
class CtrlFile : public INIParser
{
public:
   bool       armed_from_file;
   s_initval  ini;
   //   s_msgtxt   msgtext;
   //   s_alarmnum alarmnum;
   //   s_lines    lines;
   CtrlFile();
   virtual ~CtrlFile();
   bool CheckFileExists(const char *checkfilename);
   bool CreateDefaultIniFile(void);
   bool ReadIniFile(void);
   bool ReadActFiles(void);
   bool WriteActFiles(void);
   bool WriteSystemArmed(bool ctrl);
   bool ReadSystemArmed(void);
private:
   bool ReadInifile(void);
};

#endif //CTRLFILE_H_
