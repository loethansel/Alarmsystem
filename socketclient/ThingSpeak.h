/*
 * ThingSpeak.h
 *
 *  Created on: 08.01.2019
 *      Author: linux
 */

#ifndef SOCKETCLIENT_THINGSPEAK_H_
#define SOCKETCLIENT_THINGSPEAK_H_

class ThingSpeak
{
public:
    ThingSpeak();
    void pushout(uint16_t feld, float value);
    virtual ~ThingSpeak();
};

#endif /* SOCKETCLIENT_THINGSPEAK_H_ */
