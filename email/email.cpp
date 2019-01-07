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

#include "email.h"
#include "../logger/logger.h"

using namespace std;
using namespace logger;

Email::Email()
{
    // TODO Auto-generated constructor stub

}

bool Email::send(void)
{
FILE  *fp;

    fp = popen(EMAILFILENAME,"w");
    if (fp == NULL) {
        Logger::Write(Logger::ERROR,"Failed sending email");
        cout << "Failed sending email" << endl;
        return false;
    }
    else {
        Logger::Write(Logger::INFO,"Successs sending email");
        cout << "Successs sending email" << endl;
    }
    pclose(fp);
    return true;
}


Email::~Email()
{
    // TODO Auto-generated destructor stub
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
