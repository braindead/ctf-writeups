import re

VAR = re.compile(r'(.*?)([Il]{6}[0O]{4})')

env = {}
def genvar(value='<unk>'):
	global env
	i = len(env)
	env[f'v{i}'] = value
	return f'v{i}'

rename2 = {
	'v2': 'bytecode',
	'v3': 'stack',
	'v6': 'pc',
	'v4': 'bytecode_len',
	'v5': 'opcode',
	'v8': 'imm',
}

renames = {}
def rename(x):
	if x in renames:
		return rename2.get(renames[x], renames[x])
	i = f'v{len(renames)}'
	renames[x] = i
	return rename2.get(i, i)

indent = 0
with open("CMakeLists.txt") as f:
	for line in f:
		line = line.strip()
		out = []
		r = 0
		for m in VAR.finditer(line):
			head, var = m.groups()
			out.append(head)
			out.append(rename(var))
			r = m.end()
		out.append(line[r:])

		for e in ('endwhile', 'endif', 'elseif',):
			if line.startswith(e):
				indent -= 1
				break
		print('  '*indent + ''.join(out))
		for e in ('while', 'if', 'elseif',):
			if line.startswith(e):
				indent += 1
				break
