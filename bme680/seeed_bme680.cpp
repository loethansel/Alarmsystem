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
#include "seeed_bme680.h"
// NAMASPACES
using namespace std;
using namespace BlackLib;
// FOREWARD DECLARATIONS
int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
// CLASS DECLARATIONS
BlackI2C  bme_i2c(BlackLib::I2C_2, 0x76);


Seeed_BME680::Seeed_BME680()
{
    bool isOpened = bme_i2c.open( BlackLib::ReadWrite | BlackLib::NonBlock );
    if(!isOpened) std::cout << "I2C DEVICE CAN\'T OPEN.;" << std::endl;
    else cout << "BME680 on I2C_2 open...." << endl;
    sensor_param.intf = BME680_I2C_INTF;
    _i2caddr = 0x76;
    _filterEnabled = _tempEnabled = _humEnabled = _presEnabled = _gasEnabled = false;
    _meas_end = 0;
    sensor_param.dev_id    = _i2caddr;
    sensor_param.chip_id   = BME680_CHIP_ID;
    temperature = pressure = humidity = gas_resistance = 0.0;
}

float Seeed_BME680::read_temperature(void)
{
	if(read_sensor_data()) return 0;
	return sensor_result_value.temperature;
}

float Seeed_BME680::read_pressure(void)
{
	if(read_sensor_data()) return 0;
	return sensor_result_value.pressure;
}

float Seeed_BME680::read_humidity(void)
{
	if(read_sensor_data()) return 0;
	return sensor_result_value.humidity;
}

float Seeed_BME680::read_gas(void)
{
	if(read_sensor_data()) return 0;
	return sensor_result_value.gas;
}

int8_t Seeed_BME680::read_sensor_data(void) 
{
struct bme680_field_data data;
int8_t ret;
uint16_t settings_sel;

	sensor_param.power_mode          = BME680_FORCED_MODE;
	sensor_param.tph_sett.os_hum     = BME680_OS_1X;
	sensor_param.tph_sett.os_pres    = BME680_OS_16X;
	sensor_param.tph_sett.os_temp    = BME680_OS_2X;
	sensor_param.gas_sett.run_gas    = BME680_ENABLE_GAS_MEAS;
	sensor_param.gas_sett.heatr_dur  = 100;
    sensor_param.gas_sett.heatr_temp = 300;
	settings_sel = BME680_OST_SEL | BME680_OSH_SEL | BME680_OSP_SEL | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;
	// Set sensor's registers
	ret = bme680_set_sensor_settings(settings_sel, &sensor_param);
	if(ret != 0)
	{
		cout << "bme680_set_sensor_settings() ==>ret value = " << ret << endl;
		return -1;
	}
	// Set sensor's mode ,activate sensor
	ret = bme680_set_sensor_mode(&sensor_param);
	if(ret != 0)
	{
		cout << "bme680_set_sensor_mode() ==>ret value = " << ret << endl;
		return -2;
	}
	uint16_t meas_period;
	bme680_get_profile_dur(&meas_period, &sensor_param);
	
	usleep(meas_period*1000);
	
	// Get sensor's result value from registers
	ret = bme680_get_sensor_data(&data, &sensor_param);
	if(ret != 0)
	{
		cout << "bme680_get_sensor_data() ==>ret value = " << ret << endl;
		return -3;
	}
	sensor_result_value.temperature = data.temperature /  100.0;
	sensor_result_value.humidity    = data.humidity    / 1000.0;
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


float Seeed_BME680::read_altitude(float seaLevel)
{
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
    float atmospheric = read_pressure() / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/**************************************************************************/
/*!
    @brief Performs a full reading of all 4 sensors in the BME680.
    Assigns the internal Adafruit_BME680#temperature, Adafruit_BME680#pressure, Adafruit_BME680#humidity
    and Adafruit_BME680#gas_resistance member variables
    @return True on success, False on failure
*/
/**************************************************************************/
bool Seeed_BME680::performReading(void) {
  return endReading();
}

unsigned long Seeed_BME680::beginReading(void)
{
uint8_t set_required_settings = 0;
uint8_t rslt;
struct timeval tmnow;

  // A measurement is already in progress
  if(_meas_end != 0) return _meas_end;
  // Select the power mode
  // Must be set before writing the sensor configuration
  sensor_param.power_mode = BME680_FORCED_MODE;
  // Set the required sensor settings needed
  if(_tempEnabled)   set_required_settings |= BME680_OST_SEL;
  if(_humEnabled)    set_required_settings |= BME680_OSH_SEL;
  if(_presEnabled)   set_required_settings |= BME680_OSP_SEL;
  if(_filterEnabled) set_required_settings |= BME680_FILTER_SEL;
  if(_gasEnabled)    set_required_settings |= BME680_GAS_SENSOR_SEL;
  // Set the desired sensor configuration
#ifdef BME680_DEBUG
  cout << "Setting sensor settings" << endl;
#endif
  rslt = bme680_set_sensor_settings(set_required_settings, &sensor_param);
  if(rslt != BME680_OK) return 0;
  // Set the power mode
#ifdef BME680_DEBUG
  cout << "Setting power mode" << endl;
#endif
  rslt = bme680_set_sensor_mode(&sensor_param);
  if(rslt != BME680_OK) return 0;
  // Get the total measurement duration so as to sleep or wait till the measurement is complete
  uint16_t meas_period;
  bme680_get_profile_dur(&meas_period, &sensor_param);
  gettimeofday(&tmnow, NULL);
  _meas_end = (tmnow.tv_usec / 1000) + meas_period;
  return _meas_end;
}


bool Seeed_BME680::endReading(void)
{
struct timeval tmnow;
unsigned long meas_end;

   meas_end = beginReading();
   if(meas_end == 0) return false;

  gettimeofday(&tmnow, NULL);
  unsigned long now = (tmnow.tv_usec / 1000);

  if(meas_end > now) {
    unsigned long meas_period = meas_end - now;
#ifdef BME680_DEBUG
    cout << "Waiting (ms) " << meas_period;
#endif
    // Delay till the measurement is ready
    usleep((meas_period * 2) * 1000);
    // delay(meas_period * 2);
  }
  _meas_end = 0; /* Allow new measurement to begin */

#ifdef BME680_DEBUG
  cout << "t_fine = " << gas_sensor.calib.t_fine << endl;
#endif

  struct bme680_field_data data;
  cout << "Getting sensor data" << endl;
  int8_t rslt = bme680_get_sensor_data(&data, &sensor_param);
  if (rslt != BME680_OK) return false;

  if (_tempEnabled) {
#ifdef BME680_DEBUG
    cout << "Temp: " << fixed << setprecision(2) << (data.temperature / 100.0) << endl;
#endif
    temperature = data.temperature / 100.0;
  } else {
    temperature = NAN;
  }
  if (_humEnabled) {
#ifdef BME680_DEBUG
    cout << "Hum:  " << fixed << setprecision(2) << (data.humidity / 1000.0) << endl;
#endif
    humidity = data.humidity / 1000.0;
  } else {
    humidity = NAN;
  }
  if (_presEnabled) {
#ifdef BME680_DEBUG
    cout << "Pres: " << fixed << setprecision(2) << (data.pressure / 100.0) << endl;
#endif
    pressure = data.pressure / 100.0;
  } else {
    pressure = NAN;
  }
  // Avoid using measurements from an unstable heating setup */
  if (_gasEnabled) {
    if (data.status & BME680_HEAT_STAB_MSK) {
#ifdef BME680_DEBUG
      cout << "Gas resistance: " << fixed << setprecision(2) << (data.gas_resistance / 1.0) << endl;
#endif
      gas_resistance = data.gas_resistance;
    } else {
      gas_resistance = 0;
#ifdef BME680_DEBUG
      cout << "Gas reading unstable!" << endl;
#endif
    }
  }
  return true;
}

/**************************************************************************/
/*!
    @brief  Enable and configure gas reading + heater
    @param  heaterTemp Desired temperature in degrees Centigrade
    @param  heaterTime Time to keep heater on in milliseconds
    @return True on success, False on failure
*/
/**************************************************************************/
bool Seeed_BME680::setGasHeater(uint16_t heaterTemp, uint16_t heaterTime)
{
  sensor_param.gas_sett.heatr_temp = heaterTemp;
  sensor_param.gas_sett.heatr_dur = heaterTime;
  if ( (heaterTemp == 0) || (heaterTime == 0) ) {
    // disabled!
    sensor_param.gas_sett.run_gas = BME680_DISABLE_GAS_MEAS;
    _gasEnabled = false;
  } else {
    sensor_param.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    _gasEnabled = true;
  }
  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Temperature oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Temperature reading),
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/

bool Seeed_BME680::setTemperatureOversampling(uint8_t oversample)
{
  if(oversample > BME680_OS_16X) return false;
  sensor_param.tph_sett.os_temp = oversample;
  if (oversample == BME680_OS_NONE)
    _tempEnabled = false;
  else
    _tempEnabled = true;
  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Humidity oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Humidity reading),
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/

bool Seeed_BME680::setHumidityOversampling(uint8_t oversample)
{
  if (oversample > BME680_OS_16X) return false;
  sensor_param.tph_sett.os_hum = oversample;
  if (oversample == BME680_OS_NONE)
    _humEnabled = false;
  else
    _humEnabled = true;
  return true;
}


/**************************************************************************/
/*!
    @brief  Setter for Pressure oversampling
    @param  oversample Oversampling setting, can be BME680_OS_NONE (turn off Pressure reading),
    BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
    @return True on success, False on failure
*/
/**************************************************************************/
bool Seeed_BME680::setPressureOversampling(uint8_t oversample)
{
  if (oversample > BME680_OS_16X) return false;
  sensor_param.tph_sett.os_pres = oversample;
  if (oversample == BME680_OS_NONE)
    _presEnabled = false;
  else
    _presEnabled = true;
  return true;
}

/**************************************************************************/
/*!
    @brief  Setter for IIR filter.
    @param filtersize Size of the filter (in samples). Can be
    BME680_FILTER_SIZE_0 (no filtering),
    BME680_FILTER_SIZE_1,
    BME680_FILTER_SIZE_3,
    BME680_FILTER_SIZE_7,
    BME680_FILTER_SIZE_15,
    BME680_FILTER_SIZE_31,
    BME680_FILTER_SIZE_63,
    BME680_FILTER_SIZE_127
    @return True on success, False on failure
*/
/**************************************************************************/
bool Seeed_BME680::setIIRFilterSize(uint8_t filtersize) {
  if (filtersize > BME680_FILTER_SIZE_127) return false;

  sensor_param.tph_sett.filter = filtersize;
  if (filtersize == BME680_FILTER_SIZE_0)
    _filterEnabled = false;
  else
    _filterEnabled = true;
  return true;
}


int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
unsigned int readcnt;

    // read data
    memset(reg_data,0x00,len);
    readcnt = bme_i2c.readBlock(reg_addr,reg_data,len);
    if(readcnt != len) return -2;
    return 0;
}

int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
int32_t i = 0;
bool retval;

    if(len == 1) {
        retval = bme_i2c.writeByte(reg_addr,reg_data[i]);
        if(retval) return 0;
        else return -2;
    } else if(len > 1) {
        retval = bme_i2c.writeBlock(reg_addr,reg_data,len);
        if(retval) return 0;
    }
    return -2;
}


static void delay_msec(unsigned int ms)
{
  usleep(ms*1000);
}

bool Seeed_BME680::init() 
{
	// User specifies IIC protocol in constructor.
    sensor_param.intf     = BME680_I2C_INTF;
    sensor_param.read     = &iic_read;
    sensor_param.write    = &iic_write;
    sensor_param.delay_ms = delay_msec;
    int8_t ret = BME680_OK;
    // Check the wiring,Check whether the protocol stack is normal and Read the calibrated value from sensor
    ret = bme680_init(&sensor_param);
    if(ret != 0)
    {
       cout << "bme680_init() ==> ret value = " << hex << ret << endl;
	   return false;
    }
    setTemperatureOversampling(BME680_OS_8X);
    setHumidityOversampling(BME680_OS_2X);
    setPressureOversampling(BME680_OS_4X);
    setIIRFilterSize(BME680_FILTER_SIZE_3);
    setGasHeater(300, 100); // 320*C for 150 ms
    // don't do anything till we request a reading
    sensor_param.power_mode = BME680_FORCED_MODE;

// Print the calibrated value of sensor.
#if 0
  cout << "T1 =  "  << hex << setw(6) << sensor_param.calib.par_t1  << endl;
  cout << "T2 =  "  << hex << setw(6) << sensor_param.calib.par_t2  << endl;
  cout << "T3 =  "  << hex << setw(6) << sensor_param.calib.par_t3  << endl;
  cout << "P1 =  "  << hex << setw(6) << sensor_param.calib.par_p1  << endl;
  cout << "P2 =  "  << hex << setw(6) << sensor_param.calib.par_p2  << endl;
  cout << "P3 =  "  << hex << setw(6) << sensor_param.calib.par_p3  << endl;
  cout << "P4 =  "  << hex << setw(6) << sensor_param.calib.par_p4  << endl;
  cout << "P5 =  "  << hex << setw(6) << sensor_param.calib.par_p5  << endl;
  cout << "P6 =  "  << hex << setw(6) << sensor_param.calib.par_p6  << endl;
  cout << "P7 =  "  << hex << setw(6) << sensor_param.calib.par_p7  << endl;
  cout << "P8 =  "  << hex << setw(6) << sensor_param.calib.par_p8  << endl;
  cout << "P9 =  "  << hex << setw(6) << sensor_param.calib.par_p9  << endl;
  cout << "P10 = "  << hex << setw(6) << sensor_param.calib.par_p10 << endl;
  cout << "H1 =  "  << hex << setw(6) << sensor_param.calib.par_h1  << endl;
  cout << "H2 =  "  << hex << setw(6) << sensor_param.calib.par_h2  << endl;
  cout << "H3 =  "  << hex << setw(6) << sensor_param.calib.par_h3  << endl;
  cout << "H4 =  "  << hex << setw(6) << sensor_param.calib.par_h4  << endl;
  cout << "H5 =  "  << hex << setw(6) << sensor_param.calib.par_h5  << endl;
  cout << "H6 =  "  << hex << setw(6) << sensor_param.calib.par_h6  << endl;
  cout << "H7 =  "  << hex << setw(6) << sensor_param.calib.par_h7  << endl;
  cout << "G1 =  "  << hex << setw(6) << sensor_param.calib.par_gh1 << endl;
  cout << "G2 =  "  << hex << setw(6) << sensor_param.calib.par_gh2 << endl;
  cout << "G3 =  "  << hex << setw(6) << sensor_param.calib.par_gh3 << endl;
  cout << "G1 =  "  << hex << setw(6) << sensor_param.calib.par_gh1 << endl;
  cout << "G2 =  "  << hex << setw(6) << sensor_param.calib.par_gh2 << endl;
  cout << "G3 =  "  << hex << setw(6) << sensor_param.calib.par_gh3 << endl;
  cout << "Heat Range = " << hex << setw(6) << sensor_param.calib.res_heat_range << endl;
  cout << "Heat Val   = " << hex << setw(6) << sensor_param.calib.res_heat_val   << endl;
  cout << "SW Error   = " << hex << setw(6) << sensor_param.calib.range_sw_err   << endl;
#endif

  return true;
}



