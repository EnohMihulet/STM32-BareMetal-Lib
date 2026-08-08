CPU		= -mcpu=cortex-m4
FPU		= -mfpu=fpv4-sp-d16
FLOAT_ABI	= -mfloat-abi=hard
THUMB		= -mthumb

MCU = $(CPU) $(FPU) $(FLOAT_ABI) $(THUMB)

CC	= arm-none-eabi-gcc
AS	= arm-none-eabi-as
OBJCOPY	= arm-none-eabi-objcopy
SIZE	= arm-none-eabi-size

TARGET = firmware

C_SOURCES	= $(wildcard examples/nucleo_shell/*.c examples/nucleo_shell/app/*.c drivers/src/*.c shell/shell.c)
ASM_SOURCES	= startup/mcl_startup.s
LD_SCRIPT	= linker/mcl_stm32f446re.ld
BUILD_DIR	= build

OBJECTS	= $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
OBJECTS	+= $(ASM_SOURCES:%.s=$(BUILD_DIR)/%.o)
DEPFILES = $(OBJECTS:.o=.d)

C_FLAGS	 = $(MCU) -Icore -Idrivers/inc -Ishell -Iexamples/nucleo_shell -Iexamples/nucleo_shell/app -Wall -Wextra -O0 -g -MMD -MP
LD_FLAGS = $(MCU) -T$(LD_SCRIPT) -Wl,-Map=$(TARGET).map -Wl,--gc-sections -nostdlib
LD_LIBS  = -lgcc

.PHONY: all elf bin hex clean
	
all: $(TARGET).elf $(TARGET).bin $(TARGET).hex

$(BUILD_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) $(MCU) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@

elf: $(TARGET).elf

$(TARGET).elf: $(OBJECTS) $(LD_SCRIPT)
	$(CC) $(OBJECTS) $(LD_FLAGS) $(LD_LIBS) -o $@

bin: $(TARGET).elf $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

hex: $(TARGET).elf $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

clean: 
	rm -rf $(BUILD_DIR) *.elf *.bin *.hex *.map

-include $(DEPFILES)
