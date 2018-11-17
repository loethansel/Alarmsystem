################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ain_proc.cpp \
../alarmsys.cpp \
../fona.cpp \
../gsm_proc.cpp \
../infile.cpp \
../ntpserver.cpp \
../rtc.cpp \
../serialrelais.cpp \
../xbee.cpp 

OBJS += \
./ain_proc.o \
./alarmsys.o \
./fona.o \
./gsm_proc.o \
./infile.o \
./ntpserver.o \
./rtc.o \
./serialrelais.o \
./xbee.o 

CPP_DEPS += \
./ain_proc.d \
./alarmsys.d \
./fona.d \
./gsm_proc.d \
./infile.d \
./ntpserver.d \
./rtc.d \
./serialrelais.d \
./xbee.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


