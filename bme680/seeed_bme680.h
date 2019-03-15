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
// INCLUDES
#include "../bme680/bme680.h"
// NAMESPACES
using namespace std;

typedef struct Result
{
    float temperature;  
    float pressure;
    float humidity;
    float gas;
}sensor_result_t;


class Seeed_BME680
{
  public:
    Seeed_BME680();
    bool   init();
    int8_t read_sensor_data(void);
	float  read_temperature(void);
    float  read_pressure(void);
    float  read_humidity(void);
    float  read_gas(void);
    float  read_altitude(float seaLevel);
    bool   setTemperatureOversampling(uint8_t os);
    bool   setPressureOversampling(uint8_t os);
    bool   setHumidityOversampling(uint8_t os);
    bool   setIIRFilterSize(uint8_t fs);
    bool   setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);
    // Perform a reading in blocking mode.
    bool performReading(void);
    // Begin an asynchronous reading. Return When the reading would be ready as absolute time in millis().
    unsigned long beginReading(void);
    // End an asynchronous reading. Return Whether success.
    //  If the asynchronous reading is still in progress, block until it ends.
    //  If no asynchronous reading has started, this is equivalent to performReading().
    bool endReading(void);
    /// Temperature (Celsius) assigned after calling performReading() or endReading()
    float temperature;
    /// Pressure (Pascals) assigned after calling performReading() or endReading()
    float pressure;
    /// Humidity (RH %) assigned after calling performReading() or endReading()
    float humidity;
    /// Gas resistor (ohms) assigned after calling performReading() or endReading()
    float gas_resistance;
  private:
    bool _filterEnabled, _tempEnabled, _humEnabled, _presEnabled, _gasEnabled;
    uint8_t _i2caddr;
    unsigned long _meas_end;
	sensor_result_t sensor_result_value;
	// Official LIB structure
    struct bme680_dev sensor_param;
};

#endif
