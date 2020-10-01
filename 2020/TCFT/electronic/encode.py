import json
from braindead import *
import re
import lark
from collections import defaultdict
log.enable()
args = Args()
args.parse()

lib_func = lark.Lark(
'''
?start: x
?x: WORD -> f_var
  | "(" x ")" "'" -> f_not
  | x "+" x -> f_or
  | x "*" x -> f_and
  | x "^" x -> f_xor
%import common.WORD
%ignore " "
''')

class LibEval(lark.Transformer):
	def __init__(self, env):
		super().__init__(visit_tokens=True)
		self.env = env
	def f_var(self, x): return self.env[x[0]]
	def f_not(self, x): return 1^x[0]
	def f_or(self, x): return x[0]|x[1]
	def f_and(self, x): return x[0]&x[1]
	def f_xor(self, x): return x[0]^x[1]

with open('out/synth.json') as f:
	g = json.load(f)

top = g['modules']['Cipher']

net_to_unit = {}

def place_port(port, ofs):
	for i, bit in enumerate(port["bits"]):
		assert bit not in net_to_unit
		net_to_unit[bit] = ofs+(63-i)

free_unit = 191
def map_unit(p):
	if p in net_to_unit:
		return net_to_unit[p]
	else:
		global free_unit
		free_unit += 1
		net_to_unit[p] = free_unit
		return free_unit

place_port(top["ports"]["ptext"], 0)
place_port(top["ports"]["key"], 64)
place_port(top["ports"]["ctext"], 128)

name_to_gate = {}
out_circuit = []

gatecnt = defaultdict(lambda: 0)
for cell in top['cells'].values():
	ty = cell['type']
	gatecnt[ty] += 1
	if ty == 'NOT':
		ty = 'NAND'
		cell['connections']['B'] = cell['connections']['A'] 
	if ty not in name_to_gate:
		if len(name_to_gate) >= 4:
			die('too many different gates')
		name_to_gate[ty] = len(name_to_gate)
	ty = name_to_gate[ty]
	in0 = map_unit(cell['connections']['A'][0])
	in1 = map_unit(cell['connections']['B'][0])
	out = map_unit(cell['connections']['Y'][0])
	out_circuit.append((ty, in0, in1, out))

log.info('cells: %s', len(out_circuit))
for name, cnt in gatecnt.items():
	log.info('%s: %s', name, cnt)
log.info('loading liberty')

gatelib = {}
CELL_RE = re.compile(r'cell\(([A-Z]+)\)')
FUNCION_RE = re.compile(r'function: "(.*?)"')
with open('cells.lib') as f:
	cell_name = None
	for line in f:
		if m := CELL_RE.search(line):
			cell_name = m.group(1)
		elif m := FUNCION_RE.search(line):
			fcn = lib_func.parse(m.group(1))
			desc = ''
			for x in range(4):
				env = {
					'A': x>>1&1,
					'B': x>>0&1,
				}
				evl = LibEval(env)
				desc += str(evl.transform(fcn))
			log.info('gate %s: %s', cell_name, desc)
			gatelib[cell_name] = desc

with open('dist/gates', 'w') as f:
	out = [None]*4
	for name, gateid in name_to_gate.items():
		log.info('gateid %s %s: %s', gateid, name, gatelib[name])
		out[gateid] = gatelib[name]
	while out[-1] is None:
		out.pop()
	print(' '.join(out), file=f)

cell_deps = [2]*len(out_circuit)
unit_readers = defaultdict(list)

for i, (g, a, b, y) in enumerate(out_circuit):
	unit_readers[a].append(i)
	unit_readers[b].append(i)

sched_circuit = []

ready_units = list(range(128))
while len(ready_units) > 0:
	a = ready_units.pop()
	#log.info('pin %s ready', a)
	for i in unit_readers[a]:
		cell_deps[i] -= 1
		assert cell_deps[i] >= 0
		if cell_deps[i] == 0:
			g, a, b, y = out_circuit[i]
			#log.info('cell %s,%s,%s,%s ready', g, a, b, y)
			sched_circuit.append(f'{g},{a},{b},{y}')
			ready_units.append(y)

for i, (g, a, b, y) in enumerate(out_circuit):
	if cell_deps[i] != 0:
		log.error('cell %s,%s,%s,%s', g, a, b, y)

with open('dist/circuit', 'w') as f:
	f.write(' '.join(sched_circuit))
