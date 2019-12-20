LDFLAGS := -lSDL2 -lepoxy
CXXFLAGS := -std=c++14 -Wall -O2 -I.
PY := python3
TILED := tiled

IMAGES := tooru.gif tooru_crouch.GIF tooru_sprite.GIF
IMAGES += kanna_crouch.GIF kanna_sprite.GIF
IMAGES += pb.png braindead.png game.png press_start.png
IMAGES += game_over.png

all: emu hello.rom game.rom

map.bin: map.csv pack_map.py
	$(PY) pack_map.py $< $@

map.csv: map.tmx
	$(TILED) --export-map $< $@

op.spec: gen_opspec.py
	$(PY) gen_opspec.py > $@

hello.rom: hello.asm9 asm.py op.spec
	$(PY) asm.py $< $@

game.rom: game.asm9 asm.py op.spec map.bin $(IMAGES)
	$(PY) asm.py $< $@

%.o: %.cc
	$(CXX) -c -o $@ $(CXXFLAGS) $<

core.gen.cc: gen_core.py opspec.py op.spec
	$(PY) gen_core.py op.spec > $@

disasm.gen.cc: gen_disasm.py opspec.py op.spec
	$(PY) gen_disasm.py op.spec > $@

emu_core.o: core.gen.cc disasm.gen.cc emu.hh
gfx.o: emu.hh gl.hh
gl.o: gl.hh
emu.o: emu.hh

emu: emu.o emu_core.o gfx.o gl.o
	$(CXX) -o $@ $(LDFLAGS) $(CXXFLAGS) $^

.PHONY: clean

clean:
	rm -f core.gen.cc disasm.gen.cc *.o op.spec
