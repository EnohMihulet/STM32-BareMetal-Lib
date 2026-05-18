ENTRY(EntryPoint)

MEMORY
{
	/* FLASH */
	FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K

	/* RAM */
	RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Initial stack pointer */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
	/* Vector table */
	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > FLASH

	/* Code and read-only data */
	.text :
	{
		. = ALIGN(4);

		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)

		. = ALIGN(4);
		_etext = .;
	} > FLASH

	/* Exception unwinding tables, sometimes emitted by GCC */
	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > FLASH

	/* Initialized globals/static variables */
	_sidata = LOADADDR(.data);

	.data :
	{
		. = ALIGN(4);
		_sdata = .;

		*(.data)
		*(.data*)

		. = ALIGN(4);
		_edata = .;
	} > RAM AT > FLASH

	/* Uninitialized / zeroed globals/static variables */
	.bss :
	{
		. = ALIGN(4);
		_sbss = .;

		*(.bss)
		*(.bss*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;
	} > RAM
}
