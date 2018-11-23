/*
 * xbee.h
 *
 *  Created on: 13.11.2018
 *      Author: linux
 */

#ifndef XBEE_H_
#define XBEE_H_
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackUART/BlackUART.h"


using namespace std;
using namespace BlackLib;


class XBEE : public BlackUART
{
public:
	XBEE();
	virtual ~XBEE();
};

#endif /* XBEE_H_ */
