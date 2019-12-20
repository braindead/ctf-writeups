print('''
20 xx : LDI B, xx     : B <- xx
30 xx : LDI C, xx     : C <- xx
40 xx : LDI D, xx     : D <- xx
50 xx : LDI E, xx     : E <- xx
60 xx : LDI H, xx     : H <- xx
70 xx : LDI L, xx     : L <- xx
80 xx : LDI (HL), xx  : (HL) <- xx
90 xx : LDI A, xx     : A <- xx

21 yy xx : LDX BC, xxyy : BC <- xxyy
31 yy xx : LDX DE, xxyy : DE <- xxyy
41 yy xx : LDX HL, xxyy : HL <- xxyy
22 yy xx : LDX SP, xxyy : SP <- xxyy

81 : PUSH B :    { SP <- SP-1; (SP) <- B; }
91 : PUSH C :    { SP <- SP-1; (SP) <- C; }
A1 : PUSH D :    { SP <- SP-1; (SP) <- D; }
B1 : PUSH E :    { SP <- SP-1; (SP) <- E; }
C1 : PUSH H :    { SP <- SP-1; (SP) <- H; }
D1 : PUSH L :    { SP <- SP-1; (SP) <- L; }
C0 : PUSH (HL) : { SP <- SP-1; (SP) <- (HL); }
D0 : PUSH A :    { SP <- SP-1; (SP) <- A; }

51 : PUSH BC :   { SP <- SP-2; (SP) <- C; (SP+1) <- B }
61 : PUSH DE :   { SP <- SP-2; (SP) <- E; (SP+1) <- D }
71 : PUSH HL :   { SP <- SP-2; (SP) <- L; (SP+1) <- H }

82 : POP B :    { B <- (SP)   ; SP <- SP+1; }
92 : POP C :    { C <- (SP)   ; SP <- SP+1; }
A2 : POP D :    { D <- (SP)   ; SP <- SP+1; }
B2 : POP E :    { E <- (SP)   ; SP <- SP+1; }
C2 : POP H :    { H <- (SP)   ; SP <- SP+1; }
D2 : POP L :    { L <- (SP)   ; SP <- SP+1; }
C3 : POP (HL) : { (HL) <- (SP); SP <- SP+1; }
D3 : POP A :    { A <- (SP)   ; SP <- SP+1; }

52 : POP BC :   { C <- (SP); B <- (SP+1); SP <- SP+2 }
62 : POP DE :   { E <- (SP); D <- (SP+1); SP <- SP+2 }
72 : POP HL :   { L <- (SP); H <- (SP+1); SP <- SP+2 }
''')

def alu(o, l, r, op, flags):
	code = '{'
	if 'H' in flags:
		code += f'alu_temp <- {{{l}&15}} {op} {{{r}&15}};'
		code += 'FLAG_H <- {alu_temp&240 == 0} == 0;'
	if o in ('HL', 'BC', 'DE',):
		code += f'alu_temp <- {l}&65535 {op} {r}&65535;'
		if 'C' in flags:
			code += 'FLAG_C <- {alu_temp&4294901760 == 0} == 0;'
	else:
		code += f'alu_temp <- {l}&255 {op} {r}&255;'
		if 'C' in flags:
			code += 'FLAG_C <- {alu_temp&4294967040 == 0} == 0;'
	code += f'alu_temp <- {l} {op} {r};'
	code += f'{o} <- alu_temp;'
	if 'Z' in flags:
		code += f'FLAG_Z <- {o}&255 == 0;'
	if 'N' in flags:
		code += f'FLAG_N <- {{{o}&128}} == 128;'
	return code+'}'

for i, rr in enumerate(['BC', 'DE', 'HL', ['(HL)', 'A']]):
	for j in range(2):
		for si, s in enumerate([*'BCDEHL', '(HL)', 'A']):
			op = 0x09+i+j*0x80+si*16
			d = rr[j]
			if d == '(HL)' and d == s:
				pass # handled later
			else:
				print(f'{op:02X} : MOV {d}, {s} : {d} <- {s}')

print('''
ED : MOV HL, BC : HL <- BC
FD : MOV HL, DE : HL <- DE

08 : CRLFLAG : FLAG_Z <- 0; FLAG_N <- 0; FLAG_H <- 0; FLAG_C <- 0
''')

for i, f in enumerate('ZNHC'):
	for val in range(1, -1, -1):
		op = 0x18+(1-val)*16+i*32
		print(f'{op:02X} : SETFLAG {f}, {val} : FLAG_{f} <- {val}')
print()

print(f'''
83 : ADDX BC : {alu("BC", "BC", "A", "+", "ZNHC")}
93 : ADDX DE : {alu("DE", "DE", "A", "+", "ZNHC")}
A3 : ADDX HL : {alu("HL", "HL", "A", "+", "ZNHC")}
''')

for m, b, base in [
	('ADD', '+', 0x04),
	('SUB', '-', 0x84),
	('AND', '&', 0x05),
	('OR',  '|', 0x85),
	('XOR', '^', 0x06),
	]:
	for i, d in enumerate([*'BCDEHL', '(HL)', 'A']):
		op = base + i*16
		print(f'{op:02X} : {m} {d} : {alu(d, d, "A", b, "ZNHC")}')

print(f'''
A7 xx : ADDI xx : {alu("A", "A", "xx", "+", "ZNHC")}
B7 xx : SUBI xx : {alu("A", "A", "xx", "-", "ZNHC")}
C7 xx : ANDI xx : {alu("A", "A", "xx", "&", "ZNHC")}
D7 xx : ORI xx  : {alu("A", "A", "xx", "|", "ZNHC")}
E7 xx : XORI xx : {alu("A", "A", "xx", "^", "ZNHC")}
''')


for m, b, base in [
	('INC', '+', 0x03),
	('DEC', '-', 0x07),
	]:
	for i, d in enumerate([*'BCDEHL', '(HL)', 'A']):
		op = base + i*16
		print(f'{op:02X} : {m} {d} : {alu(d, d, "1", b, "ZNHC")}')

print('''
A8 : INX BC : BC <- BC + 1
B8 : INX DE : DE <- DE + 1
C8 : INX HL : HL <- HL + 1
''')

for i, s in enumerate([*'BCDEHL', '(HL)', 'A']):
	op = 0x86 + i*16
	print(f'{op:02X} : CMP {s} : FLAG_Z <- A == {s}; FLAG_N <- {s}&255 < A&255')

print('''
F7 xx : CMPI A, xx : FLAG_Z <- A == xx; FLAG_N <- A&255 < xx&255
''')

for i, s in enumerate([*'BCDEHL', '(HL)', 'A']):
	op = 0x0D + i*16
	print(f'{op:02X} : CMPS {s} : FLAG_Z <- A == {s}; FLAG_N <- {s} < A')

print('''
E0 : SIN  : SIN
E1 : SOUT : SOUT

F0 : CLRSCR : CLRSCR
F1 : DRAW   : DRAW
''')

print('''
0F yy xx : JMP xxyy : PC <- xxyy
''')

for fi, f in enumerate('ZNHC'):
	for neg in range(2):
		op = 0x1F + 32*fi + neg*16
		v = 1-neg
		cc = 'N'+f if neg else f
		print(f'{op:02X} yy xx : JMP{cc} xxyy : PC <- mux {{FLAG_{f} == {v}}} xxyy PC')


print('''
9F xx : JMP xxrel : PC <- PC + xx
''')

for fi, f in enumerate('ZNHC'):
	for neg in range(2):
		op = 0xAF + 32*fi + neg*16
		if op >= 0x100:
			op -= 0x21
		v = 1-neg
		cc = 'N'+f if neg else f
		print(f'{op:02X} xx : JMP{cc} xxrel : PC <- mux {{FLAG_{f} == {v}}} {{PC + xx}} PC')

print('''
1E yy xx : CALL xxyy : { SP <- SP-2; (SP) <- PCl; (SP+1) <- PCh; PC <- xxyy }
0E : RET : { PCl <- (SP); PCh <- (SP+1); SP <- SP+2; }
00 : NOP : {}
6C : HCF : HALT
''')

if False:
	# perfect blue extensions
	print('''
	F2 : TRAP : TRAP
	F3 : ASR : FLAG_C <- A&1; A <- A/2; FLAG_Z <- A; FLAG_H <- 0; FLAG_N <- 0
	''')
