import subprocess

PRE = '''
library(my_cells) {
'''

POST = '''
	cell(NOT) {
		area: 1;
		pin(A) { direction: input; }
		pin(Y) { direction: output; function: "(A)'"; }
	}

	cell(BUF) {
		area: 2;
		pin(A) { direction: input; }
		pin(Y) { direction: output; function: "A"; }
	}
}
'''
with open("all_cells.lib") as f:
	cells = f.read().split('\n\n')

assert len(cells) == 7

def pick(a, i, n):
	if n == 0:
		yield a
	else:
		if i >= 7:
			return
		yield from pick(a, i+1, n)
		a.append(i)
		yield from pick(a, i+1, n-1)
		a.pop()

results_f = open('results.txt', 'w')

results = []
for subset in pick([], 0, 4):
	print('trying', subset)
	subset = [1, 2, 5, 6]
	with open('cells.lib', 'w') as f:
		f.write(PRE)
		for x in subset:
			f.write(cells[x])
			f.write('\n\n')
		f.write(POST)
	break
	r = subprocess.call(['yosys', 'map.ys'])
	if r == 0:
		r = subprocess.call(['python3', 'encode.py'])
		if r == 0:
			with open('dist/circuit') as f:
				cost = len(f.read().split())
			results.append((cost, tuple(subset)))
			print(cost, *subset, file=results_f)
			results_f.flush()
for x, y in results:
	print(x, y)
