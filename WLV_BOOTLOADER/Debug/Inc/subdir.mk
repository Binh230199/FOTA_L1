################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/l152re_eeprom.c \
../Inc/l152re_rcc.c \
../Inc/l152re_systick.c 

OBJS += \
./Inc/l152re_eeprom.o \
./Inc/l152re_rcc.o \
./Inc/l152re_systick.o 

C_DEPS += \
./Inc/l152re_eeprom.d \
./Inc/l152re_rcc.d \
./Inc/l152re_systick.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/%.o: ../Inc/%.c Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32L1 -DSTM32 -DSTM32L152RETx -DSTM32L152xE -c -I../Inc -I../CMSIS/Include -I../CMSIS/STM32L1xx/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Inc

clean-Inc:
	-$(RM) ./Inc/l152re_eeprom.d ./Inc/l152re_eeprom.o ./Inc/l152re_rcc.d ./Inc/l152re_rcc.o ./Inc/l152re_systick.d ./Inc/l152re_systick.o

.PHONY: clean-Inc

