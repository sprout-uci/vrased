#include <string.h>

#define MAC_ADDR 0x0230
#define KEY_ADDR 0x6A00
#define ATTEST_DATA_ADDR 0xE000
#define ATTEST_SIZE 0x2000

extern void
hmac(
  uint8_t *mac,
  uint8_t *key,
  uint32_t keylen,
  uint8_t *data,
  uint32_t datalen
);

void my_memset(uint8_t* ptr, int len, uint8_t val) {
  int i=0;
  for(i=0; i<len; i++) ptr[i] = val;
}

void my_memcpy(uint8_t* dst, uint8_t* src, int size) {
  int i=0;
  for(i=0; i<size; i++) dst[i] = src[i];
}

__attribute__ ((section (".do_mac.call"))) void Hacl_HMAC_SHA2_256_hmac_entry() {

    //Save application stack pointer.
    //Allocate key buffer.
    uint8_t key[64] = {0};
    //Copy the key from KEY_ADDR to the key buffer.
    memcpy(key, (uint8_t*)KEY_ADDR, 64);
    hmac((uint8_t*) key, (uint8_t*) key, (uint32_t) 64, (uint8_t*) MAC_ADDR, (uint32_t) 32);
    // Uses the result in the key buffer to compute HMAC.
    // Stores the result in HMAC ADDR.
    hmac((uint8_t*) (MAC_ADDR), (uint8_t*) key, (uint32_t) 32, (uint8_t*) ATTEST_DATA_ADDR, (uint32_t) ATTEST_SIZE);

	//return;

    // setting the return addr:
    __asm__ volatile("mov    #0x0300,   r6" "\n\t");
    __asm__ volatile("mov    @(r6),     r6" "\n\t");

    // postamble
    __asm__ volatile("add     #70,    r1" "\n\t");
    __asm__ volatile( "br      #__mac_leave" "\n\t");
}

__attribute__ ((section (".do_mac.leave"))) __attribute__((naked)) void Hacl_HMAC_SHA2_256_hmac_exit() {
    __asm__ volatile("br   r6" "\n\t");
}

void VRASED (uint8_t *challenge, uint8_t *response) {
    //Copy input challenge to MAC_ADDR:
    my_memcpy ( (uint8_t*)MAC_ADDR, challenge, 32);

    //Disable interrupts:
    __dint();

    // Save current value of r5 and r6:
    __asm__ volatile("push    r5" "\n\t");
    __asm__ volatile("push    r6" "\n\t");

    // Write return address of Hacl_HMAC_SHA2_256_hmac_entry
    // to RAM:
    __asm__ volatile("mov    #0x000e,   r6" "\n\t");
    __asm__ volatile("mov    #0x0300,   r5" "\n\t");
    __asm__ volatile("mov    r0,        @(r5)" "\n\t");
    __asm__ volatile("add    r6,        @(r5)" "\n\t");

    // Save the original value of the Stack Pointer (R1):
    __asm__ volatile("mov    r1,    r5" "\n\t");

    // Set the stack pointer to the base of the exclusive stack:
    __asm__ volatile("mov    #0x1002,     r1" "\n\t");

    // Call SW-Att:
    Hacl_HMAC_SHA2_256_hmac_entry();

    // Copy retrieve the original stack pointer value:
    __asm__ volatile("mov    r5,    r1" "\n\t");

    // Restore original r5,r6 values:
    __asm__ volatile("pop   r6" "\n\t");
    __asm__ volatile("pop   r5" "\n\t");

    // Enable interrupts:
    __eint();

    // Return the HMAC value to the application:
    my_memcpy(response, (uint8_t*)MAC_ADDR, 32);
}




