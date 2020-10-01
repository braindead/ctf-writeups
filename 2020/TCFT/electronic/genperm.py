state_permutation = [0,11,6,13,10,1,12,7,5,14,3,8,15,4,9,2]
for i in range(16):
	j = state_permutation[i]
	print(f'assign x[{63 - i*4}:{63 - i*4-3}] = y[{63-j*4}:{63-j*4-3}];')
