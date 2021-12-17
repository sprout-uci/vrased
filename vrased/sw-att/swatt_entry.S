#include "vrased-config.h"

/*********** TRUSTED VRASED WRAPPER CODE (inside SW-Att) ***********/

/* Assembly stub placed at the first address of SW-Att, does the following:
 *
 * 1. Sanitize ABI by setting up exclusive stack
 * 2. Call SW-Att C code to compute MAC in trusted_wrapper.c
 * 3. Cleanup ABI and return to untrusted caller outside SW-Att.
 *
 * NOTE: Step (1) has to be done in assembly as the _untrusted_ caller cannot
 * be trusted to correctly setup the exclusive stack.
 */
    .section .do_mac.entry
    .global swatt_entry
swatt_entry:
    /* 1. Sanitize ABI (exclusive stack) */
    mov #EXCLUSIVE_STACK_BASE, r1

    /* 2. Now call trusted C code to compute MAC */
    call #Hacl_HMAC_SHA2_256_hmac_entry

    /* 3. Cleanup and return to untrusted caller */
    add #70, r1

    mov #swatt_cont_trampoline, r6
    br #__mac_leave

/*
 * Very last trusted instruction in SW-Att.
 *
 * NOTE: the symbol __mac_leave is placed in the linker script at the last 2
 * bytes of SW-Att to hold the .do_mac.leave contents below (which has to be a
 * 2-byte instruction, hence, we cannot do `br &swatt_cont` directly).
 */
    .section .do_mac.leave
swatt_exit:
    br r6


/*********** UNTRUSTED VRASED WRAPPER CODE (outside SW-Att) ***********/

/**
 * Untrusted trampoline code to transfer execution back to wrapper.c.
 *
 * NOTE: For step (3) above, we avoid the unnecessary complication of the
 * original scheme that passed an untrusted continuation address parameter in
 * RAM (via &0x0300). We now simply first jump to the fixed label `swatt_cont`
 * outside of SW-Att, where _untrusted_ trampoline code then performs the
 * indirect branch with the untrusted parameter. This avoids having to reason
 * about or check untrusted parameters (cf. note that the HW-Mod implementation
 * enforces that the last SW-Att instruction cannot jump back to the middle,
 * but such enforcement appears not strictly necessary by LTL rules 3-4).
 */
  .section .text
swatt_cont_trampoline:
    br &vrased_cont_adrs
