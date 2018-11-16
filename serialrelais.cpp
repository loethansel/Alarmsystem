/*
 * serialrelais.cpp
 *
 *  Created on: 13.11.2018
 *      Author: linux
 */

#include "serialrelais.h"

using namespace std;
using namespace BlackLib;


/*
 * multi_channel_relay.h
 * Seeed multi channel relay Arduino library
 *
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 * Author        :   lambor
 * Create Time   :   June 2018
 * Change Log    :
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

serialrelais::serialrelais() : BlackI2C(BlackLib::I2C_2, 0x11)
{
    channel_state = 0;
    bool isOpened = open( BlackLib::ReadWrite | BlackLib::NonBlock );
    if( !isOpened )
    {
        std::cout << "I2C DEVICE CAN\'T OPEN.;" << std::endl;
    }
}


uint8_t serialrelais::getFirmwareVersion(void)
{
uint8_t firmwareversion;

   writeByte(CMD_READ_FIRMWARE_VER,0x01);
   usleep(1000);
   firmwareversion = readByte(CMD_READ_FIRMWARE_VER);
   return firmwareversion;
}

void serialrelais::changeI2CAddress(uint8_t new_addr)
{
    writeByte(CMD_SAVE_I2C_ADDR,new_addr);
    setDeviceAddress(new_addr);
}

uint8_t serialrelais::getChannelState(void)
{
    return channel_state;
}

void serialrelais::channelCtrl(uint8_t state)
{
  channel_state = state;
  writeByte(CMD_CHANNEL_CTRL,channel_state);
}

void serialrelais::turn_on_channel(uint8_t channel)
{
  channel_state |= (1 << (channel-1));
  writeByte(CMD_CHANNEL_CTRL,channel_state);
}

void serialrelais::turn_off_channel(uint8_t channel)
{
  channel_state &= ~(1 << (channel-1));
  writeByte(CMD_CHANNEL_CTRL,channel_state);
}

uint8_t serialrelais::scanI2CDevice(void)
{
  uint8_t error = 0, address = 0, result = 0;
  int nDevices;

  // DEBUG_PRINT.println("Scanning...");

  nDevices = 0;
  for(address = 1; address <= 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    //setDeviceAddress(address);
    //Wire.beginTransmission(address);
    //error = Wire.endTransmission();

    if (error == 0)
    {
      result = address;
      // DEBUG_PRINT.print("I2C device found at address 0x");
      //if (address<16) {; }
        //DEBUG_PRINT.print("0");
      //DEBUG_PRINT.print(address,HEX);
      //DEBUG_PRINT.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      //DEBUG_PRINT.print("Unknown error at address 0x");
     // if (address<16)
        //DEBUG_PRINT.print("0");
        //DEBUG_PRINT.println(address,HEX);
    }
  }
  if (nDevices == 0) {
    //DEBUG_PRINT.println("No I2C devices found\n");
    result = 0x00;
  }
  else {
    //DEBUG_PRINT.print("Found ");
    //DEBUG_PRINT.print(nDevices);
    //DEBUG_PRINT.print(" devices\n");
    if(nDevices != 1) {
      result = 0x00;
    }
  }
  return result;
}

serialrelais::~serialrelais() {
	// TODO Auto-generated destructor stub
}
