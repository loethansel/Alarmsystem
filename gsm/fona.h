/*
 * fona.h
 *
 *  Created on: Nov 8, 2018
 *      Author: Pandel
 */

#ifndef FONA_H_
#define FONA_H_
// INCLUDES
#include <ctime>
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
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackUART/BlackUART.h"
#include "../files/ctrlfile.h"
#include "../alarmsys.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;
// CLASS
class FONA : public BlackUART
{
public:
    bool fonalive;
    bool fonarssi;
    bool poweredon;
    FONA(void);
    ~FONA(void);
    int SendSms(const string& telnumber, const string& text);
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
