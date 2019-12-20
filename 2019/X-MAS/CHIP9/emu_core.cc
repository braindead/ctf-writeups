#include "emu.hh"

const char *ASM_DEFS[] = {
#include "disasm.gen.cc"
};

void CHIP9::run(u32 stop_cond, int fuel)
{
	m_status = 0;
	while (fuel) {
		if (is_bp()) {
			m_status |= STATUS_BP;
		}
		if (m_status&stop_cond)
			break;
		fuel -= 1;

		u8 op = m_memory[(u16)(PC+0)];
		i32 xx = (i8)m_memory[(u16)(PC+1)];
		i32 yy = (i8)m_memory[(u16)(PC+2)];
		i32 xxyy = (xx&0xff)+(yy<<8);
		i32 alu_temp;
		//i32 discard;

		if (trace()) disasm();

		switch (op) {
		#include "core.gen.cc"
		}
	}
}
