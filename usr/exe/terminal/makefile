## Buzz User C & Assembly Program Makefile ## ~ eylon

# Use this makefile to compile a Buzz-OS compatible C or Assembly program.
# Copy this file into the root of your C project and rename it to "makefile".
# You may change the code and variables of this file according to the requirements of your project.
# Navigate to the root of your C project and enter "make". This will compile your project into
# an ELF file located under the BIN_DIR. If you wish, you could add any one of these parameters
# to the "make" command: run (run), rund (run and debug), re (delete and recompile), rer (delete,
# recompile and run), rerd (delete, recompile, run and debug), clean (delete).
# Only C code located at the SRC_DIR will be compiled. Pay attention that the compiler will
# link the bzlibc and NOT the standard glibc.
#
# !!!IMPORTANT!!! You must set the variable BUZZ with the path of the Buzz OS project on your system.

#----------------#
#---<SETTINGS>---#
#----------------#

# source code directory
SRC_DIR := src
# binary output directory
BIN_DIR := bin
# included header files directory
INC_DIR := inc

# Find C & GAS sources
SRCS := $(shell find $(SRC_DIR) -name '*.c' -or -name '*.S')
# Create a list of expected object files
OBJS := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%.o, $(SRCS))

# Path to the Buzz-OS project
BUZZ := ../../../

# Path to the file system's layout directory
FS_LAYOUT = $(BUZZ)/usr/fs_layout

# Path to final executable
EXEC := $(FS_LAYOUT)/$(shell cat .path)

# Path to LIBC
LIBC := $(BUZZ)/bin/libc/lib_bzlibc.a

# The C compiler
CC      := gcc
# Compiler flags
CFLAGS  := -I$(INC_DIR) -I$(BUZZ)/inc/libc/ -O1 -m32 -fno-pic -nostdlib -nostartfiles -nostdinc -fno-builtin -static -no-pie -Wall -Wextra -ggdb

#---------------#
#---<COMPILE>---#
#---------------#

all: $(EXEC)
	
# Compile C & GAS source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%
	@mkdir -p ${BIN_DIR}
	${CC} -c ${CFLAGS} -o $@ $<

# Link all objects into a single ELF file
$(EXEC): $(OBJS)
# Build the program
ifeq ($(_DO_BUILD), true)
	@mkdir -p ${shell dirname ${EXEC}}
	${CC} ${CFLAGS} -o $@ $^ -L$(shell dirname $(LIBC)) -l$(patsubst lib%.a,%,$(shell basename $(LIBC)))
# Build the operating system
else
	$(MAKE) -C $(BUZZ)
endif

#---------------#
#---<EXECUTE>---#
#---------------#

run: $(EXEC)
	${MAKE} run -C ${BUZZ}

rund: $(EXEC)
	${MAKE} rund -C ${BUZZ}


#-------------#
#---<UTILS>---#
#-------------#

clean:
	rm -rf ${BIN_DIR} ${EXEC}

# fully recompile
re: clean all

# fully recompile and run
rer: re run

# fully recompile and run debug
rerd: re rund
	
.PHONY: all clean re rer rerd run rund
