from syntax import tokens, Parser, InfixOp

def make_parser():
	return p

class Spec:
	def __init__(self):
		self.p = Parser(None)
		self.p.infix[':'] = InfixOp(0, InfixOp.TY_BINARY)

		self.p.infix['<-'] = InfixOp(100, InfixOp.TY_BINARY)

		self.p.infix['=='] = InfixOp(200, InfixOp.TY_BINARY)
		self.p.infix['<'] = InfixOp(400, InfixOp.TY_BINARY)

		self.p.infix['+'] = InfixOp(500, InfixOp.TY_BINARY)
		self.p.infix['-'] = InfixOp(500, InfixOp.TY_BINARY)

		self.p.infix['&'] = InfixOp(600, InfixOp.TY_BINARY)
		self.p.infix['|'] = InfixOp(600, InfixOp.TY_BINARY)
		self.p.infix['^'] = InfixOp(600, InfixOp.TY_BINARY)

		self.p.infix['*'] = InfixOp(700, InfixOp.TY_BINARY)
		self.p.infix['/'] = InfixOp(700, InfixOp.TY_BINARY)
		self.p.infix['%'] = InfixOp(700, InfixOp.TY_BINARY)
		self.rules = []

	def parse(self, path):
		with open(path) as f:
			lineno = 0
			for line in f:
				lineno += 1
				parts = line.split(':')
				if len(parts) <= 1:
					continue
				pattern = parts[0].split()
				assemly = parts[1].strip()
				action_src = parts[2].strip()
				assert len(parts) == 3
				self.p.toksource = tokens('{'+action_src+';}', path, lineno)
				action = self.p.parse_top()
				self.rules.append((pattern, assemly, action))

if __name__ == '__main__':
	s = Spec()
	s.parse('op.spec')
