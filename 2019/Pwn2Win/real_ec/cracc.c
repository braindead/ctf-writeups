/* Author: braindead
 * Team: DCUA
 * CTF: Pwn2Win
 * Task: real_ec
 */

#include "ecp_NIST256.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define info(...) fprintf(stderr, "[*] " __VA_ARGS__)
#define pdie(...) (fprintf(stderr, "[-] " __VA_ARGS__, strerror(errno)), exit(1))

static void hash(ECP_NIST256 *p, uint64_t h[2])
{
	if (ECP_NIST256_isinf(p)) {
		h[0] = h[1] = 0;
		return;
	}
	//ECP_NIST256 w;
	//ECP_NIST256_copy(&w, p);
	ECP_NIST256_affine(p);
	BIG_256_56 b;
	FP_NIST256_redc(b, &p->x);
	h[0] = b[0];
	h[1] = b[1];
}

static uint64_t hash1(ECP_NIST256 *p)
{
	uint64_t h[2];
	hash(p, h);
	return h[0];
}

typedef struct Ent {
	uint64_t hash[1];
	uint32_t exponent;
} Ent;

#define LUTBITS 25

#define MASK (((1<<LUTBITS)*2)-1)
static Ent g_hashtab[MASK+1];
static int load;

static Ent *lookup(ECP_NIST256 *p)
{
	uint64_t h[2];
	hash(p, h);

	uint64_t it = h[0];
	for (;;) {
		Ent *e = &g_hashtab[it&MASK];
		if (! (e->exponent>>31&1))
			return NULL;
		if (e->hash[0] == h[0])
			return e;
		it += 1;
	}
}

static void insert(ECP_NIST256 *p, uint32_t exp, uint8_t msk)
{
	uint64_t h[2];
	hash(p, h);

	Ent insdata;
	insdata.hash[0] = h[0];
	//insdata.hash[1] = h[1];
	insdata.exponent = exp|0x80000000u;

	uint64_t it = h[0];
	while (g_hashtab[it&MASK].exponent&0x80000000u) {
		if (g_hashtab[it&MASK].hash[0] == h[0])
			info("holy shit, a collision\n");
		it += 1;
	}
	g_hashtab[it&MASK] = insdata;
}

static ECP_NIST256 g_big_step;
#define NBASES 256
static ECP_NIST256 g_bases[NBASES];
static BIG_256_56 g_base_div[NBASES];

static int crack(uint32_t *s_, ECP_NIST256 *pk)
{
	ECP_NIST256 it;
	for (int msk = 1; msk < NBASES; msk++) {
		ECP_NIST256_copy(&it, pk);
		ECP_NIST256_mul(&it, g_base_div[msk]);
		for (int i = 0; i < 1<<(32-LUTBITS); i++) {
			if (0 && i < 10) {
				info("looking up %016lx\n", hash1(&it));
			}
			Ent *e = lookup(&it);
			if (e) {
				*s_ = (i<<LUTBITS) + (e->exponent&0x7fffffff);
				return 1;
			}
			ECP_NIST256_sub(&it, &g_big_step);
		}
	}
	return 0;
}

int main()
{
	setvbuf(stdin, 0, _IONBF, 0);
	setvbuf(stdout, 0, _IONBF, 0);
	setvbuf(stderr, 0, _IONBF, 0);

	ECP_NIST256 g_shifts[8];
	ECP_NIST256_generator(&g_shifts[0]);
	for (int i = 1; i < 8; i++) {
		ECP_NIST256_copy(&g_shifts[i], &g_shifts[i-1]);
		for (int j = 0; j < 32; j++) {
			ECP_NIST256_dbl(&g_shifts[i]);
		}
	}

	ECP_NIST256_generator(&g_big_step);
	for (int j = 0; j < LUTBITS; j++) {
		ECP_NIST256_dbl(&g_big_step);
	}

	BIG_256_56 order;
	BIG_256_56_fromBytes(order,
		"\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xbc"
		"\xe6\xfa\xad\xa7\x17\x9e\x84\xf3\xb9\xca\xc2\xfc""c%Q"
	);

	for (int msk = 1; msk < NBASES; msk++) {
		ECP_NIST256 base;
		ECP_NIST256_inf(&base);
		BIG_256_56 bmul, bit, t;
		BIG_256_56_one(bit);
		BIG_256_56_zero(bmul);
		for (int i = 0; i < 8; i++) {
			if (msk>>i&1) {
				ECP_NIST256_add(&base, &g_shifts[i]);
				BIG_256_56_copy(t, bmul);
				BIG_256_56_add(bmul, t, bit);
			}
			BIG_256_56_shl(bit, 32);
		}
		g_bases[msk] = base;
		BIG_256_56_invmodp(g_base_div[msk], bmul, order);
	}

	struct stat cache;
	int have_cache = true;
	if (stat("g_hashtab.cache", &cache) < 0) {
		if (errno == ENOENT) {
			info("cache does not exist\n");
			have_cache = false;
		} else {
			pdie("stat g_hashtab.cache: %s");
		}
	}

	if (! have_cache) {
		ECP_NIST256 it;
		ECP_NIST256_inf(&it);
		for (int i = 0; i < 1<<(LUTBITS); i++) {
			if (0 && i < 10) {
				info("inserting %d <- %016lx\n", i, hash1(&it));
			}
			insert(&it, i, 0);
			assert(lookup(&it) != NULL);
			ECP_NIST256_add(&it, &g_shifts[0]);
		}
		int fd = open("g_hashtab.cache", O_WRONLY|O_TRUNC|O_CREAT, 0666);
		if (fd < 0)
			pdie("open: %s");
		ssize_t r = write(fd, g_hashtab, sizeof g_hashtab);
		if (r != sizeof g_hashtab)
			pdie("write: %s");
		close(fd);
	} else {
		int fd = open("g_hashtab.cache", O_RDONLY);
		ssize_t r = read(fd, g_hashtab, sizeof g_hashtab);
		if (r != sizeof g_hashtab)
			pdie("read: %s");
		close(fd);
	}

	puts("READY");
	uint32_t nfork = 1;
	if (4 != read(0, &nfork, 4))
		pdie("read: %s");
	for (int i = 1; i < nfork; i++) {
		pid_t p = fork();
		if (p < 0)
			pdie("fork: %s");
		if (p == 0)
			break;
	}

	for (;;) {
		char buf[4+64];
		ssize_t r = read(0, buf, 4+64);
		if (r == 0)
			break;
		if (r < 0) {
			pdie("read: %s");
		}
		uint32_t tag = *(uint32_t*)buf;
		ECP_NIST256 pk;
		BIG_256_56 x, y;
		BIG_256_56_fromBytes(x, buf+4);
		BIG_256_56_fromBytes(y, buf+4+32);
		ECP_NIST256_set(&pk, x, y);
		uint32_t s;

		if (crack(&s, &pk))
			printf("%u OK %u\n", tag, s);
		else
			printf("%u FAIL\n", tag);
	}
	return 0;
}
