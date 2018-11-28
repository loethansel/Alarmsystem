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
    SetValue("LOGLEVEL","logdebug",  "false");
    SetValue("ADDRESS", "standort",  "Flugschule");
    SetValue("ADDRESS", "strasse",   "Flugplatzstrasse");
    SetValue("ADDRESS", "hausnummer","3");
    SetValue("ADDRESS", "stadt",     "Fehrbellin");
    SetValue("ADDRESS", "plz",       "16833");
    SetValue("ALARM",   "autoalarm", "false");
    SetValue("ALARM",   "autocnt",   "0");
    SetValue("ALARM",   "alarmtime", "20");
    SetValue("ALARM",   "alarmtext", "Flugschule/gruene Halle");
    SetValue("ALARM_LINE", "line1active", "true");
    SetValue("ALARM_LINE", "line1umax",   "1.3");
    SetValue("ALARM_LINE", "line1umin",   "0.3");
    SetValue("ALARM_LINE", "line1text",   "Halle Gruen");
    SetValue("ALARM_LINE", "line2active", "true");
    SetValue("ALARM_LINE", "line2umax",   "1.3");
    SetValue("ALARM_LINE", "line2umin",   "0.3");
    SetValue("ALARM_LINE", "line2text",   "Empfang");
    SetValue("ALARM_LINE", "line3active", "false");
    SetValue("ALARM_LINE", "line3umax",   "1.3");
    SetValue("ALARM_LINE", "line3umin",   "0.3");
    SetValue("ALARM_LINE", "line3text",   "linie3");
    SetValue("ALARM_LINE", "line4active", "false");
    SetValue("ALARM_LINE", "line4umax",   "1.3");
    SetValue("ALARM_LINE", "line4umin",   "0.3");
    SetValue("ALARM_LINE", "line4text",   "linie4");
    SetValue("NETWORK", "protocol",  "dhcp");
    SetValue("NETWORK", "address",   "192.168.2.118");
    SetValue("GSM", "livetimer",      "60");
    SetValue("GSM", "rssitimer",      "60");
    SetValue("GSM", "livedeadtime",   "300");
    SetValue("GSM", "rssideadtime",   "300");
    SetValue("GSM", "creditwarnlevel","5.0");
    SetValue("TEL_NUM", "number1", "+491759944339");
    SetValue("TEL_NUM", "name1",   "Ralf Pandel");
    SetValue("TEL_NUM", "number2", "+1746122123");
    SetValue("TEL_NUM", "name2",   "Detlef Jenke");
    SetValue("TEL_NUM", "number3", "+1715410110");
    SetValue("TEL_NUM", "name3",   "Dussmann");
    SetValue("OUT_ACTIVE", "out1", "true");
    SetValue("OUT_ACTIVE", "out2", "true");
    SetValue("OUT_ACTIVE", "out3", "true");
    SetValue("OUT_ACTIVE", "out4", "true");
    SetValue("EMAIL", "alarmmail1",  "ralf@pandel.de");
    SetValue("EMAIL", "alarmmail2",  "email@messagebird.de");
    SetValue("EMAIL", "servicemail", "ralf@pandel.de");
    SetValue("XBEE_REMOTE", "macid1", "0013A200418259D5");
    SetValue("XBEE_REMOTE", "out1", "4");
    SetValue("XBEE_REMOTE", "in1", "12");
    SetValue("XBEE_REMOTE", "macid2", "0013A200418259D5");
    SetValue("XBEE_REMOTE", "out2", "4");
    SetValue("XBEE_REMOTE", "in2", "12");
    return true;
}

bool ctrlfile::CheckFileExists(const char *checkfilename)
{
ifstream  checkfile;

   checkfile.open(checkfilename, ios_base::in);
   if(!checkfile) return false;
   checkfile.close();
   return true;
}

bool ctrlfile::ReadIniFile(void)
{
bool retval;

    retval  = ReadINI(INIFILENAME);
    if(retval) {
        ini.LOGLEVEL.logdebug  = GetValue("LOGLEVEL","logdebug");
        ini.ADDRESS.standort   = GetValue("ADDRESS" ,"standort");
        ini.ADDRESS.strasse    = GetValue("ADDRESS" ,"strasse");
        ini.ADDRESS.hausnummer = GetValue("ADDRESS" ,"hausnummer");
        ini.ADDRESS.plz        = GetValue("ADDRESS" ,"plz");
        ini.ADDRESS.stadt      = GetValue("ADDRESS" ,"stadt");
        ini.ALARM.autoalarm    = GetValue("ALARM"   ,"autoalarm");
        ini.ALARM.autocnt      = GetValue("ALARM"   ,"autocnt");
        ini.ALARM.alarmtime    = GetValue("ALARM"   ,"alarmtime");
        ini.ALARM.alarmtext    = GetValue("ALARM"   ,"alarmtext");
        ini.ALARM_LINE.lineactv[0] = GetValue("ALARM_LINE" ,"line1active");
        ini.ALARM_LINE.lineumax[0] = GetValue("ALARM_LINE" ,"line1umax");
        ini.ALARM_LINE.lineumin[0] = GetValue("ALARM_LINE" ,"line1umin");
        ini.ALARM_LINE.linetext[0] = GetValue("ALARM_LINE" ,"line1text");
        ini.ALARM_LINE.lineactv[1] = GetValue("ALARM_LINE" ,"line2active");
        ini.ALARM_LINE.lineumax[1] = GetValue("ALARM_LINE" ,"line2umax");
        ini.ALARM_LINE.lineumin[1] = GetValue("ALARM_LINE" ,"line2umin");
        ini.ALARM_LINE.linetext[1] = GetValue("ALARM_LINE" ,"line2text");
        ini.ALARM_LINE.lineactv[2] = GetValue("ALARM_LINE" ,"line3active");
        ini.ALARM_LINE.lineumax[2] = GetValue("ALARM_LINE" ,"line3umax");
        ini.ALARM_LINE.lineumin[2] = GetValue("ALARM_LINE" ,"line3umin");
        ini.ALARM_LINE.linetext[2] = GetValue("ALARM_LINE" ,"line3text");
        ini.ALARM_LINE.lineactv[3] = GetValue("ALARM_LINE" ,"line4active");
        ini.ALARM_LINE.lineumax[3] = GetValue("ALARM_LINE" ,"line4umax");
        ini.ALARM_LINE.lineumin[3] = GetValue("ALARM_LINE" ,"line4umin");
        ini.ALARM_LINE.linetext[3] = GetValue("ALARM_LINE" ,"line4text");
        ini.NETWORK.protocol     = GetValue("NETWORK" ,"protocol");
        ini.NETWORK.address      = GetValue("NETWORK" ,"address");
        ini.GSM.livetimer        = GetValue("GSM"     ,"livetimer");
        ini.GSM.rssitimer        = GetValue("GSM"     ,"rssitimer");
        ini.GSM.livedeadtime     = GetValue("GSM"     ,"livedeadtime");
        ini.GSM.rssideadtime     = GetValue("GSM"     ,"rssideadtime");
        ini.GSM.creditwarnlevel  = GetValue("GSM"     ,"creditwarnlevel");
        ini.TEL_NUM.number[0]    = GetValue("TEL_NUM" ,"number1");
        ini.TEL_NUM.name[0]      = GetValue("TEL_NUM" ,"name1");
        ini.TEL_NUM.number[1]    = GetValue("TEL_NUM" ,"number2");
        ini.TEL_NUM.name[1]      = GetValue("TEL_NUM" ,"name2");
        ini.TEL_NUM.number[2]    = GetValue("TEL_NUM" ,"number3");
        ini.TEL_NUM.name[2]      = GetValue("TEL_NUM" ,"name3");
        ini.OUT_ACTIVE.out[0]  = GetValue("OUT_ACTIVE" ,"out1");
        ini.OUT_ACTIVE.out[1]  = GetValue("OUT_ACTIVE" ,"out2");
        ini.OUT_ACTIVE.out[2]  = GetValue("OUT_ACTIVE" ,"out3");
        ini.OUT_ACTIVE.out[3]  = GetValue("OUT_ACTIVE" ,"out4");
        ini.EMAIL.alarmmail[0] = GetValue("EMAIL" ,"alarmmail1");
        ini.EMAIL.alarmmail[1] = GetValue("EMAIL" ,"alarmmail2");
        ini.EMAIL.servicemail  = GetValue("EMAIL" ,"servicemail");
        ini.XBEE.macid[0]      = GetValue("XBEE" ,"macid1");
        ini.XBEE.out[0]        = GetValue("XBEE" ,"out1");
        ini.XBEE.in[0]         = GetValue("XBEE" ,"in1");
        ini.XBEE.macid[1]      = GetValue("XBEE" ,"macid2");
        ini.XBEE.out[1]        = GetValue("XBEE" ,"out2");
        ini.XBEE.in[1]         = GetValue("XBEE" ,"in2");
        return true;
    }
    return false;
}

// Reads all files in the system
bool ctrlfile::ReadActFiles(void)
{
bool retval;

//   if(!ReadAlarmNumbers()) return false;
//   if(!ReadAlarmMsg())     return false;
   if(!ReadSystemArmed())  return false;
//   if(!ReadLines())        return false;
   return true;
}

bool ctrlfile::WriteActFiles(void)
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
	ctrlfile::WriteActFiles();
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



