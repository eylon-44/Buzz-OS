## Buzz OS User C Program Makefile ## ~ eylon

# Use this makefile to compile a Buzz OS compatible C program.
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

# Files #
# source code directory
SRC_DIR := src
# binary output directory
BIN_DIR := bin
# included header files directory
INC_DIR := inc

# find C & GAS Assembly source files within the source directory
SRCS := $(shell find $(SRC_DIR) -name '*.c' -or -name '*.S')
# create a list of expected object files
OBJS := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%.o, $(SRCS))

# Buzz OS project path
BUZZ :=

# name of the final executable
EXEC := $(BIN_DIR)/a.out
# bzlibc library path
LIBC := $(BUZZ)/bin/libc/lib_bzlibc.a

# Compiler settings #
# the C compiler
CC      := gcc
# compiler flags
CFLAGS  := -I$(INC_DIR) -I$(BUZZ)/include/libc -m32 -nostdlib -nostartfiles -nostdinc -fno-builtin -static -no-pie -Wall -Wextra -ggdb # -fno-pic

# make sure that BUZZ is defined
ifndef BUZZ
$(error BUZZ is not defined! Please set it with the path of the Buzz OS project on your system)
endif

#---------------#
#---<COMPILE>---#
#---------------#

all: $(EXEC)

# Compile C & GAS Assembly source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%
	@mkdir -p ${BIN_DIR}
	${CC} -c ${CFLAGS} -o $@ $<

# Compile the operating system (together with the libc)
OS:
	${MAKE} -C ${BUZZ}

# Link all objects into a single ELF file
$(EXEC): OS $(OBJS)
	${CC} ${CFLAGS} -L$(shell dirname $(LIBC)) -l$(patsubst lib%.a,%,$(shell basename $(LIBC))) -o $@ $(filter-out $<, $^) 

	@echo "\n. . . DONE . . .\n"


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
	rm -rf ${BIN_DIR}

# fully recompile
re: clean all

# fully recompile and run
rer: re run

# fully recompile and run debug
rerd: re rund
	
.PHONY: all clean re rer rerd run rund