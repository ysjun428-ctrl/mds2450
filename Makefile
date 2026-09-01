CROSS ?= arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
OBJDUMP := $(CROSS)objdump
SIZE    := $(CROSS)size

CASE ?= 2
OPT  ?= O2
PCLK_HZ ?= 66000000
UART_REINIT ?= 0
UART_CHANNEL ?= 1

BUILD := build/case$(CASE)_$(OPT)
OUT   := output/MDS2450_case$(CASE)_$(OPT)

CPPFLAGS := -Iinclude -DMMU_CASE=$(CASE) -DPCLK_HZ=$(PCLK_HZ) \
            -DUART_REINIT=$(UART_REINIT) -DUART_CHANNEL=$(UART_CHANNEL)
CPUFLAGS := -mcpu=arm926ej-s -march=armv5te -marm -msoft-float
CFLAGS   := $(CPUFLAGS) -$(OPT) -g3 -std=c11 -ffreestanding -fno-builtin -fno-common -fno-pic \
            -fdata-sections -ffunction-sections -Wall -Wextra -Wshadow -Wconversion
ASFLAGS  := $(CPUFLAGS) -g3
LDFLAGS  := $(CPUFLAGS) -nostdlib -nostartfiles -T linker.ld \
            -Wl,--gc-sections -Wl,-Map,$(OUT).map

C_SRCS := $(wildcard src/*.c)
S_SRCS := $(wildcard src/*.S)
OBJS   := $(patsubst src/%.c,$(BUILD)/%.o,$(C_SRCS)) \
          $(patsubst src/%.S,$(BUILD)/%.o,$(S_SRCS))

.PHONY: all build clean case0 case1 case2 case3 cases opts info
all: build

build: $(OUT).bin $(OUT).lst
	@cp $(OUT).bin output/MDS2450.bin
	@echo
	@echo "Built: $(OUT).bin"
	@echo "Latest: output/MDS2450.bin"
	@$(SIZE) $(OUT).elf

$(BUILD)/%.o: src/%.c | $(BUILD) output
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.S | $(BUILD) output
	$(CC) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@

$(OUT).elf: $(OBJS) linker.ld | output
	$(CC) $(LDFLAGS) $(OBJS) -lgcc -o $@

$(OUT).bin: $(OUT).elf
	$(OBJCOPY) -O binary $< $@

$(OUT).lst: $(OUT).elf
	$(OBJDUMP) -d -S $< > $@

$(BUILD) output:
	mkdir -p $@

case0:
	$(MAKE) CASE=0 OPT=$(OPT) build
case1:
	$(MAKE) CASE=1 OPT=$(OPT) build
case2:
	$(MAKE) CASE=2 OPT=$(OPT) build
case3:
	$(MAKE) CASE=3 OPT=$(OPT) build

cases:
	$(MAKE) CASE=0 OPT=$(OPT) build
	$(MAKE) CASE=1 OPT=$(OPT) build
	$(MAKE) CASE=2 OPT=$(OPT) build
	$(MAKE) CASE=3 OPT=$(OPT) build

opts:
	$(MAKE) CASE=$(CASE) OPT=O0 build
	$(MAKE) CASE=$(CASE) OPT=O1 build
	$(MAKE) CASE=$(CASE) OPT=O2 build
	$(MAKE) CASE=$(CASE) OPT=O3 build
	$(MAKE) CASE=$(CASE) OPT=Os build

info:
	@echo "CASE=$(CASE) OPT=$(OPT) PCLK_HZ=$(PCLK_HZ) UART_REINIT=$(UART_REINIT) UART_CHANNEL=$(UART_CHANNEL)"
	@echo "CC=$(CC)"
	@$(CC) --version | head -1

clean:
	rm -rf build output/*.elf output/*.bin output/*.map output/*.lst
