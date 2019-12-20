from dataclasses import dataclass, field
from typing import *
import lrparsing
from lrparsing import Token, Prio, Ref, THIS, Tokens
from collections import defaultdict
from opspec import Spec
import PIL.Image
import types

class ASMError(Exception):
	pass

def parse_num(s):
	if s.startswith('$'):
		x = int(s[1:], 16)
		return x
	if s.startswith('%'):
		x = int(s[1:], 2)
		return x
	return int(s)

class G(lrparsing.Grammar):
	class T(lrparsing.TokenRegistry):
		number = Token(re=r'-?[0-9]+|\$[a-fA-F0-9]+|%[01]+')
		string = Token(re=r'"([^"]|\\")*"')
		reg = Token(re=r'[A-Z][a-zA-Z0-9_?.]*')
		symbol = Token(re=r'\.?[a-z_][a-zA-Z0-9_?.]*')
		lo = lrparsing.Keyword('lo')
		hi = lrparsing.Keyword('hi')
	expr = Prio(
		'(' << THIS << ')',
		T.number,
		T.number << '*' << THIS,
		T.symbol,
		THIS << Tokens("- +") << THIS,
	)
	mem = '(' + T.reg + ')'
	operand = Prio(
		T.reg,
		expr,
		expr + T.lo,
		expr + T.hi,
		mem,
		T.string
	)
	op = T.reg + lrparsing.List(operand, ',')
	pseudo_op = T.symbol + lrparsing.List(operand, ',')
	label = T.symbol + ':'
	equ = T.symbol + '=' + expr
	START = op | label | pseudo_op | equ
	COMMENTS = Token(re=';[^\n]*')
	#WHITESPACE = '[ \t]*'

def sext(n, w):
	return n | ((0-(n>>(w-1)))<<w)

def is_8bit(x):
	return x == sext(x&0xff, 8)

@dataclass
class Expr:
	offset: int
	symbols: Dict[str, int]
	rshift: int = 0
	mask: int = -1

	def eval(self, env):
		o = self.offset
		for s, w in self.symbols.items():
			o += w*env[s]
		#print('eval', repr(self), 'to', o)
		return (o>>self.rshift)&self.mask

	def from_ast(ast, eval_label, rsh=0, mask=-1):
		d = defaultdict(lambda: 0)
		o = 0
		def walk(a, w):
			nonlocal d
			nonlocal o
			if a[0] == G.expr:
				if len(a) == 2:
					if a[1][0] == G.T.symbol:
						d[eval_label(a[1][1])] += w
					else:
						o += w*parse_num(a[1][1])
				elif len(a) == 4 and a[2][1] == '*':
					walk(a[3], w*parse_num(a[1]))
				elif len(a) == 4 and a[2][1] == '+':
					walk(a[1], w)
					walk(a[3], w)
				elif len(a) == 4 and a[2][1] == '-':
					walk(a[1], w)
					walk(a[3], -w)
				else:
					print(a[2])
					assert False
			elif a[0] == G.T.number:
				o += w*parse_num(a[1])
			elif a[0] == G.T.symbol:
				d[eval_label(a[1])] += w
			else:
				print(G.repr_parse_tree(a))
				assert False
		walk(ast, 1)
		return Expr(o, d, rsh, mask)

	def __repr__(self):
		cs = []
		for s, w in self.symbols.items():
			if w != 0:
				cs.append(f'{w:+}*{s}')
		if self.offset != 0:
			cs.append(f'{self.offset:+}')
		if self.rshift != 0:
			cs.append(f'>>{self.rshift}')
		if self.mask != -1:
			cs.append(f'&0x{self.mask:x}')
		return ' '.join(cs)

@dataclass(init=False)
class Env:
	labels: Dict[str, int] = field(default_factory=dict)

def from_pix(p):
	if isinstance(p, int):
		return 1 if p == 0 else 0
	else:
		p = p[1]
		return 1 if p == 0 else 0

def load_image(ops):
	fname = ops[0].decode()
	opts = ''
	if len(ops) >= 2:
		opts = ops[1].decode()
	invert = "I" in opts
	reflect = "R" in opts
	tiles = "T" in opts

	img = PIL.Image.open(fname)
	w, h = img.size
	pix = img.load()

	blob = bytearray()
	if tiles:
		for tj in range(h//8):
			for ti in range(h//8):
				for j in range(8):
					j = tj*8+j
					b = 0
					for i in range(8):
						i = ti*8+i
						bit = from_pix(pix[i,j])
						if invert:
							bit ^= 1
						b = b*2 + bit
					blob.append(b)
	else:
		for j in range(h):
			bits = 1
			for i in range(w):
				if reflect:
					i = w-1-i
				bit = from_pix(pix[i,j])
				if invert:
					bit ^= 1
				bits = bits*2 + bit
				if bits >= 0x100:
					blob.append(bits&0xff)
					bits = 1
			if bits >= 0x100:
				blob.append(bits&0xff)
				bits = 1
	return bytes(blob)

@dataclass
class Assembler:
	commands: List = field(default_factory=list)
	scope: List[str] = field(default_factory=list)
	label_ctr: int = 0

	def parse_op(self, _ope):
		assert _ope[0] == G.operand
		ope = _ope[1]
		if ope[0] == G.T.reg:
			return ope[1]
		elif ope[0] == G.expr:
			if len(_ope) == 3:
				if _ope[2][0] == G.T.lo:
					return Expr.from_ast(ope, self.eval_label, 0, 0xff)
				if _ope[2][0] == G.T.hi:
					return Expr.from_ast(ope, self.eval_label, 8)
				assert False
			return Expr.from_ast(ope, self.eval_label)
		elif ope[0] == G.mem:
			return f'({ope[2][1]})'
		elif ope[0] == G.T.string:
			return eval('b'+ope[1])
		else:
			#print(ope)
			assert False

	def __init__(self):
		self.commands = []
		self.scope = ''
		self.encodings = defaultdict(lambda: [])
		s = Spec()
		s.parse('op.spec')
		for byte_repr, asm, _ in s.rules:
			ts = asm.split(' ', 1)
			if len(ts) > 1:
				opname, asm = ts
			else:
				opname, asm = ts[0], ''
			operands = [x.strip() for x in asm.split(',')]
			if len(operands[0]) == 0:
				operands = []
			self.encodings[opname].append((operands, byte_repr))
			self.encodings[opname].sort(key=lambda t: len(t[1]))

	def eval_label(self, l, decl=False):
		if l.startswith('_'):
			if decl:
				if l == '_':
					self.label_ctr += 1
					return f'{self.scope}#{self.label_ctr}'
			else:
				d = None
				if l == '_':
					d = 0
				elif l.startswith('_f'):
					d = int(l[2:])
				elif l.startswith('_'):
					d = -int(l[1:])
				if d is not None:
					return f'{self.scope}#{self.label_ctr+d}'
		if l.startswith('.'):
			return self.scope + l
		else:
			if decl:
				self.scope = l
			return l

	def source(self, s):
		line_no = 0
		for l in s.split('\n'):
			line_no += 1
			try:
				l = l.strip()
				if l.startswith(';'):
					continue
				if len(l) > 0:
					t = G.parse(l)
					t = t[1]
					if t[0] == G.op:
						opcode = t[1][1]
						operands = [self.parse_op(x) for x in t[2::2]]
						self.commands.append((opcode, operands))
					elif t[0] == G.label:
						l = self.eval_label(t[1][1], decl=True)
						self.commands.append(('label', l))
					elif t[0] == G.equ:
						l = self.eval_label(t[1][1], decl=True)
						e = Expr.from_ast(t[3][1], self.eval_label)
						self.commands.append(('equ', (l, e)))
					elif t[0] == G.pseudo_op:
						opcode = t[1][1]
						operands = [self.parse_op(x) for x in t[2::2]]
						if opcode == 'incimg':
							opcode = 'db'
							operands = [load_image(operands)]
							self.commands.append((opcode, operands))
						elif opcode == 'makelut':
							expr = operands[0].decode()
							co = compile(expr, mode='eval', filename='<makelut>')
							glob = {}
							f = types.FunctionType(co, glob)
							n    = operands[1].eval({})
							lut = bytearray(n)
							for i in range(n):
								glob['i'] = i
								lut[i] = f()
							print("made", n, "byte lut")
							self.commands.append(('db', [lut]))
						elif opcode == 'incbin':
							with open(operands[0].decode(), 'rb') as f:
								self.commands.append(('db', [f.read()]))
						elif opcode == 'include':
							path = operands[0].encode()
							with open(path, 'r') as f:
								self.source(f.read())
						else:
							self.commands.append((opcode, operands))
					else:
						#print(t)
						assert False
			except ASMError as e:
				print('error on line', line_no)
		#import IPython; IPython.embed()

	def link(self):
		self.pred = {}
		pass_cnt = 0
		env = {}

		while True:
			errors = list()
			blob = bytearray()
			addr = 0
			stable = True
			fails = 0
			pass_cnt += 1
			for i, (op, ops) in enumerate(self.commands):
				#print(f'{i:03}', op, ops, addr-len(blob))
				if op == 'label':
					if ops in env and env[ops] != addr:
						errors.append(f'label {ops!r} not converged')
						fails += 1
					env[ops] = addr
				elif op == 'align':
					a = 1
					r = 0
					try:
						a = ops[0].eval(env)
					except KeyError:
						fails += 1
					if len(ops) >= 2:
						try:
							r = ops[1].eval(env)
						except KeyError:
							fails += 1
					while addr%a != r:
						blob.append(0)
						addr += 1
				elif op == 'equ':
					try:
						env[ops[0]] = ops[1].eval(env)
					except KeyError:
						fails += 1
				elif op == 'org':
					try:
						addr = ops[0].eval(env)
					except KeyError:
						fails += 1
				elif op == 'seek':
					try:
						oval = ops[0].eval(env)
						if oval > len(blob):
							raise ASMError('seek: cannot seek backwards')
						while len(blob) < oval:
							blob.append(0)
							addr += 1
					except KeyError:
						fails += 1
				elif op == 'rb':
					o = ops[0]
					if isinstance(o, Expr):
						try:
							oval = o.eval(env)
						except KeyError:
							oval = 1
							fails += 1
					addr += oval
				elif op == 'zb':
					o = ops[0]
					if isinstance(o, Expr):
						try:
							oval = o.eval(env)
						except KeyError:
							oval = 1
							fails += 1
					addr += oval
					blob += b'\x00'*oval
				elif op == 'dw':
					for o in ops:
						if isinstance(o, Expr):
							try:
								oval = o.eval(env)
							except KeyError:
								oval = 0xffff
								fails += 1
							blob.append(oval&0xff)
							blob.append(oval>>8&0xff)
							addr += 2
						else:
							assert False
				elif op == 'db':
					for o in ops:
						if isinstance(o, Expr):
							try:
								oval = o.eval(env)
							except KeyError:
								oval = 255
								fails += 1
							blob.append(oval)
							addr += 1
						else:
							blob += o
							addr += len(o)
				else:
					encs = self.encodings[op]
					if len(encs) == 0:
						raise ASMError(f'unknown instruction {op!r}')
					good_arity = False
					good_types = False
					found_enc = False
					min_len = 9999
					for eops, wire_fmt in encs:
						#print(ops, eops)
						if len(eops) != len(ops):
							continue
						good_arity = True
						tfail = False
						fail = False
						xx = 255
						yy = 255
						for o, eo in zip(ops, eops):
							if isinstance(o, Expr):
								try:
									oval = o.eval(env)
								except KeyError as ke:
									errors.append('label not defined: '+ke.args[0])
									fail = True
									break
							if eo in ('xx', 'xxrel', 'xxyy',):
								if not isinstance(o, Expr):
									tfail = True
									break
							if eo == 'xx':
								xx = oval&0xff
								#if oval&0xff != oval:
								#	fail = True
							elif eo == 'xxrel':
								xx = (oval-addr-len(wire_fmt))&0xff
								if not is_8bit(oval-addr-len(wire_fmt)):
									fail = True
							elif eo == 'xxyy':
								yy = oval&0xff
								xx = oval>>8&0xff
							else:
								if o != eo:
									tfail = True
									break
						if not tfail:
							good_types = True
							min_len = min(min_len, len(wire_fmt))
							if not fail:
								found_enc = True
								break
					if not good_arity:
						raise ASMError(f'invalid arity for instruction {op!r} {ops}')
					if not good_types:
						raise ASMError(f'invalid operands for instruction {op!r} {ops}')
					if not found_enc:
						fails += 1
						errors.append(f'could not encode {op} {", ".join((map(str, ops)))}')
						for _ in range(min_len):
							blob.append(255)
							addr += 1
					else:
						for w in wire_fmt:
							if w == 'xx':
								blob.append(xx)
								addr += 1
							elif w == 'xxrel':
								blob.append(xx)
								addr += 1
							elif w == 'yy':
								blob.append(yy)
								addr += 1
							else:
								blob.append(int(w, 16))
								addr += 1
			print('pass', pass_cnt, 'fails', fails)
			if fails == 0:
				break
			if pass_cnt > 10:
				print('bailing out')
				for e in errors:
					print(e)
				exit(1)
		return blob, env

if __name__ == '__main__':
	import sys
	a = Assembler()
	with open(sys.argv[1]) as f:
		a.source(f.read())
	blob, env = a.link()
	with open(sys.argv[2], 'wb') as f:
		f.write(blob)
	with open(sys.argv[2]+'.map', 'w') as f:
		for k, v in env.items():
			f.write(f'{k} = ${v:04x}\n')
	print('assembled to', len(blob), 'bytes')
