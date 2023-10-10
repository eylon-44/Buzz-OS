## Buzz OS Config Makefile ## ~ eylon

# Set main directories :: ROOT_DIR is global and set in the main makefile
SRC_DIR := $(ROOT_DIR)/src
BIN_DIR := $(ROOT_DIR)/bin
INCLUDE_DIR := $(SRC_DIR)/include

# Executables
DISK_IMG := $(BIN_DIR)/disk.img
BOOT_BIN := $(BIN_DIR)/bootloader/bootsector.bin
KRNL_BIN := $(BIN_DIR)/kernel/kernel.bin

# Compiler settings
CC      := gcc
CFLAGS  := -I$(INCLUDE_DIR) -m32 -fno-PIC -ffreestanding

AS      := nasm
ASFLAGS := -f elf32

LD      := ld
LDFLAGS := -m elf_i386 --oformat binary

# Kernel specifics
KRNL_OFFSET := 0x1000
KRNL_ENTRY  := $(BIN_DIR)/kernel/kernel_entry.o