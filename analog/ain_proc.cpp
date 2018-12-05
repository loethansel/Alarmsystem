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


//---------------------------------------------------------------------------
// AINTASK
//---------------------------------------------------------------------------
void *AinTask(void *value)
{
static  clock_t output_evt,tmeas_now;
static  int     seccnt;
float   valueFloat[4];
stringstream ss;
string       s;
int     i;
bool    lineactive[MAXLINE];
bool    linethreshold[MAXLINE];
bool    prealert[4];
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
              if(CTRLFILE->ini.ALARM_LINE.lineactv[i] == "true") lineactive[i] = true;
              else lineactive[i] = false;
              linethreshold[i]   = false;
          }
          // log the analog values every hour
          if(seccnt++ >= INFOTIME) {
             for(i=0;i<MAXLINE;i++) {
                 if(lineactive[i]) {
                     ss.str("");
                     ss.clear();
                     ss << "value LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
                     s = ss.str();
                     Logger::Write(Logger::DEBUG,s);
                 }
             }
             seccnt = 0;
          }
          // first read all the lines to maxline
          for(i=0;i<MAXLINE;i++) {
              // Values to standard out
              // cout << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
              // check if value is off-limit
              prealert[i] = false;
              umin = stof(CTRLFILE->ini.ALARM_LINE.lineumin[i]);
              umax = stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]);
              if((valueFloat[i] >= umax) || (valueFloat[i] <= umin)) {
                  // set alarm if line is permitted
                  if(lineactive[i]) { prealert[i] = true; linethreshold[i] = true; }
              }
          }
          // wait 100ms to eleminate spikes and fail alerts
          usleep(100000);
          // reread the values
          valueFloat[0] = analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[1] = analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[2] = analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[3] = analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          // second read all the lines to maxline
          for(i=0;i<MAXLINE;i++) {
              // check if value is off-limit
              umin = stof(CTRLFILE->ini.ALARM_LINE.lineumin[i]);
              umax = stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]);
              if((valueFloat[i] >= umax) || (valueFloat[i] <= umin)) {
                  if(armed && (prealert[i] == true)) {
                      alarmactive = true;
                      if(CTRLFILE->ini.ALARM_LINE.linelog == "true") {
                          ss.str("");
                          ss.clear();
                          ss << "alarm LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
                          s = ss.str();
                          Logger::Write(Logger::INFO,s);
                      }
                  }
              }
          }
          // Check if all active lines are idle
          if((linethreshold[0] == 0) && (linethreshold[1] == 0) && (linethreshold[2] == 0) && (linethreshold[3] == 0)) contactopen = false;
          else contactopen = true;
       }
   }
   pthread_exit(NULL);
}





