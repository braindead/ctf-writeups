ENTRY(_start)

SECTIONS
{
	. = 0x08048000;
	.text :
	{
		*(.text*)
		*(.rodata*)
		*(.data.rel.ro*)

		. = ALIGN(8);
		__crt_init_array_begin = .;
		KEEP(*(.init_array))
		__crt_init_array_end = .;
	}

	_start = _start; /* this makes ld -r keep the symbol */

	. = ALIGN(0x1000);
	.data :
	{
		*(.data*)
	}

	.bss :
	{
		*(.bss*)
	}

	/DISCARD/ :
	{
		*(*)
	}
}
