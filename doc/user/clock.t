@c
@c  COPYRIGHT (c) 1996.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Clock Manager, Clock Manager Introduction, INTERRUPT_IS_IN_PROGRESS - Is an ISR in Progress, Top
@end ifinfo
@chapter Clock Manager
@ifinfo
@menu
* Clock Manager Introduction::
* Clock Manager Background::
* Clock Manager Operations::
* Clock Manager Directives::
@end menu
@end ifinfo

@ifinfo
@node Clock Manager Introduction, Clock Manager Background, Clock Manager, Clock Manager
@end ifinfo
@section Introduction

The clock manager provides support for time of day
and other time related capabilities.  The directives provided by
the clock manager are:

@itemize @bullet
@item @code{clock_set} - Set system date and time
@item @code{clock_get} - Get system date and time information
@item @code{clock_tick} - Announce a clock tick
@end itemize

@ifinfo
@node Clock Manager Background, Required Support, Clock Manager Introduction, Clock Manager
@end ifinfo
@section Background
@ifinfo
@menu
* Required Support::
* Time and Date Data Structures::
* Clock Tick and Timeslicing::
* Delays::
* Timeouts::
@end menu
@end ifinfo

@ifinfo
@node Required Support, Time and Date Data Structures, Clock Manager Background, Clock Manager Background
@end ifinfo
@subsection Required Support

For the features provided by the clock manager to be
utilized, periodic timer interrupts are required.  Therefore, a
real-time clock or hardware timer is necessary to create the
timer interrupts.  The clock_tick directive is normally called
by the timer ISR to announce to RTEMS that a system clock tick
has occurred.  Elapsed time is measured in ticks.  A tick is
defined to be an integral number of microseconds which is
specified by the user in the Configuration Table.

@ifinfo
@node Time and Date Data Structures, Clock Tick and Timeslicing, Required Support, Clock Manager Background
@end ifinfo
@subsection Time and Date Data Structures

The clock facilities of the clock manager operate
upon calendar time.  These directives utilize the following date
and time @value{STRUCTURE} for the native time and date format:

@ifset is-C
@example
struct rtems_tod_control @{
  rtems_unsigned32 year;   /* greater than 1987 */
  rtems_unsigned32 month;  /* 1 - 12 */
  rtems_unsigned32 day;    /* 1 - 31 */
  rtems_unsigned32 hour;   /* 0 - 23 */
  rtems_unsigned32 minute; /* 0 - 59 */
  rtems_unsigned32 second; /* 0 - 59 */
  rtems_unsigned32 ticks;  /* elapsed between seconds */
@};

typedef struct rtems_tod_control rtems_time_of_day;
@end example
@end ifset

@ifset is-Ada
@example
type Time_Of_Day is
   record
      Year    : RTEMS.Unsigned32; -- year, A.D.
      Month   : RTEMS.Unsigned32; -- month, 1 .. 12
      Day     : RTEMS.Unsigned32; -- day, 1 .. 31
      Hour    : RTEMS.Unsigned32; -- hour, 0 .. 23
      Minute  : RTEMS.Unsigned32; -- minute, 0 .. 59
      Second  : RTEMS.Unsigned32; -- second, 0 .. 59
      Ticks   : RTEMS.Unsigned32; -- elapsed ticks between seconds
   end record;
@end example
@end ifset


The native date and time format is the only format
supported when setting the system date and time using the
clock_get directive.  Some applications expect to operate on a
"UNIX-style" date and time data structure.  The clock_get
directive can optionally return the current date and time in the
following @value{STRUCTURE}:

@ifset is-C
@example
@group
typedef struct @{
  rtems_unsigned32 seconds;       /* seconds since RTEMS epoch*/
  rtems_unsigned32 microseconds;  /* since last second        */
@} rtems_clock_time_value;
@end group
@end example
@end ifset

@ifset is-Ada
@example
type Clock_Time_Value is
   record
      Seconds      : Unsigned32;
      Microseconds : Unsigned32;
   end record;
@end example
@end ifset

The seconds field in this @value{STRUCTURE} is the number of
seconds since the RTEMS epoch of January 1, 1988.

@ifinfo
@node Clock Tick and Timeslicing, Delays, Time and Date Data Structures, Clock Manager Background
@end ifinfo
@subsection Clock Tick and Timeslicing

Timeslicing is a task scheduling discipline in which
tasks of equal priority are executed for a specific period of
time before control of the CPU is passed to another task.  It is
also sometimes referred to as the automatic round-robin
scheduling algorithm.  The length of time allocated to each task
is known as the quantum or timeslice.

The system's timeslice is defined as an integral
number of ticks, and is specified in the Configuration Table.
The timeslice is defined for the entire system of tasks, but
timeslicing is enabled and disabled on a per task basis.

The clock_tick directive implements timeslicing by
decrementing the running task's time-remaining counter when both
timeslicing and preemption are enabled.  If the task's timeslice
has expired, then that task will be preempted if there exists a
ready task of equal priority.

@ifinfo
@node Delays, Timeouts, Clock Tick and Timeslicing, Clock Manager Background
@end ifinfo
@subsection Delays

A sleep timer allows a task to delay for a given
interval or up until a given time, and then wake and continue
execution.  This type of timer is created automatically by the
task_wake_after and task_wake_when directives and, as a result,
does not have an RTEMS ID.  Once activated, a sleep timer cannot
be explicitly deleted.  Each task may activate one and only one
sleep timer at a time.

@ifinfo
@node Timeouts, Clock Manager Operations, Delays, Clock Manager Background
@end ifinfo
@subsection Timeouts

Timeouts are a special type of timer automatically
created when the timeout option is used on the
message_queue_receive, event_receive, semaphore_obtain and
region_get_segment directives.  Each task may have one and only
one timeout active at a time.  When a timeout expires, it
unblocks the task with a timeout status code.

@ifinfo
@node Clock Manager Operations, Announcing a Tick, Timeouts, Clock Manager
@end ifinfo
@section Operations
@ifinfo
@menu
* Announcing a Tick::
* Setting the Time::
* Obtaining the Time::
@end menu
@end ifinfo

@ifinfo
@node Announcing a Tick, Setting the Time, Clock Manager Operations, Clock Manager Operations
@end ifinfo
@subsection Announcing a Tick

RTEMS provides the clock_tick directive which is
called from the user's real-time clock ISR to inform RTEMS that
a tick has elapsed.  The tick frequency value, defined in
microseconds, is a configuration parameter found in the
Configuration Table.  RTEMS divides one million microseconds
(one second) by the number of microseconds per tick to determine
the number of calls to the clock_tick directive per second.  The
frequency of clock_tick calls determines the resolution
(granularity) for all time dependent RTEMS actions.  For
example, calling clock_tick ten times per second yields a higher
resolution than calling clock_tick two times per second.  The
clock_tick directive is responsible for maintaining both
calendar time and the dynamic set of timers.

@ifinfo
@node Setting the Time, Obtaining the Time, Announcing a Tick, Clock Manager Operations
@end ifinfo
@subsection Setting the Time

The clock_set directive allows a task or an ISR to
set the date and time maintained by RTEMS.  If setting the date
and time causes any outstanding timers to pass their deadline,
then the expired timers will be fired during the invocation of
the clock_set directive.

@ifinfo
@node Obtaining the Time, Clock Manager Directives, Setting the Time, Clock Manager Operations
@end ifinfo
@subsection Obtaining the Time

The clock_get directive allows a task or an ISR to
obtain the current date and time or date and time related
information.  The current date and time can be returned in
either native or UNIX-style format.  Additionally, the
application can obtain date and time related information such as
the number of seconds since the RTEMS epoch, the number of ticks
since the executive was initialized, and the number of ticks per
second.  The information returned by the clock_get directive is
dependent on the option selected by the caller.  The following
options are available:

@itemize @bullet
@item @code{CLOCK_GET_TOD} - obtain native style date and time

@item @code{CLOCK_GET_TIME_VALUE} - obtain UNIX-style date and time

@item @code{CLOCK_GET_TICKS_SINCE_BOOT} - obtain number of ticks
since RTEMS was initialized

@item @code{CLOCK_GET_SECONDS_SINCE_EPOCH} - obtain number of seconds
since RTEMS epoch

@item @code{CLOCK_GET_TICKS_PER_SECOND} - obtain number of clock ticks
per second

@end itemize

Calendar time operations will return an error code if
invoked before the date and time have been set.

@ifinfo
@node Clock Manager Directives, CLOCK_SET - Set system date and time, Obtaining the Time, Clock Manager
@end ifinfo
@section Directives
@ifinfo
@menu
* CLOCK_SET - Set system date and time::
* CLOCK_GET - Get system date and time information::
* CLOCK_TICK - Announce a clock tick::
@end menu
@end ifinfo

This section details the clock manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@ifinfo
@node CLOCK_SET - Set system date and time, CLOCK_GET - Get system date and time information, Clock Manager Directives, Clock Manager Directives
@end ifinfo
@subsection CLOCK_SET - Set system date and time

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_clock_set(
  rtems_time_of_day *time_buffer
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Clock_Set (
   Time_Buffer : in     RTEMS.Time_Of_Day;
   Result      :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - date and time set successfully@*
@code{INVALID_TIME_OF_DAY} - invalid time of day

@subheading DESCRIPTION:

This directive sets the system date and time.  The
date, time, and ticks in the time_buffer @value{STRUCTURE} are all
range-checked, and an error is returned if any one is out of its
valid range.

@subheading NOTES:

Years before 1988 are invalid.

The system date and time are based on the configured
tick rate (number of microseconds in a tick).

Setting the time forward may cause a higher priority
task, blocked waiting on a specific time, to be made ready.  In
this case, the calling task will be preempted after the next
clock tick.

Re-initializing RTEMS causes the system date and time
to be reset to an uninitialized state.  Another call to
clock_set is required to re-initialize the system date and time
to application specific specifications.

@page
@ifinfo
@node CLOCK_GET - Get system date and time information, CLOCK_TICK - Announce a clock tick, CLOCK_SET - Set system date and time, Clock Manager Directives
@end ifinfo
@subsection CLOCK_GET - Get system date and time information

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_clock_get(
  rtems_clock_get_options  option,
  void                    *time_buffer
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Clock_Get (
   Option      : in     RTEMS.Clock_Get_Options;
   Time_Buffer : in     RTEMS.Address;
   Result      :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - current time obtained successfully@*
@code{NOT_DEFINED} - system date and time is not set

@subheading DESCRIPTION:

This directive obtains the system date and time.  If
the caller is attempting to obtain the date and time (i.e.
option is set to either @code{CLOCK_GET_SECONDS_SINCE_EPOCH},
@code{CLOCK_GET_TOD}, or @code{CLOCK_GET_TIME_VALUE}) and the date and time
has not been set with a previous call to clock_set, then the
@code{NOT_DEFINED} status code is returned.  The caller can always
obtain the number of ticks per second (option is
@code{CLOCK_GET_TICKS_PER_SECOND}) and the number of ticks since the
executive was initialized option is @code{CLOCK_GET_TICKS_SINCE_BOOT}).

@subheading NOTES:

This directive is callable from an ISR.

This directive will not cause the running task to be
preempted.  Re-initializing RTEMS causes the system date and
time to be reset to an uninitialized state.  Another call to
clock_set is required to re-initialize the system date and time
to application specific specifications.

@page
@ifinfo
@node CLOCK_TICK - Announce a clock tick, Timer Manager, CLOCK_GET - Get system date and time information, Clock Manager Directives
@end ifinfo
@subsection CLOCK_TICK - Announce a clock tick

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_clock_tick( void );
@end example
@end ifset

@ifset is-Ada
@example
procedure Clock_Tick (
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - current time obtained successfully

@subheading DESCRIPTION:

This directive announces to RTEMS that a system clock
tick has occurred.  The directive is usually called from the
timer interrupt ISR of the local processor.  This directive
maintains the system date and time, decrements timers for
delayed tasks, timeouts, rate monotonic periods, and implements
timeslicing.

@subheading NOTES:

This directive is typically called from an ISR.

The microseconds_per_tick and ticks_per_timeslice
parameters in the Configuration Table contain the number of
microseconds per tick and number of ticks per timeslice,
respectively.

