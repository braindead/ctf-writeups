The compression function is almost a Davies-Meyer construction:

	def davies_meyer(cipher): return lambda a, b: cipher(a, key=b) ^ a

Except that they are missing the xor:

	def bad_compression(cipher): return lambda a, b: cipher(a, key=b)

This allows us to use a meet-in-the-middle technique when looking for preimages.
