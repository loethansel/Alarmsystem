 /* ALARM TO EMAIL */
//---------------------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------------------
#include <sys/time.h>
#include "alarmsys.h"
#include "logger/logger.h"

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
bool auto_disarmed;

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
bool file_work(void)
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
void init_system(void)
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
bool init_tasks(void)
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
bool switch_relais(bool onoff)
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
void set_armed(void)
{
int i;
bool retval;

    // only set to armed if not alarm line is active or armed yet
    if(armed || contactopen) return;
    cout << "scharf" << endl;
    Logger::Write(Logger::INFO, "alarmsystem ARMED!");
    CTRLFILE->Clear();
    retval = CTRLFILE->ReadIniFile();
    if(retval) { Logger::Write(Logger::INFO,  "reading INI file during getting armed"); }
    else       { Logger::Write(Logger::ERROR, "could not read INI file ==> exit"); program_end = true; }
    armed         = true;
    armed_blocked = false;
    auto_disarmed = false;
    CTRLFILE->WriteSystemArmed(true);
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
void set_unarmed(void)
{
    Logger::Write(Logger::INFO, "alarmsystem DISARMED!");
    cout << "unscharf" << endl;
    armed         = false;
    alarmactive   = false;
    buzzeralarm   = false;
    armed_blocked = true;
    auto_disarmed = false;
    Logger::Write(Logger::INFO,"set alarm-actors off");
    switch_relais(OFF);
    RADIORELAIS->switch_xbee(OFF);
    CTRLFILE->WriteSystemArmed(false);
    OUT_LED->setValue(low);
    OUT_BUZZER->setValue(high);
    sleep(1);
    OUT_BUZZER->setValue(low);
}

// Interval Timer Handler
void main_handler(union sigval arg)
{
static int sectimer   = 0;
static int mintimer   = 0;
static int hourtimer  = 0;
string autoalarmstr;
static int alarmtime;
static int autoalarmtime;

    // Read action-files every Second
    CTRLFILE->ReadActFiles();
    sectimer++;
    if(sectimer >= 60) { mintimer++; sectimer = 0; }
    if(mintimer >= 60) { hourtimer++; mintimer= 0; sectimer = 0; }
    // !!! ****** ALARMOUTPUT ****** !!!
    //-----------------------------------------------------------
    // set alarm output actors
    //-----------------------------------------------------------
    if(alarmactive && !armed_blocked && armed) {
        Logger::Write(Logger::INFO,"set alarm-actors on");
        cout << "set alarm actors" << endl;
        switch_relais(ON);
        RADIORELAIS->switch_xbee(ON);
        EMAILALARM->send();
        buzzeralarm = true;
        sendsms     = true;
        // reset alarm-time-counter
        sectimer      = 0;
        mintimer      = 0;
        hourtimer     = 0;
        armed_blocked = true;
    }
    //-----------------------------------------------------------
    // buzzer-alarm
    //----------------------------------------------------------
    if(buzzeralarm) {
        OUT_BUZZER->setValue(high);
        usleep(100000);
        OUT_BUZZER->setValue(low);
        usleep(100000);
    }
    //-----------------------------------------------------------
    // alarm-time, waiting to set unarmed in minutes
    //-----------------------------------------------------------
    alarmtime = stoi(CTRLFILE->ini.ALARM.alarmtime);
    if((mintimer >= alarmtime) && alarmactive) {
        Logger::Write(Logger::INFO,"alarmtime elapsed => set auto disarmed");
        set_unarmed();
        auto_disarmed = true;
        // reset alarm-time-counter for autoarm
        sectimer      = 0;
        mintimer      = 0;
        hourtimer     = 0;
    }
    //-----------------------------------------------------------
    // autoalarm
    //-----------------------------------------------------------
    autoalarmtime = stoi(CTRLFILE->ini.ALARM.autotime);
    if(auto_disarmed && (CTRLFILE->ini.ALARM.autoalarm == "true") && (mintimer >= autoalarmtime)) {
        Logger::Write(Logger::INFO,"autoalarm => set auto armed");
        set_armed();
        auto_disarmed = false;
    }
}

void create_timer_mainproc(int i)
{
timer_t timer_id;
int status;
struct itimerspec ts;
struct sigevent se;
long long nanosecs = 1000000 * 100 * i * i;

    // Set the sigevent structure to cause the signal to be delivered by creating a new thread.
    se.sigev_notify            = SIGEV_THREAD;
    se.sigev_value.sival_ptr   = &timer_id;
    se.sigev_notify_function   = main_handler;
    se.sigev_notify_attributes = NULL;

    ts.it_value.tv_sec  = nanosecs / 1000000000;
    ts.it_value.tv_nsec = nanosecs % 1000000000;
    ts.it_interval.tv_sec  = 1;
    ts.it_interval.tv_nsec = 300000;

    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if (status == -1) cout << "Create timer" << endl;
    // TODO maybe we'll need to have an array of itimerspec
    status = timer_settime(timer_id, 0, &ts, 0);
    if (status == -1) cout << "Set timer" << endl;
}



//-----------------------------------------------------------
// MAINTASK
//----------------------------------------------------------
void *MainTask(void *value)
{
uint8_t version;

   // check relais
   version      = RELAIS->getFirmwareVersion();
   if(version == 0) Logger::Write(Logger::ERROR, "serial relais did not respond");
   // switch off serial relais
   switch_relais(OFF);
   // switch off radio relais
   RADIORELAIS->switch_xbee(OFF);
   armed_blocked = false;
   buzzeralarm   = false;

   create_timer_mainproc(10000);
   // forever main task ...
   while(1) {
       // intern signal program end
       if(program_end) break;
       // free cpu-time
	   usleep(100000);
	    //-----------------------------------------------------------
	    // read input arm taster
	    //-----------------------------------------------------------
	    if(((IN_SCHARF->getNumericValue() == high) || (CTRLFILE->armed_from_file)))  {
	        set_armed();
	    }
	    //-----------------------------------------------------------
	    // read input disarm taster
	    //-----------------------------------------------------------
	    if(IN_UNSCHARF->getNumericValue() == high)  {
	        set_unarmed();
	    }
	    if(!(CTRLFILE->armed_from_file))  {
	        if(armed) set_unarmed();
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

    Logger::Start(Logger::DEBUG, LOGFILENAME);
    Logger::Write(Logger::INFO,  "initializing alarmsystem");

    program_end   = false;
    sendsms       = false;
    armed         = false;
    alarmactive   = false;
    contactopen   = false;
    buzzeralarm   = false;
    auto_disarmed = false;
    armed_blocked = true;

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
    Logger::Write(Logger::INFO, "...leaving alarmsystem process!");
    Logger::Stop();
    return 0;
}



