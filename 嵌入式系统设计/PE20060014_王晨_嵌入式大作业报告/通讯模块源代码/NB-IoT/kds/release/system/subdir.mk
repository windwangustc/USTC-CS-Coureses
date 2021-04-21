################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/ZGlobals.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/adc.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/delay.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/eeprom.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/fifo.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/gpio.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/i2c.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/timer.c \
D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/uart.c 

OBJS += \
./system/ZGlobals.o \
./system/adc.o \
./system/delay.o \
./system/eeprom.o \
./system/fifo.o \
./system/gpio.o \
./system/i2c.o \
./system/timer.o \
./system/uart.o 

C_DEPS += \
./system/ZGlobals.d \
./system/adc.d \
./system/delay.d \
./system/eeprom.d \
./system/fifo.d \
./system/gpio.d \
./system/i2c.d \
./system/timer.d \
./system/uart.d 


# Each subdirectory must supply rules for building sources it contributes
system/ZGlobals.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/ZGlobals.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I"D:\8033\M5310\shuncom_project_0417\kl17\Components\ATProcessingFunc" -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"system/ZGlobals.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/adc.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/delay.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/delay.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/eeprom.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/eeprom.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/fifo.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/fifo.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/gpio.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/i2c.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/timer.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/uart.o: D:/8033/M5310/shuncom_project_BC28/kl17/Components/system/uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../board -I../../platform/drivers/inc/flash -I../../Components/osal -I../../Components/hal -I../../Components/system -I../../Components/boards -I../../Components/CRC -I../../Components/ATProcessingFunc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


