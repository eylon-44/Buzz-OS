https://wiki.osdev.org/ELF
https://wiki.osdev.org/ELF_Tutorial
https://docs.oracle.com/cd/E19455-01/806-3773/elf-2/index.html

# ELF

The ELF file format has only one header with fixed placement: the ELF header, present at the beginning of every file.
The format itself is extremely flexible as the positioning, size, and purpose of every header (save the ELF header) is described by another header in the file.

