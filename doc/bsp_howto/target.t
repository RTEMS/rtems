@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Target Dependent Files

RTEMS has a multi-layered approach to portability.  This is done to
maximize the amount of software that can be reused.  Much of the
RTEMS source code can be reused on all RTEMS platforms.  Other parts
of the executive are specific to hardware in some sense.
RTEMS classifies target dependent code based upon its dependencies
into one of the following categories.

@itemize @bullet
@item CPU dependent
@item Board dependent
@item Peripheral dependent
@end itemize

@section CPU Dependent

This class of code includes the foundation
routines for the executive proper such as the context switch and
the interrupt subroutine implementations.  Sources for the supported
processor families can be found in @code{c/src/exec/score/cpu}.
A good starting point for a new family of processors is the
@code{no_cpu} directory, which holds both prototypes and
descriptions of each needed CPU dependent function. 

CPU dependent code is further subcategorized if the implementation is
dependent on a particular CPU model.  For example, the MC68000 and MC68020
processors are both members of the m68k CPU family but there are significant
differents between these CPU models which RTEMS must take into account.

@section Board Dependent

This class of code provides the most specific glue between RTEMS and
a particular board.  This code is represented by the Board Support Packages
and associated Device Drivers.  Sources for the BSPs included in the
RTEMS distribution are located in the directory @code{c/src/lib/libbsp}.
The BSP source directory is further subdivided based on the CPU family
and BSP.

Some BSPs may support multiple board models within a single board family.
This is necessary when the board supports multiple variants on a 
single base board.  For example, the Motorola MVME162 board family has a
fairly large number of variations based upon the particular CPU model
and the peripherals actually placed on the board.

@section Peripheral Dependent

This class of code provides a reusable library of peripheral device
drivers which can be tailored easily to a particular board.  The
libchip library is a collection of reusable software objects that
correspond to standard controllers.  Just as the hardware engineer
chooses a standard controller when designing a board, the goal of
this library is to let the software engineer do the same thing.

The source code for the reusable peripheral driver library may be found
in the directory @code{c/src/lib/libchip}.  The source code is further
divided based upon the class of hardware.  Example classes include serial
communications controllers, real-time clocks, non-volatile memory, and
network controllers.

@section Questions to Ask

When evaluating what is required to support RTEMS applications on
a particular target board, the following questions should be asked:

@itemize @bullet

@item Does a BSP for this board exist?

@item Does a BSP for a similar board exists?

@item Is the board's CPU supported?

@end itemize

If there is already a BSP for the board, then things may already be ready
to start developing application software.  All that remains is to verify
that the existing BSP provides device drivers for all the peripherals
on the board that the application will be using.  For example, the application
in question may require that the board's Ethernet controller be used and
the existing BSP may not support this.

If the BSP does not exist and the board's CPU model is supported, then
examine the reusable chip library and existing BSPs for a close match.  
Other BSPs and libchip provide starting points for the development 
of a new BSP.  It is often possible to copy existing components in
the reusable chip library or device drivers from BSPs from different
CPU families as the starting point for a new device driver. 
This will help reduce the development effort required.  

If the board's CPU family is supported but the particular CPU model on
that board is not, then the RTEMS port to that CPU family will have to
be augmented.  After this is done, development of the new BSP can proceed.

Otherwise both CPU dependent code and the BSP will have to be written.

Regardless of the amount of development required, OAR Corporation
offers custom development services to assist RTEMS users.
For more information on custom development, training courses, and
support, contact OAR Corporation at
@uref{http://www.oarcorp.com}.


@section CPU Dependent Executive Files

The CPU dependent files in the RTEMS executive source code are found
in the following directory:

@example
c/src/exec/score/cpu/@i{CPU}
@end example

where @i{CPU} is replaced with the CPU family name.

Within each CPU dependent directory inside the executive proper is a 
file named @code{@i{CPU}.h} which contains information about each of the
supported CPU models within that family.

@section CPU Dependent Support Files

The CPU dependent support files contain routines which aid in the development
of applications using that CPU family.  For example, the support routines
may contain standard trap handlers for alignment or floating point exceptions
or device drivers for peripheral controllers found on the CPU itself.
This class of code may be found in the following directory:

@example
c/src/lib/libcpu/@i{CPU}
@end example

CPU model dependent support code is found in the following directory:

@example
c/src/lib/libcpu/@i{CPU}/@i{CPU_MODEL}
@end example

@section Board Support Package Structure

The BSPs are all under the c/src/lib/libbsp directory.  Below this
directory, there is a subdirectory for each CPU family.  Each BSP
is found under the subdirectory for the appropriate processor
family (m68k, powerpc, etc.).  In addition, there is source code
available which may be shared across all BSPs regardless of
the CPU family or just across BSPs within a single CPU family.  This
results in a BSP using the following directories:

@example
c/src/lib/libbsp/shared
c/src/lib/libbsp/@i{CPU}/shared
c/src/lib/libbsp/@i{CPU}/@i{BSP}
@end example

Under each BSP specific directory, there is a collection of
subdirectories.  For commonly provided functionality, the BSPs
follow a convention on subdirectory naming.  The following list
describes the commonly found subdirectories under each BSP.

@itemize @bullet

@item @b{console}:
is technically the serial driver for the BSP rather
than just a console driver, it deals with the board
UARTs (i.e. serial devices).

@item @b{clock}:
support for the clock tick -- a regular time basis to the kernel.

@item @b{timer}:
support of timer devices.

@item @b{rtc}:
support for the hardware real-time clock.

@item @b{nvmem}:
support for non-volatile memory such as EEPROM or Flash.

@item @b{network}:
the Ethernet driver.

@item @b{shmsupp}:
support of shared memory driver MPCI layer in a multiprocessor system,

@item @b{include}:
include files for this BSP.

@item @b{wrapup}:
bundles all the components necessary to construct the BSP library.

@end itemize

The build order of the BSP is determined by the Makefile structure.
This structure is discussed in more detail in the @ref{Makefiles}
chapter.

@b{NOTE:} This manual refers to the gen68340 BSP for numerous concrete
examples.  You should have a copy of the gen68340 BSP available while
reading this piece of documentation.   This BSP is located in the
following directory:

@example
c/src/lib/libbsp/m68k/gen68340
@end example

Later in this document, the $BSP340_ROOT label will be used
to refer to this directory.

