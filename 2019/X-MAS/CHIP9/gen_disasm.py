from opspec import Spec
import sys
from braindead import log, info, error, die
from syntax import *
log.enable()

s = Spec()
s.parse(sys.argv[1])
info('loaded %s rules', len(s.rules))

lut = ['"\\x01invalid"']*256
for pattern, asm, action in s.rules:
	b = int(pattern[0], 16)
	asm = asm.replace('xxyy', '\\x02""')
	asm = asm.replace('xxrel', '\\x01""')
	asm = asm.replace('xx', '\\x01""')
	asm = asm.replace('yy', '\\x03""')
	fmt = '"'
	fmt += f'\\x{len(pattern):02x}""'
	fmt += asm
	lut[b] = fmt+'"'

for op in range(256):
	print(lut[op], ",")
