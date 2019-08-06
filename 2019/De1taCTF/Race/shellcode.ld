MEMORY
{
	shellcode (rx) : ORIGIN = 0, LENGTH = 1M
	ram (rwx) : ORIGIN = 0, LENGTH = 0
}

SECTIONS
{
	. = 0;
	.shellcode :
	{
		*(.0text)
		*(.text)
		*(.text.*)
		*(.rodata*)
		*(.rodata.*)
	} >shellcode

	_RW_START = .;
	.rwdata :
	{
		*(.data)
		*(.data.*)
		*(.bss)
		*(.bss.*)
	} >ram
	_RW_END = .;

	/DISCARD/ : /* this is needed to remove crap like eh_frame */
	{
		*(*)
	}
}

ASSERT(_RW_START == _RW_END, "error: shellcode contains rw .data section")
