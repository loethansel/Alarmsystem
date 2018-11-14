/*
 * fona.h
 *
 *  Created on: Nov 8, 2018
 *      Author: linux
 */

#ifndef FONA_H_
#define FONA_H_
#include<string>
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"

using namespace std;
using namespace BlackLib;

class FONA : public BlackUART
{
public:
    bool fonalive;
    bool fonarssi;
    bool poweredon;
    FONA(void);
    ~FONA(void);
    int SendSms(void);
    int LiveCheck();
    int RxLevelCheck();
    int CreditCheck();
    int Power_On(void);
    int Power_Off(void);
    int Reset_Module(void);
    int IsRunning(void);
    int IsStopped(void);
private:
    int   rxpegel_numeric;
    char  rxpegel_aschar[10];
    char  credit_aschar[10];
    float credit_numeric;
    char  writeArr[255];
    char  readArr[255];
    int   InitIo(void);
};

#endif /* FONA_H_ */
