################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashCommandSequence.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashEraseSector.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashInit.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashProgram.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashProgramCheck.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashReadResource.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashVerifySection.c \
D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/fsl_flash_lpm_callback.c 

OBJS += \
./platform/flash/drivers/FlashCommandSequence.o \
./platform/flash/drivers/FlashEraseSector.o \
./platform/flash/drivers/FlashInit.o \
./platform/flash/drivers/FlashProgram.o \
./platform/flash/drivers/FlashProgramCheck.o \
./platform/flash/drivers/FlashReadResource.o \
./platform/flash/drivers/FlashVerifySection.o \
./platform/flash/drivers/fsl_flash_lpm_callback.o 

C_DEPS += \
./platform/flash/drivers/FlashCommandSequence.d \
./platform/flash/drivers/FlashEraseSector.d \
./platform/flash/drivers/FlashInit.d \
./platform/flash/drivers/FlashProgram.d \
./platform/flash/drivers/FlashProgramCheck.d \
./platform/flash/drivers/FlashReadResource.d \
./platform/flash/drivers/FlashVerifySection.d \
./platform/flash/drivers/fsl_flash_lpm_callback.d 


# Each subdirectory must supply rules for building sources it contributes
platform/flash/drivers/FlashCommandSequence.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashCommandSequence.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashEraseSector.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashEraseSector.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashInit.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashInit.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashProgram.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashProgram.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashProgramCheck.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashProgramCheck.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashReadResource.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashReadResource.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/FlashVerifySection.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/FlashVerifySection.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

platform/flash/drivers/fsl_flash_lpm_callback.o: D:/shuncom/SzCodeSvn/shuncom_project/kl17/platform/drivers/src/flash/fsl_flash_lpm_callback.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DDEBUG -DCPU_MKL17Z32VFM4 -I../../../../../platform/CMSIS/Include -I../../../../../platform/devices -I../../../../../platform/devices/MKL17Z644/include -I../../../../../platform/devices/MKL17Z644/startup -I../../../../../platform/utilities/inc -I../../../../../platform/hal/inc -I../../../../../platform/drivers/inc -I../../../../../platform/system/inc -I../../../../../platform/osa/inc -std=gnu99 -fno-common  -ffreestanding  -fno-builtin  -mapcs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


