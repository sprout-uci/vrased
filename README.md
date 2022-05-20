# VRASED: Verifiable Remote Attestation for Simple Embedded Devices

Remote Attestation (RA) is a distinct security service that allows a trusted verifier to
measure the software state of an untrusted remote prover.
If correctly implemented, RA allows the verifier to remotely detect if the prover is in an illegal or 
compromised state. 

VRASED is a formally verified Hardware/Software co-design for remote attestation, aimed at low-end embedded systems, e.g., simple IoT devices.
It provides a level of security comparable to HW-based approaches, while relying on SW to minimize additional HW costs.
VRASED's prototype, contained in this repository, is implemented by modifying the openmsp430 open-hardware platform (https://github.com/olgirard/openmsp430).

For more details please check VRASED's paper available at: https://www.usenix.org/conference/usenixsecurity19/presentation/de-oliveira-nunes

### See also:

- [APEX:](https://github.com/sprout-uci/APEX) Verified architecture for proofs of remote software execution on low-end embedded systems.

- [PURE:](https://github.com/sprout-uci/vrased/tree/pure) Extension of VRASED to implement provably secure and verified proofs of software update, erasure and systemwide reset on low-end embedded systems.


### VRASED Directory Structure

    vrased
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
    │   └── soundness_and_security_proofs
    └── vrased
        ├── hw-mod
        │   └── hw-mod-auth
        └── sw-att
            └── hacl-c

## Dependencies

Environment (processor and OS) used for development and verification:
Intel i7-3770
Ubuntu 16.04.3 LTS

Dependencies on Ubuntu:

		sudo apt-get install bison pkg-config gawk clang flex gcc-msp430 iverilog
		cd scripts && make install

To run soundness and security proofs, install Spot: https://spot.lrde.epita.fr/install.html

## Building VRASED Software
To generate the Microcontroller program memory configuration containing VRASED trusted software (SW-Att) and sample application (in application/main.c) code run:

        cd scripts
        make mem

To clean the built files run:

        make clean

As a result of the build, two files pmem.mem and smem.mem should be created inside msp_bin directory:

- pmem.mem program memory contents corresponding the application binaries

- smem.mem contains SW-Att binaries.

        Note: Latest Build tested using msp430-gcc (GCC) 4.6.3 2012-03-01

## Running VRASED Prototype on FPGA

This is an example of how to Synthesize and prototype VRASED using Basys3 FPGA and XILINX Vivado v2017.4 (64-bit) IDE for Linux

- Vivado IDE is available to download at: https://www.xilinx.com/support/download.html

- Basys3 Reference/Documentation is available at: https://reference.digilentinc.com/basys3/refmanual

#### Creating a Vivado Project for VRASED

1- Clone this repository;

2 - Follow the steps in "Building VRASED Software" (above) to generate .mem files

2- Start Vivado. On the upper left select: File -> New Project

3- Follow the wizard, select a project name and location . In project type, select RTL Project and click Next.

4- In the "Add Sources" window, select Add Files and add all *.v and *.mem files contained in the following directories of this reposiroty:

        openmsp430/fpga
        openmsp430/msp_core
        openmsp430/msp_memory
        openmsp430/msp_periph
        /vrased/hw-mod
        /msp_bin

and select Next.

5- In the "Add Constraints" window, select add files and add the file

        openmsp430/contraints_fpga/Basys-3-Master.xdc

and select Next.

        Note: this file needs to be modified accordingly if you are running VRASED in a different FPGA.

6- In the "Default Part" window select "Boards", search for Basys3, select it, and click Next.

        Note: if you don't see Basys3 as an option you may need to download Basys3 to Vivado.

7- Select "Finish". This will conclude the creation of a Vivado Project for VRASED.

Now we need to configure the project for systhesis.

8- In the PROJECT MANAGER "Sources" window, search for openMSP430_fpga (openMSP430_fpga.v) file, right click it and select "Set as Top".
This will make openMSP430_fpga.v the top module in the project hierarchy. Now it's name should apear in bold letters.

9- In the same "Sources" window, search for openMSP430_defines.v file, right click it and select Set File Type and, from the dropdown menu select "Verilog Header".

Now we are ready to synthesize openmsp430 with VRASED's hardware the following steps might take several minutes.

10- On the left menu of the PROJECT MANAGER click "Run Synthesis", select execution parameters (e.g, number of CPUs used for synthesis) according to your PC's capabilities.

11- If synthesis succeeds, you will be prompted with the next step. Select "Run Implementation" and wait a few more minutes (tipically ~3-10 minutes).

12- If implementation succeeds select "Generate Bitstream" in the following window. This will generate the configuration binary to step up the FPGA according to VRASED hardware and software.

13- After the bitstream is generated, select "Open Hardware Manager", connect the FPGA to you computers USB port and click "Auto-Connect".
Your FPGA should be now displayed on the hardware manager menu.

        Note: if you don't see your FPGA after auto-connect you might need to download Basys3 drivers to your computer.

14- Right-click your FPGA and select "Program Device" to program the FPGA.

15- If everything succeds your FPGA should be displaying a pattern similar to the on in the video in /demo/vrased.mp4.

When all LD0 to LD8 are on at the same time, VRASED is computing attestation of the openmsp430's program memory.

## Running VRASED on Vivado Simulation Tools


## Running VRASED via Command Line Simulation

        cd scripts
        make run

This command line simulation is the fastest option, but it will only print the contents of the microcontroller registers at each cycle. For more resourceful simulation follow "Running VRASED on Vivado Simulation Tools" above.

## VRASED Verification

First we need to install the verification tools:

        cd scripts
        make install

To check HW-Mod against VRASED subproperties using NuSMV run:

        make verify

For VRASED end-to-end soundness and security computer proofs check the readme file in:

        verification_specs/soundness_and_security_proofs

## A Note on the Extent of VRASED Verification

VRASED verification extends exclusively to the RA module (within VRASED directory in this repo).

As noted in VRASED paper, verification of the underlying MCU core or proving the correctness of the integration of VRASED module with any specific core is not the focus of this project.
VRASED RA module is verified for a generic machine model (its integration with the openMSP430 core is done only as exemplary prototype). Assuring that the axioms in VRASED machine model hold when VRASED is instantiated to a particular MCU core is required in order to obtain VRASED guarantees.

For example, [this paper](https://jovanbulck.github.io/files/oakland22-gap.pdf) discusses examples where machine model axioms were not carefully observed (in the particular case of the openMSP430-based sample prototype) or intentionally removed (e.g., hardware attacks that modify code in ROM or introduce malicious yet trusted peripherals within the MCU internal bus), leading to important security consequences. We note however that none of the reported issues lie within VRASED verified RA module or falsify verified properties, but stem inobservance or active removal of required assumptions.
In sum, two 2 of the 7 reported issues led to modifications to the openMSP430-based instanciation of VRASED to correctly observe axioms regarding MCU signals.
Another 2 issues stem from invasive hardware modifications that violate VRASED threat model.
The remaining 3 do not apply to VRASED verified TCB but to a modified fork of the VRASED project, with unverified added functionality, that was, at the time, under construction.
We provicde more detailed comments about the reported issues [here](https://github.com/sprout-uci/vrased/blob/master/docs/comments_gap.pdf) and thank the authors M. Bognar, J. Van Bulck and F. Piessens for their study emphasizing the importance of fact-checking assumptions when instanciating provably secure systems.

See VRASED [paper](https://www.usenix.org/system/files/sec19-nunes.pdf) for details on the machine model requirements and verified guarantees (that apply if and only if the machine model requirements are observed).
