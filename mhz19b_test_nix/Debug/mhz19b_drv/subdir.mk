################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mhz19b_drv/mhz19b.c \
../mhz19b_drv/mhz19b_port_nix.c 

OBJS += \
./mhz19b_drv/mhz19b.o \
./mhz19b_drv/mhz19b_port_nix.o 

C_DEPS += \
./mhz19b_drv/mhz19b.d \
./mhz19b_drv/mhz19b_port_nix.d 


# Each subdirectory must supply rules for building sources it contributes
mhz19b_drv/%.o: ../mhz19b_drv/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


