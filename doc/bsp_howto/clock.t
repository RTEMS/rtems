@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Clock Driver

@subsection = Introduction

The clock driver aims at giving a steady time basis to the kernel, so that
the RTEMS primitives that need a clock tick work properly. <insert a link
to the Clock Driver defs>

The clock driver is located in the clock directory of the BSP. 

@subsection = Primitives

@subsection = Initialization

The major and minor numbers of the clock driver can be made available to
the others, such as the Shared Memory Driver. 

Then you have to program your integrated processor periodic interval timer
so that an interrupt is generated every m microseconds, where m =
BSP_Configuration.microseconds_per_tick. Sometimes the periodic interval
timer can use a prescaler so you have to look carefully at your user's
manual to determine the correct value. 

You must use the RTEMS primitive to put your clock interrupt routine in
the VBR: 

@example
rtems_interrupt_catch (InterruptHandler, CONSOLE_VECTOR, &old_handler); 
@end example

@subsection = The clock interrupt subroutine

It only has to inform the kernel that a ticker has elapsed, so call : 

@example
rtems_clock_tick(); 
@end example

