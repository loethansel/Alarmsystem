#ifndef INFILE_H_
#define INFILE_H_

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


// xx SekundenCheck des GSM-Moduls
#define NUMBERFILE "/home/debian/Alarmsystem/configfiles/gsm_number.txt"
#define MSGFILE    "/home/debian/Alarmsystem/configfiles/gsm_alarmmsg.txt"
#define MAX_NUM       5
#define MAX_MSG       5
#define MAX_LINE_LEN 50
#define MAX_NUM_LEN  50
#define MAX_MSG_LEN  50

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
} s_alarmzentrale;

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

extern bool ReadAlarmNumbers(void);
extern bool ReadAlarmMsg(void);
extern bool ReadFiles(void);

#endif //INFILE_H_
