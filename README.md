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
[`wrapper.c`](https://github.com/martonbognar/vrased-gap/blob/master/vrased/sw-att/wrapper.c#L113).
The required attack code is selected using C precompiler directives, depending
on the value of `__ATTACK` (i.e., a number between 1 and 6, set in the
top-level attack runner script in the `scripts` directory).

## Running the proof-of-concept attacks

To run the VRASED attacks, simply proceed as follows:

```bash
$ cd scripts
$ ./B-1-dma-translation.sh # or select other attack scripts in this directory
```
