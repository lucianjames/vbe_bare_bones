#!/bin/bash
export PATH="$HOME/opt/cross/bin:$PATH"

OUTNAME="bbkernel.elf"

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wpedantic
nasm -felf32 boot.nas -o boot.o
i686-elf-gcc -T linker.ld -o $OUTNAME -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

if grub-file --is-x86-multiboot2 $OUTNAME; then
  echo multiboot2 confirmed
else
  echo multiboot2 not confirmed
fi
