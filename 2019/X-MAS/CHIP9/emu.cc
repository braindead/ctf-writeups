#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <time.h>
#include "emu.hh"
#include "gfx.hh"

static constexpr int WIN_SCALE = 3;

CHIP9::CHIP9()
{
	memset(this, 0, sizeof *this);
	clr_all_bp();
}
void CHIP9::clr_all_bp()
{
	memset(m_bp, 0, sizeof m_bp);
}

void CHIP9::set_all_bp()
{
	memset(m_bp, -1, sizeof m_bp);
}

unsigned long getns()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (unsigned long)ts.tv_sec*1000000000 + ts.tv_nsec;
}

void CHIP9::disasm()
{
	u8 op = m_memory[(u16)(PC+0)];
	u8 xx = m_memory[(u16)(PC+1)];
	u8 yy = m_memory[(u16)(PC+2)];
	u16 xxyy = (xx&0xff)+(yy<<8);
	const char *def = ASM_DEFS[op];
	char flg[5] = "ZNHC";
	if (!FLAG_Z) flg[0] = '.';
	if (!FLAG_N) flg[1] = '.';
	if (!FLAG_H) flg[2] = '.';
	if (!FLAG_C) flg[3] = '.';
	for (int i = 0; i < 8; i++)
		printf("%02hhX", (u8)m_memory[(u16)(SP+i)]);
	printf(" ");
	printf("%.4s A=%02X SP=%04X BC=%04X DE=%04X HL=%04X PC=%04X  ",
		flg,
		A&0xFF, SP&0xFFFF, BC&0xFFFF, DE&0xFFFF, HL&0xFFFF, PC&0xFFFF);
	int i = 0;
	for (; i < *def; i++)
		printf("%02X", m_memory[(u16)(PC+i)]&0xFF);
	while (i < 3)
		printf("  "), i++;
	printf("  ");
	while (*++def) {
		if (*def == 1) printf("%02Xh", xx&0xFF);
		else if (*def == 2) printf("%04Xh", xxyy&0xFFFF);
		else if (*def == 3) printf("%02Xh", yy&0xFF);
		else putchar(*def);
	}
	printf("\n");
}

void CHIP9::_do_DRAW()
{
	int y = B;
	int x = C;
	if (y <= 0) return;
	if (y >= 64) return;
	//printf("DRAW %i,%i %02X\n", x, y, A);
	for (int i = 0; i < 8; i++) if (x+i >= 0 && x+i < 128)
		m_vram[m_cur_buf][y][x+i] = -(A>>(7-i)&1);
}

void CHIP9::_do_CLRSCR()
{
	m_cur_buf ^= 1;
	memset(m_vram[m_cur_buf], 0, sizeof m_vram[m_cur_buf]);
	m_status |= STATUS_FLIP;
}

void CHIP9::_do_SOUT()
{
	if (0) {
		fputs("\x1b[33m", stdout);
		putchar(A);
		fputs("\x1b[0m", stdout);
		fflush(stdout);
	} else {
		int c = A;
		if (c < 0x20)
			c = '.';
		if (c > 0x7e)
			c = '.';
		printf("SERIAL_OUT %02x %c\n", (int)A, c);
		disasm();
	}
}

void CHIP9::_do_SIN() // forgive me, Father, for I have sinned
{
	printf("SERIAL_IN> ");
	fflush(stdout);
	A = getchar();
	while (getchar() != '\n')
		;
}

void CHIP9::_do_HALT()
{
	m_status |= STATUS_HALT;
}

void CHIP9::_do_TRAP()
{
	m_status |= STATUS_TRAP;
	disasm();
}

static CHIP9 chip;

#include <SDL2/SDL.h>

static void sdl_error()
{
	printf("SDL Error: %s\n", SDL_GetError());
	exit(1);
}

#define CHECK_SDL(x) if (x) sdl_error();

static int map_pad_key(int ks)
{
	switch (ks) {
	case SDLK_UP: return 7;
	case SDLK_LEFT: return 6;
	case SDLK_DOWN: return 5;
	case SDLK_RIGHT: return 4;
	case SDLK_z: return 3;
	case SDLK_x: return 2;
	case SDLK_1: return 1;
	case SDLK_2: return 0;
	default: return -1;
	}
}

static void handle(SDL_Event &ev)
{
	switch (ev.type) {
	case SDL_QUIT:
		exit(1);
		break;
	case SDL_KEYDOWN: {
		if (ev.key.keysym.sym == SDLK_q)
			exit(1);
		int k = map_pad_key(ev.key.keysym.sym);
		//printf("PAD %d\n", k);
		if (k >= 0)
			chip.m_memory[0xf000] |= 1<<k;
	}; break;
	case SDL_KEYUP: {
		int k = map_pad_key(ev.key.keysym.sym);
		if (k >= 0)
			chip.m_memory[0xf000] &= ~((u8)1<<k);
	}; break;
	}
}

int main(int argc, char **argv)
{
	const char *bootrom = "bootrom";
	const char *user_rom = "rom";
	if (argc >= 3) {
		bootrom = argv[1];
		user_rom = argv[2];
	} else if (argc >= 2) {
		user_rom = argv[1];
	}
	FILE *f = fopen(bootrom, "rb");
	fread(chip.m_memory, 1, 0x10000, f);
	fclose(f);
	f = fopen(user_rom, "rb");
	fread(chip.m_memory+0x597, 1, 0x10000-0x597, f);
	fclose(f);

	CHECK_SDL( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) )
	signal(SIGINT, SIG_DFL);

	SDL_Window *win = SDL_CreateWindow(
		"CHIP9",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		128*WIN_SCALE,
		64*WIN_SCALE,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (! win)
		sdl_error();
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	auto gl_ctx = SDL_GL_CreateContext(win);
	if (! gl_ctx)
		sdl_error();
	SDL_GL_MakeCurrent(win, gl_ctx);

	if (0) {
		while (chip.PC != 0x030B) {
			chip.run(0, 1);
			//chip.disasm();
		}
		printf("bootloader skip finished\n");
	}

	if (0) {
		while (chip.PC != 0) {
			chip.run(0, 1);
			chip.disasm();
		}
		printf("rebootloader skip finished\n");
	}

	{
		FILE *f = fopen("memdump.bin", "wb");
		fwrite(chip.m_memory, 1, 0x10000, f);
		fclose(f);
	}

	bool trace_mode = 0;

	i8 shown_buf = 0;
	chip.m_cur_buf = 1;

	GFX gfx(128, 64);

	for (;;) {
		u32 frame_start = SDL_GetTicks();
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			handle(ev);
		}

		if (trace_mode) {
			chip.disasm();
			//getchar();
			chip.run(0, 1);
			if (chip.PC == 0x030B)
				chip.FLAG_H = 1;
		} else {
			chip.run(CHIP9::STATUS_FLIP|CHIP9::STATUS_HALT, -1);
		}

		bool redraw = false;
		if (trace_mode) {
			shown_buf = chip.m_cur_buf;
			redraw = true;
		} else if (shown_buf == chip.m_cur_buf) {
			shown_buf ^= 1;
			redraw = true;
		}

		if (redraw) {
			int w, h;
			SDL_GL_GetDrawableSize(win, &w, &h);
			gfx.render(w, h, (const char*)chip.m_vram[shown_buf]);
			SDL_GL_SwapWindow(win);
		}
		if (! trace_mode) {
			u32 frame_end = SDL_GetTicks();
			u32 frame_time = frame_end - frame_start;
			printf("load %.1lf%%\n", (double)frame_time/40*100);
			if (frame_time < 40)
				usleep(40000-frame_time*1000);
		}
	}
}
