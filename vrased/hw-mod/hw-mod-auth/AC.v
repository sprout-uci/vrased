
module  AC (
    clk,
    pc,
    data_addr,
    data_en,
    //pc_en,

    reset,
);

input		clk;
input   [15:0]  pc;
//input		pc_en;
input   [15:0]  data_addr;
input           data_en;
output          reset;

// MACROS ///////////////////////////////////////////
parameter SMEM_BASE = 16'hA000;
parameter SMEM_SIZE = 16'h4000;
//
parameter KMEM_BASE = 16'hFEFE;
parameter KMEM_SIZE = 16'h0040;
/////////////////////////////////////////////////////



parameter LAST_SMEM_ADDR = SMEM_BASE + SMEM_SIZE - 2;

parameter RESET_HANDLER = 16'hfffe;
parameter RUN  = 1'b0, KILL = 1'b1;
//-------------Internal Variables---------------------------
reg             state;
reg             key_res;
//

initial
    begin
        state = KILL;
        key_res = 1'b1;
    end

wire is_mid_rom = pc > SMEM_BASE && pc < LAST_SMEM_ADDR;
wire is_first_rom = pc == SMEM_BASE;
wire is_last_rom = pc == LAST_SMEM_ADDR;
wire is_in_rom = is_mid_rom | is_first_rom | is_last_rom;
wire is_outside_rom = pc < SMEM_BASE | pc > LAST_SMEM_ADDR;

wire access_key = (data_addr >= KMEM_BASE && data_addr < KMEM_BASE + KMEM_SIZE) && data_en;
wire invalid_access_key = is_outside_rom && access_key;

always @(posedge clk)
if( state == RUN && invalid_access_key) 
    state <= KILL;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_key)
    state <= RUN;
else state <= state;

always @(posedge clk)
if (state == RUN && invalid_access_key)
    key_res <= 1'b1;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_key)
    key_res <= 1'b0;
else if (state == KILL)
    key_res <= 1'b1;
else if (state == RUN)
    key_res <= 1'b0;
else key_res <= 1'b0;

assign reset = key_res;

endmodule
