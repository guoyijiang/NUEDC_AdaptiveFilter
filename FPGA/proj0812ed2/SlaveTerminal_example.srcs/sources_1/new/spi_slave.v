//////////////////////////////////////////////////////////////////////////////////
// Create Date: 2017/05/21 22:30:13
// Design Name: GYJ
// Module Name: my_spi_slave
// Description: 
// 
//////////////////////////////////////////////////////////////////////////////////
`timescale 1ns/1ps
`define spi_working spi_state[0]
`define spi_recv_data spi_state[1]

/*
CS	----\__20ns_________________________________/----30ns----\
SCK	____________/-1-\__/-2-\__  ...  __/-16-\_______________
?OUT@(posedge SCK)
?shifting after (posedge SCK)
?working@(negedge SCK)

t_sck > (2+2)*10ns
need 3*10ns to ready for next transform
*/
module my_spi_slave(
 	input wire clk,
	input wire rst,
	
	input wire [15:0]SEND_BUF,
	output wire [15: 0] RECEIVE_BUF,
	output reg [1:0] spi_state,

	input wire SCK,
	input wire CS,
	input wire MOSI,
	output  wire MISO
	);
	
	reg [15:0]receive_buf,shift_buf;
	
	//syn CS
	reg [1:0]cs_syn = 2'b11;
	wire CS_U,CS_D;
	always@(posedge clk) begin
		if(~rst) cs_syn <= 2'b11;
		else begin
			cs_syn <= {cs_syn[0],CS};	
		end	
	end
	assign CS_U = (cs_syn == 2'b01);
	assign CS_D = (cs_syn == 2'b10);
	
	//syn SCK
	reg [1:0]sck_syn = 2'b00;
	wire SCK_U,SCK_D;
	always@(posedge clk) begin
		if(~rst) sck_syn <= 2'b0;
		else begin
			sck_syn <= {sck_syn[0],SCK};	
		end
	end
	assign SCK_U = (sck_syn == 2'b01);	
	assign SCK_D = (sck_syn == 2'b10);

	//shiftbuf & MISO(output)
	reg miso;
	always@(posedge clk) begin
		if(~rst) begin
			miso <= 1'b1;
			receive_buf <= 16'b0;
			shift_buf <= 16'b0;
		end
		else begin
			if(CS_D) shift_buf <= SEND_BUF;
			else if(CS_U) receive_buf <= shift_buf;
			else if((cs_syn == 2'b0 ) && SCK_U) begin
				miso <= shift_buf[15];
				shift_buf <= (shift_buf << 1);
			end
			else if((cs_syn == 2'b0 ) && SCK_D) 
				shift_buf[0] <= MOSI;
		end

	end
	assign MISO = miso;
	
	//receive_buf
	assign RECEIVE_BUF = receive_buf;
	
	//`spi_working
	always@(posedge clk) begin
		if(cs_syn == 2'b0) `spi_working <= 1'b1;
		else `spi_working <= 1'b0;
	end	
	
	//`spi_recv_data
	always@(posedge clk) begin
		if(CS_U) `spi_recv_data <= 1'b1;
		else `spi_recv_data <= 1'b0;
	end	
endmodule



