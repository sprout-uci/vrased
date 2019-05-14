
module  proof_reset (
    clk,
    pc,
//    data_addr,
//    data_en,
    //pc_en,

    res
);

input		clk;
input   [15:0]  pc;
output          res;

// MACROS ///////////////////////////////////////////
parameter SMEM_BASE = 16'hE000;
parameter SMEM_SIZE = 16'h1000;
parameter FST_POR_ADDR = 16'h1234;
parameter LST_POR_ADDR = 16'h123F;
parameter RESET_HANDLER = 16'hfffe;
parameter RUN  = 2'b00, KILL = 2'b01, PoR = 2'b10;
/////////////////////////////////////////////////////

//-------------Internal Variables---------------------------
reg             [1:0] state;
reg             reset_out;
//



initial
    begin
        state = KILL;
        reset_out = 1'b1;
    end

wire is_fst_PoR = pc == FST_POR_ADDR;
wire is_lst_PoR = pc == LST_POR_ADDR;
wire is_lst_RC  = pc == LAST_SMEM_ADDR;
wire PC_is_zero = pc == RESET_HANDLER;
parameter LAST_SMEM_ADDR = SMEM_BASE + SMEM_SIZE - 2;


always @(posedge clk)
if( state == RUN && is_fst_PoR) 
    state <= PoR;
else if( state == RUN && is_lst_PoR) 
    state <= KILL;
else if (state == PoR && (is_lst_PoR || is_lst_RC))
    state <= KILL;
else if (state == KILL && PC_is_zero)
    state <= RUN;
else state <= state;

always @(posedge clk)
if (state == PoR && (is_lst_PoR || is_lst_RC))
    reset_out <= 1'b1;
else if (state == RUN && is_lst_PoR)
    reset_out <= 1'b1;
else if (state == KILL)
    reset_out <= 1'b1;
else reset_out <= 1'b0;

assign res = reset_out;

endmodule
