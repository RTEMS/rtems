@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter HP-7100 Timing Data

@section Introduction

The timing data for the PA-RISC version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the PA-RISC version of RTEMS.

@section Hardware Platform

No directive execution times are reported for the
HP-7100 because the target platform was proprietary and
executions times could not be released.

@section Interrupt Latency

The maximum period with traps disabled or the
processor interrupt level set to it's highest value inside RTEMS
is less than RTEMS_MAXIMUM_DISABLE_PERIOD
microseconds including the instructions which
disable and re-enable interrupts.  The time required for the
HP-7100 to vector an interrupt and for the RTEMS entry overhead
before invoking the user's trap handler are a total of
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
microseconds.  These combine to yield a worst case interrupt
latency of less than RTEMS_MAXIMUM_DISABLE_PERIOD +
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK microseconds at 15 Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS for the HP-7100 is hand calculated.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microsections for the HP-7100 when no floating point context
switch is saved or restored.  Saving and restoring the floating
point context adds additional time to the context
switch procedure.  Additional execution time is required when a
TASK_SWITCH user extension is configured.  The use of the
TASK_SWITCH extension is application dependent.  Thus, its
execution time is not considered part of the raw context switch
time.

Since RTEMS was designed specifically for embedded
missile applications which are floating point intensive, the
executive is optimized to avoid unnecessarily saving and
restoring the state of the numeric coprocessor.  On many
processors, the state of the numeric coprocessor is only saved
when an FLOATING_POINT task is dispatched and that task was not
the last task to utilize the coprocessor.  In a system with only
one FLOATING_POINT task, the state of the numeric coprocessor
will never be saved or restored.  When the first FLOATING_POINT
task is dispatched, RTEMS does not need to save the current
state of the numeric coprocessor.  As discussed in the Register
Usage section, on the HP-7100 the every task is considered to be
floating point registers and , as a rsule, every context switch
involves saving and restoring the state of the floating point
unit.

The following table summarizes the context switch
times for the HP-7100 processor:

@example
no times are available for the HP-7100
@end example

@section Directive Times

No execution times are available for the HP-7100
because the target platform was proprietary and no timing
information could be released.

