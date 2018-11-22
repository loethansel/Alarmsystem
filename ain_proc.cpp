/*
 * ain_proc.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: Pandel
 */
// INCLUDES
#include "ain_proc.h"
#include "alarmsys.h"
// GLOBALVARS
pthread_t aintask;

// AINTASK
void *AinTask(void *value)
{
static  clock_t output_evt,tmeas_now;
bool    ledon = false;
string  valueStr;
int     valueInt;
float   valueFloat[4];
int     i;
BlackLib::BlackADC analog0(BlackLib::AIN0 );
BlackLib::BlackADC analog1(BlackLib::AIN1 );
BlackLib::BlackADC analog2(BlackLib::AIN2 );
BlackLib::BlackADC analog3(BlackLib::AIN3 );

//   fptr_t  analogfkt[4];
//   analogfkt[0] = analog0.getConvertedValue;

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
          // Read all the lines
          for(i=0;i<MAXLINE;i++){
              cout << "LINIE" << tostr(i+1) << ": " << fixed << setprecision(3) << valueFloat[i] << endl;
              if((valueFloat[i] >= UMAX) || (valueFloat[i] <= UMIN)) {
                  alarmactive = true;
                  // TODO: Werte ins Logfile
              }
          }
       }
   }
   pthread_exit(NULL);
}





