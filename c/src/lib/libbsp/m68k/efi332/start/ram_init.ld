OUTPUT_ARCH(m68k)
__DYNAMIC  =  0;

start = 0;

MEMORY
{
  ram     : ORIGIN = 0xfffa00, LENGTH = 256K
}

SECTIONS
{
  .text : {
	*(.text)
        } >ram
  .data : {
        *(.data)
        } >ram
   .bss : {
        *(.bss)
        } >ram 
}
