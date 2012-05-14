@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

@subsection Timer Event Notification Manifest Constants

@example
CLOCK_REALTIME, Constant, Implemented
TIMER_ABSTIME, Constant, Implemented
@end example

@section Clock and Timer Functions

@subsection Clocks

@example
clock_settime(), Function, Partial Implementation
clock_gettime(), Function, Partial Implementation
clock_getres(), Function, Implemented
@end example

@subsection Create a Per-Process Timer

@example
timer_create(), Function, Implemented
@end example

@subsection Delete a Per-Process Timer

@example
timer_delete(), Function, Implemented
@end example

@subsection Per-Process Timers

@example
timer_settime(), Function, Implemented
timer_gettime(), Function, Implemented
timer_getoverrun(), Function, Implemented
@end example

@subsection High Resolution Sleep

@example
nanosleep(), Function, Implemented
@end example

