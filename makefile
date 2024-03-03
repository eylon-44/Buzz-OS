## Buzz OS Main Makefile ## ~ eylon

#----------------#
#---<SETTINGS>---#
#----------------#

# Set main directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := include
SYMBOLS_DIR := $(BIN_DIR)/symbols

# All kernel related code directories
KRNL_DIRS := kernel drivers cpu utils
KRNL_DIRS := $(patsubst %, $(SRC_DIR)/%, $(KRNL_DIRS))
KRNL_SRCS := $(shell find $(KRNL_DIRS) -name '*.asm' -or -name '*.c')
KRNL_OBJS := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%.o, $(KRNL_SRCS))

# Executables
DISK_IMG := $(BIN_DIR)/disk.img
BOOT_BIN := $(BIN_DIR)/boot/bootloader.bin
KRNL_BIN := $(BIN_DIR)/kernel/kernel.bin

# Compiler settings
CC      := gcc
CFLAGS  := -I$(INCLUDE_DIR) -m32 -nostdlib -fno-builtin -fno-pic -static -ffreestanding -no-pie -Wall -Wextra -Werror -ggdb -Og

AS      := nasm
ASFLAGS := -f elf32 -g -F dwarf

LD      := ld
LDFLAGS := -m elf_i386 --oformat binary -nostdlib
KRNL_LD_SCRIPT := auto/kernel.ld

KRNL_ENTRY  := $(BIN_DIR)/kernel/kernel_entry.asm.o

SECTOR_SIZE  := 512		# sector size in bytes
DISK_SECTORS := 2048	# disk size in sectors, 2048=1MB
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

# Compile C kernel sources into objects
$(BIN_DIR)/%.c.o: $(SRC_DIR)/%.c
	mkdir -p $(shell dirname $@)
	gcc ${CFLAGS} -c -o $@ $<

# Compile Assembly kernel sources into objects
$(BIN_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	mkdir -p $(shell dirname $@)
	nasm ${ASFLAGS} -o $@ $<

# Link the kernel into a single binary file
$(KRNL_BIN): $(KRNL_OBJS)
	# Make the kernel's binary
	${LD} -T ${KRNL_LD_SCRIPT} ${LDFLAGS} -o $@ $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $(KRNL_OBJS))

	# Make the kernel's symbol table
	@mkdir -p ${SYMBOLS_DIR}
	${LD} -T ${KRNL_LD_SCRIPT} -m elf_i386 -o ${SYMBOLS_DIR}/kernel.elf $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $(KRNL_OBJS))

# Build the disk image
$(DISK_IMG): $(KRNL_BIN) $(BOOT_BIN)
	# create an empty disk
	dd if=/dev/zero   of=${DISK_IMG} bs=${SECTOR_SIZE} count=${DISK_SECTORS}
	# load the bootloader into the first sector
	dd if=${BOOT_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=0 conv=notrunc count=${BOOT_SECTORS}
	# load the kernel into the second sector
	dd if=${KRNL_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=$(BOOT_SECTORS) conv=notrunc count=${KRNL_SECTORS}

	@echo "\n. . . DONE . . .\n"
	@tree ${BIN_DIR}

#---------------#
#---<EXECUTE>---#
#---------------#

QEMU_FLAGS := -machine q35 -drive file=$(DISK_IMG),index=0,media=disk,format=raw
run: $(DISK_IMG)
	qemu-system-i386 ${QEMU_FLAGS}

runc:
	echo TODO

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
	
.PHONY: all clean re rer rerd run runc rund
