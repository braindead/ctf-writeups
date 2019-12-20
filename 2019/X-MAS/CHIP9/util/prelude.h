#pragma once
#include "config.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>

int app_main(int argc, char **argv, char **environ);

#if CONFIG_NOSTD

namespace std {
	template <typename T>
	struct remove_reference { typedef T type; };
	template <typename T>
	struct remove_reference<T&> { typedef T type; };

	template <typename T>
	typename remove_reference<T>::type&&
	move(T &&arg)
	{
		using R = typename remove_reference<T>::type&&;
		return static_cast<R>(arg);
	}

	template <typename T>
	typename remove_reference<T>::type&&
	forward(typename remove_reference<T>::type &arg)
	{
		using R = typename remove_reference<T>::type&&;
		return static_cast<R>(arg);
	}

	template <typename T>
	typename remove_reference<T>::type&&
	forward(typename remove_reference<T>::type &&arg)
	{
		using R = typename remove_reference<T>::type&&;
		return static_cast<R>(arg);
	}

	template <typename T>
	void swap(T &a, T &b)
	{
		T temp = a;
		a = b;
		b = temp;
	}
}

void *operator new(size_t n);
inline void *operator new(size_t n, void *p) { return p; }
void operator delete(void *x) throw();
void operator delete(void *x, size_t n);
void *operator new[](size_t n);
void operator delete[](void *x) throw();

#else
#include <utility>
#include <new> // for placement new
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

template <typename T>
constexpr T min(T a, T b) { return b < a ? b : a; }

template <typename T>
constexpr T max(T a, T b) { return b > a ? b : a; }

#define UNREACHABLE() assert(0);

inline u64 pow2_ceil(u64 x)
{
	x -= 1;
	x |= x>>1;
	x |= x>>2;
	x |= x>>4;
	x |= x>>8;
	x |= x>>16;
	x |= x>>32;
	x += 1;
	return x;
}
