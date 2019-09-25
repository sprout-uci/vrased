//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
// 
// *File Name: tb_openMSP430_fpga.v
// 
// *Module Description:
//                      openMSP430 FPGA testbench
//
// *Author(s):
//              - Olivier Girard,    olgirard@gmail.com
//
//----------------------------------------------------------------------------
// $Rev$
// $LastChangedBy$
// $LastChangedDate$
//----------------------------------------------------------------------------
`include "timescale.v"
`ifdef OMSP_NO_INCLUDE
`else
`include "openMSP430_defines.v"
`endif

module  tb_openMSP430_fpga;

//
// Wire & Register definition
//------------------------------

// Clock & Reset
reg               CLK_100MHz;
reg               RESET;

// Slide Switches
reg               SW7;
reg               SW6;
reg               SW5;
reg               SW4;
reg               SW3;
reg               SW2;
reg               SW1;
reg               SW0;

// Push Button Switches
reg               BTN2;
reg               BTN1;
reg               BTN0;

// LEDs
wire              LED8;
wire              LED7;
wire              LED6;
wire              LED5;
wire              LED4;
wire              LED3;
wire              LED2;
wire              LED1;
wire              LED0;

// Four-Sigit, Seven-Segment LED Display
wire              SEG_A;
wire              SEG_B;
wire              SEG_C;
wire              SEG_D;
wire              SEG_E;
wire              SEG_F;
wire              SEG_G;
wire              SEG_DP;
wire              SEG_AN0;
wire              SEG_AN1;
wire              SEG_AN2;
wire              SEG_AN3;

// UART
reg               UART_RXD;
wire              UART_TXD;

// Core debug signals
wire   [8*32-1:0] i_state;
wire   [8*32-1:0] e_state;
wire       [31:0] inst_cycle;
wire   [8*32-1:0] inst_full;
wire       [31:0] inst_number;
wire       [15:0] inst_pc;
wire   [8*32-1:0] inst_short;

// Testbench variables
integer           i;
integer           error;
reg               stimulus_done;


//
// Include files
//------------------------------

// CPU & Memory registers
//`include "registers.v"

// GPIO
wire         [7:0] p3_din = dut.p3_din;
wire         [7:0] p3_dout = dut.p3_dout;
wire         [7:0] p3_dout_en = dut.p3_dout_en;

wire         [7:0] p1_din = dut.p1_din;
wire         [7:0] p1_dout = dut.p1_dout;
wire         [7:0] p1_dout_en = dut.p1_dout_en;


// RESET SIGNAL
wire         puc_rst = dut.puc_rst;
wire         reset_pin_n = dut.reset_pin_n;

// CPU registers
//======================

wire       [15:0] r0    = dut.openMSP430_0.execution_unit_0.register_file_0.r0;
wire       [15:0] r1    = dut.openMSP430_0.execution_unit_0.register_file_0.r1;
wire       [15:0] r2    = dut.openMSP430_0.execution_unit_0.register_file_0.r2;
wire       [15:0] r3    = dut.openMSP430_0.execution_unit_0.register_file_0.r3;
wire       [15:0] r4    = dut.openMSP430_0.execution_unit_0.register_file_0.r4;
wire       [15:0] r5    = dut.openMSP430_0.execution_unit_0.register_file_0.r5;
wire       [15:0] r6    = dut.openMSP430_0.execution_unit_0.register_file_0.r6;
wire       [15:0] r7    = dut.openMSP430_0.execution_unit_0.register_file_0.r7;
wire       [15:0] r8    = dut.openMSP430_0.execution_unit_0.register_file_0.r8;
wire       [15:0] r9    = dut.openMSP430_0.execution_unit_0.register_file_0.r9;
wire       [15:0] r10   = dut.openMSP430_0.execution_unit_0.register_file_0.r10;
wire       [15:0] r11   = dut.openMSP430_0.execution_unit_0.register_file_0.r11;
wire       [15:0] r12   = dut.openMSP430_0.execution_unit_0.register_file_0.r12;
wire       [15:0] r13   = dut.openMSP430_0.execution_unit_0.register_file_0.r13;
wire       [15:0] r14   = dut.openMSP430_0.execution_unit_0.register_file_0.r14;
wire       [15:0] r15   = dut.openMSP430_0.execution_unit_0.register_file_0.r15;

// RAM cells
//======================

wire       [15:0] srom_cen = dut.openMSP430_0.srom_cen;

wire       [15:0] key200 = dut.openMSP430_0.skey_0.mem[0];
wire       [15:0] key202 = dut.openMSP430_0.skey_0.mem[1];
wire       [15:0] key204 = dut.openMSP430_0.skey_0.mem[2];
wire       [15:0] key206 = dut.openMSP430_0.skey_0.mem[3];


wire       [15:0] mem200 = dut.openMSP430_0.srom_0.mem[0];
wire       [15:0] mem202 = dut.openMSP430_0.srom_0.mem[1];
wire       [15:0] mem204 = dut.openMSP430_0.srom_0.mem[2];
wire       [15:0] mem206 = dut.openMSP430_0.srom_0.mem[3];
wire       [15:0] mem208 = dut.openMSP430_0.srom_0.mem[4];
wire       [15:0] mem20A = dut.openMSP430_0.srom_0.mem[5];
wire       [15:0] mem20C = dut.openMSP430_0.srom_0.mem[6];
wire       [15:0] mem20E = dut.openMSP430_0.srom_0.mem[7];
wire       [15:0] mem210 = dut.openMSP430_0.srom_0.mem[8];
wire       [15:0] mem212 = dut.openMSP430_0.srom_0.mem[9];
wire       [15:0] mem214 = dut.openMSP430_0.srom_0.mem[10];
wire       [15:0] mem216 = dut.openMSP430_0.srom_0.mem[11];
wire       [15:0] mem218 = dut.openMSP430_0.srom_0.mem[12];
wire       [15:0] mem21A = dut.openMSP430_0.srom_0.mem[13];
wire       [15:0] mem21C = dut.openMSP430_0.srom_0.mem[14];
wire       [15:0] mem21E = dut.openMSP430_0.srom_0.mem[15];
wire       [15:0] mem220 = dut.openMSP430_0.srom_0.mem[16];
wire       [15:0] mem222 = dut.openMSP430_0.srom_0.mem[17];
wire       [15:0] mem224 = dut.openMSP430_0.srom_0.mem[18];
wire       [15:0] mem226 = dut.openMSP430_0.srom_0.mem[19];
wire       [15:0] mem228 = dut.openMSP430_0.srom_0.mem[20];
wire       [15:0] mem22A = dut.openMSP430_0.srom_0.mem[21];
wire       [15:0] mem22C = dut.openMSP430_0.srom_0.mem[22];
wire       [15:0] mem22E = dut.openMSP430_0.srom_0.mem[23];
wire       [15:0] mem230 = dut.openMSP430_0.srom_0.mem[24];
wire       [15:0] mem232 = dut.openMSP430_0.srom_0.mem[25];
wire       [15:0] mem234 = dut.openMSP430_0.srom_0.mem[26];
wire       [15:0] mem236 = dut.openMSP430_0.srom_0.mem[27];
wire       [15:0] mem238 = dut.openMSP430_0.srom_0.mem[28];
wire       [15:0] mem23A = dut.openMSP430_0.srom_0.mem[29];
wire       [15:0] mem23C = dut.openMSP430_0.srom_0.mem[30];
wire       [15:0] mem23E = dut.openMSP430_0.srom_0.mem[31];
wire       [15:0] mem240 = dut.openMSP430_0.srom_0.mem[32];
wire       [15:0] mem242 = dut.openMSP430_0.srom_0.mem[33];
wire       [15:0] mem244 = dut.openMSP430_0.srom_0.mem[34];
wire       [15:0] mem246 = dut.openMSP430_0.srom_0.mem[35];
wire       [15:0] mem248 = dut.openMSP430_0.srom_0.mem[36];
wire       [15:0] mem24A = dut.openMSP430_0.srom_0.mem[37];
wire       [15:0] mem24C = dut.openMSP430_0.srom_0.mem[38];
wire       [15:0] mem24E = dut.openMSP430_0.srom_0.mem[39];
wire       [15:0] mem250 = dut.openMSP430_0.srom_0.mem[40];
wire       [15:0] mem252 = dut.openMSP430_0.srom_0.mem[41];
wire       [15:0] mem254 = dut.openMSP430_0.srom_0.mem[42];
wire       [15:0] mem256 = dut.openMSP430_0.srom_0.mem[43];
wire       [15:0] mem258 = dut.openMSP430_0.srom_0.mem[44];
wire       [15:0] mem25A = dut.openMSP430_0.srom_0.mem[45];
wire       [15:0] mem25C = dut.openMSP430_0.srom_0.mem[46];
wire       [15:0] mem25E = dut.openMSP430_0.srom_0.mem[47];
wire       [15:0] mem260 = dut.openMSP430_0.srom_0.mem[48];
wire       [15:0] mem262 = dut.openMSP430_0.srom_0.mem[49];
wire       [15:0] mem264 = dut.openMSP430_0.srom_0.mem[50];
wire       [15:0] mem266 = dut.openMSP430_0.srom_0.mem[51];
wire       [15:0] mem268 = dut.openMSP430_0.srom_0.mem[52];
wire       [15:0] mem26A = dut.openMSP430_0.srom_0.mem[53];
wire       [15:0] mem26C = dut.openMSP430_0.srom_0.mem[54];
wire       [15:0] mem26E = dut.openMSP430_0.srom_0.mem[55];
wire       [15:0] mem270 = dut.openMSP430_0.srom_0.mem[56];
wire       [15:0] mem272 = dut.openMSP430_0.srom_0.mem[57];
wire       [15:0] mem274 = dut.openMSP430_0.srom_0.mem[58];
wire       [15:0] mem276 = dut.openMSP430_0.srom_0.mem[59];
wire       [15:0] mem278 = dut.openMSP430_0.srom_0.mem[60];
wire       [15:0] mem27A = dut.openMSP430_0.srom_0.mem[61];
wire       [15:0] mem27C = dut.openMSP430_0.srom_0.mem[62];
wire       [15:0] mem27E = dut.openMSP430_0.srom_0.mem[63];
// Verilog stimulus
//`include "stimulus.v"

//
// Initialize Program Memory
//------------------------------

////
//// Initialize ROM
////------------------------------
////integer tb_idx;
//initial
//  begin
//     // Initialize data memory
////     for (tb_idx=0; tb_idx < `DMEM_SIZE/2; tb_idx=tb_idx+1)
////        dmem_0.mem[tb_idx] = 16'h0000;

//     // Initialize program memory
//     //$readmemh("smem.mem", dut.openMSP430_0.srom_0.mem);
//     //
//     $readmemh("pmem.mem", dut.openMSP430_0.srom_0.mem);
//  end
  
  

//
// Generate Clock & Reset
//------------------------------
initial
  begin
     CLK_100MHz = 1'b0;
     forever #10 CLK_100MHz <= ~CLK_100MHz; // 100 MHz
  end

initial
  begin
     RESET         = 1'b0;
     #100 RESET    = 1'b1;
     #600 RESET    = 1'b0;
  end

//
// Global initialization
//------------------------------
initial
  begin
     error         = 0;
     stimulus_done = 1;
     SW7           = 1'b0;  // Slide Switches
     SW6           = 1'b0;
     SW5           = 1'b0;
     SW4           = 1'b0;
     SW3           = 1'b0;
     SW2           = 1'b0;
     SW1           = 1'b0;
     SW0           = 1'b0;
     BTN2          = 1'b0;  // Push Button Switches
     BTN1          = 1'b0;
     BTN0          = 1'b0;
    // UART_RXD      = 1'b0;  // UART
  end

//
// openMSP430 FPGA Instance
//----------------------------------

openMSP430_fpga dut (

// Clock Sources
    .CLK_100MHz    (CLK_100MHz),
    //.CLK_SOCKET   (1'b0),

// Slide Switches
    .SW7          (SW7),
    .SW6          (SW6),
    .SW5          (SW5),
    .SW4          (SW4),
    .SW3          (SW3),
    .SW2          (SW2),
    .SW1          (SW1),
    .SW0          (SW0),

// Push Button Switches
    .BTN3         (RESET),
    .BTN2         (BTN2),
    .BTN1         (BTN1),
    .BTN0         (BTN0),

// LEDs
    .LED8         (LED8),
    .LED7         (LED7),
    .LED6         (LED6),
    .LED5         (LED5),
    .LED4         (LED4),
    .LED3         (LED3),
    .LED2         (LED2),
    .LED1         (LED1),
    .LED0         (LED0),

// Four-Sigit, Seven-Segment LED Display
    .SEG_A        (SEG_A),
    .SEG_B        (SEG_B),
    .SEG_C        (SEG_C),
    .SEG_D        (SEG_D),
    .SEG_E        (SEG_E),
    .SEG_F        (SEG_F),
    .SEG_G        (SEG_G),
    .SEG_DP       (SEG_DP),
    .SEG_AN0      (SEG_AN0),
    .SEG_AN1      (SEG_AN1),
    .SEG_AN2      (SEG_AN2),
    .SEG_AN3      (SEG_AN3)
    );

   
//
// Debug utility signals
//----------------------------------------
/*
msp_debug msp_debug_0 (

// OUTPUTs
    .e_state      (e_state),       // Execution state
    .i_state      (i_state),       // Instruction fetch state
    .inst_cycle   (inst_cycle),    // Cycle number within current instruction
    .inst_full    (inst_full),     // Currently executed instruction (full version)
    .inst_number  (inst_number),   // Instruction number since last system reset
    .inst_pc      (inst_pc),       // Instruction Program counter
    .inst_short   (inst_short),    // Currently executed instruction (short version)

// INPUTs
    .mclk         (mclk),          // Main system clock
    .puc_rst      (puc_rst)        // Main system reset
);
*/
//
// Generate Waveform
//----------------------------------------
initial
  begin
   `ifdef VPD_FILE
     $vcdplusfile("tb_openMSP430_fpga.vpd");
     $vcdpluson();
   `else
     `ifdef TRN_FILE
        $recordfile ("tb_openMSP430_fpga.trn");
        $recordvars;
     `else
        $dumpfile("tb_openMSP430_fpga.vcd");
        $dumpvars(0, tb_openMSP430_fpga);
     `endif
   `endif
  end

//
// End of simulation
//----------------------------------------

initial // Timeout
  begin
   `ifdef NO_TIMEOUT
   `else
     `ifdef VERY_LONG_TIMEOUT
       #500000000;
     `else     
     `ifdef LONG_TIMEOUT
       #5000000;
     `else     
       #500000;
     `endif
     `endif
       $display(" ===============================================");
       $display("|               SIMULATION FAILED               |");
       $display("|              (simulation Timeout)             |");
       $display(" ===============================================");
       $finish;
   `endif
  end

initial // Normal end of test
  begin
     @(inst_pc===16'hffff)
     $display(" ===============================================");
     if (error!=0)
       begin
	  $display("|               SIMULATION FAILED               |");
	  $display("|     (some verilog stimulus checks failed)     |");
       end
     else if (~stimulus_done)
       begin
	  $display("|               SIMULATION FAILED               |");
	  $display("|     (the verilog stimulus didn't complete)    |");
       end
     else 
       begin
	  $display("|               SIMULATION PASSED               |");
       end
     $display(" ===============================================");
     $finish;
  end


//
// Tasks Definition
//------------------------------

   task tb_error;
      input [65*8:0] error_string;
      begin
	 $display("ERROR: %s %t", error_string, $time);
	 error = error+1;
      end
   endtask


endmodule
