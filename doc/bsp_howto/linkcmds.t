@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Linker Script

@section What is a "linkcmds" file?

The linkcmds file is a script which is passed to the linker at linking
time. It holds somewhat the board memory configuration. 

@section Image of an Executable

A program destined to be embedded has some specificities. Embedded
machines often mean average performances and small memory usage. 

Two types of memories have to be distinguished: one is volatile but on
read and write access (RAM), while the other is non-volatile but read only
(ROM). Even though RAM and ROM can be found in every personal computer,
one generally doesn't care about them , because a program is always put in
RAM and is executed in RAM. That's a bit different in embedded
development: the target will execute the program each time it's reboot or
switched on, which means the program is stored in ROM. On the other hand,
data processing occurs in RAM. 

This leads us to the structure of an embedded program: it is roughly made
of sections. For example, if using COFF on the Motorola m68k family
of microprocessors, then the following sections will be present.

@table @b

@item the code (@code{.text}) section
holds the program's main
code, so that it doesn't have to be modified. This section
may be placed in ROM. 

@item the non-initialized data (@code{.bss}) section
holds uninitialized variables of the program. It can stay in RAM. 
XXX 

@item the initialized data (@code{.data}) section 
holds the
program data which are to be modified during the program's life, which
means they have to be in RAM. On another hand, these variables must be set
to predefined values, which have to be stored in ROM... 

@end table

That brings us up to the notion of the image of an executable: it consists
in the set of the program sections. 

As a program executable has many sections (note that the user can define
his own, and that compilers define theirs without any notice), one has to
state in which type of memory (RAM or ROM) the sections will be arranged.
For instance, a program compiled for a Personal Computer will see all the
sections to go to RAM, while a program destined to be embedded will see
some of his sections going into the ROM. 

The connection between a section and where that section is loaded into
memory is made at link time.  One has to let the linker know where
the different sections location are to be placed once they are in
memory. 

The following example shows a simple layout of program sections.  With
some object formats, there are many more sections but the basic 
layout is conceptually similar.

@example
@group
          +-----------------+
          |     .text       |  RAM or ROM
          +-----------------+
          |     .data       |  RAM
          +-----------------+
          |     .bss        |  RAM
          +-----------------+
@end group
@end example

The GNU linker has a command language to specify the image format.  This
command language can be quite complicated but most of what is required
can be learned by careful examination of a well-documented example.
The following is a heavily commented version of the linker script 
used with the the @code{gen68340} BSP  This file can be found at
$BSP340_ROOT/startup/linkcmds. 


@example
/*
 *  Specify that the output is to be coff-m68k regardless of what the
 *  native object format is.
 */

OUTPUT_FORMAT(coff-m68k) 

/*
 *  Set the amount of RAM on the target board.
 *
 *  NOTE: The default may be overridden by passing an argument to ld.
 */

RamSize = DEFINED(RamSize) ? RamSize : 4M;

/*
 *  Set the amount of RAM to be used for the application heap.  Objects
 *  allocated using malloc() come from this area.  Having a tight heap size
 *  is somewhat difficult and multiple attempts to squeeze it may be needed
 *  if you want to save the memory usage.  If you allocate all objects from
 *  the heap at system initialization time, this eases the sizing of the
 *  application heap.  
 *
 *  NOTE 1: The default may be overridden by passing an argument to ld.
 *
 *  NOTE 2: The TCP/IP stack requires additional memory in the Heap.
 *  
 *  NOTE 3: The GNAT/RTEMS run-time requires additional memory in the Heap.
 */

HeapSize = DEFINED(HeapSize) ? HeapSize : 0x10000;

/*
 *  Set the size of the starting stack used during BSP initialization
 *  until first task switch.  After that point, task stacks allocated
 *  by RTEMS are used.
 *
 *  NOTE: The default may be overridden by passing an argument to ld.
 */

StackSize = DEFINED(StackSize) ? StackSize : 0x1000;

/*
 *  Starting addresses and length of RAM and ROM.
 *
 *  The addresses must be valid addresses on the board.  The Chip Selects
 *  should be initialized such that the code addresses are valid.
 */

MEMORY @{
    ram : ORIGIN = 0x10000000, LENGTH = 4M
    rom : ORIGIN = 0x01000000, LENGTH = 4M
@}

/*
 *  This is for the network driver.  See the Networking documentation
 *  for more details.
 */

ETHERNET_ADDRESS = DEFINED(ETHERNET_ADDRESS) ? ETHERNET_ADDRESS : 0xDEAD12;

/*
 *  The following defines the order in which the sections should go.
 *  It also defines a number of variables which can be used by the
 *  application program.
 *
 *  NOTE: Each variable appears with 1 or 2 leading underscores to insure
 *        that the variable is accessible from C code with a single
 *        underscore.  Some object formats automatically add a leading
 *        underscore to all C global symbols.
 */

SECTIONS @{

  /*
   *  Make the RomBase variable available to the application.
   */

  _RamSize = RamSize;
  __RamSize = RamSize;

  /*
   *  Boot PROM  - Set the RomBase variable to the start of the ROM.
   */

  rom : @{
    _RomBase = .;
    __RomBase = .;
  @} >rom

  /*
   * Dynamic RAM - set the RamBase variable to the start of the RAM.
   */

  ram : @{
    _RamBase = .;
    __RamBase = .;
  @} >ram

  /*
   *  Text (code) goes into ROM
   */

  .text : @{
    /*
     *  Create a symbol for each object (.o).
     */

    CREATE_OBJECT_SYMBOLS

    /*
     *  Put all the object files code sections here.
     */

    *(.text)

    . = ALIGN (16);      /*  go to a 16-byte boundary */

    /*
     *  C++ constructors and destructors
     *
     *  NOTE:  See the CROSSGCC mailing-list FAQ for
     *         more details about the "[......]".
     */

    __CTOR_LIST__ = .;
   [......]
    __DTOR_END__ = .;

    /*
     *  Declares where the .text section ends.
     */

    etext = .;
    _etext = .;
  @} >rom 

  /*
   *  Exception Handler Frame section
   */

  .eh_fram : @{
    . = ALIGN (16);
    *(.eh_fram)
  @} >ram

  /*
   *  GCC Exception section
   */

  .gcc_exc : @{
    . = ALIGN (16);
    *(.gcc_exc)
  @} >ram

  /*
   *  Special variable to let application get to the dual-ported
   *  memory.
   */

  dpram : @{
    m340 = .;
    _m340 = .;
    . += (8 * 1024);
  @} >ram

  /*
   *  Initialized Data section goes in RAM
   */

  .data : @{
    copy_start = .;
    *(.data)

    . = ALIGN (16);
    _edata = .;
    copy_end = .;
  @} >ram

  /*
   *  Uninitialized Data section goes in ROM
   */

  .bss : @{
    /*
     *  M68K specific: Reserve some room for the Vector Table 
     *  (256 vectors of 4 bytes).
     */

    M68Kvec = .;
    _M68Kvec = .;
    . += (256 * 4);

    /*
     *  Start of memory to zero out at initialization time.
     */

    clear_start = .;

    /*
     *  Put all the object files uninitialized data sections
     *  here.
     */

    *(.bss)

    *(COMMON)

    . = ALIGN (16);
    _end = .;

    /*
     *  Start of the Application Heap
     */

    _HeapStart = .;
    __HeapStart = .;
    . += HeapSize;  

    /*
     *  The Starting Stack goes after the Application Heap.
     *  M68K stack grows down so start at high address.
     */

    . += StackSize;
    . = ALIGN (16);
    stack_init = .;

    clear_end = .;

    /*
     *  The RTEMS Executive Workspace goes here.  RTEMS
     *  allocates tasks, stacks, semaphores, etc. from this
     *  memory.
     */

    _WorkspaceBase = .;
    __WorkspaceBase = .;
  @} >ram
@}
@end example

Now there's a problem with the initialized data: the @code{.data} section
has to be in RAM as these data may be modified during the program execution.
But how will the values be initialized at boot time?

One approach is to place the entire program image in RAM and reload 
the image in its entirety each time the program is run.  This is fine
for use in a debug environment where a high-speed connection is available
between the development host computer and the target.  But even in this
environment, it is cumbersome.

The solution is to place a copy of the initialized data in a separate 
area of memory and copy it into the proper location each time the
program is started.  It is common practice to place a copy of the initialized
@code{.data} section the end of the code (@code{.text} section
(i.e. in ROM) when building a PROM image. The GNU tool @code{objcopy}
can be used for this purpose. 

The following figure illustrates the steps a linked program goes through
to become a downloadabled image.

@example
@group
+--------------+     +--------------------+
| .data    RAM |     | .data          RAM |
+--------------+     +--------------------+
| .bss     RAM |     | .bss           RAM |
+--------------+     +--------------------+         +----------------+ 
| .text    ROM |     | .text          ROM |         |     .text      |
+--------------+     +--------------------+         +----------------+ 
                     | copy of .data  ROM |         | copy of .data  |
                     +--------------------+         +----------------+ 

      Step 1                Step 2                       Step 3
@end group
@end example

In Step 1, the program is linked together using the BSP linker script.

In Step 2, a copy is made of the @code{.data} section and placed
after the @code{.text} section so it can be placed in PROM.  This step
is done after the linking time.  There is an example
of doing this in the file $RTEMS_ROOT/make/custom/gen68340.cfg: 

@example
# make a PROM image using objcopy
m68k-rtems-objcopy \
--adjust-section-vma .data= \

`m68k-rtems-objdump --section-headers \
$(basename $@@).exe \
| awk '[...]` \
$(basename $@@).exe
@end example

NOTE: The address at which the copy of the @code{.data} section is
specified by extracting the address of the end of the @code{.text}
section (i.e. in ROM) with an @code{awk} script.  The details of how
this is done are not relevant.

Step 3 shows the final executable image as it logically appears in
the target's non-volatile program memory.  The board initialization
code will copy the initialized data initial values (which are stored in
ROM) to their reserved location in RAM. 

