
# STM32f103c8t6  STM32F10X_MD
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

PUB_CFLAGS = -g -specs=nano.specs -flto -Wl,--gc-sections -ffunction-sections -fdata-sections \
 -mthumb -mcpu=cortex-m3

ASFLAGS = 

CFLAGS = -Wall -Wextra -g -std=c11 -mthumb
CFLAGS += -nostdlib -ffreestanding -fno-builtin

CFLAGS += -DSTM32F10X_MD \
		-DUSE_STDPERIPH_DRIVER \
		-mcpu=cortex-m3

CFLAGS += \
	-I./user \
	-I./core \
	-I./system/sys \
	-I./system/delay \
	-I./system/usart \
	-I./system/sys \
	-I./system/sys \
	-I./STM32F10x_FWLib/inc

LDFLAGS = -lm -nostdlib -nostartfiles -mthumb
LDFLAGS += -Tstm32_flash.ld
LDFLAGS += -Wl,--gc-sections

OBJCOPY_FLAGS = -j .isr_vector \
    -j .text \
    -j .data \
    -j .rodata \
    -j .init_array \
    -j .fini_array

TARGET = int_test

ASM_SOURCES = ./core/startup_stm32f10x_md.s

C_SOURCES = user/main.c \
	user/system_stm32f10x.c \
	system/delay/delay.c \
	system/usart/usart.c \
	STM32F10x_FWLib/src/stm32f10x_usart.c \
	STM32F10x_FWLib/src/misc.c \
	STM32F10x_FWLib/src/stm32f10x_gpio.c \
	STM32F10x_FWLib/src/stm32f10x_rcc.c

C_OBJS = $(C_SOURCES:.c=.o)
ASM_OBJS = $(ASM_SOURCES:.s=.o)
OBJS = $(C_OBJS) $(ASM_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(PUB_CFLAGS) $(LDFLAGS) $(OBJS) -o $@.elf 
	$(OBJCOPY) -O binary $(OBJCOPY_FLAGS) -S --strip-all  $@.elf  $@.bin
	@ls -al $@.elf  $@.bin

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(PUB_CFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin
.PHONY: rebuild
rebuild: clean all