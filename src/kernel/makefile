## Kernel Makefile ## ~ eylon

BIN_DIR := ../../bin/kernel

KRNL_SRCS := $(shell find -name '*.asm')
KRNL_OBJS := $(patsubst %.asm, $(BIN_DIR)/%.o, $(KRNL_SRCS))

all: $(KRNL_OBJS)

$(BIN_DIR)/%.o: %.asm
	mkdir -p ${shell dirname $@}
	nasm -f bin -o $@ $<
