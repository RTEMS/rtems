@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Clock Manager

@cindex clock

@section Introduction

The clock manager provides support for time of day
and other time related capabilities.  The directives provided by
the clock manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}clock_set} - Set system date and time
@item @code{@value{DIRPREFIX}clock_get} - Get system date and time information
@item @code{@value{DIRPREFIX}clock_tick} - Announce a clock tick
@end itemize

@section Background

@subsection Required Support

For the features provided by the clock manager to be
utilized, periodic timer interrupts are required.  Therefore, a
real-time clock or hardware timer is necessary to create the
timer interrupts.  The @code{@value{DIRPREFIX}clock_tick}
directive is normally called
by the timer ISR to announce to RTEMS that a system clock tick
has occurred.  Elapsed time is measured in ticks.  A tick is
defined to be an integral number of microseconds which is
specified by the user in the Configuration Table.

@subsection Time and Date Data Structures

The clock facilities of the clock manager operate
upon calendar time.  These directives utilize the following date
and time @value{STRUCTURE} for the native time and date format:


@ifset is-C
@findex rtems_time_of_day
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
@code{@value{DIRPREFIX}clock_get} directive.  Some applications
expect to operate on a "UNIX-style" date and time data structure.  The 
@code{@value{DIRPREFIX}clock_get} directive can optionally return
the current date and time in the
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

@subsection Clock Tick and Timeslicing

@cindex timeslicing

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

The @code{@value{DIRPREFIX}clock_tick}
directive implements timeslicing by
decrementing the running task's time-remaining counter when both
timeslicing and preemption are enabled.  If the task's timeslice
has expired, then that task will be preempted if there exists a
ready task of equal priority.

@subsection Delays

@cindex delays

A sleep timer allows a task to delay for a given
interval or up until a given time, and then wake and continue
execution.  This type of timer is created automatically by the
@code{@value{DIRPREFIX}task_wake_after}
and @code{@value{DIRPREFIX}task_wake_when} directives and, as a result,
does not have an RTEMS ID.  Once activated, a sleep timer cannot
be explicitly deleted.  Each task may activate one and only one
sleep timer at a time.

@subsection Timeouts

@cindex timeouts

Timeouts are a special type of timer automatically
created when the timeout option is used on the
@code{@value{DIRPREFIX}message_queue_receive},
@code{@value{DIRPREFIX}event_receive},
@code{@value{DIRPREFIX}semaphore_obtain} and
@code{@value{DIRPREFIX}region_get_segment} directives.  
Each task may have one and only one timeout active at a time.  
When a timeout expires, it unblocks the task with a timeout status code.

@section Operations

@subsection Announcing a Tick

RTEMS provides the @code{@value{DIRPREFIX}clock_tick} directive which is
called from the user's real-time clock ISR to inform RTEMS that
a tick has elapsed.  The tick frequency value, defined in
microseconds, is a configuration parameter found in the
Configuration Table.  RTEMS divides one million microseconds
(one second) by the number of microseconds per tick to determine
the number of calls to the
@code{@value{DIRPREFIX}clock_tick} directive per second.  The
frequency of @code{@value{DIRPREFIX}clock_tick}
calls determines the resolution
(granularity) for all time dependent RTEMS actions.  For
example, calling @code{@value{DIRPREFIX}clock_tick}
ten times per second yields a higher
resolution than calling @code{@value{DIRPREFIX}clock_tick}
two times per second.  The @code{@value{DIRPREFIX}clock_tick}
directive is responsible for maintaining both
calendar time and the dynamic set of timers.

@subsection Setting the Time

The @code{@value{DIRPREFIX}clock_set} directive allows a task or an ISR to
set the date and time maintained by RTEMS.  If setting the date
and time causes any outstanding timers to pass their deadline,
then the expired timers will be fired during the invocation of
the @code{@value{DIRPREFIX}clock_set} directive.

@subsection Obtaining the Time

The @code{@value{DIRPREFIX}clock_get} directive allows a task or an ISR to
obtain the current date and time or date and time related
information.  The current date and time can be returned in
either native or UNIX-style format.  Additionally, the
application can obtain date and time related information such as
the number of seconds since the RTEMS epoch, the number of ticks
since the executive was initialized, and the number of ticks per
second.  The information returned by the
@code{@value{DIRPREFIX}clock_get} directive is
dependent on the option selected by the caller.  This
is specified using one of the following constants
associated with the enumerated type
@code{@value{DIRPREFIX}clock_get_options}:

@findex rtems_clock_get_options 

@itemize @bullet
@item @code{@value{RPREFIX}CLOCK_GET_TOD} - obtain native style date and time

@item @code{@value{RPREFIX}CLOCK_GET_TIME_VALUE} - obtain UNIX-style
date and time

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_SINCE_BOOT} - obtain number of ticks
since RTEMS was initialized

@item @code{@value{RPREFIX}CLOCK_GET_SECONDS_SINCE_EPOCH} - obtain number
of seconds since RTEMS epoch

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_PER_SECOND} - obtain number of clock
ticks per second

@end itemize

Calendar time operations will return an error code if
invoked before the date and time have been set.

@section Directives

This section details the clock manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection CLOCK_SET - Set system date and time

@subheading CALLING SEQUENCE:

@cindex set the time of day

@ifset is-C
@findex rtems_clock_set
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
@code{@value{RPREFIX}SUCCESSFUL} - date and time set successfully@*
@code{@value{RPREFIX}INVALID_TIME_OF_DAY} - invalid time of day

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
@code{@value{DIRPREFIX}clock_set} is required to re-initialize
the system date and time to application specific specifications.

@c
@c
@c
@page
@subsection CLOCK_GET - Get system date and time information

@cindex obtain the time of day

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_clock_get
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
@code{@value{RPREFIX}SUCCESSFUL} - current time obtained successfully@*
@code{@value{RPREFIX}NOT_DEFINED} - system date and time is not set

@subheading DESCRIPTION:

This directive obtains the system date and time.  If
the caller is attempting to obtain the date and time (i.e.
option is set to either @code{@value{RPREFIX}CLOCK_GET_SECONDS_SINCE_EPOCH},
@code{@value{RPREFIX}CLOCK_GET_TOD}, or
@code{@value{RPREFIX}CLOCK_GET_TIME_VALUE}) and the date and time
has not been set with a previous call to
@code{@value{DIRPREFIX}clock_set}, then the
@code{@value{RPREFIX}NOT_DEFINED} status code is returned. 
The caller can always obtain the number of ticks per second (option is
@code{@value{RPREFIX}CLOCK_GET_TICKS_PER_SECOND}) and the number of
ticks since the executive was initialized option is
@code{@value{RPREFIX}CLOCK_GET_TICKS_SINCE_BOOT}).

The @code{option} argument may taken on any value of the enumerated
type @code{rtems_clock_get_options}.  The data type expected for
@code{time_buffer} is based on the value of @code{option} as
indicated below:

@findex rtems_clock_get_options 
@ifset is-C
@itemize @bullet
@item @code{@value{RPREFIX}CLOCK_GET_TOD} - (rtems_time_of_day *)

@item @code{@value{RPREFIX}CLOCK_GET_TIME_VALUE} - (rtems_clock_time_value *)

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_SINCE_BOOT} - (rtems_interval *)

@item @code{@value{RPREFIX}CLOCK_GET_SECONDS_SINCE_EPOCH} - (rtems_interval *)

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_PER_SECOND} - (rtems_interval *)

@end itemize
@end ifset

@ifset is-Ada
@itemize @bullet
@item @code{@value{RPREFIX}CLOCK_GET_TOD} - Address of an variable of
type RTEMS.Time_Of_Day

@item @code{@value{RPREFIX}CLOCK_GET_TIME_VALUE} - Address of an variable of
type RTEMS.Clock_Time_Value

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_SINCE_BOOT} - Address of an
variable of type RTEMS.Interval

@item @code{@value{RPREFIX}CLOCK_GET_SECONDS_SINCE_EPOCH} - Address of an
variable of type RTEMS.Interval

@item @code{@value{RPREFIX}CLOCK_GET_TICKS_PER_SECOND} - Address of an
variable of type RTEMS.Interval

@end itemize
@end ifset

@subheading NOTES:

This directive is callable from an ISR.

This directive will not cause the running task to be
preempted.  Re-initializing RTEMS causes the system date and
time to be reset to an uninitialized state.  Another call to
@code{@value{DIRPREFIX}clock_set} is required to re-initialize the
system date and time to application specific specifications.

@c
@c
@c
@page
@subsection CLOCK_TICK - Announce a clock tick

@cindex clock tick

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_clock_tick
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
@code{@value{RPREFIX}SUCCESSFUL} - current time obtained successfully

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

