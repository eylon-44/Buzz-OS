## Buzz OS Main Makefile ## ~ eylon

##################
#### SETTINGS ####
##################

# Set main directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := $(SRC_DIR)/include

# Global variables
export TOP_DIR := $(shell pwd)
export COMMON_MK  := $(TOP_DIR)/$(SRC_DIR)/common.mk

### DANGER ZONE ### (spooky)
# TODO comment
BOOT_DIR := $(SRC_DIR)/bootloader
KRNL_DIR := $(SRC_DIR)/kernel
DRVR_DIR := $(SRC_DIR)/drivers


# $$ KERNEL LINKED DIRECTORIES
# $$ KENREL LINKED OBJECTS

# Executables
DISK_IMG := $(BIN_DIR)/disk.img
BOOT_BIN := $(BIN_DIR)/bootloader/bootsector.bin
KRNL_BIN := $(BIN_DIR)/kernel/kernel.bin

# Compiler settings
export CC      := gcc
export CFLAGS  := -I$(INCLUDE_DIR) -m32 -fno-PIC -ffreestanding

export AS      := nasm
export ASFLAGS := -f elf32

LD      := ld
LDFLAGS := -m elf_i386 -Ttext 0x1000 --oformat binary


##################
#### COMPILE #####
##################

all: $(DISK_IMG)

# Build the bootloader
$(BOOT_BIN): $(BOOT_DIR)
	$(MAKE) -C $< BIN_OUT=$(TOP_DIR)/$@

# RM goal, call all makefiles needed to build the kernel related objects (from kernel, drivers, etc)
# RM and link them into one binary file.
# RM $$ all kernel related src dir (drivers, kernel, etc)
# RM Compile everything related to the kernel -> Link all with the kernel entry at the start
# Build and link the kernel
$(KRNL_BIN):
	$(MAKE) -C $<
	# Init the makefile of each kernel related folder (kernel, drivers...)
	# Find their objects
	# Link all the objects into "kernel.bin" with the kernel_entry.o at the start


# Build the disk image
$(DISK_IMG): $(BOOT_BIN) $(KRNL_BIN)
	dd if=/dev/zero of=${DISK_IMG} bs=512 count=2880 	                  # create an empty disk
	dd if=${BOOT_BIN} of=${DISK_IMG} bs=512 count=1 seek=0 conv=notrunc   # load the bootloader into the first sector
	dd if=${KRNL_BIN} of=${DISK_IMG} bs=512 count=16 seek=1 conv=notrunc  # load the kernel into the second sector


#################
#### EXECUTE ####
#################

QEMU_FLAGS := -machine q35 -drive file=$(DISK_IMG),format=raw
run: $(DISK_IMG)
	qemu-system-i386 ${QEMU_FLAGS}

rund: $(DISK_IMG)
	qemu-system-i386 ${QEMU_FLAGS} -gdb tcp::26000 -S &
	gdb -x .gdbinit ${GDB_FLAGS}


###############
#### UTILS ####
###############

clean:
	rm -rf ${BIN_DIR}

.PHONY: all clean run rund