## Bootloader Makefile ## ~ eylon

BIN_DIR     := ../../bin/bootloader
BIN_OUT     := $(BIN_DIR)/bootsector.bin
BOOT_SCRIPT := bootsector.asm

# Find Assembly sources
BOOT_SRCS := $(shell find -name '*.asm')

all: $(BIN_OUT)

# Compile
$(BIN_OUT): $(BOOT_SRCS)
	@mkdir -p ${shell dirname $@}
	nasm -f bin -o $@ $(BOOT_SCRIPT)