@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Linkcmds

@subsection = What is a "linkcmds" file?

The linkcmds file is a script which is passed to the linker at linking
time. It holds somewhat the board memory configuration. 

@subsection = Image of an Executable

A program destined to be embedded has some specificities. Embedded
machines often mean average performances and small memory usage. 

Two types of memories have to be distinguished: one is volatile but on
read and write access (RAM), while the other is non-volatile but read only
(ROM). Even though RAM and ROM can be found in every personal computer,
one generally doesn't care about them , because a program is always put in
RAM and is executed in RAM. That's a bit different in embedded
development: the target will execute the program each time it's reboot or
switched on, which means the program is stored in ROM. On the other hand,
data pr ocessing occurs in RAM. 



That leads us to the structure of an embedded program: it's roughly made
of sections, for instance for the Motorola 68k family of microprocessors : 

@itemize @bullet

@item the code section (aka ".txt" section): it holds the program's main
code, so that it doesn't have to be modified. It's placed in ROM. 

@item the non-initialized data section (aka ".bss" section): it holds non
initialized variables of the program. It can stay in RAM. 

@item the initialized data section (aka ".data" section): it holds the
program data which are to be modified during the program's life, which
means they have to be in RAM. On another hand, these variables must be set
to predefined values, which have to be
 stored in ROM... 

@end itemize

That brings us up to the notion of the image of an executable: it consists
in the set of the program sections. 



As a program executable has many sections (note that the user can define
his own, and that compilers define theirs without any notice), one has to
state in which type of memory (RAM or ROM) the sections will be arranged.
For instance, a program compiled f or a Personal Computer will see all the
sections to go to RAM, while a program destined to be embedded will see
some of his sections going into the ROM. 



The (section, area of memory) connection is made at linking time. One have
to make the linker know the different sections location once they're in
memory. 


Figure 2 : sections location in memory 

The GNU linker has a command language to specify the image format.  Let's
have a look to the "gen68340"  BSP linkcmds, which can be found at
$BSP340_ROOT/startup/linkcmds. 


@example
OUTPUT_FORMAT(coff-m68k) 

RamSize = DEFINED(RamSize) ? RamSize : 4M;
HeapSize = DEFINED(HeapSize) ? HeapSize : 0x10000;
StackSize = DEFINED(StackSize) ? StackSize : 0x1000;

MEMORY {
          ram : ORIGIN = 0x10000000, LENGTH = 4M
          rom : ORIGIN = 0x01000000, LENGTH = 4M
}

ETHERNET_ADDRESS = DEFINED(ETHERNET_ADDRESS) ? ETHERNET_ADDRESS : 0xDEAD12;

/*
 * Load objects
 */
SECTIONS {
	/*
	 * Hardware variations
	 */
	_RamSize = RamSize;
	__RamSize = RamSize;

        /*
         * Boot PROM
         */
        rom : {
                _RomBase = .;
                __RomBase = .;
        } >rom

        /*
         * Dynamic RAM
         */
        ram : {
                _RamBase = .;
                __RamBase = .;
        } >ram

        /*
         * Text, data and bss segments
         */
        .text : {
                CREATE_OBJECT_SYMBOLS


                *(.text)


                . = ALIGN (16);


		/*
		 * C++ constructors
		 */
		__CTOR_LIST__ = .;
               [......]
		__DTOR_END__ = .;

                etext = .;
                _etext = .;
        } >rom 

	.eh_fram : {
		. = ALIGN (16);
		*(.eh_fram)
	} >ram

	.gcc_exc : {
		. = ALIGN (16);
		*(.gcc_exc)
	} >ram

        dpram : {
                m340 = .;
                _m340 = .;
                . += (8 * 1024);
        } >ram

        .data : {
                copy_start = .;
                *(.data)

                . = ALIGN (16);
                _edata = .;
                copy_end = .;
        } >ram

        .bss : {
                M68Kvec = .;
                _M68Kvec = .;
                . += (256 * 4);


                clear_start = .;

                *(.bss)

                *(COMMON)

                . = ALIGN (16);
                _end = .;

                _HeapStart = .;
                __HeapStart = .;
                . += HeapSize;  
                . += StackSize;
                . = ALIGN (16);
                stack_init = .;

                clear_end = .;

                _WorkspaceBase = .;
                __WorkspaceBase = .;



        } >ram
}
@end example

executable format is COFF

your default board RAM size here. 

Note that it's possible to change it by passing an argument to ld

the default heap size: beware! the heap must be large enough to contain: 

@itemize @bullet

@item your program's static allocations

@item your program's dynamic allocations

@item ALL YOUR PROCESS'S STACK

@end itemize

Having a tight heap size is somewhat difficult and many tries are needed
if you want to save the memory usage. 


The Stacksize should only be large enough to hold the stack in the
initialization sequence. Then the tasks stacks will be allocated in the
Heap. 

Start address of RAM and its length. 

The start address must be a valid address. Most often RAM is assigned to a
given chip of memory on the board, and a Chip Select is assigned to this
chip with an address (see the Initialization sequence chapter).


Start address of ROM and its length.  Same remarks as above. 

this is for the network driver (see KA9Q documentation for more details) 

define where the sections should go: 

define some variables that the user code can access.

set the RomBase variable to the start of the ROM.

set the RamBase variable to the start of the RAM.

states that a symbol shall be created for each object (.o)

insert the .text sections of every object

go to a frontier of 16 bytes (just keep it as is)

reserve some place for C++ constructors and destructors.

just keep it as is (or see CROSSGCC mailing-list FAQ for more details)

declares where the .txt section ends.

the .txt section goes in ROM!

this section is created by GCC

put it in RAM.

this section is created by GCC

put it in RAM

room for peripherals

needs 8 Kbytes
put it in RAM

the initialized data section

put all the objects' .data sections in the .data section of the image

.data section goes in RAM

the non initialized data section


reserve some room for the Table of Vector Interrupts (256 vectors of 4 bytes)

pointer to the start of the non initialized data

put all the objects .bss sections in the .bss section of the image

put all the non initialized data, which are not in .bss sections

Heap start

reserve some room for the heap

reserve some room for the stack

top of the stack (remember the stack grows with addresses going down)

end of the non initialized data

start of the RTEMS Workspace (holds RTEMS configuration table and a few
other things, its size is calculated by RTEMS according to your
configuration, <INSERT A LINK HERE>) 

the resulting .bss section goes in RAM


Now there's a problem with the initialized data: the .data section has to
be in RAM as these data are to be modified during the program execution.
But how will they be initialized at boot time? 



One should be aware of the running executable image and the file to
download to the target image being different! In practice, the initialized
data section is copied at the end of the code section (i.e. in ROM) when
building a PROM image. The GNU tool obj copy can be used for this purpose. 



Figure 3 : copy of the initialized data section to build a PROM image



This process is made after the linking time, and you can find an example
of how it is done in $RTEMS_ROOT/make/custom/gen68340.cfg : 

@example
# make a prom image
m68k-rtems-objcopy \
--adjust-section-vma .data= \

`m68k-rtems-objdump --section-headers \
$(basename $@).exe \
| awk '[...]` \
$(basename $@).exe
@end example

use the target objcopy

we want to move the start of the section .data

in the resulting image

the address is given by extracting the address of the end of the .text
section (i.e. in ROM) with an awk script (don't care about it) 

process the image which have just been linked



The board initialization code (cf. 6) will copy the initialized data
initial values (which are stored in ROM) to their reserved location in
RAM. 

