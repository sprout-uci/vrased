
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
parameter SDATA_BASE = 16'h400;
parameter SDATA_SIZE = 16'hC00;
//
parameter CTR_BASE = 16'h0270;
parameter CTR_SIZE = 16'h0020;
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
wire invalid_write_ctr = (dma_addr >= CTR_BASE && dma_addr < CTR_BASE + CTR_SIZE) && dma_en;

always @(posedge clk)
if( state == RUN && (invalid_access_x_stack || invalid_write_ctr) )
    state <= KILL;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_x_stack && !invalid_write_ctr)
    state <= RUN;
else state <= state;

always @(posedge clk)
if (state == RUN && ( invalid_access_x_stack || invalid_write_ctr) )
    key_res <= 1'b1;
else if (state == KILL && pc == RESET_HANDLER && !invalid_access_x_stack && !invalid_write_ctr)
    key_res <= 1'b0;
else if (state == KILL)
    key_res <= 1'b1;
else if (state == RUN)
    key_res <= 1'b0;
else key_res <= 1'b0;

assign reset = key_res;

endmodule
