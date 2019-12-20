import re

class Loc:
	def __init__(self, file, line, col):
		self.file = file
		self.line = line
		self.col = col
	def __repr__(self): return f'Loc({self.file!r}, {self.line}, {self.col})'
	def __str__(self): return f'{self.file}:{self.line}:{self.col}'

class Node:
	def __init__(self, loc):
		self.loc = loc
	def __repr__(self): return f'{self.__class__.__name__}({self.val!r})'
	def __eq__(self, o): return isinstance(o, self.__class__) and self.val == o.val
	def is_the(self, tok_class, val): return isinstance(self, tok_class) and self.val == val
	def is_a(self, tok_class): return isinstance(self, tok_class)

class Id(Node):
	RE = re.compile(r'[a-zA-Z_][a-zA-Z0-9_?]*')
	def __init__(self, loc, val):
		super().__init__(loc)
		self.val = val

class Op(Node):
	RE = re.compile(r'[!@#$%^&*()_+/|:=\-<>.]+')
	def __init__(self, loc, val):
		super().__init__(loc)
		self.val = val

class Num(Node):
	RE = re.compile(r'[0-9_]+')
	def __init__(self, loc, val):
		super().__init__(loc)
		self.val = int(val.replace('_', ''))

class Str(Node):
	def __init__(self, loc, val):
		super().__init__(loc)
		self.val = val

class KW(Node):
	def __init__(self, loc, val):
		super().__init__(loc)
		self.val = val

KEYWORDS = [
	'(', '[', '{',
	')', ']', '}',
	',', ';', '\\',
]

WS = re.compile(r'[ \t\r\f]+')

class ParseError(Exception):
	def __init__(self, loc, msg):
		super().__init__(msg)
		self.loc = loc
	def __str__(self): return f'{self.loc}: {" ".join(self.args)}'

def tokens(source, filename='<unknown>', line=1):
	ofs = 0
	line_start = ofs
	rules = [Id, Op, Num]

	def loc(): return Loc(filename, line, ofs-line_start+1)

	while ofs < len(source):
		#print(ofs, repr(source[ofs:ofs+50]))

		# whitespace
		m = WS.match(source, ofs)
		if m:
			_, ofs = m.span()
			continue
		if source.startswith('\n', ofs):
			line += 1
			ofs += 1
			line_start = ofs
			continue

		# comments
		if source.startswith(';;{', ofs):
			ofs += 3
			level = 1
			while ofs < len(source):
				ch = source[ofs]
				ofs += 1
				if ch == '{':
					level += 1
				elif ch == '}':
					level -= 1
					if level == 0:
						break
				elif ch == '\n':
					line += 1
					line_start = ofs
			continue
		if source.startswith(';;', ofs):
			end = source.find('\n', ofs)
			if end < -1:
				break
			ofs = end # let whitespace code handle the \n
			continue

		# keywords
		match = False
		for kw in KEYWORDS:
			if source.startswith(kw, ofs):
				yield KW(loc(), kw)
				ofs += len(kw)
				match = True
				break
		if match:
			continue

		# regular tokens
		for rule in rules:
			m = rule.RE.match(source, ofs)
			if m:
				yield rule(loc(), m.group(0))
				_, ofs = m.span()
				match = True
				break
		if match:
			continue

		# string literals
		if source.startswith('"', ofs):
			chunks = []
			ofs += 1
			l = ofs
			while True:
				if ofs >= len(source):
					raise ParseError(loc() ,f'unterminated string literal')
				ch = source[ofs]
				if ch == '"':
					break
				if ch == '\\':
					if l != ofs:
						chunks.append(source[l:ofs])
					ofs += 1
					if ofs >= len(source):
						raise ParseError(loc() ,f'unterminated string literal')
					ch = source[ofs]
					if ch == 'n':
						ofs += 1
						chunks.append('\n')
					if ch == 'x':
						ofs += 1
						if ofs+2 > len(source):
							raise ParseError(loc() ,f'unterminated string literal')
						chunks.append(chr(int(source[ofs:ofs+2], 16)))
						ofs += 2
					raise ParseError(loc() ,f'bad \-escape in string')
				else:
					ofs += 1
			chunks.append(source[l:ofs])
			yield Str(loc(), ''.join(chunks))
			ofs += 1
			continue

		raise ParseError(loc() ,f'unexpected char {source[ofs]!r}')

class PrettyPrint:
	class IndentCM:
		def __init__(self, pp, inc):
			self.pp = pp
			self.old = pp.indent
			self.inc = inc
		def __enter__(self):
			self.pp.indent += self.inc
		def __exit__(self):
			self.pp.indent = self.old

	def __init__(self, sink):
		self.indent = 0
		self.sink = sink

	def write(self, x):
		self.sink.write(x)

	def writeline(self, x):
		self.write('  '*self.indent)
		self.write(x)
		self.write('\n')

	def withindent(self):
		return IndentCM(self, 2)

class Block(Node):
	def __init__(self, loc, stmt, val):
		super().__init__(loc)
		self.statements = stmt
		self.val = val
	def __repr__(self):
		chunks = ['{ ']
		for s in self.statements:
			chunks.append(repr(s))
			chunks.append('; ')
		if self.val is not None:
			chunks.append(repr(self.val))
			chunks.append(' ')
		chunks.append('}')
		return ''.join(chunks)

class Tuple(Node):
	def __init__(self, loc, args, kwargs):
		super().__init__(loc)
		self.args = args
		self.kwargs = kwargs
	def __repr__(self):
		chunks = []
		for a in self.args:
			chunks.append(repr(a))
		for k, v in self.kwargs:
			chunks.append(repr(k)+'='+repr(v))
		return 'Tuple('+', '.join(chunks)+')'

class SymRef(Node):
	def __init__(self, loc, path, absolute):
		super().__init__(loc)
		self.path = tuple(path)
		self.absolute = absolute
	def __repr__(self):
		x = '\\'.join(map(lambda x: x.val, self.path))
		if self.absolute:
			x = '\\'+x
		return x
	def __eq__(self, o):
		if not isinstance(o, SymRef):
			return False
		return self.path == o.path
	def __hash__(self):
		h = hash(self.path)
		if self.absolute:
			h = (h*102391121+9485734)&0xffffffffffffffff
		return h

class Form(Node):
	def __init__(self, loc, elements):
		super().__init__(loc)
		assert isinstance(elements, list)
		for x in elements:
			assert isinstance(x, Node)
		self.elements = elements
	def __repr__(self):
		chunks = []
		for el in self.elements:
			chunks.append(repr(el))
		return '('+ ' '.join(chunks) + ')'

class InfixOp:
	TY_BINARY = 0
	TY_PREFIX = 1
	def __init__(self, priority, type):
		self.type = type
		self.priority = priority


class Parser:
	def __init__(self, toksource):
		self.infix = {} # str -> InfixOp
		self.ll = None
		self.toksource = toksource
		self._loc = None

	def loc(self):
		return self._loc

	def token(self):
		if self.ll is not None:
			t = self.ll
			self.ll = None
		else:
			try:
				t = next(self.toksource)
				#print(t.loc, t)
				self._loc = t.loc
			except StopIteration:
				t = None
		return t

	def put_back(self, tok):
		assert self.ll is None
		self.ll = tok

	def parse_tuple(self):
		# '(' already eaten
		loc = self.loc()
		args = []
		kwargs = []
		while True:
			x = self.parse()
			t = self.token()
			if t.is_the(KW, ','):
				if x is None:
					raise ParseError(self.loc(), 'expression expected before ","')
				args.append(x)
			elif t.is_the(KW, ')'):
				if x is not None: # () is valid syntax
					args.append(x)
				break
			elif t.is_the(KW, '='):
				if not x.is_a(SymRef):
					raise ParseError(self.loc(), 'lhs of "=" in tuple must be a symbol reference')
				y = self.parse()
				kwargs[x] = y
			else:
				raise ParseError(self.loc(), f'expected KW(",") or KW(")"); got {t!r}')
		return Tuple(loc, args, kwargs)

	def parse_block(self):
		# '{' already eaten
		loc = self.loc()
		args = []
		last = None
		while True:
			x = self.parse()
			t = self.token()
			if t.is_the(KW, ';'):
				if x is None:
					raise ParseError(self.loc(), 'expression expected before ";"')
				args.append(x)
			elif t.is_the(KW, '}'):
				if x is not None: # {} is valid syntax
					last = x
				break
			else:
				raise ParseError(self.loc(), f'expected KW(",") or KW(")"); got {t!r}')
		return Block(loc, args, last)

	def parse_path(self):
		loc = None
		t = self.token()
		absolute = False
		if t.is_the(KW, '\\'):
			absolute = True
			loc = t.loc
		else:
			self.put_back(t)

		path = []
		while True:
			t = self.token()
			if t.is_a(Id):
				path.append(t)
				loc = loc or t.loc
			else:
				self.put_back(t)
				raise ParseError(self.loc() ,f'expected identifier')
				break
			t = self.token()
			if not t.is_the(KW, '\\'):
				self.put_back(t)
				break
		return SymRef(loc, path, absolute)

	def parse(self):
		form = []
		while True:
			t = self.token()
			if t is None:
				break
			if t.is_the(KW, '('):
				form.append(self.parse_tuple())
			elif t.is_the(KW, '{'):
				form.append(self.parse_block())
			elif t.is_a(Id) or t.is_the(KW, '\\'):
				self.put_back(t)
				form.append(self.parse_path())
			elif t.is_a(Op) or t.is_a(Num) or t.is_a(Str):
				form.append(t)
			else:
				self.put_back(t)
				break
		if len(form) == 0:
			return None
		if len(form) > 0:
			loc = form[0].loc
		return self.process_infix(form)

	def parse_top(self) -> Node:
		x = self.parse()
		if x is not None:
			t = self.token()
			if t is not None and not t.is_the(KW, ';'):
				raise ParseError(self.loc(), f'expected ";"')
		else:
			t = self.token()
			if t is None:
				return x
			if t.is_the(KW, ';'): # double ; not allowed
				raise ParseError(self.loc(), f'expected expression before ";"')
		return x

	def process_infix(self, toks):
		# subforms delimited by operators
		toks_ = []
		run = []
		for t in toks:
			if isinstance(t, Op):
				if len(run) > 0:
					toks_.append(Form(run[0].loc, run))
				run = []
				toks_.append(t)
			else:
				run.append(t)
		if len(toks_) == 0: # no operators
			return Form(run[0].loc, run)
		if len(run) > 0:
			toks_.append(Form(run[0].loc, run))
		toks = toks_
		# simple quadratic time algorithm:
		#   repeatedly find the leftmost highest priority Op and fold it
		while True:
			#print(toks)
			pivot = None
			pri = -1
			for i, t in enumerate(toks):
				if not isinstance(t, Op):
					continue
				if t.val not in self.infix:
					raise ParseError(t.loc, f'unknown infix operator {t.val!r}')
				tpri = self.infix[t.val].priority
				if tpri > pri:
					pivot = i
					pri = tpri
			if pivot is None:
				break
			op = toks[pivot]
			typ = self.infix[op.val].type
			if typ == InfixOp.TY_PREFIX:
				if pivot+1 >= len(toks):
					raise ParseError(op.loc, 'expected an expression after prefix operator')
				toks[pivot:pivot+2] = [Form(op.loc, toks[pivot:pivot+2])]
			elif typ == InfixOp.TY_BINARY:
				if pivot+1 >= len(toks):
					raise ParseError(op.loc, 'expected an expression after infix operator')
				if pivot-1 < 0:
					raise ParseError(op.loc, 'expected an expression before infix operator')
				toks[pivot-1:pivot+2] = [Form(op.loc, [toks[pivot], toks[pivot-1], toks[pivot+1]])]
			else:
				assert False
		assert len(toks) == 1
		return toks[0]
