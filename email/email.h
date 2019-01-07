/*
 * email.h
 *
 *  Created on: Nov 27, 2018
 *      Author: linux
 */

#ifndef EMAIL_EMAIL_H_
#define EMAIL_EMAIL_H_

#define EMAILFILENAME    "/home/debian/Alarmsystem/files/mailer.sh"

namespace std
{

class Email
{
public:
    Email();
    bool send(void);
    virtual ~Email();
};

} /* namespace std */

#endif /* EMAIL_EMAIL_H_ */
