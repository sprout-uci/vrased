
module  dma_X_stack (
    clk,
    pc,
    dma_addr,
    dma_en,

    reset,
);

input           clk;
input   [15:0]  pc;
input   [15:0]  dma_addr;
input           dma_en;
output          reset;

// MACROS ///////////////////////////////////////////
parameter SDATA_BASE = 16'hA000;
parameter SDATA_SIZE = 16'h1000;
//
/////////////////////////////////////////////////////

parameter RESET_HANDLER = 16'h0000;
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

wire invalid_access_x_stack = (dma_addr >= SDATA_BASE && dma_addr < SDATA_BASE + SDATA_SIZE) && dma_en;

always @(posedge clk) 
if( state == RUN && invalid_access_x_stack) 
    state <= KILL;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_x_stack)
    state <= RUN;
else state <= state;

always @(posedge clk)
if (state == RUN && invalid_access_x_stack)
    key_res <= 1'b1;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_x_stack)
    key_res <= 1'b0;
else if (state == KILL)
    key_res <= 1'b1;
else if (state == RUN)
    key_res <= 1'b0;
else key_res <= 1'b0;

assign reset = key_res;

endmodule
