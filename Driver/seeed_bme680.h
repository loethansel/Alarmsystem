/*
 * seeed_bme680.h
 * Library for BME680
 *
 * Copyright (c) 2013 Seeed Technology Co., Ltd.
 * Author        :   downey
 * Create Time   :   2017/12/08
 * Change Log    :
*/
#ifndef _SEEED_BME680_H
#define _SEEED_BME680_H
// INCLUDES PROGRAM
#include <ctime>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "bme680.h"
#include "../blacklib/BlackLib.h"
#include "../blacklib/BlackI2C/BlackI2C.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;


typedef struct Result
{
    float temperature;  
    float pressure;
    float humidity;
    float gas;
}sensor_result_t;


class Seeed_BME680 : public BlackI2C
{
  public:
    Seeed_BME680();
    bool  init();
    int8_t read_sensor_data(void);
	float  read_temperature(void);
    float  read_pressure(void);
    float  read_humidity(void);
    float  read_gas(void);
	sensor_result_t sensor_result_value;
  private:       
	int8_t iic_write(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
	int8_t iic_read(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
    struct bme680_dev sensor_param;      /**< Official LIB structure.*/
};

#endif
