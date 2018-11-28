/*
 * ain_proc.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include "ain_proc.h"
#include "../logger/logger.h"
//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;
// GLOBALVARS
pthread_t aintask;


// AINTASK
void *AinTask(void *value)
{
static  clock_t output_evt,tmeas_now;
float   valueFloat[4];
stringstream ss;
string       s;
int     i;
int     seccnt;
bool    active[MAXLINE];
float   umin, umax;
// Analg Input Declaration
BlackADC analog0(BlackLib::AIN0 );
BlackADC analog1(BlackLib::AIN1 );
BlackADC analog2(BlackLib::AIN2 );
BlackADC analog3(BlackLib::AIN3 );

//   fptr_t  analogfkt[4];
//   analogfkt[0] = analog0.getConvertedValue;
/*
typedef struct {
    bool   account;
    float  umin;
    float  umax;
} s_linestxt;
typedef struct {
    s_linestxt l[MAXLINES];
    int    cnt;
} s_lines;
*/
   //Logger::Write(Logger::INFO,teststr);
   output_evt = 0;
   seccnt     = 0;
   while(1) {
       // INTERES SIGNAL PRGRAM END!!
       if(program_end) break;
       // second clock for time dependent functions
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt +1) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          valueFloat[0] = analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[1] = analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[2] = analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[3] = analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          // Check if line is active
          for(i=0;i<MAXLINE;i++) {
              if(CTRLFILE->ini.ALARM_LINE.lineactv[i] == "true") active[i] = true;
              else active[i] = false;
          }
          // log the analog values every hour
          if(seccnt++>= 3600) {
             for(i=0;i<MAXLINE;i++) {
                 if(active[i]) {
                    s = ss.str();
                    Logger::Write(Logger::DEBUG,s);
                 }
             }
             seccnt = 0;
          }
          // Read all the lines to maxline
          for(i=0;i<MAXLINE;i++) {
              // Values to standard out
              cout << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
              // check if value is off-limit
              umin = stof(CTRLFILE->ini.ALARM_LINE.lineumin[i]);
              umax = stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]);
              if((valueFloat[i] >= umax) || (valueFloat[i] <= umin)) {
                  // set alarm if line is permitted
                  if(active[i]) {
                      alarmactive = true;
                      // LOGGER!!!!!
                      /*
                      ss.clear();
                      ss << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[0] << endl;
                      s.clear();
                      s = ss.str();
                      Logger::Write(Logger::INFO,s);
                      */
                  }
              }
          }
       }
   }
   pthread_exit(NULL);
}





