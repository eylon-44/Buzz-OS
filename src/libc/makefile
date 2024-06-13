## bzlibc Makefile ## ~ eylon

## Setting ##
CC	   ?= gcc
CFLAGS := -I$(INC_DIR)/libc -ggdb -m32 -nostdlib -nostdinc -fno-builtin -fno-pic -static -ffreestanding -no-pie -Wall -Wextra -Werror
AR    ?= ar
RANLIB ?= ranlib

LIB_BIN_DIR := $(BIN_DIR)/libc
LIB_BIN := $(LIB_BIN_DIR)/lib_bzlibc.a

LIB_SRCS := $(shell find . -name '*.S' -or -name '*.c')
LIB_OBJS := $(patsubst %, $(LIB_BIN_DIR)/%.o, $(LIB_SRCS))

## Compile ##
all: $(LIB_BIN)

# Build C and Assmebly objects
$(LIB_BIN_DIR)/%.o: %
	@mkdir -p $(shell dirname $@)
	${CC} ${CFLAGS} -c -o $@ $<
	
# Link and archive
$(LIB_BIN): $(LIB_OBJS)
	@mkdir -p $(shell dirname $@)
	${AR} rcs $@ $^
	${RANLIB} $@