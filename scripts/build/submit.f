//=============================================================================
// Copyright (C) 2001 Authors
//
// This source file may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// This source file is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This source is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this source; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------------
//
// File Name: submit.f
//
// Author(s):
//             - Olivier Girard,    olgirard@gmail.com
//
//-----------------------------------------------------------------------------
// $Rev$
// $LastChangedBy$
// $LastChangedDate$
//=============================================================================

//=============================================================================
// Testbench related
//=============================================================================

+incdir+../openmsp430/simulation/
../openmsp430/simulation/tb_openMSP430.v
../openmsp430/simulation/io_cell.v
../openmsp430/simulation/msp_debug.v

+incdir+../openmsp430/msp_memory/
../openmsp430/msp_memory/ram.v
../openmsp430/msp_memory/rom.v
../openmsp430/msp_memory/keyrom.v
../openmsp430/msp_memory/pmem.v

+incdir+../vrased/hw-mod/
../vrased/hw-mod/vrased.v

//=============================================================================
// CPU
//=============================================================================

+incdir+../openmsp430/msp_core/
../openmsp430/msp_core/openMSP430_defines.v
../openmsp430/msp_core/openMSP430.v
../openmsp430/msp_core/omsp_frontend.v
../openmsp430/msp_core/omsp_execution_unit.v
../openmsp430/msp_core/omsp_register_file.v
../openmsp430/msp_core/omsp_alu.v
../openmsp430/msp_core/omsp_sfr.v
../openmsp430/msp_core/omsp_clock_module.v
../openmsp430/msp_core/omsp_mem_backbone.v
../openmsp430/msp_core/omsp_watchdog.v
../openmsp430/msp_core/omsp_dbg.v
../openmsp430/msp_core/omsp_dbg_uart.v
../openmsp430/msp_core/omsp_dbg_i2c.v
../openmsp430/msp_core/omsp_dbg_hwbrk.v
../openmsp430/msp_core/omsp_multiplier.v
../openmsp430/msp_core/omsp_sync_reset.v
../openmsp430/msp_core/omsp_sync_cell.v
../openmsp430/msp_core/omsp_scan_mux.v
../openmsp430/msp_core/omsp_and_gate.v
../openmsp430/msp_core/omsp_wakeup_cell.v
../openmsp430/msp_core/omsp_clock_gate.v
../openmsp430/msp_core/omsp_clock_mux.v


//=============================================================================
// Peripherals
//=============================================================================

+incdir+../openmsp430/msp_periph/
../openmsp430/msp_periph/omsp_gpio.v
../openmsp430/msp_periph/omsp_timerA.v
//../../../rtl/verilog/periph/omsp_uart.v
../openmsp430/msp_periph/template_periph_8b.v
../openmsp430/msp_periph/template_periph_16b.v
