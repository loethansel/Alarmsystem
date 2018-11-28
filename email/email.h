/*
 * email.h
 *
 *  Created on: Nov 27, 2018
 *      Author: linux
 */

#ifndef EMAIL_EMAIL_H_
#define EMAIL_EMAIL_H_

namespace std
{

class email
{
public:
    email();
    bool send(void);
    virtual ~email();
};

} /* namespace std */

#endif /* EMAIL_EMAIL_H_ */
