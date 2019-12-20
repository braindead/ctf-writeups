#pragma once
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "config.h"

#ifdef CONFIG_HAVE_INT128
using Hash = uint64_t;

struct Hasher {
	Hasher()
	{
		m_state = 0x42d0a2de2c9d5892UL;
	}

	void push64(uint64_t w)
	{
		w ^= 0x851d875b539ab275;
		__uint128_t m = (__uint128_t)w*m_state;
		uint64_t lo = m;
		uint64_t hi = m>>64;
		m_state = lo ^ hi;
	}

	void push32(uint32_t w) { push64(w); }
	void push16(uint16_t w) { push64(w); }
	void push8(uint8_t w) { push64(w); }

	void push_mem(const uint8_t *buf, size_t len)
	{
		const uint8_t *it = buf;
		const uint8_t *end = it + (len&~7);
		while (it < end) {
			uint64_t w;
			memcpy(&w, it, 8);
			push64(w);
			it += 8;
		}

		uint64_t w = 0;
		size_t r = len&7;
		while (r--)
			w = (w<<8) + *it++;
		push64(w);
	}

	Hash digest()
	{
		return m_state;
	}
private:
	unsigned long long m_state;
};
#else
using Hash = uint32_t;

struct Hasher {
	Hasher()
	{
		m_state = 0xb2ee44eeUL;
	}


	void push32(uint32_t w)
	{
		w ^= 0x895e7500UL;
		uint64_t m = (uint64_t)w*m_state;
		uint32_t lo = m;
		uint32_t hi = m>>32;
		m_state = lo ^ hi;
	}

	void push64(uint64_t w) { push32(w); push32(w>>32); }
	void push16(uint16_t w) { push32(w); }
	void push8(uint8_t w) { push32(w); }

	void push_mem(const uint8_t *buf, size_t len)
	{
		const uint8_t *it = buf;
		const uint8_t *end = it + (len&~3);
		while (it < end) {
			uint32_t w;
			memcpy(&w, it, 4);
			push32(w);
			it += 4;
		}

		uint32_t w = 0;
		size_t r = len&3;
		while (r--)
			w = (w<<8) + *it++;
		push32(w);
	}

	Hash digest()
	{
		return m_state;
	}
private:
	unsigned long long m_state;
};
#endif

inline Hasher &operator<<(Hasher &h, int i)
{ h.push32(i); return h; }
