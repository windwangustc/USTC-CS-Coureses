################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/MKL17Z644/fsl_clock_MKL17Z644.c \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/fsl_clock_manager.c \
D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/fsl_clock_manager_common.c 

OBJS += \
./platform/clock/system/fsl_clock_MKL17Z644.o \
./platform/clock/system/fsl_clock_manager.o \
./platform/clock/system/fsl_clock_manager_common.o 

C_DEPS += \
./platform/clock/system/fsl_clock_MKL17Z644.d \
./platform/clock/system/fsl_clock_manager.d \
./platform/clock/system/fsl_clock_manager_common.d 


# Each subdirectory must supply rules for building sources it contributes
platform/clock/system/fsl_clock_MKL17Z644.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/MKL17Z644/fsl_clock_MKL17Z644.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/clock/system/fsl_clock_manager.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/fsl_clock_manager.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/clock/system/fsl_clock_manager_common.o: D:/8033/M5310/shuncom_project_BC28/kl17/platform/system/src/clock/fsl_clock_manager_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -DNDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -I../../../../../platform/drivers/inc/flash -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


