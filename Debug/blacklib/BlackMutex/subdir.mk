################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackMutex/BlackMutex.cpp 

OBJS += \
./blacklib/BlackMutex/BlackMutex.o 

CPP_DEPS += \
./blacklib/BlackMutex/BlackMutex.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackMutex/%.o: ../blacklib/BlackMutex/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


