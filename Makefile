
# STM32f103c8t6  STM32F10X_MD
SPEC_TOOLCHAIN := 1

ifdef SPEC_TOOLCHAIN
AS = /home/zx/software/xilinx_2019_1_vivado/SDK/2019.1/gnu/aarch32/lin/gcc-arm-none-eabi/bin/arm-none-eabi-as
CC = /home/zx/software/xilinx_2019_1_vivado/SDK/2019.1/gnu/aarch32/lin/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
OBJCOPY = /home/zx/software/xilinx_2019_1_vivado/SDK/2019.1/gnu/aarch32/lin/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy
else
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
endif

$(info AS = $(AS))
$(info CC = $(CC))
$(info OBJCOPY = $(OBJCOPY))

PUB_CFLAGS = -g -specs=nano.specs -flto -Wl,--gc-sections -ffunction-sections -fdata-sections \
 -mthumb -mcpu=cortex-m3

ASFLAGS = 

# -g(debug)                           : 既是编译参数, 也是链接参数 | 用于在编译时生成调试信息, 既是编译参数, 也是链接参数
# -flto(Link Time Optimization)       : 既是编译参数, 也是链接参数 | 确定某个函数或变量从未被使用。它可以安全地将整个函数/变量从最终二进制文件中彻底删除
# -mthumb                             : 编译参数 | 指定编译器生成 Thumb 指令集的代码，而不是 ARM 指令集
# -ffunction-sections -fdata-sections : 编译参数 | 每个函数独立的段 每个数据变量独立的段
# -mcpu=cortex-m3 !!!                 : 不给链接器的话造成除法计算卡死
PUB_CFLAGS = -g -flto -mthumb -ffunction-sections -fdata-sections -mcpu=cortex-m3

# -Wall                               : 编译参数 | 启用所有常见的警告信息
# -Wextra                             : 编译参数 | 启用额外的、比 -Wall 更详细的警告信息
# -std=c11                            : 编译参数 | 用于指定编译器遵循 C11 标准进行编译
# -ffreestanding                      : 编译参数 | 告诉编译器代码将在独立环境中运行（没有操作系统的裸机环境）
# -fno-builtin                        : 编译参数 | 禁用 GCC 的内建函数优化
# -mcpu=cortex-m3                     : 编译参数 | 指定目标处理器为 ARM Cortex-M3 内核
CFLAGS = -Wall -Wextra -std=c11 -ffreestanding -fno-builtin -mcpu=cortex-m3
CFLAGS += -DSTM32F10X_MD \
		-DUSE_STDPERIPH_DRIVER

CFLAGS += \
	-I./user \
	-I./core \
	-I./system/sys \
	-I./system/delay \
	-I./system/usart \
	-I./system/sys \
	-I./system/sys \
	-I./STM32F10x_FWLib/inc

# -Wl,--gc-sections(Garbage Collect Sections) : 链接参数 | 用于在链接阶段删除未使用的代码段和数据段
# -Wl,--build-id=none                         : 链接参数 | 禁用生成 build-id（构建标识符）
# -lm 
# -specs=nano.specs :
# -nostdlib                                   : 链接参数 | 告诉编译器在链接时不使用标准系统库和启动文件
# -nostartfiles                               : 链接参数 | 告诉链接器不要使用标准的启动文件 , 但仍然链接标准库
LDFLAGS = -Wl,--gc-sections -Wl,--build-id=none -lm -nostdlib -nostartfiles 
LDFLAGS += -Tstm32_flash.ld

OBJCOPY_FLAGS = \
	-j .isr_vector \
	-j .text \
	-j .data \
	-j .rodata \
	-j .init_array \
	-j .fini_array \
	-R .note.gnu.build-i --remove-section=.note.*

TARGET = int_test

ASM_SOURCES = ./core/startup_stm32f10x_md.s

C_SOURCES = user/main.c \
	user/system_stm32f10x.c \
	system/delay/delay.c \
	system/usart/usart.c \
	STM32F10x_FWLib/src/stm32f10x_usart.c \
	STM32F10x_FWLib/src/misc.c \
	STM32F10x_FWLib/src/stm32f10x_gpio.c \
	STM32F10x_FWLib/src/stm32f10x_tim.c \
	STM32F10x_FWLib/src/stm32f10x_exti.c \
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
