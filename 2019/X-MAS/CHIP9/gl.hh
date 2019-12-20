#pragma once

#include <epoxy/gl.h>
#include "util/string.h"
#include "util/vector.h"

namespace gl {

int compile(GLuint handle, const Str &source);

template <GLenum GLTYPE>
struct Shader {
	GLuint m_handle;
	Shader() : m_handle(0) { }
	~Shader()
	{
		if (m_handle)
			glDeleteShader(m_handle);
	}
	int compile(const Str &s) { return gl::compile(m_handle, s); }
	void operator=(Shader<GLTYPE> &&s)
	{
		if (m_handle)
			glDeleteShader(m_handle);
		m_handle = s.m_handle;
		s.m_handle = 0;
	}
	void create()
	{
		assert(!m_handle);
		m_handle = glCreateShader(GLTYPE);
	}
};

using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

struct Program {
	GLuint m_handle;
	Program() : m_handle(0) {}
	~Program()
	{
		if (m_handle)
			glDeleteProgram(m_handle);
	}
	void create()
	{
		assert(!m_handle);
		m_handle = glCreateProgram();
	}
	void use()
	{
		assert(m_handle);
		glUseProgram(m_handle);
	}
	template <GLenum TYPE>
	void attach(const Shader<TYPE> &s)
	{
		glAttachShader(m_handle, s.m_handle);
	}
	int link();

	void set_uniform_name(int u, const char *name)
	{
		if (u >= (int)m_uniforms.size())
			m_uniforms.resize(u+1);
		m_uniforms[u] = glGetUniformLocation(m_handle, name);
	}
	GLint uniform(int u) { return m_uniforms[u]; }

	void set_attrib_name(int u, const char *name)
	{
		if (u >= (int)m_attribs.size())
			m_attribs.resize(u+1);
		m_attribs[u] = glGetAttribLocation(m_handle, name);
	}
	GLint attrib(int u) { return m_attribs[u]; }

private:
	Vector<GLint> m_uniforms;
	Vector<GLint> m_attribs;
};

void check_error(const char *where=nullptr);

struct ExtensionInfo {
	const char *name;
	bool is_available;
	operator bool() const
	{ return is_available; }
};

#define EXTENSIONS(_) \
	_(EXT_clear_texture)

#define decl_ext(name) \
	extern ExtensionInfo name;
EXTENSIONS(decl_ext)
#undef decl_ext

void query_extensions();

}

