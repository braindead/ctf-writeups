
tweetnacl.so:
	(: \
	&& curl -s https://tweetnacl.cr.yp.to/20140427/tweetnacl.h \
	&& curl -s https://tweetnacl.cr.yp.to/20140427/tweetnacl.c \
	&& echo '#include <sys/random.h>' \
	&& echo '#include <stdlib.h>' \
	&& echo 'void randombytes(u8 *p, u64 n)' \
	&& echo '{ for (ssize_t r = 0; n -= r; p += r)' \
	&& echo 'if (0 > (r = getrandom(p, n, 0))) exit(-1); }' \
	)| \
	fgrep -v tweetnacl.h | \
	gcc -shared -O2 -Wl,-s -fpic -xc - -o tweetnacl.so

clean:
	rm -f tweetnacl.so

