@c
@c  COPYRIGHT (c) 1988-2002.
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
The gen68340 BSP source code is in the following directory:

@example
c/src/lib/libbsp/m68k/gen68340
@end example

The following source code files are in this subdirectory.

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

@section Required Global Variables

Although not strictly part of initialization, there are a few global
variables assumed to exist by many support components.  These 
global variables are usually declared in the file @code{startup/bspstart.c}
that provides most of the BSP specific initialization.  The following is
a list of these global variables:

@itemize @bullet
@item @code{BSP_Configuration} is the BSP's writable copy of the RTEMS
Configuration Table.

@item @code{Cpu_table} is the RTEMS CPU Dependent Information Table.

@item @code{bsp_isr_level} is the interrupt level that is set at
system startup.  It will be restored when the executive returns
control to the BSP.

@end itemize

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

The label (symbolic name) associated with the starting address of the
program is typically called @code{start}.  The start object file
is the first object file linked into the program image so it is insured
that the start code is at offset 0 in the @code{.text} section.  It is
the responsibility of the linker script in conjunction with the 
compiler specifications file to put the start code in the correct location
in the application image.

@subsection boot_card() - Boot the Card

The @code{boot_card()} is the first C code invoked.  Most of the BSPs
use the same shared version of @code{boot_card()} which is located in
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
in this table.  This may include increasing the maximum number
of some types of RTEMS system objects to reflect the needs of 
the BSP and the base set of device drivers. This routine will
typically also install routines for one or more of the following
initialization hooks:

@itemize @bullet
@item BSP Pretasking Hook
@item BSP Predriver Hook
@item BSP Postdriver Hook
@end itemize

One of the most important functions performed by this routine
is determining where the RTEMS Workspace is to be
located in memory.  All RTEMS objects and task stacks will be
allocated from this Workspace.  The RTEMS Workspace is distinct
from the application heap used for @code{malloc()}.  Many BSPs
place the RTEMS Workspace area at the end of RAM although this is
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
routine performs some explicit initialization.  This routine 
sets the variable @code{rtems_progname} and initiates 
multitasking via a call to the RTEMS directive
@code{rtems_initialize_executive_late}.  It is important to note
that the executive does not return to this routine until the
RTEMS directive @code{rtems_shutdown_executive} is invoked.

The RTEMS initialization procedure is described in the @b{Initialization
Manager} chapter of the @b{RTEMS Application C User's Guide}.
Please refer to that manual for more information.

@subsection RTEMS Pretasking Callback

The @code{pretasking_hook} entry in the RTEMS CPU Configuration
Table may be the address of a user provided routine that is
invoked once RTEMS API initialization is complete but before interrupts
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
Most BSPs set the debug level for the system and initialize the
RTEMS C Library support in their
implementation of @code{bsp_pretasking_hook()}.  This initialization
includes the application heap used by the @code{malloc} family
of routines as well as the reentrancy support for the C Library.

The routine @code{bsp_libc_init} routine invoked from the
@code{bsp_pretasking_hook()} routine is passed the starting
address, length, and growth amount passed to @code{sbrk}.  
This "sbrk amount" is only used if the heap runs out of
memory.  In this case, the RTEMS malloc implementation will
invoked @code{sbrk} to obtain more memory.  See
@ref{Miscellaneous Support Files sbrk() Implementation} for more details.

@subsection RTEMS Predriver Callback

The @code{predriver_hook} entry in the RTEMS CPU Configuration
Table may be the address of a user provided routine that is
is invoked immediately before the the device drivers and MPCI
are initialized. RTEMS
initialization is complete but interrupts and tasking are disabled.
This field may be NULL to indicate that the hook is not utilized.

Most BSPs do not use this callback.

@subsection Device Driver Initialization

At this point in the initialization sequence, the initialization 
routines for all of the device drivers specified in the Device
Driver Table are invoked.  The initialization routines are invoked
in the order they appear in the Device Driver Table.

The Driver Address Table is part of the RTEMS Configuration Table. It
defines device drivers entry points (initialization, open, close, read,
write, and control). For more information about this table, please
refer to the @b{Configuring a System} chapter in the
@b{RTEMS Application C User's Guide}.

The RTEMS initialization procedure calls the initialization function for
every driver defined in the RTEMS Configuration Table (this allows
one to include only the drivers needed by the application). 

All these primitives have a major and a minor number as arguments: 

@itemize @bullet

@item the major number refers to the driver type,

@item the minor number is used to control two peripherals with the same
driver (for instance, we define only one major number for the serial
driver, but two minor numbers for channel A and B if there are two
channels in the UART). 

@end itemize

@subsection RTEMS Postdriver Callback

The @code{postdriver_hook} entry in the RTEMS CPU Configuration
Table may be the address of a user provided routine that is
invoked immediately after the the device drivers and MPCI are initialized.
Interrupts and tasking are disabled.  The postdriver hook is optional.

Although optional, most of the RTEMS BSPs provide a postdriver hook
callback.  This routine is usually called @code{bsp_postdriver_hook}
and is found in the file:

@example
c/src/lib/libbsp/CPU/BSP/startup/bsppost.c
@end example

The @code{bsp_postdriver_hook()} routine is the appropriate place to
perform initialization that must be performed before the first task 
executes but requires that a device driver be initialized.  The 
shared implementation of the postdriver hook opens the default 
standard in, out, and error files and associates them with 
@code{/dev/console}.

@section The Interrupt Vector Table

The Interrupt Vector Table is called different things on different
processor families but the basic functionality is the same.  Each
entry in the Table corresponds to the handler routine for a particular
interrupt source.  When an interrupt from that source occurs, the 
specified handler routine is invoked.  Some context information is
saved by the processor automatically when this happens.  RTEMS saves
enough context information so that an interrupt service routine
can be implemented in a high level language.

On some processors, the Interrupt Vector Table is at a fixed address.  If
this address is in RAM, then usually the BSP only has to initialize
it to contain pointers to default handlers.  If the table is in ROM,
then the application developer will have to take special steps to
fill in the table.

If the base address of the Interrupt Vector Table can be dynamically 
changed to an arbitrary address, then the RTEMS port to that processor
family will usually allocate its own table and install it.  For example,
on some members of the Motorola MC68xxx family, the Vector Base Register
(@code{vbr}) contains this base address.  

@subsection Interrupt Vector Table on the gen68340 BSP

The gen68340 BSP provides a default Interrupt Vector Table in the
file @code{$BSP_ROOT/start340/start340.s}.  After the @code{entry}
label is the definition of space reserved for the table of
interrupts vectors.  This space is assigned the symbolic name
of @code{__uhoh} in the @code{gen68340} BSP.

At @code{__uhoh} label is the default interrupt handler routine. This
routine is only called when an unexpected interrupts is raised.  One can
add their own routine there (in that case there's a call to a routine -
$BSP_ROOT/startup/dumpanic.c - that prints which address caused the
interrupt and the contents of the registers, stack, etc.), but this should
not return. 

@section Chip Select Initialization

When the microprocessor accesses a memory area, address decoding is
handled by an address decoder, so that the microprocessor knows which
memory chip(s) to access.   The following figure illustrates this:

@example
@group
                     +-------------------+
         ------------|                   |
         ------------|                   |------------
         ------------|      Address      |------------
         ------------|      Decoder      |------------
         ------------|                   |------------
         ------------|                   |
                     +-------------------+
           CPU Bus                           Chip Select
@end group
@end example


The Chip Select registers must be programmed such that they match
the @code{linkcmds} settings. In the gen68340 BSP, ROM and RAM
addresses can be found in both the @code{linkcmds} and initialization
code, but this is not a great way to do this.  It is better to
define addresses in the linker script.

@section Integrated Processor Registers Initialization

The CPUs used in many embedded systems are highly complex devices
with multiple peripherals on the CPU itself.  For these devices,
there are always some specific integrated processor registers
that must be initialized.  Refer to the processors' manuals for
details on these registers and be VERY careful programming them.

@section Data Section Recopy

The next initialization part can be found in
@code{$BSP340_ROOT/start340/init68340.c}. First the Interrupt
Vector Table is copied into RAM, then the data section recopy is initiated
(_CopyDataClearBSSAndStart in @code{$BSP340_ROOT/start340/startfor340only.s}). 

This code performs the following actions:

@itemize @bullet

@item copies the .data section from ROM to its location reserved in RAM
(see @ref{Linker Script Initialized Data} for more details about this copy),

@item clear @code{.bss} section (all the non-initialized
data will take value 0). 

@end itemize

@section RTEMS-Specific Initialization

@section The RTEMS configuration table

The RTEMS configuration table contains the maximum number of objects RTEMS
can handle during the application (e.g. maximum number of tasks,
semaphores, etc.). It's used to allocate the size for the RTEMS inner data
structures. 

The RTEMS configuration table is application dependent, which means that
one has to provide one per application. It is usually defined
by defining macros and including the header file @code{<confdefs.h>}.
In simple applications such as the tests provided with RTEMS, it is
commonly found in the main module of the application.  For more complex
applications, it may be in a file by itself.

The header file @code{<confdefs.h>} defines a constant table named
@code{Configuration}.  It is common practice for the BSP to copy 
this table into a modifiable copy named @code{BSP_Configuration}.
This copy of the table is modified to define the base address of the
RTEMS Executive Workspace as well as to reflect any BSP and
device driver requirements not automatically handled by the application.

For more information on the RTEMS Configuration Table, refer to the
@b{RTEMS Application C User's Guide}.

