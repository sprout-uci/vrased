module  X_stack (
    pc,
    data_addr,
    data_en,
    data_wr,

    reset
);

input   [15:0]  pc;
input   [15:0]  data_addr;
input           data_en;
input           data_wr;
output          reset;

// MACROS ///////////////////////////////////////////
parameter SDATA_BASE = 16'hA000;
parameter SDATA_SIZE = 16'h1000;
//
parameter HMAC_BASE = 16'h8000;
parameter HMAC_SIZE = 16'h001F;
//
parameter CTR_BASE = 16'h9000;
parameter CTR_SIZE = 16'h001F;
//
parameter SMEM_BASE = 16'hE000;
parameter SMEM_SIZE = 16'h1000;
//
parameter KMEM_BASE = 16'hFEFE;
parameter KMEM_SIZE = 16'h001F;
/////////////////////////////////////////////////////


parameter RESET_HANDLER = 16'h0000;
parameter RUN  = 1'b0, KILL = 1'b1;
//-------------Internal Variables---------------------------
reg             state;
reg             key_res;
//

initial
    begin
        state = RUN;
        key_res = 1'b0;
    end

wire pc_not_in_srom = pc < SMEM_BASE || pc > SMEM_BASE + SMEM_SIZE -2;
wire pc_in_srom = !pc_not_in_srom;

wire daddr_not_in_sdata = data_addr < SDATA_BASE || data_addr > SDATA_BASE + SDATA_SIZE -1;
wire daddr_in_sdata = !daddr_not_in_sdata;

wire daddr_not_in_krom = data_addr < KMEM_BASE || data_addr > KMEM_BASE + KMEM_SIZE -1;
wire daddr_in_krom = !daddr_not_in_krom;

wire daddr_not_in_HMAC = data_addr < HMAC_BASE || data_addr > HMAC_BASE + HMAC_SIZE -1;
wire daddr_in_HMAC = !daddr_not_in_HMAC;

wire daddr_not_in_ctr = data_addr < CTR_BASE || data_addr > CTR_BASE + CTR_SIZE -1;
wire daddr_in_ctr = !daddr_not_in_ctr;

wire violation1 = pc_not_in_srom && daddr_in_sdata && (data_en || data_wr);
wire violation2 = pc_in_srom && data_wr && daddr_not_in_sdata && daddr_not_in_HMAC && daddr_not_in_ctr;
wire violation3 = pc_not_in_srom && daddr_in_ctr && data_wr;

// State transition logic//////
always @(*)
if(state == RUN && violation1)
    state <= KILL;
else if (state == RUN && violation2)
    state <= KILL;
else if (state == RUN && violation3)
    state <= KILL;
else if (state == KILL && (pc == RESET_HANDLER) && !violation1 && !violation2 && !violation3)
    state <= RUN;
else state <= state;
//////////////////////////////

// Output logic //////////////
always @(*)
if( 
    (state == RUN && violation1) ||
    (state == RUN && violation2) ||
    (state == RUN && violation3)
)
    key_res <= 1'b1;
else if (state == KILL && pc == RESET_HANDLER && !violation1 && !violation2 && !violation3)
    key_res <= 1'b0;
else if (state == KILL)
    key_res <= 1'b1;
else
    key_res <= 1'b0;

/////////////////////////////

assign reset = key_res;

endmodule
