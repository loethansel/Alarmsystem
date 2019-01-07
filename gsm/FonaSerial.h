/*
 * FonaSerial.h
 *
 *  Created on: Jan 3, 2019
 *      Author: linux
 */

#ifndef GSM_FONASERIAL_H_
#define GSM_FONASERIAL_H_

using namespace std;

class FonaSerial {
public:
    FonaSerial();
    bool serialopen();
    bool serialclose();
    bool isopen();
    void flush();
    char read();
    bool read(char *readbuff, int len);
    bool write(uint8_t val);
    bool write(char *text, uint8_t len);
    bool write(const uint8_t *val, uint16_t len);
    bool write(const char *text, uint8_t len);
    bool print(const char *text);
    bool print(char *text);
    bool print(uint16_t val);
    bool print(int32_t val);
    bool print(uint32_t val);
    bool println(int32_t val,uint8_t dechex);
    bool println(const char *text);
    bool println(char *text);
    bool println(uint8_t val);
    bool println();
    bool available();
    bool peek();
    ~FonaSerial();
private:
    char writearr[255];
};


#endif /* GSM_FONASERIAL_H_ */
