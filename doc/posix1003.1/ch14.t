@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Clocks and Timers

@section Data Definitions for Clocks and Timers

@subsection Time Value Specification Structures

@subsection Timer Event Notification Control Block

@subsection Type Definitions

@subsection Manifest Constants

@section Clock and Timer Functions

@subsection Clocks

@example
clock_settime()
clock_gettime()
clock_getres()
@end example

@subsection Create a Per-Process Timer

@example
timer_create()
@end example

@subsection Delete a Per-Process Timer

@example
timer_delete()
@end example

@subsection Per-Process Timers

@example
timer_settime()
timer_gettime()
timer_getoverrun()
@end example

@subsection High Resolution Sleep

@example
nanosleep()
@end example

