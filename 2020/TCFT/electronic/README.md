Verilog implementation of the cipher is in `sbox.v` (the sbox) and `cipher.v` (everything else).

Run `yosys synth.ys` to synthesize and map the module, output will be in `out/synth.{json,v,blif}`.

Then run `python3 encode.py` to convert the design to a form accepted by challenge server,
`dist/gates` and `dist/circuit` will be created.

Implementation
==============
This is very straightforward translation of reference python code into verilog. SBox is implemented
with a simple case statement.  Stripts `gen*.py` were used to generate parts of the verilog code.

ABC maps the design to gates defined in `cells.lib`: XOR, OR, ORNOT, NAND.
It all should map to 15385 gates.

Choice of Gates
===============
There are only 8 unique gates with 2 inputs and 1 output. All 4-element subsets of them were tried
and the one that produced the smallest mapping was chosen. See `try_all_subsets.py`.

Encoding
========
Script `encode.py` parses `out/synth.json` and `cells.lib`, evaluates the functions in `cells.lib` to LUTs (using lark parser generator) , renames nets, topologically sorts all gates, and finally dumps to `dist{circuit,gates}`.
