module ImageFilter #(
    parameter BIT_PER_PIXEL = 8,
    parameter NUM_PIXELS = 9
) (
    // signals to connect to an Avalon clock source interface
    input clk,
    input reset,
    // signals to connect to an Avalon-MM slave interface
    input wire[3:0] address,
    input read,
    output wire[31:0] readdata,
    input write,
    input wire[31:0] writedata,
    output reg waitrequest,
    // Non-Avalon Interface IO
    input wire[3:0] SW,
    output wire[6:0] fpga_led_internal
);

// define names for local addresses (in the address space of this peripheral) that are to be designated by DMAC
localparam PIXEL_0_ADDR = 4'b0000;
localparam PIXEL_1_ADDR = 4'b0001;
localparam PIXEL_2_ADDR = 4'b0010;
localparam PIXEL_3_ADDR = 4'b0011;
localparam PIXEL_4_ADDR = 4'b0100;
localparam PIXEL_5_ADDR = 4'b0101;
localparam PIXEL_6_ADDR = 4'b0110;
localparam PIXEL_7_ADDR = 4'b0111;
localparam PIXEL_8_ADDR = 4'b1000;

// state names
localparam IDLE = 3'b000;
localparam READING = 3'b001;
localparam INPUTTING = 3'b010;
localparam GRAYSCALING = 3'b011;
localparam CONVOLVING = 3'b100;
localparam OUTPUTREADY = 3'b101;

// register to store current state
reg [2:0] state;
assign fpga_led_internal = {{4'b0}, state};

// registers to temporarily hold the pixel data
reg[BIT_PER_PIXEL-1:0] pixel_0_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_0_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_0_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_1_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_1_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_1_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_2_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_2_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_2_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_3_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_3_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_3_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_4_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_4_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_4_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_5_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_5_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_5_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_6_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_6_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_6_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_7_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_7_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_7_blue_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_8_red_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_8_green_tmp;
reg[BIT_PER_PIXEL-1:0] pixel_8_blue_tmp;

// registers to simultaneously input the pixel data to grayscale module
reg[BIT_PER_PIXEL-1:0] pixel_0_red;
reg[BIT_PER_PIXEL-1:0] pixel_0_green;
reg[BIT_PER_PIXEL-1:0] pixel_0_blue;
reg[BIT_PER_PIXEL-1:0] pixel_1_red;
reg[BIT_PER_PIXEL-1:0] pixel_1_green;
reg[BIT_PER_PIXEL-1:0] pixel_1_blue;
reg[BIT_PER_PIXEL-1:0] pixel_2_red;
reg[BIT_PER_PIXEL-1:0] pixel_2_green;
reg[BIT_PER_PIXEL-1:0] pixel_2_blue;
reg[BIT_PER_PIXEL-1:0] pixel_3_red;
reg[BIT_PER_PIXEL-1:0] pixel_3_green;
reg[BIT_PER_PIXEL-1:0] pixel_3_blue;
reg[BIT_PER_PIXEL-1:0] pixel_4_red;
reg[BIT_PER_PIXEL-1:0] pixel_4_green;
reg[BIT_PER_PIXEL-1:0] pixel_4_blue;
reg[BIT_PER_PIXEL-1:0] pixel_5_red;
reg[BIT_PER_PIXEL-1:0] pixel_5_green;
reg[BIT_PER_PIXEL-1:0] pixel_5_blue;
reg[BIT_PER_PIXEL-1:0] pixel_6_red;
reg[BIT_PER_PIXEL-1:0] pixel_6_green;
reg[BIT_PER_PIXEL-1:0] pixel_6_blue;
reg[BIT_PER_PIXEL-1:0] pixel_7_red;
reg[BIT_PER_PIXEL-1:0] pixel_7_green;
reg[BIT_PER_PIXEL-1:0] pixel_7_blue;
reg[BIT_PER_PIXEL-1:0] pixel_8_red;
reg[BIT_PER_PIXEL-1:0] pixel_8_green;
reg[BIT_PER_PIXEL-1:0] pixel_8_blue;

// wires to transfer the output of grayscale_converter to convolver
wire[BIT_PER_PIXEL-1:0] pixel_0_out;
wire[BIT_PER_PIXEL-1:0] pixel_1_out;
wire[BIT_PER_PIXEL-1:0] pixel_2_out;
wire[BIT_PER_PIXEL-1:0] pixel_3_out;
wire[BIT_PER_PIXEL-1:0] pixel_4_out;
wire[BIT_PER_PIXEL-1:0] pixel_5_out;
wire[BIT_PER_PIXEL-1:0] pixel_6_out;
wire[BIT_PER_PIXEL-1:0] pixel_7_out;
wire[BIT_PER_PIXEL-1:0] pixel_8_out;

/////////////////////////////////////////
//  STATIC CONNECTIONS OF THE MODULES  //
/////////////////////////////////////////

GrayscaleConverter #(
    .BIT_PER_PIXEL (BIT_PER_PIXEL),
    .NUM_PIXELS (NUM_PIXELS)
) grayscale_converter(
    .clk (clk),
    .reset (reset),
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
);

// wire to transfer the output of convolver (8bit)
wire[BIT_PER_PIXEL-1:0] conv_out;

Convolver #(
    .BIT_PER_PIXEL (BIT_PER_PIXEL),
    .NUM_PIXELS (NUM_PIXELS)
) convolver(
    .clk (clk),
    .reset (reset),
    .mode (SW[0]),
    .pixel_0 (pixel_0_out),
    .pixel_1 (pixel_1_out),
    .pixel_2 (pixel_2_out),
    .pixel_3 (pixel_3_out),
    .pixel_4 (pixel_4_out),
    .pixel_5 (pixel_5_out),
    .pixel_6 (pixel_6_out),
    .pixel_7 (pixel_7_out),
    .pixel_8 (pixel_8_out),
    .conv_out (conv_out)
);

// connect the output of convolver to the output of this module
assign readdata = {{3{8'h00}}, conv_out};

//////////////////////////////////////
//  DYNAMIC BEHAVIOR OF THE MODULE  //
//////////////////////////////////////

// behaviors for each state
always @(posedge clk, posedge reset) begin
    if (reset) begin
        state <= IDLE;
        pixel_0_red <= 8'h00;
        pixel_0_green <= 8'h00;
        pixel_0_blue <= 8'h00;
        pixel_1_red <= 8'h00;
        pixel_1_green <= 8'h00;
        pixel_1_blue <= 8'h00;
        pixel_2_red <= 8'h00;
        pixel_2_green <= 8'h00;
        pixel_2_blue <= 8'h00;
        pixel_3_red <= 8'h00;
        pixel_3_green <= 8'h00;
        pixel_3_blue <= 8'h00;
        pixel_4_red <= 8'h00;
        pixel_4_green <= 8'h00;
        pixel_4_blue <= 8'h00;
        pixel_5_red <= 8'h00;
        pixel_5_green <= 8'h00;
        pixel_5_blue <= 8'h00;
        pixel_6_red <= 8'h00;
        pixel_6_green <= 8'h00;
        pixel_6_blue <= 8'h00;
        pixel_7_red <= 8'h00;
        pixel_7_green <= 8'h00;
        pixel_7_blue <= 8'h00;
        pixel_8_red <= 8'h00;
        pixel_8_green <= 8'h00;
        pixel_8_blue <= 8'h00;
        pixel_0_red_tmp <= 8'h00;
        pixel_0_green_tmp <= 8'h00;
        pixel_0_blue_tmp <= 8'h00;
        pixel_1_red_tmp <= 8'h00;
        pixel_1_green_tmp <= 8'h00;
        pixel_1_blue_tmp <= 8'h00;
        pixel_2_red_tmp <= 8'h00;
        pixel_2_green_tmp <= 8'h00;
        pixel_2_blue_tmp <= 8'h00;
        pixel_3_red_tmp <= 8'h00;
        pixel_3_green_tmp <= 8'h00;
        pixel_3_blue_tmp <= 8'h00;
        pixel_4_red_tmp <= 8'h00;
        pixel_4_green_tmp <= 8'h00;
        pixel_4_blue_tmp <= 8'h00;
        pixel_5_red_tmp <= 8'h00;
        pixel_5_green_tmp <= 8'h00;
        pixel_5_blue_tmp <= 8'h00;
        pixel_6_red_tmp <= 8'h00;
        pixel_6_green_tmp <= 8'h00;
        pixel_6_blue_tmp <= 8'h00;
        pixel_7_red_tmp <= 8'h00;
        pixel_7_green_tmp <= 8'h00;
        pixel_7_blue_tmp <= 8'h00;
        pixel_8_red_tmp <= 8'h00;
        pixel_8_green_tmp <= 8'h00;
        pixel_8_blue_tmp <= 8'h00;
        waitrequest <= 0;
    end
    else begin
        case (state)
            IDLE: begin
                if (write) begin
                    // read pixel_0 and move state to "READING".
                    // if we just write "state <= READING" here,
                    // the state transfer will take 1 clock cycle
                    // so the data of pixel_0 may be lost without
                    // being saved to any register (hypothesis)
                    pixel_0_red_tmp <= writedata[7:0];
                    pixel_0_green_tmp <= writedata[15:8];
                    pixel_0_blue_tmp <= writedata[23:16];
                    state <= READING;
                end
                else begin
                    state <= IDLE;
                end
            end

            READING: begin
                if (write) begin
                    case (address)
                        // PIXEL_0_ADDR: begin
                        //     pixel_0_red_tmp <= writedata[7:0];
                        //     pixel_0_green_tmp <= writedata[15:8];
                        //     pixel_0_blue_tmp <= writedata[23:16];
                        //     state <= READING;
                        // end
                        PIXEL_1_ADDR: begin
                            pixel_1_red_tmp <= writedata[7:0];
                            pixel_1_green_tmp <= writedata[15:8];
                            pixel_1_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_2_ADDR: begin
                            pixel_2_red_tmp <= writedata[7:0];
                            pixel_2_green_tmp <= writedata[15:8];
                            pixel_2_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_3_ADDR: begin
                            pixel_3_red_tmp <= writedata[7:0];
                            pixel_3_green_tmp <= writedata[15:8];
                            pixel_3_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_4_ADDR: begin
                            pixel_4_red_tmp <= writedata[7:0];
                            pixel_4_green_tmp <= writedata[15:8];
                            pixel_4_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_5_ADDR: begin
                            pixel_5_red_tmp <= writedata[7:0];
                            pixel_5_green_tmp <= writedata[15:8];
                            pixel_5_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_6_ADDR: begin
                            pixel_6_red_tmp <= writedata[7:0];
                            pixel_6_green_tmp <= writedata[15:8];
                            pixel_6_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_7_ADDR: begin
                            pixel_7_red_tmp <= writedata[7:0];
                            pixel_7_green_tmp <= writedata[15:8];
                            pixel_7_blue_tmp <= writedata[23:16];
                            state <= READING;
                        end
                        PIXEL_8_ADDR: begin
                            pixel_8_red_tmp <= writedata[7:0];
                            pixel_8_green_tmp <= writedata[15:8];
                            pixel_8_blue_tmp <= writedata[23:16];
                            state <= INPUTTING;
                        end

                        // when an unexpected address is specified
                        default: begin
                            state <= IDLE;
                        end
                    endcase
                end
            end

            INPUTTING: begin
                pixel_0_red <= pixel_0_red_tmp;
                pixel_0_green <= pixel_0_green_tmp;
                pixel_0_blue <= pixel_0_blue_tmp;
                pixel_1_red <= pixel_1_red_tmp;
                pixel_1_green <= pixel_1_green_tmp;
                pixel_1_blue <= pixel_1_blue_tmp;
                pixel_2_red <= pixel_2_red_tmp;
                pixel_2_green <= pixel_2_green_tmp;
                pixel_2_blue <= pixel_2_blue_tmp;
                pixel_3_red <= pixel_3_red_tmp;
                pixel_3_green <= pixel_3_green_tmp;
                pixel_3_blue <= pixel_3_blue_tmp;
                pixel_4_red <= pixel_4_red_tmp;
                pixel_4_green <= pixel_4_green_tmp;
                pixel_4_blue <= pixel_4_blue_tmp;
                pixel_5_red <= pixel_5_red_tmp;
                pixel_5_green <= pixel_5_green_tmp;
                pixel_5_blue <= pixel_5_blue_tmp;
                pixel_6_red <= pixel_6_red_tmp;
                pixel_6_green <= pixel_6_green_tmp;
                pixel_6_blue <= pixel_6_blue_tmp;
                pixel_7_red <= pixel_7_red_tmp;
                pixel_7_green <= pixel_7_green_tmp;
                pixel_7_blue <= pixel_7_blue_tmp;
                pixel_8_red <= pixel_8_red_tmp;
                pixel_8_green <= pixel_8_green_tmp;
                pixel_8_blue <= pixel_8_blue_tmp;
                state <= GRAYSCALING;
                waitrequest <= 1'b1;
            end

            GRAYSCALING: begin
                state <= CONVOLVING;
            end

            CONVOLVING: begin
                state <= OUTPUTREADY;
            end

            OUTPUTREADY: begin
                waitrequest <= 1'b0;
                if (read) begin
                    state <= IDLE;
                end
                else begin
                    state <= OUTPUTREADY;
                end
            end

            default: begin
                state <= IDLE;
            end
        endcase
    end
end

endmodule