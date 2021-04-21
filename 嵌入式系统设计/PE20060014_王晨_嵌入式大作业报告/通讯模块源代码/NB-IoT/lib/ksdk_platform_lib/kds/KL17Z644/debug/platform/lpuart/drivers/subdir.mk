################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_common.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_dma_driver.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_driver.c 

OBJS += \
./platform/lpuart/drivers/fsl_lpuart_common.o \
./platform/lpuart/drivers/fsl_lpuart_dma_driver.o \
./platform/lpuart/drivers/fsl_lpuart_driver.o 

C_DEPS += \
./platform/lpuart/drivers/fsl_lpuart_common.d \
./platform/lpuart/drivers/fsl_lpuart_dma_driver.d \
./platform/lpuart/drivers/fsl_lpuart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
platform/lpuart/drivers/fsl_lpuart_common.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/lpuart/drivers/fsl_lpuart_dma_driver.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_dma_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/lpuart/drivers/fsl_lpuart_driver.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/lpuart/fsl_lpuart_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


