/*
 * ain_proc.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include "ain_proc.h"
// GLOBALVARS
pthread_t aintask;


// AINTASK
void *AinTask(void *value)
{
static  clock_t output_evt,tmeas_now;
//bool    ledon = false;
string  valueStr;
//int     valueInt;
float   valueFloat[4];
int     i;
int     maxline;
BlackLib::BlackADC analog0(BlackLib::AIN0 );
BlackLib::BlackADC analog1(BlackLib::AIN1 );
BlackLib::BlackADC analog2(BlackLib::AIN2 );
BlackLib::BlackADC analog3(BlackLib::AIN3 );

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

   output_evt = 0;

   while(1) {
       // second clock for time dependent functions
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt +1) ) {
          output_evt = clock() / CLOCKS_PER_SEC;

          valueFloat[0] = analog0.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[1] = analog1.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[2] = analog2.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          valueFloat[3] = analog3.getConvertedValue(BlackLib::dap2) * VOLTPRODIGIT;
          // copy the file readed maxlines cnt
          maxline = stoi(CTRLFILE->ini.ALARM_LINE.cnt);
          // chek the value and correct
          if(maxline > MAXLINE) maxline = MAXLINE;
          // Read all the lines to maxline
          for(i=0;i<maxline;i++) {
              // Values to standard out
              cout << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
              // copy value for application
              //CTRLFILE->ini.lines.lineactive[i] = valueFloat[i];
              // check if value is off-limit
              if((valueFloat[i] >= stof(CTRLFILE->ini.ALARM_LINE.lineumax[i])) || (valueFloat[i] <= stof(CTRLFILE->ini.ALARM_LINE.lineumax[i]))) {
                  // set alarm if line is permitted
                  if(CTRLFILE->ini.ALARM_LINE.lineactive[i].at(0) == '1') alarmactive = true;
                  //if(CTRLFILE->ini.lines.lineactive[i]) alarmactive = true;
                  // TODO: Werte ins Logfile
              }
          }
       }
   }
   pthread_exit(NULL);
}





