/*
 * infile.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include "infile.h"
// NAMESPACES
using namespace std;

s_alarmzentrale alarmzentrale;
s_msgtxt        msgtext;
// FOREWARD DECLARATIONS
bool ReadAlarmNumbers(void);
bool ReadAlarmMsg(void);
bool ReadFiles(void);

// Reads all files in the system
bool ReadFiles(void)
{
   if(!ReadAlarmNumbers()) return false;
   if(!ReadAlarmMsg())     return false;
   return true;
}

// Reads the file with Alarm numbers
bool ReadAlarmNumbers(void)
{
ifstream     numberfile;
string       s;
stringstream ss;
const char numberfilename[] = NUMBERFILE;
char line[255];
int retval;
int i,len1,len2;
int pos_a, pos_b, pos_c;

   numberfile.open(numberfilename, ios_base::in);
   if(!numberfile) {
       cout << "Alarmnummern: Datei kann nicht geöffnet werden." << endl;
       return false;
   } else {
       numberfile.getline(line,MAX_NUM_LEN,'\n');
       s = line;
       retval = s.find("STX");
       if(retval != -1) {
           alarmzentrale.numcnt = 0;
           for(i=0;i<MAX_NUM;i++) {
              // read line
              numberfile.getline(line,MAX_NUM_LEN,'\n');
              s = line;
              // check End Of Text
              retval = s.find("ETX");
              if(retval != -1) break;
              // if not ETX read new number
              pos_a = s.find_first_of(':',0);
              pos_b = s.find_first_of('-',0);
              pos_c = s.find_first_of(';',0);
              len1 = pos_b - pos_a -1;
              if(len1 > MAX_NUM_LEN) len1 = MAX_NUM_LEN;
              len2 = pos_c - pos_b -1;
              if(len2 > MAX_NUM_LEN) len2 = MAX_NUM_LEN;
              // read number "+491759944339"
              alarmzentrale.numname[i].numberlen = s.copy(alarmzentrale.numname[i].number,len1,pos_a+1);
              // read name "Ralf"
              alarmzentrale.numname[i].namelen   = s.copy(alarmzentrale.numname[i].name,len2,pos_b+1);
              // numbercnt increment
              alarmzentrale.numcnt++;
           }
       } else {
           cout << "Alarmnummern: STX nicht gefunden." << endl;
           return false;
       }

   }
   numberfile.close();
   return true;
}

// Reads the file with Alarm Messages
bool ReadAlarmMsg(void)
{
ifstream     msgfile;
string       s;
stringstream ss;
const char msgfilename[] = MSGFILE;
char line[255];
int retval;
int i,len1,len2;
int pos_a, pos_b, pos_c;

   msgfile.open(msgfilename, ios_base::in);
   if(!msgfile) {
       cout << "Alarmnummern: Datei kann nicht geöffnet werden." << endl;
       return false;
   } else {
       msgfile.getline(line,MAX_MSG_LEN,'\n');
       s = line;
       retval = s.find("STX");
       if(retval != -1) {
           alarmzentrale.numcnt = 0;
           for(i=0;i<MAX_NUM;i++) {
              // read line
              msgfile.getline(line,MAX_MSG_LEN,'\n');
              s = line;
              // check End Of Text
              retval = s.find("ETX");
              if(retval != -1) break;
              // if not ETX read new number
              pos_a = s.find_first_of(':',0);
              pos_b = s.find_first_of('-',0);
              pos_c = s.find_first_of(';',0);
              len1 = pos_b - pos_a -1;
              if(len1 > MAX_MSG_LEN) len1 = MAX_MSG_LEN;
              len2 = pos_c - pos_b -1;
              if(len2 > MAX_MSG_LEN) len2 = MAX_MSG_LEN;
              // read number "+491759944339"
              msgtext.msgid[i].msgtxtlen = s.copy(msgtext.msgid[i].msgtxt,len1,pos_a+1);
              // read name "Ralf"
              msgtext.msgid[i].msgtxtlen = s.copy(msgtext.msgid[i].msgtxt,len2,pos_b+1);
              // numbercnt increment
              msgtext.msgcnt++;
           }
       } else {
           cout << "Alarmnummern: STX nicht gefunden." << endl;
           return false;
       }

   }
   msgfile.close();
   return true;
}

void readini(void)
{
/*
    char BUFFER01[100];
    char BUFFER02[100];
    char szIniFile[MAX_PATH];

    //Speichert die ini Datei im selben Verzeichnis wie Die Exe
    GetModuleFileName(NULL,szIniFile,sizeof(szIniFile));
    *(strrchr(szIniFile,'\\')+1) = 0;
    lstrcat(szIniFile,"test.ini");

    //WritePrivateProfileString Schreibt informationen in eine INI Datei
    WritePrivateProfileString("EINSTELLUNGEN" , "WERT01" , "Test-01" , szIniFile);
    WritePrivateProfileString("EINSTELLUNGEN" , "WERT02" , "Test-02" , szIniFile);

    //GetPrivateProfileString Liesst die informationen aus der INI Datei
    GetPrivateProfileString("EINSTELLUNGEN", "WERT01" , "Test-Fehler" , BUFFER01, sizeof(szIniFile), szIniFile);
    GetPrivateProfileString("EINSTELLUNGEN", "WERT02" , "Test-Fehler" , BUFFER02, sizeof(szIniFile), szIniFile);

    //Dateien aus INI Laden
    printf("%s\n","[EINSTELLUNGEN]");
    printf("%s\n", BUFFER01);
    printf("%s\n", BUFFER02);
    printf("%s\n","..");
*/
}



