col = [
	[15, 14, 13, 12],
	[11, 10, 9, 8],
	[7, 6, 5, 4],
	[3, 2, 1, 0],
]

def rot(r, n):
	return r[n:] + r[:n]

newcol = [None]*4
newcol[0] = [rot(col[1],1), rot(col[2],2), rot(col[3],3)]
newcol[1] = [rot(col[0],3), rot(col[2],1), rot(col[3],2)]
newcol[2] = [rot(col[0],2), rot(col[1],3), rot(col[3],1)]
newcol[3] = [rot(col[0],1), rot(col[1],2), rot(col[2],3)]

for i in range(4):
	for j in range(4):
		o = 15 - (i*4 + j)
		lhs = []
		for x in newcol[i]:
			lhs.append(x[j])
		lhs = "^".join(map(lambda i: f'x[{i}]', lhs))
		print(f'assign y[{o}] = {lhs};')
