# Buzz Operating System 🚀

<img src="https://github.com/eylon-44/Buzz-OS/assets/67273282/22e46b24-26ef-4a0d-930f-22e857af5d55" align="right" width="225" alt="Buzz Logo" title="Buzz Logo">
Buzz is a hobby operating system built for learning purposes. Buzz is probably the best operating system in the world (as of 1918 A.D.).

<img src="https://github.com/eylon-44/Buzz-OS/assets/67273282/92ede007-b87f-4993-b111-982da3ac916e" height="250" alt="Buzz Demo" title="Buzz Demo">

### Table of Contents 🤸

- [Quick Start for Turtles 🐢](#quick-start-for-turtles)
    - [Requirements 📝](#requirements)
    - [Install & Build 🛠️](#install-and-build)
    - [Run 🏃](#run)
- [Manual for Wizards 🧙‍♂️](#manual-for-wizards)
    - [Use 🦧](#use)
    - [Write Your Own Programs 🧑‍💻](#write-your-own-programs)
    - [Configurate 🪄](#configurate)
- [Features 🐦‍🔥](#features)
    <!-- - [Bootloader 🤰](#bootloader)
    - [Device Drivers 💪](#device-drivers)
    - [Kernel 🧠](#kernel)
    - [User 🫀](#user) -->
- [Issues 🙈](#issues)

## <a name="quick-start-for-turtles">Quick Start for Turtles 🐢</a>

### <a name="requirements">Requirements 📝</a>

The operating system can only be compiled on Unix machines. If you happen to be using a different system, you can download the binary from [here](https://github.com/eylon-44/Buzz-OS/releases/tag/v1.0).

Make sure you have all of the listed bellow installed before continuing to the next step.

| Requirement                             | Installation                         |
| --------------------------------------- | ------------------------------------ |
| [Qemu](https://www.qemu.org/)              | `sudo apt install qemu-system`     |
| [GCC](https://gcc.gnu.org/)                | `sudo apt install build-essential` |
| [Make](https://www.gnu.org/software/make/) | `sudo apt install build-essential` |
| [Python3](https://www.python.org/)         | `sudo apt install python3`         |

### <a name="install-and-build">Install & Build 🛠️</a>

Clone the repository into your machine and compile with Make.
```console
git clone https://github.com/eylon-44/Buzz-OS
cd Buzz-OS
make
```

### <a name="run">Run 🏃</a>

Inside the project's root directory, enter `make run`. The system will boot up and greet you with a terminal program. Happy buzzing 🐝.

> **Quick Tips**<br>
> • Use the `help` command to get information about available commands
> • Execute a program by inputing its path
> • Open a tab with `Ctrl+Shift+T`, close it with `Ctrl+Shift+W`, and switch to a different one with `Alt+[tab index]` •

## <a name="manual-for-wizards">Manual for Wizards 🧙‍♂️</a>

### <a name="use">Use 🦧</a>


describe a few instruction/command then put a giff of them

<img src="https://github.com/eylon-44/Buzz-OS/assets/67273282/43fd70ab-b8ed-4c92-ab18-1564da29adad" height="250" alt="Buzz Logo" title="Buzz Logo">


### <a name="write-your-own-programs">Write Your Own Programs 🧑‍💻</a>

Buzz supplies an easy interface for writing your own user programs in C and Assembly languages.

A custom build system and C standard library is provided, so you can focus on writing awesome code. The Buzz C standard library implements more than 70 functions you can use to write your code better and faster, and the Buzz user Makefile allows you to build Buzz compatible programs with a single command.

All user programs reside in the `usr/exe` directory within the project. Each program is a directory containing a Makefile, a `.path` file, a source code directory, and an include directory. The most basic user program structure can be defined as follows:
```console
├── inc
├── makefile
├── .path
└── src
    └── main.c
```

You can easily generate it by copying the `_busy` process.
```console
cd usr/exe
cp -r _busy MyCoolProgram
cd MyCoolProgram
mkdir inc   # _busy doesn't have an include directory
```

You can now write your code under the `src` directory and your header files under the `inc` directory.

When it's time to compile, enter `make`. The build system will compile all `.c` and `.S` files under the `src` directory, include the `inc` directory, and link the program with the Buzz C standard library.

Note that in each user program, a `.path` file can be found. This file holds the path that will lead to your program once in the system. You should set this file before compiling.
```console
echo /home/apps/MyCoolProgram.elf > .path
```

For available functions and header files from the C standard library, check out the [library's interface](inc/libc/).

### <a name="configurate">Configurate 🪄</a>

Buzz allows you to configure the file system already at compile time.

The `usr/file` directory within the project acts as the file system's root directory. Any file placed in this directory will later appear in the operating system's file system at the same relative location.

<img src="https://github.com/eylon-44/Buzz-OS/assets/67273282/1d4ad551-a39c-43ba-80cc-d84c9df35711" height="250" alt="Creating Files" title="Creating Files">


You can also add your own terminal commands.

Any executable placed in the `/bin` directory within the operating system's file system can be used as a terminal command. Note that the terminal must be restarted for the changes to take effect.

<img src="https://github.com/eylon-44/Buzz-OS/assets/67273282/3c6bd2a4-e409-4221-bbc7-d2bde2e887ea" height="250" alt="Creating New Commands" title="Creating New Commands">


## <a name="features">Features 🐦‍🔥</a>

#### <a name="bootloader">Bootloader 🤰</a>

* First-stage bootloader sets up the environment, loads the second stage bootloader and jumps to it
* Second-stage bootloader loads the kernel from the file system and jumps to it

#### <a name="kernel">Kernel 🧠</a>

* Hardware, exception, and software interrupt handling
* Protected memory management
* Process management
* Priority-based multiprocessing scheduling
* File system
* Standard streams and multi-tab textual display implementation
* More than 25 system call

#### <a name="device-drivers">Device Drivers 💪</a>

* Keyboard driver
* Timer driver
* Screen driver (VGA)
* Hard disk driver (PATA)

#### <a name="user">User 🫀</a>

* Custom C standard library with more than 70 functions
* Custom build system for user programs
* More than 15 user programs and commands included

## <a name="issues">Issues 🙈</a>

If my code sucks, please [open an issuse](https://github.com/eylon-44/Buzz-OS/issues/new) and let me know!

<img src="https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExdXF4OHY5ZDQydmF2NjloajN4MDZoeWJ6NGE1dmJnZzVhb2RhdTdkcyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Y1ybQjHgteZLa/giphy.webp" height="65" alt="Buzz GIF" title="Buzz GIF">

#### Like the work? 😍

Consider giving the repository a star 😎