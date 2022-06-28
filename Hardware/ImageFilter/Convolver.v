module Convolver #(
    parameter BIT_PER_PIXEL = 8,
    parameter NUM_PIXELS = 9
) (
    input
        clk,
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
    output[7:0]
        conv_out_binarized
);

parameter TMP_WIRE_WIDTH = 10;  // 10 bit is the maximum required width to store the output
wire[TMP_WIRE_WIDTH-1:0] conv_out_tmp;
wire[BIT_PER_PIXEL-1:0] conv_out_binarized_tmp;

assign conv_out_tmp = mode ?
    pixel_1 + pixel_3 - 4*pixel_4 + pixel_5 + pixel_7 :
    pixel_0 + pixel_1 + pixel_2 - pixel_6 - pixel_7 - pixel_8;

// set conv_out_binarized 1 if conv_out_tmp is larger than or equal to 2^8
assign conv_out_binarized_tmp = (|conv_out_tmp[9:8]) ? 8'hff : 8'h0;

DFlipFlop #(.NUM_BITS(8)) d_ff_out (
    .clk (clk),
    .D (conv_out_binarized_tmp),
    .Q (conv_out_binarized)
);

endmodule