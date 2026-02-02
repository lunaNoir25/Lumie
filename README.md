# The Luminous Operating System

Lumie 0.1.0
Copyright (C) 2026 Luna Moonlit Noir <lunaNoir.sk@gmail.com>

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## Description

A simple, very early operating system built with the Limine Protocol.
Lumie appears to be in the late 70s to early 80s, but 64-bit UEFI.
> This will be updated as more features are added.

## Current Features

 * Limine Protocol - Boot Implementation
 * FAT12/16 support - Direct-to-disk file reading.
 * Unix-like Minimal Shell - Simple command line with 'ls', 'cat', 'uptime', etc..
 * Custom Graphics - Coloured text support and uses the Spleen font.

## Current Limitations

 * Legacy BIOS - Lumie only supports 64-bit UEFI with no plans for Legacy BIOS support in the near future.
 * PS/2 Emulation - Lumie does not have a USB driver, PS/2 emulation is required to test on real hardware.
 * Virtual File System - There is no VFS, meaning Lumie doesn't follow the Linux Filesystem Hierarchy Standard, but will once a VFS is implemented.
 * User Space - Lumie is very minimal right now, and does not provide a user space.
 * Read Only - Lumie currently has no way to write files, only read.
 * Flat Directory Structure - Limited to only the root folder, directories are not yet supported.
 * Single-Sector Reads - Lumie assumes files fit in one 512-byte sector. It cannot read files larger than that.

## Build Instructions

> These build instructions assume you are using Linux.

The following features are required to build Lumie:
 * x86_64-elf-gcc - Cross Compiler
 * nasm - Assembler
 * objcopy - Part of binutils; required for the font
 * GNU make - For make commands
 * tar - For initramfs directory
 * xoriso - To create the ISO file
 * Limine - Bootloader
 * OVMF (Open Virtual Machine Firmware) - UEFI firmware files for QEMU
 * QEMU (qemu-system-x86_64) - Emulator to test Lumie

The source code provides a simple './run.sh' file that uses QEMU to run, and cleaned immediately after use.
That said, if you only want the ISO, simply run 'make' and a generated ISO file will appear in the root folder.

> [!Important]
> x86_64-elf-gcc must be installed in your system's PATH.
> The Makefile assumes the OVMF files are in a specific directory, if they're installed in another directory,
> change the lines under 'run: iso' in the Makefile to point to your OVMF files.
> If you do not know how to do this... ¯\\_(ツ)_/¯

## Third-Party Assets

- The default font is Spleen 8x16 by Frederic Cambus under the BSD 2-Clause License. <https://github.com/fcambus/spleen>
- Lumie utilizes the Limine Bootloader and follows the Limine Protocol for kernel initialization and booting under the BSD 2-Clause License. Limine is made by Mintsuki. <https://codeberg.org/Limine/Limine>
