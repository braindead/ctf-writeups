#pragma once

struct GFX {
	struct Impl;
	GFX(int w, int h);
	void render(int draw_w, int draw_h, const char *vram);
private:
	Impl *pimpl;
};
