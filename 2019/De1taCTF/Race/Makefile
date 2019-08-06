define shellcode-x86_64 =
	clang++ -g -Os -fno-rtti -fno-exceptions -nostdlib -Wall -fno-delete-null-pointer-checks\
		-fno-stack-check -fno-stack-protector -fno-builtin\
		-fPIE\
		$< -c -o $@.o
	ld -T shellcode.ld $@.o -o $@.bin --oformat=binary
	ld $@.o -o $@
	(echo format ELF64 executable; echo entry _start; echo _start file \"$@.bin\") > $@.elf.asm
	fasm $@.elf.asm $@.small_elf
endef

define shellcode-x86_32 =
	clang++ -m32 -march=i386 -g -O0 -fno-rtti -fno-exceptions -nostdlib -Wall -fno-delete-null-pointer-checks\
		-fno-stack-check -fno-stack-protector\
		-fPIE -fno-plt\
		$< -c -o $@.o
	ld -m elf_i386 -T shellcode.ld $@.o -o $@.bin --oformat=binary
	(echo format ELF executable; echo entry _start; echo _start file \"$@.bin\") > $@.elf.asm
	fasm $@.elf.asm $@.small_elf
endef

all: sploit

sploit: sploit2.cxx shellcode.hxx
	$(shellcode-x86_64)

install: sploit Makefile
	sudo mount myroot.img /mnt/tmp3
	sudo install -m4777 -o0 -g0 sploit.small_elf /mnt/tmp3/sploit
	sudo umount /mnt/tmp3
	date > install

run: install
	./start.sh

.PHONY: clean
clean:
	rm -f sploit.bin sploit sploit.o
