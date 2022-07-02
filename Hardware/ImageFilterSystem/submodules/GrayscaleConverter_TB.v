`timescale 1 ns / 100 ps

module GrayscaleConverter_TB;

// test input
reg[7:0] pixel_0_red;
reg[7:0] pixel_0_green;
reg[7:0] pixel_0_blue;
reg[7:0] pixel_1_red;
reg[7:0] pixel_1_green;
reg[7:0] pixel_1_blue;
reg[7:0] pixel_2_red;
reg[7:0] pixel_2_green;
reg[7:0] pixel_2_blue;
reg[7:0] pixel_3_red;
reg[7:0] pixel_3_green;
reg[7:0] pixel_3_blue;
reg[7:0] pixel_4_red;
reg[7:0] pixel_4_green;
reg[7:0] pixel_4_blue;
reg[7:0] pixel_5_red;
reg[7:0] pixel_5_green;
reg[7:0] pixel_5_blue;
reg[7:0] pixel_6_red;
reg[7:0] pixel_6_green;
reg[7:0] pixel_6_blue;
reg[7:0] pixel_7_red;
reg[7:0] pixel_7_green;
reg[7:0] pixel_7_blue;
reg[7:0] pixel_8_red;
reg[7:0] pixel_8_green;
reg[7:0] pixel_8_blue;

// output values to examine
wire[7:0] pixel_0_out;
wire[7:0] pixel_1_out;
wire[7:0] pixel_2_out;
wire[7:0] pixel_3_out;
wire[7:0] pixel_4_out;
wire[7:0] pixel_5_out;
wire[7:0] pixel_6_out;
wire[7:0] pixel_7_out;
wire[7:0] pixel_8_out;

// instantiate the module
GrayscaleConverter converter(
    .pixel_0_red (pixel_0_red),
    .pixel_0_green (pixel_0_green),
    .pixel_0_blue (pixel_0_blue),
    .pixel_1_red (pixel_1_red),
    .pixel_1_green (pixel_1_green),
    .pixel_1_blue (pixel_1_blue),
    .pixel_2_red (pixel_2_red),
    .pixel_2_green (pixel_2_green),
    .pixel_2_blue (pixel_2_blue),
    .pixel_3_red (pixel_3_red),
    .pixel_3_green (pixel_3_green),
    .pixel_3_blue (pixel_3_blue),
    .pixel_4_red (pixel_4_red),
    .pixel_4_green (pixel_4_green),
    .pixel_4_blue (pixel_4_blue),
    .pixel_5_red (pixel_5_red),
    .pixel_5_green (pixel_5_green),
    .pixel_5_blue (pixel_5_blue),
    .pixel_6_red (pixel_6_red),
    .pixel_6_green (pixel_6_green),
    .pixel_6_blue (pixel_6_blue),
    .pixel_7_red (pixel_7_red),
    .pixel_7_green (pixel_7_green),
    .pixel_7_blue (pixel_7_blue),
    .pixel_8_red (pixel_8_red),
    .pixel_8_green (pixel_8_green),
    .pixel_8_blue (pixel_8_blue),
    .pixel_0_out (pixel_0_out),
    .pixel_1_out (pixel_1_out),
    .pixel_2_out (pixel_2_out),
    .pixel_3_out (pixel_3_out),
    .pixel_4_out (pixel_4_out),
    .pixel_5_out (pixel_5_out),
    .pixel_6_out (pixel_6_out),
    .pixel_7_out (pixel_7_out),
    .pixel_8_out (pixel_8_out)
)

initial begin
    pixel_0_red = 8'd90
    pixel_0_green = 8'd90
    pixel_0_blue = 8'd90

    pixel_1_red = 8'd90
    pixel_1_green = 8'd90
    pixel_1_blue = 8'd90

    pixel_2_red = 8'd90
    pixel_2_green = 8'd90
    pixel_2_blue = 8'd90

    pixel_3_red = 8'd90
    pixel_3_green = 8'd90
    pixel_3_blue = 8'd90

    pixel_4_red = 8'd90
    pixel_4_green = 8'd90
    pixel_4_blue = 8'd90

    pixel_5_red = 8'd90
    pixel_5_green = 8'd90
    pixel_5_blue = 8'd90

    pixel_6_red = 8'd90
    pixel_6_green = 8'd90
    pixel_6_blue = 8'd90

    pixel_7_red = 8'd90
    pixel_7_green = 8'd90
    pixel_7_blue = 8'd90

    pixel_8_red = 8'd90
    pixel_8_green = 8'd90
    pixel_8_blue = 8'd90

    #100 $finish;
end

endmodule