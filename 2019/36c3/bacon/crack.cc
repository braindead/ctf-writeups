#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

#define INFO(...) fprintf(stderr, "[*] " __VA_ARGS__)
#define ERR(...) fprintf(stderr, "[-] " __VA_ARGS__)
#define SUCC(...) fprintf(stderr, "[+] " __VA_ARGS__)

using u32 = uint32_t;
using u64 = uint64_t;

static inline u32 rotl(u32 x, u32 w) { return ((x<<w)&0xffffff)|((x&0xffffff)>>(24-w)); }
static inline u32 rotr(u32 x, u32 w) { return ((x<<(24-w))&0xffffff)|((x&0xffffff)>>w); }

void Round(u32 &x, u32 &y, u32 k)
{
	x = rotr(x, 8);
	x += y;
	x ^= k;
	y = rotl(y, 3);
	y ^= x;
	x &= 0xffffff;
	y &= 0xffffff;
}

void InvRound(u32 &x, u32 &y, u32 k)
{
	y ^= x;
	y = rotr(y, 3);
	x ^= k;
	x -= y;
	x = rotl(x, 8);

	x &= 0xffffff;
	y &= 0xffffff;
}

void Speck_72_48(const u32 key[3], u32 blk[2])
{
	u32 x = blk[0];
	u32 y = blk[1];

	u32 l_ = key[0];
	u32 l = key[1];
	u32 k = key[2];

	for (int i = 0; i < 22; i++) {
		//printf("subkey %06x\n", (unsigned)k);
		Round(x, y, k);

		//printf("keyderiv %06x %06x %06x\n", l, k, l_);
		u32 t = (rotr(l, 8) + k) ^ i;
		k = rotl(k, 3) ^ t;
		t &= 0xffffff;
		k &= 0xffffff;
		l = l_;
		l_ = t;
	}

	blk[0] = x;
	blk[1] = y;
}

void Speck_72_48_decrypt(const u32 key[3], u32 blk[2])
{
	u32 l_ = key[0];
	u32 l = key[1];
	u32 k = key[2];

	u32 rk[21];
	for (int i = 0; i < 22; i++) {
		rk[i] = k;
		u32 t = (rotr(l, 8) + k) ^ i;
		k = rotl(k, 3) ^ t;
		t &= 0xffffff;
		k &= 0xffffff;
		l = l_;
		l_ = t;
	}

	u32 x = blk[0];
	u32 y = blk[1];

	for (int i = 21; i >= 0; i--) {
		InvRound(x, y, rk[i]);
	}

	blk[0] = x;
	blk[1] = y;
}

static int from_hexdigit(int c)
{
	if ((unsigned)(c-'0') <= 9u)
		return c - '0';
	c |= 32;
	return c - 'a' + 10;
}

static void from_hex(u32 *legs, int n, const char *hex)
{
	for (int i = 0; i < n; i++) {
		u32 t = 0;
		for (int j = 0; j < 6; j++)
			t = t*16 + from_hexdigit(*hex++);
		legs[i] = t;
	}
}

static void to_hex(char *hex, const u32 *legs, int n)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 6; j++)
			*hex++ = "0123456789abcdef"[legs[i]>>(4*(5-j))&0xf];
	}
	*hex++ = '\0';
}

static void kat()
{
	u32 key[3];
	u32 blk[2];
	from_hex(key, 3, "1211100a0908020100");
	from_hex(blk, 2, "20796c6c6172");
	char buf[32];
	to_hex(buf, key, 3);
	printf("key: %s\n", buf);
	to_hex(buf, blk, 2);
	printf("blk: %s\n", buf);

	Speck_72_48(key, blk);

	to_hex(buf, blk, 2);
	printf("out: %s\n", buf);

	Speck_72_48_decrypt(key, blk);

	to_hex(buf, blk, 2);
	printf("blk: %s\n", buf);
}

struct MCG_XSL_RR_128_64 {
	MCG_XSL_RR_128_64()
	{
		m_state = 17989591723665298623uLL;
		m_state += (unsigned __int128)17483003998085732324uLL<<64;
	}
	void seed(u32 x) { m_state = x; next64(); }

	u64 next64()
	{
		u64 out = m_state^(m_state>>64);
		u64 rot = m_state>>122;
		out = (out>>rot)|(out<<(64-rot));
		m_state *= 10685092299560468389uLL;
		return out;
	}

	u32 next32() { return next64(); }

	using HAVE_64BIT_OUTPUT = int; // for use with SFINAE
private:
	unsigned __int128 m_state;
};

using FastRNG = MCG_XSL_RR_128_64;

struct Slot {
	u32 input[2];
	u32 key[3];
};

static FastRNG g_rng;
static constexpr size_t TABLE_SIZE = 2<<24;
static constexpr size_t TABLE_MASK = TABLE_SIZE-1;
static Slot g_table[TABLE_SIZE];

static bool insert(const Slot &slot)
{
	size_t i = slot.input[0]&TABLE_MASK;
	g_table[i] = slot;
	return true;
}

static Slot *lookup(const u32 input[2])
{
	size_t i = input[0]&TABLE_MASK;
	Slot *s = &g_table[i];
	if (s->input[0] == input[0]
	 && s->input[1] == input[1]) {
		return s;
	}
	return nullptr;
}

int main(int argc, char **argv)
{
	u32 image[2];
	from_hex(image, 2, argv[1]);

	u32 m1[3];
	u32 m2[3];
	u32 m_[3] =  { 0, 0, 0x12 };

	u32 y2[2] = { image[0], image[1] };
	Speck_72_48_decrypt(m_, y2);

	if (0) {
		Speck_72_48(m_, y2);
		assert(y2[0] == image[0]);
		assert(y2[1] == image[1]);
		Speck_72_48_decrypt(m_, y2);
	}

	INFO("precomputing\n");
	u32 items = 0;
	while (items < 0x1000000) {
		u64 x = g_rng.next64();
		m2[0] = x>> 0&0xffffff;
		m2[1] = x>>32&0xffffff;
		m2[2] = g_rng.next32() & 0xffffff;

		u32 y1[2] = { y2[0], y2[1] };
		Speck_72_48_decrypt(m2, y1);
		Slot slot { { y1[0], y1[1] }, { m2[0], m2[1], m2[2] } };
		if (insert(slot))
			items += 1;
	}

	INFO("bruting\n");
	for (;;) {
		u64 x = g_rng.next64();
		m1[0] = x>> 0&0xffffff;
		m1[1] = x>>32&0xffffff;
		m1[2] = g_rng.next32() & 0xffffff;

		u32 y1[2] = { 0, 0 };
		Speck_72_48(m1, y1);
		Slot *s = lookup(y1);
		if (s) {
			SUCC("Found!\n");

			memcpy(m2, s->key, sizeof(m2));

			u32 s[2] = { 0, 0 };
			Speck_72_48(m1, s);
			Speck_72_48(m2, s);
			Speck_72_48(m_, s);
			assert(s[0] == image[0]);
			assert(s[1] == image[1]);

			char buf[64];
			to_hex(buf +  0, m1, 3);
			to_hex(buf + 18, m2, 3);
			printf("%s\n", buf);
			return 1;
		}
	}

	return 0;
}
