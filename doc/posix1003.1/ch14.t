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

@example
struct timespec, Type, Implemented
struct itimerspec, Type, Implemented
@end example

@subsection Timer Event Notification Control Block

@subsection Type Definitions

@example
clockid_t, Type, Implemented
timerid_t, Type, Implemented
@end example

@subsection Manifest Constants

@section Clock and Timer Functions

@subsection Clocks

@example
clock_settime(), Function, Partial Implementation
clock_gettime(), Function, Partial Implementation
clock_getres(), Function, Implemented
@end example

@subsection Create a Per-Process Timer

@example
timer_create(), Function, Dummy Implementation
@end example

@subsection Delete a Per-Process Timer

@example
timer_delete(), Function, Dummy Implementation
@end example

@subsection Per-Process Timers

@example
timer_settime(), Function, Dummy Implementation
timer_gettime(), Function, Dummy Implementation
timer_getoverrun(), Function, Dummy Implementation
@end example

@subsection High Resolution Sleep

@example
nanosleep(), Function, Implemented
@end example

