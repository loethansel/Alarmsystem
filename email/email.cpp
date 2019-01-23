/*
 * email.cpp
 *
 *  Created on: Nov 27, 2018
 *      Author: linux
 */
// INCLUDE SYSTEM
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "../logger/logger.h"
#include "../files/ctrlfile.h"
#include "../alarmsys.h"
#include "email.h"

using namespace std;
using namespace logger;

const char *a_filename="/home/debian/Alarmsystem/files/alarmmailer.sh";
const char *s_filename="/home/debian/Alarmsystem/files/servicemailer.sh";


void Email::CreateCheckFile(const char *fname,string text,string mailaddr)
{
ofstream mailfile;
int file_exists;

    //first check if the file exists...
    mailfile.open(fname,ios::in);
    if(mailfile.is_open()) { file_exists = 1; mailfile.close(); }
    //...then open it in the appropriate way
    if(file_exists != 1)
    {
       cout << "alarmfile does not exist!" << endl;
       mailfile.open(fname,ios::out);
       cout << "file opened succesfully!" << endl;
       mailfile << "#!/bin/bash" << endl;
       mailfile << "echo \"Alarmanlage\" | mail -s \""
                << text
                << "\" "
                << mailaddr << endl;
       mailfile.close();
       chmod(fname,S_IRWXO | S_IRWXU | S_IRWXG);
    }

}

void Email::CreateFile(const char *fname,string text,string mailaddr)
{
ofstream mailfile;
int file_exists;

    //first check if the file exists...
    cout << "alarmfile does not exist!" << endl;
    mailfile.open(fname,ios::trunc);
    cout << "file opened succesfully!" << endl;
    mailfile << "#!/bin/bash" << endl;
    mailfile << "echo \""
             << text
             << "\" | mail -s \""
             << ctrlfile->ini.ALARM.alarmtext
             << "\" "
             << mailaddr << endl;
    mailfile.close();
    chmod(fname,S_IRWXO | S_IRWXU | S_IRWXG);
}


Email::Email()
{
    CreateCheckFile(a_filename,"Gruene Halle",ctrlfile->ini.EMAIL.alarmmail[0]);
    CreateCheckFile(s_filename,"Gruene Halle",ctrlfile->ini.EMAIL.alarmmail[1]);
}

bool Email::execute(const char *filename)
{
FILE  *fp;

    fp = popen(filename,"w");
    if(fp == NULL) {
        Logger::Write(Logger::ERROR,"Failed sending email");
        cout << "Failed sending email" << endl;
        return false;
    }
    else {
        Logger::Write(Logger::INFO,"Successs sending email");
        cout << "Successs sending email" << endl;
        return true;
    }
    pclose(fp);
}


bool Email::send(uint8_t cmd,string text)
{

    switch(cmd) {
       case SERVICEMAIL:
           CreateFile(s_filename,text,ctrlfile->ini.EMAIL.servicemail);
           execute(s_filename);
       break;
       case ALARMMAIL:
           CreateFile(a_filename,text,ctrlfile->ini.EMAIL.alarmmail[0]);
           execute(a_filename);
           CreateFile(a_filename,text,ctrlfile->ini.EMAIL.alarmmail[1]);
           execute(a_filename);
       break;
       default:
       break;
    }
    return true;
}


Email::~Email()
{
}

// fp = popen("./mailer.sh","w");
// if (fp == NULL) cout << "Failed sending email" << endl;
// else            cout << "Successs sending email" << endl;
// pclose(fp);

/*
apt-get install ssmtp mailutils

  /etc/ssmtp/ssmtp.conf

# echo "Test" | mail -s "Test" email@email.de
#--------------------------------------------------
# Config file for sSMTP sendmail
#
# The person who gets all mail for userids < 1000
# Make this empty to disable rewriting.
root=email@email.de

# The place where the mail goes. The actual machine name is required no
# MX records are consulted. Commonly mailhosts are named mail.domain.com
mailhub=smtp.strato.de:587
AuthUser=email@email.de
AuthPass=*****
UserTLS=YES
UseSTARTTLS=YES
AuthLogin=YES

# Where will the mail seem to come from?
rewriteDomain=pxxdel.de

# The full hostname
hostname=email@email.de

# Are users allowed to set their own From: address?
# YES - Allow the user to specify their own From: address
# NO - Use the system generated From: address
FromLineOverride=YES

--------------------------------------------------------
Script mailer.sh

#!/bin/bash
echo "Alarm" | mail -s "Alarm" email@email.de

--------------------------------------------------------
*/
