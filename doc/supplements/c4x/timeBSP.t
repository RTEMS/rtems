@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@include ../../common/timemac.texi
@tex
\global\advance \smallskipamount by -4pt
@end tex

@chapter BSP_FOR_TIMES Timing Data

@section Introduction

The timing data for the XXX version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the interrupt latency and the context switch
times as they pertain to the XXX version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Motorola
BSP_FOR_TIMES CPU board.  The BSP_FOR_TIMES is a 20Mhz board with one wait
state dynamic memory and a XXX numeric coprocessor.  The
Zilog 8036 countdown timer on this board was used to measure
elapsed time with a one-half microsecond resolution.  All
sources of hardware interrupts were disabled, although the
interrupt level of the XXX allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  The worst case times of the XXX microprocessor
were used for each instruction.  Zero wait state memory was
assumed.  The total CPU cycles executed with interrupts
disabled, including the instructions to disable and enable
interrupts, was divided by 20 to simulate a 20Mhz XXX.  It
should be noted that the worst case instruction times for the
XXX assume that the internal cache is disabled and that no
instructions overlap.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than RTEMS_MAXIMUM_DISABLE_PERIOD 
microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the XXX to vector an interrupt and for the RTEMS entry
overhead before invoking the user's interrupt handler are a
total of RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK 
microseconds.  These combine to yield a worst case
interrupt latency of less than 
RTEMS_MAXIMUM_DISABLE_PERIOD + RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK 
microseconds at 20Mhz.  [NOTE:  The maximum period with interrupts
disabled was last determined for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed and based upon
worst case (i.e. CPU cache disabled and no instruction overlap)
times for a 20Mhz XXX.  The interrupt vector and entry
overhead time was generated on an BSP_FOR_TIMES benchmark platform
using the Multiprocessing Communications registers to generate
as the interrupt source.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the BSP_FOR_TIMES benchmark platform when no floating
point context is saved or restored.  Additional execution time
is required when a TASK_SWITCH user extension is configured.
The use of the TASK_SWITCH extension is application dependent.
Thus, its execution time is not considered part of the raw
context switch time.

Since RTEMS was designed specifically for embedded
missile applications which are floating point intensive, the
executive is optimized to avoid unnecessarily saving and
restoring the state of the numeric coprocessor.  The state of
the numeric coprocessor is only saved when an FLOATING_POINT
task is dispatched and that task was not the last task to
utilize the coprocessor.  In a system with only one
FLOATING_POINT task, the state of the numeric coprocessor will
never be saved or restored.  When the first FLOATING_POINT task
is dispatched, RTEMS does not need to save the current state of
the numeric coprocessor.

The exact amount of time required to save and restore
floating point context is dependent on whether an XXX or
XXX is being used as well as the state of the numeric
coprocessor.  These numeric coprocessors define three operating
states: initialized, idle, and busy.  RTEMS places the
coprocessor in the initialized state when a task is started or
restarted.  Once the task has utilized the coprocessor, it is in
the idle state when floating point instructions are not
executing and the busy state when floating point instructions
are executing.  The state of the coprocessor is task specific.

The following table summarizes the context switch
times for the BSP_FOR_TIMES benchmark platform:

