import csv
import sys

rows = []
with open(sys.argv[1]) as f:
	r = csv.reader(f)
	for row in r:
		rows.append([int(x) for x in row])

blob = bytearray()
w, h = 128, 64
for y in range(h):#-1, -1, -1):
	for x in range(w):
		blob.append(rows[y][x])

with open(sys.argv[2], 'wb') as f:
	f.write(blob)

"""
img = PIL.Image.open(open(sys.argv[1], 'rb'))
w, h = img.size

sink = BitSink()
pix = img.load()
for y in range(h):
	for x in range(w):
		p = pix[x, h-1-y]
		sink.put(p, 4)

sink.flush()
with open(sys.argv[2], 'wb') as f:
	f.write(sink.bytes)
"""
