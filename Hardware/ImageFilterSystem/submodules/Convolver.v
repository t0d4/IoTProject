module Convolver #(
    parameter BIT_PER_PIXEL = 8,
    parameter NUM_PIXELS = 9
) (
    input
        clk,
        reset,
        mode,
    input wire[BIT_PER_PIXEL-1:0]
        pixel_0,
        pixel_1,
        pixel_2,
        pixel_3,
        pixel_4,
        pixel_5,
        pixel_6,
        pixel_7,
        pixel_8,
    output wire[BIT_PER_PIXEL-1:0]
        conv_out
);

parameter TMP_WIRE_WIDTH = 12;  // 12 bit is the maximum required width to store the output
reg[TMP_WIRE_WIDTH-1:0] conv_out_tmp;
reg[TMP_WIRE_WIDTH-1:0] conv_out_tmp_sum_of_positive;
reg[TMP_WIRE_WIDTH-1:0] conv_out_tmp_sum_of_negative;

// if conv_out_tmp is negative, conv_out will be zero.
// otherwise, if conv_out_tmp is more than 255, conv_out will be 255.
//            if not, conv_out_tmp is ensured to be within [0, 255] so conv_out will be conv_out_tmp itself.
assign conv_out = (conv_out_tmp_sum_of_negative>conv_out_tmp_sum_of_positive) ? 8'h00 :
                  (|conv_out_tmp[TMP_WIRE_WIDTH-1:8]) ? 8'hff : conv_out_tmp[BIT_PER_PIXEL-1:0];

always @(posedge clk, posedge reset) begin
    if (reset) begin
        conv_out_tmp <= 8'b00000000;
    end
    else begin
        if (mode) begin
            conv_out_tmp <= pixel_1 + pixel_3 - 4*pixel_4 + pixel_5 + pixel_7;
            conv_out_tmp_sum_of_positive <= pixel_1 + pixel_3 + pixel_5 + pixel_7;
            conv_out_tmp_sum_of_negative <= 4*pixel_4;
        end
        else begin
            conv_out_tmp <= pixel_0 + 2*pixel_1 + pixel_2 + 2*pixel_3 - 12*pixel_4 + 2*pixel_5 + pixel_6 + 2*pixel_7 + pixel_8;
            conv_out_tmp_sum_of_positive <= pixel_0 + 2*pixel_1 + pixel_2 + 2*pixel_3 + 2*pixel_5 + pixel_6 + 2*pixel_7 + pixel_8;
            conv_out_tmp_sum_of_negative <= 12*pixel_4;
        end
    end
end

endmodule