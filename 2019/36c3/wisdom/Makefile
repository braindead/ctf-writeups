CXX := clang++
CXXFLAGS := -Os -Wall -std=c++14 -mno-sse -Wno-unused-function
CXXFLAGS += -nostdinc -nostdinc++ -nostdlib -m32 -fno-stack-protector -ffreestanding -static -fno-PIC
LDFLAGS = -nostdlib -m32 -fno-stack-protector -ffreestanding  -Wl,-T -Wl,pwn.x -no-pie 

all: pwn pwn.stripped s2 s2.stripped

dummy.o: dummy.s
	as --32 -o $@ $<

pwn: pwn.o
	$(CXX) $(LDFLAGS) -o $@ $^

stage2.o: dummy.inc.h
s2: stage2.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.stripped: %
	cp $< $@
	sstrip $@

deploy: .deploy
.deploy: pwn.stripped s2.stripped
	# CENSORED
	@touch .deploy

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f pwn.o pwn pwn.stripped PB.o

#PB.o: dummy.s
#	as --32 -march=i386 -o $@ $<

PB.o: kern.asm
	fasm $< $@

dummy.inc.h: dummy.o
	bin2c $< -n dummy > $@
