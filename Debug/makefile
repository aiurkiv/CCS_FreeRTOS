################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ti-cgt-arm_20.2.7.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./startup_ccs.obj" \
"./src/apps/app/app.obj" \
"./src/apps/app_led_blink/app_led_blin.obj" \
"./src/drivers/pinout/pinout.obj" \
"./src/drivers/uart/drv_uart.obj" \
"./src/drivers/uart/uartstdio.obj" \
"./src/initialization.obj" \
"./src/tasks_work.obj" \
"./src/services/adc/adc_service.obj" \
"./src/services/buttons/buttons.obj" \
"./src/third_party/FreeRTOS/Source/croutine.obj" \
"./src/third_party/FreeRTOS/Source/event_groups.obj" \
"./src/third_party/FreeRTOS/Source/list.obj" \
"./src/third_party/FreeRTOS/Source/queue.obj" \
"./src/third_party/FreeRTOS/Source/tasks.obj" \
"./src/third_party/FreeRTOS/Source/timers.obj" \
"./src/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/port.obj" \
"./src/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/portasm.obj" \
"./src/third_party/FreeRTOS/Source/portable/MemMang/heap_4.obj" \
"../main_ccs.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:/ti/TivaWare_C_Series-2.2.0.295/driverlib/ccs/Debug/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/apps/app/subdir_vars.mk
-include src/apps/app_led_blink/subdir_vars.mk
-include src/drivers/pinout/subdir_vars.mk
-include src/drivers/uart/subdir_vars.mk
-include src/subdir_vars.mk
-include src/services/adc/subdir_vars.mk
-include src/services/buttons/subdir_vars.mk
-include src/third_party/FreeRTOS/Source/subdir_vars.mk
-include src/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/subdir_vars.mk
-include src/third_party/FreeRTOS/Source/portable/MemMang/subdir_vars.mk
-include subdir_rules.mk
-include src/apps/app/subdir_rules.mk
-include src/apps/app_led_blink/subdir_rules.mk
-include src/drivers/pinout/subdir_rules.mk
-include src/drivers/uart/subdir_rules.mk
-include src/subdir_rules.mk
-include src/services/adc/subdir_rules.mk
-include src/services/buttons/subdir_rules.mk
-include src/third_party/FreeRTOS/Source/subdir_rules.mk
-include src/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/subdir_rules.mk
-include src/third_party/FreeRTOS/Source/portable/MemMang/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
freertos.out 

EXE_OUTPUTS__QUOTED += \
"freertos.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "freertos.out"

# Tool invocations
freertos.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=TARGET_IS_TM4C129_RA2 -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual -z -m"hello_ccs.map" --heap_size=0 --stack_size=512 -i"C:/ti/ti-cgt-arm_20.2.7.LTS/lib" -i"C:/ti/ti-cgt-arm_20.2.7.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="freertos_linkInfo.xml" --rom_model -o "freertos.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "startup_ccs.obj" "src\apps\app\app.obj" "src\apps\app_led_blink\app_led_blin.obj" "src\drivers\pinout\pinout.obj" "src\drivers\uart\drv_uart.obj" "src\drivers\uart\uartstdio.obj" "src\initialization.obj" "src\tasks_work.obj" "src\services\adc\adc_service.obj" "src\services\buttons\buttons.obj" "src\third_party\FreeRTOS\Source\croutine.obj" "src\third_party\FreeRTOS\Source\event_groups.obj" "src\third_party\FreeRTOS\Source\list.obj" "src\third_party\FreeRTOS\Source\queue.obj" "src\third_party\FreeRTOS\Source\tasks.obj" "src\third_party\FreeRTOS\Source\timers.obj" "src\third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\port.obj" "src\third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\portasm.obj" "src\third_party\FreeRTOS\Source\portable\MemMang\heap_4.obj" 
	-$(RM) "main.d" "startup_ccs.d" "src\apps\app\app.d" "src\apps\app_led_blink\app_led_blin.d" "src\drivers\pinout\pinout.d" "src\drivers\uart\drv_uart.d" "src\drivers\uart\uartstdio.d" "src\initialization.d" "src\tasks_work.d" "src\services\adc\adc_service.d" "src\services\buttons\buttons.d" "src\third_party\FreeRTOS\Source\croutine.d" "src\third_party\FreeRTOS\Source\event_groups.d" "src\third_party\FreeRTOS\Source\list.d" "src\third_party\FreeRTOS\Source\queue.d" "src\third_party\FreeRTOS\Source\tasks.d" "src\third_party\FreeRTOS\Source\timers.d" "src\third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\port.d" "src\third_party\FreeRTOS\Source\portable\MemMang\heap_4.d" 
	-$(RM) "src\third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\portasm.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccs2041/ccs//utils/tiobj2bin/tiobj2bin" "freertos.out" "freertos.bin" "C:/ti/ti-cgt-arm_20.2.7.LTS/bin/armofd" "C:/ti/ti-cgt-arm_20.2.7.LTS/bin/armhex" "C:/ti/ccs2041/ccs//utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

