################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS_Source/ARM_CM0/port.c 

OBJS += \
./FreeRTOS_Source/ARM_CM0/port.o 

C_DEPS += \
./FreeRTOS_Source/ARM_CM0/port.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS_Source/ARM_CM0/%.o: ../FreeRTOS_Source/ARM_CM0/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\BUJILU\workspace.kds\test\FreeRTOS_Source\include" -I"C:\Users\BUJILU\workspace.kds\test\FreeRTOS_Source\ARM_CM0" -I"../Sources" -I"../Includes" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


