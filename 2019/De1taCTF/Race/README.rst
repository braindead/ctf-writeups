Overview
========

We have a race condition that causes arbitrary size kmalloc UAF.
Instead of leaking a heap pointer and then using ret2dir to upload a shellcode,
we are just going to poison the "kmalloc-192" kmem_cache with a garbage pointer like 0xdeadbeef.
After our exploit exits (or crashes), we just type "exit" into our unprivileged shell
and magically get a root shell back!

Why this works
==============

After we exit, the init script /etc/init.d/rcS will poweroff the VM. However,
the broken kmalloc-192 cache causes the reboot() syscall to panic, which SIGKILLs
/bin/poweroff and leaves us with a root shell!

Some other syscalls like sendfile() are also broken, so ``cat /flag`` won't work.
This can be remedied by using ``od -x /flag`` instead.
