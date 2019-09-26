# PURE: Using Verified Remote Attestation to Obtain Proofs of Update, Reset and Erasure in Low-End Embedded Systems
PURE shows how a secure remote attestation architecture can be extended to enable useful security services for low-end embedded devices.
In particular, PURE extends [VRASED](https://github.com/sprout-uci/vrased), to implement provably secure software update, erasure, and systemwide resets.
If serially composed as one monolithic operation, these features guarantee to a verifier that a remote prover has been updated to a functional and malware-free state, and was properly initialized after such process.
Changes introduced by PURE to VRASED trusted components (available in this branch) are also formally verified.

For more details please check PURE paper available at: http://sprout.ics.uci.edu/projects/attestation/papers/pure.pdf

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

To install the verification tools:

        cd scripts
        make install

To check HW-Mod against both VRASED and PURE subproperties using NuSMV run:

        make verify

For PURE PoR LTL computer proof check the readme file in:

        verification_specs/pure_PoR_proofs/

## Building PURE Software and Running PURE on FPGA

See README.md in the [master branch](https://github.com/sprout-uci/vrased).
Follow the same steps using the source files contained in this branch instead.
