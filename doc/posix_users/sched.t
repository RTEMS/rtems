@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Scheduler Manager

@section Introduction

The scheduler manager ...

The directives provided by the scheduler manager are:

@itemize @bullet
@item @code{sched_get_priority_min} - Get Minimum Priority Value
@item @code{sched_get_priority_max} - Get Maximum Priority Value
@item @code{sched_rr_get_interval} - Get Timeslicing Quantum
@item @code{sched_yield} - Yield the Processor
@end itemize

@section Background

@subsection Priority

In the RTEMS implementation of the POSIX API, the priorities range from
the low priority of @code{sched_get_priority_min()} to the highest priority of
@code{sched_get_priority_max()}. Numerically higher values represent higher
priorities.

@subsection Scheduling Policies

The following scheduling policies are available:

@table @b
@item SCHED_FIFO
Priority-based, preemptive scheduling with no timeslicing. This is equivalent
to what is called "manual round-robin" scheduling.

@item SCHED_RR
Priority-based, preemptive scheduling with timeslicing. Time quantums are
maintained on a per-thread basis and are not reset at each context switch.
Thus, a thread which is preempted and subsequently resumes execution will
attempt to complete the unused portion of its time quantum.

@item SCHED_OTHER
Priority-based, preemptive scheduling with timeslicing. Time quantums are
maintained on a per-thread basis and are reset at each context switch.

@item SCHED_SPORADIC
Priority-based, preemptive scheduling utilizing three additional parameters:
budget, replenishment period, and low priority. Under this policy, the
thread is allowed to execute for "budget" amount of time before its priority
is lowered to "low priority". At the end of each replenishment period,
the thread resumes its initial priority and has its budget replenished.

@end table

@section Operations

There is currently no text in this section.

@section Directives

This section details the scheduler manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection sched_get_priority_min - Get Minimum Priority Value

@findex sched_get_priority_min
@cindex  get minimum priority value

@subheading CALLING SEQUENCE:

@example
#include <sched.h>

int sched_get_priority_min(
  int policy
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINVAL
The indicated policy is invalid.

@end table

@subheading DESCRIPTION:

This routine return the minimum (numerically and logically lowest) priority
for the specified @code{policy}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection sched_get_priority_max - Get Maximum Priority Value

@findex sched_get_priority_max
@cindex  get maximum priority value

@subheading CALLING SEQUENCE:

@example
#include <sched.h>

int sched_get_priority_max(
  int policy
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item EINVAL
The indicated policy is invalid.

@end table

@subheading DESCRIPTION:

This routine return the maximum (numerically and logically highest) priority
for the specified @code{policy}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection sched_rr_get_interval - Get Timeslicing Quantum

@findex sched_rr_get_interval
@cindex  get timeslicing quantum

@subheading CALLING SEQUENCE:

@example
#include <sched.h>

int sched_rr_get_interval(
  pid_t            pid,
  struct timespec *interval
);
@end example

@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:

@table @b
@item ESRCH
The indicated process id is invalid.

@item EINVAL
The specified interval pointer parameter is invalid.

@end table

@subheading DESCRIPTION:

This routine returns the length of the timeslice quantum in the
@code{interval} parameter for the specified @code{pid}.

@subheading NOTES:

The @code{pid} argument should be 0 to indicate the calling process.

@c
@c
@c
@page
@subsection sched_yield - Yield the Processor

@findex sched_yield
@cindex  yield the processor

@subheading CALLING SEQUENCE:

@example
#include <sched.h>

int sched_yield( void );
@end example

@subheading STATUS CODES:

This routine always returns zero to indicate success.

@subheading DESCRIPTION:

This call forces the calling thread to yield the processor to another
thread. Normally this is used to implement voluntary round-robin
task scheduling.

@subheading NOTES:

NONE
