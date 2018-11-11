################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackThread/BlackThread.cpp 

OBJS += \
./blacklib/BlackThread/BlackThread.o 

CPP_DEPS += \
./blacklib/BlackThread/BlackThread.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackThread/%.o: ../blacklib/BlackThread/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


