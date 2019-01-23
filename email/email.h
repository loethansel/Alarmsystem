/*
 * email.h
 *
 *  Created on: Nov 27, 2018
 *      Author: linux
 */

#ifndef EMAIL_EMAIL_H_
#define EMAIL_EMAIL_H_

//#define EMAILFILENAME "/home/debian/Alarmsystem/files/mailer.sh"
#define SERVICEMAIL 0
#define ALARMMAIL   1

using namespace std;

class Email
{
public:
    Email();
    void CreateCheckFile(const char *fname,string text,string mailaddr);
    void CreateFile(const char *fname,string text,string mailaddr);
    bool send(uint8_t cmd,string text);
    bool execute(const char *filename);
    virtual ~Email();
};

#endif /* EMAIL_EMAIL_H_ */
