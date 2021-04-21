################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_common.c \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_dma_shared_function.c \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_shared_function.c 

OBJS += \
./platform/spi/drivers/fsl_spi_common.o \
./platform/spi/drivers/fsl_spi_dma_shared_function.o \
./platform/spi/drivers/fsl_spi_shared_function.o 

C_DEPS += \
./platform/spi/drivers/fsl_spi_common.d \
./platform/spi/drivers/fsl_spi_dma_shared_function.d \
./platform/spi/drivers/fsl_spi_shared_function.d 


# Each subdirectory must supply rules for building sources it contributes
platform/spi/drivers/fsl_spi_common.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/spi/drivers/fsl_spi_dma_shared_function.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_dma_shared_function.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/spi/drivers/fsl_spi_shared_function.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/drivers/src/spi/fsl_spi_shared_function.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


