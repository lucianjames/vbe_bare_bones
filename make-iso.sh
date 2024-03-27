OUTNAME="bbkernel.elf"
cp $OUTNAME "./isoroot/boot/"
grub-mkrescue isoroot -o bbkernel.iso