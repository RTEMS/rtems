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
reset/reboot. Its purpose is to initialize the board for the application.
This chapter contains a narrative description of the initialization
process followed by a description of each of the files and routines
commonly found in the BSP related to initialization.  The remainder of
this chapter covers special issues which require attention such
as interrupt vector table and chip select initialization.

Most of the examples in this chapter will be based on the gen68340 BSP
initialization code.  Like most BSPs, the initialization for this 
BSP is divided into two subdirectories under the BSP source directory.
The gen68340 BSP source code in the following directory:

@example
c/src/lib/libbsp/m68k/gen68340: 
@end example

@itemize @bullet

@item @code{start340}: assembly language code which contains early
initialization routines

@item @code{startup}: C code with higher level routines (RTEMS
initialization related) 

@end itemize

@b{NOTE:} The directory @code{start340} is simply named @code{start} or 
start followed by a BSP designation.

In the @code{start340} directory are two source files.  The file
@code{startfor340only.s} is the simpler of these files as it only has 
initialization code for a MC68340 board.  The file @code{start340.s}
contains initialization for a 68349 based board as well.

@section Board Initialization

This section describes the steps an application goes through from the
time the first BSP code is executed until the first application task
executes.  The routines invoked during this will be discussed and
their location in the RTEMS source tree pointed out.

@subsection Start Code - Assembly Language Initialization

The assembly language code in the directory @code{start} is
the first part of the application to execute.  It is 
responsible for initializing the processor and board enough to execute
the rest of the BSP.  This includes:

@itemize @bullet
@item initializing the stack
@item zeroing out the uninitialized data section @code{.bss}
@item disabling external interrupts
@item copy the initialized data from ROM to RAM
@end itemize

The general rule of thumb is that the
start code in assembly should do the minimum necessary to allow C code
to execute to complete the initialization sequence.  

The initial assembly language start code completes its execution by
invoking the shared routine @code{boot_card()}.

@subsection boot_card() - Boot the Card

The @code{boot_card()} is the first C code invoked.  Most of the BSPs
use the sams shared version of @code{boot_card()} which is located in
the following file:

@example
c/src/lib/libbsp/shared/main.c
@end example

The @code{boot_card()} routine performs the following functions:

@itemize @bullet

@item initializes the shared fields of the CPU Configuration Table
(variable name @code{Cpu_table}) to a default state,

@item copies the application's RTEMS Configuration Table
(variable name @code{Configuration}) to the BSP's Configuration
Table (variable name @code{BSP_Configuration}) so it can be modified
as necessary without copying the original table,

@item invokes the BSP specific routine @code{bsp_start()},

@item invokes the RTEMS directive @code{rtems_initialize_executive_early()}
to initialize the executive, C Library, and all device drivers but
return without initiating multitasking or enabling interrupts,

@item invokes the shared @code{main()} in the same file as
@code{boot_card()} which does not return until the
@code{rtems_shutdown_executive} directive is called, and

@item invokes the BSP specific routine @code{bsp_cleanup()} to perform
any necessary board specific shutdown actions.

@end itemize

It is important to note that the executive and much of the
support environment must be initialized before invoking @code{main()}.

@subsection bsp_start() - BSP Specific Initialization

This is the first BSP specific C routine to execute during system
initialization.  This routine often performs required fundamental
hardware initialization such as setting bus controller registers
that do not have a direct impact on whether or not C code can execute.
The source code for this routine is usually found in the following
file:

@example
c/src/lib/libbsp/CPU/BSP/startup/bspstart.c
@end example

This routine is also responsible for overriding the default settings
in the CPU Configuration Table and setting port specific entries
in this table.  This routine will typically install routines
for one or more of the following initialization hooks:

@itemize @bullet
@item BSP Pretasking Hook
@item BSP Predriver Hook
@item BSP Postdriver Hook
@end itemize

One of the most important functions performed by this routine
is determining where the RTEMS Executive Work Space is to be
located in memory.  All RTEMS objects and task stacks will be
allocated from this Workspace.  The RTEMS Workspace is distinct
from the application heap used for @code{malloc()}.

Many BSPs place this area at the end of RAM although this is
certainly not a requirement.

After completing execution, this routine returns to the
@code{boot_card()} routine.

@subsection main() - C Main

This routine is the C main entry point.  This is a special routine 
and the GNU Compiler Suite treats it as such.  The GNU C Compiler
recognizes @code{main()} and automatically inserts a call to the
compiler run-time support routine @code{__main()} as the first
code executed in @code{main()}.

The routine @code{__main()} initializes the compiler's basic run-time
support library and, most importantly, invokes the C++ global 
constructors.  

The precise placement of when @code{main()} is invoked in the 
RTEMS initialization sequence insures that C Library and non-blocking
calls can be made in global C++ constructors.

The shared implementation of this routine is located in the following file:

@example
c/src/lib/libbsp/shared/main.c
@end example

In addition to the implicit invocation of @code{__main}, this 
routine performs some explitit initialization.  This routine 
sets the variable @code{rtems_progname} and initiates 
multitasking via a call to the RTEMS directive
@code{rtems_initialize_executive_late}.  It is important to note
that the executive does not return to this routine until the
RTEMS directive @code{rtems_shutdown_executive} is invoked.

@subsection RTEMS Pretasking Callback

The @code{pretasking_hook} entry in the RTEMS CPU Configuration
Table may be the address of a user provided routine that is
invoked once RTEMS initialization is complete but before interrupts
and tasking are enabled.  No tasks -- not even the IDLE task -- have
been created when this hook is invoked.  The pretasking hook is optional.

Although optional, most of the RTEMS BSPs provide a pretasking hook
callback.  This routine is usually called @code{bsp_pretasking_hook}
and is found in the file:

@example
c/src/lib/libbsp/CPU/BSP/startup/bspstart.c
@end example

The @code{bsp_pretasking_hook()} routine is the appropriate place to
initialize any support components which depend on the RTEMS APIs.
Most BSPs initialize the RTEMS C Library support in their
implementation of @code{bsp_pretasking_hook()}.  This initialization
includes the application heap as well as the reentrancy support
for the C Library.

@subsection RTEMS Predriver Callback

XXX is the address of the user provided
routine which is invoked with tasking enabled immediately before
the MPCI and device drivers are initialized. RTEMS
initialization is complete, interrupts and tasking are enabled,
but no device drivers are initialized.  This field may be NULL to
indicate that the hook is not utilized.

@subsection Device Driver Initialization

At this point in the initialization sequence, the initialization 
routines for all of the device drivers specified in the Device
Driver Table are invoked.

@subsection RTEMS Postdriver Callback

XXX is the address of the user provided
routine which is invoked with tasking enabled immediately after
the MPCI and device drivers are initialized. RTEMS
initialization is complete, interrupts and tasking are enabled,
and the device drivers are initialized.  This field may be NULL
to indicate that the hook is not utilized.


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

This code performs the following actions:

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

@table @b

@item bspstart.c

It starts the application.  It includes application, board, and monitor
specific initialization and configuration. 

@item bspstart.c

@table @b
@item bsp_pretasking_hook

It starts libc support (needed to allocate some memory using C primitive
malloc for example). Heap size must be passed in argument, this is the one
which is defined in the linkcmds (cf. 5.) 


@end table

@item bspclean.c

@table @b

@item bsp_cleanup

Return control to the monitor. 

@end table

@end table

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

