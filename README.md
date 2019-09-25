# PURE: Using Verified Remote Attestation to Obtain Proofs of Update, Reset and Erasure in Low-End Embedded Systems
We show how a secure RA architecture can be extended to enable important and
useful security services for low-end embedded devices. In particular, we extend the formally verified RA architecture, VRASED (see below), to
implement provably secure software update, erasure, and systemwide resets. When (serially) composed, these features guarantee
to verifier that a remote prover has been updated to a functional and
malware-free state, and was properly initialized after such process.

### Directory Structure

.
├── application
│   └── simulation
├── demo
├── msp_bin
├── openmsp430
│   ├── contraints_fpga
│   ├── fpga
│   ├── msp_core
│   ├── msp_memory
│   ├── msp_periph
│   └── simulation
├── scripts
│   ├── build
│   └── verif-tools
├── verification_specs
│   ├── pure_PoR_proofs
│   └── vrased_soundness_and_security_proofs
└── vrased
    ├── hw-mod
    │   └── hw-mod-auth
    └── sw-att
        └── hacl-c

## PURE Verification

First we need to install the verification tools:

        cd scripts
        make install

To check HW-Mod against VRASED subproperties using NuSMV run:

        make verify

For PURE end-to-end soundness and security computer proofs check the readme file in:

        verification_specs/pure_PoR_proofs/

## VRASED Verification

See README.md in the master branch
