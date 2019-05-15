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
//                      ROM model 
//
// *Author(s):
//              - Olivier Girard,    olgirard@gmail.com
//              - Aurélien Francillon
//

module keyrom (
// OUTPUTs
    rom_dout,                      // ROM data output

// INPUTs
    rom_addr,                      // ROM address
    rom_cen,                       // ROM chip enable (low active)
    rom_clk                       // ROM clock
);

// PARAMETERs
//============
parameter ADDR_MSB   =  4;         // MSB of the address bus
parameter MEM_SIZE   =  20;       // Memory size in bytes

// OUTPUTs
//============
output      [15:0] rom_dout;       // ROM data output

// INPUTs
//============
input [ADDR_MSB:0] rom_addr;       // ROM address
input              rom_cen;        // ROM chip enable (low active)
input              rom_clk;        // ROM clock


// ROM
//============

(* rom_style = "block" *) reg         [15:0] mem [0:(MEM_SIZE/2)-1];
reg   [ADDR_MSB:0] rom_addr_reg;
integer i;

initial
  begin
     //$display("Loading SKEY");
     for(i=0; i<MEM_SIZE/2; i=i+1) begin
	   mem[i] = 16'h0000;
     end
     mem[0] = 16'h0123;
     mem[1] = 16'h4567;
     mem[2] = 16'h89ab;
     mem[3] = 16'hcdef; 
     // Uncomment for Xilinx synthesis
     //$readmemh("skey.mem",mem);
     $display("key: %h %h %h", mem[0], mem[8], mem[16]);
  end

always @(posedge rom_clk)
  if (~rom_cen & rom_addr<(MEM_SIZE/2))
      rom_addr_reg <= rom_addr;

assign rom_dout = mem[rom_addr_reg];
endmodule // keyrom
