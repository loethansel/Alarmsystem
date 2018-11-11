################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackPWM/BlackPWM.cpp 

OBJS += \
./blacklib/BlackPWM/BlackPWM.o 

CPP_DEPS += \
./blacklib/BlackPWM/BlackPWM.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackPWM/%.o: ../blacklib/BlackPWM/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


