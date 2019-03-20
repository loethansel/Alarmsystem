 /* ALARM TO EMAIL */
//---------------------------------------------------------------------------
// INCLUDE
//---------------------------------------------------------------------------
// INCLUDES SYSTEM
#include <iomanip>
// PROJECT
#include "alarmsys.h"
#include "logger/logger.h"
#include "timer/EmaTimer.h"
#include "bme680/seeed_bme680.h"
#include "xbee/xbeeproc.h"
#include "socketclient/ThingSpeak.h"
#include "socketserver/DisplayServer.h"
#include "relais/serialrelais.h"
#include "gsm/gsm_proc.h"
#include "analog/ain_proc.h"

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
bool  program_end;
bool  sendsms;
bool  armed;
bool  alarmactive;
bool  silentactive;
bool  contactopen;
bool  buzzeralarm;
bool  alarm_blocked;
bool  silent_blocked;
bool  xbeetest;

//---------------------------------------------------------------------------
// Threads Declarations
//---------------------------------------------------------------------------
pthread_t maintask;
//---------------------------------------------------------------------------
// FOREWARD Declarations
//---------------------------------------------------------------------------
void bme680_handler(union sigval arg);
void input_handler(union sigval arg);
void buzzer_handler(union sigval arg);
void disarm_handler(union sigval arg);
void autoalarm_handler(union sigval arg);
void termination_handler(int sig);
void *MainTask(void *value);
//---------------------------------------------------------------------------
// CLASS Declarations
//---------------------------------------------------------------------------
Seeed_BME680 bme;
Alert        ema;
EmaTimer bme680timer(bme680_handler);
EmaTimer inputtimer(input_handler);
EmaTimer buzzertimer(buzzer_handler);
EmaTimer disarmtimer(disarm_handler);
EmaTimer autoalarmtimer(autoalarm_handler);
// OUTPUTS
BlackLib::BlackGPIO  out_buzzer(BlackLib::GPIO_117,BlackLib::output,BlackLib::FastMode); // P9.25
BlackLib::BlackGPIO  out_led(BlackLib::GPIO_115,BlackLib::output,BlackLib::FastMode);    // P9.27
// INPUTS
BlackLib::BlackGPIO  in_arm(BlackLib::GPIO_50 ,BlackLib::input);    // P9.14
BlackLib::BlackGPIO  in_disarm(BlackLib::GPIO_51 ,BlackLib::input); // P9.16
// I2C-serialrelais
SerialRelais serialrelais;
// EMAIL
Email         *emailalarm;
// FILES
CtrlFile      *ctrlfile;
ThingSpeak    *tspeak;
// DISPLAY
DisplayServer *display;



//----------------------------------------------------------
// BME680_HANDLER 1h
//----------------------------------------------------------
void bme680_handler(union sigval arg)
{
string       s;
stringstream ss;

    if(!bme.performReading()) {
        Logger::Write(Logger::ERROR,"bm680 read failure");
        // message to display
        display->SetVal(BMESTATE,"readerror");
    }
    else {
        ss << "bme680: "
           << "temperature = " << bme.temperature << "*C; "
           << "humidity = "    << bme.humidity    << "%; "
           << "pressure = "    << bme.pressure    << "hPa; "
           << "gas resist= "   << bme.gas_resistance << " -;";
        s = ss.str();
        Logger::Write(Logger::INFO,s);
        // values to display
        ss.str(""); ss.clear();
        ss << fixed << setprecision(2) << bme.temperature;
        display->SetVal(TEMPSTATE,ss.str());
        ss.str(""); ss.clear();
        ss << fixed << setprecision(2) << bme.pressure;
        display->SetVal(PRESSTATE,ss.str());
        ss.str(""); ss.clear();
        ss << fixed << setprecision(2) << bme.humidity;
        display->SetVal(HUMISTATE,ss.str());
        // values to inet
        tspeak->setval(TEMPFIELD,bme.temperature);
        // message to display
        display->SetVal(BMESTATE,"running");
    }
    bme680timer.StartTimer();
    tspeak->pushall();
}

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
        ctrlfile->ReadActFiles();
        if(!(ctrlfile->armed_from_file))  {
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
    if(((in_arm.getNumericValue() == high) || (ctrlfile->armed_from_file)))  {
        ema.set_armed();
    }
    //-----------------------------------------------------------
    // read input disarm taster
    //-----------------------------------------------------------
    if(in_disarm.getNumericValue() == high)  {
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
        out_buzzer.setValue(high);
        usleep(100000);
        out_buzzer.setValue(low);
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
    if(ctrlfile->ini.ALARM.autoalarm == "true") autoalarmtimer.StartTimer();
}

//----------------------------------------------------------
// AUTOALARM_HANDLER
//----------------------------------------------------------
void autoalarm_handler(union sigval arg)
{
static int autocount = 0;

    Logger::Write(Logger::INFO,"autoalarm => set auto armed");
    if(++autocount <= stoi(ctrlfile->ini.ALARM.autocnt)) ema.set_armed();
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
   // ... LEAVING ALARMSYS
   Logger::Write(Logger::INFO, "...leaving alarmsystem process!");
   Logger::Stop();
   cout << "...bye bye" << endl;
   exit(0);
}

//-----------------------------------------------------------
// FILE_WORK
// READING OR WRITING FILES
//----------------------------------------------------------
bool Alert::file_work(void)
{
bool retval;

    // FILES
    ctrlfile = new CtrlFile;
    // INI-FILE
    if(!ctrlfile->CheckFileExists(INIFILENAME)) {
        // Write Inifile first
        ctrlfile->CreateDefaultIniFile();
        retval =  ctrlfile->WriteINI(INIFILENAME);
        if(retval) { Logger::Write(Logger::INFO, "creating default INI"); }
        else       { Logger::Write(Logger::ERROR, "could not create default INI"); return false; }
    } else {
        retval = ctrlfile->ReadIniFile();
        if(retval) { Logger::Write(Logger::INFO, "reading INI file"); }
        else       { Logger::Write(Logger::ERROR, "could not read INI file"); return false; }
    }
    // ACTION-CONTROL FILES
    Logger::Write(Logger::INFO, "read/write action control-files");
    if(!ctrlfile->CheckFileExists(ARMEDFILE)) {
        retval = ctrlfile->WriteActFiles();
        if(retval) { Logger::Write(Logger::INFO, "creating controlfile"); }
        else       { Logger::Write(Logger::ERROR,"could not create controlfile => exit"); return false; }
    } else {
        // Read Action ctrlfiles
        retval = ctrlfile->ReadActFiles();
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
stringstream ss;
string s;
string directionbuff[] = {"0","in","out","both"};

    // INPUTS
    ss.clear(); ss.str("");
    ss << "in_arm GPIO"  << dec << to_string(in_arm.getName()) << " "
       << "exported: "  << (in_arm.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
       << "direction: " << directionbuff[in_arm.getDirection()] << " "
       << "value: "     << in_arm.getValue();
    s = ss.str();
    Logger::Write(Logger::INFO,s);
    ss.clear(); ss.str("");
    ss << "in_disarm GPIO"  << dec << to_string(in_disarm.getName()) << " "
       << "exported: "  << (in_disarm.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
       << "direction: " << directionbuff[in_disarm.getDirection()] << " "
       << "value: "     << in_disarm.getValue();
    s = ss.str();
    Logger::Write(Logger::INFO,s);
    // OUTPUTS
    out_buzzer.setValue(low);
    ss.clear(); ss.str("");
    ss << "out_buzzer GPIO"  << dec << to_string(out_buzzer.getName()) << " "
       << "exported: "  << (out_buzzer.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
       << "direction: " << directionbuff[out_buzzer.getDirection()] << " "
       << "value: "     << out_buzzer.getValue();
    s = ss.str();
    Logger::Write(Logger::INFO,s);
    out_led.setValue(low);
    ss.clear(); ss.str("");
    ss << "out_led GPIO"  << dec << to_string(out_led.getName()) << " "
       << "exported: "  << (out_led.fail(BlackLib::BlackGPIO::exportErr)?"false":"true") << " "
       << "direction: " << directionbuff[out_led.getDirection()] << " "
       << "value: "     << out_led.getValue();
    s = ss.str();
    Logger::Write(Logger::INFO,s);
    // THINGSPEAK
    tspeak     = new ThingSpeak;
    // DISPLAY
    display    = new DisplayServer;
    // EMAIL
    emailalarm = new Email;
    emailalarm->send(SERVICEMAIL,"program started!");
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
    // Create XBEE-Task
    if(pthread_create(&xbeetask, NULL,&XbeeTask,NULL)) {
        Logger::Write(Logger::ERROR, "error creating XBee-task => exit");
        return false;
    }
    // Create Display-Task
    if(pthread_create(&displaytask, NULL,&DisplayTask,NULL)) {
        Logger::Write(Logger::ERROR, "error creating Display-task => exit");
        return false;
    }
    // message to display
    display->SetVal(SYSSTATE,"running");
    if(pthread_join(displaytask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined DISPLAY-task => exit");
    if(pthread_join(xbeetask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined XBEE-task => exit");
    if(pthread_join(aintask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined AIN-task => exit");
    if(pthread_join(gsmtask,NULL))  return false;
    Logger::Write(Logger::INFO, "joined GSM-task => exit");
    if(pthread_join(maintask,NULL)) return false;
    Logger::Write(Logger::INFO, "joined MAIN-task => exit");
    // free the memory
    emailalarm->send(SERVICEMAIL,"program terminated!");
    // message to display
    display->SetVal(SYSSTATE,"terminated");
    delete emailalarm;  // alarmmail-Modul
    delete ctrlfile;    // File-IO Modul
    delete tspeak;      // webinterface
    delete display;     // display
    return true;
}

//-----------------------------------------------------------
// SWITCH_serialrelais (serial)
//----------------------------------------------------------
bool Alert::switch_relais(bool onoff)
{   // switch on serial serialrelais
    if(onoff) {
        if(ctrlfile->ini.OUT_ACTIVE.out[0] == "true") serialrelais.turn_on_channel(1);
        if(ctrlfile->ini.OUT_ACTIVE.out[1] == "true") serialrelais.turn_on_channel(2);
        if(ctrlfile->ini.OUT_ACTIVE.out[2] == "true") serialrelais.turn_on_channel(3);
        if(ctrlfile->ini.OUT_ACTIVE.out[3] == "true") serialrelais.turn_on_channel(4);
    }
    // switch off serial serialrelais
    else {
        serialrelais.turn_off_channel(1);
        serialrelais.turn_off_channel(2);
        serialrelais.turn_off_channel(3);
        serialrelais.turn_off_channel(4);
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

    // only set to armed if not alarm line is active or armed yet
    if(armed || contactopen) return;
    Logger::Write(Logger::INFO, "alarmsystem ARMED!");
    ctrlfile->WriteSystemArmed(true);
    ctrlfile->Clear();
    retval = ctrlfile->ReadIniFile();
    if(retval) { Logger::Write(Logger::INFO,  "reading INI file during getting armed"); }
    else       { Logger::Write(Logger::ERROR, "could not read INI file ==> exit"); program_end = true; }
    armed          = true;
    alarm_blocked  = false;
    silent_blocked = false;
    out_led.setValue(high);
    for(i=0;i<3;i++) {
       out_buzzer.setValue(high);
       usleep(250000);
       out_buzzer.setValue(low);
       usleep(250000);
    }
    out_buzzer.setValue(high);
    usleep(500000);
    out_buzzer.setValue(low);
    XBeeSwitch(XBEEONOFF,CLR);
    tspeak->pushout(ARMEDFIELD,100.0);
}

//-----------------------------------------------------------
// SETUNARMED
//----------------------------------------------------------
void Alert::set_unarmed(void)
{
    display->SetVal(ALARMSTATE,"alarmoff");
    Logger::Write(Logger::INFO, "alarmsystem DISARMED!");
    buzzertimer.StopTimer();
    armed          = false;
    alarmactive    = false;
    buzzeralarm    = false;
    silentactive   = false;
    alarm_blocked  = true;
    silent_blocked = true;
    Logger::Write(Logger::INFO,"set alarm-actors off");
    switch_relais(OFF);
    ctrlfile->WriteSystemArmed(false);
    out_led.setValue(low);
    out_buzzer.setValue(high);
    sleep(1);
    out_buzzer.setValue(low);
    XBeeSwitch(XBEETIME,CLR);
    XBeeSwitch(XBEEALARM,CLR);
    XBeeSwitch(XBEEONOFF,SET);
    tspeak->pushout(ARMEDFIELD,0.0);
}

// Interval Timer Handler
//void main_handler(union sigval arg)
void Alert::main_handler(void)
{
    //-----------------------------------------------------------
    // !!! ****** ALARMOUTPUT ****** !!!
    // set alarm output actors
    //-----------------------------------------------------------
    if(alarmactive && !alarm_blocked && armed) {
        display->SetVal(ALARMSTATE,"alarmactive");
        Logger::Write(Logger::INFO,"set alarm-actors on");
        buzzertimer.StartTimer();
        switch_relais(ON);
        XBeeSwitch(XBEEALARM,SET);
        emailalarm->send(ALARMMAIL,"Halle Gruen");
        buzzeralarm   = true;
        sendsms       = true;
        alarm_blocked = true;
        disarmtimer.StartTimer();
    }
    //-----------------------------------------------------------
    // silent alarm: send only email and sms
    //-----------------------------------------------------------
    if(silentactive && armed) {
        if(!silent_blocked) {
            emailalarm->send(ALARMMAIL,"Halle Gruen");
            sendsms        = true;
            silent_blocked = true;
        }
    } else silent_blocked = false;
    //-----------------------------------------------------------
    // xbeetest
    //-----------------------------------------------------------
    if(xbeetest) {
        Logger::Write(Logger::INFO,"xbeetest");
        xbeetest = false;
        XBeeSwitch(XBEEALL,SET);
        sleep(2);
        XBeeSwitch(XBEEALL,CLR);
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

   // check serialrelais
   version = serialrelais.getFirmwareVersion();
   if(version == 0) {
       display->SetVal(RELAISSTATE,"error");
       Logger::Write(Logger::ERROR, "serial serialrelais did not respond");
   } else display->SetVal(RELAISSTATE,"ok");
    // switch off serial serialrelais
   ema.switch_relais(OFF);
   display->SetVal(ALARMSTATE,"alarmoff");
   try {
   // setup for disarm after alarm
   alarmtime = stoi(ctrlfile->ini.ALARM.alarmtime);
   disarmtimer.Create_Timer(0x00,(alarmtime*60));
   // setup for autoarm after alarm ends
   autoalarmtime = stoi(ctrlfile->ini.ALARM.autotime);
   autoalarmtimer.Create_Timer(0x00,(autoalarmtime*60));
   } catch(const exception& e) { cout << "catched exception maintask timer: " << e.what() << endl; }
   // set alarm buzzer cyclic
   buzzertimer.Create_Timer(100,0);
   // read digital an file inputs cyclic
   inputtimer.Create_Timer(100,0);
   inputtimer.StartTimer();
   // bme680 sensor
   if(bme.init()) {
       // read digital an file inputs cyclic
       try {
       bme680timer.Create_Timer(0,stoi(ctrlfile->ini.ALARM.infotime)*60);
       bme680timer.StartTimer();
       } catch(const exception& e) { cout << "catched exception maintask infotime: " << e.what() << endl; }
       Logger::Write(Logger::INFO,"bm680 init successful");
   }
   else Logger::Write(Logger::ERROR,"bm680 init failed");
   // forever main task ...
   while(1) {
       // intern signal program end
       if(program_end) break;
       ema.main_handler();
       // free cpu-time
	   usleep(200);
   }
   pthread_exit(NULL);
}

Alert::Alert()
{
    program_end    = false;
    sendsms        = false;
    armed          = false;
    alarmactive    = false;
    contactopen    = false;
    buzzeralarm    = false;
    silentactive   = false;
    silent_blocked = true;
    alarm_blocked  = true;
    xbeetest       = false;
}

Alert::~Alert()
{
}

//---------------------------------------------------------------------------
// MAIN
//-------
// getestet & ok
//---------------------------------------------------------------------------
int main()
{
struct sigaction action;
stringstream ss;
string        s;

    ss << "initializing alarmsystem software version: " << VERSION << " "
       << "date: " << __DATE__ << " "
       << "time: " << __TIME__;
    s = ss.str();
    cout << s << endl;
    Logger::Start(Logger::DEBUG, LOGFILENAME);
    Logger::Write(Logger::INFO, s);

    // Set Termination Handler
    action.sa_handler = termination_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = SA_NODEFER;
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGINT,  &action, NULL);

    // FILE READING WRITING
    if(!ema.file_work()) { cout << "error reading inifile" << endl; return 0; }
    // IO'S AND CLASSES
    ema.init_system();
    // INITIALISE TASKS
    if(ema.init_tasks()) { cout << "error init tasks" << endl; return 0; }
    return 0;
}



