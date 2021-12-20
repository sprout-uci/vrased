# Security analysis of VRASED

[![CI](https://github.com/martonbognar/vrased-gap/actions/workflows/ci.yaml/badge.svg)](https://github.com/martonbognar/vrased-gap/actions/workflows/ci.yaml)

This repository contains part of the source code accompanying our paper "Mind
the Gap: Studying the Insecurity of Provably Secure Embedded Trusted Execution
Architectures" to appear at the IEEE Symposium on Security and Privacy 2022.
More information on the paper and links to other investigated systems can be
found in the top-level [gap-attacks](https://github.com/martonbognar/gap-attacks) repository.

> M. Bognar, J. Van Bulck, and F. Piessens, "Mind the Gap: Studying the Insecurity of Provably Secure Embedded Trusted Execution Architectures," in 2022 IEEE Symposium on Security and Privacy (S&P).

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

| Paper reference | Proof-of-concept attack | Patch? | Description |
|-----------------|---------------|:-------------:|-------------|
| VI-B1           | [B-1-dma-translation.sh](scripts/B-1-dma-translation.sh) | [`1ae6d1a`](https://github.com/martonbognar/vrased-gap/commit/1ae6d1a53c7d7632c89aaf98ae73617bc193376d) | Leak full key[0:63] via incorrect DMA address translation. |
| VI-B2           | [B-2-key-size.sh](scripts/B-2-key-size.sh) | [`fa806f7`](https://github.com/martonbognar/vrased-gap/commit/fa806f787353e078bb9e2e470a5eea90288fef12) | Leak key[31:63] via inconsistent key sizes. |
| Appendix-C-1    | [A-1-register-leak.sh](scripts/A-1-register-leak.sh) | [`bdc42f3`](https://github.com/martonbognar/vrased-gap/commit/bdc42f36aa4f1a355397a85b5413af43ee2c1d68) | Leak uncleared caller-save registers after SW-Att execution. |

### Missing attacker capabilities

| Paper reference | Proof-of-concept attack | Patch? | Description |
|-----------------|---------------|:-------------:|-------------|
| VI-C1           | [apex-gap repository](https://github.com/martonbognar/apex-gap) | :x: | Secure metadata corruption with a peripheral. |
| VI-C2           | [C-2-stack-pointer.sh](scripts/C-2-stack-pointer.sh) | [`8fdc298`](https://github.com/martonbognar/vrased-gap/commit/8fdc2988c6908f7f6d96685eab6370e57bc4eaba) | Leak key[0:21] with stack pointer poisoning. |
| VI-C3           | [C-3-auth-timing.sh](scripts/C-3-auth-timing.sh) | [`531db52`](https://github.com/martonbognar/vrased-gap/commit/531db52f07e3a26dbc5a6f11c621f82bcbbe775d) | Leak authentication token with start-to-end timing. |
| VI-C4           | [C-4-nemesis.sh](scripts/C-4-nemesis.sh) | [`70e2bf4`](https://github.com/martonbognar/vrased-gap/commit/70e2bf4c294e6b1cc7fbff20b01b7295d0f990f9) | Leak authentication token with Nemesis side channel. |
| VI-C5           | [C-5-dma-sc.sh](scripts/C-5-dma-sc.sh) | [`621f2a8`](https://github.com/martonbognar/vrased-gap/commit/621f2a8b40579adc198b4261c63d5ded91a64a6d) | Leak authentication token with DMA side channel. |

### Deductive errors

| Paper reference | Proof-of-concept attack | Patch? | Description |
|-----------------|---------------|:-------------:|-------------|
| VI-D | N/A (see paper) | :x: | Missing assumptions about the core. |

## Source code organization

This repository is a fork of the upstream
[sprout-uci/vrased](https://github.com/sprout-uci/vrased)
repository that contains the source code of a verifiable remote attesation
hardware-software co-design, described in the following paper.

> I. D. O. Nunes, K. Eldefrawy, N. Rattanavipanon, M. Steiner, and G. Tsudik, "VRASED: A verified hardware/software co-design for remote attestation," in 28th USENIX Security Symposium, 2019, pp. 1429–1446.

The original upstream VRASED system is accessible via commit
[`4a29c24`](https://github.com/martonbognar/vrased-gap/commit/4a29c248d55b132bacf2fd0e8b659d561478b8b6)
and earlier. All subsequent commits implement our test framework and
proof-of-concept attacks.

**:warning: VRASED derivatives.**
Multiple derived architectures have been published that are
directly derived from the open-source VRASED research prototype and
use its security arguments as the basis of their own.
At the time of writing, open-source VRASED-based security architectures include
[RATA (CCS'21)](https://github.com/sprout-uci/RATA),
[APEX (USENIX'20)](https://github.com/sprout-uci/APEX), and
[PURE (ICCAD'19)](https://github.com/sprout-uci/vrased/tree/pure).
While we only validated the attacks in this repository on the original VRASED
base architecture, they may similarly affect these derived architectures.

## Attack code and experimental setup

Our attacks are integrated into the _untrusted_ VRASED wrapper code.
Specifically, we extended the untrusted VRASED invocation code in
[`wrapper.c`](vrased/sw-att/wrapper.c#L113).
The required attack code is selected using C precompiler directives, depending
on the value of `__ATTACK` (i.e., a number between 1 and 7, set in the
top-level attack runner script in the `scripts` directory).

VRASED includes an alternative, and similarly verified,
version of HW-Mod to optionally support verifier authentication (cf. paper).
Unfortunately, however, while the added functionality to support verifier
authentication is rather limited, both versions of HW-Mod do not share a
unified implementation nor proof code base.
Our continuous integration setup, hence, runs all (applicable) attacks against
both the default [`hw-mod`](vrased/hw-mod) and the alternative
[`hw-mod-auth`](vrased/hw-mod/hw-mod-auth).
Results for both versions of HW-Mod can be viewed in the [GitHub Actions
log](https://github.com/martonbognar/vrased-gap/actions).

**:warning: HW-Mod-Auth.** 
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

## Installation

The original installation instructions of VRASED can be found [here](README-original.md).

What follows are minimal instructions to get the experimental environment up and running on Ubuntu (tested on 20.04).

- Prerequisites:
  ```bash
  $ sudo apt-get install bison pkg-config gawk clang flex gcc-msp430 iverilog expect-dev libffi-dev
  ```
- Checkout VRASED:
  ```bash
  $ git clone https://github.com/martonbognar/vrased-gap.git
  $ cd vrased-gap
  ```
- Optional: following steps are only needed when you want to run VRASED with our mitigations:
  ```bash
  $ git checkout mitigations
  $ mkdir build
  $ cd build
  $ cmake ..
  $ cd ..
  ```

## Running the proof-of-concept attacks

To run the VRASED attacks, simply proceed as follows:

```bash
$ mkdir build && cd build && cmake .. && cd ..
$ cd scripts
$ ./B-1-dma-translation.sh # or select other attack scripts in this directory
```
