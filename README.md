# Security analysis of VRASED

[![CI](https://github.com/martonbognar/vrased-gap/actions/workflows/ci.yaml/badge.svg)](https://github.com/martonbognar/vrased-gap/actions/workflows/ci.yaml)

This repository contains part of the source code accompanying our paper
"Showcasing the gap between formal guarantees and real-world security in
embedded architectures" to appear at the IEEE Symposium on Security and Privacy 2022.
More information on the paper and links to other investigated systems can be
found in the top-level [gap-attacks](https://github.com/martonbognar/gap-attacks) repository.

**:heavy_check_mark: Continuous integration.** 
A full reproducible build and reference output for all of the VRASED attack
experiments, executed via a cycle-accurate `iverilog` simulation of the
openMSP430 core, can be viewed in the [GitHub Actions log](https://github.com/martonbognar/vrased-gap/actions).
We also integrated VRASED's machine-checked proofs into the CI framework,
showing that our attacks remain entirely undedected by the current proof
strategy.

**:no_entry_sign: Mitigations.**
Where applicable, we provide simple patches for the identified implementation
flaws in a separate [mitigations](https://github.com/martonbognar/vrased-gap/tree/mitigations)
branch, referenced in the table below.
Note, however, that these patches merely fix the identified vulnerabilities in
the VRASED reference implementation in an _ad-hoc_ manner.
Specifically, our patches do not address the root cause for these oversights
(i.e., in terms of preventing implementation-model mismatch, missing attacker
capabilities, deductive errors) and cannot in any other way guarantee the
absence of further vulnerabilities.
We provide more discussion on mitigations and guidelines in the paper.

## Overview

### Implementation/model mismatches

| Paper reference | Proof-of-concept attack | Mitigation? | Description |
|-----------------|---------------|-------------|-------------|
| VI-B1           | [B-1-dma-translation.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/B-1-dma-translation.sh) | :heavy_check_mark: [todo](todo) | Leak full key[0:63] via incorrect DMA address translation. |
| VI-B2           | [B-2-key-size.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/B-2-key-size.sh) | :heavy_check_mark: [todo](todo) | Leak key[31:63] via inconsistent key sizes. |

### Missing attacker capabilities

| Paper reference | Proof-of-concept attack | Mitigation? | Description |
|-----------------|---------------|-------------|-------------|
| VI-C1           | [apex-gap repository](https://github.com/martonbognar/apex-gap) | :heavy_check_mark: [todo](todo) | Secure metadata corruption with a peripheral. |
| VI-C2           | [C-2-stack-pointer.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/C-2-stack-pointer.sh) | :heavy_check_mark: [todo](todo) | Leak key[0:21] with stack pointer poisoning. |
| VI-C3           | [C-3-auth-timing.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/C-3-auth-timing.sh) | :heavy_check_mark: [todo](todo) | Leak authentication token with start-to-end timing. |
| VI-C4           | [C-4-nemesis.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/C-4-nemesis.sh) | :heavy_check_mark: [todo](todo) | Leak authentication token with Nemesis side channel. |
| VI-C5           | [C-5-dma-sc.sh](https://github.com/martonbognar/vrased-gap/blob/master/scripts/C-5-dma-sc.sh) | :heavy_check_mark: [todo](todo) | Leak authentication token with DMA side channel. |

### Deductive errors

| Paper reference | Proof-of-concept attack | Mitigation? | Description |
|-----------------|---------------|-------------|-------------|
| VI-D | N/A (see paper) | :x: | Missing assumptions about the core. |

## Source code organization

This repository is a fork of the upstream
[sprout-uci/vrased](https://github.com/sprout-uci/vrased)
repository that contains the source code of a verifiable remote attesation
hardware-software co-design, described in the following paper.

> I. D. O. Nunes, K. Eldefrawy, N. Rattanavipanon, M. Steiner, and G. Tsudik, "VRASED: A verified hardware/software co-design for remote attestation," in 28th USENIX Security Symposium, 2019, pp. 1429–1446.

The original upstream VRASED system is accessible via commit
[4a29c24](https://github.com/martonbognar/vrased-gap/commit/4a29c248d55b132bacf2fd0e8b659d561478b8b6)
and earlier. All subsequent commits implement our test framework and
proof-of-concept attacks.

Our attacks are integrated into the _untrusted_ VRASED wrapper code.
Specifically, we extended the untrusted VRASED invocation code in
[`wrapper.c`](vrased/sw-att/wrapper.c#L113).
The required attack code is selected using C precompiler directives, depending
on the value of `__ATTACK` (i.e., a number between 1 and 6, set in the
top-level attack runner script in the `scripts` directory).

**:warning: Note (HW-Mod-Auth).** VRASED includes an alternative, and similarly verified,
version of HW-Mod to support verifier authentication (cf. paper).
Unfortunately, however, while the added functionality to support verifier
authentication is rather limited, both versions of HW-Mod do not share a
unified implementation nor proof code base.
Our continuous integration setup, hence, runs all (applicable) attacks against
both the default [`hw-mod`](vrased/hw-mod) and the alternative
[`hw-mod-auth`](vrased/hw-mod/hw-mod-auth).
Results for both versions of HW-Mod can be viewed in the [GitHub Actions
log](https://github.com/martonbognar/vrased-gap/actions).
Importantly, our experiments revealed several divergences and additional
shortcomings of the verified HW-Mod-Auth module:
* HW-Mod-Auth does _not_ monitor the `irq` signal and, hence, does not comply
  with the explicit VRASED atomicity design requirement. This important
  requirement also seems to be entirely missing from the HW-Mod-Auth LTL
  requirements, and this implementation oversight was, hence, not caught by the
  proof.  (Also note that, in the absence of resets on interrupts, the
  C-4-nemesis side-channel attack, of course, does not apply to HW-Mod-Auth).
* The HW-Mod-Auth implementation (but not proof, cf. below) interestingly
  appears to have been parameterized with the correct key size, making
  the optional HW-Mod-Auth resistant against attack B-2-key-size.
* The proof accompanying HW-Mod-Auth does currently _not_ succeed: it was
  incorrectly parameterized with wrong `KMEM_SIZE` and `CTR_SIZE` parameters
  that are out-of-sync with the implementation. This could have been detected,
  as the current proof fails for the accompanying implementation, even
  generating insightful counterexamples (cf. continuous integration logs).

## Running the proof-of-concept attacks

To run the VRASED attacks, simply proceed as follows:

```bash
$ cd scripts
$ ./B-1-dma-translation.sh # or select other attack scripts in this directory
```
