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
// *File Name: ram.v
// 
// *Module Description:
//                      Scalable RAM model
//
// *Author(s):
//              - Olivier Girard,    olgirard@gmail.com
//
//----------------------------------------------------------------------------
// $Rev$
// $LastChangedBy$
// $LastChangedDate$
//----------------------------------------------------------------------------

module pmem (

// OUTPUTs
    ram_dout,                      // RAM data output
//    ER_max,         // VAPE
//    OR_max,         // VAPE


// INPUTs
    ram_addr,                      // RAM address
    ram_cen,                       // RAM chip enable (low active)
    ram_clk,                       // RAM clock
    ram_din,                       // RAM data input
    ram_wen                        // RAM write enable (low active)
//    exec_flag
);

// PARAMETERs
//============
parameter ADDR_MSB   =  6;         // MSB of the address bus
parameter MEM_SIZE   =  256;       // Memory size in bytes

// VAPE PARAMETERS
//parameter ER_MAX_addr   =  ((16'hFF00-16'hE000) >> 1);
//parameter OR_MAX_addr   =  ER_MAX_addr + 16'h0001;
//parameter CHAL_addr   =  ER_MAX_addr + 16'h0002;
//parameter EXEC_addr   =  ER_MAX_addr + 16'h0004;


// OUTPUTs
//============
output      [15:0] ram_dout;       // RAM data output
// VAPE
//output      [15:0] ER_max;         // VAPE
//output      [15:0] OR_max;         // VAPE

// INPUTs
//============
input [ADDR_MSB:0] ram_addr;       // RAM address
input              ram_cen;        // RAM chip enable (low active)
input              ram_clk;        // RAM clock
input       [15:0] ram_din;        // RAM data input
input        [1:0] ram_wen;        // RAM write enable (low active)
//input              exec_flag;      // VAPE

// RAM
//============

reg         [15:0] mem [0:(MEM_SIZE/2)-1];

reg   [ADDR_MSB:0] ram_addr_reg;

wire        [15:0] mem_val = mem[ram_addr];

//
// Initialize Program Memory
//------------------------------

initial
   begin
      // Read memory file
      $readmemh("./pmem.mem", mem);
//      mem[ER_MAX_addr] <= 16'hE3AE;
//      mem[OR_MAX_addr] <= 16'hF004;
end

  
always @(posedge ram_clk)
  if (~ram_cen & ram_addr<(MEM_SIZE/2))
    begin
      if      (ram_wen==2'b00) mem[ram_addr] <= ram_din;
      else if (ram_wen==2'b01) mem[ram_addr] <= {ram_din[15:8], mem_val[7:0]};
      else if (ram_wen==2'b10) mem[ram_addr] <= {mem_val[15:8], ram_din[7:0]};
      ram_addr_reg <= ram_addr;

      // VAPE
//      if      (exec_flag == 1'b1) mem[EXEC_addr] <= 16'hFFFF;
//      else    mem[EXEC_addr] <= 16'h0000;
    end

assign ram_dout = mem[ram_addr_reg];

//assign ER_max = mem[ER_MAX_addr];
//assign OR_max = mem[OR_MAX_addr];


endmodule // ram
