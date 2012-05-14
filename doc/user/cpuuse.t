@c
@c  COPYRIGHT (c) 1988-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 

@chapter CPU Usage Statistics

@section Introduction

The CPU usage statistics manager is an RTEMS support
component that provides a convenient way to manipulate
the CPU usage information associated with each task
The routines provided by the CPU usage statistics manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}cpu_usage_report} - Report CPU Usage Statistics
@item @code{@value{DIRPREFIX}cpu_usage_reset} - Reset CPU Usage Statistics
@end itemize

@section Background

When analyzing and debugging real-time applications, it is important
to be able to know how much CPU time each task in the system consumes.
This support component provides a mechanism to easily obtain this
information with little burden placed on the target.

The raw data is gathered as part of performing a context switch.  RTEMS
keeps track of how many clock ticks have occurred which the task being
switched out has been executing.  If the task has been running less than
1 clock tick, then for the purposes of the statistics, it is assumed to
have executed 1 clock tick.  This results in some inaccuracy but the
alternative is for the task to have appeared to execute 0 clock ticks.

RTEMS versions newer than the 4.7 release series, support the ability
to obtain timestamps with nanosecond granularity if the BSP provides
support.  It is a desirable enhancement to change the way the usage
data is gathered to take advantage of this recently added capability.
Please consider sponsoring the core RTEMS development team to add
this capability.

@section Operations

@subsection Report CPU Usage Statistics

The application may dynamically report the CPU usage for every
task in the system by calling the
@code{@value{DIRPREFIX}cpu_usage_report} routine.
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
-------------------------------------------------------------------------------
                              CPU USAGE BY THREAD
------------+----------------------------------------+---------------+---------
 ID         | NAME                                   | SECONDS       | PERCENT
------------+----------------------------------------+---------------+---------
 0x04010001 | IDLE                                   |             0 |   0.000
 0x08010002 | TA1                                    |          1203 |   0.748
 0x08010003 | TA2                                    |           203 |   0.126
 0x08010004 | TA3                                    |           202 |   0.126
------------+----------------------------------------+---------------+---------
 TICKS SINCE LAST SYSTEM RESET:                                           1600
 TOTAL UNITS:                                                             1608
-------------------------------------------------------------------------------
@end group
@end example

Notice that the "TOTAL UNITS" is greater than the ticks per reset.
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

@subsection Reset CPU Usage Statistics

Invoking the @code{@value{DIRPREFIX}cpu_usage_reset} routine resets
the CPU usage statistics for all tasks in the system. 

@section Directives

This section details the CPU usage statistics manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection cpu_usage_report - Report CPU Usage Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_cpu_usage_report( void );
@end example
@end ifset

@ifset is-Ada
@example
procedure CPU_Usage_Report;
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine prints out a table detailing the CPU usage statistics for
all tasks in the system.

@subheading NOTES:

The table is printed using the @code{printk} routine.

@page
@subsection cpu_usage_reset - Reset CPU Usage Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void rtems_cpu_usage_reset( void );
@end example
@end ifset

@ifset is-Ada
@example
procedure CPU_Usage_Reset;
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
