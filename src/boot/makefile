## Bootloader Makefile ## ~ eylon

# Generate a flat binary image bootloader that is built out of a bootloader and a bootsector which loads the bootloader
# Note that [INC_DIR], [BIN_DIR], [BOOT_BIN] and [BOOT_BIN_DIR] are passed by the parent makefile

# Constants
CC 	   ?= gcc
LD    ?= ld
CFLAGS := -I$(INC_DIR) -I$(INC_DIR)/libc -m32 -nostdlib -nostdinc -fno-builtin -fno-pic -static -ffreestanding -no-pie -Wall -Wextra -Werror -ggdb -Og
BOOTMAIN_OFFSET := 0x1000
SECTOR_SIZE := 512

# MBR settings
MBR_DIR	:= mbr
MBR_SRC := $(MBR_DIR)/bootsector.asm
MBR_BIN := $(BOOT_BIN_DIR)/$(MBR_DIR)/bootsector.bin

# Bootmain settings
BM_DIR   := bootmain
BM_SRCS  := $(shell find $(BM_DIR) -name '*.c')
BM_OBJS  := $(patsubst %.c, $(BOOT_BIN_DIR)/%.o, $(BM_SRCS))
BM_BIN   := $(BOOT_BIN_DIR)/$(BM_DIR)/bootmain.bin
BM_ENTRY := $(BOOT_BIN_DIR)/$(BM_DIR)/bootmain.o
# The bootloader uses functions that are already used by the kernel, so
# instead of rewriting them we link them with the bootloader.
BM_UTILS := drivers/ports.c.o \
	drivers/pata.c.o \
	libc/string/strcpy.c.o \
	libc/string/strtok.c.o \
	libc/string/strcmp.c.o \
	libc/string/memcpy.c.o \
	libc/string/memset.c.o \
	libc/string/strchr.c.o

BM_UTILS := $(patsubst %, $(BIN_DIR)/%, $(BM_UTILS))

# Build directories
HOME_DIR := $(shell echo $$HOME)


# Compile
all: $(BOOT_BIN)

# Compile the MBR
$(MBR_BIN): $(shell find $(MBR_DIR) -name '*.asm')
	@mkdir -p ${shell dirname $@}
	nasm -I$(MBR_DIR) -f bin -o $@ ${MBR_SRC}

# Compile the main boot section
$(BOOT_BIN_DIR)/$(BM_DIR)/%.o: $(BM_DIR)/%.c
	@mkdir -p ${shell dirname $@}
	${CC} ${CFLAGS} -c -o $@ $<

# Link the main boot section
$(BM_BIN): $(BM_ENTRY) $(filter-out $(BM_ENTRY), $(BM_OBJS))
	@mkdir -p ${shell dirname $@}
	${LD} -m elf_i386 --oformat binary -Ttext=${BOOTMAIN_OFFSET} -o $@ $^ ${BM_UTILS}

	# [DEBUG]
	@mkdir -p ${HOME_DIR}/Code/os/bin/symbols
	${LD} -m elf_i386 -Ttext=${BOOTMAIN_OFFSET} -o ${HOME_DIR}/Code/os/bin/symbols/BM.elf $^ ${BM_UTILS}

# Build the full bootloader disk image
$(BOOT_BIN): $(MBR_BIN) $(BM_BIN)
	@mkdir -p ${shell dirname $@}

	# create an empty disk
	dd if=/dev/zero  of=${BOOT_BIN} bs=${SECTOR_SIZE} count=${shell expr $(MBR_SECTORS) + $(BM_SECTORS)}
	# load the MBR into the first sector
	dd if=${MBR_BIN} of=${BOOT_BIN} bs=${SECTOR_SIZE} seek=0 conv=notrunc count=${MBR_SECTORS}
	# load the bootmain into the next sector
	dd if=${BM_BIN}  of=${BOOT_BIN} bs=${SECTOR_SIZE} seek=${MBR_SECTORS} conv=notrunc count=${BM_SECTORS}