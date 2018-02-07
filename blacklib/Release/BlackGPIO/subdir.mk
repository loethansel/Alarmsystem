################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlackGPIO/BlackGPIO.cpp 

OBJS += \
./BlackGPIO/BlackGPIO.o 

CPP_DEPS += \
./BlackGPIO/BlackGPIO.d 


# Each subdirectory must supply rules for building sources it contributes
BlackGPIO/%.o: ../BlackGPIO/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


