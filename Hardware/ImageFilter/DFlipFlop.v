module DFlipFlop #(
    parameter NUM_BITS = 8
) (
    input
        clk,
        sync_reset,
    input wire[NUM_BITS-1:0]
        D,
    output reg[NUM_BITS-1:0]
        Q
);

always @(posedge clk) begin
    if(sync_reset==1'b1) begin
        // Due to the syntax of verilog, we can't avoid hard-coding the number "8" below
        Q <= 8'b0;
    end else begin
        Q <= D;
    end
end

endmodule