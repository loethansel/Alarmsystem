#ifndef CTRLFILE_H_
#define CTRLFILE_H_

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

// xx SekundenCheck des GSM-Moduls
#define NUMBERFILE "/home/debian/Alarmsystem/configfiles/gsm_number.txt"
#define MSGFILE    "/home/debian/Alarmsystem/configfiles/gsm_alarmmsg.txt"
#define ARMEDFILE  "/home/debian/Alarmsystem/configfiles/sys_armed.txt"
#define LINESFILE  "/home/debian/Alarmsystem/configfiles/sys_lines.txt"
#define MAX_NUM       5
#define MAX_MSG       5
#define MAXLINES     10
#define MAX_LINE_LEN 50
#define MAX_NUM_LEN  50
#define MAX_MSG_LEN  50
// TYPEDEFS
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
} s_linestxt;
typedef struct {
    s_linestxt l[MAXLINES];
    int    cnt;
} s_lines;



// CLASSES
class ctrlfile {
public:
   bool       armed_from_file;
   s_msgtxt   msgtext;
   s_alarmnum alarmnum;
   s_lines    lines;
   ctrlfile();
   virtual ~ctrlfile();
   bool ReadFiles(void);
   bool WriteFiles(void);
   bool WriteSystemArmed(bool ctrl);
   bool ReadAlarmNumbers(void);
   bool ReadAlarmMsg(void);
   bool ReadSystemArmed(void);
   bool ReadLines(void);
private:
   bool ReadInifile(void);
};

#endif //CTRLFILE_H_
