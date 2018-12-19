/**
 * @file seeed_bme680.cpp
 * Library for BME680
 *
 * Copyright (c) 2013 Seeed Technology Co., Ltd.
 * @author        :   downey
 * @date Create Time   :   2017/12/08
 * Change Log    :
 */

/**
 * @brief The main driver file of BME680 sensor.
 *
 * BME680 support for temperature,humidity,indoor-air_quality(gas) and pressure value measurement,
 * The result of measurement is stored in class  Seeed_BME680->sensor_result_value.
 * BME680 support for two communication protocol-SPI and IIC,The different communication  protocol
 * corresponding different constructor.Furthermore,you can customize the pin when your development board's SPI
 * interface is mismatch with official.All you have to do is choose different ways to instantiate object.
 */
#include "../driver/seeed_bme680.h"
// NAMASPACES
using namespace std;
using namespace BlackLib;
// FOREWARD DECLARATIONS
int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
// CLASS DECLARATIONS
BlackI2C  bme_i2c(BlackLib::I2C_2, 0x76);

/**@brief constructor of IIC interface.
 * @param addr The BME680 device IIC address.
 * @return NONE.
 */
Seeed_BME680::Seeed_BME680()
{
    bool isOpened = bme_i2c.open( BlackLib::ReadWrite | BlackLib::NonBlock );
    if(!isOpened) std::cout << "I2C DEVICE CAN\'T OPEN.;" << std::endl;
    // ????
    sensor_param.intf = BME680_I2C_INTF;
}

float Seeed_BME680:: read_temperature(void)
{
	if(read_sensor_data())
	{
		return 0;
	}
	return sensor_result_value.temperature;
}

float Seeed_BME680:: read_pressure(void)
{
	if(read_sensor_data())
	{
		return 0;
	}
	return sensor_result_value.pressure;
}

float Seeed_BME680:: read_humidity(void)
{
	if(read_sensor_data())
	{
		return 0;
	}
	return sensor_result_value.humidity;
}

float Seeed_BME680:: read_gas(void)
{
	if(read_sensor_data())
	{
		return 0;
	}
	return sensor_result_value.gas;
}

int8_t Seeed_BME680::read_sensor_data(void) 
{
	
	struct bme680_field_data data;
	
	int8_t ret;
	sensor_param.power_mode = BME680_FORCED_MODE;
	uint16_t settings_sel;
	sensor_param.tph_sett.os_hum = BME680_OS_1X;
	sensor_param.tph_sett.os_pres = BME680_OS_16X;
	sensor_param.tph_sett.os_temp = BME680_OS_2X;
	sensor_param.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	// sensor_param.gas_sett.heatr_dur = 150;
	// sensor_param.gas_sett.heatr_temp = 320;
	sensor_param.gas_sett.heatr_dur = 100;
    sensor_param.gas_sett.heatr_temp = 300;
	settings_sel = BME680_OST_SEL | BME680_OSH_SEL | BME680_OSP_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;
	/*Set sensor's registers*/
	ret = bme680_set_sensor_settings(settings_sel, &sensor_param);
	if(ret != 0)
	{
		//Serial.print("bme680_set_sensor_settings() ==>ret value = ");
		//Serial.println(ret);
		return -1;
	}
	/*Set sensor's mode ,activate sensor*/
	ret = bme680_set_sensor_mode(&sensor_param);
	if(ret != 0)
	{
		//Serial.print("bme680_set_sensor_mode() ==>ret value = ");
		//Serial.println(ret);
		return -2;
	}
	
	uint16_t meas_period;
	bme680_get_profile_dur(&meas_period, &sensor_param);
	
	sleep(meas_period);
	
	/*Get sensor's result value from registers*/
	ret = bme680_get_sensor_data(&data, &sensor_param);
	if(ret != 0)
	{
		//Serial.print("bme680_get_sensor_data() ==>ret value = ");
		//Serial.println(ret);
		return -3;
	}
	sensor_result_value.temperature = data.temperature / 100.0;
	sensor_result_value.humidity    = data.humidity / 1000.0;
	sensor_result_value.pressure    = data.pressure;
	if (data.status & BME680_HEAT_STAB_MSK) 
	{
		sensor_result_value.gas = data.gas_resistance;
	} 
	else 
	{
		sensor_result_value.gas = 0;
	}
 	return BME680_OK;
}

int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  int32_t i = 0;
  //! unsicher, ob ich es richtig gemacht habe: testen!!!!
  bme_i2c.writeByte(reg_addr,len);
  if(len != bme_i2c.writeByte(reg_addr,len))
  {
    return 1;
  }
  for(i=0;i<len;i++)
  {
  	 reg_data[i]=(uint8_t) bme_i2c.readByte(reg_addr);
  }
  return 0;
}

int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
int32_t i = 0;

	for(i=0;i<len;i++)
	{
	    bme_i2c.writeByte(reg_addr,reg_data[i]);
	}
	return 0;
}


static void delay_msec(unsigned int ms)
{
  usleep(ms*1000);
}

bool Seeed_BME680::init() 
{
	/*User specifies IIC protocol in constructor.*/
    if(sensor_param.intf==BME680_I2C_INTF)
	{
	    sensor_param.read  = iic_read;
	    sensor_param.write = iic_write;
  	} 
    sensor_param.delay_ms = delay_msec;
    int8_t ret = BME680_OK;
    /*Check the wiring,Check whether the protocol stack is normal and Read the calibrated value from sensor */
    ret = bme680_init(&sensor_param);
    if(ret != 0)
    {
       // Serial.print("bme680_init() ==>ret value = ");
	   // Serial.println(ret);
	   return false;
    }
/*Print the calibrated value of sensor.*/
#if 0
  Serial.print("T1 = "); Serial.println(sensor_param.calib.par_t1);
  Serial.print("T2 = "); Serial.println(sensor_param.calib.par_t2);
  Serial.print("T3 = "); Serial.println(sensor_param.calib.par_t3);
  Serial.print("P1 = "); Serial.println(sensor_param.calib.par_p1);
  Serial.print("P2 = "); Serial.println(sensor_param.calib.par_p2);
  Serial.print("P3 = "); Serial.println(sensor_param.calib.par_p3);
  Serial.print("P4 = "); Serial.println(sensor_param.calib.par_p4);
  Serial.print("P5 = "); Serial.println(sensor_param.calib.par_p5);
  Serial.print("P6 = "); Serial.println(sensor_param.calib.par_p6);
  Serial.print("P7 = "); Serial.println(sensor_param.calib.par_p7);
  Serial.print("P8 = "); Serial.println(sensor_param.calib.par_p8);
  Serial.print("P9 = "); Serial.println(sensor_param.calib.par_p9);
  Serial.print("P10 = "); Serial.println(sensor_param.calib.par_p10);
  Serial.print("H1 = "); Serial.println(sensor_param.calib.par_h1);
  Serial.print("H2 = "); Serial.println(sensor_param.calib.par_h2);
  Serial.print("H3 = "); Serial.println(sensor_param.calib.par_h3);
  Serial.print("H4 = "); Serial.println(sensor_param.calib.par_h4);
  Serial.print("H5 = "); Serial.println(sensor_param.calib.par_h5);
  Serial.print("H6 = "); Serial.println(sensor_param.calib.par_h6);
  Serial.print("H7 = "); Serial.println(sensor_param.calib.par_h7);
  Serial.print("G1 = "); Serial.println(sensor_param.calib.par_gh1);
  Serial.print("G2 = "); Serial.println(sensor_param.calib.par_gh2);
  Serial.print("G3 = "); Serial.println(sensor_param.calib.par_gh3);
  Serial.print("G1 = "); Serial.println(sensor_param.calib.par_gh1);
  Serial.print("G2 = "); Serial.println(sensor_param.calib.par_gh2);
  Serial.print("G3 = "); Serial.println(sensor_param.calib.par_gh3);
  Serial.print("Heat Range = "); Serial.println(sensor_param.calib.res_heat_range);
  Serial.print("Heat Val = "); Serial.println(sensor_param.calib.res_heat_val);
  Serial.print("SW Error = "); Serial.println(sensor_param.calib.range_sw_err);
#endif

  return true;
}



