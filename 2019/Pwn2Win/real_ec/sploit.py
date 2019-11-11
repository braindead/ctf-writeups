#!/usr/bin/python3 -u

from fastecdsa import keys, curve
from fastecdsa.point import Point
import hashlib
import signal
import sys
import struct
import random
import os
import hashlib
import re
from braindead import *
log.enable()
Args().parse()

cracker = io.process(['./cracc'], stderr='pass')
info('waiting for precomputation to finish')
cracker.recvuntil('READY\n')
cracker.send(le.p32(4)) # worker count

conn = io.connect(('167.172.225.246', 1337))
pks = conn.recvline().decode()
nums = re.compile('0x[0-9a-f]*').findall(pks)
for i in range(0, len(nums), 2):
	x, y = map(lambda x: int(x, 16), nums[i:i+2])
	buf = le.p32(i) + x.to_bytes(32, 'big') + y.to_bytes(32, 'big')
	cracker.send(buf)

info('waiting for keys')
outputs = []
for _ in range(250):
	resp = cracker.recvline().decode().split()
	tag = int(resp[0])
	if resp[1] != 'OK':
		die("Failed to take a logarithm")
	s = int(resp[2])
	info('recovered: %u %x', tag, s)
	outputs.append((tag, s))
info('outputs: %s', len(outputs))

outputs.sort()
recovered_secret = bytearray()
for _, x in outputs:
	recovered_secret += be.p32(x)
proof = hashlib.sha256(recovered_secret).hexdigest()
success('proof: %s', proof)

conn.sendafter(': ', proof+'\n')

io.interactive(conn)
