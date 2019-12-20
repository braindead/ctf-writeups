#pragma once

extern const char *ASM_DEFS[];

#include <cstdint>
#include <cstdio>
#include <cassert>
#include <cstring>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i8  =  int8_t;
using i16 =  int16_t;
using i32 =  int32_t;

struct CHIP9 {
	CHIP9();
	bool trace() const { return false; }
	void run(u32 stop_mask, int max_steps=-1);
	void disasm();
	void _do_DRAW();
	void _do_CLRSCR();
	void _do_SOUT();
	void _do_SIN();
	void _do_HALT();
	void _do_TRAP();

	union { i16 BC; struct { i8 C, B; }; };
	union { i16 DE; struct { i8 E, D; }; };
	union { i16 HL; struct { i8 L, H; }; };
	union { i16 PC; struct { i8 PCl, PCh; }; };
	i8 A;
	i16 SP;
	i8 FLAG_Z;
	i8 FLAG_N;
	i8 FLAG_H;
	i8 FLAG_C;

	bool is_bp(u16 a) { return m_bp[a>>5]>>(a&31)&1; }
	bool is_bp() { return is_bp(PC); }
	void set_bp(u16 a) { m_bp[a>>5] |= (u32)1<<(a&31); }
	void clr_bp(u16 a) { m_bp[a>>5] &= ~((u32)1<<(a&31)); }
	void clr_bp() { clr_bp(PC); }
	void clr_all_bp();
	void set_all_bp();

	i8 m_memory[0x10000];
	u32 m_bp[0x10000/32];
	u8 m_vram[2][64][128];

	i8 m_cur_buf = 0;
	u32 m_status = 0;

	enum {
		STATUS_HALT = 1<<0,
		STATUS_FLIP = 1<<1,
		STATUS_TRAP = 1<<2,
		STATUS_BP   = 1<<3,
	};
};
