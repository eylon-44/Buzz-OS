## bzlibc Makefile ## ~ eylon

## Setting ##
CC	   := gcc
CFLAGS := -I$(INC_DIR)/libc -m32 -nostdlib -nostdinc -fno-builtin -fno-pic -static -ffreestanding -no-pie -Wall -Wextra -Werror

AS      := nasm
ASFLAGS := -f elf32 -g -F dwarf

LIB_BIN_DIR := $(BIN_DIR)/libc
LIB_BIN := $(LIB_BIN_DIR)/lib_bzlibc.a

LIB_SRCS := $(shell find . -name '*.asm' -or -name '*.c')
LIB_OBJS := $(patsubst %, $(LIB_BIN_DIR)/%.o, $(LIB_SRCS))

## Compile ##
all: $(LIB_BIN)

# Build C objects
$(LIB_BIN_DIR)/%.c.o: %.c
	@mkdir -p $(shell dirname $@)
	${CC} ${CFLAGS} -c -o $@ $<
	
# Build Assembly objects
$(LIB_BIN_DIR)/%.asm.o: %.asm
	@mkdir -p $(shell dirname $@)
	${AS} ${ASFLAGS} -o $@ $<

# Link and archive
$(LIB_BIN): $(LIB_OBJS)
	@mkdir -p $(shell dirname $@)
	ar rcs $@ $^
	ranlib $@