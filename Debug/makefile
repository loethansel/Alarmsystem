################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xbee/subdir.mk
-include timer/subdir.mk
-include socketserver/subdir.mk
-include socketclient/subdir.mk
-include rtc/subdir.mk
-include relais/subdir.mk
-include logger/subdir.mk
-include gsm/subdir.mk
-include files/subdir.mk
-include email/subdir.mk
-include bme680/subdir.mk
-include blacklib/Release/BlackUART/subdir.mk
-include blacklib/Release/BlackTime/subdir.mk
-include blacklib/Release/BlackThread/subdir.mk
-include blacklib/Release/BlackSPI/subdir.mk
-include blacklib/Release/BlackPWM/subdir.mk
-include blacklib/Release/BlackMutex/subdir.mk
-include blacklib/Release/BlackI2C/subdir.mk
-include blacklib/Release/BlackGPIO/subdir.mk
-include blacklib/Release/BlackDirectory/subdir.mk
-include blacklib/Release/BlackADC/subdir.mk
-include blacklib/Release/subdir.mk
-include blacklib/BlackUART/subdir.mk
-include blacklib/BlackTime/subdir.mk
-include blacklib/BlackThread/subdir.mk
-include blacklib/BlackSPI/subdir.mk
-include blacklib/BlackPWM/subdir.mk
-include blacklib/BlackMutex/subdir.mk
-include blacklib/BlackI2C/subdir.mk
-include blacklib/BlackGPIO/subdir.mk
-include blacklib/BlackDirectory/subdir.mk
-include blacklib/BlackADC/subdir.mk
-include blacklib/subdir.mk
-include analog/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Alarmsystem

# Tool invocations
Alarmsystem: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-linux-gnueabihf-g++  -o "Alarmsystem" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(CLA_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(INO_DEPS)$(PDE_DEPS)$(K_DEPS)$(C_DEPS)$(CPP_DEPS) Alarmsystem
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
