/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
    // next line per http://postwarrior.com/arduino-ethershield-error-prog_char-does-not-name-a-type/

#include <ctime>
#include <mutex>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "Adafruit_FONA.h"
#include "FonaSerial.h"
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackUART/BlackUART.h"
#include "../files/ctrlfile.h"
#include "../alarmsys.h"
#include "../logger/logger.h"
#include "../blacklib/BlackErr.h"


using namespace std;
using namespace BlackLib;
using namespace logger;
BlackLib::BlackGPIO  pwr_out(GPIO_60,output,FastMode);
BlackLib::BlackGPIO  pwr_in(GPIO_49,input);
BlackLib::BlackGPIO  rst_out(GPIO_48,output,FastMode);
FonaSerial *mySerial;
/*
bool prog_char_strlcmp(char *text, const char *ctext, int len)
{
string hstr = text;

   if(hstr.compare(ctext,len) == 0) return false;
   else return true;
}
*/
bool prog_char_strcmp(char *text, const char *ctext)
{
string hstr1  = text;
string hstr2  = ctext;
int    len    = hstr2.length();

   if(hstr1.compare(0,len,ctext) == 0) {
       return 0;
   }
   else {
       return 1;
   }
}
void prog_char_strcpy(char *text, const char *ctext)
{
string hstr = ctext;

   hstr.copy(text,hstr.length());
}
uint8_t prog_char_strlen(const char * ctext)
{
string hstr = ctext;

   return hstr.length();
}
char *prog_char_strstr(char *text, const char *ctext)
{
string hstr = text;
int retval;

   retval = hstr.find(ctext);
   if(retval != -1) {
       return text;
   } else return 0;
}

void DEBUG_PRINT(const char *text)
{
#ifdef FONADEBUG
   cout << text;
#endif
}
void DEBUG_PRINTLN(const char *text)
{
#ifdef FONADEBUG
   cout << text << endl;
#endif
}

Adafruit_FONA::Adafruit_FONA(void)
{
  _type           = FONA3G_E;
  apn             = "FONAnet";
  apnusername     = 0;
  apnpassword     = 0;
  httpsredirect   = false;
  useragent       = "FONA";
  ok_reply        = "OK";
  fonalive        = false;
  poweredon       = false;
  fonarssi        = false;
  fonanet         = false;
  rxpegel_numeric = 0;
  credit_numeric  = 0;
  // serial interface
  mySerial        = new FonaSerial();
}

int Adafruit_FONA::Power_On(void)
{
unsigned int i;

    // try for 3 times to power up
    for(i=0;i<3;i++) {
        // button pressed for 1 second
        pwr_out.setValue(low);
        sleep(1);
        pwr_out.setValue(high);
        // check pwrpin and AT\r => OK
        if(IsRunning()) {
           this->poweredon = true;
           return true;
        }
    }
    this->poweredon = false;
    return false;
}

int Adafruit_FONA::Power_Off(void)
{
unsigned int i;
    // is the module down yet?
    if(pwr_in.getNumericValue() == low) {
        begin();
        return true;
    }
    // try to power down the module for 3 times
    for(i=0;i<3;i++) {
        // button pressed for 1 second
        pwr_out.setValue(low);
        sleep(1);
        pwr_out.setValue(high);
        // check the powerpin is low
        if(IsStopped()) return true;
    }
    return false;
}

int Adafruit_FONA::IsStopped(void)
{
int i;

   // wait x seconds till module powered down
   for(i=0;i<15;i++) {
     // check the powerpin in high
     if(pwr_in.getNumericValue() == low) return true;
     sleep(1);
   }
   return false;
}

bool Adafruit_FONA::IsRunning(void)
{
int i;
   // wait x seconds till module powered down
   for(i=0;i<15;i++) {
     // check the powerpin in high
     if(pwr_in.getNumericValue() == high) return true;
     sleep(1);
   }
   return false;
}

Adafruit_FONA::~Adafruit_FONA()
{
   delete mySerial;
}


uint8_t Adafruit_FONA::type(void)
{
  return _type;
}

bool Adafruit_FONA::begin()
{
stringstream ss;
string s;
string directionbuff[] = {"0","in","out","both"};

   if(!mySerial->isopen()) {
      if(!mySerial->serialopen()) return false;
   }
   credit_aschar[0] = '-';
   credit_aschar[1] = '-';
   credit_aschar[2] = '.';
   credit_aschar[3] = '-';
   credit_aschar[4] = '-';
   credit_aschar[5] = '\0';
   credit_numeric   = 0.0;
   // GPIOS
   ss.clear(); ss.str("");
   ss << "pwr_out GPIO"  << dec << to_string(pwr_out.getName()) << " "
      << "exported: "  << (pwr_out.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
      << "direction: " << directionbuff[pwr_out.getDirection()] << " "
      << "value: "     << pwr_out.getValue();
   s = ss.str();
   Logger::Write(Logger::INFO,s);
   ss.clear(); ss.str("");
   ss << "pwr_in GPIO"  << dec << to_string(pwr_in.getName()) << " "
      << "exported: "  << (pwr_in.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
      << "direction: " << directionbuff[pwr_in.getDirection()] << " "
      << "value: "     << pwr_in.getValue();
   s = ss.str();
   Logger::Write(Logger::INFO,s);
   ss.clear(); ss.str("");
   ss << "rst_out GPIO"  << dec << to_string(rst_out.getName()) << " "
      << "exported: "  << (rst_out.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
      << "direction: " << directionbuff[rst_out.getDirection()] << " "
      << "value: "     << rst_out.getValue();
   s = ss.str();
   Logger::Write(Logger::INFO,s);
   // hardware reset and power button
   Logger::Write(Logger::INFO,"fona force hardware reset");
   pwr_out.setValue(high);
   rst_out.setValue(high);
   usleep(10000);
   rst_out.setValue(low);
   usleep(100000);
   rst_out.setValue(high);
   usleep(100000);
   // button pressed for 1 second
   if(!Power_On()) {
       Logger::Write(Logger::ERROR,"fona power button failed");
       return false;
   } else {
       Logger::Write(Logger::INFO,"fona power button succeeded");
   }
   // reset and power button end
   DEBUG_PRINTLN("Attempting to open comm with ATs");
   // give 7 seconds to reboot
  if(!LiveCheck(7000)) {
    Logger::Write(Logger::ERROR,"first fona livecheck failed");
    DEBUG_PRINTLN("Timeout: No response to AT... last ditch attempt.");
    if(!LiveCheck(7000)) {
       Logger::Write(Logger::ERROR,"second fona livecheck failed");
    } else poweredon = true;
  } else poweredon = true;
  // turn off Echo!
  sendCheckReply("ATE0", ok_reply);
  usleep(100000);
  if(!sendCheckReply("ATE0", ok_reply)) {
      Logger::Write(Logger::ERROR,"third fona livecheck failed (ATE0)");
      return false;
  }
  // turn on hangupitude
  sendCheckReply("AT+CVHU=0", ok_reply);
  usleep(100000);
  flushInput();
  DEBUG_PRINT("\t---> ");
  DEBUG_PRINTLN("ATI");
  mySerial->println("ATI");
  readline(500, true);
  DEBUG_PRINT("\t<--- ");
  DEBUG_PRINTLN(replybuffer);
  // check module type out
  if (prog_char_strstr(replybuffer,"SIM808 R14") != 0) {
    _type = FONA808_V2;
    Logger::Write(Logger::INFO,"fona is SIM808 R14");
  } else if(prog_char_strstr(replybuffer,"SIM808 R13") != 0) {
    _type = FONA808_V1;
    Logger::Write(Logger::INFO,"fona is SIM808 R13");
  } else if(prog_char_strstr(replybuffer,"SIM800 R13") != 0) {
    _type = FONA800L;
    Logger::Write(Logger::INFO,"fona is SIM800 R13");
  } else if(prog_char_strstr(replybuffer,"SIMCOM_SIM5320A") != 0) {
    _type = FONA3G_A;
    Logger::Write(Logger::INFO,"fona is SIMCOM_SIM5320A");
  } else if(prog_char_strstr(replybuffer,"SIMCOM_SIM5320E") != 0) {
    _type = FONA3G_E;
    Logger::Write(Logger::INFO,"fona is SIMCOM_SIM5320E");
  }
  if (_type == FONA800L) {
    // determine if L or H
    DEBUG_PRINT("\t---> "); DEBUG_PRINTLN("AT+GMM");
    mySerial->println("AT+GMM");
    readline(500, true);
    DEBUG_PRINT("\t<--- ");
    DEBUG_PRINTLN(replybuffer);
    if (prog_char_strstr(replybuffer,"SIM800H") != 0) {
      _type = FONA800H;
      Logger::Write(Logger::INFO,"fona is SIM800H");
    }
  }
  // set strage memory to ME
#if defined(FONA_PREF_SMS_STORAGE)
  if(sendCheckReply("AT+CPMS=" FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE "," FONA_PREF_SMS_STORAGE, "+CPMS: ") != 0) {
      Logger::Write(Logger::ERROR,"fona set memory storage to ME failed (AT+CPMS=)");
  } else {
      Logger::Write(Logger::INFO,"fona set memory storage to ME succeeded (AT+CPMS= 0,255,0,255,0,255)");
  }
#endif
  return true;
}

int Adafruit_FONA::RxLevelCheck()
{
int i;
uint8_t state;

    state = getNetworkStatus();
    if(state == 1) this->fonanet = true;
    else this->fonanet = false;

    for(i=0;i<5;i++) {
        rxpegel_numeric = getRSSI();
        if(rxpegel_numeric == 0) continue;
        else {
           this->fonarssi = true;
           return true;
        }
        sleep(1);
    }
   this->fonarssi = false;
   return false;
}

bool Adafruit_FONA::LiveCheck(uint16_t timeout) {

   while(timeout > 0) {
       mySerial->read();
       while(mySerial->available()) { mySerial->read(); usleep(40000); }
       if(sendCheckReply("AT", ok_reply)) break;
       mySerial->read();
       while(mySerial->available()) { mySerial->read(); usleep(40000); }
       if(sendCheckReply("AT","AT")) break;
       usleep(500000);
       timeout-=500;
   }
   if(timeout <= 0) {
      this->fonalive = false;
      return false;
   } else {
      this->fonalive = true;
      return true;
   }
}

int Adafruit_FONA::CreditCheck()
{
string rbuff;
string tbuff;
size_t length;
stringstream ss;
int pos;
int i;
int retval;
char readArr[255];
char writeArr[255];

    tbuff.clear();
    tbuff = "AT+CUSD=1,\"*100#\",15\r";
    length = tbuff.copy(writeArr,tbuff.size(),0);
    writeArr[length] = '\0';
    mySerial->flush();
    if(!mySerial->write(writeArr,length)) return false;
    for(i=0;i<15;i++) {
        sleep(1);
        mySerial->read(readArr,100);
        rbuff.clear();
        rbuff = readArr;
        // cout << rbuff << endl;
        retval = rbuff.find("EUR");
        if(retval != -1) {
            pos = rbuff.find_first_of('.',0);
            if(pos != -1) {
               length = rbuff.copy(credit_aschar,5,pos-2);
               credit_aschar[length] = '\0';
               rbuff = credit_aschar;
               ss.str("");
               ss.clear();
               ss << rbuff;
               credit_numeric = stof(ss.str());
               // cout << "credit: "  << ss.str() << endl;
               return true;
            }
        }
    }
    return false;
}

bool Adafruit_FONA::SendSms(const string& telnumber, const string& text)
{
stringstream ss;
string tbuff;
int  length;
char message[255];
char number[50];

   // messagetext
   tbuff.clear();
   tbuff = credit_aschar;
   ss.str(""); ss.clear();
   ss << text << " Q:" << dec << rxpegel_numeric << " G:" << tbuff << ".-EUR";
   cout << ss.str() << endl;
   tbuff.clear();
   tbuff = ss.str();
   length = tbuff.copy(message,tbuff.size(),0);
   message[length] = '\0';
   // phonenumber
   tbuff.clear();
   ss.str(""); ss.clear();
   ss << telnumber;
   tbuff = ss.str();
   length = tbuff.copy(number,tbuff.size(),0);
   number[length] = '\0';
   if(!sendSMS(number,message)) {
       ss << "sms send to number:" << tbuff << " failed";
       Logger::Write(Logger::ERROR,ss.str());
       return false;
   }
   else {
       ss << "sms send to number:" << tbuff << " succeeded";
       Logger::Write(Logger::INFO,ss.str());
       return true;
   }
}

/********* Serial port ********************************************/
bool Adafruit_FONA::setBaudrate(uint16_t baud)
{
  return sendCheckReply("AT+IPREX=", baud, ok_reply);
}

/********* Real Time Clock ********************************************/

bool Adafruit_FONA::readRTC(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *hr, uint8_t *min, uint8_t *sec)
{
  uint16_t v;
  sendParseReply("AT+CCLK?", "+CCLK: ", &v, '/', 0);
  *year = v;
  // DEBUG_PRINTLN(*year);
  return true;
}

bool Adafruit_FONA::enableRTC(uint8_t i)
{
  if(!sendCheckReply("AT+CLTS=", i, ok_reply))
    return false;
  return sendCheckReply("AT&W", ok_reply);
}


/********* BATTERY & ADC ********************************************/

/* returns value in mV (uint16_t) */
bool Adafruit_FONA::getBattVoltage(uint16_t *v)
{
  return sendParseReply("AT+CBC","+CBC: ", v, ',', 2);
}

/* returns value in mV (uint16_t) */
bool Adafruit_FONA_3G::getBattVoltage(uint16_t *v)
{
  float f;
  bool b = sendParseReply("AT+CBC", "+CBC: ", &f, ',', 2);
  *v = f*1000;
  return b;
}

/* returns the percentage charge of battery as reported by sim800 */
bool Adafruit_FONA::getBattPercent(uint16_t *p)
{
  return sendParseReply("AT+CBC", "+CBC: ", p, ',', 1);
}

bool Adafruit_FONA::getADCVoltage(uint16_t *v)
{
  return sendParseReply("AT+CADC?", "+CADC: 1,", v);
}

/********* SIM ***********************************************************/

uint8_t Adafruit_FONA::unlockSIM(char *pin)
{
  char sendbuff[14] = "AT+CPIN=";
  sendbuff[8] = pin[0];
  sendbuff[9] = pin[1];
  sendbuff[10] = pin[2];
  sendbuff[11] = pin[3];
  sendbuff[12] = '\0';

  return sendCheckReply(sendbuff, ok_reply);
}

uint8_t Adafruit_FONA::getSIMCCID(char *ccid)
{
  getReply("AT+CCID");
  // up to 28 chars for reply, 20 char total ccid
  if (replybuffer[0] == '+') {
    // fona 3g?
    strncpy(ccid, replybuffer+8, 20);
  } else {
    // fona 800 or 800
    strncpy(ccid, replybuffer, 20);
  }
  ccid[20] = 0;
  readline(); // eat 'OK'
  return strlen(ccid);
}

/********* IMEI **********************************************************/

uint8_t Adafruit_FONA::getIMEI(char *imei)
{
  getReply("AT+GSN");
  // up to 15 chars
  strncpy(imei, replybuffer, 15);
  imei[15] = 0;
  readline(); // eat 'OK'
  return strlen(imei);
}

/********* NETWORK *******************************************************/

uint8_t Adafruit_FONA::getNetworkStatus(void)
{
uint16_t status;

  if(!sendParseReply("AT+CREG?", "+CREG: ", &status, ',', 1)) return 0;
  return static_cast<uint8_t>(status & 0xFF);
}


uint8_t Adafruit_FONA::getRSSI(void)
{
uint16_t reply;

  if(!sendParseReply("AT+CSQ", "+CSQ: ", &reply) ) return 0;
  return static_cast<uint8_t>(reply & 0xFF);
}

/********* AUDIO *******************************************************/

bool Adafruit_FONA::setAudio(uint8_t a)
{
  // 0 is headset, 1 is external audio
  if (a > 1) return false;
  return sendCheckReply("AT+CHFA=", a, ok_reply);
}

uint8_t Adafruit_FONA::getVolume(void)
{
uint16_t reply;

  if(!sendParseReply("AT+CLVL?", "+CLVL: ", &reply) ) return 0;
  return static_cast<uint8_t>(reply & 0xFF);
}

bool Adafruit_FONA::setVolume(uint8_t i)
{
  return sendCheckReply("AT+CLVL=", i, ok_reply);
}

bool Adafruit_FONA::playDTMF(char dtmf)
{
  char str[4];
  str[0] = '\"';
  str[1] = dtmf;
  str[2] = '\"';
  str[3] = 0;
  return sendCheckReply("AT+CLDTMF=3,", str, ok_reply);
}

bool Adafruit_FONA::playToolkitTone(uint8_t t, uint16_t len)
{
  return sendCheckReply("AT+STTONE=1,", t, len, ok_reply);
}

bool Adafruit_FONA_3G::playToolkitTone(uint8_t t, uint16_t len)
{
  if(!sendCheckReply("AT+CPTONE=", t, ok_reply)) return false;
  usleep(len*1000);
  return sendCheckReply("AT+CPTONE=0", ok_reply);
}

bool Adafruit_FONA::setMicVolume(uint8_t a, uint8_t level)
{
  // 0 is headset, 1 is external audio
  if (a > 1) return false;
  return sendCheckReply("AT+CMIC=", a, level, ok_reply);
}

/********* FM RADIO *******************************************************/


bool Adafruit_FONA::FMradio(bool onoff, uint8_t a)
{
  if(!onoff) {
    return sendCheckReply("AT+FMCLOSE", ok_reply);
  }
  // 0 is headset, 1 is external audio
  if (a > 1) return false;
  return sendCheckReply("AT+FMOPEN=", a, ok_reply);
}

bool Adafruit_FONA::tuneFMradio(uint16_t station)
{
  // Fail if FM station is outside allowed range.
  if ((station < 870) || (station > 1090)) return false;
  return sendCheckReply("AT+FMFREQ=", station, ok_reply);
}

bool Adafruit_FONA::setFMVolume(uint8_t i)
{
  // Fail if volume is outside allowed range (0-6).
  if (i > 6) return false;
  // Send FM volume command and verify response.
  return sendCheckReply("AT+FMVOLUME=", i, ok_reply);
}

int8_t Adafruit_FONA::getFMVolume()
{
uint16_t level;

  if(!sendParseReply("AT+FMVOLUME?", "+FMVOLUME: ", &level) ) return 0;
  return level;
}

int8_t Adafruit_FONA::getFMSignalLevel(uint16_t station)
{
  // Fail if FM station is outside allowed range.
  if ((station < 875) || (station > 1080)) return -1;
  // Send FM signal level query command.
  // Note, need to explicitly send timeout so right overload is chosen.
  getReply("AT+FMSIGNAL=", station, FONA_DEFAULT_TIMEOUT_MS);
  // Check response starts with expected value.
  char *p = prog_char_strstr(replybuffer, "+FMSIGNAL: ");
  if (p == 0) return -1;
  p+=11;
  // Find second colon to get start of signal quality.
  p = strchr(p, ':');
  if (p == 0) return -1;
  p+=1;
  // Parse signal quality.
  int8_t level = atoi(p);
  readline();  // eat the "OK"
  return level;
}

/********* PWM/BUZZER **************************************************/

bool Adafruit_FONA::setPWM(uint16_t period, uint8_t duty)
{
  if(period > 2000) return false;
  if(duty   >  100) return false;
  return sendCheckReply("AT+SPWM=0,", period, duty, ok_reply);
}

/********* CALL PHONES **************************************************/
bool Adafruit_FONA::callPhone(char *number)
{
  char sendbuff[35] = "ATD";
  strncpy(sendbuff+3, number, min(30, (int)strlen(number)));
  uint8_t x = strlen(sendbuff);
  sendbuff[x] = ';';
  sendbuff[x+1] = 0;
  // DEBUG_PRINTLN(sendbuff);
  return sendCheckReply(sendbuff, ok_reply);
}


uint8_t Adafruit_FONA::getCallStatus(void)
{
uint16_t phoneStatus;
  // 1, since 0 is actually a known, good reply
  if(!sendParseReply("AT+CPAS", "+CPAS: ", &phoneStatus)) return FONA_CALL_FAILED;
  // 0 ready, 2 unkown, 3 ringing, 4 call in progress
  return phoneStatus;
}

bool Adafruit_FONA::hangUp(void)
{
  return sendCheckReply("ATH0", ok_reply);
}

bool Adafruit_FONA_3G::hangUp(void)
{
  getReply("ATH");
  return (prog_char_strstr(replybuffer,"VOICE CALL: END") != 0);
}

bool Adafruit_FONA::pickUp(void)
{
  return sendCheckReply("ATA", ok_reply);
}

bool Adafruit_FONA_3G::pickUp(void)
{
  return sendCheckReply("ATA", "VOICE CALL: BEGIN");
}


void Adafruit_FONA::onIncomingCall() {

  DEBUG_PRINT("> "); DEBUG_PRINTLN("Incoming call...");
  Adafruit_FONA::_incomingCall = true;
}

bool Adafruit_FONA::_incomingCall = false;

bool Adafruit_FONA::callerIdNotification(bool enable, uint8_t interrupt)
{
  if(enable){
    //attachInterrupt(interrupt, onIncomingCall, FALLING);
    return sendCheckReply("AT+CLIP=1", ok_reply);
  }
  //detachInterrupt(interrupt);
  return sendCheckReply("AT+CLIP=0", ok_reply);
}

bool Adafruit_FONA::incomingCallNumber(char* phonenum)
{
  //+CLIP: "<incoming phone number>",145,"",0,"",0
  if(!Adafruit_FONA::_incomingCall) return false;
  readline();
  while(!prog_char_strcmp(replybuffer,"RING") == 0) {
    flushInput();
    readline();
  }
  readline(); //reads incoming phone number line
  parseReply("+CLIP: \"", phonenum, '"');
  DEBUG_PRINT("Phone Number: ");
  DEBUG_PRINTLN(replybuffer);
  Adafruit_FONA::_incomingCall = false;
  return true;
}

/********* SMS **********************************************************/

uint8_t Adafruit_FONA::getSMSInterrupt(void)
{
uint16_t reply;

  if(!sendParseReply("AT+CFGRI?", "+CFGRI: ", &reply) ) return 0;
  return reply;
}

bool Adafruit_FONA::setSMSInterrupt(uint8_t i)
{
  return sendCheckReply("AT+CFGRI=", i, ok_reply);
}

int8_t Adafruit_FONA::getNumSMS(void)
{
uint16_t numsms;

  // get into text mode
  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return -1;
  // ask how many sms are stored
  if(sendParseReply("AT+CPMS?", FONA_PREF_SMS_STORAGE ",", &numsms)) return numsms;
  if(sendParseReply("AT+CPMS?", "\"SM\",", &numsms))   return numsms;
  if(sendParseReply("AT+CPMS?", "\"SM_P\",", &numsms)) return numsms;
  return -1;
}

// Reading SMS's is a bit involved so we don't use helpers that may cause delays or debug
// printouts!
bool Adafruit_FONA::readSMS(uint8_t i, char *smsbuff, uint16_t maxlen, uint16_t *readlen)
{
  // text mode
  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return false;
  // show all text mode parameters
  if(!sendCheckReply("AT+CSDH=1", ok_reply)) return false;
  // parse out the SMS len
  uint16_t thesmslen = 0;
  //DEBUG_PRINT("AT+CMGR=");
  //DEBUG_PRINTLN(static_cast<const char*>(to_string(i)));
  //getReply("AT+CMGR=", i, 1000);  //  do not print debug!
  mySerial->print("AT+CMGR=");
  mySerial->println(i);
  readline(1000); // timeout
  //DEBUG_PRINT(F("Reply: ")); DEBUG_PRINTLN(replybuffer);
  // parse it out...
  DEBUG_PRINTLN(replybuffer);
  if(!parseReply("+CMGR:", &thesmslen, ',', 11)) {
    *readlen = 0;
    return false;
  }
  readRaw(thesmslen);
  flushInput();
  uint16_t thelen = min(maxlen, (uint16_t)strlen(replybuffer));
  strncpy(smsbuff, replybuffer, thelen);
  smsbuff[thelen] = 0; // end the string
  DEBUG_PRINTLN(replybuffer);
  *readlen = thelen;
  return true;
}

// Retrieve the sender of the specified SMS message and copy it as a string to
// the sender buffer.  Up to senderlen characters of the sender will be copied
// and a null terminator will be added if less than senderlen charactesr are
// copied to the result.  Returns true if a result was successfully retrieved,
// otherwise false.
bool Adafruit_FONA::getSMSSender(uint8_t i, char *sender, int senderlen)
{
  // Ensure text mode and all text mode parameters are sent.
  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return false;
  if(!sendCheckReply("AT+CSDH=1", ok_reply)) return false;
  //DEBUG_PRINT("AT+CMGR=");
  //DEBUG_PRINTLN(i);
  // Send command to retrieve SMS message and parse a line of response.
  mySerial->print("AT+CMGR=");
  mySerial->println(i);
  readline(1000);
  DEBUG_PRINTLN(replybuffer);
  // Parse the second field in the response.
  bool result = parseReplyQuoted("+CMGR:", sender, senderlen, ',', 1);
  // Drop any remaining data from the response.
  flushInput();
  return result;
}

bool Adafruit_FONA::sendSMS(char *smsaddr, char *smsmsg)
{
stringstream ss;

  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return false;
  char sendcmd[30] = "AT+CMGS=\"";
  // 9 bytes beginning, 2 bytes for close quote + null
  strncpy(sendcmd+9, smsaddr, 30-9-2);
  sendcmd[strlen(sendcmd)] = '\"';
  if(!sendCheckReply(sendcmd,"> ",2000)) {
      ss << "no > :o( "
         << "SendCMD: " << sendcmd << " failed";
      Logger::Write(Logger::ERROR,ss.str());
      return false;
  }
  DEBUG_PRINT("> ");
  cout << smsmsg;
  mySerial->println(smsmsg);
  mySerial->println();
  mySerial->write(0x1A);
  DEBUG_PRINTLN("^Z");
  if((_type == FONA3G_A) || (_type == FONA3G_E)) {
    // Eat two sets of CRLF
    readline(200);
    DEBUG_PRINT("Line 1: "); DEBUG_PRINTLN(replybuffer);
    readline(200);
    DEBUG_PRINT("Line 2: "); DEBUG_PRINTLN(replybuffer);
  }
  readline(10000);
  // read the +CMGS reply, wait up to 10 seconds!!!
  //DEBUG_PRINT("Line 3: "); DEBUG_PRINTLN(strlen(replybuffer));
  if(strstr(replybuffer, "+CMGS") == 0) {
      ss << "no +CMGS reply was: "
         << replybuffer;
      Logger::Write(Logger::ERROR,ss.str());
      return false;
  }
  readline(1000); // read OK
  //DEBUG_PRINT("* "); DEBUG_PRINTLN(replybuffer);
  if(strcmp(replybuffer, "OK") != 0) return false;
  return true;
}


bool Adafruit_FONA::deleteSMS(uint8_t i)
{
  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return false;
  // read an sms
  char sendbuff[12] = "AT+CMGD=000";
  sendbuff[8] = (i / 100) + '0';
  i %= 100;
  sendbuff[9] = (i / 10) + '0';
  i %= 10;
  sendbuff[10] = i + '0';
  return sendCheckReply(sendbuff, ok_reply, 2000);
}

bool Adafruit_FONA::deleteSMS_all()
{
  if(!sendCheckReply("AT+CMGF=1", ok_reply)) return false;
  char sendbuff[12] = "AT+CMGD=1,4";
  return sendCheckReply(sendbuff, ok_reply, 2000);
}


/********* USSD *********************************************************/

bool Adafruit_FONA::sendUSSD(char *ussdmsg, char *ussdbuff, uint16_t maxlen, uint16_t *readlen)
{
  if(!sendCheckReply("AT+CUSD=1", ok_reply)) return false;
  char sendcmd[30] = "AT+CUSD=1,\"";
  strncpy(sendcmd+11, ussdmsg, 30-11-2);  // 11 bytes beginning, 2 bytes for close quote + null
  sendcmd[strlen(sendcmd)] = '\"';
  if(!sendCheckReply(sendcmd, ok_reply)) {
    *readlen = 0;
    return false;
  } else {
      readline(10000); // read the +CUSD reply, wait up to 10 seconds!!!
      //DEBUG_PRINT("* "); DEBUG_PRINTLN(replybuffer);
      char *p = prog_char_strstr(replybuffer, "+CUSD: ");
      if (p == 0) {
        *readlen = 0;
        return false;
      }
      p+=7; //+CUSD
      // Find " to get start of ussd message.
      p = strchr(p, '\"');
      if (p == 0) {
        *readlen = 0;
        return false;
      }
      p+=1; //"
      // Find " to get end of ussd message.
      char *strend = strchr(p, '\"');
      uint16_t lentocopy = min(maxlen-1, strend - p);
      strncpy(ussdbuff, p, lentocopy+1);
      ussdbuff[lentocopy] = 0;
      *readlen = lentocopy;
  }
  return true;
}


/********* TIME **********************************************************/

bool Adafruit_FONA::enableNetworkTimeSync(bool onoff)
{
  if (onoff) {
    if(!sendCheckReply("AT+CLTS=1", ok_reply)) return false;
  } else {
    if(!sendCheckReply("AT+CLTS=0", ok_reply)) return false;
  }
  flushInput(); // eat any 'Unsolicted Result Code'
  return true;
}

bool Adafruit_FONA::enableNTPTimeSync(bool onoff, FONAFlashStringPtr ntpserver)
{
  if (onoff) {
    if(!sendCheckReply("AT+CNTPCID=1", ok_reply)) return false;
    mySerial->print("AT+CNTP=\"");
    if(ntpserver != 0) {
      mySerial->print(ntpserver);
    } else {
      mySerial->print("pool.ntp.org");
    }
    mySerial->println("\",0");
    readline(FONA_DEFAULT_TIMEOUT_MS);
    if(strcmp(replybuffer, "OK") != 0) return false;
    if(!sendCheckReply("AT+CNTP", ok_reply, 10000)) return false;
    uint16_t status;
    readline(10000);
    if(!parseReply("+CNTP:", &status)) return false;
  } else {
    if(!sendCheckReply("AT+CNTPCID=0", ok_reply)) return false;
  }
  return true;
}

bool Adafruit_FONA::getTime(char *buff, uint16_t maxlen)
{
  getReply("AT+CCLK?", (uint16_t) 10000);
  if (strncmp(replybuffer, "+CCLK: ", 7) != 0) return false;
  char *p = replybuffer+7;
  uint16_t lentocopy = min(maxlen-1, (int)strlen(p));
  strncpy(buff, p, lentocopy+1);
  buff[lentocopy] = 0;
  readline(); // eat OK
  return true;
}

/********* GPS **********************************************************/


bool Adafruit_FONA::enableGPS(bool onoff)
{
uint16_t state;

  // first check if its already on or off
  if (_type == FONA808_V2) {
    if(!sendParseReply("AT+CGNSPWR?", "+CGNSPWR: ", &state)) return false;
  } else {
    if(!sendParseReply("AT+CGPSPWR?", "+CGPSPWR: ", &state)) return false;
  }
  if(onoff && !state) {
    if(_type == FONA808_V2) {
      // try GNS command
      if(!sendCheckReply("AT+CGNSPWR=1", ok_reply)) return false;
    } else {
      if(!sendCheckReply("AT+CGPSPWR=1", ok_reply)) return false;
    }
  } else if(!onoff && state) {
    if(_type == FONA808_V2) {
      // try GNS command
      if(!sendCheckReply("AT+CGNSPWR=0", ok_reply)) return false;
    } else {
      if(!sendCheckReply("AT+CGPSPWR=0", ok_reply)) return false;
    }
  }
  return true;
}



bool Adafruit_FONA_3G::enableGPS(bool onoff)
{
uint16_t state;

  // first check if its already on or off
  if(!Adafruit_FONA::sendParseReply("AT+CGPS?", "+CGPS: ", &state)) return false;
  if (onoff && !state) {
    if(!sendCheckReply("AT+CGPS=1", ok_reply)) return false;
  } else if (!onoff && state) {
    if(!sendCheckReply("AT+CGPS=0", ok_reply)) return false;
    // this takes a little time
    readline(2000); // eat '+CGPS: 0'
  }
  return true;
}

int8_t Adafruit_FONA::GPSstatus(void)
{
  if (_type == FONA808_V2) {
    // 808 V2 uses GNS commands and doesn't have an explicit 2D/3D fix status.
    // Instead just look for a fix and if found assume it's a 3D fix.
    getReply("AT+CGNSINF");
    char *p = prog_char_strstr(replybuffer,"+CGNSINF: ");
    if (p == 0) return -1;
    p+=10;
    readline(); // eat 'OK'
    if (p[0] == '0') return 0; // GPS is not even on!
    p+=2; // Skip to second value, fix status.
    //DEBUG_PRINTLN(p);
    // Assume if the fix status is '1' then we have a 3D fix, otherwise no fix.
    if (p[0] == '1') return 3;
    else return 1;
  }
  if (_type == FONA3G_A || _type == FONA3G_E) {
    // FONA 3G doesn't have an explicit 2D/3D fix status.
    // Instead just look for a fix and if found assume it's a 3D fix.
    getReply("AT+CGPSINFO");
    char *p = prog_char_strstr(replybuffer,"+CGPSINFO:");
    if (p == 0) return -1;
    if (p[10] != ',') return 3; // if you get anything, its 3D fix
    return 0;
  }
  else {
    // 808 V1 looks for specific 2D or 3D fix state.
    getReply("AT+CGPSSTATUS?");
    char *p = prog_char_strstr(replybuffer, "SSTATUS: Location ");
    if (p == 0) return -1;
    p+=18;
    readline(); // eat 'OK'
    //DEBUG_PRINTLN(p);
    if (p[0] == 'U') return 0;
    if (p[0] == 'N') return 1;
    if (p[0] == '2') return 2;
    if (p[0] == '3') return 3;
  }
  // else
  return 0;
}

uint8_t Adafruit_FONA::getGPS(uint8_t arg, char *buffer, uint8_t maxbuff)
{
int32_t x = arg;

  if ( (_type == FONA3G_A) || (_type == FONA3G_E) ) {
    getReply("AT+CGPSINFO");
  } else if (_type == FONA808_V1) {
    getReply("AT+CGPSINF=", x);
  } else {
    getReply("AT+CGNSINF");
  }

  char *p = prog_char_strstr(replybuffer, "SINF");
  if (p == 0) {
    buffer[0] = 0;
    return 0;
  }
  p+=6;
  uint8_t len = max(maxbuff-1, (int)strlen(p));
  strncpy(buffer, p, len);
  buffer[len] = 0;
  readline(); // eat 'OK'
  return len;
}

bool Adafruit_FONA::getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude)
{
char gpsbuffer[120];

  // we need at least a 2D fix
  if(GPSstatus() < 2) return false;
  // grab the mode 2^5 gps csv from the sim808
  uint8_t res_len = getGPS(32, gpsbuffer, 120);
  // make sure we have a response
  if(res_len == 0) return false;

  if(_type == FONA3G_A || _type == FONA3G_E) {
    // Parse 3G respose
    // +CGPSINFO:4043.000000,N,07400.000000,W,151015,203802.1,-12.0,0.0,0
    // skip beginning
    char *tok;
   // grab the latitude
    char *latp = strtok(gpsbuffer, ",");
    if (! latp) return false;
    // grab latitude direction
    char *latdir = strtok(NULL, ",");
    if (! latdir) return false;
    // grab longitude
    char *longp = strtok(NULL, ",");
    if (! longp) return false;
    // grab longitude direction
    char *longdir = strtok(NULL, ",");
    if (! longdir) return false;
    // skip date & time
    tok = strtok(NULL, ",");
    tok = strtok(NULL, ",");
   // only grab altitude if needed
    if (altitude != NULL) {
      // grab altitude
      char *altp = strtok(NULL, ",");
      if (! altp) return false;
      *altitude = atof(altp);
    }
    // only grab speed if needed
    if (speed_kph != NULL) {
      // grab the speed in km/h
      char *speedp = strtok(NULL, ",");
      if (! speedp) return false;
      *speed_kph = atof(speedp);
    }
    // only grab heading if needed
    if (heading != NULL) {
      // grab the speed in knots
      char *coursep = strtok(NULL, ",");
      if (! coursep) return false;
      *heading = atof(coursep);
    }
    double latitude = atof(latp);
    double longitude = atof(longp);
    // convert latitude from minutes to decimal
    float degrees = floor(latitude / 100);
    double minutes = latitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;
    // turn direction into + or -
    if (latdir[0] == 'S') degrees *= -1;
    *lat = degrees;
    // convert longitude from minutes to decimal
    degrees = floor(longitude / 100);
    minutes = longitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;
    // turn direction into + or -
    if (longdir[0] == 'W') degrees *= -1;
    *lon = degrees;
  } else if (_type == FONA808_V2) {
    // Parse 808 V2 response.  See table 2-3 from here for format:
    // http://www.adafruit.com/datasheets/SIM800%20Series_GNSS_Application%20Note%20V1.00.pdf
    // skip GPS run status
    char *tok = strtok(gpsbuffer, ",");
    if (! tok) return false;
    // skip fix status
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // skip date
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // grab the latitude
    char *latp = strtok(NULL, ",");
    if (! latp) return false;
    // grab longitude
    char *longp = strtok(NULL, ",");
    if (! longp) return false;
    *lat = atof(latp);
    *lon = atof(longp);
    // only grab altitude if needed
    if (altitude != NULL) {
      // grab altitude
      char *altp = strtok(NULL, ",");
      if (! altp) return false;
      *altitude = atof(altp);
    }
    // only grab speed if needed
    if (speed_kph != NULL) {
      // grab the speed in km/h
      char *speedp = strtok(NULL, ",");
      if (! speedp) return false;
      *speed_kph = atof(speedp);
    }
    // only grab heading if needed
    if (heading != NULL) {
      // grab the speed in knots
      char *coursep = strtok(NULL, ",");
      if (! coursep) return false;
      *heading = atof(coursep);
    }
  }
  else {
    // Parse 808 V1 response.
    // skip mode
    char *tok = strtok(gpsbuffer, ",");
    if (! tok) return false;
    // skip date
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // skip fix
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // grab the latitude
    char *latp = strtok(NULL, ",");
    if (! latp) return false;
    // grab latitude direction
    char *latdir = strtok(NULL, ",");
    if (! latdir) return false;
    // grab longitude
    char *longp = strtok(NULL, ",");
    if (! longp) return false;
    // grab longitude direction
    char *longdir = strtok(NULL, ",");
    if (! longdir) return false;
    double latitude = atof(latp);
    double longitude = atof(longp);
    // convert latitude from minutes to decimal
    float degrees = floor(latitude / 100);
    double minutes = latitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;
    // turn direction into + or -
    if (latdir[0] == 'S') degrees *= -1;
    *lat = degrees;
    // convert longitude from minutes to decimal
    degrees = floor(longitude / 100);
    minutes = longitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;
    // turn direction into + or -
    if (longdir[0] == 'W') degrees *= -1;
    *lon = degrees;
    // only grab speed if needed
    if (speed_kph != NULL) {
      // grab the speed in knots
      char *speedp = strtok(NULL, ",");
      if (! speedp) return false;
      // convert to kph
      *speed_kph = atof(speedp) * 1.852;
    }
    // only grab heading if needed
    if (heading != NULL) {
      // grab the speed in knots
      char *coursep = strtok(NULL, ",");
      if (! coursep) return false;
      *heading = atof(coursep);
    }
    // no need to continue
    if(altitude == NULL) return true;
    // we need at least a 3D fix for altitude
    if(GPSstatus() < 3) return false;
    // grab the mode 0 gps csv from the sim808
    res_len = getGPS(0, gpsbuffer, 120);
    // make sure we have a response
    if(res_len == 0) return false;
    // skip mode
    tok = strtok(gpsbuffer, ",");
    if (! tok) return false;
    // skip lat
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // skip long
    tok = strtok(NULL, ",");
    if (! tok) return false;
    // grab altitude
    char *altp = strtok(NULL, ",");
    if (! altp) return false;
    *altitude = atof(altp);
  }
  return true;
}

bool Adafruit_FONA::enableGPSNMEA(uint8_t i)
{
char sendbuff[15] = "AT+CGPSOUT=000";

  sendbuff[11] = (i / 100) + '0';
  i %= 100;
  sendbuff[12] = (i / 10) + '0';
  i %= 10;
  sendbuff[13] = i + '0';
  if (_type == FONA808_V2) {
    if(i)
      return sendCheckReply("AT+CGNSTST=1", ok_reply);
    else
      return sendCheckReply("AT+CGNSTST=0", ok_reply);
  } else {
    return sendCheckReply(sendbuff, ok_reply, 2000);
  }
}


/********* GPRS **********************************************************/


bool Adafruit_FONA::enableGPRS(bool onoff)
{

  if (onoff) {
    // disconnect all sockets
    sendCheckReply("AT+CIPSHUT","SHUT OK", 20000);
    if(!sendCheckReply("AT+CGATT=1", ok_reply, 10000)) return false;
    // set bearer profile! connection type GPRS
    if(!sendCheckReply("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", ok_reply, 10000)) return false;
    // set bearer profile access point name
    if (apn) {
      // Send command AT+SAPBR=3,1,"APN","<apn value>" where <apn value> is the configured APN value.
      if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"APN\",", apn, ok_reply, 10000)) return false;
      // send AT+CSTT,"apn","user","pass"
      flushInput();
      mySerial->print("AT+CSTT=\"");
      mySerial->print(apn);
      if (apnusername) {
        mySerial->print("\",\"");
        mySerial->print(apnusername);
      }
      if (apnpassword) {
        mySerial->print("\",\"");
        mySerial->print(apnpassword);
      }
      mySerial->println("\"");

      DEBUG_PRINT("\t---> "); DEBUG_PRINT("AT+CSTT=\"");
      DEBUG_PRINT(apn);
      if (apnusername) {
        DEBUG_PRINT("\",\"");
        DEBUG_PRINT(apnusername);
      }
      if (apnpassword) {
        DEBUG_PRINT("\",\"");
        DEBUG_PRINT(apnpassword);
      }
      DEBUG_PRINTLN("\"");
      if (! expectReply(ok_reply)) return false;
      // set username/password
      if (apnusername) {
        // Send command AT+SAPBR=3,1,"USER","<user>" where <user> is the configured APN username.
        if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"USER\",", apnusername, ok_reply, 10000)) return false;
      }
      if (apnpassword) {
        // Send command AT+SAPBR=3,1,"PWD","<password>" where <password> is the configured APN password.
        if(!sendCheckReplyQuoted("AT+SAPBR=3,1,\"PWD\",", apnpassword, ok_reply, 10000)) return false;
      }
    }
    // open GPRS context
    if(!sendCheckReply("AT+SAPBR=1,1", ok_reply, 30000)) return false;
    // bring up wireless connection
    if(!sendCheckReply("AT+CIICR", ok_reply, 10000)) return false;
  } else {
    // disconnect all sockets
    if(!sendCheckReply("AT+CIPSHUT", "SHUT OK", 20000)) return false;
    // close GPRS context
    if(!sendCheckReply("AT+SAPBR=0,1", ok_reply, 10000)) return false;
    if(!sendCheckReply("AT+CGATT=0", ok_reply, 10000)) return false;
  }
  return true;
}

bool Adafruit_FONA_3G::enableGPRS(bool onoff)
{
  if (onoff) {
    // disconnect all sockets
    //sendCheckReply(F("AT+CIPSHUT"), F("SHUT OK"), 5000);
    if(!sendCheckReply("AT+CGATT=1", ok_reply, 10000)) return false;
    // set bearer profile access point name
    if(apn) {
      // Send command AT+CGSOCKCONT=1,"IP","<apn value>" where <apn value> is the configured APN name.
      if(!sendCheckReplyQuoted("AT+CGSOCKCONT=1,\"IP\",", apn, ok_reply, 10000)) return false;
      // set username/password
      if (apnusername) {
         char authstring[100] = "AT+CGAUTH=1,1,\"";
         char *strp = authstring + strlen(authstring);
         prog_char_strcpy(strp,apnusername);
	     strp+=prog_char_strlen(apnusername);
         strp[0] = '\"';
         strp++;
         strp[0] = 0;
         if (apnpassword) {
            strp[0] = ','; strp++;
            strp[0] = '\"'; strp++;
            prog_char_strcpy(strp, apnpassword);
            strp+=prog_char_strlen(apnpassword);
            strp[0] = '\"';
            strp++;
	        strp[0] = 0;
         }
         if(!sendCheckReply(authstring, ok_reply, 10000)) return false;
      }
    }
    // connect in transparent
    if(!sendCheckReply("AT+CIPMODE=1", ok_reply, 10000)) return false;
    // open network (?)
    if(!sendCheckReply("AT+NETOPEN=,,1","Network opened", 10000)) return false;
    readline(); // eat 'OK'
  } else {
    // close GPRS context
    if(!sendCheckReply("AT+NETCLOSE","Network closed", 10000)) return false;
    readline(); // eat 'OK'
  }
  return true;
}

uint8_t Adafruit_FONA::GPRSstate(void)
{
uint16_t state;

  if(!sendParseReply("AT+CGATT?", "+CGATT: ", &state)) return -1;
  return state;
}

void Adafruit_FONA::setGPRSNetworkSettings(FONAFlashStringPtr apn, FONAFlashStringPtr username, FONAFlashStringPtr password)
{
  this->apn = apn;
  this->apnusername = username;
  this->apnpassword = password;
}

bool Adafruit_FONA::getGSMLoc(uint16_t *errorcode, char *buff, uint16_t maxlen)
{
  getReply("AT+CIPGSMLOC=1,1", (uint16_t)10000);
  if(!parseReply("+CIPGSMLOC: ", errorcode)) return false;
  char *p = replybuffer+14;
  uint16_t lentocopy = min(maxlen-1, (int)strlen(p));
  strncpy(buff, p, lentocopy+1);
  readline(); // eat OK
  return true;
}

bool Adafruit_FONA::getGSMLoc(float *lat, float *lon)
{
uint16_t returncode;
char gpsbuffer[120];

  // make sure we could get a response
  if(!getGSMLoc(&returncode, gpsbuffer, 120)) return false;
  // make sure we have a valid return code
  if(returncode != 0) return false;
  // +CIPGSMLOC: 0,-74.007729,40.730160,2015/10/15,19:24:55
  // tokenize the gps buffer to locate the lat & long
  char *longp = strtok(gpsbuffer, ",");
  if(!longp) return false;
  char *latp = strtok(NULL, ",");
  if(!latp) return false;
  *lat = atof(latp);
  *lon = atof(longp);
  return true;

}
/********* TCP FUNCTIONS  ************************************/


bool Adafruit_FONA::TCPconnect(char *server, uint16_t port)
{
  flushInput();

  // close all old connections
  if(!sendCheckReply("AT+CIPSHUT", "SHUT OK", 20000) ) return false;

  // single connection at a time
  if(!sendCheckReply("AT+CIPMUX=0", ok_reply)) return false;

  // manually read data
  if(!sendCheckReply("AT+CIPRXGET=1", ok_reply)) return false;


  DEBUG_PRINT("AT+CIPSTART=\"TCP\",\"");
  DEBUG_PRINT(server);
  DEBUG_PRINT("\",\"");
  //DEBUG_PRINT(port);
  DEBUG_PRINTLN("\"");

  mySerial->print("AT+CIPSTART=\"TCP\",\"");
  mySerial->print(server);
  mySerial->print("\",\"");
  mySerial->print(port);
  mySerial->println("\"");

  if(!expectReply(ok_reply)) return false;
  if(!expectReply("CONNECT OK")) return false;
  // looks like it was a success (?)
  return true;
}

bool Adafruit_FONA::TCPclose(void)
{
  return sendCheckReply("AT+CIPCLOSE", ok_reply);
}

bool Adafruit_FONA::TCPconnected(void)
{
  if(!sendCheckReply("AT+CIPSTATUS", ok_reply, 100)) return false;
  readline(100);
  DEBUG_PRINT ("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return (strcmp(replybuffer, "STATE: CONNECT OK") == 0);
}

bool Adafruit_FONA::TCPsend(char *packet, uint8_t len)
{
  DEBUG_PRINT("AT+CIPSEND=");
  // DEBUG_PRINTLN(len);
#ifdef ADAFRUIT_FONA_DEBUG
  for (uint16_t i=0; i<len; i++) {
  DEBUG_PRINT(" 0x");
  DEBUG_PRINT(packet[i], HEX);
  }
#endif
  //DEBUG_PRINTLN();
  mySerial->print("AT+CIPSEND=");
  mySerial->println(len);
  readline();
  DEBUG_PRINT("\t<--- ");
  DEBUG_PRINTLN(replybuffer);

  if(replybuffer[0] != '>') return false;
  mySerial->write(packet, len);
  readline(3000); // wait up to 3 seconds to send the data

  DEBUG_PRINT("\t<--- ");
  DEBUG_PRINTLN(replybuffer);

  return (strcmp(replybuffer, "SEND OK") == 0);
}

uint16_t Adafruit_FONA::TCPavailable(void)
{
uint16_t avail;

  if(!sendParseReply("AT+CIPRXGET=4", "+CIPRXGET: 4,", &avail, ',', 0) ) return false;
  //DEBUG_PRINT (avail); DEBUG_PRINTLN(F(" bytes available"));
  return avail;
}


uint16_t Adafruit_FONA::TCPread(uint8_t *buff, uint8_t len)
{
uint16_t avail;

  mySerial->print("AT+CIPRXGET=2,");
  mySerial->println(len);
  readline();
  if (!parseReply("+CIPRXGET: 2,", &avail, ',', 0)) return false;
  readRaw(avail);
#ifdef ADAFRUIT_FONA_DEBUG
  DEBUG_PRINT (avail); DEBUG_PRINTLN(F(" bytes read"));
  for (uint8_t i=0;i<avail;i++) {
  DEBUG_PRINT(F(" 0x")); DEBUG_PRINT(replybuffer[i], HEX);
  }
  DEBUG_PRINTLN();
#endif
  memcpy(buff, replybuffer, avail);
  return avail;
}



/********* HTTP LOW LEVEL FUNCTIONS  ************************************/

bool Adafruit_FONA::HTTP_init()
{
  return sendCheckReply("AT+HTTPINIT", ok_reply);
}

bool Adafruit_FONA::HTTP_term()
{
  return sendCheckReply("AT+HTTPTERM", ok_reply);
}

void Adafruit_FONA::HTTP_para_start(FONAFlashStringPtr parameter, bool quoted) {
  flushInput();
  DEBUG_PRINT("\t---> ");
  DEBUG_PRINT("AT+HTTPPARA=\"");
  DEBUG_PRINT(parameter);
  //DEBUG_PRINTLN('"');

  mySerial->print("AT+HTTPPARA=\"");
  mySerial->print(parameter);
  if(quoted) mySerial->print("\",\"");
  else       mySerial->print("\",");
}

bool Adafruit_FONA::HTTP_para_end(bool quoted)
{
  if(quoted) mySerial->println('"');
  else       mySerial->println();
  return expectReply(ok_reply);
}
/*
bool Adafruit_FONA::HTTP_para(FONAFlashStringPtr parameter, const char *value)
{
  HTTP_para_start(parameter, true);
  mySerial->print(value);
  return HTTP_para_end(true);
}
*/
bool Adafruit_FONA::HTTP_para(FONAFlashStringPtr parameter, FONAFlashStringPtr value)
{
  HTTP_para_start(parameter, true);
  mySerial->print(value);
  return HTTP_para_end(true);
}

bool Adafruit_FONA::HTTP_para(FONAFlashStringPtr parameter, int32_t value)
{
  HTTP_para_start(parameter, false);
  mySerial->print(value);
  return HTTP_para_end(false);
}

bool Adafruit_FONA::HTTP_data(uint32_t size, uint32_t maxTime)
{
  flushInput();
  DEBUG_PRINT("\t---> ");
  DEBUG_PRINT("AT+HTTPDATA=");
  //DEBUG_PRINT(size);
  //DEBUG_PRINT(',');
  //DEBUG_PRINTLN(maxTime);
  mySerial->print("AT+HTTPDATA=");
  mySerial->print(size);
  mySerial->print(",");
  mySerial->println(maxTime);
  return expectReply("DOWNLOAD");
}

bool Adafruit_FONA::HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout)
{
  // Send request.
  if(!sendCheckReply("AT+HTTPACTION=", method, ok_reply)) return false;
  // Parse response status and size.
  readline(timeout);
  if(!parseReply("+HTTPACTION:", status, ',', 1)) return false;
  if(!parseReply("+HTTPACTION:", datalen, ',', 2)) return false;
  return true;
}

bool Adafruit_FONA::HTTP_readall(uint16_t *datalen)
{
  getReply("AT+HTTPREAD");
  if(!parseReply("+HTTPREAD:", datalen, ',', 0)) return false;
  return true;
}

bool Adafruit_FONA::HTTP_ssl(bool onoff)
{
  return sendCheckReply("AT+HTTPSSL=", onoff ? 1 : 0, ok_reply);
}

/********* HTTP HIGH LEVEL FUNCTIONS ***************************/

bool Adafruit_FONA::HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen){
  if(!HTTP_setup(url)) return false;
  // HTTP GET
  if(!HTTP_action(FONA_HTTP_GET, status, datalen, 30000)) return false;
  //DEBUG_PRINT(F("Status: ")); DEBUG_PRINTLN(*status);
  //DEBUG_PRINT(F("Len: ")); DEBUG_PRINTLN(*datalen);
  // HTTP response data
  if(!HTTP_readall(datalen)) return false;
  return true;
}

/*
boolean Adafruit_FONA_3G::HTTP_GET_start(char *ipaddr, char *path, uint16_t port, uint16_t *status, uint16_t *datalen)
{
  char send[100] = "AT+CHTTPACT=\"";
  char *sendp = send + strlen(send);
  memset(sendp, 0, 100 - strlen(send));
  strcpy(sendp, ipaddr);
  sendp+=strlen(ipaddr);
  sendp[0] = '\"';
  sendp++;
  sendp[0] = ',';
  itoa(sendp, port);
  getReply(send, 500);
  return;
  if(!HTTP_setup(url)) return false;
  // HTTP GET
  if(!HTTP_action(FONA_HTTP_GET, status, datalen)) return false;
  // DEBUG_PRINT("Status: "); DEBUG_PRINTLN(*status);
  // DEBUG_PRINT("Len: "); DEBUG_PRINTLN(*datalen);
  // HTTP response data
  if(!HTTP_readall(datalen)) return false;
  return true;
}
*/

void Adafruit_FONA::HTTP_GET_end(void)
{
  HTTP_term();
}

bool Adafruit_FONA::HTTP_POST_start(char *url, FONAFlashStringPtr contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen)
{
  if(!HTTP_setup(url)) return false;
  if(!HTTP_para("CONTENT", contenttype)) return false;
  // HTTP POST data
  if(!HTTP_data(postdatalen, 10000)) return false;
  mySerial->write(postdata, postdatalen);
  if(!expectReply(ok_reply)) return false;
  // HTTP POST
  if(!HTTP_action(FONA_HTTP_POST, status, datalen)) return false;
  // DEBUG_PRINT(F("Status: ")); DEBUG_PRINTLN(*status);
  // DEBUG_PRINT(F("Len: ")); DEBUG_PRINTLN(*datalen);
  // HTTP response data
  if(!HTTP_readall(datalen)) return false;
  return true;
}

void Adafruit_FONA::HTTP_POST_end(void)
{
  HTTP_term();
}

void Adafruit_FONA::setUserAgent(FONAFlashStringPtr useragent)
{
  this->useragent = useragent;
}

void Adafruit_FONA::setHTTPSRedirect(bool onoff)
{
  httpsredirect = onoff;
}

/********* HTTP HELPERS ****************************************/

bool Adafruit_FONA::HTTP_setup(char *url)
{
  // Handle any pending
  HTTP_term();
  // Initialize and set parameters
  if (! HTTP_init())
    return false;
  if(!HTTP_para("CID", 1)) return false;
  if(!HTTP_para("UA", useragent)) return false;
  if(!HTTP_para("URL", url)) return false;
  // HTTPS redirect
  if(httpsredirect) {
    if(!HTTP_para("REDIR",1)) return false;
    if(!HTTP_ssl(true)) return false;
  }
  return true;
}

/********* HELPERS *********************************************/

bool Adafruit_FONA::expectReply(FONAFlashStringPtr reply, uint16_t timeout)
{
  readline(timeout);
  DEBUG_PRINT("\t<--- ");
  DEBUG_PRINTLN(replybuffer);
  return (prog_char_strcmp(replybuffer,reply) == 0);
}

/********* LOW LEVEL *******************************************/

void Adafruit_FONA::flushInput()
{
// Read all available serial input to flush pending data.
uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(mySerial->available()) {
            mySerial->read();
            timeoutloop = 0;  // If char was received reset the timer
        }
        usleep(1100);
    }
}

uint16_t Adafruit_FONA::readRaw(uint16_t b)
{
uint16_t idx = 0;

  while (b && (idx < sizeof(replybuffer)-1)) {
    if(mySerial->available()) {
      replybuffer[idx] = mySerial->read();
      idx++;
      b--;

    }
  }
  replybuffer[idx] = 0;
  return idx;
}

uint8_t Adafruit_FONA::readline(uint16_t timeout, bool multiline)
{
static uint8_t replyidx;

  replyidx = 0;
  while(timeout--) {
    if(replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }
    // read so much char as possible, if empty break
    do {
      char c =  mySerial->read();
      if(!mySerial->available()) break;
      if(c == '\r') continue;
      if(c == 0xA) {
        // the first 0x0A is ignored
        if(replyidx == 0) continue;
        if(!multiline) {
          // the second 0x0A is the end of the line
          timeout = 0;
          break;
        }
      }
      replybuffer[replyidx] = c;
      //DEBUG_PRINT(c, HEX); DEBUG_PRINT("#"); DEBUG_PRINTLN(c);
      replyidx++;
    } while(mySerial->available());
    if(timeout == 0) {
      //DEBUG_PRINTLN(F("TIMEOUT"));
      break;
    }
    usleep(1000);
  }
  replybuffer[replyidx] = 0;  // null term
  return replyidx;
}

uint8_t Adafruit_FONA::getReply(char *send, uint16_t timeout)
{
static uint8_t readcnt;

  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINTLN(send);
  mySerial->println(send);
  readcnt = readline(timeout);
  DEBUG_PRINT ("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return readcnt;
}

uint8_t Adafruit_FONA::getReply(FONAFlashStringPtr send, uint16_t timeout)
{
  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINTLN(send);
  mySerial->println(send);
  uint8_t l = readline(timeout);
  DEBUG_PRINT("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t Adafruit_FONA::getReply(FONAFlashStringPtr prefix, char *suffix, uint16_t timeout)
{
  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINTLN(suffix);
  mySerial->print(prefix);
  mySerial->println(suffix);
  uint8_t l = readline(timeout);
  DEBUG_PRINT("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
uint8_t Adafruit_FONA::getReply(FONAFlashStringPtr prefix, int32_t suffix, uint16_t timeout)
{
  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINT(prefix); // DEBUG_PRINTLN(suffix, DEC);
  mySerial->print(prefix);
  mySerial->println(suffix, 10);
  uint8_t l = readline(timeout);
  DEBUG_PRINT ("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return l;
}

// Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
uint8_t Adafruit_FONA::getReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout)
{
  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINT(prefix);
  //DEBUG_PRINT(suffix1, DEC); DEBUG_PRINT(','); DEBUG_PRINTLN(suffix2, DEC);
  mySerial->print(prefix);
  mySerial->print(suffix1);
  mySerial->print(',');
  mySerial->println(suffix2, 10);
  uint8_t l = readline(timeout);
  DEBUG_PRINT("\t<--- "); DEBUG_PRINTLN(replybuffer);

  return l;
}

// Send prefix, ", suffix, ", and newline. Return response (and also set replybuffer with response).
uint8_t Adafruit_FONA::getReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, uint16_t timeout)
{
  flushInput();
  DEBUG_PRINT("\t---> "); DEBUG_PRINT(prefix);
  //DEBUG_PRINT('"'); DEBUG_PRINT(suffix); DEBUG_PRINTLN('"');
  mySerial->print(prefix);
  mySerial->print('"');
  mySerial->print(suffix);
  mySerial->println('"');
  uint8_t l = readline(timeout);
  DEBUG_PRINT("\t<--- "); DEBUG_PRINTLN(replybuffer);
  return l;
}

bool Adafruit_FONA::sendCheckReply(char *send, char *reply, uint16_t timeout)
{
  if(!getReply(send, timeout)) return false;
/*
  for (uint8_t i=0; i<strlen(replybuffer); i++) {
  DEBUG_PRINT(replybuffer[i], HEX); DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN();
  for (uint8_t i=0; i<strlen(reply); i++) {
    DEBUG_PRINT(reply[i], HEX); DEBUG_PRINT(" ");
  }
  DEBUG_PRINTLN();
  */
  return (strcmp(replybuffer, reply) == 0);
}

bool Adafruit_FONA::sendCheckReply(FONAFlashStringPtr send, FONAFlashStringPtr reply, uint16_t timeout)
{
  if(!getReply(send, timeout)) return false;
  if(prog_char_strcmp(replybuffer, reply) == 0) {
      return true;
  }  else {
      return false;
  }
  return(prog_char_strcmp(replybuffer, reply) == 0);
//  return (prog_char_strcmp(replybuffer,reply) == 0);
}

bool Adafruit_FONA::sendCheckReply(char* send, FONAFlashStringPtr reply, uint16_t timeout)
{
  if(!getReply(send, timeout)) return false;
  if(prog_char_strcmp(replybuffer, reply) == 0) {
      return true;
  }  else {
      return false;
  }
//  return(prog_char_strcmp(replybuffer, reply) == 0);
}


// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
bool Adafruit_FONA::sendCheckReply(FONAFlashStringPtr prefix, char *suffix, FONAFlashStringPtr reply, uint16_t timeout)
{
  getReply(prefix, suffix, timeout);
  return(prog_char_strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
bool Adafruit_FONA::sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix, FONAFlashStringPtr reply, uint16_t timeout)
{
  getReply(prefix, suffix, timeout);
  return(prog_char_strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
bool Adafruit_FONA::sendCheckReply(FONAFlashStringPtr prefix, int32_t suffix1, int32_t suffix2, FONAFlashStringPtr reply, uint16_t timeout)
{
  getReply(prefix, suffix1, suffix2, timeout);
  return (prog_char_strcmp(replybuffer, reply) == 0);
}

// Send prefix, ", suffix, ", and newline.  Verify FONA response matches reply parameter.
bool Adafruit_FONA::sendCheckReplyQuoted(FONAFlashStringPtr prefix, FONAFlashStringPtr suffix, FONAFlashStringPtr reply, uint16_t timeout)
{
  getReplyQuoted(prefix, suffix, timeout);
  return(prog_char_strcmp(replybuffer, reply) == 0);
}


bool Adafruit_FONA::parseReply(FONAFlashStringPtr toreply, uint16_t *v, char divider, uint8_t index)
{ // get the pointer to the voltage
  char *p = prog_char_strstr(replybuffer, toreply);
  if (p == 0) return false;
  p += prog_char_strlen(toreply);
  //DEBUG_PRINTLN(p);
  for (uint8_t i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
    //DEBUG_PRINTLN(p);
  }
  *v = atoi(p);
  return true;
}

bool Adafruit_FONA::parseReply(FONAFlashStringPtr toreply, char *v, char divider, uint8_t index)
{
uint8_t i=0;
  char *p = prog_char_strstr(replybuffer, toreply);
  if (p == 0) return false;
  p += prog_char_strlen(toreply);
  for (i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if(!p) return false;
    p++;
  }
  for(i=0; i<strlen(p);i++) {
    if(p[i] == divider) break;
    v[i] = p[i];
  }
  v[i] = '\0';
  return true;
}

// Parse a quoted string in the response fields and copy its value (without quotes)
// to the specified character array (v).  Only up to maxlen characters are copied
// into the result buffer, so make sure to pass a large enough buffer to handle the
// response.
bool Adafruit_FONA::parseReplyQuoted(FONAFlashStringPtr toreply, char *v, int maxlen, char divider, uint8_t index)
{
uint8_t i=0, j;
  // Verify response starts with toreply.
  char *p = prog_char_strstr(replybuffer, toreply);
  if(p == 0) return false;
  p += prog_char_strlen(toreply);
  // Find location of desired response field.
  for(i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
  }
  // Copy characters from response field into result string.
  for(i=0, j=0; j<maxlen && i<strlen(p); ++i) {
    // Stop if a divier is found.
    if(p[i] == divider)
      break;
    // Skip any quotation marks.
    else if(p[i] == '"')
      continue;
    v[j++] = p[i];
  }
  // Add a null terminator if result string buffer was not filled.
  if(j < maxlen) v[j] = '\0';
  return true;
}

bool Adafruit_FONA::sendParseReply(FONAFlashStringPtr tosend, FONAFlashStringPtr toreply, uint16_t *v, char divider, uint8_t index)
{
  getReply(tosend);
  if(!parseReply(toreply, v, divider, index)) return false;
  readline(); // eat 'OK'
  return true;
}

// needed for CBC and others
bool Adafruit_FONA_3G::sendParseReply(FONAFlashStringPtr tosend, FONAFlashStringPtr toreply, float *f, char divider, uint8_t index)
{
  getReply(tosend);
  if(!parseReply(toreply, f, divider, index)) return false;
  readline(); // eat 'OK'
  return true;
}

bool Adafruit_FONA_3G::parseReply(FONAFlashStringPtr toreply, float *f, char divider, uint8_t index)
{
  // get the pointer to the voltage
  char *p = prog_char_strstr(replybuffer, toreply);
  if(p == 0) return false;
  p += prog_char_strlen(toreply);
  //DEBUG_PRINTLN(p);
  for(uint8_t i=0; i<index;i++) {
    // increment dividers
    p = strchr(p, divider);
    if (!p) return false;
    p++;
    //DEBUG_PRINTLN(p);
  }
  *f = atof(p);
  return true;
}
