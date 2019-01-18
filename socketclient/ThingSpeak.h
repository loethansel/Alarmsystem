/*
 * ThingSpeak.h
 *
 *  Created on: 08.01.2019
 *      Author: linux
 */

#ifndef SOCKETCLIENT_THINGSPEAK_H_
#define SOCKETCLIENT_THINGSPEAK_H_

#define ARMEDFIELD  1
#define RSSIFIELD   2
#define CREDITFIELD 3
#define LINE1FIELD  4
#define LINE2FIELD  5
#define LINE3FIELD  6
#define LINE4FIELD  7
#define TEMPFIELD   8

using namespace std;

class ThingSpeak
{
public:
    ThingSpeak();
    void setval(uint8_t field,float value);
    void pushout(uint16_t feld, float value);
    void pushall(void);
    virtual ~ThingSpeak();
private:
    float fieldval[8];
    string writekey;
    string readkey;
};

#endif /* SOCKETCLIENT_THINGSPEAK_H_ */
