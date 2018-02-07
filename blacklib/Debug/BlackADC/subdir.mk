################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlackADC/BlackADC.cpp 

OBJS += \
./BlackADC/BlackADC.o 

CPP_DEPS += \
./BlackADC/BlackADC.d 


# Each subdirectory must supply rules for building sources it contributes
BlackADC/%.o: ../BlackADC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


