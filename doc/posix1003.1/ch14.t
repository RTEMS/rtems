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
clock_settime(), Function
clock_gettime(), Function
clock_getres(), Function
@end example

@subsection Create a Per-Process Timer

@example
timer_create(), Function
@end example

@subsection Delete a Per-Process Timer

@example
timer_delete(), Function
@end example

@subsection Per-Process Timers

@example
timer_settime(), Function
timer_gettime(), Function
timer_getoverrun(), Function
@end example

@subsection High Resolution Sleep

@example
nanosleep(), Function
@end example

