#!/bin/bash
export PATH="$HOME/opt/cross/bin:$PATH"

OUTNAME="bbkernel.elf"

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wpedantic
i686-elf-as boot.s -o boot.o
i686-elf-gcc -T linker.ld -o $OUTNAME -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

if grub-file --is-x86-multiboot $OUTNAME; then
  echo multiboot confirmed
else
  echo multiboot not confirmed
fi
