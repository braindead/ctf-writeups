import re
import struct

class LUT:
	def __init__(self, width, lut, inputs, o):
		self.width = width
		self.lut = lut
		assert len(inputs) == width
		self.ins = [inputs[i] for i in range(width)]
		self.out = o

luts = []

with open('zybo_rocketchip_ZynqMediumFPGAConfig.v', 'r') as f:
	txt = f.read()

idx_pattern = re.compile(r'\[([0-9]+)\]')
lut_pattern = re.compile(r'LUT[0-9]+ #\(')
init_pattern = re.compile(r'\.INIT\(')
hex_pattern = re.compile(r"([0-9]+)'h([0-9a-fA-F]+)")
i_pattern = re.compile(r"\.I([0-9]+)\((.*?)\)")
o_pattern = re.compile(r"\.O\((.*?)\)")
end_pattern = re.compile(r";")
pos = 0
while True:
	m = lut_pattern.search(txt, pos)
	if m is None:
		break
	_, pos = m.span()
	m = init_pattern.search(txt, pos)
	_, pos = m.span()
	m = hex_pattern.search(txt, pos)
	_, pos = m.span()
	nbits = int(m.group(1))
	bits = int(m.group(2), 16)
	n_inputs = 0
	while (nbits>>n_inputs) > 1:
		n_inputs += 1
	inputs = {}
	for _ in range(n_inputs):
		m = i_pattern.search(txt, pos)
		_, pos = m.span()
		inputs[int(m.group(1))] = m.group(2)
	m = o_pattern.search(txt, pos)
	out = m.group(1)
	_, pos = m.span()

	#print(nbits, hex(bits), out, inputs)
	luts.append(LUT(n_inputs, bits, inputs, out))

magics = {}

for l in luts:
	if l.out.startswith('\\trig_'):
		x = 0
		for i in l.ins:
			if i.startswith('dcacheArb_io_requestor_1_s1_data_data'):
				x += 1
		if x == l.width:
			print('found detector', l.out, l.ins)
			magic_kind = l.out[:8]
			if magic_kind not in magics:
				magics[magic_kind] = {}
			m = magics[magic_kind]
			for i in range(1<<l.width):
				if l.lut>>i == 1:
					correct_input = i
					break
			for i, inp in enumerate(l.ins):
				idx = int(idx_pattern.search(inp).group(1))
				m[idx] = (correct_input>>i)&1

print(magics)
for mtag, bit_dict in magics.items():
	mval = 0
	for pos, val in bit_dict.items():
		mval += val<<pos
	print(mtag, hex(mval), struct.pack('<Q', mval))
