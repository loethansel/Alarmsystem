################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlackI2C/BlackI2C.cpp 

OBJS += \
./BlackI2C/BlackI2C.o 

CPP_DEPS += \
./BlackI2C/BlackI2C.d 


# Each subdirectory must supply rules for building sources it contributes
BlackI2C/%.o: ../BlackI2C/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


