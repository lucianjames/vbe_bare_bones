CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Wpedantic
LDFLAGS = -ffreestanding -nostdlib -lgcc

SRCDIR := src
OBJDIR := obj
BINDIR := bin
ISODIR := isoroot

LINKSCRIPT := linker.ld

KBINNAME := bb_vbe_kernel.elf
ISONAME := bb_vbe.iso

# Detect source files automatically
SRCS := $(wildcard $(SRCDIR)/*.c)
NASMS := $(wildcard $(SRCDIR)/*.nas)

# Generate corresponding object file names
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS)) \
		$(patsubst $(SRCDIR)/%.nas,$(OBJDIR)/%.o,$(NASMS))

# Rule for creating obj/bin/iso directories
$(OBJDIR) $(BINDIR) $(ISODIR): 
	mkdir -p $@

# Pattern rule for creating object files from the C files in the src dir
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR) # %.o depends on %.c and objdir existing, otherwise compilation is impossible!
	$(CC) $(CFLAGS) -c $< -o $@ 

# Pattern rule for assembling NASM files
$(OBJDIR)/%.o: $(SRCDIR)/%.nas | $(OBJDIR)
	nasm -felf32 $< -o $@

# Rule for building the ELF file.
elf: $(OBJS) | $(BINDIR) # Requires
	$(CC) $(LDFLAGS) -T $(LINKSCRIPT) -o $(BINDIR)/$(KBINNAME) $(OBJS)

# Build the iso!
iso: elf ./grub.cfg | $(ISODIR)
	mkdir -p ./$(ISODIR)/boot/grub
	cp grub.cfg ./$(ISODIR)/boot/grub
	cp $(BINDIR)/$(KBINNAME) ./$(ISODIR)/boot/
	grub-mkrescue $(ISODIR) -o $(ISONAME)

# Build ISO as the default/all target
all: iso

# Build the ISO then test it in a QEMU VM
runvm: iso
	qemu-system-i386 -boot d -cdrom $(ISONAME) -serial file:serial.log -monitor stdio

# Clean up build files
clean:
	rm -r $(OBJDIR) $(BINDIR) $(ISODIR) $(ISONAME) serial.log

# Tells make that these target names are not associated with filenames
.PHONY: elf iso all runvm clean