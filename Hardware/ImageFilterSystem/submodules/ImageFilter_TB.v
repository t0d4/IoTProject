`timescale 1ns / 1ps

module ImageFilter_TB;
    // signals to connect to an Avalon clock source interface
    reg clk;
    reg reset;
    // signals to connect to an Avalon-MM slave interface
    reg[3:0] address;
    reg read;
    wire[31:0] readdata;
    reg write;
    reg[31:0] writedata;
    wire waitrequest;
    // Non-Avalon Interface IO
    reg[3:0] SW;

parameter CLK_PERIOD = 20;

ImageFilter ImageFilterInstance(
    .clk(clk),
    .reset(reset),
    .address(address),
    .read(read),
    .readdata(readdata),
    .write(write),
    .writedata(writedata),
    .waitrequest(waitrequest),
    .SW(SW)
);

// let's start with resetting modules
initial begin
    clk = 0;
    SW = 4'b0000;
    reset = 1;
    #10 reset = 0;
end

// control clock signal
always begin
    #(CLK_PERIOD/2) clk = 1;
    #(CLK_PERIOD/2) clk = 0;
end

// define tasks for write and read
task avalon_bus_write_test(
    input[3:0] test_write_address,
    input[31:0] test_write_data,
    input test_write
);
    begin
        @(posedge clk);
            #1
            address = test_write_address;  // set address
            write = test_write;            // set write signal
            writedata = test_write_data;  // set data
    end
endtask

task avalon_bus_read_test(
    input[3:0] test_read_address
);
    begin
        @(posedge clk);
            #1;
            read = 1;  // enable read
            address = test_read_address;  // set address
        @(posedge clk);
            #1;
            read = 0;  // disable read
    end
endtask

initial begin
    address = 4'b0;
    read = 0;
    write = 0;
    writedata = 32'b0;
    $timeformat(-9, 2, " ns", 20);

    #110;
    avalon_bus_write_test(4'b0000, {{8'd0},{8'd103},{8'd141},{8'd102}}, 1'b1);
    avalon_bus_write_test(4'b0001, {{8'd0},{8'd106},{8'd112},{8'd109}}, 1'b1);
    avalon_bus_write_test(4'b0010, {{8'd0},{8'd196},{8'd167},{8'd118}}, 1'b1);
    avalon_bus_write_test(4'b0011, {{8'd0},{8'd129},{8'd163},{8'd100}}, 1'b1);
    avalon_bus_write_test(4'b0100, {{8'd0},{8'd142},{8'd111},{8'd120}}, 1'b1);
    avalon_bus_write_test(4'b0101, {{8'd0},{8'd135},{8'd216},{8'd247}}, 1'b1);
    avalon_bus_write_test(4'b0110, {{8'd0},{8'd120},{8'd147},{8'd169}}, 1'b1);
    avalon_bus_write_test(4'b0111, {{8'd0},{8'd171},{8'd142},{8'd173}}, 1'b1);
    avalon_bus_write_test(4'b1000, {{8'd0},{8'd224},{8'd143},{8'd113}}, 1'b1);
    @(posedge clk) begin
        write = 0;  // disable write
    end
    #100;
    avalon_bus_read_test(4'b0000);
    #50;
    $stop;
end

endmodule