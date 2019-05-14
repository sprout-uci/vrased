









































`define OMSP_NO_INCLUDE
`ifdef OMSP_NO_INCLUDE
`else
`include "openMSP430_undefines.v"
`endif

























`define PMEM_SIZE_8_KB










`define DMEM_SIZE_24_KB















`define SMEM_SIZE_16_KB


`define SKEY_SIZE 64
`define SKEY_BASE 16'h6A00
`define SKEY_AWIDTH 5



`define MULTIPLIER



`define DBG_EN
















`define USER_VERSION 5'b00000











`define WATCHDOG





`define DMA_IF_EN





`define NMI








`define IRQ_16




















`define SYNC_NMI






















`define PER_SIZE_512_B
































`define DBG_UART




























































`define PMEM_CUSTOM_AWIDTH      10
`define PMEM_CUSTOM_SIZE      2048


`define DMEM_CUSTOM_AWIDTH       6
`define DMEM_CUSTOM_SIZE       128


`define PER_CUSTOM_AWIDTH        8
`define PER_CUSTOM_SIZE        512




















`ifdef ASIC









`define CLOCK_GATING















`ifdef ASIC_CLOCKING









`define LFXT_DOMAIN














`define MCLK_MUX









`define SMCLK_MUX











`define WATCHDOG_MUX













`define MCLK_DIVIDER







`define SMCLK_DIVIDER







`define ACLK_DIVIDER













`define CPUOFF_EN









`define SCG0_EN








`define SCG1_EN










`define OSCOFF_EN














`endif
`endif
















`ifdef PMEM_SIZE_59_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     60416
`endif
`ifdef PMEM_SIZE_55_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     56320
`endif
`ifdef PMEM_SIZE_54_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     55296
`endif
`ifdef PMEM_SIZE_51_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     52224
`endif
`ifdef PMEM_SIZE_48_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     49152
`endif
`ifdef PMEM_SIZE_41_KB
  `define PMEM_AWIDTH      15
  `define PMEM_SIZE     41984
`endif
`ifdef PMEM_SIZE_32_KB
  `define PMEM_AWIDTH      14
  `define PMEM_SIZE     32768
`endif
`ifdef PMEM_SIZE_24_KB
  `define PMEM_AWIDTH      14
  `define PMEM_SIZE     24576
`endif
`ifdef PMEM_SIZE_16_KB
  `define PMEM_AWIDTH      13
  `define PMEM_SIZE     16384
`endif
`ifdef PMEM_SIZE_12_KB
  `define PMEM_AWIDTH      13
  `define PMEM_SIZE     12288
`endif
`ifdef PMEM_SIZE_8_KB
  `define PMEM_AWIDTH      12
  `define PMEM_SIZE      8192
`endif
`ifdef PMEM_SIZE_4_KB
  `define PMEM_AWIDTH      11
  `define PMEM_SIZE      4096
`endif
`ifdef PMEM_SIZE_2_KB
  `define PMEM_AWIDTH      10
  `define PMEM_SIZE      2048
`endif
`ifdef PMEM_SIZE_1_KB
  `define PMEM_AWIDTH       9
  `define PMEM_SIZE      1024
`endif
`ifdef PMEM_SIZE_CUSTOM
  `define PMEM_AWIDTH       `PMEM_CUSTOM_AWIDTH
  `define PMEM_SIZE         `PMEM_CUSTOM_SIZE
`endif


`ifdef DMEM_SIZE_32_KB
  `define DMEM_AWIDTH       14
  `define DMEM_SIZE      32768
`endif
`ifdef DMEM_SIZE_24_KB
  `define DMEM_AWIDTH       14
  `define DMEM_SIZE      24576
`endif
`ifdef DMEM_SIZE_16_KB
  `define DMEM_AWIDTH       13
  `define DMEM_SIZE      16384
`endif
`ifdef DMEM_SIZE_10_KB
  `define DMEM_AWIDTH       13
  `define DMEM_SIZE      10240
`endif
`ifdef DMEM_SIZE_8_KB
  `define DMEM_AWIDTH       12
  `define DMEM_SIZE       8192
`endif
`ifdef DMEM_SIZE_5_KB
  `define DMEM_AWIDTH       12
  `define DMEM_SIZE       5120
`endif
`ifdef DMEM_SIZE_4_KB
  `define DMEM_AWIDTH       11
  `define DMEM_SIZE       4096
`endif
`ifdef DMEM_SIZE_2p5_KB
  `define DMEM_AWIDTH       11
  `define DMEM_SIZE       2560
`endif
`ifdef DMEM_SIZE_2_KB
  `define DMEM_AWIDTH       10
  `define DMEM_SIZE       2048
`endif
`ifdef DMEM_SIZE_1_KB
  `define DMEM_AWIDTH        9
  `define DMEM_SIZE       1024
`endif
`ifdef DMEM_SIZE_512_B
  `define DMEM_AWIDTH        8
  `define DMEM_SIZE        512
`endif
`ifdef DMEM_SIZE_256_B
  `define DMEM_AWIDTH        7
  `define DMEM_SIZE        256
`endif
`ifdef DMEM_SIZE_128_B
  `define DMEM_AWIDTH        6
  `define DMEM_SIZE        128
`endif
`ifdef DMEM_SIZE_CUSTOM
  `define DMEM_AWIDTH       `DMEM_CUSTOM_AWIDTH
  `define DMEM_SIZE         `DMEM_CUSTOM_SIZE
`endif


`ifdef PER_SIZE_32_KB
  `define PER_AWIDTH        14
  `define PER_SIZE       32768
`endif
`ifdef PER_SIZE_16_KB
  `define PER_AWIDTH        13
  `define PER_SIZE       16384
`endif
`ifdef PER_SIZE_8_KB
  `define PER_AWIDTH        12
  `define PER_SIZE        8192
`endif
`ifdef PER_SIZE_4_KB
  `define PER_AWIDTH        11
  `define PER_SIZE        4096
`endif
`ifdef PER_SIZE_2_KB
  `define PER_AWIDTH        10
  `define PER_SIZE        2048
`endif
`ifdef PER_SIZE_1_KB
  `define PER_AWIDTH         9
  `define PER_SIZE        1024
`endif
`ifdef PER_SIZE_512_B
  `define PER_AWIDTH         8
  `define PER_SIZE         512
`endif
`ifdef PER_SIZE_CUSTOM
  `define PER_AWIDTH        `PER_CUSTOM_AWIDTH
  `define PER_SIZE          `PER_CUSTOM_SIZE
`endif








`ifdef SMEM_SIZE_32_KB
  `define SMEM_AWIDTH      14
  `define SMEM_SIZE     32768
`endif

`ifdef SMEM_SIZE_16_KB
  `define SMEM_AWIDTH      13
  `define SMEM_SIZE     16384
`endif


`define DMEM_BASE  `PER_SIZE
`define PMEM_OFFSET (16'hFFFF - `PMEM_SIZE + 1)
`define SMEM_BASE   (16'hFFFF - `PMEM_SIZE - `SMEM_SIZE + 1)


`define PMEM_MSB   `PMEM_AWIDTH-1
`define DMEM_MSB   `DMEM_AWIDTH-1
`define PER_MSB    `PER_AWIDTH-1
`define SMEM_MSB    `SMEM_AWIDTH-1
`define SKEY_MSB    `SKEY_AWIDTH-1


`ifdef  IRQ_16
`define IRQ_NR 16
`endif
`ifdef  IRQ_32
`define IRQ_NR 32
`define IRQ_NR_GE_32
`endif
`ifdef  IRQ_64
`define IRQ_NR 64
`define IRQ_NR_GE_32
`endif






`define INST_SO  0
`define INST_JMP 1
`define INST_TO  2


`define RRC    0
`define SWPB   1
`define RRA    2
`define SXT    3
`define PUSH   4
`define CALL   5
`define RETI   6
`define IRQ    7


`define JNE    0
`define JEQ    1
`define JNC    2
`define JC     3
`define JN     4
`define JGE    5
`define JL     6
`define JMP    7


`define MOV    0
`define ADD    1
`define ADDC   2
`define SUBC   3
`define SUB    4
`define CMP    5
`define DADD   6
`define BIT    7
`define BIC    8
`define BIS    9
`define XOR   10
`define AND   11


`define DIR      0
`define IDX      1
`define INDIR    2
`define INDIR_I  3
`define SYMB     4
`define IMM      5
`define ABS      6
`define CONST    7


`define I_IRQ_FETCH 3'h0
`define I_IRQ_DONE  3'h1
`define I_DEC       3'h2
`define I_EXT1      3'h3
`define I_EXT2      3'h4
`define I_IDLE      3'h5



`define E_IRQ_0     4'h2
`define E_IRQ_1     4'h1
`define E_IRQ_2     4'h0
`define E_IRQ_3     4'h3
`define E_IRQ_4     4'h4
`define E_SRC_AD    4'h5
`define E_SRC_RD    4'h6
`define E_SRC_WR    4'h7
`define E_DST_AD    4'h8
`define E_DST_RD    4'h9
`define E_DST_WR    4'hA
`define E_EXEC      4'hB
`define E_JUMP      4'hC
`define E_IDLE      4'hD


`define ALU_SRC_INV   0
`define ALU_INC       1
`define ALU_INC_C     2
`define ALU_ADD       3
`define ALU_AND       4
`define ALU_OR        5
`define ALU_XOR       6
`define ALU_DADD      7
`define ALU_STAT_7    8
`define ALU_STAT_F    9
`define ALU_SHIFT    10
`define EXEC_NO_WR   11


`define DBG_UART_WR   18
`define DBG_UART_BW   17
`define DBG_UART_ADDR 16:11


`define HALT        0
`define RUN         1
`define ISTEP       2
`define SW_BRK_EN   3
`define FRZ_BRK_EN  4
`define RST_BRK_EN  5
`define CPU_RST     6


`define HALT_RUN    0
`define PUC_PND     1
`define SWBRK_PND   3
`define HWBRK0_PND  4
`define HWBRK1_PND  5


`define BRK_MODE_RD 0
`define BRK_MODE_WR 1
`define BRK_MODE    1:0
`define BRK_EN      2
`define BRK_I_EN    3
`define BRK_RANGE   4


`define DIVAx       5:4
`define DMA_CPUOFF  0
`define DMA_OSCOFF  1
`define DMA_SCG0    2
`define DMA_SCG1    3


`define SELMx       7
`define DIVMx       5:4
`define SELS        3
`define DIVSx       2:1


`ifdef CPUOFF_EN
  `define MCLK_CGATE
`else
`ifdef MCLK_DIVIDER
  `define MCLK_CGATE
`endif
`endif


`ifdef SCG1_EN
  `define SMCLK_CGATE
`else
`ifdef SMCLK_DIVIDER
  `define SMCLK_CGATE
`endif
`endif









`define CPU_VERSION   3'h3


`define DBG_SWBRK_OP 16'h4343





`define DBG_UART_AUTO_SYNC















`define DBG_UART_BAUD 2000000
`define DBG_DCO_FREQ  20000000
`define DBG_UART_CNT ((`DBG_DCO_FREQ/`DBG_UART_BAUD)-1)


`define SYNC_DBG_UART_RXD


`ifdef DBG_HWBRK_RANGE
 `define HWBRK_RANGE 1'b1
`else
 `define HWBRK_RANGE 1'b0
`endif


`define DBG_UART_XFER_CNT_W 16


`ifdef DBG_EN
 `ifdef DBG_UART
   `ifdef DBG_I2C
CONFIGURATION ERROR: I2C AND UART DEBUG INTERFACE ARE BOTH ENABLED
   `endif
 `else
   `ifdef DBG_I2C
   `else
CONFIGURATION ERROR: I2C OR UART DEBUG INTERFACE SHOULD BE ENABLED
   `endif
 `endif
`endif














`ifdef  IRQ_16
  `ifdef  IRQ_32
CONFIGURATION ERROR: ONLY ONE OF THE IRQ NUMBER OPTION CAN BE SELECTED
  `endif
  `ifdef  IRQ_64
CONFIGURATION ERROR: ONLY ONE OF THE IRQ NUMBER OPTION CAN BE SELECTED
  `endif
`endif
`ifdef  IRQ_32
  `ifdef  IRQ_64
CONFIGURATION ERROR: ONLY ONE OF THE IRQ NUMBER OPTION CAN BE SELECTED
  `endif
`endif
`ifdef LFXT_DOMAIN
`else
 `ifdef MCLK_MUX
CONFIGURATION ERROR: THE MCLK_MUX CAN ONLY BE ENABLED IF THE LFXT_DOMAIN IS ENABLED AS WELL
 `endif
 `ifdef SMCLK_MUX
CONFIGURATION ERROR: THE SMCLK_MUX CAN ONLY BE ENABLED IF THE LFXT_DOMAIN IS ENABLED AS WELL
 `endif
 `ifdef WATCHDOG_MUX
CONFIGURATION ERROR: THE WATCHDOG_MUX CAN ONLY BE ENABLED IF THE LFXT_DOMAIN IS ENABLED AS WELL
 `else
   `ifdef WATCHDOG_NOMUX_ACLK
CONFIGURATION ERROR: THE WATCHDOG_NOMUX_ACLK CAN ONLY BE ENABLED IF THE LFXT_DOMAIN IS ENABLED AS WELL
   `endif
 `endif
 `ifdef OSCOFF_EN
CONFIGURATION ERROR: THE OSCOFF LOW POWER MODE CAN ONLY BE ENABLED IF THE LFXT_DOMAIN IS ENABLED AS WELL
 `endif
`endif