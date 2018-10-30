################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackADC/BlackADC.cpp 

OBJS += \
./blacklib/BlackADC/BlackADC.o 

CPP_DEPS += \
./blacklib/BlackADC/BlackADC.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackADC/%.o: ../blacklib/BlackADC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -Ipthread -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


