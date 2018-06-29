`timescale 1ns / 1ps
module measureT(
    clk,
    rst,//-
	gateIn,
	mode, // 0:hard 1:soft
    data_in,//signed 12bit
    T
    );
    input wire clk;
    input wire rst;
	input wire gateIn;
	input wire mode;
    input wire signed [11:0] data_in;
    output reg[31:0] T = 'd0;
    //迟滞比较器
    reg gate;
    reg gate_l;
    reg gateSoft;
 always@(posedge clk) begin
     if(rst) gateSoft<= 1'b0;
     else if(data_in > 11'sd400) gateSoft <= 1'b1;
     else if(data_in < -11'sd400) gateSoft <= 1'b0;
 end 
     	
    always@(posedge clk) begin
        if(rst) gate_l<= 1'b0;
        else begin
            if(mode) gate <= gateSoft;
            else gate <= gateIn;
            gate_l <= gate;
        end
    end 
	
    //脉冲提取
    wire PULSE;
    assign PULSE = ({gate,gate_l} == 2'b10);
    //state 
    reg state = 'd0;
    reg[31:0] Tcnt = 'd0;
    always@(posedge clk) begin
        if(PULSE) begin
            T <= Tcnt + 1'd1;
            Tcnt <= 'd0;
        end
        else Tcnt <= Tcnt + 1'd1;   
    end
endmodule
