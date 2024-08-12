################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/App/appLcdSpeedTest.c \
../Core/Src/App/beer_60x100_16.c \
../Core/Src/App/testbitmap.c 

OBJS += \
./Core/Src/App/appLcdSpeedTest.o \
./Core/Src/App/beer_60x100_16.o \
./Core/Src/App/testbitmap.o 

C_DEPS += \
./Core/Src/App/appLcdSpeedTest.d \
./Core/Src/App/beer_60x100_16.d \
./Core/Src/App/testbitmap.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/App/%.o Core/Src/App/%.su Core/Src/App/%.cyclo: ../Core/Src/App/%.c Core/Src/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32Projects/Lora_Remote_BOARD_V1/Core/Src/Lcd" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-App

clean-Core-2f-Src-2f-App:
	-$(RM) ./Core/Src/App/appLcdSpeedTest.cyclo ./Core/Src/App/appLcdSpeedTest.d ./Core/Src/App/appLcdSpeedTest.o ./Core/Src/App/appLcdSpeedTest.su ./Core/Src/App/beer_60x100_16.cyclo ./Core/Src/App/beer_60x100_16.d ./Core/Src/App/beer_60x100_16.o ./Core/Src/App/beer_60x100_16.su ./Core/Src/App/testbitmap.cyclo ./Core/Src/App/testbitmap.d ./Core/Src/App/testbitmap.o ./Core/Src/App/testbitmap.su

.PHONY: clean-Core-2f-Src-2f-App

