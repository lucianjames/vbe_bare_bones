/* 
	Multiboot header constants
*/
.set MB_ALIGN,    1<<0             			// align loaded modules on page boundaries
.set MB_MEMINFO,  1<<1             			// provide memory map
.set MB_FLAGS,    MB_ALIGN | MB_MEMINFO  	// Assemble the multiboot flag field
.set MB_MAGIC,    0x1BADB002       			// Magic number for bootloader to find
.set MB_CHECKSUM, (0-(MB_MAGIC + MB_FLAGS)) // checksum of above, to prove we are multiboot

/* 
	Declare a multiboot header that marks the program as a kernel.
*/
.section .multiboot
	.align 4
	.long MB_MAGIC
	.long MB_FLAGS
	.long MB_CHECKSUM


/*
	Create space for a 16KiB stack.
	The stack is in its own section so it can be marked nobits, making the kernel file smaller as it will not contain an uninitialized stack
*/
.section .bss
	.align 16 // The stack on x86 must be 16-byte aligned according to the System V ABI standard, not doing so will cause UB
	stack_bottom:
		.skip 16384 # 16 KiB
	stack_top:

/*
	The code to start the kernel
*/
.section .text
.global _start
.type _start, @function
_start:
	/*
		Set up the stack in order for the C code to function properly
	*/
	mov $stack_top, %esp

	/*
		Run the kernel
	*/
	call kernel_main

	/*
		If everything is done with the kernel, lets just hang the whole system :P
	*/
	hang:
		cli			// Disable CPU Interrupts
		hlt 		// Halt the CPU
		jmp hang	// Jump back to the start of the hang loop


/*
	Sets the size of the _start symbol to the current location '.' minus its start.
	This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
