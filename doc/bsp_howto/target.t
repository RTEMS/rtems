@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Target Dependent and Board Dependent Files

RTEMS divides board dependencies into two parts :

@itemize @bullet

@item the CPU dependant code : it provides basics calls to the kernel,
just as the context switch or the interrupt subroutine implementations.
Sources for the supported families of processors can be found in
$RTEMS_ROOT/c/src/exec/score/cpu. A good starti ng point for a new family
of processors is the no_cpu directory, which holds both prototypes and
descriptions of each needed CPU dependant function. 

@item the board dependant code : it includes support for a given board,
such as the board initialization code and drivers for the various devices:
clock, UARTs, ethernet board, etc.


@end itemize

Porting RTEMS on a new board should raise two questions :

@itemize @bullet

@item is the main board CPU supported ?

@item does a BSP for a similar board exists ?

@end itemize

If the main board CPU is supported, you will only have do write the Board
Support Package. Otherwise you'll have to write both CPU dependant code
and the BSP. One should always start writing a BSP from a similar one. 

