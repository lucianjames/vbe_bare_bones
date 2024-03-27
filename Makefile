CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wpedantic
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc

SRCDIR := src
OBJDIR := obj
BINDIR := bin
ISODIR := isoroot

KBINNAME := bb_vbe_kernel.elf

# Detect source files automatically
SRCS := $(wildcard $(SRCDIR)/*.c)
NASMS := $(wildcard $(SRCDIR)/*.nas)

# Generate corresponding object file names
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS)) \
		$(patsubst $(SRCDIR)/%.nas,$(OBJDIR)/%.o,$(NASMS))

.PHONY: all clean iso elf

all: elf iso

elf: $(BINDIR)/$(KBINNAME)

iso: $(BINDIR)/$(KBINNAME) | elf grub.cfg
	mkdir -p ./$(ISODIR)/boot/grub
	cp grub.cfg ./$(ISODIR)/boot/grub
	cp $(BINDIR)/$(KBINNAME) ./$(ISODIR)/boot/
	grub-mkrescue $(ISODIR) -o $(KBINNAME).iso

$(BINDIR)/$(KBINNAME): $(OBJS) | $(BINDIR)
	$(CC) -T linker.ld -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.nas | $(OBJDIR)
	nasm -felf32 $< -o $@

$(OBJDIR) $(BINDIR) $(ISODIR):
	mkdir -p $@

clean:
	rm -r $(OBJDIR) $(BINDIR) $(ISODIR) $(KBINNAME).iso serial.log

runvm: iso
	qemu-system-i386 -boot d -cdrom $(KBINNAME).iso -serial file:serial.log -monitor stdio