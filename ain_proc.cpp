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
static clock_t output_evt,tmeas_now;
bool ledon = false;

   output_evt = 0;

   while(1) {
       // second clock for time dependent functions
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt +1) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          if(ledon) {
              // LED->setValue(low);
              ledon = false;
          }
          else {
              // LED->setValue(high);
              ledon = true;
          }
       }
       // TODO: fillwith live
   }
   pthread_exit(NULL);
}





