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
// I2C-RELAIS
serialrelais *RELAIS;
// I2C-RELAIS
xbee         *RADIORELAIS;
// EMAIL
email        *EMAILALARM;

void termination_handler(int sig)
{
stringstream ss;
string        s;

    ss << "Caught Signal: " << dec << sig << endl;
    s = ss.str();
    Logger::Write(Logger::INFO,s);

   Logger::Write(Logger::INFO, "Caught Signal: ");// << sig);
   // Logfile last Output
   Logger::Write(Logger::INFO, "close logfile..........");// << sig);
   // close Logfile
   ofs.close();
   exit(0);
}

// READING OR WRITING FILES
bool file_work(void)
{
bool retval;

    // INI-FILE
    if(!CTRLFILE->CheckFileExists(INIFILENAME)) {
        // Write Inifile first
        CTRLFILE->CreateDefaultIniFile();
        retval =  CTRLFILE->WriteINI(INIFILENAME);
        if(retval) { Logger::Write(Logger::INFO, "Creating Default INI"); }
        else       { Logger::Write(Logger::ERROR, "could not create Default INI"); return false; }
    } else {
        retval = CTRLFILE->ReadIniFile();
        if(retval) { Logger::Write(Logger::INFO, "Reading INI File"); }
        else       { Logger::Write(Logger::ERROR, "could not read INI File"); return false; }
    }
    // ACTION-CONTROL FILES
    Logger::Write(Logger::INFO, "Read/Write Action Control-Files");
    if(!CTRLFILE->CheckFileExists(ARMEDFILE)) {
        retval = CTRLFILE->WriteActFiles();
        if(retval) { Logger::Write(Logger::INFO, "Creating Controlfile"); }
        else       { Logger::Write(Logger::ERROR,"could not create Controlfile => exit"); return false; }
    } else {
        // Read Action ctrlfiles
        retval = CTRLFILE->ReadActFiles();
        if(retval)  { Logger::Write(Logger::INFO, "Reading Controlfile"); }
        else        { Logger::Write(Logger::ERROR,"could not read Controlfile => exit"); return false; }
    }
   return true;
}

void init_system(void)
{
    // INPUTS
    Logger::Write(Logger::INFO, "Main: Input GPIO's exported to /sys/class/gpio");
    IN_SCHARF   = new BlackGPIO(BlackLib::GPIO_50 ,BlackLib::input); // P9.14
    IN_UNSCHARF = new BlackGPIO(BlackLib::GPIO_51 ,BlackLib::input); // P9.16
    // OUTPUTS
    Logger::Write(Logger::INFO, "Main: Output GPIO's exported to /sys/class/gpio");
    OUT_BUZZER = new BlackGPIO(BlackLib::GPIO_117,BlackLib::output,BlackLib::FastMode); // P9.25
    OUT_BUZZER->setValue(low);
    OUT_LED    = new BlackGPIO(BlackLib::GPIO_115,BlackLib::output,BlackLib::FastMode); // P9.27
    OUT_LED->setValue(low);
    // FILES
    CTRLFILE = new ctrlfile;
    // RELAIS
    RELAIS   = new serialrelais;
    // XBEE-RADIO-RELAIS
    RADIORELAIS = new xbee;
    // EMAILALARM
    EMAILALARM = new email;

    // GPIO-OVERVIEW
    // GPIO_117 == P9.25  (OUT)
    // GPIO_115 == P9.27  (OUT)
    // GPIO_50  == P9.14  (OUT)
    // GPIO_51  == P9.16  (OUT)
    // GPIO_3   == P9.21  (OUT)
    // GPIO_2   == P9.22  (OUT)
    // GPIO_66  == P8.07  (IN)
    // GPIO_67  == P8.08  (IN)
    // GPIO_69  == P8.09  (IN)
    // GPIO_68  == P8.10  (IN)
    // GPIO_45  == P8.11  (IN)
    // GPIO_44  == P8.12  (IN)
    // GPIO_23  == P8.13  (IN)
    // GPIO_26  == P8.14  (IN)
}

bool init_tasks(void)
{
    Logger::Write(Logger::INFO, "Intialize tasks");
    // Create Main-Task
    if(pthread_create(&maintask, NULL,&MainTask,NULL)) {
        Logger::Write(Logger::ERROR, "error creating Main-task => exit");
        return false;
    }
    // Create ANALOG-Task
    if(pthread_create(&aintask, NULL,&AinTask,NULL)) {
        Logger::Write(Logger::ERROR, "error creating AIN-task => exit");
        return false;
    }
    // Create GSM-Task
    if(pthread_create(&gsmtask, NULL,&GsmTask,NULL)) {
        Logger::Write(Logger::ERROR, "error creating GSM-task => exit");
        return false;
    }
    if(pthread_join(aintask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined AIN-task => exit");
    if(pthread_join(gsmtask,NULL))  return false;
    Logger::Write(Logger::INFO, "joined GSM-task => exit");
    if(pthread_join(maintask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined MAIN-task => exit");
    delete IN_SCHARF;   // P9.14
    delete IN_UNSCHARF; // P9.16
    delete OUT_BUZZER;  // P9.25
    delete OUT_LED;     // P9.27
    delete CTRLFILE;    // File-IO Modul
    delete RELAIS;      // I2C-Relais-Modul
    delete RADIORELAIS; // xbee Relais-Modul
    delete EMAILALARM;  // emailalarm
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
bool retval;

    // only set to armed if not alarm line is active or armed yet
    if(armed || alarmactive) return;
    Logger::Write(Logger::INFO, "Alarmanlage scharf geschaltet!");
    retval = CTRLFILE->ReadIniFile();
    if(retval) { Logger::Write(Logger::INFO, "Reading INI File during getting armed"); }
    else       { Logger::Write(Logger::ERROR, "could not read INI File ==> exit"); program_end = true; }
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
    Logger::Write(Logger::INFO, "Alarmanlage unscharf geschaltet!");
    armed       = false;
    alarmactive = false;
    switch_relais(OFF);
    RADIORELAIS->switch_xbee(OFF);
    CTRLFILE->WriteSystemArmed(false);
    OUT_LED->setValue(low);
    OUT_BUZZER->setValue(high);
    sleep(2);
    OUT_BUZZER->setValue(low);
}

void *MainTask(void *value)
{
uint8_t version;
bool    armed_flag    = false;
//bool    unarmed_flag  = false;
static clock_t output_evt,tmeas_now;
static int sectimer   = 0;
static int mintimer   = 0;
static int hourtimer  = 0;
string autoalarmstr;
string alarmtime;
// bool   retval;


   // TEST
   version      = RELAIS->getFirmwareVersion();
   if(version == 0) Logger::Write(Logger::ERROR, "Relaisausgänge arbeiten nicht!");
   switch_relais(OFF);
   RADIORELAIS->switch_xbee(OFF);

   output_evt = 0;
   while(1) {
       // INTERES SIGNAL PRGRAM END!!
       if(program_end) break;
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
          // !!! ****** ALARMAUSGABE ****** !!!
          //-----------------------------------------------------------
          // SET ALARM OUTPUT ACTORS
          //-----------------------------------------------------------
          if(alarmactive && !armed_flag && armed) {
              switch_relais(ON);
              RADIORELAIS->switch_xbee(ON);
              EMAILALARM->send();
              // reset alarmtimecounter
              output_evt = 0;
              sectimer   = 0;
              mintimer   = 0;
              hourtimer  = 0;
              armed_flag = true;
          }
       }
       //-----------------------------------------------------------
       // Alarm Time to set unarmed
       //-----------------------------------------------------------
       if((mintimer >= ALARMTIME) && armed && armed_flag) {
           set_unarmed();
           armed_flag = false;
       }
       //-----------------------------------------------------------
       // Buzzeralarm
       //-----------------------------------------------------------
       if(armed && armed_flag) {
           OUT_BUZZER->setValue(high);
           usleep(250000);
           OUT_BUZZER->setValue(low);
           usleep(250000);
       }
       //-----------------------------------------------------------
       // Scharfschalter Einlesen
       //-----------------------------------------------------------
       // read control-file
       CTRLFILE->ReadActFiles();
       if(((IN_SCHARF->getNumericValue() == high) || (CTRLFILE->armed_from_file)))  {
            set_armed();
            armed_flag = false;
        }
        //-----------------------------------------------------------
        // Unscharfschalter Einlesen
        //-----------------------------------------------------------
        if(((IN_UNSCHARF->getNumericValue() == high) || !(CTRLFILE->armed_from_file)))  {
            set_unarmed();
            armed_flag = true;
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
struct sigaction action;

    Logger::Start(Logger::DEBUG, "/home/debian/Alarmsystem/files/alarm.log");
    Logger::Write(Logger::INFO,  "initializing Alarmsystem");

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

    // IO'S AND CLASSES
    init_system();
    // FILE READING WRITING
    if(!file_work()) return 0;
    // INITIALISE TASKS
    if(!init_tasks()) return 0;
    // ... LEAVING ALARMSYS
    Logger::Write(Logger::INFO, "...leaving Alarmsystem Process!");
    Logger::Stop();
    return 0;
}



