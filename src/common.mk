## Common Makefile ## ~ eylon

BIN_DIR  := ../../$(BIN_DIR)/kernel
BIN_OUT    := $(BIN_DIR)/kernel.bin
KRNL_ENTRY := $(BIN_DIR)/kernel_entry.o

# Find C and Assembly sources
KRNL_SRCS := $(shell find -name '*.asm' -or -name '*.c' | sed 's|^\./||')
KRNL_OBJS := $(patsubst %.c, $(BIN_DIR)/%.o, $(patsubst %.asm, $(BIN_DIR)/%.o, $(KRNL_SRCS)))

all: $(BIN_OUT)

# Link all objects
$(BIN_OUT): $(KRNL_OBJS) # TODO links with drivers?
	ld -m elf_i386 -Ttext 0x1000 --oformat binary -o $@ $(KRNL_ENTRY) $(filter-out $(KRNL_ENTRY), $^)

# Compile C objects
$(BIN_DIR)/%.o: %.c
	@mkdir -p ${shell dirname $@}
	${CC} ${CFLAGS} -c -o $@ $<

# Compile Assembly objects
$(BIN_DIR)/%.o: %.asm
	@mkdir -p ${shell dirname $@}
	${AS} ${ASFLAGS} -o $@ $<