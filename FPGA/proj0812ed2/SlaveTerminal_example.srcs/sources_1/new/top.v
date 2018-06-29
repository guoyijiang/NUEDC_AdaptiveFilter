//new sim
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Create Date: 2017/04/30 14:41:14
//Author:GYJ
// Design Name: terminal
// Target Devices: 
// Tool Versions: vivado 2016.4
// Description: 
//
//////////////////////////////////////////////////////////////////////////////////

`define nWREG 10
`define nRREG  15
module top(
        input wire clk,
        input wire crystal_clk,
        //spi slave
        input wire CS,   
        input wire SCK,
        output wire MISO,
        input wire MOSI,
        //clk's
        output wire si5351_scl,
        inout wire si5351_sda,
        //LED
        output reg [7:0] led,
         //user's
        input wire [7:0] ad9288_a,
        input wire [7:0] ad9288_b,
        output reg [9:0] dac5652a_a,
        output reg [9:0] dac5652a_b,
        
//        output wire CS_ADS7883A,
//        input  wire DI_ADS7883A,
//        output wire SCK_ADS7883A,
//        output wire CS_ADS7883B,
//        input  wire DI_ADS7883B,
//        output wire SCK_ADS7883B,
        
        input wire GATE
        
//        output wire SCK_DAC8811,
//        output wire DO_DAC8811,
//        output wire CS_DAC8811        
        
    );

/*******************************************************************************************/
//RST
    reg rst;
    initial rst <= 1'b0;
    always@(posedge clk) begin
     if(~rst) rst <= 1'b1;
    end
/*******************************************************************************************/
 //CLK
       Si5351_Init clk_init(
       .clk(crystal_clk),
       .sclx(si5351_scl),
       .sda(si5351_sda)
       );
/*******************************************************************************************/
//BUILD STAGE
        wire[15:0] SEND_BUF,RECEIVE_BUF;
        wire[1:0]SPI_STATE;
        reg[31:0] WREG[0:`nWREG];
        reg[31:0] RREG[`nWREG+1:`nRREG];
        //initial WREG
        initial $readmemh("reg_ini.dat", WREG);
        
        //SPI_SLAVE
        //spi_working     SPI_STATE[0]
        //receive_data     SPI_STATE[1]
        my_spi_slave my_spi_slave_inst(
        .clk(clk),
        .rst(rst),
        .SEND_BUF(SEND_BUF),
        .RECEIVE_BUF(RECEIVE_BUF),
        .spi_state(SPI_STATE),
        .SCK(SCK),
        .CS(CS),
        .MOSI(MOSI),
        .MISO(MISO)
        );    
    
        //STATE: receive_cnt
        //RECEIVE: receive_buf[0:2]
        reg[1:0]receive_cnt = 2'b0;
        reg[15:0] receive_buf[0:2];// 2lsb 1msb 0addr
        always@(posedge clk) begin
            if(~rst) begin
                receive_cnt <= 2'b0;
            end
            else begin
                if(SPI_STATE[1]) begin
                    case(receive_cnt)
                        2'd0:                                     
                            if((RECEIVE_BUF[15:8] == 8'b10000000)||(RECEIVE_BUF[15:8] == 8'b01000000) ) begin
                                if(RECEIVE_BUF[7:0] < `nRREG) begin                            
                                    receive_buf[0]<= RECEIVE_BUF;
                                    receive_cnt <= 2'd1;
                                end            
                            end
                        2'd1: begin
                            receive_buf[1]<= RECEIVE_BUF;
                            receive_cnt <= 2'd2;                
                        end
                        2'd2: begin
                            receive_buf[2]<= RECEIVE_BUF;
                            receive_cnt <= 2'd0;
                        end
                        default: receive_cnt<= 2'b0;
                    endcase
                end
            end
        end    
    
        //STATE CHANGE
        reg[1:0] receive_cnt_l;
        wire STEP_0to1,STEP_1to2,STEP_2to0,READ_EN,WRITE_EN;
        always@(posedge clk) receive_cnt_l <= receive_cnt;
        assign STEP_0to1 = (receive_cnt[0]&(~receive_cnt_l[0]));
        assign STEP_1to2 = (receive_cnt[1]&(~receive_cnt_l[1]));
        assign STEP_2to0 = (receive_cnt_l[1]&(~receive_cnt[1]));
        assign READ_EN      = (receive_buf[0][15:8] == 8'b01000000 );
        assign WRITE_EN  = (receive_buf[0][15:8] == 8'b10000000 );
        
        //READ: SEND DATA
        reg[15:0]send_buf;    
        always@(posedge clk) begin
            if(~rst) begin
            send_buf <= 16'b0;    
            end
            else if(READ_EN) begin
                if(STEP_0to1) begin
					if(receive_buf[0][7:0] <= `nWREG)
						send_buf <= WREG[receive_buf[0][7:0]][31:16];
					else if(receive_buf[0][7:0] <=`nRREG)
						send_buf <= RREG[receive_buf[0][7:0]][31:16];
				end
                if(STEP_1to2) begin
					if(receive_buf[0][7:0] <= `nWREG)
						send_buf <= WREG[receive_buf[0][7:0]][15 :0];
					else if(receive_buf[0][7:0] <=`nRREG)
						send_buf <= RREG[receive_buf[0][7:0]][15 :0];
				end				
            end
        end    
        assign SEND_BUF = send_buf;    
        
        //WRITE: UPDATE WREG
        reg[7:0] addr;
        reg reg_valid;
        always@(posedge clk) begin
            if(~rst) begin
                reg_valid <= 1'b0;
                addr <= 8'b0;
            end
            else begin
                if(STEP_2to0 & WRITE_EN & (receive_buf[0][7:0] <= `nWREG)) begin
                    reg_valid <= 1'b1;
                    addr <= receive_buf[0][7:0];
                    WREG[receive_buf[0][7:0]] <= {receive_buf[1],receive_buf[2]};
                end
                else reg_valid <= 1'b0;
            end
        end
 /*******************************************************************************************/
 //USER'S LOGIC
     //example  WREG[0]
     //REGSHIFT WREG1
     //DATASWITCH WREG2 DATASWITCH2 WREG3
     // Tcnt RREG11
     //SIN RREG12
     //COS RREG13
    reg  signed [7:0] data_a,data_b;  //a:dsn in     b:noise in
//    wire [11:0]ads7883_dataIn;  //input dsnin
//    wire signed [11:0] dsnintemp;
//    wire signed [11:0] dsnintemp2;
//    wire signed [28:0] dsnintemp3;
//    wire signed [11:0] dsnin;
//    assign dsnintemp = ads7883_dataIn + 12'd2048 ;
//    assign dsnin = dsnintemp + 12'sd120 + $signed(WREG[7]);
//    assign dsnintemp3 = dsnintemp2*17'sd63015;
//    assign dsnin = dsnintemp3[26:15];	

//dsn in
wire signed [11:0] dsnintemp;
wire signed [28:0] dsnintemp2;
wire signed [11:0] dsnin;
assign dsnintemp = $signed( {data_a,4'b0} )-12'sd199 -$signed(WREG[6]);
//assign noiseintemp2 = dsnintemp*17'sd42975;
assign dsnintemp2 = dsnintemp*17'sd46811;
assign dsnin = dsnintemp2[26:15]; 
//assign dsnin = dsnintemp; 
 
 //noise in    
wire signed [11:0] noiseintemp;
wire signed [28:0] noiseintemp2;
wire signed [11:0] noisein;
assign noiseintemp = $signed( {data_b,4'b0} )-12'sd75 -$signed(WREG[7]);
// assign noiseintemp2 = noiseintemp*17'sd42975;
assign noiseintemp2 = noiseintemp*17'sd23346;//22776
assign noisein = noiseintemp2[26:15];
//assign noisein = noiseintemp;

    
    wire signed[11:0] ip2HzdDoutsin;  //out multi
    wire signed[11:0] ip2HzdDoutcos;
    wire signed[31:0] doutsin32 = ip2HzdDoutsin;
    wire signed[31:0] doutcos32 = ip2HzdDoutcos;
    always@(posedge clk) RREG[12] <= doutsin32;
    always@(posedge clk) RREG[13] <= doutcos32;    
//ADS7883 f= 5MSPS
//     wire ads7883Finished; //5M
//     ads7883_5M_En ads7883_5M_En_inst(
//        .clk(clk),
//        .rst(rst),
//        .workEn(1'd1),
//        .finished5M(ads7883Finished),
//        .recivData(ads7883_dataIn),
//        .CS_A(CS_ADS7883A),
//        .SCK_A(SCK_ADS7883A),
//        .MISO_A(DI_ADS7883A),
//        .CS_B(CS_ADS7883B),
//        .SCK_B(SCK_ADS7883B),
//        .MISO_B(DI_ADS7883B)
//     );
//RAM
     wire noiseSpFinished;
     assign noiseSpFinished = 1'd1;
//     reg en50M = 1'd0;
//     always@(posedge clk) en50M <= ~en50M;
//     assign noiseSpFinished = en50M;
     
     wire  [11:0] rddata;  //cosÐÅºÅ
     reg   [11:0] wrdata = 'd0; 
     reg wren = 'd0;
     wire [13:0] wraddr;
     reg  [13:0] rdaddr = 'd0;
     reg  [13:0] delta = 'd2; 
         //wr
     always @(posedge clk) begin
         if(noiseSpFinished) begin
             wren <= 1'd1;
             wrdata <= noisein;
         end
         else wren <= 1'd0;
     end
     always@(posedge clk) begin
         if(noiseSpFinished) rdaddr <= rdaddr + 1'd1;
     end
     assign wraddr = rdaddr + (delta -1'd1);
     blk_mem_gen_0 bram_inst
        (
         .clka(clk),
         .ena(1'b1),
         .wea(wren),
         .addra(wraddr),
         .dina(wrdata),
         .clkb(clk),
         .enb(1'b1),
         .addrb(rdaddr),
         .doutb(rddata)
       ); 
 //mesure T
    (* mark_debug = "true" *) wire [31:0] T;
    measureT(
     .clk(clk),
     .rst(~rst),//-
     .gateIn(GATE),
     .mode(WREG[8][0]),
     .data_in(noisein),//signed 12bit
     .T(T)
     );
    always@(posedge clk) RREG[11] <= T;
    always@(posedge clk) delta <= (T>>2); //SHIFT PI/2
    //always@(posedge clk) delta <= 10'd50;

//RAM2
    reg wren2 = 'd0;
    wire [13:0] wraddr2;
    reg  [13:0] rdaddr2 = 'd0;
    wire [13:0] delta2;
    reg   [11:0] wrdata2 = 'd0;
    wire  [11:0] rddata2;
    assign delta2 = WREG[1][13:0];//shift
        //wren data
    always @(posedge clk) begin
        if(noiseSpFinished) begin
            wren2 <= 1'd1;
            wrdata2 <= noisein;
        end
        else wren2 <= 1'd0;
    end
    //wraddr  rdaddr
    always@(posedge clk) begin
        if(noiseSpFinished) rdaddr2 <= rdaddr2 + 1'd1;
    end
    assign wraddr2 = rdaddr2 + (delta2 -1'd1);
    blk_mem_gen_0 bram_inst2
       (
        .clka(clk),
        .ena(1'b1),
        .wea(wren2),
        .addra(wraddr2),
        .dina(wrdata2),
        .clkb(clk),
        .enb(1'b1),
        .addrb(rdaddr2),
        .doutb(rddata2)
      );  
//multi
      wire signed[11:0] cosx;
      wire signed[11:0] sinx;
      wire signed[23:0] multitempsinx;
      wire signed[23:0] multitempcosx;
      
      wire signed[11:0] multisinx;
      wire signed[11:0] multicosx;
      assign cosx = noisein;
      assign sinx = rddata;
      assign multitempsinx = (sinx*dsnin);
      assign multitempcosx = (cosx*dsnin);
      assign multisinx = multitempsinx[23:12];
      assign multicosx = multitempcosx[23:12];     
      
 //ip2Hz
       lp2Hz100M 
      #( .W(12)
      )lp2Hz100M_sin
      (
          .clk(clk),
          .rst(~rst),
          .in(multisinx),
          .out(ip2HzdDoutsin)
      );     
       lp2Hz100M 
      #( .W(12)
      )lp2Hz100M_cos
      (
          .clk(clk),
          .rst(~rst),
          .in(multicosx),
          .out(ip2HzdDoutcos)
      ); 
 //test
 wire signed[11:0] testout;
        lp2Hz100M 
       #( .W(12)
       )lp2Hz100M_test
       (
           .clk(clk),
           .rst(~rst),
           .in(noisein),
           .out(testout)
       );                    
 // SUB
 wire signed[12:0] sysOUT_sum;
 wire signed[11:0] sysOUT_nolift;
 
 wire signed[29:0] sysOUTtemp;
 wire signed [11:0] sysOUTtemp2;
 wire [9:0] sysOUT;
 

 
 assign sysOUT_sum = dsnin - $signed(rddata2);
 assign sysOUT_nolift = sysOUT_sum[12:1];
 assign sysOUTtemp = sysOUT_nolift*18'sd117029;
 assign sysOUTtemp2 = sysOUTtemp[26:15]; 
 
 assign sysOUT = sysOUTtemp2[11:2] + 10'd512;
    
    
// wire signed [11:0] dsnintemp;
// wire signed [28:0] dsnintemp2;
// wire signed [11:0] dsnin;
// assign dsnintemp = $signed( {data_a,4'b0} )-12'sd199 -$signed(WREG[6]);
// assign dsnintemp2 = dsnintemp*17'sd46811;
// assign dsnin = dsnintemp2[26:15]; 
 
          
 //HADDA
    reg [9:0] hdaData = 'd0; 
    reg [9:0] hdaData2 = 'd0; 
//    reg [15:0] ldaData = 'd0;
    always@(posedge clk) begin
        case (WREG[2])
            32'd0:  hdaData <= {data_a,2'b0} + 10'd512;
            32'd1:  hdaData <= {data_b,2'b0} + 10'd512;
            32'd2:  hdaData <= dsnin[11:2] + 10'd512;
            32'd3:  hdaData <= sinx[11:2] + 10'd512;
            32'd4:  hdaData <= cosx[11:2] + 10'd512; 
            32'd5:  hdaData <= multisinx[11:2] + 10'd512;
            32'd6:  hdaData <= multicosx[11:2] + 10'd512;
            32'd7:  hdaData <= ip2HzdDoutsin[11:2] + 10'd512;
            32'd8:  hdaData <= ip2HzdDoutcos[11:2] + 10'd512;
            32'd9:  hdaData <= sysOUT;
            32'd10:  hdaData <= testout[11:2] + 10'd512;  
            32'd11:  hdaData <= rddata2[11:2] + 10'd512;
            32'd12:  hdaData <= 10'd512; 
            default: hdaData <= {data_a,2'b0} + 10'd512;
        endcase
    end
    always@(posedge clk) begin
        case (WREG[3])
            32'd0:  hdaData2 <= {data_a,2'b0} + 10'd512;
            32'd1:  hdaData2 <= {data_b,2'b0} + 10'd512;
            32'd2:  hdaData2 <= dsnin[11:2] + 10'd512;
            32'd3:  hdaData2 <= sinx[11:2] + 10'd512;
            32'd4:  hdaData2 <= cosx[11:2] + 10'd512; 
            32'd5:  hdaData2 <= multisinx[11:2] + 10'd512;
            32'd6:  hdaData2 <= multicosx[11:2] + 10'd512;
            32'd7:  hdaData2 <= ip2HzdDoutsin[11:2] + 10'd512;
            32'd8:  hdaData2 <= ip2HzdDoutcos[11:2] + 10'd512;
            32'd9:  hdaData2 <= sysOUT;
            32'd10:  hdaData2 <= testout[11:2] + 10'd512;
            32'd11:  hdaData2 <= rddata2[11:2] + 10'd512;
            32'd12:  hdaData2 <= 10'd512;          
            default: hdaData2 <= {data_a,2'b0} + 10'd512;
        endcase
    end    
    always@(posedge clk)begin
        data_a <= ad9288_a;
        data_b <= ad9288_b;
//        dac5652a_a[9:0] <= {data_a + 8'h80,2'b0};
        dac5652a_a[9:0] <= hdaData;
        dac5652a_b[9:0] <= hdaData2;
    end
 /*******************************************************************************************/    
endmodule
