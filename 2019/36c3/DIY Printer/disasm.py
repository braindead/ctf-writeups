import functools
from pysmt.shortcuts import *
import sys

bytecode = "P003I013WI015O1O1RTO2TO2QBO2DE1E1I013O1P000BO1TO1P002LBO1TDE1E1E1I008P003LI004P005LO1P000BO1TO1P002LBO1TDE1E1E1I005O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1I010TI013O1O1NTP000BO2O2TQDO2TO2NDE1E1I002I005O1O1TNO2TO2NDO2O2NP002LDE1E1O1P000BO1TO1P002LBO1TDE1E1E1I009O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1I005P019I012VI003VO1O1TNO2TO2NDO2O2NP002LDE1E1I000O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P004I004VI014I004O1O1TNO2TO2NDO2O2NP002LDE1E1O1P000BO1TO1P002LBO1TDE1E1E1I012O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P003I001WI004I003O1O1RTO2TBO2DE1E1O1P000BO1TO1P002LBO1TDE1E1E1I014O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P014I005I007O1O1RTO2TBO2DE1E1O1O1RTO2TBO2DE1E1I013O1O1RTO2TBO2DE1E1I011O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P001I006O1O1NTP000BO2O2TQDO2TO2NDE1E1I001O1O1TNO2TO2NDO2O2NP002LDE1E1P001I007O1O1NTP000BO2O2TQDO2TO2NDE1E1I002WO1O1RTO2TO2QBO2DE1E1I007O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P011P001I013I014O1O1RTO2TBO2DE1E1HO1O1RTO2TBO2DE1E1I015O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1I012I006O1P000BO1TO1P002LBO1TDE1E1E1I012O1O1RTO2TBO2DE1E1I006O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P002I001WP002I014WO1O1RTO2TBO2DE1E1P001HI001O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1I004I001I003O1P000BO1TO1P002LBO1TDE1E1E1O1O1TNO2TO2NDO2O2NP002LDE1E1I008O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P001P001I009I000O1O1RTO2TBO2DE1E1WHI003O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P003I010I009O1P000BO1TO1P002LBO1TDE1E1E1HI004O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1P007I006O1O1NTP000BO2O2TQDO2TO2NDE1E1P001I010TO1O1RTO2TBO2DE1E1HI010O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1I012I004O1O1RTO2TBO2DE1E1I002O1P000BO1TO1P002LBO1TDE1E1E1O1O1RTO2TO2QBO2DE1E1"

def disasm(pc):
	x = bytecode[pc]
	pc += 1

	if x == 'P':   # -- imm
		return pc+3, 'push', int(bytecode[pc:pc+3])
	elif x == 'I': # -- chr
		return pc+3, 'push_input_char', int(bytecode[pc:pc+3])
	elif x == 'D': # b a -- a+b
		return pc+0, 'add'
	elif x == 'B': # b a -- a-b
		return pc+0, 'sub'
	elif x == 'L': # b a -- a*b
		return pc+0, 'mul'
	elif x == 'V': # b a -- a/b  guessed
		return pc+0, 'div'
	elif x == 'R': # b a -- a^b
		return pc+0, 'xor'
	elif x == 'Q': # b a -- a|b
		return pc+0, 'or'
	elif x == 'N': # b a -- a&b
		return pc+0, 'and'
	elif x == 'H': # b a -- a<<b
		return pc+0, 'sll'
	elif x == 'W': # b a -- a>>b
		return pc+0, 'slr'
	elif x == 'T': # a -- ~a
		return pc+0, 'not'
	elif x == 'O': # aI ... -- aI ... aI
		return pc+1, 'pick', int(bytecode[pc:pc+1])
	elif x == 'E': # aI ... -- ...
		return pc+1, 'nip', int(bytecode[pc:pc+1])
	else:
		return pc, 'BAD OPCODE', x

BV8 = BVType(8)

problem = TRUE()

password = []
for i in range(16):
	p = Symbol(f'p{i}', BV8)
	password.append(p)
	problem = And(problem, BVULE(BV(20, 8), p))
	problem = And(problem, BVULT(p, BV(127, 8)))

stack = []
pos = 0
while pos < len(bytecode):
	npos, op, *args = disasm(pos)
	print(f'{len(stack):02}', f'{pos:03}', op, *args)
	pos = npos

	if op == 'push': stack.append(BV(args[0], 8))
	elif op == 'push_input_char': stack.append(password[args[0]])
	elif op == 'add': a = stack.pop(); b = stack.pop(); stack.append(a + b)
	elif op == 'sub': a = stack.pop(); b = stack.pop(); stack.append(a - b)
	elif op == 'mul': a = stack.pop(); b = stack.pop(); stack.append(a * b)
	elif op == 'div': a = stack.pop(); b = stack.pop(); stack.append(a / b)
	elif op == 'xor': a = stack.pop(); b = stack.pop(); stack.append(a ^ b)
	elif op == 'or' : a = stack.pop(); b = stack.pop(); stack.append(a | b)
	elif op == 'and': a = stack.pop(); b = stack.pop(); stack.append(a & b)
	elif op == 'sll': a = stack.pop(); b = stack.pop(); stack.append(BVLShl(a, b))
	elif op == 'slr': a = stack.pop(); b = stack.pop(); stack.append(BVLShr(a, b))
	elif op == 'not': a = stack.pop(); stack.append(BVNot(a))
	elif op == 'pick': stack.append(stack[-1-args[0]])
	elif op == 'nip': stack.pop(-1-args[0])
print(len(stack))

problem = And(problem, Equals(BVZero(8), stack[0]))
write_smtlib(problem, 'problem.smtlib')

with Solver('cvc4', 'QF_BV') as s:
	if not s.is_sat(problem):
		print('not sat')
		sys.exit(1)
	m = s.get_model()
	ps = ''
	for p in password:
		ps += chr(m.get_value(p).constant_value())
	print(repr(ps))
