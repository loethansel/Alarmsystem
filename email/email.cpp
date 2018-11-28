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

namespace std
{

email::email()
{
    // TODO Auto-generated constructor stub

}

bool email::send(void)
{
FILE  *fp;

    fp = popen("./mailer.sh","w");
    if (fp == NULL) cout << "Failed sending email" << endl;
    else            cout << "Successs sending email" << endl;
    pclose(fp);


}


email::~email()
{
    // TODO Auto-generated destructor stub
}

} /* namespace std */


// fp = popen("./mailer.sh","w");
// if (fp == NULL) cout << "Failed sending email" << endl;
// else            cout << "Successs sending email" << endl;
// pclose(fp);



/*


apt-get install ssmtp mailutils

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
