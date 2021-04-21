################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include system/crypto/subdir.mk
-include system/subdir.mk
-include startup/subdir.mk
-include sources/subdir.mk
-include osal/subdir.mk
-include hal/subdir.mk
-include board/subdir.mk
-include CRC/subdir.mk
-include ATProcessingFunc/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
shuncom_project_MKL17Z32VFM4.bin \


# All Target
all: shuncom_project_MKL17Z32VFM4.elf secondary-outputs

# Tool invocations
shuncom_project_MKL17Z32VFM4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -T ../../platform/devices/MKL17Z644/linker/gcc/MKL17Z32xxx4_flash.ld -Xlinker --gc-sections -Wl,-Map,"shuncom_project_MKL17Z32VFM4.map" -Xlinker -z  -Xlinker muldefs   --specs=nano.specs          -Wall  -fno-common  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin  -Os  -mapcs  -Xlinker -static -o "shuncom_project_MKL17Z32VFM4.elf" -Xlinker --start-group $(OBJS) $(USER_OBJS) $(LIBS) -Xlinker --end-group
	@echo 'Finished building target: $@'
	@echo ' '

shuncom_project_MKL17Z32VFM4.bin: shuncom_project_MKL17Z32VFM4.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "shuncom_project_MKL17Z32VFM4.elf"  "shuncom_project_MKL17Z32VFM4.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(S_UPPER_DEPS) shuncom_project_MKL17Z32VFM4.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
