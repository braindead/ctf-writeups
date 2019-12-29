Intro
=====

We are given an unprivileged shell on [SerenityOS](https://github.com/SerenityOS/serenity/)
and we need to read a flag from ATA device `/dev/hdb`.

Even though we have privileges to open `/dev/hdb` for reading,
reading from it will do nothing: `PATADiskDevice::read()` method is a no-op.
Instead we use escalate our privileges to root and then load a kernel module
to call `PATADiskDevice::read_block()`.

SerenityOS 0-day kernel LPE
===========================

We will exploit two bugs:

1. syscall `writev` does not validate `iov_base` pointers, which can be used to read arbitrary kernel memory
2. TOCTOU in syscall `clock_nanosleep`, which allows us to write 4 zero bytes to arbitrary kernel memory

Exploiting `writev` is simple:

	static void memcpy_from_kernel(char *dest, u32 src, u32 n)
	{
		iovec iov { (void*)src, n };
		syscall(SC_writev, pipe_fd[1], &iov, 1);
		syscall(SC_read, pipe_fd[0], dest, n);
	}

We use the above primitive to find our `Process` structure in memory (no KASRL to worry about here!),
and then zero out it's `euid` field.

`clock_nanosleep` works like this:

	int Process::sys$clock_nanosleep(SC_clock_nanosleep_ *params)
	{
		validate params->requested_sleep;
		validate params->remaining_sleep;
		sleep for asked time;
		if (sleep as interrupted) {
			params->remaining_sleep = time left to sleep;
		}
	}

The bug is that the `params` struct is stored in userspace and can be modified by
another thread while the handler is sleeping. The kernel is full of such
vulnerabilities, but `clock_nanosleep` is the easiest to exploit, due to it's sleeping nature.

To exploit it we:

1. Create a thread that will change `remaining_sleep` to address of our euid after 1 second
2. Create a process that will wake us up after 2 seconds
3. Enter `clock_nanosleep` for 2.5 seconds

Achieving ring-0 code execution
===============================

You might think that all we need to do now is run `modload GetFlagModule.o`.
But unfortunately module loading is completely broken in SerenityOS: when loading
module's elf file it places executable code into a rw- memory segment and forgets
to make it executable before calling `module_init`.

Possible workaround is to exploit a stack BOF in `ELFImage::Section::relocations()`,
but that is complicated by stack canary with a null byte.

Instead we observe that module is loaded somewhere near `0xc0bbb000` and no range checks
are done when resolving symbols like `module_init` and `module_fini`. With an appropriate
`module_fini` offset module loader will call into our userspace process (no SMEP to worry about!).
To improve reliability we use a 20Mb nopsled.
