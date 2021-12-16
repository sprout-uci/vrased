# Simple CMake file to automatically parameterize all needed Verilog/C/LTL
# implementation and proof files for VRASED with consistent protected memory
# addresses and sizes from a _single_ definition in this file below.
#
# NOTE: Addresses and sizes below are specified as hexadecimal numbers and
# transparently converted to decimal.
#
# NOTE: Address port width (*_AWIDTH) values for openMSP430 need to allow to
# address all _words_ of the corresponding section, i.e., log2(*_SIZE / 2).
#
project(vrased NONE)
cmake_minimum_required(VERSION 3.13)

macro(hex2dec var hexval)
    math(EXPR decval "${hexval}" OUTPUT_FORMAT DECIMAL)
    message(STATUS "Setting ${var}: ${hexval} (hex) => ${decval} (dec)")
    set(${var} ${decval})
endmacro()

# ROM region for SW-Att code (private)
hex2dec(VRASED_CR_BASE      0xA000)
hex2dec(VRASED_CR_SIZE      0x4000)
hex2dec(VRASED_CR_AWIDTH    0xD)

# ROM region for SW-Att key (private)
hex2dec(VRASED_KR_BASE      0x6A00)
hex2dec(VRASED_KR_SIZE      0x0040)
hex2dec(VRASED_KR_AWIDTH    0x5)

# RAM region for SW-Att exclusive stack (private)
hex2dec(VRASED_XS_BASE      0x0400)
hex2dec(VRASED_XS_SIZE      0x0C00)

# RAM region for SW-Att challenge and MAC result (shared)
hex2dec(VRASED_MR_BASE      0x0230)
hex2dec(VRASED_MR_SIZE      0x0020)

# RAM region for VRASED_A authentication counter
hex2dec(VRASED_CTR_BASE     0x0270)
hex2dec(VRASED_CTR_SIZE     0x0020)

add_subdirectory(openmsp430/msp_core)
add_subdirectory(vrased/sw-att)
add_subdirectory(verification_specs)
