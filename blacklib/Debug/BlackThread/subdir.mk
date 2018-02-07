################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlackThread/BlackThread.cpp 

OBJS += \
./BlackThread/BlackThread.o 

CPP_DEPS += \
./BlackThread/BlackThread.d 


# Each subdirectory must supply rules for building sources it contributes
BlackThread/%.o: ../BlackThread/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


