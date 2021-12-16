`include "X_stack.v"	
`include "AC.v"	
`include "atomicity.v"	
`include "dma_AC.v"	
`include "dma_detect.v"	
`include "dma_X_stack.v"
`include "proof_reset.v"

`ifdef OMSP_NO_INCLUDE
`else
`include "openMSP430_defines.v"
`endif

module vrased (
    clk,
    pc,
    data_en,
    data_wr,
    data_addr,
    
    dma_addr,
    dma_en,

    irq,
    
    reset
);
input           clk;
input   [15:0]  pc;
input           data_en;
input           data_wr;
input   [15:0]  data_addr;
input   [15:0]  dma_addr;
input           dma_en;
input           irq;
output          reset;

parameter RESET_HANDLER = 16'h0000;

wire    X_stack_reset;
X_stack #(
    .RESET_HANDLER  (RESET_HANDLER)
) X_stack_0 (
    .clk        (clk),
    .pc         (pc),
    .data_addr  (data_addr),
    .r_en       (data_en),
    .w_en       (data_wr),
    .reset      (X_stack_reset)
);

wire    AC_reset;
AC #(
    .RESET_HANDLER  (RESET_HANDLER)
) AC_0 (
    .clk        (clk),
    .pc         (pc),
    .data_addr  (data_addr),
    .data_en    (data_en),
    .reset      (AC_reset)
);

wire    atomicity_reset;
atomicity #(
    .RESET_HANDLER  (RESET_HANDLER)
) atomicity_0 (
    .clk        (clk),
    .pc         (pc),
    .irq        (irq),
    .reset      (atomicity_reset)
);


wire    dma_AC_reset;
dma_AC #(
    .RESET_HANDLER  (RESET_HANDLER)
) dma_AC_0 (
    .clk        (clk),
    .pc         (pc),
    .dma_addr   (dma_addr),
    .dma_en     (dma_en),
    .reset      (dma_AC_reset)
);

wire   dma_detect_reset;
dma_detect #(
    .RESET_HANDLER  (RESET_HANDLER)
) dma_write_detect_0 (
    .clk        (clk),
    .pc         (pc),
    .dma_addr   (dma_addr),
    .dma_en     (dma_en),
	.irq		(irq),
    .reset      (dma_detect_reset) 
);

wire   dma_X_stack_reset;
dma_X_stack #(
    .RESET_HANDLER  (RESET_HANDLER)
) dma_X_stack_0 (
    .clk        (clk),
    .pc         (pc),
    .dma_addr   (dma_addr),
    .dma_en     (dma_en),
    .reset      (dma_X_stack_reset) 
);

assign reset = X_stack_reset | AC_reset | dma_AC_reset | dma_detect_reset | dma_X_stack_reset | atomicity_reset;


endmodule
