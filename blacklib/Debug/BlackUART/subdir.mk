################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlackUART/BlackUART.cpp 

OBJS += \
./BlackUART/BlackUART.o 

CPP_DEPS += \
./BlackUART/BlackUART.d 


# Each subdirectory must supply rules for building sources it contributes
BlackUART/%.o: ../BlackUART/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


