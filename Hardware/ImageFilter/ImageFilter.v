module ImageFilter # (
    parameter BIT_PER_PIXEL = 8
) (
    input
        clk,
    input wire[BIT_PER_PIXEL-1:0]
        pixel_0_red,
        pixel_0_green,
        pixel_0_blue,
        pixel_1_red,
        pixel_1_green,
        pixel_1_blue,
        pixel_2_red,
        pixel_2_green,
        pixel_2_blue,
        pixel_3_red,
        pixel_3_green,
        pixel_3_blue,
        pixel_4_red,
        pixel_4_green,
        pixel_4_blue,
        pixel_5_red,
        pixel_5_green,
        pixel_5_blue,
        pixel_6_red,
        pixel_6_green,
        pixel_6_blue,
        pixel_7_red,
        pixel_7_green,
        pixel_7_blue,
        pixel_8_red,
        pixel_8_green,
        pixel_8_blue,
    output wire[BIT_PER_PIXEL-1:0]
        filter_out
);


endmodule