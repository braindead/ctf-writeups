module TweakLFSR (
	x,
	y,
);
	input [3:0] x;
	output [3:0] y;

	assign y[0] = x[1];
	assign y[1] = x[2];
	assign y[2] = x[3];
	assign y[3] = x[0] ^ x[1];
endmodule

module TweakKey (
	x,
	y,
);
	input [63:0] x;
	output [63:0] y;

    // for b in [0,1,3,4,8,11,13]:
	TweakLFSR _t0 ( .x(x[63:60]), .y(y[63:60]) );
	TweakLFSR _t1 ( .x(x[59:56]), .y(y[59:56]) );
	assign y[55:52] = x[55:52];
	TweakLFSR _t3 ( .x(x[51:48]), .y(y[51:48]) );
	TweakLFSR _t4 ( .x(x[47:44]), .y(y[47:44]) );
	assign y[43:40] = x[43:40];
	assign y[39:36] = x[39:36];
	assign y[35:32] = x[35:32];
	TweakLFSR _t8 ( .x(x[31:28]), .y(y[31:28]) );
	assign y[27:24] = x[27:24];
	assign y[23:20] = x[23:20];
	TweakLFSR _t11 ( .x(x[19:16]), .y(y[19:16]) );
	assign y[15:12] = x[15:12];
	TweakLFSR _t13 ( .x(x[11:8]), .y(y[11:8]) );
	assign y[7:4] = x[7:4];
	assign y[3:0] = x[3:0];

endmodule

module PermuteState (
	x,
	y,
);
	input [63:0] x;
	output [63:0] y;

	// state_permutation = [0,11,6,13,10,1,12,7,5,14,3,8,15,4,9,2]
	assign y[63:60] = x[63:60];
	assign y[59:56] = x[19:16];
	assign y[55:52] = x[39:36];
	assign y[51:48] = x[11:8];
	assign y[47:44] = x[23:20];
	assign y[43:40] = x[59:56];
	assign y[39:36] = x[15:12];
	assign y[35:32] = x[35:32];
	assign y[31:28] = x[43:40];
	assign y[27:24] = x[7:4];
	assign y[23:20] = x[51:48];
	assign y[19:16] = x[31:28];
	assign y[15:12] = x[3:0];
	assign y[11:8]  = x[47:44];
	assign y[7:4]   = x[27:24];
	assign y[3:0]   = x[55:52];
endmodule

module MixColumn ( x, y, );
	input [15:0] x;
	output [15:0] y;

	assign y[15] = x[10]^x[5]^x[0];
	assign y[14] = x[9]^x[4]^x[3];
	assign y[13] = x[8]^x[7]^x[2];
	assign y[12] = x[11]^x[6]^x[1];
	assign y[11] = x[12]^x[6]^x[1];
	assign y[10] = x[15]^x[5]^x[0];
	assign y[9] = x[14]^x[4]^x[3];
	assign y[8] = x[13]^x[7]^x[2];
	assign y[7] = x[13]^x[8]^x[2];
	assign y[6] = x[12]^x[11]^x[1];
	assign y[5] = x[15]^x[10]^x[0];
	assign y[4] = x[14]^x[9]^x[3];
	assign y[3] = x[14]^x[9]^x[4];
	assign y[2] = x[13]^x[8]^x[7];
	assign y[1] = x[12]^x[11]^x[6];
	assign y[0] = x[15]^x[10]^x[5];
endmodule

/*
def MixColumns(state):
    mixed_state = [0 for _ in range(16)]
    for i in range(4):
        incol = [state[0+i], state[4+i], state[8+i], state[12+i]]
        outcol = MixColumns_M41(incol)
        mixed_state[0+i], mixed_state[4+i], mixed_state[8+i], mixed_state[12+i] = outcol
    return mixed_state
*/
module MixColumns ( x, y, );
	input [63:0] x;
	output [63:0] y;

	MixColumn _mix0 ( .x({ x[63:60], x[47:44], x[31:28], x[15:12] }), .y({ y[63:60], y[47:44], y[31:28], y[15:12] }) );
	MixColumn _mix1 ( .x({ x[59:56], x[43:40], x[27:24], x[11:8]  }), .y({ y[59:56], y[43:40], y[27:24], y[11:8]  }) );
	MixColumn _mix2 ( .x({ x[55:52], x[39:36], x[23:20], x[7:4]   }), .y({ y[55:52], y[39:36], y[23:20], y[7:4]   }) );
	MixColumn _mix3 ( .x({ x[51:48], x[35:32], x[19:16], x[3:0]   }), .y({ y[51:48], y[35:32], y[19:16], y[3:0]   }) );
endmodule

module SBox ( x, y );
	input [7:0] x;
	output [7:0] y;
	always@(*) case (x)
		`include "sbox.v"
	endcase
endmodule

module SubBytes ( x, y );
	input [63:0] x;
	output [63:0] y;
	SBox _s0 ( .x(x[63:56]), .y(y[63:56]) );
	SBox _s1 ( .x(x[55:48]), .y(y[55:48]) );
	SBox _s2 ( .x(x[47:40]), .y(y[47:40]) );
	SBox _s3 ( .x(x[39:32]), .y(y[39:32]) );
	SBox _s4 ( .x(x[31:24]), .y(y[31:24]) );
	SBox _s5 ( .x(x[23:16]), .y(y[23:16]) );
	SBox _s6 ( .x(x[15:8]), .y(y[15:8]) );
	SBox _s7 ( .x(x[7:0]), .y(y[7:0]) );
endmodule

module Round (
	im,
	ik,
	om,
	ok,
);
	input [63:0] im;
	input [63:0] ik;
	output [63:0] om;
	output [63:0] ok;

	TweakKey tweak_key ( .x(ik), .y(ok) );

	wire [63:0] m1;
	assign m1 = im ^ ik;

	wire [63:0] m2;
	PermuteState _p ( .x(m1), .y(m2) );

	wire [63:0] m3;
	MixColumns _m ( .x(m2), .y(m3) );

	SubBytes _s ( .x(m3), .y(om) );
endmodule

module Cipher (
	ptext,
	key,
	ctext,
);
	input [63:0] ptext;
	input [63:0] key;
	output [63:0] ctext;

	wire [63:0] key2;
	wire [63:0] ptext2;
	Round _round1 ( .im(ptext), .ik(key), .ok(key2), .om(ptext2) );
	Round _round2 ( .im(ptext2), .ik(key2), .om(ctext) );
endmodule
