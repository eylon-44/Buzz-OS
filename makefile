## Buzz OS Main Makefile ## ~ eylon

SRC_DIR := src
BIN_DIR := bin

BOOT_DIR := $(SRC_DIR)/bootloader
KRNL_DIR := $(SRC_DIR)/kernel

DISK := $(BIN_DIR)/disk.img
BOOT := $(BIN_DIR)/bootloader/bootsector.bin
KRNL := $(BIN_DIR)/kernel/kernel.bin

#---# COMPILE #---#
all: disk

# Make the bootloader
bootloader:
	make -C ${BOOT_DIR}
# Make the kernel
kernel:
	make -C ${KRNL_DIR}

# Build the disk image
disk: bootloader kernel
	dd if=/dev/zero of=${DISK} bs=512 count=2880 	              # create an empty disk
	dd if=${BOOT} of=${DISK} bs=512 count=1 seek=0 conv=notrunc   # load the bootloader into the first sector
	dd if=${KRNL} of=${DISK} bs=512 count=16 seek=1 conv=notrunc  # load the kernel into the second sector


#---# EXECUTE #---#
QEMU_FLAGS := -machine q35 -drive file=$(DISK),format=raw
run: disk
	qemu-system-i386 ${QEMU_FLAGS}

rund: disk
	qemu-system-i386 ${QEMU_FLAGS} -gdb tcp::26000 -S &
	gdb -x .gdbinit ${GDB_FLAGS}

clean:
	rm -rf ${BIN_DIR}

.PHONY: all disk bootloader kernel clean run rund
