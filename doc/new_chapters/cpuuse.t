@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter CPU Usage Statistics

@section Introduction

The CPU usage statistics manager is an RTEMS support
component that provides a convenient way to manipulate
the CPU usage information associated with each task
The routines provided by the CPU usage statistics manager are:

@itemize @bullet
@item @code{CPU_usage_Dump} - Report CPU Usage Statistics
@item @code{CPU_usage_Reset} - Reset CPU Usage Statistics
@end itemize

@section Background

@section Operations

@section Report CPU Usage Statistics

@subsection Reporting Period Statistics

The application may dynamically report the CPU usage for every
task in the system by calling the @code{CPU_usage_Dump} routine.
This routine prints a table with the following information per task:

@itemize @bullet
@item task id
@item task name
@item number of clock ticks executed
@item percentage of time consumed by this task
@end itemize

The following is an example of the report generated:

@example
@group
CPU Usage by thread
   ID        NAME        TICKS    PERCENT
0x04010001   IDLE           0     0.000
0x08010002   TA1         1203     0.748
0x08010003   TA2          203     0.126
0x08010004   TA3          202     0.126

Ticks since last reset = 1600

Total Units = 1608
@end group
@end example

Notice that the "Total Units" is greater than the ticks per reset.
This is an artifact of the way in which RTEMS keeps track of CPU 
usage.  When a task is context switched into the CPU, the number
of clock ticks it has executed is incremented.  While the task
is executing, this number is incremented on each clock tick.  
Otherwise, if a task begins and completes execution between
successive clock ticks, there would be no way to tell that it 
executed at all.

Another thing to keep in mind when looking at idle time, is that
many systems -- especially during debug -- have a task providing
some type of debug interface.  It is usually fine to think of the
total idle time as being the sum of the IDLE task and a debug
task that will not be included in a production build of an application.

@section Reset CPU Usage Statistics

Invoking the @code{CPU_usage_Reset} routine resets the CPU usage 
statistics for all tasks in the system. 

@section Directives

This section details the CPU usage statistics manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection CPU_usage_Dump - Report CPU Usage Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void CPU_usage_Dump( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine prints out a table detailing the CPU usage statistics for
all tasks in the system.

@subheading NOTES:

NONE

@page
@subsection CPU_usage_Reset - Reset CPU Usage Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void CPU_usage_Reset( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine re-initializes the CPU usage statistics for all tasks
in the system to their initial state.  The initial state is that
a task has not executed and thus has consumed no CPU time.
default state which is when zero period executions have occurred.

@subheading NOTES:

NONE
