module GrayscaleConverter #(
    parameter BIT_PER_PIXEL = 8,
    parameter NUM_PIXELS = 9
) (
    input
        clk,
        reset,
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
    output reg[BIT_PER_PIXEL-1:0]
        pixel_0_out,
        pixel_1_out,
        pixel_2_out,
        pixel_3_out,
        pixel_4_out,
        pixel_5_out,
        pixel_6_out,
        pixel_7_out,
        pixel_8_out
);

// local parameter definitions
localparam TMP_WIRE_WIDTH = 10;  // 10 bit is the minimum required width to store the sum of three 8 bit numbers

// set of 10 bit wires that temporarily store the value of pixel_red + pixel_green + pixel_blue
wire [TMP_WIRE_WIDTH-1:0] pixelwise_sums [NUM_PIXELS-1:0];
wire [TMP_WIRE_WIDTH-1:0] pixelwise_means [NUM_PIXELS-1:0];

// wires to temporarily store the output value
wire [BIT_PER_PIXEL-1:0] pixel_0_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_1_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_2_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_3_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_4_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_5_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_6_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_7_out_tmp;
wire [BIT_PER_PIXEL-1:0] pixel_8_out_tmp;

// calculate pixel-wise sum
assign pixelwise_sums[0] = pixel_0_red + pixel_0_green + pixel_0_blue;
assign pixelwise_sums[1] = pixel_1_red + pixel_1_green + pixel_1_blue;
assign pixelwise_sums[2] = pixel_2_red + pixel_2_green + pixel_2_blue;
assign pixelwise_sums[3] = pixel_3_red + pixel_3_green + pixel_3_blue;
assign pixelwise_sums[4] = pixel_4_red + pixel_4_green + pixel_4_blue;
assign pixelwise_sums[5] = pixel_5_red + pixel_5_green + pixel_5_blue;
assign pixelwise_sums[6] = pixel_6_red + pixel_6_green + pixel_6_blue;
assign pixelwise_sums[7] = pixel_7_red + pixel_7_green + pixel_7_blue;
assign pixelwise_sums[8] = pixel_8_red + pixel_8_green + pixel_8_blue;

// calculate pixel-wise mean
assign pixelwise_means[0] = pixelwise_sums[0] / 3;
assign pixelwise_means[1] = pixelwise_sums[1] / 3;
assign pixelwise_means[2] = pixelwise_sums[2] / 3;
assign pixelwise_means[3] = pixelwise_sums[3] / 3;
assign pixelwise_means[4] = pixelwise_sums[4] / 3;
assign pixelwise_means[5] = pixelwise_sums[5] / 3;
assign pixelwise_means[6] = pixelwise_sums[6] / 3;
assign pixelwise_means[7] = pixelwise_sums[7] / 3;
assign pixelwise_means[8] = pixelwise_sums[8] / 3;

// assign to the output
assign pixel_0_out_tmp = pixelwise_means[0][BIT_PER_PIXEL-1:0];
assign pixel_1_out_tmp = pixelwise_means[1][BIT_PER_PIXEL-1:0];
assign pixel_2_out_tmp = pixelwise_means[2][BIT_PER_PIXEL-1:0];
assign pixel_3_out_tmp = pixelwise_means[3][BIT_PER_PIXEL-1:0];
assign pixel_4_out_tmp = pixelwise_means[4][BIT_PER_PIXEL-1:0];
assign pixel_5_out_tmp = pixelwise_means[5][BIT_PER_PIXEL-1:0];
assign pixel_6_out_tmp = pixelwise_means[6][BIT_PER_PIXEL-1:0];
assign pixel_7_out_tmp = pixelwise_means[7][BIT_PER_PIXEL-1:0];
assign pixel_8_out_tmp = pixelwise_means[8][BIT_PER_PIXEL-1:0];

always @(posedge clk, posedge reset) begin
    if (reset) begin
        pixel_0_out <= 8'h00;
        pixel_1_out <= 8'h00;
        pixel_2_out <= 8'h00;
        pixel_3_out <= 8'h00;
        pixel_4_out <= 8'h00;
        pixel_5_out <= 8'h00;
        pixel_6_out <= 8'h00;
        pixel_7_out <= 8'h00;
        pixel_8_out <= 8'h00;
    end else begin
        pixel_0_out <= pixel_0_out_tmp;
        pixel_1_out <= pixel_1_out_tmp;
        pixel_2_out <= pixel_2_out_tmp;
        pixel_3_out <= pixel_3_out_tmp;
        pixel_4_out <= pixel_4_out_tmp;
        pixel_5_out <= pixel_5_out_tmp;
        pixel_6_out <= pixel_6_out_tmp;
        pixel_7_out <= pixel_7_out_tmp;
        pixel_8_out <= pixel_8_out_tmp;
    end
end

endmodule