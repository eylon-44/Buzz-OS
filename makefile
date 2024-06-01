## Buzz OS Main Makefile ## ~ eylon

#----------------#
#---<SETTINGS>---#
#----------------#

# Set main directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := inc
SYMBOLS_DIR := $(BIN_DIR)/symbols

# All kernel related code directories
KRNL_DIRS := kernel drivers
KRNL_DIRS := $(patsubst %, $(SRC_DIR)/%, $(KRNL_DIRS))
KRNL_SRCS := $(shell find $(KRNL_DIRS) -name '*.S' -or -name '*.c')
KRNL_OBJS := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%.o, $(KRNL_SRCS))

# Executables
DISK_IMG := $(BIN_DIR)/disk.img
BOOT_BIN := $(BIN_DIR)/boot/bootloader.bin
KRNL_BIN := $(BIN_DIR)/kernel/kernel.elf
LIBC_BIN := $(BIN_DIR)/libc/lib_bzlibc.a

# File System
FS_ROOT		:= usr/fs_layout
FS_BUILDER	:= python3 auto/fs/main.py
FS_IMG		:= $(BIN_DIR)/fs.img
FS_START	:= 768	# disk sector number at which the file system starts; must match FS_START_SECTOR definition in kernel/fs.h

# Compiler settings
CC      := gcc
CFLAGS  := -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/libc -m32 -nostdlib -nostdinc -fno-builtin -fno-pic -static -ffreestanding -no-pie -Wall -Wextra -Werror -ggdb

AS      := nasm
ASFLAGS := -f elf32 -g -F dwarf

LD      := ld
LDFLAGS := -m elf_i386 -nostdlib -L$(shell dirname $(LIBC_BIN)) -l$(patsubst lib%.a,%,$(shell basename $(LIBC_BIN)))
KRNL_LD_SCRIPT := auto/kernel.ld

KRNL_ENTRY  := $(BIN_DIR)/kernel/kernel_entry.S.o

SECTOR_SIZE  := 512		# sector size in bytes
DISK_SECTORS := 8192	# disk size in sectors, 8192=4MB
KRNL_SECTORS := 512		# kernel size in sectors
MBR_SECTORS  := 1													# MBR size in sectors
BM_SECTORS   := 20													# bootmain size in sectors
BOOT_SECTORS := $(shell expr $(MBR_SECTORS) + $(BM_SECTORS))		# bootloader size in sectors

#---------------#
#---<COMPILE>---#
#---------------#

all: $(DISK_IMG)

# Build the bootloader
$(BOOT_BIN): $(SRC_DIR)/boot
	$(MAKE) -C $< BOOT_BIN_DIR=$(shell pwd)/$(BIN_DIR)/boot \
				  BOOT_BIN=$(shell pwd)/$@ 					\
				  INC_DIR=$(shell pwd)/$(INCLUDE_DIR)		\
				  BIN_DIR=$(shell pwd)/$(BIN_DIR)			\
				  MBR_SECTORS=$(MBR_SECTORS)				\
				  BM_SECTORS=$(BM_SECTORS)

# Compile C and Assembly kernel sources
$(BIN_DIR)/%.o: $(SRC_DIR)/%
	mkdir -p $(shell dirname $@)
	${CC} ${CFLAGS} -c -o $@ $<

# Build bzlibc
$(LIBC_BIN): $(SRC_DIR)/libc
	$(MAKE) -C $<	INC_DIR=$(shell pwd)/$(INCLUDE_DIR)		\
					BIN_DIR=$(shell pwd)/$(BIN_DIR)
					

# Link the kernel into a single binary file
$(KRNL_BIN): $(LIBC_BIN) $(KRNL_OBJS)
	# Make the kernel's binary
	${LD} -o $@ $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $(KRNL_OBJS)) -T ${KRNL_LD_SCRIPT} ${LDFLAGS}

# Build the file system
$(FS_IMG): $(FS_ROOT)
	${FS_BUILDER} $< $@

# Build the disk image
$(DISK_IMG): $(KRNL_BIN) $(BOOT_BIN) $(FS_IMG)
	# Create an empty disk
	dd if=/dev/zero   of=${DISK_IMG} bs=${SECTOR_SIZE} count=${DISK_SECTORS}
	# Load the bootloader into the first sector
	dd if=${BOOT_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=0 conv=notrunc count=${BOOT_SECTORS}
	# Load the kernel into the second sector
	dd if=${KRNL_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=${BOOT_SECTORS} conv=notrunc count=${KRNL_SECTORS}
	# Load the file system into its predefined sector
	dd if=${FS_IMG} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=${FS_START} conv=notrunc

	# debug
	dd if=test/test of=${DISK_IMG} bs=${SECTOR_SIZE} seek=512 conv=notrunc count=10

	@echo "\n. . . DONE . . .\n"

#---------------#
#---<EXECUTE>---#
#---------------#

QEMU_FLAGS := -serial mon:stdio -drive file=$(DISK_IMG),index=0,media=disk,format=raw -m 512
run: $(DISK_IMG)
	qemu-system-i386 ${QEMU_FLAGS}

rund: $(DISK_IMG)
	qemu-system-i386 ${QEMU_FLAGS} -s -S &
	gdb -x auto/gdbinit ${GDB_FLAGS}


#-------------#
#---<UTILS>---#
#-------------#

clean:
	rm -rf ${BIN_DIR}

# fully recompile
re: clean all

# fully recompile and run
rer: re run

# fully recompile and run debug
rerd: re rund
	
.PHONY: all clean re rer rerd run rund
