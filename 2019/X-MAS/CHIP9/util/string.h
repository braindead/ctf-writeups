#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "util/prelude.h"
#include "util/hasher.h"

struct Str {
	Str() : m_begin(nullptr), m_end(nullptr) { }
	Str(const Str &s) = default;
	template <size_t Size>
	Str(const char literal[Size]) : m_begin((char*)literal), m_end((char*)literal+Size-1) { }
	Str(const char *literal) : m_begin((char*)literal), m_end((char*)literal+strlen(literal)) { }
	Str(const char *b, const char *e) : m_begin((char*)b), m_end((char*)e) { }
	Str(const char *buf, size_t len) { m_begin = (char*)buf; m_end = (char*)buf+len; }

	const char *begin() const { return m_begin; }
	const char *end() const { return m_end; }
	const char *cbegin() const { return m_begin; }
	const char *cend() const { return m_end; }
	size_t size() const { return m_end - m_begin; }
	char operator[](size_t i) const { return m_begin[i]; }

	bool startswith(char c) const { return size() >= 1 && (*this)[0] == c; }
	bool startswith(Str pref) const;
	bool contains(char c) const { return memchr(begin(), c, size()) != nullptr; }
	long find(Str needle) const;
	long find(char c) const;

	Str substr(size_t ofs, size_t len) { return Str(m_begin+ofs, m_begin+ofs+len); }
	Str substr(size_t ofs) { return Str(m_begin+ofs, m_end); }

	bool operator==(Str k) const
	{ return size() == k.size() && ! memcmp(begin(), k.begin(), size()); }

	void to_hash(Hasher &h) const { h.push_mem((u8*)m_begin, size()); }
	bool is_valid() const { return m_begin != nullptr; }

	using const_iterator = const char*;

protected:
	char *m_begin, *m_end;
	//int m_line, m_col;
};

struct MutStr : Str {
	MutStr() : Str() { }
	template <size_t Size>
	MutStr(char buf[Size]) { m_begin = buf; m_end = buf+Size; }
	MutStr(char *buf, size_t len) { m_begin = buf; m_end = buf+len; }
	MutStr(char *buf, char *end) { m_begin = buf; m_end = end; }
	char &operator[](size_t i) { return m_begin[i]; }
	MutStr substr(size_t ofs, size_t len) { return MutStr(m_begin+ofs, m_begin+ofs+len); }
	MutStr substr(size_t ofs) { return MutStr(m_begin+ofs, m_end); }

	using iterator = char*;
	char *begin() { return m_begin; }
	char *end() { return m_end; }
};

struct String : MutStr {
	String() : MutStr() { }
	String(size_t n) { m_begin = (char*)malloc(n); m_end = m_begin+n; }
	String(Str s) : String(s.size()) { memcpy(begin(), s.begin(), size()); }
	static String invalid() { return String(); }
	~String() { if (m_begin) free(m_begin); }
	String(String &&s)
	{
		m_begin = s.m_begin;
		m_end = s.m_end;
		s.m_begin = nullptr;
	}
	void operator=(String &&s)
	{
		m_begin = s.m_begin;
		m_end = s.m_end;
		s.m_begin = nullptr;
	}
	void operator=(const Str &s)
	{
		if (m_begin) free(m_begin);
		m_begin = (char*)malloc(s.size());
		m_end = m_begin+s.size();
		memcpy(m_begin, s.begin(), s.size());
	}

	static String zero_terminate(Str s)
	{
		String mem(s.size()+1);
		memcpy(mem.begin(), s.begin(), s.size());
		mem.begin()[s.size()] = '\0';
		return mem;
	}
};
