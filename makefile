## Buzz OS Main Makefile ## ~ eylon

SRC_DIR := src
BIN_DIR := bin

BOOT_DIR := $(SRC_DIR)/bootloader
KRNL_DIR := $(SRC_DIR)/kernel

DISK := $(BIN_DIR)/disk.img
BOOT := $(BIN_DIR)/bootloader/bootloader.o
KRNL := $(BIN_DIR)/kernel/kernel.o


## Compile ##
all: disk

bootloader:
	make -C ${BOOT_DIR}
kernel:
	make -C ${KRNL_DIR}

disk: bootloader kernel
	dd if=/dev/zero of=${DISK} bs=512 count=2880 	             # create an empty disk
	dd if=${BOOT} of=${DISK} bs=512 count=1 seek=0 conv=notrunc  # load the bootloader into the first sector
	dd if=${KRNL} of=${DISK} bs=512 count=1 seek=1 conv=notrunc  # load the kernel into the second sector

## Execute ##
run: disk
	qemu-system-i386 -machine q35 -fda $< -gdb tcp::26000 -S &

rund: run
	gdb -x .gdbinit ${GDB_FLAGS}

clean:
	rm -rf ${BIN_DIR}

.PHONY: all disk bootloader kernel clean run rund