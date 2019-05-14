module  X_stack (
    clk,
    pc,
    data_addr,
    r_en,
    w_en,
    //pc_en,

    reset
);

input		clk;
input   [15:0]  pc;
//input		pc_en;
input   [15:0]  data_addr;
input           r_en;
input           w_en;
output          reset;

// MACROS ///////////////////////////////////////////
parameter SDATA_BASE = 16'hA000;
parameter SDATA_SIZE = 16'h1000;
//
parameter HMAC_BASE = 16'h8000;
parameter HMAC_SIZE = 16'h001F;
//
parameter SMEM_BASE = 16'hE000;
parameter SMEM_SIZE = 16'h1000;
//
parameter KMEM_BASE = 16'hFEFE;
parameter KMEM_SIZE = 16'h001F;
/////////////////////////////////////////////////////


parameter RESET_HANDLER = 16'hfffe;
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

wire violation1 = pc_not_in_srom && daddr_in_sdata && (r_en || w_en);
wire violation2 = pc_in_srom && w_en && daddr_not_in_sdata && daddr_not_in_HMAC;

// State transition logic//////
always @(posedge clk)
if(state == RUN && pc_not_in_srom && daddr_in_sdata && (r_en || w_en))
    state <= KILL;
else if (state == RUN && pc_in_srom && w_en && daddr_not_in_sdata && daddr_not_in_HMAC)
    state <= KILL;
else if (state == KILL && (pc == RESET_HANDLER) && !violation1 && !violation2)
    state <= RUN;
else state <= state;
//////////////////////////////

// Output logic //////////////
always @(posedge clk)
if( 
    (state == RUN && pc_not_in_srom && daddr_in_sdata && (r_en || w_en)) ||
    (state == RUN && pc_in_srom && w_en && daddr_not_in_sdata && daddr_not_in_HMAC)
)
    key_res <= 1'b1;
else if (state == KILL && pc == RESET_HANDLER && !violation1 && !violation2)
    key_res <= 1'b0;
else if (state == KILL)
    key_res <= 1'b1;
else
    key_res <= 1'b0;

/////////////////////////////

assign reset = key_res;

endmodule
