/*
 * multi_channel_relay.cpp
 * Seeed multi channel relay Arduino library
 *
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 * Author        :   lambor
 * Create Time   :   June 2018
 * Change Log    :
 */
#ifndef SERIALRELAIS_H_
#define SERIALRELAIS_H_
// INCLUDES PROGRAM
#include "blacklib/BlackLib.h"
#include "blacklib/BlackI2C/BlackI2C.h"
// NAMESPACES
using namespace std;
using namespace BlackLib;
// DEFINES
#define CHANNLE1_BIT  0x01
#define CHANNLE2_BIT  0x02
#define CHANNLE3_BIT  0x04
#define CHANNLE4_BIT  0x08
#define CHANNLE5_BIT  0x10
#define CHANNLE6_BIT  0x20
#define CHANNLE7_BIT  0x40
#define CHANNLE8_BIT  0x80

#define CMD_CHANNEL_CTRL        0x10
#define CMD_SAVE_I2C_ADDR       0x11
#define CMD_READ_I2C_ADDR       0x12
#define CMD_READ_FIRMWARE_VER   0x13
// CLASSES
class serialrelais : public BlackI2C
{
    public:
        serialrelais(void);
        ~serialrelais(void);
        /**
         * @brief Change device address from old_addr to new_addr.
         * @param new_addr, the address to use.
                            old_addr, the original address
         * @return None
        */
        void changeI2CAddress(uint8_t new_addr);
        /**
         * /brief Get channel state
         * /return One byte value to indicate channel state
         *               the bits range from 0 to 7 represents channel 1 to 8
        */
        uint8_t getChannelState(void);
        /**
         * @brief Read firmware version from on board MCU
         * @param
         * @return Firmware version in byte
        */
        uint8_t getFirmwareVersion(void);
        /**
         * @brief Control relay channels
         * @param state, use one Byte to represent 8 channel
         * @return None
        */
        void channelCtrl(uint8_t state);
        /**
         * @brief Turn on one of 8 channels
         * @param channel, channel to control with (range form 1 to 8)
         * @return None
        */
        void turn_on_channel(uint8_t channel);
        /**
         * @brief Turn off on of 8 channels
         * @param channel, channel to control with (range form 1 to 8)
         * @return None
        */
        void turn_off_channel(uint8_t channel);
        /**
         * @brief Scan I2C device, return the address if there is only one i2c device
         * @param
         * @return device address
        */
        uint8_t scanI2CDevice(void);
    private:
        int channel_state;  // Value to save channel state
};

#endif /* SERIALRELAIS_H_ */
