@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@include common/timemac.texi
@tex
\global\advance \smallskipamount by -4pt
@end tex
 
@chapter CPU386 Timing Data

@section Introduction

The timing data for the i386 version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the i386 version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Force
Computers CPU386 board.  The CPU386 is a 16 Mhz board with zero
wait state dynamic memory and an i80387 numeric coprocessor.
One of the count-down timers provided by a Motorola MC68901 was
used to measure elapsed time with one microsecond resolution.
All sources of hardware interrupts are disabled, although the
interrupt level of the i386 allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  Zero wait state memory was assumed.  The total CPU
cycles executed with interrupts disabled, including the
instructions to disable and enable interrupts, was divided by 16
to simulate a i386 executing at 16 Mhz.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than RTEMS_MAXIMUM_DISABLE_PERIOD microseconds 
including the instructions
which disable and re-enable interrupts.  The time required for
the i386 to generate an interrupt using the int instruction,
vectoring to an interrupt handler, and for the RTEMS entry
overhead before invoking the user's interrupt handler are a
total of 12 microseconds.  These combine to yield a worst case
interrupt latency of less 
RTEMS_MAXIMUM_DISABLE_PERIOD + RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK 
microseconds.  [NOTE:  The
maximum period with interrupts disabled within RTEMS was last
calculated for Release RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed.  The interrupt
vector and entry overhead time was generated on the Force
Computers CPU386 benchmark platform using the int instruction as
the interrupt source.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the Force Computers CPU386 benchmark platform.
This time represents the raw context switch time with no user
extensions configured.  Additional execution time is required
when a TASK_SWITCH user extension is configured.  The use of the
TASK_SWITCH extension is application dependent.  Thus, its
execution time is not considered part of the base context switch
time.

Since RTEMS was designed specifically for embedded
missile applications which are floating point intensive, the
executive is optimized to avoid unnecessarily saving and
restoring the state of the numeric coprocessor.  The state of
the numeric coprocessor is only saved when a FLOATING_POINT task
is dispatched and that task was not the last task to utilize the
coprocessor.  In a system with only one FLOATING_POINT task, the
state of the numeric coprocessor will never be saved or
restored.  When the first FLOATING_POINT task is dispatched,
RTEMS does not need to save the current state of the numeric
coprocessor.

The exact amount of time required to save and restore
floating point context is dependent on the state of the numeric
coprocessor.  RTEMS places the coprocessor in the initialized
state when a task is started or restarted.  Once the task has
utilized the coprocessor, it is in the idle state when floating
point instructions are not executing and the busy state when
floating point instructions are executing.  The state of the
coprocessor is task specific.

The following table summarizes the context switch
times for the Force Computers CPU386 benchmark platform:

