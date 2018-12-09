 /* ALARM TO EMAIL */
//---------------------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------------------
#include <mutex>
#include <ctime>
#include "alarmsys.h"
#include "logger/logger.h"
#include "timer/EmaTimer.h"

//---------------------------------------------------------------------------
// USING NAMESPACE
//---------------------------------------------------------------------------
using namespace std;
using namespace BlackLib;
using namespace logger;
//---------------------------------------------------------------------------
// GLOBAL VAR Declarations
//---------------------------------------------------------------------------
ofstream ofs;
bool program_end;
bool sendsms;
bool armed;
bool alarmactive;
bool contactopen;
bool buzzeralarm;
bool armed_blocked;

//---------------------------------------------------------------------------
// Threads Declarations
//---------------------------------------------------------------------------
pthread_t maintask;
//---------------------------------------------------------------------------
// FOREWARD Declarations
//---------------------------------------------------------------------------
void input_handler(union sigval arg);
void buzzer_handler(union sigval arg);
void disarm_handler(union sigval arg);
void autoalarm_handler(union sigval arg);
void termination_handler(int sig);
void *MainTask(void *value);
//---------------------------------------------------------------------------
// CLASS Declarations
//---------------------------------------------------------------------------
Alert    ema;
EmaTimer inputtimer(input_handler);
EmaTimer buzzertimer(buzzer_handler);
EmaTimer disarmtimer(disarm_handler);
EmaTimer autoalarmtimer(autoalarm_handler);

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


//----------------------------------------------------------
// INPUT_HANDLER 100ms
//----------------------------------------------------------
void input_handler(union sigval arg)
{
static int  cnt         = 0;
static bool pressedlock = false;

    //-----------------------------------------------------------
    // read input arm file every second
    //-----------------------------------------------------------
    if(cnt++ >= 10) {
        CTRLFILE->ReadActFiles();
        if(!(CTRLFILE->armed_from_file))  {
            if(armed) {
                ema.set_unarmed();
                autoalarmtimer.StopTimer();
                disarmtimer.StopTimer();
            }
        }
        cnt = 0;
    }
    //-----------------------------------------------------------
    // read input arm taster
    //-----------------------------------------------------------
    if(((IN_SCHARF->getNumericValue() == high) || (CTRLFILE->armed_from_file)))  {
        ema.set_armed();
    }
    //-----------------------------------------------------------
    // read input disarm taster
    //-----------------------------------------------------------
    if(IN_UNSCHARF->getNumericValue() == high)  {
        if(!pressedlock) {
            ema.set_unarmed();
            autoalarmtimer.StopTimer();
            disarmtimer.StopTimer();
        }
        pressedlock = true;
    } else pressedlock = false;
    inputtimer.StartTimer();
}

//----------------------------------------------------------
// BUZZER_HANDLER 500ms
//----------------------------------------------------------
void buzzer_handler(union sigval arg)
{
    if(buzzeralarm) {
        OUT_BUZZER->setValue(high);
        usleep(100000);
        OUT_BUZZER->setValue(low);
        usleep(100000);
    }
    buzzertimer.StartTimer();
}

//----------------------------------------------------------
// DISARM_HANDLER
//----------------------------------------------------------
void disarm_handler(union sigval arg)
{
    Logger::Write(Logger::INFO,"alarmtime elapsed => set auto disarmed");
    ema.set_unarmed();
    if(CTRLFILE->ini.ALARM.autoalarm == "true") autoalarmtimer.StartTimer();
}

//----------------------------------------------------------
// AUTOALARM_HANDLER
//----------------------------------------------------------
void autoalarm_handler(union sigval arg)
{
static int autocount = 0;

    Logger::Write(Logger::INFO,"autoalarm => set auto armed");
    if(++autocount <= stoi(CTRLFILE->ini.ALARM.autocnt)) ema.set_armed();
}

//----------------------------------------------------------
// TERMINATION_HANDLER
//----------------------------------------------------------
void termination_handler(int sig)
{
stringstream ss;
string        s;

   ss << "caught signal: " << dec << sig << " => process termination..." << endl;
   s = ss.str();
   Logger::Write(Logger::INFO,s);
   exit(0);
}

//-----------------------------------------------------------
// FILE_WORK
// READING OR WRITING FILES
//----------------------------------------------------------
bool Alert::file_work(void)
{
bool retval;

    // INI-FILE
    if(!CTRLFILE->CheckFileExists(INIFILENAME)) {
        // Write Inifile first
        CTRLFILE->CreateDefaultIniFile();
        retval =  CTRLFILE->WriteINI(INIFILENAME);
        if(retval) { Logger::Write(Logger::INFO, "creating default INI"); }
        else       { Logger::Write(Logger::ERROR, "could not create default INI"); return false; }
    } else {
        retval = CTRLFILE->ReadIniFile();
        if(retval) { Logger::Write(Logger::INFO, "reading INI file"); }
        else       { Logger::Write(Logger::ERROR, "could not read INI file"); return false; }
    }
    // ACTION-CONTROL FILES
    Logger::Write(Logger::INFO, "read/write action control-files");
    if(!CTRLFILE->CheckFileExists(ARMEDFILE)) {
        retval = CTRLFILE->WriteActFiles();
        if(retval) { Logger::Write(Logger::INFO, "creating controlfile"); }
        else       { Logger::Write(Logger::ERROR,"could not create controlfile => exit"); return false; }
    } else {
        // Read Action ctrlfiles
        retval = CTRLFILE->ReadActFiles();
        if(retval)  { Logger::Write(Logger::INFO, "reading controlfile"); }
        else        { Logger::Write(Logger::ERROR,"could not read controlfile => exit"); return false; }
    }
   return true;
}

//-----------------------------------------------------------
// INIT_SYSTEM
//----------------------------------------------------------
void Alert::init_system(void)
{
    // INPUTS
    Logger::Write(Logger::INFO, "input GPIO's exported to /sys/class/gpio");
    IN_SCHARF   = new BlackGPIO(BlackLib::GPIO_50 ,BlackLib::input); // P9.14
    IN_UNSCHARF = new BlackGPIO(BlackLib::GPIO_51 ,BlackLib::input); // P9.16
    // OUTPUTS
    Logger::Write(Logger::INFO, "output GPIO's exported to /sys/class/gpio");
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

//-----------------------------------------------------------
// INIT_TASKS
//----------------------------------------------------------
bool Alert::init_tasks(void)
{
    Logger::Write(Logger::INFO, "intialize tasks");
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
    // free the memory
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

//-----------------------------------------------------------
// SWITCH_RELAIS (serial)
//----------------------------------------------------------
bool Alert::switch_relais(bool onoff)
{   // switch on serial relais
    if(onoff) {
        RELAIS->turn_on_channel(1);
        RELAIS->turn_on_channel(2);
        RELAIS->turn_on_channel(3);
        RELAIS->turn_on_channel(4);
    }
    // switch off serial relais
    else {
        RELAIS->turn_off_channel(1);
        RELAIS->turn_off_channel(2);
        RELAIS->turn_off_channel(3);
        RELAIS->turn_off_channel(4);
    }
    return true;
}

//-----------------------------------------------------------
// SET_ARMED
//----------------------------------------------------------
void Alert::set_armed(void)
{
int i;
bool  retval;
mutex mtx;

    // only set to armed if not alarm line is active or armed yet
    if(armed || contactopen) return;
    cout << "scharf" << endl;
    Logger::Write(Logger::INFO, "alarmsystem ARMED!");
    mtx.lock();
    CTRLFILE->WriteSystemArmed(true);
    CTRLFILE->Clear();
    retval = CTRLFILE->ReadIniFile();
    mtx.unlock();
    if(retval) { Logger::Write(Logger::INFO,  "reading INI file during getting armed"); }
    else       { Logger::Write(Logger::ERROR, "could not read INI file ==> exit"); program_end = true; }
    armed         = true;
    armed_blocked = false;
    OUT_LED->setValue(high);
    for(i=0;i<3;i++) {
       OUT_BUZZER->setValue(high);
       usleep(250000);
       OUT_BUZZER->setValue(low);
       usleep(250000);
    }
    OUT_BUZZER->setValue(high);
    usleep(500000);
    OUT_BUZZER->setValue(low);
}


//-----------------------------------------------------------
// SETUNARMED
//----------------------------------------------------------
void Alert::set_unarmed(void)
{
mutex mtx;

    Logger::Write(Logger::INFO, "alarmsystem DISARMED!");
    buzzertimer.StopTimer();
    cout << "unscharf" << endl;
    armed         = false;
    alarmactive   = false;
    buzzeralarm   = false;
    armed_blocked = true;
    Logger::Write(Logger::INFO,"set alarm-actors off");
    mtx.lock();
    RADIORELAIS->switch_xbee(OFF);
    switch_relais(OFF);
    CTRLFILE->WriteSystemArmed(false);
    mtx.unlock();
    OUT_LED->setValue(low);
    OUT_BUZZER->setValue(high);
    sleep(1);
    OUT_BUZZER->setValue(low);
}

// Interval Timer Handler
//void main_handler(union sigval arg)
void Alert::main_handler(void)
{
    //-----------------------------------------------------------
    // !!! ****** ALARMOUTPUT ****** !!!
    // set alarm output actors
    //-----------------------------------------------------------
    if(alarmactive && !armed_blocked && armed) {
        Logger::Write(Logger::INFO,"set alarm-actors on");
        cout << "set alarm actors" << endl;
        buzzertimer.StartTimer();
        switch_relais(ON);
        RADIORELAIS->switch_xbee(ON);
        EMAILALARM->send();
        buzzeralarm = true;
        sendsms     = true;
        disarmtimer.StartTimer();
        armed_blocked = true;
    }
}

//-----------------------------------------------------------
// MAINTASK
//----------------------------------------------------------
void *MainTask(void *value)
{
uint8_t version;
int     alarmtime;
int     autoalarmtime;

   // check relais
   version      = RELAIS->getFirmwareVersion();
   if(version == 0) Logger::Write(Logger::ERROR, "serial relais did not respond");
   // switch off serial relais
   ema.switch_relais(OFF);
   // switch off radio relais
   RADIORELAIS->switch_xbee(OFF);
   armed_blocked = false;
   buzzeralarm   = false;
   // setuup for disarm after alarm
   alarmtime = stoi(CTRLFILE->ini.ALARM.alarmtime);
   disarmtimer.Create_Timer(0x00,(alarmtime*60));
   // setup for autoarm after alarm ends
   autoalarmtime = stoi(CTRLFILE->ini.ALARM.autotime);
   autoalarmtimer.Create_Timer(0x00,(autoalarmtime*60));
   // set alarm buzzer cyclic
   buzzertimer.Create_Timer(100,0);
   // read digital an file inputs cyclic
   inputtimer.Create_Timer(100,0);
   inputtimer.StartTimer();

   // forever main task ...
   while(1) {
       // intern signal program end
       if(program_end) break;
       ema.main_handler();
       // free cpu-time
	   sleep(1);
   }
   pthread_exit(NULL);
}

Alert::Alert()
{
    program_end   = false;
    sendsms       = false;
    armed         = false;
    alarmactive   = false;
    contactopen   = false;
    buzzeralarm   = false;
    armed_blocked = true;
}

Alert::~Alert()
{
    // ... LEAVING ALARMSYS
    Logger::Write(Logger::INFO, "...leaving alarmsystem process!");
    Logger::Stop();
    cout << "...bye bye" << endl;
}

//---------------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------------
// getestet & ok
//---------------------------------------------------------------------------
int main()
{
struct sigaction action;

    Logger::Start(Logger::DEBUG, LOGFILENAME);
    Logger::Write(Logger::INFO,  "initializing alarmsystem");

    // Set Termination Handler
    action.sa_handler = termination_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_NODEFER;
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGINT,  &action, NULL);

    // IO'S AND CLASSES
    ema.init_system();
    // FILE READING WRITING
    if(!ema.file_work()) return 0;
    // INITIALISE TASKS
    if(ema.init_tasks()) return 0;
    return 0;
}



