
module  atomicity (
    //clk,    //INPUT
    pc,     //INPUT
    //pc_en,

    reset   //OUTPUT
);

//input		    clk;
input   [15:0]  pc;
//input	    	pc_en;
output          reset;

// FSM States //////////////////////////////////////////////////////////
parameter notRC  = 3'b000;
parameter fstRC = 3'b001;
parameter lastRC = 3'b010;
parameter midRC = 3'b011;
parameter kill = 3'b100;
///////////////////////////////////////////////////////////////////////

parameter RESET_HANDLER = 16'h0000;

// MACROS ///////////////////////////////////////////
parameter SMEM_BASE = 16'hE000;
parameter SMEM_SIZE = 16'h1000;
parameter LAST_SMEM_ADDR = SMEM_BASE + SMEM_SIZE - 2;
/////////////////////////////////////////////////////




reg     [2:0]   pc_state; // 3 bits for 5 states
reg             atomicity_res;

initial
begin
        pc_state = kill;
        atomicity_res = 1'b1;
end

wire is_mid_rom = pc > SMEM_BASE && pc < LAST_SMEM_ADDR;
wire is_first_rom = pc == SMEM_BASE;
wire is_last_rom = pc == LAST_SMEM_ADDR;
wire is_in_rom = is_mid_rom | is_first_rom | is_last_rom;
wire is_outside_rom = pc < SMEM_BASE | pc > LAST_SMEM_ADDR;
always @(*)
begin
    //if(pc_en) begin
    case (pc_state)
        notRC:
            if (is_outside_rom)
                pc_state <= notRC;
            else if (is_first_rom)
                pc_state <= fstRC;
            else if (is_mid_rom || is_last_rom)
                pc_state <= kill;
            else // only necessary if we include if(pc_en)
                pc_state <= pc_state;
        
        midRC:
            if (is_mid_rom)
                pc_state <= midRC;
            else if (is_last_rom)
                pc_state <= lastRC;
            else if (is_outside_rom || is_first_rom)
                pc_state <= kill; 
	    //else if (pc_en && is_first_rom)
		//pc_state = fstRC;
            else
                pc_state <= pc_state;
                
        fstRC:
            if (is_mid_rom) 
                pc_state <= midRC;
            else if (is_first_rom) 
                pc_state = fstRC;
            else if (is_outside_rom || is_first_rom || is_last_rom) 
                pc_state <= kill;
	    //else if (pc_en && is_last_rom)
		//pc_state = lastRC;
            else 
                pc_state <= pc_state;
            
        lastRC:
            if (is_outside_rom)
                pc_state <= notRC;
            else if (is_last_rom) 
                pc_state = lastRC;
	    //else if (pc_en && is_first_rom)
		//pc_state = firstRC;
	    //else if (pc_en && is_mid_rom)
		//pc_state = midRC;
	    else if (is_first_rom || is_last_rom || is_mid_rom)
		      pc_state <= kill;
            else pc_state <= pc_state;
                
        kill:
            if (pc == RESET_HANDLER)
                pc_state <= notRC;
            else
                pc_state <= pc_state;
                
    endcase
    //end
    //else pc_state <= pc_state;
end

////////////// OUTPUT LOGIC //////////////////////////////////////
always @(*)
begin
    if ( /*pc_en &&*/ (
        (pc_state == fstRC && !is_mid_rom && !is_first_rom) ||
        (pc_state == lastRC && !is_outside_rom && !is_last_rom) ||
        (pc_state == midRC && !is_last_rom && !is_mid_rom) ||
        (pc_state == notRC && !is_outside_rom && !is_first_rom)||
	(pc_state == kill && pc != RESET_HANDLER)
       )
    )begin
            atomicity_res <= 1'b1;
    end
    else begin
            atomicity_res <= 1'b0;
    end

end


assign reset = atomicity_res;


endmodule
