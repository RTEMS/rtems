@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Initialization Code

@section Introduction

The initialization code is the first piece of code executed when there's a
reset/reboot. It aims at initializing the main functions of the board. As
an introduction I advise you to give a look to the gen68340 BSP
initialization code, which is split onto t wo directories: 

@itemize @bullet

@item $BSP340_ROOT/start340/: assembly language code which contains early
initialization routines,

@item $BSP340_ROOT/startup/: C code with higher level routines (RTEMS
initialization related). 

@end itemize

@section Board Initialization

You'll find two files under the $BSP340_ROOT/start340/ directory, open
rtemsfor340only.s which holds initialization code for a MC68340 board only
and is simpler. 

@section The Interrupts Vector Table

After the entry label starts a code section in which some room is
allocated for the table of interrupts vectors. They are assigned to the
address of the __uhoh label. 

At __uhoh label you can find the default interrupt handler routine. This
routine is only called when an unexpected interrupts is raised. You can
add your own routine there (in that case there's a call to a routine -
$BSP_ROOT/startup/dumpanic.c - that pri nts which address caused the
interrupt and the contents of the registers, stack...), but this should
not return. 

@section Chip Select Initialization

When the microprocessor accesses a memory area, address decoding is
handled by an address decoder (!), so that the microprocessor knows which
memory chip to access. 

Figure 4 : address decoding

You have to program your Chip Select registers in order that they match
the linkcmds settings. In this BSP ROM and RAM addresses can be found in
both the linkcmds and initialization code, but this is not a great way to
do, better use some shared variables . 

@section Integrated processor registers initialization

There are always some specific integrated processor registers
initialization to do. Integrated processors' user manuals often detail
them. 

@section Data section recopy

The next initialization part can be found in
$BSP340_ROOT/start340/init68340.c. First the Interrupt Vector Table is
copied into RAM, then the data section recopy is initiated
(_CopyDataClearBSSAndStart in $BSP340_ROOT/start340/startfor340only.s). 

This code :

@itemize @bullet

@item copies the .data section from ROM to its location reserved in RAM
(see 5.2 for more details about this copy),

@item clear .bss section (all the non-initialized data will take value 0). 

@end itemize

Then control is passed to the RTEMS-specific initialization code. 

@section RTEMS-Specific Initialization

@section The RTEMS configuration table

The RTEMS configuration table contains the maximum number of objects RTEMS
can handle during the application (e.g. maximum number of tasks,
semaphores, etc.). It's used to allocate the size for the RTEMS inner data
structures. 

The RTEMS configuration table is application dependant, which means that
one has to provide one per application. It's usually an header file
included in the main module of the application. 

The BSP_Configuration label points on this table. 

For more information on the RTEMS configuration table, refer to C user's
guide, chapter 23 <insert a link here>. 

@section RTEMS initialization procedure

The RTEMS initialization procedure is described in the 3rd chapter of the
C user's manual <insert a link here>. Please read it carefully. 

There are a few BSP specific functions called from the initialization
manager. They can be found in the startup directory of the BSP. 



File

Function

Note

bspstart.c



It starts the application.  It includes application, board, and monitor
specific initialization and configuration. 

bspstart.c

bsp_pretasking_hook

It starts libc support (needed to allocate some memory using C primitive
malloc for example). Heap size must be passed in argument, this is the one
which is defined in the linkcmds (cf. 5.) 

bspstart.c

bsp_start

Here the user and application specific configuration table has been
"loaded" so that BSP_Configuration is up to date. 

You can make last modifications here, for instance reserve more room for
the RTEMS Work Space, or adjust the heap size (you can for example use the
memory left for the lone heap). 

bspclean.c

bsp_cleanup

Return control to the monitor. 

@section Drivers initialization

The Driver Address Table is part of the RTEMS configuration table. It
defines RTEMS drivers entry points (initialization, open, close, read,
write, and control). For more information about this table, check C User's
manual chapter 21 section 6 <insert a l ink here>. 

The RTEMS initialization procedure calls the initialization function for
every driver defined in the RTEMS Configuration Table (this permits to add
only the drivers needed by the application). 

All these primitives have a major and a minor number as arguments: 

@itemize @bullet

@item the major number refers to the driver type,

@item the minor number is used to control two peripherals with the same
driver (for instance, we define only one major number for the serial
driver, but two minor numbers for channel A and B if there are two
channels in the UART). 

@end itemize

