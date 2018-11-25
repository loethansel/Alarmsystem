 /* ALARM TO EMAIL */
//---------------------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------------------
#include "alarmsys.h"
#include "logger/logger.h"

//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;
//---------------------------------------------------------------------------
// GLOBAL Defines
//---------------------------------------------------------------------------
//#define DEBUG 1
//---------------------------------------------------------------------------
// GLOBAL Declarations
//---------------------------------------------------------------------------
ofstream ofs;
bool program_end;
bool sendsms;
bool armed;
bool alarmactive;

//---------------------------------------------------------------------------
// Threads Declarations
//---------------------------------------------------------------------------
pthread_t maintask;
//---------------------------------------------------------------------------
// FOREWARD Declarations
//---------------------------------------------------------------------------
void *MainTask(void *value);
// OUTPUTS
BlackGPIO    *OUT_BUZZER;
BlackGPIO    *OUT_LED;
// INPUTS
BlackGPIO    *IN_SCHARF;
BlackGPIO    *IN_UNSCHARF;
// FILES
ctrlfile     *CTRLFILE;
INIParser    *INIFILE;
serialrelais *RELAIS;

//---------------------------------------------------------------------------
// function: DebugOut
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void DebugOut(const char *string,unsigned int value,unsigned char valout)
{
#if(DEBUG == 1)
   if(valout) {
       std::cout << string << std::hex << value << std::endl;
   } else {
       std::cout << string << std::endl;
   }
#endif
}

//---------------------------------------------------------------------------
// function: WriteLog
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EXIT Function
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void termination_handler(int sig)
{
   Logger::Write(Logger::INFO, "Main: Caught Signal: ");// << sig);
   // Logfile last Output
   Logger::Write(Logger::INFO, "Main: close logfile..........");// << sig);
   // close Logfile
   ofs.close();
   exit(0);
}

//---------------------------------------------------------------------------
// create_logfile Function
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
void create_logfile(void)
{
char     timestamp[22];
time_t   now;
stringstream s;
#define LOGFILENAME "alrlog_"

   now = time(0);
   strftime(timestamp, 22, "%Y%m%d%H%M%S", localtime(&now));
   s << LOGFILENAME << timestamp << ".txt";
   ofs.open (string(s.str()));
   // Logfile Output umleiten
   clog.rdbuf(ofs.rdbuf());
   Logger::Write(Logger::INFO, "AL_main: Logfile Created!");
}

void init_system(void)
{
    // INPUTS
    Logger::Write(Logger::INFO, "Main: Input GPIO's exported to /sys/class/gpio");
    IN_SCHARF   = new BlackGPIO(BlackLib::GPIO_50 ,BlackLib::input); // P9.14
    IN_UNSCHARF = new BlackGPIO(BlackLib::GPIO_51 ,BlackLib::input); // P9.16
    // OUTPUTS
    Logger::Write(Logger::INFO, "Main: Output GPIO's exported to /sys/class/gpio");
    OUT_BUZZER = new BlackGPIO(BlackLib::GPIO_117,BlackLib::output,BlackLib::FastMode);  // P9.25
    OUT_BUZZER->setValue(low);
    OUT_LED    = new BlackGPIO(BlackLib::GPIO_115,BlackLib::output,BlackLib::FastMode); // P9.27
    OUT_LED->setValue(low);
    // FILES
    CTRLFILE = new ctrlfile;
    // RELAIS
    RELAIS   = new serialrelais;
    INIFILE  = new INIParser;



    // out_2 = new BlackLib::BlackGPIO(BlackLib::GPIO_50 ,BlackLib::output,BlackLib::FastMode); // P9.14
    // out_4 = new BlackLib::BlackGPIO(BlackLib::GPIO_51 ,BlackLib::output,BlackLib::FastMode); // P9.16
    // out_5 = new BlackLib::BlackGPIO(BlackLib::GPIO_3  ,BlackLib::output,BlackLib::FastMode); // P9.21
    // out_6 = new BlackLib::BlackGPIO(BlackLib::GPIO_2  ,BlackLib::output,BlackLib::FastMode); // P9.22
    // in_1 = new BlackLib::BlackGPIO(BlackLib::GPIO_115,BlackLib::input); // P9.27
    // in_2 = new BlackLib::BlackGPIO(BlackLib::GPIO_66 ,BlackLib::input); // P8.07
    // in_3 = new BlackLib::BlackGPIO(BlackLib::GPIO_67 ,BlackLib::input); // P8.08
    // in_4 = new BlackLib::BlackGPIO(BlackLib::GPIO_69 ,BlackLib::input); // P8.09
    // in_5 = new BlackLib::BlackGPIO(BlackLib::GPIO_68 ,BlackLib::input); // P8.10
    // in_6 = new BlackLib::BlackGPIO(BlackLib::GPIO_45 ,BlackLib::input); // P8.11
    // in_7 = new BlackLib::BlackGPIO(BlackLib::GPIO_44 ,BlackLib::input); // P8.12
    // in_8 = new BlackLib::BlackGPIO(BlackLib::GPIO_23 ,BlackLib::input); // P8.13
    // in_9 = new BlackLib::BlackGPIO(BlackLib::GPIO_26 ,BlackLib::input); // P8.14
}

bool init_tasks(void)
{   // Create Gsm-Task
    if(pthread_create(&maintask, NULL,&MainTask,NULL)) return false;
    if(pthread_create(&aintask, NULL,&AinTask,NULL))   return false;
    if(pthread_create(&gsmtask, NULL,&GsmTask,NULL))   return false;
    if(pthread_join(aintask,NULL))  return false;
    if(pthread_join(gsmtask,NULL))  return false;
    if(pthread_join(maintask,NULL)) return false;
    delete IN_SCHARF;   // P9.14
    delete IN_UNSCHARF; // P9.16
    delete OUT_BUZZER;  // P9.25
    delete OUT_LED;     // P9.27
    delete CTRLFILE;    // File-IO Modul
    delete RELAIS;      // Relais
    delete INIFILE;     // IninFile read/write
    return true;
}

bool switch_relais(bool onoff)
{
    if(onoff) {
        RELAIS->turn_on_channel(1);
        RELAIS->turn_on_channel(2);
        RELAIS->turn_on_channel(3);
        RELAIS->turn_on_channel(4);
    }
    else {
        RELAIS->turn_off_channel(1);
        RELAIS->turn_off_channel(2);
        RELAIS->turn_off_channel(3);
        RELAIS->turn_off_channel(4);
    }
    return true;
}

void set_armed(void)
{
int i;
    // only set to armed if not alarm line is active or armed yet
    if(armed || alarmactive) return;
    Logger::Write(Logger::INFO, "AL_main: Alarmanlage scharf geschaltet!");
    armed  = true;
    CTRLFILE->WriteSystemArmed(true);
    OUT_LED->setValue(high);
    for(i=0;i<3;i++) {
       OUT_BUZZER->setValue(high);
       usleep(500000);
       OUT_BUZZER->setValue(low);
       usleep(500000);
    }
    OUT_BUZZER->setValue(high);
    sleep(1);
    OUT_BUZZER->setValue(low);
}


void set_unarmed(void)
{
    if(!armed) return;
    Logger::Write(Logger::INFO, "AL_main: Alarmanlage unscharf geschaltet!");
    armed       = false;
    alarmactive = false;
    switch_relais(OFF);

    CTRLFILE->WriteSystemArmed(false);
    OUT_LED->setValue(low);
    OUT_BUZZER->setValue(high);
    sleep(2);
    OUT_BUZZER->setValue(low);
}

void *MainTask(void *value)
{
//FILE   *fp = NULL;
//bool   outok;
bool   barrier = false;
static clock_t output_evt,tmeas_now;
static int sectimer   = 0;
static int mintimer   = 0;
static int hourtimer  = 0;
string autoalarmstr;
string alarmtime;
bool   retval;


   // TEST
   uint8_t      version;
   version      = RELAIS->getFirmwareVersion();
   switch_relais(OFF);
   if(version == 0) cout << "Relaisausgänge arbeiten nicht!" << endl;
   // END RELAIS

   output_evt = 0;
   while(1) {
       //-----------------------------------------------------------
       // Sendesperre z.B. nicht mehr als einen Alarm/min. melden
       //-----------------------------------------------------------
       tmeas_now = clock() / CLOCKS_PER_SEC;
       if(tmeas_now >= (output_evt + 1) ) {
          output_evt = clock() / CLOCKS_PER_SEC;
          sectimer++;
          if(sectimer >= 60) {
              sectimer = 0;
              if(mintimer++ >= 60) {
                 mintimer = 0;
                 if(hourtimer++ >= 24) hourtimer = 0;
              }
          }
          // read control-file cyclic 1 sec.
          CTRLFILE->ReadFiles();
          // !!! ****** ALARMAUSGABE ****** !!!
          if(alarmactive && !barrier && armed) {
              switch_relais(ON);
              // reset alarmtimecounter
              output_evt = 0;
              sectimer   = 0;
              mintimer   = 0;
              hourtimer  = 0;
              barrier = true;
          }
       }
       //-----------------------------------------------------------
       // Alarm Time to set unarmed
       //-----------------------------------------------------------
       if((mintimer >= ALARMTIME) && armed && alarmactive) {
           set_unarmed();
           barrier = false;
       }
       //-----------------------------------------------------------
       // Buzzeralarm
       //-----------------------------------------------------------
       if(armed && alarmactive) {
           OUT_BUZZER->setValue(high);
           usleep(250000);
           OUT_BUZZER->setValue(low);
           usleep(250000);
       }
       //-----------------------------------------------------------
       // Scharfschalter Einlesen
       //-----------------------------------------------------------
//       if(((IN_SCHARF->getNumericValue() == high) || (CTRLFILE->armed_from_file)) && !armed)  {
       if(((IN_SCHARF->getNumericValue() == high) || (CTRLFILE->armed_from_file)))  {
           set_armed();
           barrier = false;
       }
       //-----------------------------------------------------------
       // Unscharfschalter Einlesen
       //-----------------------------------------------------------
//       if(((IN_UNSCHARF->getNumericValue() == high) || !(CTRLFILE->armed_from_file)) && armed)  {
       if(((IN_UNSCHARF->getNumericValue() == high) || !(CTRLFILE->armed_from_file)))  {
           set_unarmed();
       }
   }
   pthread_exit(NULL);
}



//---------------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
int main()
{

    Logger::Start(Logger::DEBUG, "/Users/d042762/a.log");
    Logger::Write(Logger::INFO, "This message comes from task1");

struct sigaction action;
int    retval;

    program_end = false;
    sendsms     = false;
    armed       = false;
    alarmactive = false;

    // Set Termination Handler
    action.sa_handler = termination_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_NODEFER;
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGINT,  &action, NULL);

    // Logfile
    create_logfile();
    // IO'S, FILES CLASSES
    init_system();
    // Write Inifile first
    retval =  INIFILE->WriteINI(INIFILENAME);
    if(retval) {

    }

    // read inputfiles
    if(!CTRLFILE->ReadFiles())  { cout << "couldt not read controlfiles => exit"  << endl; return 0; }
    // write ctrlfiles
    if(!CTRLFILE->WriteFiles()) { cout << "couldt not write controlfiles => exit" << endl; return 0; }
    // TASKS
    if(!init_tasks())           { cout << "error while creating tasks => exit" << endl; return 0; };
    // first Logmessage
    return 0;
}


// fp = popen("./mailer.sh","w");
// if (fp == NULL) cout << "Failed sending email" << endl;
// else            cout << "Successs sending email" << endl;
// pclose(fp);



/*

  /etc/ssmtp/ssmtp.conf


# echo "Test" | mail -s "Test" ralf@pandel.de
#--------------------------------------------------
# Config file for sSMTP sendmail
#
# The person who gets all mail for userids < 1000
# Make this empty to disable rewriting.
root=ralf@pandel.de

# The place where the mail goes. The actual machine name is required no
# MX records are consulted. Commonly mailhosts are named mail.domain.com
mailhub=smtp.strato.de:587
AuthUser=ralf@pandel.de
AuthPass=
UserTLS=YES
UseSTARTTLS=YES
AuthLogin=YES


# Where will the mail seem to come from?
rewriteDomain=pandel.de

# The full hostname
hostname=ralf@pandel.de

# Are users allowed to set their own From: address?
# YES - Allow the user to specify their own From: address
# NO - Use the system generated From: address
FromLineOverride=YES

--------------------------------------------------------
Script mailer.sh

#!/bin/bash
echo "Alarm" | mail -s "Alarm" ralf@pandel.de

*/

