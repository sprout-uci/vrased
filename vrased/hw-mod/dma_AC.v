
module  dma_AC (
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
//parameter SMEM_BASE = 16'hE000;
//parameter SMEM_SIZE = 16'h1000;
//
parameter KMEM_BASE = 16'hFEFE;
parameter KMEM_SIZE = 16'h001F;
/////////////////////////////////////////////////////



//parameter LAST_SMEM_ADDR = SMEM_BASE + SMEM_SIZE - 2;

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

wire invalid_access_key = (dma_addr >= KMEM_BASE && dma_addr < KMEM_BASE + KMEM_SIZE) && dma_en;

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
