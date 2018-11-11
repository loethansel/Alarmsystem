################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../blacklib/BlackDirectory/BlackDirectory.cpp 

OBJS += \
./blacklib/BlackDirectory/BlackDirectory.o 

CPP_DEPS += \
./blacklib/BlackDirectory/BlackDirectory.d 


# Each subdirectory must supply rules for building sources it contributes
blacklib/BlackDirectory/%.o: ../blacklib/BlackDirectory/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


