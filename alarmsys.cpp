 /*

 ####################################################################################
 #  BlackLib Library controls Beaglebone Black's inputs and outputs.                #
 #  Copyright (C) 2013-2015 by Yigit YUCE                                           #
 ####################################################################################
 #                                                                                  #
 #  This file is part of BlackLib library.                                          #
 #                                                                                  #
 #  BlackLib library is free software: you can redistribute it and/or modify        #
 #  it under the terms of the GNU Lesser General Public License as published by     #
 #  the Free Software Foundation, either version 3 of the License, or               #
 #  (at your option) any later version.                                             #
 #                                                                                  #
 #  BlackLib library is distributed in the hope that it will be useful,             #
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of                  #
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   #
 #  GNU Lesser General Public License for more details.                             #
 #                                                                                  #
 #  You should have received a copy of the GNU Lesser General Public License        #
 #  along with this program.  If not, see <http://www.gnu.org/licenses/>.           #
 #                                                                                  #
 #  For any comment or suggestion please contact the creator of BlackLib Library    #
 #  at ygtyce@gmail.com                                                             #
 #                                                                                  #
 ####################################################################################

 */
#include "blacklib/BlackLib.h"
#include "blacklib/BlackUART/BlackUART.h"
#include <string>
#include <iostream>
#include <iomanip>
/*
#include "blacklib/examples/example_GPIO.h"
#include "blacklib/examples/example_ADC.h"
#include "blacklib/examples/example_PWM.h"
#include "blacklib/examples/example_SPI.h"
#include "blacklib/examples/example_UART.h"
#include "blacklib/examples/example_I2C.h"
#include "blacklib/examples/example_directory.h"
#include "blacklib/examples/example_threadAndMutex.h"
#include "blacklib/examples/example_time.h"
*/

void Init_UART()
{
    /*
     * This part of the example code is about BlackUART class, write/read buffer and write/read
     * string initialization.
     *
     * 1) BlackUART(enum uartName uart);
     * 2) BlackUART(enum uartName uart, struct BlackUartProperties uartProperties);
     * 3) BlackUART(enum uartName uart, enum baudRate uartBaud, enum parity uartParity,
     *                                  enum stopBits uartStopBits, enum characterSize uartCharSize);
     */

    BlackLib::BlackUART gsm( BlackLib::UART1,
                             BlackLib::Baud9600,
                             BlackLib::ParityEven,
                             BlackLib::StopOne,
                             BlackLib::Char8);


    bool isOpened = gsm.open( BlackLib::ReadWrite | BlackLib::NonBlock );

    if(!isOpened) {
        std::cout << "UART DEVICE CAN\'T OPEN" << std::endl;
        exit(1);
    }
    std::cout << std::endl;
    std::cout << "Device Path     : " << gsm.getPortName() << std::endl;
    std::cout << "Read Buf. Size  : " << gsm.getReadBufferSize() << std::endl;
    std::cout << "BaudRate In/Out : " << gsm.getBaudRate( BlackLib::input) << "/"
                                      << gsm.getBaudRate( BlackLib::output) << std::endl;
    std::cout << "Character Size  : " << gsm.getCharacterSize() << std::endl;
    std::cout << "Stop Bit Size   : " << gsm.getStopBits() << std::endl;
    std::cout << "Parity          : " << gsm.getParity() << std::endl << std::endl;
}



int main()
{

    Init_UART();


//    example_GPIO();
//    example_ADC();
//    example_PWM();
//    example_SPI();
//    example_UART();
//    example_I2C();
//    example_directory();
//    example_threadAndMutex();
//    example_time();


    return 0;
}
