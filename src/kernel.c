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
                    : "Nd" (port)
    );
    return data;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %0, %1" 
                    :
                    : "a" (data), "Nd" (port)
    );
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
 
   // Check if serial is faulty (i.e: not same byte as sent)
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT, 0x00);        // Send test data
   if(inb(PORT) != 0x00) {
      return 1;
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
    Shamelessly copypasted utils
*/

// A utility function to reverse a string
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
// Implementation of citoa()
char* citoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}





#define MBH2_VALID_MAGIC 0x36d76289
#define ASM_INFINITE_LOOP asm volatile ("1: jmp 1b");

/*
    The kernel main function
*/
void kernel_main(unsigned long MBH2_MAGIC, unsigned long MBH2_INFO_ADDR){

    init_serial();
    
    /*
        Verify we have been booted by a multiboot2 compliant bootloader
    */
    if(MBH2_MAGIC != MBH2_VALID_MAGIC){
        write_serial_str("ERR: MBH2_MAGIC IS NOT 0x36d76289\n");
    }else{
        write_serial_str("OK: Valid multiboot2 magic\n");
    }

    /*
        Verify mbh2 info address (TODO: figure out why this works)
    */
    if(MBH2_INFO_ADDR & 7){
        write_serial_str("ERR: MBH2_INFO_ADDR & 7\n");
    }
    char addrlenstr[32];
    citoa(*(unsigned*)MBH2_INFO_ADDR, addrlenstr, 10);
    write_serial_str("MBH2_INFO_ADDR LEN: ");
    write_serial_str(addrlenstr);
    write_serial('\n');


    write_serial_str("Hello FROM the kernel!\n");
}


void kernel_startup_failure(){
    init_serial();
    write_serial_str("There was an error while trying to set up the environment for the kernel to boot :(\n");
}