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


CtrlFile::CtrlFile(void)
{
   armed_from_file = false;
}

bool CtrlFile::CreateDefaultIniFile()
{
    SetValue("LOGLEVEL","logdebug",  "false");
    SetValue("ADDRESS", "standort",  "Flugschule");
    SetValue("ADDRESS", "strasse",   "Flugplatzstrasse");
    SetValue("ADDRESS", "hausnummer","3");
    SetValue("ADDRESS", "stadt",     "Fehrbellin");
    SetValue("ADDRESS", "plz",       "16833");
    SetValue("ALARM",   "autoalarm", "true");
    SetValue("ALARM",   "autotime",  "30");
    SetValue("ALARM",   "autocnt",   "1");
    SetValue("ALARM",   "alarmtime", "20");
    SetValue("ALARM",   "alarmtext", "Flugschule/Rundhalle");
    SetValue("ALARM",   "infotime",  "3600");
    SetValue("ALARM_LINE", "line1active", "true");
    SetValue("ALARM_LINE", "line1umax",   "1.0");
    SetValue("ALARM_LINE", "line1umin",   "0.3");
    SetValue("ALARM_LINE", "line1text",   "Halle Gruen");
    SetValue("ALARM_LINE", "line1silent", "false");
    SetValue("ALARM_LINE", "line2active", "true");
    SetValue("ALARM_LINE", "line2umax",   "1.0");
    SetValue("ALARM_LINE", "line2umin",   "0.3");
    SetValue("ALARM_LINE", "line2text",   "Empfang    ");
    SetValue("ALARM_LINE", "line2silent", "false");
    SetValue("ALARM_LINE", "line3active", "true");
    SetValue("ALARM_LINE", "line3umax",   "1.0");
    SetValue("ALARM_LINE", "line3umin",   "0.3");
    SetValue("ALARM_LINE", "line3text",   "Buero      ");
    SetValue("ALARM_LINE", "line3silent", "false");
    SetValue("ALARM_LINE", "line4active", "true");
    SetValue("ALARM_LINE", "line4umax",   "1.0");
    SetValue("ALARM_LINE", "line4umin",   "0.3");
    SetValue("ALARM_LINE", "line4text",   "Halle Grau ");
    SetValue("ALARM_LINE", "line4silent", "true");
    SetValue("ALARM_LINE", "linelog",     "true");
    SetValue("NETWORK", "protocol",  "dhcp");
    SetValue("NETWORK", "address",   "192.168.2.118");
    SetValue("GSM", "livetimer",      "1");
    SetValue("GSM", "rssitimer",      "1");
    SetValue("GSM", "livedeadtime",   "5");
    SetValue("GSM", "rssideadtime",   "5");
    SetValue("GSM", "creditwarnlevel","5.0");
    SetValue("GSM", "simpin",         "0000");
    SetValue("TEL_NUM", "number1",  "+4900000000");
    SetValue("TEL_NUM", "name1",    "Name1");
    SetValue("TEL_NUM", "number2",  "+4900000000");
    SetValue("TEL_NUM", "name2",    "Name2");
    SetValue("TEL_NUM", "number3",  "+4900000000");
    SetValue("TEL_NUM", "name3",    "Wachschutz");
    SetValue("TEL_NUM", "numbercnt","3");
    SetValue("OUT_ACTIVE", "out1", "true");
    SetValue("OUT_ACTIVE", "out2", "true");
    SetValue("OUT_ACTIVE", "out3", "true");
    SetValue("OUT_ACTIVE", "out4", "true");
    SetValue("EMAIL", "alarmmail1",  "ralf@xxxndel.de");
    SetValue("EMAIL", "alarmmail2",  "email@messagebird.de");
    SetValue("TSPEAK","hostname", "api.thingspeak.com");
    SetValue("TSPEAK","port",     "80");
    SetValue("TSPEAK","channelid","671677");
    SetValue("TSPEAK","writekey", "DR31KS856DPF1VJP");
    SetValue("TSPEAK","readkey",  "GK5NGDORRBPSOTYE");
    SetValue("TSPEAK","interval", "1");
    SetValue("TIMESW", "onhour",   "22");
    SetValue("TIMESW", "offhour",  "5");
    SetValue("TIMESW", "cyclesecs","3600");
    SetValue("TIMESW", "onsecs",   "900");
    // xbee section
    SetValue("XBEE_ALARMOUT1",   "name",    "Sirene");
    SetValue("XBEE_ALARMOUT1",   "addr64",  "0x0013A200418259D5");
    SetValue("XBEE_ALARMOUT1",   "addr16",  "0xFFFE");
    SetValue("XBEE_ALARMOUT1",   "framet",  "0x17");
    SetValue("XBEE_ALARMOUT1",   "destend", "0x03");
    SetValue("XBEE_ALARMOUT1",   "cluster", "0x0006");
    SetValue("XBEE_ALARMOUT1",   "profile", "0x0104");
    SetValue("XBEE_ALARMOUT1",   "payload1","443405");
    SetValue("XBEE_ALARMOUT1",   "payload2","443404");
    SetValue("XBEE_ALARMOUT2",   "name",    "tester");
    SetValue("XBEE_ALARMOUT2",   "addr64",  "0x7CB03EAA00B239EA");
    SetValue("XBEE_ALARMOUT2",   "addr16",  "0xFFFE");
    SetValue("XBEE_ALARMOUT2",   "framet",  "0x11");
    SetValue("XBEE_ALARMOUT2",   "destend", "0x03");
    SetValue("XBEE_ALARMOUT2",   "cluster", "0x0006");
    SetValue("XBEE_ALARMOUT2",   "profile", "0x0104");
    SetValue("XBEE_ALARMOUT2",   "payload1","0100010010");
    SetValue("XBEE_ALARMOUT2",   "payload2","0100000010");
    SetValue("XBEE_ALARMIN1",    "name",    "nc");
    SetValue("XBEE_ALARMIN1",    "addr64",  "0x00");
    SetValue("XBEE_ALARMIN1",    "addr16",  "0xFFFE");
    SetValue("XBEE_ALARMIN1",    "framet",  "0x17");
    SetValue("XBEE_ALARMIN1",    "destend", "0x03");
    SetValue("XBEE_ALARMIN1",    "cluster", "0x0006");
    SetValue("XBEE_ALARMIN1",    "profile", "0x0104");
    SetValue("XBEE_ALARMIN1",    "payload1","443103");
    SetValue("XBEE_ALARMIN1",    "payload2","443103");
    SetValue("XBEE_ALARMIN2",    "name",    "nc");
    SetValue("XBEE_ALARMIN2",    "addr64",  "0x00");
    SetValue("XBEE_ALARMIN2",    "addr16",  "0xFFFE");
    SetValue("XBEE_ALARMIN2",    "framet",  "0x17");
    SetValue("XBEE_ALARMIN2",    "destend", "0x03");
    SetValue("XBEE_ALARMIN2",    "cluster", "0x0006");
    SetValue("XBEE_ALARMIN2",    "profile", "0x0104");
    SetValue("XBEE_ALARMIN2",    "payload1","443103");
    SetValue("XBEE_ALARMIN2",    "payload2","443103");
    SetValue("XBEE_TIMESWITCH1", "name",    "Lampe-WC");
    SetValue("XBEE_TIMESWITCH1", "addr64",  "0x7CB03EAA00B07F57");
    SetValue("XBEE_TIMESWITCH1", "addr16",  "0xFFFE");
    SetValue("XBEE_TIMESWITCH1", "framet",  "0x11");
    SetValue("XBEE_TIMESWITCH1", "destend", "0x03");
    SetValue("XBEE_TIMESWITCH1", "cluster", "0x0006");
    SetValue("XBEE_TIMESWITCH1", "profile", "0x0104");
    SetValue("XBEE_TIMESWITCH1", "payload1","0100010010");
    SetValue("XBEE_TIMESWITCH1", "payload2","0100000010");
    SetValue("XBEE_TIMESWITCH1", "name",    "Lampe-Wozi");
    SetValue("XBEE_TIMESWITCH2", "addr64",  "0x7CB03EAA00B08A30");
    SetValue("XBEE_TIMESWITCH2", "addr16",  "0xFFFE");
    SetValue("XBEE_TIMESWITCH2", "framet",  "0x11");
    SetValue("XBEE_TIMESWITCH2", "destend", "0x03");
    SetValue("XBEE_TIMESWITCH2", "cluster", "0x0006");
    SetValue("XBEE_TIMESWITCH2", "profile", "0x0104");
    SetValue("XBEE_TIMESWITCH2", "payload1","0100010010");
    SetValue("XBEE_TIMESWITCH2", "payload2","0100000010");
    SetValue("XBEE_ONOFF1",      "name",    "nc");
    SetValue("XBEE_ONOFF1",      "addr64",  "0x00");
    SetValue("XBEE_ONOFF1",      "addr16",  "0xFFFE");
    SetValue("XBEE_ONOFF1",      "framet",  "0x17");
    SetValue("XBEE_ONOFF1",      "destend", "0x03");
    SetValue("XBEE_ONOFF1",      "cluster", "0x0006");
    SetValue("XBEE_ONOFF1",      "profile", "0x0104");
    SetValue("XBEE_ONOFF1",      "payload1","443103");
    SetValue("XBEE_ONOFF1",      "payload2","443103");
    SetValue("XBEE_ONOFF2",      "name",    "nc");
    SetValue("XBEE_ONOFF2",      "addr64",  "0x00");
    SetValue("XBEE_ONOFF2",      "addr16",  "0xFFFE");
    SetValue("XBEE_ONOFF2",      "framet",  "0x17");
    SetValue("XBEE_ONOFF2",      "destend", "0x03");
    SetValue("XBEE_ONOFF2",      "cluster", "0x0006");
    SetValue("XBEE_ONOFF2",      "profile", "0x0104");
    SetValue("XBEE_ONOFF2",      "payload1","443103");
    SetValue("XBEE_ONOFF2",      "payload2","443103");
    return true;
}

bool CtrlFile::CheckFileExists(const char *checkfilename)
{
ifstream  checkfile;

   checkfile.open(checkfilename, ios_base::in);
   if(!checkfile) return false;
   checkfile.close();
   return true;
}

bool CtrlFile::ReadIniFile(void)
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
        ini.ALARM.autotime     = GetValue("ALARM"   ,"autotime");
        ini.ALARM.alarmtime    = GetValue("ALARM"   ,"alarmtime");
        ini.ALARM.alarmtext    = GetValue("ALARM"   ,"alarmtext");
        ini.ALARM.infotime     = GetValue("ALARM"   ,"infotime");
        ini.ALARM_LINE.lineactv[0] = GetValue("ALARM_LINE" ,"line1active");
        ini.ALARM_LINE.lineumax[0] = GetValue("ALARM_LINE" ,"line1umax");
        ini.ALARM_LINE.lineumin[0] = GetValue("ALARM_LINE" ,"line1umin");
        ini.ALARM_LINE.linetext[0] = GetValue("ALARM_LINE" ,"line1text");
        ini.ALARM_LINE.linecalm[0] = GetValue("ALARM_LINE" ,"line1silent");
        ini.ALARM_LINE.lineactv[1] = GetValue("ALARM_LINE" ,"line2active");
        ini.ALARM_LINE.lineumax[1] = GetValue("ALARM_LINE" ,"line2umax");
        ini.ALARM_LINE.lineumin[1] = GetValue("ALARM_LINE" ,"line2umin");
        ini.ALARM_LINE.linetext[1] = GetValue("ALARM_LINE" ,"line2text");
        ini.ALARM_LINE.linecalm[1] = GetValue("ALARM_LINE" ,"line2silent");
        ini.ALARM_LINE.lineactv[2] = GetValue("ALARM_LINE" ,"line3active");
        ini.ALARM_LINE.lineumax[2] = GetValue("ALARM_LINE" ,"line3umax");
        ini.ALARM_LINE.lineumin[2] = GetValue("ALARM_LINE" ,"line3umin");
        ini.ALARM_LINE.linetext[2] = GetValue("ALARM_LINE" ,"line3text");
        ini.ALARM_LINE.linecalm[2] = GetValue("ALARM_LINE" ,"line3silent");
        ini.ALARM_LINE.lineactv[3] = GetValue("ALARM_LINE" ,"line4active");
        ini.ALARM_LINE.lineumax[3] = GetValue("ALARM_LINE" ,"line4umax");
        ini.ALARM_LINE.lineumin[3] = GetValue("ALARM_LINE" ,"line4umin");
        ini.ALARM_LINE.linetext[3] = GetValue("ALARM_LINE" ,"line4text");
        ini.ALARM_LINE.linecalm[3] = GetValue("ALARM_LINE" ,"line4silent");
        ini.ALARM_LINE.linelog     = GetValue("ALARM_LINE" ,"linelog");
        ini.NETWORK.protocol     = GetValue("NETWORK" ,"protocol");
        ini.NETWORK.address      = GetValue("NETWORK" ,"address");
        ini.GSM.livetimer        = GetValue("GSM"     ,"livetimer");
        ini.GSM.rssitimer        = GetValue("GSM"     ,"rssitimer");
        ini.GSM.livedeadtime     = GetValue("GSM"     ,"livedeadtime");
        ini.GSM.rssideadtime     = GetValue("GSM"     ,"rssideadtime");
        ini.GSM.creditwarnlevel  = GetValue("GSM"     ,"creditwarnlevel");
        ini.GSM.simpin           = GetValue("GSM"     ,"simpin");
        ini.TEL_NUM.number[0]    = GetValue("TEL_NUM" ,"number1");
        ini.TEL_NUM.name[0]      = GetValue("TEL_NUM" ,"name1");
        ini.TEL_NUM.number[1]    = GetValue("TEL_NUM" ,"number2");
        ini.TEL_NUM.name[1]      = GetValue("TEL_NUM" ,"name2");
        ini.TEL_NUM.number[2]    = GetValue("TEL_NUM" ,"number3");
        ini.TEL_NUM.name[2]      = GetValue("TEL_NUM" ,"name3");
        ini.TEL_NUM.numbercnt    = GetValue("TEL_NUM" ,"numbercnt");
        ini.OUT_ACTIVE.out[0]   = GetValue("OUT_ACTIVE" ,"out1");
        ini.OUT_ACTIVE.out[1]   = GetValue("OUT_ACTIVE" ,"out2");
        ini.OUT_ACTIVE.out[2]   = GetValue("OUT_ACTIVE" ,"out3");
        ini.OUT_ACTIVE.out[3]   = GetValue("OUT_ACTIVE" ,"out4");
        ini.EMAIL.alarmmail[0]  = GetValue("EMAIL" ,"alarmmail1");
        ini.EMAIL.alarmmail[1]  = GetValue("EMAIL" ,"alarmmail2");
        ini.EMAIL.servicemail   = GetValue("EMAIL" ,"servicemail");
        ini.TSPEAK.hostname     = GetValue("TSPEAK","hostname");
        ini.TSPEAK.port         = GetValue("TSPEAK","port");
        ini.TSPEAK.channelid    = GetValue("TSPEAK","channelid");
        ini.TSPEAK.writekey     = GetValue("TSPEAK","writekey");
        ini.TSPEAK.readkey      = GetValue("TSPEAK","readkey");
        ini.TSPEAK.interval     = GetValue("TSPEAK","interval");
        ini.TIMESW.onhour       = GetValue("TIMESW","onhour");
        ini.TIMESW.offhour      = GetValue("TIMESW","offhour");
        ini.TIMESW.cyclesecs    = GetValue("TIMESW","cyclesecs");
        ini.TIMESW.onsecs       = GetValue("TIMESW","onsecs");
        // xbee section alarm out
        ini.XBEE[XBEE_ALROUT1].name     = GetValue("XBEE_ALARMOUT1"   ,"name");
        ini.XBEE[XBEE_ALROUT1].addr64   = GetValue("XBEE_ALARMOUT1"   ,"addr64");
        ini.XBEE[XBEE_ALROUT1].addr16   = GetValue("XBEE_ALARMOUT1"   ,"addr16");
        ini.XBEE[XBEE_ALROUT1].framet   = GetValue("XBEE_ALARMOUT1"   ,"framet");
        ini.XBEE[XBEE_ALROUT1].destend  = GetValue("XBEE_ALARMOUT1"   ,"destend");
        ini.XBEE[XBEE_ALROUT1].cluster  = GetValue("XBEE_ALARMOUT1"   ,"cluster");
        ini.XBEE[XBEE_ALROUT1].profile  = GetValue("XBEE_ALARMOUT1"   ,"profile");
        ini.XBEE[XBEE_ALROUT1].payload1 = GetValue("XBEE_ALARMOUT1"   ,"payload1");
        ini.XBEE[XBEE_ALROUT1].payload2 = GetValue("XBEE_ALARMOUT1"   ,"payload2");
        ini.XBEE[XBEE_ALROUT2].name     = GetValue("XBEE_ALARMOUT2"   ,"name");
        ini.XBEE[XBEE_ALROUT2].addr64   = GetValue("XBEE_ALARMOUT2"   ,"addr64");
        ini.XBEE[XBEE_ALROUT2].addr16   = GetValue("XBEE_ALARMOUT2"   ,"addr16");
        ini.XBEE[XBEE_ALROUT2].framet   = GetValue("XBEE_ALARMOUT2"   ,"framet");
        ini.XBEE[XBEE_ALROUT2].destend  = GetValue("XBEE_ALARMOUT2"   ,"destend");
        ini.XBEE[XBEE_ALROUT2].cluster  = GetValue("XBEE_ALARMOUT2"   ,"cluster");
        ini.XBEE[XBEE_ALROUT2].profile  = GetValue("XBEE_ALARMOUT2"   ,"profile");
        ini.XBEE[XBEE_ALROUT2].payload1 = GetValue("XBEE_ALARMOUT2"   ,"payload1");
        ini.XBEE[XBEE_ALROUT2].payload2 = GetValue("XBEE_ALARMOUT2"   ,"payload2");
        // xbee section alarm in
        ini.XBEE[XBEE_ALR_IN1].name     = GetValue("XBEE_ALARMIN1"   ,"name");
        ini.XBEE[XBEE_ALR_IN1].addr64   = GetValue("XBEE_ALARMIN1"   ,"addr64");
        ini.XBEE[XBEE_ALR_IN1].addr16   = GetValue("XBEE_ALARMIN1"   ,"addr16");
        ini.XBEE[XBEE_ALR_IN1].framet   = GetValue("XBEE_ALARMIN1"   ,"framet");
        ini.XBEE[XBEE_ALR_IN1].destend  = GetValue("XBEE_ALARMIN1"   ,"destend");
        ini.XBEE[XBEE_ALR_IN1].cluster  = GetValue("XBEE_ALARMIN1"   ,"cluster");
        ini.XBEE[XBEE_ALR_IN1].profile  = GetValue("XBEE_ALARMIN1"   ,"profile");
        ini.XBEE[XBEE_ALR_IN1].payload1 = GetValue("XBEE_ALARMIN1"   ,"payload1");
        ini.XBEE[XBEE_ALR_IN1].payload2 = GetValue("XBEE_ALARMIN1"   ,"payload2");
        ini.XBEE[XBEE_ALR_IN2].name     = GetValue("XBEE_ALARMIN2"   ,"name");
        ini.XBEE[XBEE_ALR_IN2].addr64   = GetValue("XBEE_ALARMIN2"   ,"addr64");
        ini.XBEE[XBEE_ALR_IN2].addr16   = GetValue("XBEE_ALARMIN2"   ,"addr16");
        ini.XBEE[XBEE_ALR_IN2].framet   = GetValue("XBEE_ALARMIN2"   ,"framet");
        ini.XBEE[XBEE_ALR_IN2].destend  = GetValue("XBEE_ALARMIN2"   ,"destend");
        ini.XBEE[XBEE_ALR_IN2].cluster  = GetValue("XBEE_ALARMIN2"   ,"cluster");
        ini.XBEE[XBEE_ALR_IN2].profile  = GetValue("XBEE_ALARMIN2"   ,"profile");
        ini.XBEE[XBEE_ALR_IN2].payload1 = GetValue("XBEE_ALARMIN2"   ,"payload1");
        ini.XBEE[XBEE_ALR_IN2].payload2 = GetValue("XBEE_ALARMIN2"   ,"payload2");
        // xbee section time switch
        ini.XBEE[XBEE_TIMESW1].name     = GetValue("XBEE_TIMESWITCH1"   ,"name");
        ini.XBEE[XBEE_TIMESW1].addr64   = GetValue("XBEE_TIMESWITCH1"   ,"addr64");
        ini.XBEE[XBEE_TIMESW1].addr16   = GetValue("XBEE_TIMESWITCH1"   ,"addr16");
        ini.XBEE[XBEE_TIMESW1].framet   = GetValue("XBEE_TIMESWITCH1"   ,"framet");
        ini.XBEE[XBEE_TIMESW1].destend  = GetValue("XBEE_TIMESWITCH1"   ,"destend");
        ini.XBEE[XBEE_TIMESW1].cluster  = GetValue("XBEE_TIMESWITCH1"   ,"cluster");
        ini.XBEE[XBEE_TIMESW1].profile  = GetValue("XBEE_TIMESWITCH1"   ,"profile");
        ini.XBEE[XBEE_TIMESW1].payload1 = GetValue("XBEE_TIMESWITCH1"   ,"payload1");
        ini.XBEE[XBEE_TIMESW1].payload2 = GetValue("XBEE_TIMESWITCH1"   ,"payload2");
        ini.XBEE[XBEE_TIMESW2].name     = GetValue("XBEE_TIMESWITCH2"   ,"name");
        ini.XBEE[XBEE_TIMESW2].addr64   = GetValue("XBEE_TIMESWITCH2"   ,"addr64");
        ini.XBEE[XBEE_TIMESW2].addr16   = GetValue("XBEE_TIMESWITCH2"   ,"addr16");
        ini.XBEE[XBEE_TIMESW2].framet   = GetValue("XBEE_TIMESWITCH2"   ,"framet");
        ini.XBEE[XBEE_TIMESW2].destend  = GetValue("XBEE_TIMESWITCH2"   ,"destend");
        ini.XBEE[XBEE_TIMESW2].cluster  = GetValue("XBEE_TIMESWITCH2"   ,"cluster");
        ini.XBEE[XBEE_TIMESW2].profile  = GetValue("XBEE_TIMESWITCH2"   ,"profile");
        ini.XBEE[XBEE_TIMESW2].payload1 = GetValue("XBEE_TIMESWITCH2"   ,"payload1");
        ini.XBEE[XBEE_TIMESW2].payload2 = GetValue("XBEE_TIMESWITCH2"   ,"payload2");
        // xbee section on off
        ini.XBEE[XBEE_ON_OFF1].name     = GetValue("XBEE_ONOFF1"   ,"name");
        ini.XBEE[XBEE_ON_OFF1].addr64   = GetValue("XBEE_ONOFF1"   ,"addr64");
        ini.XBEE[XBEE_ON_OFF1].addr16   = GetValue("XBEE_ONOFF1"   ,"addr16");
        ini.XBEE[XBEE_ON_OFF1].framet   = GetValue("XBEE_ONOFF1"   ,"framet");
        ini.XBEE[XBEE_ON_OFF1].destend  = GetValue("XBEE_ONOFF1"   ,"destend");
        ini.XBEE[XBEE_ON_OFF1].cluster  = GetValue("XBEE_ONOFF1"   ,"cluster");
        ini.XBEE[XBEE_ON_OFF1].profile  = GetValue("XBEE_ONOFF1"   ,"profile");
        ini.XBEE[XBEE_ON_OFF1].payload1 = GetValue("XBEE_ONOFF1"   ,"payload1");
        ini.XBEE[XBEE_ON_OFF1].payload2 = GetValue("XBEE_ONOFF1"   ,"payload2");
        ini.XBEE[XBEE_ON_OFF2].name     = GetValue("XBEE_ONOFF2"   ,"name");
        ini.XBEE[XBEE_ON_OFF2].addr64   = GetValue("XBEE_ONOFF2"   ,"addr64");
        ini.XBEE[XBEE_ON_OFF2].addr16   = GetValue("XBEE_ONOFF2"   ,"addr16");
        ini.XBEE[XBEE_ON_OFF2].framet   = GetValue("XBEE_ONOFF2"   ,"framet");
        ini.XBEE[XBEE_ON_OFF2].destend  = GetValue("XBEE_ONOFF2"   ,"destend");
        ini.XBEE[XBEE_ON_OFF2].cluster  = GetValue("XBEE_ONOFF2"   ,"cluster");
        ini.XBEE[XBEE_ON_OFF2].profile  = GetValue("XBEE_ONOFF2"   ,"profile");
        ini.XBEE[XBEE_ON_OFF2].payload1 = GetValue("XBEE_ONOFF2"   ,"payload1");
        ini.XBEE[XBEE_ON_OFF2].payload2 = GetValue("XBEE_ONOFF2"   ,"payload2");
        return true;
    }
    return false;
}

// Reads all files in the system
bool CtrlFile::ReadActFiles(void)
{
   if(!ReadSystemArmed())  return false;
   return true;
}

bool CtrlFile::WriteActFiles(void)
{
   if(!WriteSystemArmed(false)) return false;
   return true;
}


bool CtrlFile::WriteSystemArmed(bool ctrl)
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
   armedfile.close();
   return true;
}

bool CtrlFile::ReadSystemArmed(void)
{
ifstream     armedfile;
string       s;
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
   armedfile.close();
   return true;
}


CtrlFile::~CtrlFile(void)
{
	CtrlFile::WriteActFiles();
}

