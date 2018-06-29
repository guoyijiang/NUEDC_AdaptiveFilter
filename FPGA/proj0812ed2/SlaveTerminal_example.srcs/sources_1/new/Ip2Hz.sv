
`timescale 1ns/100ps
`default_nettype none

module Counter #(
    parameter M = 100
)(
    input wire clk, rst, en,
    output logic [$clog2(M) - 1 : 0] cnt,
    output logic co
);
    assign co = en & (cnt == M - 1);
    always_ff@(posedge clk) begin
        if(rst) cnt <= '0;
        else if(en) begin
            if(cnt < M - 1) cnt <= cnt + 1'b1;
            else cnt <= '0;
        end
    end
endmodule

module InterpDeci #( parameter W = 10 )(
    input wire clk, rst, eni, eno,
    input wire signed [W-1:0] in,
    output logic signed [W-1:0] out
);
    logic signed [W-1:0] candi;
    always_ff@(posedge clk) begin
        if(rst) candi <= '0;
        else if(eni) candi <= in;
        else if(eno) candi <= '0;
    end
    always_ff@(posedge clk) begin
        if(rst) out <= '0;
        else if(eno) out <= candi;
    end
endmodule

module Integrator #( parameter W = 10 )(
    input wire clk, rst, en,
    input wire signed [W-1:0] in,
    output logic signed [W-1:0] out
);
    always_ff@(posedge clk) begin
        if(rst) out <= '0;
        else if(en) out <= out + in;
    end
endmodule

module Comb #( parameter W = 10, M = 1 )(
    input wire clk, rst, en,
    input wire signed [W-1:0] in,
    output logic signed [W-1:0] out
);
    logic signed [W-1:0] dly[M];    // imp z^-M
    generate
        if(M > 1) begin
            always_ff@(posedge clk) begin
                if(rst) dly <= '{M{'0}};
                else if(en) dly <= {in, dly[0:M-2]};
            end
        end
        else begin
            always_ff@(posedge clk) begin
                if(rst) dly <= '{M{'0}};
                else if(en) dly[0] <= in;
            end
        end
    endgenerate
    always_ff@(posedge clk) begin
        if(rst) out <= '0;
        else if(en) out <= in - dly[M-1];
    end
endmodule

module CicDownSampler #( parameter W = 10, R = 4, M = 1, N = 2 )(
    input wire clk, rst, eni, eno,
    input wire signed [W-1:0] in,
    output logic signed [W-1:0] out
);
    localparam real GAIN = 1e+024;
    localparam integer DW = 92;
    logic signed [DW-1:0] intgs_data[N+1];
    assign intgs_data[0] = in;
    generate
        for(genvar k = 0; k < N; k++) begin : Intgs
            Integrator #(DW) theIntg(
                clk, rst, eni, intgs_data[k], intgs_data[k+1]);
        end
    endgenerate
    logic signed [DW-1:0] combs_data[N+1];
    InterpDeci #(DW) theDeci(
        clk, rst, eni, eno, intgs_data[N], combs_data[0]);
    generate
        for(genvar k = 0; k < N; k++) begin : Combs
            Comb #(DW, M) theComb(
                clk, rst, eno, combs_data[k], combs_data[k+1]);
        end
    endgenerate
    // Q1.(DW-1)
    wire signed [DW-1:0] attn = 'sd2476;
    always_ff@(posedge clk) begin
        if(rst) out <= '0;
        else if(eno) out <= ((2*DW)'(combs_data[N]) * (2*DW)'(attn)) >>> (DW-1);
        //else if(eno) out <= ((184)'(combs_data[N]) * (184)'(attn)) >>> 91;
       // else if(eno) out <= (184'sd1 * combs_data[N] * (184)'(attn)) >>> 91;
        //else if(eno) out <= ((184)'(combs_data[N]) * (184)'(attn)) >>> (DW-1);
    end
endmodule

module lp2Hz100M
#(
    parameter W = 12
)(
    input wire clk, rst,
    input wire signed [W-1 : 0] in,
    output logic signed [W-1 : 0] out
);
    wire en_100Hz, en_50Hz, en_25Hz;
    Counter #(1_000_000) Cnt1M(clk, rst, 1'b1, , en_100Hz);
    Counter #(2) Cnt2a(clk, rst, en_100Hz, , en_50Hz),
                 Cnt2b(clk, rst, en_50Hz, , en_25Hz);
    wire signed [W-1:0] cic_out;
    CicDownSampler #(W, 1_000_000, 1, 4) cicDs(clk, rst, 1'b1, en_100Hz, in, cic_out);
    wire signed [W-1:0] fir1_out, fir2_out;
    // @100Hz, 7Hz \ 43Hz
    FIR_Kaiser_13Taps_LP_0d500 fir1(clk, en_100Hz, rst, cic_out, fir1_out);
    // @50Hz, 6Hz \ 44Hz
    FIR_Kaiser_17Taps_LP_0d500 fir2(clk, en_50Hz, rst, fir1_out, fir2_out);
    // @25Hz, 3Hz \ 6Hz
    FIR_Kaiser_21Taps_LP_0d250 fir3(clk, en_25Hz, rst, fir2_out, out);
endmodule