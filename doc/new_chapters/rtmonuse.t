@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Rate Monotonic Period Statistics

@section Introduction

The rate monotonic period statistics manager is an RTEMS support
component that maintains statistics on the execution characteristics
of each task using a period.  The routines provided by the rate
monotonic period statistics manager are:

@itemize @bullet
@item @code{Period_usage_Initialize} - Initialize the Period Statistics
@item @code{Period_usage_Reset} -  Reset the Period Statistics
@item @code{Period_usage_Update} - Update the Statistics for this Period
@item @code{Period_usage_Dump} - Report Period Statistics Usage
@end itemize

@section Background

@section Period Statistics

This manager maintains a set of statistics on each period.  The following
is a list of the information kept:

@itemize @bullet
@item @code{id}
is the id of the period.

@item @code{count}
is the total number of periods executed.

@item @code{missed_count}
is the number of periods that were missed.

@item @code{min_cpu_time}
is the minimum amount of CPU execution time consumed
on any execution of the periodic loop.

@item @code{max_cpu_time}
is the maximum amount of CPU execution time consumed
on any execution of the periodic loop.

@item @code{total_cpu_time}
is the total amount of CPU execution time consumed
by executions of the periodic loop.

@item @code{min_wall_time}
is the minimum amount of wall time that passed
on any execution of the periodic loop.

@item @code{max_wall_time}
is the maximum amount of wall time that passed
on any execution of the periodic loop.

@item @code{total_wall_time}
is the total amount of wall time that passed
during executions of the periodic loop.

@end itemize

The above information is inexpensive to maintain and can provide very
useful insights into the execution characteristics of a periodic
task loop.

@subsection Analysis of the Reported Information

The period statistics reported must be analyzed by the user in terms
of what the applications is.  For example, in an application where
priorities are assigned by the Rate Monotonic Algorithm, it would
be very undesirable for high priority (i.e. frequency) tasks to
miss their period.  Similarly, in nearly any application, if a 
task were supposed to execute its periodic loop every 10 milliseconds
and it averaged 11 milliseconds, then application requirements
are not being met.  

The information reported can be used to determine the "hot spots"
in the application.  Given a period's id, the user can determine
the length of that period.  From that information and the CPU usage,
the user can calculate the percentage of CPU time consumed by that
periodic task.  For example, a task executing for 20 milliseconds
every 200 milliseconds is consuming 10 percent of the processor's
execution time.  This is usually enough to make it a good candidate
for optimization.

However, execution time alone is not enough to gauge the value of
optimizing a particular task.  It is more important to optimize
a task executing 2 millisecond every 10 milliseconds (20 percent
of the CPU) than one executing 10 milliseconds every 100 (10 percent
of the CPU).  As a general rule of thumb, the higher frequency at
which a task executes, the more important it is to optimize that 
task.

@section Operations

@subsection Initializing the Period Statistics

The period statistics manager must be explicitly initialized before
any calls to this manager.  This is done by calling the
@code{Period_usage_Initialize} service.

@subsection Updating Period Statistics

It is the responsibility of each period task loop to update the statistics
on each execution of its loop.  The following is an example of a 
simple periodic task that uses the period statistics manager:

@example
@group
rtems_task Periodic_task()
@{
  rtems_name        name;
  rtems_id          period;
  rtems_status_code status;

  name = rtems_build_name( 'P', 'E', 'R', 'D' );

  (void) rate_monotonic_create( name, &period );

  while ( 1 ) @{
    if ( rate_monotonic_period( period, 100 ) == TIMEOUT )
      break;

    /* Perform some periodic actions */

    /* Report statistics */
    Period_usage_Update( period_id );
  @}

  /* missed period so delete period and SELF */

  (void) rate_monotonic_delete( period );
  (void) task_delete( SELF );
@}
@end group
@end example

@subsection Reporting Period Statistics

The application may dynamically report the period usage for every
period in the system by calling the @code{Period_usage_Dump} routine.
This routine prints a table with the following information per period:

@itemize @bullet
@item period id
@item id of the task that owns the period
@item number of periods executed
@item number of periods missed
@item minimum/maximum/average cpu use per period
@item minimum/maximum/average wall time per period
@end itemize

The following is an example of the report generated:

@example
@group
Period information by period
   ID      OWNER   PERIODS  MISSED    CPU TIME    WALL TIME
0x28010001  TA1       502     0       0/1/ 1.00    0/0/0.00
0x28010002  TA2       502     0       0/1/ 1.00    0/0/0.00
0x28010003  TA3       502     0       0/1/ 1.00    0/0/0.00
0x28010004  TA4       502     0       0/1/ 1.00    0/0/0.00
0x28010005  TA5        10     0       0/1/ 0.90    0/0/0.00
@end group
@end example

@section Routines

This section details the rate monotonic period statistics manager's routines.
A subsection is dedicated to each of this manager's routines
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection Period_usage_Initialize - Initialize the Period Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Period_usage_Initialize( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine allocates the table used to contain the period statistics.
This table is then initialized by calling the @code{Period_usage_Reset}
service.

@subheading NOTES:

This routine invokes the @code{malloc} routine to dynamically allocate
memory. 

@page
@subsection Period_usage_Reset - Reset the Period Statistics

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Period_usage_Reset( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine re-initializes the period statistics table to its
default state which is when zero period executions have occurred.

@subheading NOTES:

NONE

@page
@subsection Period_usage_Update - Update the Statistics for this Period

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Period_usage_Update(
  rtems_id   id
);
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

The @code{Period_usage_Update} routine must be invoked at the "bottom"
of each periodic loop iteration to update the statistics.

@subheading NOTES:

NONE

@page
@subsection Period_usage_Dump - Report Period Statistics Usage

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void Period_usage_Dump( void );
@end example
@end ifset

@ifset is-Ada
@example
An Ada interface is not currently available.
@end example
@end ifset

@subheading STATUS CODES: NONE

@subheading DESCRIPTION:

This routine prints out a table detailing the period statistics for
all periods in the system.

@subheading NOTES:

NONE
