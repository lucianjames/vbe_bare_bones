// GCC-provided headers, provides useful things such as fixed-width types
#include <stddef.h>
#include <stdint.h>

// Checks to ensure we are compiling this properly
#if defined(__linux__)
    #error "This must be compiled with a cross-compiler"
#elif !defined(__i386__)
    #error "This must be compiled with an x86-elf compiler"
#endif


/*
    Shamelessly copypasted
    Ill clean it all up and style it to my liking laterrr
*/

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile ("inb %1, %0" 
                    : "=a" (data) 
                    : "Nd" (port));
    return data;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" 
                    :
                    : "a" (data), "Nd" (port));
}

#define PORT 0x3f8          // COM1
 
static int init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
 
   // Check if serial is faulty (i.e: not same byte as sent)
   outb(PORT, 0x00);
   unsigned char checkv = inb(PORT);
   if(checkv != 0x00) {
      return checkv;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}

int serial_received() {
   return inb(PORT + 5) & 1;
}
 
char read_serial() {
   while (serial_received() == 0);
 
   return inb(PORT);
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}

void write_serial_str(const char* str){
    for(size_t i=0; str[i] !='\0'; i++){
        write_serial(str[i]);
    }
}







/*
    VGA buffer
*/
// x86's VGA textmode buffer. Writing data to this memory location will display text.
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
// Default VGA cols/rows size
#define VGA_COLS 80
#define VGA_ROWS 25
int term_col = 0;
int term_row = 0;
uint8_t term_colour = 0x0A; // black bg, green fg (special hacker 1337 edition)


/*
    Initialises the terminal by clearing the buffer memory
*/
void term_init(){
    // Clear the VGA buffer
    for(int idx=0; idx<VGA_COLS*VGA_ROWS; idx++){
        vga_buffer[idx] = ((uint16_t)term_colour << 8) | ' '; // Term colour goes in the first 8 bits of the uint16_t, character to display goes in the last 8 bits
    }
}


/*
    Place a single character onto the screen
*/
void term_putchar(char c){
    switch(c){
        case '\n':
        {
            term_col = 0;
            term_row++;
            break;
        }
        default:
        {
            vga_buffer[(VGA_COLS * term_row) + term_col] = ((uint16_t)term_colour << 8) | c;
            term_col++;
        }
    }
    /*
        TODO: handle going past col/row limits
    */
}


/*
    Place a const char* onto the screen
*/
void term_printstr(const char* str){
    for(size_t i=0; str[i] !='\0'; i++){
        term_putchar(str[i]);
    }
}


/*
    The kernel main function
*/
void kernel_main(){
    term_init();

    int r = init_serial();
    if(r!=0){
        term_printstr("ERROR!!! Serial is faulty! inb(PORT + 0) != 0xAE\n");
    }else{
        write_serial_str("Hello FROM the kernel!\n");
        term_printstr("Hello kernel\n");
        term_printstr(":D\n");
    }
}
