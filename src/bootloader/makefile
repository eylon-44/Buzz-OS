## Bootloader Makefile ## ~ eylon
# *This is a child Makefile that won't work without his parent Makefile

BOOT_SCRIPT := bootsector.asm

# Find Assembly sources
BOOT_SRCS := $(shell find -name '*.asm')

all: $(BIN_OUT)

# Compile
$(BIN_OUT): $(BOOT_SRCS)
	@mkdir -p ${shell dirname $@}
	$(AS) -f bin -o $@ $(BOOT_SCRIPT)