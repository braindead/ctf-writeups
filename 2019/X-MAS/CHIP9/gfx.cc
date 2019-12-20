#include "gl.hh"
#include "gfx.hh"
#include <cstdio>
#include <epoxy/gl_generated.h>

#define INFO(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

struct iv2 {
	int x, y;
	iv2(int x, int y) : x(x), y(y) { }
	iv2() : iv2(0, 0) { }
	bool operator==(iv2 i) const { return x == i.x && y == i.y; }
	bool operator!=(iv2 i) const { return !(*this == i); }
};

static const Str VERT = R"END(
#version 100
precision lowp float;

attribute vec2 aPos;
varying vec2 vTexPos;

void main()
{
	vec2 t = vec2(aPos.x, -aPos.y);
	vTexPos = t;//t*0.5 + 0.5;
	gl_Position = vec4(aPos, 0.0, 1.0);
}
)END";

static const Str FRAG = R"END(
#version 100
precision mediump float;

varying vec2 vTexPos;
uniform sampler2D uSampler;

void main()
{
	vec2 t = vTexPos;
	t *= log(6.0+dot(t, t))*0.55;
	float x = texture2D(uSampler, t*0.5+0.5).x;
	float scanline = abs(0.5 - fract(t.y*64.0));
	x *= 0.7 + 0.3*scanline;
	x *= 1.2;
	// tone mapping goes here
	gl_FragColor = vec4(
		x*1.3-0.35,
		x,
		x*1.3-0.32,
		1.0
	);
}
)END";

struct GFX::Impl {
	Impl(int w, int h)
		: m_vram_size(w, h)
	{
		m_screen = new u8[w*h];
		memset(m_screen, 0, w*h);
		glGenTextures(1, &m_vram);
		glBindTexture(GL_TEXTURE_2D, m_vram);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glBindTexture(GL_TEXTURE_2D, 0);

		INFO("GL Version: %s\n", (const char*)glGetString(GL_VERSION));
		INFO("GL Vendor: %s\n", (const char*)glGetString(GL_VENDOR));
		INFO("GL Renderer: %s\n", (const char*)glGetString(GL_RENDERER));

		m_vert.create();
		m_vert.compile(VERT);
		m_frag.create();
		m_frag.compile(FRAG);
		m_prog.create();
		m_prog.attach(m_vert);
		m_prog.attach(m_frag);
		m_prog.link();
		gl::check_error("init");
	}

	void render(int drawable_w, int drawable_h, const char *vram)
	{

		glClearColor(0.2, 0.6, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, drawable_w, drawable_h);
		for (int i = 0; i < m_vram_size.x*m_vram_size.y; i++) {
			auto v = (u8)vram[i];
			v -= v>>3;
			v += 31;
			auto &s = m_screen[i];
			s = s*2/8 + v*6/8;
		}
		gl::check_error("start frame");

		m_prog.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_vram);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ROW_LENGTH, 128);
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			GL_RED,
			m_vram_size.x, m_vram_size.y,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_screen);
		gl::check_error("texture upload");

		GLfloat two_tris[8] = {
			-1, -1,
			+1, -1,
			+1, +1,
			-1, +1,
		};

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, two_tris);
		glUniform1i(0, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
		gl::check_error("render end");
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	iv2 m_vram_size;
	u8 *m_screen;
	GLuint m_vram;
	gl::Program m_prog;
	gl::VertexShader m_vert;
	gl::FragmentShader m_frag;
};

GFX::GFX(int w, int h) { this->pimpl = new Impl(w, h); }
void GFX::render(int dw, int dh, const char *vram) { this->pimpl->render(dw, dh, vram); }
