from braindead import *
from collections import defaultdict
log.enable()
args = Args()
args.parse()

KEY_MASK = 0x0ffffffc0ffffffc0ffffffc0fffffff
MASK = (1<<128)-1
PRIME = 2**130 - 5

def to_blocks(m):
	for i in range(0, len(m), 16):
		c = int.from_bytes(m[i:i+16]+b'\x01', 'little')
		yield c

def onetimeauth(r, s, m):
	rbar = int.from_bytes(r, 'little') & KEY_MASK
	p = 2**130 - 5
	h = 0
	for c in to_blocks(m):
		h = (h+c)*rbar%p
	h += int.from_bytes(s, 'little')
	return (h&((1<<128)-1)).to_bytes(16, 'little')

def pure_poly1305(r, s, m):
	assert (r&KEY_MASK) == r
	p = 2**130 - 5
	h = 0
	for c in m:
		h = (h+c)*r%p
	return (h+s)&((1<<128)-1) # no mod p!

debug_key = bytes.fromhex('eb2af171e10064239bd05efb4557d73b8a412d64b116adca6e176ad554c6c95f')

def div(a, b):
	assert b != 0
	return a*pow(b, -1, PRIME)%PRIME

def recover(r1, s1, t1, r2, s2, t2):
	if r1 == r2:
		raise LuckException()
	t1 = (t1-s1)&MASK
	t2 = (t2-s2)&MASK
	t1 = div(t1, r1)
	t2 = div(t2, r2)
	d = (r1-r2)%PRIME
	m1 = div((t1-t2)%PRIME, d)
	m2 = (t1 - m1*r1)%PRIME
	m2_ = (t2 - m1*r2)%PRIME

	if m2 != m2_:
		raise LuckException()

	m1 = m1.to_bytes(17, 'little')
	m2 = m2.to_bytes(17, 'little')
	if m1[16] != 1:
		raise LuckException()
	if m2[16] != 1:
		raise LuckException()
	return m1[:16] + m2[:16]

def solve(pairs, chall):
	secrets = defaultdict(lambda: 0)
	for rs1, t1 in pairs:
		r1 = int.from_bytes(rs1[:16], 'little') & 0x0ffffffc0ffffffc0ffffffc0fffffff
		s1 = int.from_bytes(rs1[16:], 'little')
		t1 = int.from_bytes(t1, 'little')
		for rs2, t2 in pairs:
			r2 = int.from_bytes(rs2[:16], 'little') & 0x0ffffffc0ffffffc0ffffffc0fffffff
			s2 = int.from_bytes(rs2[16:], 'little')
			t2 = int.from_bytes(t2, 'little')
			try:
				secrets[recover(r1, s1, t1, r2, s2, t2)] += 1
			except LuckException:
				pass
	secret, evidence = max(secrets.items(), key=lambda x: x[1])
	success('secret: %s', secret.hex())
	success('evidence: %s', evidence)

	ans = onetimeauth(chall[:16], chall[16:], secret)
	return ans

if False:
	s = '''
	3b3eed915b73f80d8bd8c981c9662e916d5a910bc25813c382c307e1e999a165 3676f5fbecb9e7d812c22c6d3fae9f50
	3c365d79d0d068e89b7b1aad4cfd76c0afe0b0326c4b82272a3970921109740f 1001f9993c8cc948ce3e3987555e8313
	2192d5d31853a832519f3a9a794fe4540c2dd30c107fcd264ff033bd41c85851 016eaf15e5a33e2755377a165c27e5da
	aa0097ff20a8e055687414289ba0870e64fc9bf182ed37b012accd6279748bba 9e7fac56a936f3503cafec9b6e51f796
	76ee97d4b9662e542b45ce1ef1cfc1b19ecb7d2769121fd62c75aa4bcecab857 67719d4e5d41613312c488e78cc30b81
	d20715fef64bcd7fd30557ea305ba5cd3108bcd602239377266f2bcb9a34d28f 1581d9fdb16776274fa0cb9b3a1ca07f
	176df23042ef8ad9df7fce22162d3b3e0ea210e130ed3ac000ac7df6e45ac1a6 092f5bb07091b1c637614789abe71be6
	af017e4607c2551ff28dcc10353dfd748ccd637b77cb11e65f385ac1694609e6 8d67bf4aee03584519cdceaf089e422a
	b7e1dadef18977a75638e1ab36234f6729f3eaf0b550c927694ed674af3284ac 2381d1caecc38ea3eb0ff1697cde3d93
	b428c86c8e9248e87b54f602213fbb28012b62dcee5715e2bc09c204e5e378af d013b0b4ffa4102bb3c28674dbca8300
	390eb70f00f70ae809ad791cdfe2e25164a47cb968d19786cd45c4d1f37b8b95 daf22f10710512b87d7b37ed8c4fd1aa
	25885c77f2ddd8ec97c83cb3d161ce7520b610deca41d33c585f173f233a8922 815cb9b852401dd8d6493e8c9762fac4
	280bbdc33d5c37ce7473f8f86c44e668759cf432c46efec046bf21cb906ba25b e21eae5c48ba3ab4cab9182738bfd75a
	854b6d20f24dac7a03a1f8c4c37b4e361141fdbf9190951c2f4b54a1f611193a f200f7835abdc8b2a404363e3fded6ac
	3248c81f4fe42f1110c0cd8eb07362bac011fae12cc30402aa6e253234afe271 e2138753d4dd88fe42e59fcbcccca279
	88784047cd0d49ac4ccbd74d9bb5b6f4652d1837ef9d6c1a0b67cc379a80a99c 0226f2eea43442dfee00caa7642b1e3f
	d48ab6ffb9e11754a1419fa3b2a658e93ccf8c25ffed3abddbae64b174ed5590 93bf48519a4db83371109fbc56c42d6a
	c8052068edd20ac61885081fb46cb6f88384ab1e723c25eeb3bbeb6dc401c3b4 d284b2e4873a280ad5a4b7acc8bec246
	5c7d548115cb8884b75533b5ad0a5ac3261594c07b132a61ac037517247db6d2 36cb67f848e19c68624652b6939f5e24
	16a516c381b32f8f98432ae7381fafb4b36488afd19bae665665091afe58aef5 7d228162d4dc3a688c526a1395449156
	2e8c340f129ca66ffe5d138752ebf359cd759fc45fc58bd2f43bf478879c60d6 33afca7a2e7247bee2bd7025e61fd7b6
	7f785a3a9e932568253d9881c6f5cb8bf8758835e55501883867696bec6b186b 5a9be3a171afe86116946691bc4154cd
	227e09232549a2e4ae6d46cde7181de29f0829ec1b76677db2880ee7ac0f0cc3 65af18566dc6bf0fdf1b1471bf08f755
	61fe4b6b60e7d2abed58a9047b011dd91d219b91d408b279b8d51354d14be4c8 e2635a830e251e7a186520c2db9d757c
	9e59c4f6788b6d7fde11f1b6a02d7d85cfb3ac2f37a1bb69592bad02cb439e7d 8160c53bd0d21438ff3858b8219efabd
	d73a3b2c27049465701d2501e8c54608bf88d03aed627430cf5ab4df25aa328f 719a2ad9a8bd4a3c194f47b715f40611
	7af3142bca14fb61cd95dc227092a79fee9d8e713fbd50ff63923c07a926a558 82477d0d032a88e903fb8ce71b1756ae
	7e06b306f192ead6c1af5aa5fd8df385046b625ea9852c12c69887a9640f6236 b4e93ccea35f18b8d2a7f107596d7431
	3fbd686c8161f7c363143134ca560064ca487302caddc1347f00f8bb1abb964d 3d5f95550d6a27bf3da4b2e8121e918c
	b8973ced589a3fad5d38e5becaad8b692971e959bae72fb68d840ee2fe4df02f 52fa89cf4dea5818487cbc6e64d77801
	83d50539d5c7941a6e1e0b25f658dd721005800f77d36c5cfbeaa6771d983114 3cbb97ae5129e8f5ca3d5fb8c6becccf
	730f37f41dbb141e32367d656190d9cd92a2306df0bd773070b5f29f1cf849d9 f6b04b6bae442ca5f2a0e1c869e0e944
	0610392a8bb9f7fb3167a23b72850465facc6ebf76c3e93eeeaae498ac7b725b ????????????????????????????????
	'''

	pairs = []
	for l in s.split('\n'):
		ts = l.split()
		if len(ts) == 0:
			continue
		if ts[1] == '????????????????????????????????':
			chall = ts[0]
		else:
			pairs.append((bytes.fromhex(ts[0]), bytes.fromhex(ts[1])))
	ans, _ = solve(pairs, chall)
	print(ans.hex())

r = io.connect(('88.198.156.141', 2833))
pairs = []
while True:
	ts = r.recvline().decode().split()
	if ts[1] == '????????????????????????????????':
		chall = bytes.fromhex(ts[0])
		break
	else:
		pairs.append((bytes.fromhex(ts[0]), bytes.fromhex(ts[1])))
ans = solve(pairs, chall)
r.sendline(ans.hex())
io.interactive(r)
