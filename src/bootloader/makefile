## Bootloader Makefile ## ~ eylon

BOOT_SCRIPT := bootsector.asm

# Find Assembly sources
BOOT_SRCS := $(shell find -name '*.asm')

# Compile
all: $(BOOT_BIN)

$(BOOT_BIN): $(BOOT_SRCS)
	@mkdir -p ${shell dirname $@}
	nasm -f bin -o $@ ${BOOT_SCRIPT}