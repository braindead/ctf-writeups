from opspec import Spec
import sys
from braindead import log, info, error, die
from syntax import *
log.enable()
log.root_logger.setLevel(log.logging.WARNING)

temp_ctr = 0
def temp(out, val, bits=32):
	global temp_ctr
	temp_ctr += 1
	out.append(f'i{bits} _t{temp_ctr} = {val};')
	return f'_t{temp_ctr}'

def rvalue(out, ast):
	info('rvalue %s', ast)
	if ast.is_a(Form) and len(ast.elements) == 1:
		return rvalue(out, ast.elements[0])
	if ast.is_a(Num):
		return str(ast.val)
	elif ast.is_a(Block):
		for stmt in ast.statements:
			walk(out, stmt)
		return rvalue(out, ast.val)
	if ast.is_a(Form):
		op = ast.elements[0]
		if op.is_the(Op, '-'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) - ({r})')
		if op.is_the(Op, '+'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) + ({r})')
		if op.is_the(Op, '<'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) < ({r})')
		if op.is_the(Op, '/'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) / ({r})')
		if op.is_the(Op, '&'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) & ({r})')
		if op.is_the(Op, '|'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) | ({r})')
		if op.is_the(Op, '^'):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) ^ ({r})')
		if op.is_the(Op, '=='):
			l = rvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			return temp(out, f'({l}) == ({r})')
		if op.is_a(SymRef) and str(op) == 'mux':
			c = rvalue(out, ast.elements[1])
			t = rvalue(out, ast.elements[2])
			f = rvalue(out, ast.elements[3])
			return temp(out, f'({c}) ? ({t}) : ({f})')
	if ast.is_a(Tuple) and len(ast.args) == 1:
		l = rvalue(out, ast.args[0])
		return f'm_memory[(u16)({l})]'
	if ast.is_a(SymRef):
		return '(i32)'+repr(ast)
	raise NotImplementedError()

def lvalue(out, ast):
	info('lvalue %s', ast)
	if ast.is_a(Form) and len(ast.elements) == 1:
		return lvalue(out, ast.elements[0])
	if ast.is_a(Tuple) and len(ast.args) == 1:
		r = rvalue(out, ast.args[0])
		return f'm_memory[(u16)({r})]'
	if ast.is_a(SymRef):
		return repr(ast)
	raise NotImplementedError()

def walk(out, ast):
	info('walk %s', ast)
	if ast.is_a(Form) and len(ast.elements) == 1:
		return walk(out, ast.elements[0])
	if ast.is_a(Form):
		op = ast.elements[0]
		if op.is_the(Op, '<-'): # store
			l = lvalue(out, ast.elements[1])
			r = rvalue(out, ast.elements[2])
			out.append(f'({l}) = ({r});')
			return
	elif ast.is_a(SymRef):
		out.append(f'_do_{ast!s}();')
		return
	elif ast.is_a(Block):
		for stmt in ast.statements:
			walk(out, stmt)
		if ast.val is not None:
			walk(out, ast.val)
		return
	raise NotImplementedError()

def compile_to_c(ast):
	info('compile %s', ast)
	out = []
	walk(out, ast)
	return ' '.join(out)

s = Spec()
s.parse(sys.argv[1])
info('loaded %s rules', len(s.rules))

lut = [None]*256
ilen = [0]*256
for pattern, asm, action in s.rules:
	b = int(pattern[0], 16)
	ilen[b] = len(pattern)
	if lut[b] is not None:
		die('duplicate opcode %02x', b)
	source = compile_to_c(action)
	lut[b] = source

for op in range(256):
	if lut[op] is not None:
		print(f'case 0x{op:02x}: {{')
		print('   ', 'PC +=', ilen[op], ';')
		print('   ', lut[op])
		print('   ', '} break;')
