################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../OTA/SIM7x00/sim7x00.c 

OBJS += \
./OTA/SIM7x00/sim7x00.o 

C_DEPS += \
./OTA/SIM7x00/sim7x00.d 


# Each subdirectory must supply rules for building sources it contributes
OTA/SIM7x00/%.o: ../OTA/SIM7x00/%.c OTA/SIM7x00/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L152xC -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/This PC/STM32CubeIDE/VER_100/OTAFIRMWARE/OTA/FLASH" -I"C:/Users/This PC/STM32CubeIDE/VER_100/OTAFIRMWARE/OTA/SIM7x00" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-OTA-2f-SIM7x00

clean-OTA-2f-SIM7x00:
	-$(RM) ./OTA/SIM7x00/sim7x00.d ./OTA/SIM7x00/sim7x00.o

.PHONY: clean-OTA-2f-SIM7x00

