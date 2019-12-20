#define LOG_TAG "gl"
#include "gl.hh"
#include <cstdio>

namespace gl {

int compile(GLuint handle, const Str &source)
{
	assert(handle);
	const char *ptr = source.begin();
	int len = source.size();
	glShaderSource(handle, 1, &ptr, &len);
	glCompileShader(handle);
	GLint ok;
	GLint slen = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &slen);
	if (slen > 1) {
		Vector<char> info;
		info.reserve(slen);
		glGetShaderInfoLog(handle, slen, NULL, info.begin());
		info.set_size(slen);
		fputs("======= BEGIN COMPILE INFO LOG =======\n", stderr);
		fwrite(info.begin(), 1, info.size()-1, stderr);
		fputs("======== END COMPILE INFO LOG ========\n", stderr);
	}
	glGetShaderiv(handle, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		fprintf(stderr, "failed to compile shader\n");
		return 1;
	}
	return 0;
}

int Program::link()
{
	glLinkProgram(m_handle);
	int ok;
	GLint len = 0;
	glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		Vector<char> info;
		info.reserve(len);
		glGetProgramInfoLog(m_handle, len, NULL, info.begin());
		info.set_size(len);
		fputs("======= BEGIN LINK INFO LOG =======\n", stderr);
		fwrite(info.begin(), 1, info.size()-1, stderr);
		fputs("======== END LINK INFO LOG ========\n", stderr);
	}
	glGetProgramiv(m_handle, GL_LINK_STATUS, &ok);
	if (!ok) {
		fprintf(stderr, "failed to link program\n");
		return 1;
	}
	return 0;
}

void check_error(const char *where)
{
	GLenum err = glGetError();
	if (err == GL_NO_ERROR)
		return;
	if (where)
		fprintf(stderr, "!!!!!!!!! GL ERROR %d in %s !!!!!!!!!\n", err, where);
	else
		fprintf(stderr, "!!!!!!!!! GL ERROR %d !!!!!!!!!\n", err);
}

}

