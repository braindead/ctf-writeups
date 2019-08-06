/* Copyright (C) 2019 braindead <braindeaded@protonmail.com> All Rights Reserved.
 */

#include <cstdint>
#include <cstddef>
extern "C" {
#define private _private
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/quota.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <mqueue.h>
#include <keyutils.h>
#include <linux/capability.h>
#include <linux/aio_abi.h>
#undef private
}

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;
using addr = u8*;

#define __INLINE inline __attribute__ ((always_inline))

namespace sys {
	__INLINE i64 syscall(u32 sysno)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a, u64 b)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 rsi asm ("rsi") = b;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi), "r" (rsi)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a, u64 b, u64 c)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 rsi asm ("rsi") = b;
		register u64 rdx asm ("rdx") = c;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a, u64 b, u64 c, u64 d)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 rsi asm ("rsi") = b;
		register u64 rdx asm ("rdx") = c;
		register u64 r10 asm ("r10") = d;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a, u64 b, u64 c, u64 d, u64 e)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 rsi asm ("rsi") = b;
		register u64 rdx asm ("rdx") = c;
		register u64 r10 asm ("r10") = d;
		register u64 r8 asm ("r8") = e;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	__INLINE i64 syscall(u32 sysno, u64 a, u64 b, u64 c, u64 d, u64 e, u64 f)
	{
		register u32 rax asm ("eax") = sysno;
		register u64 rdi asm ("rdi") = a;
		register u64 rsi asm ("rsi") = b;
		register u64 rdx asm ("rdx") = c;
		register u64 r10 asm ("r10") = d;
		register u64 r8 asm ("r8") = e;
		register u64 r9 asm ("r9") = f;
		register u64 ret asm ("rax");
		asm volatile (
			"syscall"
			: "=r" (ret)
			: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8), "r" (r9)
			: "rcx", "r11", "memory"
		);
		return ret;
	}

	using umode_t = mode_t;
	using loff_t = off_t;
	using qid_t = int;
	using old_time32_t = time_t;
	using rwf_t = int;
	#include "linux_x86_64.gen"
}

struct String {
	template <size_t LEN>
	String(const char (&buf)[LEN]) : data(buf), size(LEN-1) { }
	const char *const data;
	const size_t size;
};

__attribute__((noreturn))
extern "C" void _start();
