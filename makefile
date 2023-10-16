## Buzz OS Main Makefile ## ~ eylon

###############################################################
### WARNING! THIS MAKEFILE IS HORRIBLE BUT IT WORKS FOR NOW ###
###           IF YOU LIKE YOUR EYES DON'T READ IT           ###
###############################################################

#----------------#
#---<SETTINGS>---#
#----------------#

# Set main directories
SRC_DIR := src
BIN_DIR := bin
INCLUDE_DIR := $(SRC_DIR)/include

# Meaning to all code that is related to the kernel and not only the kernel directory
KRNL_DIRS := kernel drivers cpu
KRNL_DIRS := $(patsubst %, $(SRC_DIR)/%, $(KRNL_DIRS))
KRNL_SRCS := $(shell find $(KRNL_DIRS) -name '*.asm' -or -name '*.c')
KRNL_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(patsubst $(SRC_DIR)/%.asm, $(BIN_DIR)/%.o, $(KRNL_SRCS)))

# Executables
DISK_IMG := $(BIN_DIR)/disk.img
BOOT_BIN := $(BIN_DIR)/bootloader/bootsector.bin
KRNL_BIN := $(BIN_DIR)/kernel/kernel.bin

# Compiler settings
CC      := gcc
CFLAGS  := -I$(INCLUDE_DIR) -m32 -fno-PIC -ffreestanding -g

AS      := nasm
ASFLAGS := -f elf32 -g -F dwarf

LD      := ld
LDFLAGS := -m elf_i386 --oformat binary

# Kernel specifics
KRNL_OFFSET := 0x1000
KRNL_ENTRY  := $(BIN_DIR)/kernel/kernel_entry.o

#---------------#
#---<COMPILE>---#
#---------------#

all: $(DISK_IMG)

# Build the bootloader
$(BOOT_BIN): $(SRC_DIR)/bootloader
	$(MAKE) -C $< BOOT_BIN=$(shell pwd)/$@ -s

# Compile C kernel sources into objects
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(shell dirname $@)
	gcc -I${INCLUDE_DIR} ${CFLAGS} -c -o $@ $<

# Compile Assembly kernel sources into objects
$(BIN_DIR)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(shell dirname $@)
	nasm ${ASFLAGS} -o $@ $<

# Link the kernel into a single binary file
$(KRNL_BIN): $(KRNL_OBJS)
	${LD} ${LDFLAGS} -Ttext ${KRNL_OFFSET} -o $@ $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $(KRNL_OBJS))

	# Make the kernel's symbol table
	${LD} -o ${BIN_DIR}/kernel/kernel.elf -m elf_i386 $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $(KRNL_OBJS))

# Build the disk image
$(DISK_IMG): $(BOOT_BIN) $(KRNL_BIN)
	dd if=/dev/zero of=${DISK_IMG} bs=512 count=2880 	                  # create an empty disk
	dd if=${BOOT_BIN} of=${DISK_IMG} bs=512 count=1 seek=0 conv=notrunc   # load the bootloader into the first sector
	dd if=${KRNL_BIN} of=${DISK_IMG} bs=512 count=16 seek=1 conv=notrunc  # load the kernel into the second sector


#---------------#
#---<EXECUTE>---#
#---------------#

QEMU_FLAGS := -machine q35 -drive file=$(DISK_IMG),format=raw
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

.PHONY: all clean run rund
