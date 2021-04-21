################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/8033/M5310/shuncom_project_BC28/kl17/board/board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/delay-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/flash-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/gpio-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/rtc-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/spi-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/uart-board.c \
D:/8033/M5310/shuncom_project_BC28/kl17/board/utilities.c 

OBJS += \
./board/board.o \
./board/delay-board.o \
./board/flash-board.o \
./board/gpio-board.o \
./board/rtc-board.o \
./board/spi-board.o \
./board/uart-board.o \
./board/utilities.o 

C_DEPS += \
./board/board.d \
./board/delay-board.d \
./board/flash-board.d \
./board/gpio-board.d \
./board/rtc-board.d \
./board/spi-board.d \
./board/uart-board.d \
./board/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
board/board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/delay-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/delay-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/flash-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/flash-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/gpio-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/gpio-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/rtc-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/rtc-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/spi-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/spi-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/uart-board.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/uart-board.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

board/utilities.o: D:/8033/M5310/shuncom_project_BC28/kl17/board/utilities.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DCPU_MKL17Z32VFM4 -DUSING_NCN8025_INTERFACE=1 -DFREEDOM -DDISABLE_WDOG=0 -I../../platform/osa/inc -I../../platform/utilities/inc -I../../platform/CMSIS/Include -I../../platform/devices -I../../platform/devices/MKL17Z644/include -I../../platform/devices/MKL17Z644/startup -I../../platform/hal/inc -I../../platform/drivers/inc -I../../platform/system/inc -I../../ -I../../board -I../../platform/drivers/src/adc16 -I../../platform/drivers/src/cmp -I../../platform/drivers/src/crc -I../../platform/drivers/src/dma -I../../platform/drivers/src/flexio -I../../platform/drivers/src/gpio -I../../platform/drivers/src/i2c -I../../platform/drivers/src/lptmr -I../../platform/drivers/src/lpuart -I../../platform/drivers/src/pit -I../../platform/drivers/src/rtc -I../../platform/drivers/src/spi -I../../platform/drivers/src/tpm -I../../platform/drivers/src/uart -I../../platform/drivers/src/vref -I../../platform/drivers/src/cop -I../../platform/drivers/src/smartcard -I../../platform/drivers/src/smartcard/interface -I../../platform/drivers/inc/flash -I../../Components/boards -I../../Components/osal -I../../Components/hal -I../../Components/system -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


