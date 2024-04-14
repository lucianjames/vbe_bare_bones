CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Wpedantic -O3
CFLAGS_DEBUG = -std=gnu99 -ffreestanding -Wall -Wextra -Wpedantic -g
LDFLAGS = -ffreestanding -nostdlib -lgcc
LDFLAGS_DEBUG = -ffreestanding -nostdlib -lgcc -g

SRCDIR := src
OBJDIR := obj
BINDIR := bin
ISODIR := isoroot

LINKSCRIPT := linker.ld

KBINNAME := bb_vbe_kernel.elf
ISONAME := bb_vbe.iso

FONTFILE := zap-vga09.psf
FONTOBJ := $(OBJDIR)/font.o

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
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@ 

# Pattern rule for assembling NASM files
$(OBJDIR)/%.o: $(SRCDIR)/%.nas | $(OBJDIR)
	nasm -felf32 $< -o $@

# Rule for creating font object
font: | $(OBJDIR)
	objcopy -O elf32-i386 -B i386 -I binary $(FONTFILE) $(FONTOBJ)

# Rule for building the ELF file.
elf: $(OBJS) font | $(BINDIR) # Requires
	$(CC) $(LDFLAGS_DEBUG) -T $(LINKSCRIPT) -o $(BINDIR)/$(KBINNAME) $(OBJS) $(FONTOBJ)

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
	qemu-system-i386 -boot d -cdrom $(ISONAME) -serial file:serial.log -monitor stdio -vga std

runvmgdb: iso
	qemu-system-i386 -s -S -boot d -cdrom $(ISONAME) -serial file:serial.log -monitor stdio -vga std -d cpu_reset

# Clean up build files
clean:
	rm -r $(OBJDIR) $(BINDIR) $(ISODIR) $(ISONAME) serial.log

# Tells make that these target names are not associated with filenames
.PHONY: font elf iso all runvm runvmgdb clean
