@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter CPU and Board Dependent Files

RTEMS divides board dependencies into two parts:

@itemize @bullet

@item the CPU dependant code : it provides basics calls to the kernel,
just as the context switch or the interrupt subroutine implementations.
Sources for the supported families of processors can be found in
$RTEMS_ROOT/c/src/exec/score/cpu. A good starti ng point for a new family
of processors is the no_cpu directory, which holds both prototypes and
descriptions of each needed CPU dependant function. 

@item the board dependant code : it includes support for a given board,
such as the board initialization code and drivers for the various devices:

@end itemize

Porting RTEMS on a new board should raise two questions:

@itemize @bullet

@item is the main board CPU supported ?

@item does a BSP for a similar board exists ?

@end itemize

If the main board CPU is supported, you will only have do write the Board
Support Package. Otherwise you'll have to write both CPU dependant code
and the BSP. One should always start writing a BSP from a similar one. 

@section CPU Dependent Executive Files

XXX

@section CPU Dependent Support Files

XXX

@section Board Support Package Structure

The BSPs are kept in the $RTEMS_ROOT/c/src/lib/libbsp directory. They
are filed under the processor family (m68k, powerpc, etc.). A given BSP
consists in the following directories: 

@itemize @bullet

@item @b{clock} - 
support for the realtime clock, which provides a regular
time basis to the kernel,

@item @b{console} - 
rather the serial driver than only a console driver, it
deals with the board UARTs (i.e. serial devices),

@item @b{include} - 
the include files,

@item @b{startup} - 
the board initialization code,

@item @b{timer} - 
support of timer devices,

@item @b{shmsupp} - 
support of shared memory in a multiprocessor system,

@item @b{network} - 
the Ethernet driver. 

@item @b{rtc} - 
the real-time clock driver. 

@item @b{wrapup} - XXX

@end itemize

Another important element are the makefiles, which have to be provided by
the user. 


@b{NOTE:} This manual refers to the gen68340 BSP for numerous concrete
examples.  You should have a copy of the gen68340 BSP available while
reading this piece of documentation.   This BSP is located in the
following directory:

@example
$RTEMS_ROOT/c/src/lib/libbsp/m68k/gen68340
@end example

Later in this document, the $BSP340_ROOT label will be used
to refer to this directory.

