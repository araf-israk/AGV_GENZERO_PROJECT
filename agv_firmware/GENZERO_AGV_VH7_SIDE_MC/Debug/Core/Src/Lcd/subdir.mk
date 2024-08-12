################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Lcd/ili9486.c \
../Core/Src/Lcd/lcd_io_gpio8.c \
../Core/Src/Lcd/stm32_adafruit_lcd.c 

OBJS += \
./Core/Src/Lcd/ili9486.o \
./Core/Src/Lcd/lcd_io_gpio8.o \
./Core/Src/Lcd/stm32_adafruit_lcd.o 

C_DEPS += \
./Core/Src/Lcd/ili9486.d \
./Core/Src/Lcd/lcd_io_gpio8.d \
./Core/Src/Lcd/stm32_adafruit_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Lcd/%.o Core/Src/Lcd/%.su Core/Src/Lcd/%.cyclo: ../Core/Src/Lcd/%.c Core/Src/Lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32CUBEIDE/PROJECTS/GENZERO_AGV_VH7_SIDE_MC/Core/Src/Lcd" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Lcd

clean-Core-2f-Src-2f-Lcd:
	-$(RM) ./Core/Src/Lcd/ili9486.cyclo ./Core/Src/Lcd/ili9486.d ./Core/Src/Lcd/ili9486.o ./Core/Src/Lcd/ili9486.su ./Core/Src/Lcd/lcd_io_gpio8.cyclo ./Core/Src/Lcd/lcd_io_gpio8.d ./Core/Src/Lcd/lcd_io_gpio8.o ./Core/Src/Lcd/lcd_io_gpio8.su ./Core/Src/Lcd/stm32_adafruit_lcd.cyclo ./Core/Src/Lcd/stm32_adafruit_lcd.d ./Core/Src/Lcd/stm32_adafruit_lcd.o ./Core/Src/Lcd/stm32_adafruit_lcd.su

.PHONY: clean-Core-2f-Src-2f-Lcd

