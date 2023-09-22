## Bootloader Makefile ## ~ eylon

BIN_DIR := ../../bin/bootloader

BOOT_SRCS := $(shell find -name '*.asm')
BOOT_OBJS := $(patsubst %.asm, $(BIN_DIR)/%.o, $(BOOT_SRCS))

all: $(BOOT_OBJS)

$(BIN_DIR)/%.o: %.asm
	mkdir -p ${shell dirname $@}
	nasm -f bin -o $@ $<
