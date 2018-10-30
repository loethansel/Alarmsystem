################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackTime/BlackTime.cpp 

OBJS += \
./blacklib/BlackTime/BlackTime.o 

CPP_DEPS += \
./blacklib/BlackTime/BlackTime.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackTime/%.o: ../blacklib/BlackTime/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -Ipthread -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


