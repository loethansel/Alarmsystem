/*
 * infile.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include "ctrlfile.h"
// NAMESPACES
using namespace std;


ctrlfile::ctrlfile(void)
{
   armed_from_file = false;
}

bool ctrlfile::CreateDefaultIniFile()
{
    SetValue("LOGLEVEL", "LOGDEBUG", "false");
    SetValue("ADDRESS", "standort", "Flugschule");
    SetValue("ADRESS", "strasse", "Flugplatzstrasse");
    SetValue("ADRESS", "hausnummer", "3");
    SetValue("ADRESS", "stadt", "Fehrbellin");
    SetValue("ADRESS", "plz", "16833");
    SetValue("ALARM", "autoalarm", "false");
    SetValue("ALARM", "autocnt", "0");
    SetValue("ALARM", "alarmtime", "20");
    SetValue("ALARM", "alarmtext", "Flugschule/gruene Halle");
    SetValue("ALARM_LINE", "lineactive", "false");
    SetValue("ALARM_LINE", "lineumax", "false");
    SetValue("ALARM_LINE", "lineumin", "false");
    SetValue("ALARM_LINE", "linetext", "false");
    SetValue("ALARM_LINE", "cnt", "false");
    SetValue("NETWORK", "protocol", "false");
    SetValue("GSM", "livetimer", "false");
    SetValue("GSM", "rssitimer", "false");
    SetValue("GSM", "livedeadtime", "false");
    SetValue("GSM", "rssideadtime", "false");
    SetValue("GSM", "creditwarnlevel", "false");
    SetValue("TEL_NUM", "number", "false");
    SetValue("TEL_NUM", "name", "false");
    SetValue("OUT_ACTIVE", "outactive", "false");
    SetValue("EMAIL", "alarmmail", "false");
    SetValue("EMAIL", "servicemail", "false");


    return true;
}

// Reads all files in the system
bool ctrlfile::ReadFiles(void)
{
bool retval;
string autoalarmstr;
string alarmtime;

    // INIFILETEST
    retval  = ReadINI(INIFILENAME);
    if(retval) {
        autoalarmstr = GetValue("ALARM","autoalarm");
        alarmtime    = GetValue("ALARM","alarmtime");
    }

//   if(!ReadAlarmNumbers()) return false;
//   if(!ReadAlarmMsg())     return false;
   if(!ReadSystemArmed())  return false;
//   if(!ReadLines())        return false;
   return true;
}

bool ctrlfile::WriteFiles(void)
{
   if(!WriteSystemArmed(false)) return false;
   return true;
}


bool ctrlfile::WriteSystemArmed(bool ctrl)
{
ofstream     armedfile;
const char   armedfilename[] = ARMEDFILE;

   armed_from_file = ctrl;
   armedfile.open(armedfilename, ios_base::out);
   if(!armedfile) {
       cout << "SystemArmed: Datei kann nicht zum Schreiben geöffnet werden." << endl;
       return false;
   }
   else {
      armedfile << "STX" << endl;
      if(ctrl) armedfile << "1" << endl;
      else     armedfile << "0" << endl;
	  armedfile << "ETX" << endl;
   }
   return true;
}

bool ctrlfile::ReadSystemArmed(void)
{
ifstream     armedfile;
string       s;
stringstream ss;
const char numberfilename[] = ARMEDFILE;
char       line[255];
int        retval;
bool       readval;

   armedfile.open(numberfilename, ios_base::in);
   if(!armedfile) {
       cout << "SystemArmed: Datei kann nicht zum lesen geöffnet werden." << endl;
       return false;
   }
   else {
      armedfile.getline(line,MAX_NUM_LEN,'\n');
      s = line;
      retval = s.find("STX");
      if(retval != -1) {
         armedfile.getline(line,MAX_NUM_LEN,'\n');
         if(line[0] == '1') readval = true;
         if(line[0] == '0') readval = false;
      } else return false;
      armedfile.getline(line,MAX_NUM_LEN,'\n');
      s = line;
      retval = s.find("ETX");
      if(retval == -1) return false;
   }
   armed_from_file = readval;
   return true;
}
/*

// Reads the file with Alarm numbers
bool ctrlfile::ReadAlarmNumbers(void)
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
           alarmnum.numcnt = 0;
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
              alarmnum.numname[i].numberlen = s.copy(alarmnum.numname[i].number,len1,pos_a+1);
              // read name "Ralf"
              alarmnum.numname[i].namelen   = s.copy(alarmnum.numname[i].name,len2,pos_b+1);
              // numbercnt increment
              alarmnum.numcnt++;
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
bool ctrlfile::ReadAlarmMsg(void)
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
           alarmnum.numcnt = 0;
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
*/

ctrlfile::~ctrlfile(void)
{
	ctrlfile::WriteFiles();
}
/*

// Reads the file with Alarm numbers
bool ctrlfile::ReadLines(void)
{
ifstream     linefile;
string       s;
stringstream ss;
const char linesfilename[] = LINESFILE;
char line[255],hstr[255];
int retval;
int i,len1,len2,len3;
int pos_a, pos_b, pos_c, pos_d;

   linefile.open(linesfilename, ios_base::in);
   if(!linefile) {
       cout << "Alarmlinien: Datei kann nicht geöffnet werden." << endl;
       return false;
   } else {
       linefile.getline(line,MAX_NUM_LEN,'\n');
       s = line;
       retval = s.find("STX");
       if(retval != -1) {
           lines.cnt = 0;
           for(i=0;i<MAX_NUM;i++) {
              // read line
              linefile.getline(line,MAX_NUM_LEN,'\n');
              s = line;
              // check End Of Text
              retval = s.find("ETX");
              if(retval != -1) break;
              // if not ETX Check string
              pos_a = s.find_first_of(':',0);
              pos_b = s.find_first_of('-',0);
              pos_c = s.find_first_of('+',0);
              pos_d = s.find_first_of(';',0);
              len1 = 1;
              len2 = pos_c - pos_b -1;
              len3 = pos_d - pos_c -1;
              if(len2 > MAX_LINE_LEN) len2 = MAX_LINE_LEN;
              if(len3 > MAX_LINE_LEN) len3 = MAX_LINE_LEN;
              // copy first sign
              s.copy(hstr,len1,pos_a+1);
              if(hstr[0] == '1') lines.l[i].account = true;
              else               lines.l[i].account = false;
              // copy of Umin
              s.copy(hstr,len2,pos_b+1);
              lines.l[i].umin = stof(hstr);
              // copy of Umax
              s.copy(hstr,len3,pos_c+1);
              lines.l[i].umax = stof(hstr);
              // numbercnt increment
              lines.cnt++;
           }
       } else {
           cout << "Alarmnummern: STX nicht gefunden." << endl;
           return false;
       }
   }
   linefile.close();
   return true;
}
*/



