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
BOOT_BIN := $(BIN_DIR)/bootloader/bootsector.bin
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
KRNL_SECTORS := 40		# kernel size in sectors [WARNING!] can't set this value above 54 for now, see issue #1 in github
BOOT_SECTORS := 1  		# bootloader size in sectors, must be one as this is a bootsector
#---------------#
#---<COMPILE>---#
#---------------#

all: $(DISK_IMG)

# Build the bootloader
$(BOOT_BIN): $(SRC_DIR)/bootloader
	$(MAKE) -C $< BOOT_BIN=$(shell pwd)/$@ -s

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
$(DISK_IMG): $(BOOT_BIN) $(KRNL_BIN)
	@# check that the kernel binary fully fits the disk
	@if [ $$(stat -c %s $(KRNL_BIN)) -gt $$(( ${KRNL_SECTORS} * ${SECTOR_SIZE} )) ]; then\
		echo "\n------------<ERROR: KERNEL OVERFLOWS THE DISK>------------" \
		"\n$$> Change [makefile/KRNL_SECTORS] [makefile/DISK_SECTORS], and [bootloader/disk.asm/load_kernel/al] as needed";\
		exit 1;\
	fi

	dd if=/dev/zero   of=${DISK_IMG} bs=${SECTOR_SIZE} count=${DISK_SECTORS} 						 # create an empty disk
	dd if=${BOOT_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=0 conv=notrunc count=${BOOT_SECTORS}     # load the bootloader into the first sector
	dd if=${KRNL_BIN} of=${DISK_IMG} bs=${SECTOR_SIZE} seek=1 conv=notrunc count=${KRNL_SECTORS}     # load the kernel into the second sector

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
