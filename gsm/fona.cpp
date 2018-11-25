/*
 * fona.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: Pandel
 */
// INCLUDES
#include "fona.h"
// Declaration

using namespace std;
using namespace BlackLib;

BlackGPIO  *pwr_out;
BlackGPIO  *pwr_in;
BlackGPIO  *rst_out;


FONA::FONA() : BlackUART(UART2,Baud115200,ParityNo,StopOne,Char8)
{
    bool isOpened = open( BlackLib::ReadWrite | BlackLib::NonBlock );
    // check the open succeed
    if(!isOpened ) cout << "FONA on UART2 can\'t open!" << endl;
    else {
        InitIo();
        cout << "Fona on UART2 is open...." << endl;
    }
    this->fonalive = false;
    this->fonarssi = false;
    this->poweredon  = false;
}

int FONA::InitIo(void)
{
    pwr_out = new BlackGPIO(GPIO_60 ,output,FastMode); // P9.12
    pwr_in  = new BlackGPIO(GPIO_49 ,input);           // P9.23
    rst_out = new BlackGPIO(GPIO_48 ,output,FastMode); // P9.15
    pwr_out->setValue(high);
    rst_out->setValue(high);
    return true;
}

int FONA::Power_On(void)
{
unsigned int i;

    // force hardwarereset
    Reset_Module();
    credit_aschar[0] = '-';
    credit_aschar[1] = '-';
    credit_aschar[2] = '.';
    credit_aschar[3] = '-';
    credit_aschar[4] = '-';
    credit_aschar[5] = '\0';
    credit_numeric   = 0.0;
    // wait a moment
    sleep(1);
    // try for 3 times to power up
    for(i=0;i<3;i++) {
        // button pressed for 1 second
        pwr_out->setValue(low);
        sleep(1);
        pwr_out->setValue(high);
        // check pwrpin and AT\r => OK
        if(IsRunning()) {
            if(LiveCheck()) {
            	this->poweredon = true;
            	return true;
            }
        }
    }
	this->poweredon = false;
    return false;
}

int FONA::Power_Off(void)
{
unsigned int i;

    // is the module down yet?
    if(pwr_in->getNumericValue() == low) {
        Reset_Module();
        return true;
    }
    // try to power down the module for 3 times
    for(i=0;i<3;i++) {
        // button pressed for 1 second
        pwr_out->setValue(low);
        sleep(1);
        pwr_out->setValue(high);
        // check the powerpin is low
        if(IsStopped()) return true;
    }
    return false;
}

int FONA::IsStopped(void)
{
int i;

   // wait x seconds till module powered down
   for(i=0;i<15;i++) {
     // check the powerpin in high
     if(pwr_in->getNumericValue() == low) return true;
     sleep(1);
   }
   return false;
}



int FONA::IsRunning(void)
{
int i;

   // wait x seconds till module powered down
   for(i=0;i<15;i++) {
     // check the powerpin in high
     if(pwr_in->getNumericValue() == high) return true;
     sleep(1);
   }
   return false;
}


int FONA::Reset_Module(void)
{
    rst_out->setValue(low);
    usleep(500000);
    rst_out->setValue(high);
    return true;
}


int FONA::LiveCheck()
{
string rbuff;
string tbuff;
size_t length;
int i;
int retval;

   for(i=0;i<10;i++) {
       tbuff.clear(); rbuff.clear();
       tbuff = "AT\r";
       length = tbuff.copy(writeArr,tbuff.size(),0);
       writeArr[length] = '\0';
       flush(bothDirection);
       if(!write(writeArr,length)) return false;
       usleep(40000);
       read(readArr,100);
       rbuff = readArr;
       retval = rbuff.find("OK");
       if(retval != -1) {
           this->fonalive = true;
    	   return true;
       }
       sleep(1);
   }
   this->fonalive = false;
   return false;
}

int FONA::RxLevelCheck()
{
string rbuff;
string tbuff;
size_t length;
stringstream ss;
int pos;
int retval;
int i;

    tbuff.clear();
    tbuff = "AT+CSQ\r";
    length = tbuff.copy(writeArr,tbuff.size(),0);
    writeArr[length] = '\0';
    flush(bothDirection);
    if(!write(writeArr,length)) return false;
    for(i=0;i<5;i++) {
        sleep(1);
        rbuff.clear();
        read(readArr,100);
        rbuff = readArr;
        cout << rbuff << endl;
        retval = rbuff.find("OK");
        if(retval != -1) {
            pos = rbuff.find_first_of(',',0);
            if(pos != -1) {
               length = rbuff.copy(rxpegel_aschar,2,pos-2);
               rxpegel_aschar[length] = '\0';
               rbuff = rxpegel_aschar;
               ss << rbuff;
               rxpegel_numeric = stoi(ss.str());
               cout << "Empfangspegel: "  << ss.str() << endl;
               if(rxpegel_numeric == 0) {
                   this->fonarssi = false;
            	   return false;
               }
               else {
                   this->fonarssi = true;
            	   return true;
               }
            }
        }
    }
   return false;
}

int FONA::CreditCheck()
{
string rbuff;
string tbuff;
size_t length;
stringstream ss;
int pos;
int i;
int retval;

    tbuff.clear();
    tbuff = "AT+CUSD=1,\"*100#\",15\r";
    length = tbuff.copy(writeArr,tbuff.size(),0);
    writeArr[length] = '\0';
    flush(bothDirection);
    if(!write(writeArr,length)) return false;
    for(i=0;i<15;i++) {
        sleep(1);
        read(readArr,100);
        rbuff.clear();
        rbuff = readArr;
        cout << rbuff << endl;
        retval = rbuff.find("EUR");
        if(retval != -1) {
            pos = rbuff.find_first_of('.',0);
            if(pos != -1) {
               length = rbuff.copy(credit_aschar,5,pos-2);
               credit_aschar[length] = '\0';
               rbuff = credit_aschar;
               ss << rbuff;
               credit_numeric = stof(ss.str());
               cout << "Guthaben: "  << ss.str() << endl;
            return true;
            }
        }
    }
   return false;
}

int FONA::SendSms(void)
{
string rbuff;
string tbuff;
string tbuff1;
size_t length;
stringstream ss;
stringstream rd;
stringstream wr;
int i;
int retval;
//char  number[50];

       // check the module with some AT\r commands
       if(!LiveCheck()) return false;

       // sms service ok?
       flush(bothDirection);
       tbuff.clear(); rbuff.clear();
       tbuff = "AT+CMGS=?\r";
       length = tbuff.copy(writeArr,tbuff.size(),0);
       writeArr[length] = '\0';
       if(!write(writeArr,length)) return false;
       usleep(40000);
       if(!read(readArr,100)) return false;
       rbuff = readArr;
       cout << rbuff << endl;
       retval = rbuff.find("OK");
       if(retval == -1) return false;

       // Auf Textmode umstellen
       flush(bothDirection);
       tbuff.clear(); rbuff.clear();
       tbuff = "AT+CMGF=1\r";
       length = tbuff.copy(writeArr,tbuff.size(),0);
       writeArr[length] = '\0';
       if(!write(writeArr,length)) return false;
       usleep(40000);
       if(!read(readArr,100)) return false;
       rbuff = readArr;
       cout << rbuff << endl;
       retval = rbuff.find("OK");
       if(retval == -1) return false;

       // send SMS header
       flush(bothDirection);
       tbuff.clear(); rbuff.clear();

       //!! weitermachen
       //number = CTRLFILE->alarmnum.numname[0].number;

       tbuff = "AT+CMGS=\"+491759944339\"\r";
       length = tbuff.copy(writeArr,tbuff.size(),0);
       writeArr[length] = '\0';
       if(!write(writeArr,length)) return false;
       usleep(100000);
       if(!read(readArr,100)) return false;
       rbuff = readArr;
       cout << rbuff << endl;
       retval = rbuff.find(">");
       if(retval == -1) return false;
       sleep(1);
       // send SMS body
       flush(bothDirection);
       tbuff.clear();
       tbuff = credit_aschar;
       ss << "Alarm Halle Grau!" << " Q:" << dec << rxpegel_numeric << " G:" << tbuff << ".-EUR";
       cout << ss.str() << endl;
       tbuff.clear();
       tbuff = ss.str();
       cout << tbuff.c_str() << endl;
       length = tbuff.copy(writeArr,tbuff.size(),0);
       writeArr[length] = '\032';
       if(!write(writeArr,length+1)) return false;
       for(i=0;i<15;i++) {
          sleep(1);
          rbuff.clear();
          read(readArr,100);
          rbuff = readArr;
          cout << rbuff << endl;
          retval = rbuff.find("+CMGS:");
          if(retval != -1) return true;
       }
       return false;
       // todo:
       // SMS aus dem Speicher wieder entfernen
       // AT+CPMS=? => +CPMS: ("ME","MT","SM"),("ME","MT","SM"),("ME","MT","SM")
       // AT+CPMS? => +CPMS: "ME",anz,55,"ME",anz,55,"ME",anz,55
       // AT+CPMS="ME","ME","ME => +CPMS: anz,255,anz,255,anz,255
       //
}

FONA::~FONA()
{
    close();
    delete pwr_out;
    delete pwr_in;
    delete rst_out;
}
