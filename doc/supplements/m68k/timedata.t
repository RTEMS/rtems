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

@ifinfo
@node MVME136 Timing Data, MVME136 Timing Data Introduction, Timing Specification Terminology, Top
@end ifinfo
@chapter MVME136 Timing Data
@ifinfo
@menu
* MVME136 Timing Data Introduction::
* MVME136 Timing Data Hardware Platform::
* MVME136 Timing Data Interrupt Latency::
* MVME136 Timing Data Context Switch::
* MVME136 Timing Data Directive Times::
* MVME136 Timing Data Task Manager::
* MVME136 Timing Data Interrupt Manager::
* MVME136 Timing Data Clock Manager::
* MVME136 Timing Data Timer Manager::
* MVME136 Timing Data Semaphore Manager::
* MVME136 Timing Data Message Manager::
* MVME136 Timing Data Event Manager::
* MVME136 Timing Data Signal Manager::
* MVME136 Timing Data Partition Manager::
* MVME136 Timing Data Region Manager::
* MVME136 Timing Data Dual-Ported Memory Manager::
* MVME136 Timing Data I/O Manager::
* MVME136 Timing Data Rate Monotonic Manager::
@end menu
@end ifinfo

@ifinfo
@node MVME136 Timing Data Introduction, MVME136 Timing Data Hardware Platform, MVME136 Timing Data, MVME136 Timing Data
@end ifinfo
@section Introduction

The timing data for the MC68020 version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the interrupt latency and the context switch
times as they pertain to the MC68020 version of RTEMS.

@ifinfo
@node MVME136 Timing Data Hardware Platform, MVME136 Timing Data Interrupt Latency, MVME136 Timing Data Introduction, MVME136 Timing Data
@end ifinfo
@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Motorola
MVME135 CPU board.  The MVME135 is a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ
Mhz board with one wait
state dynamic memory and a MC68881 numeric coprocessor.  The
Zilog 8036 countdown timer on this board was used to measure
elapsed time with a one-half microsecond resolution.  All
sources of hardware interrupts were disabled, although the
interrupt level of the MC68020 allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  The worst case times of the MC68020 microprocessor
were used for each instruction.  Zero wait state memory was
assumed.  The total CPU cycles executed with interrupts
disabled, including the instructions to disable and enable
interrupts, was divided by 20 to simulate a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ
Mhz MC68020.  It
should be noted that the worst case instruction times for the
MC68020 assume that the internal cache is disabled and that no
instructions overlap.

@ifinfo
@node MVME136 Timing Data Interrupt Latency, MVME136 Timing Data Context Switch, MVME136 Timing Data Hardware Platform, MVME136 Timing Data
@end ifinfo
@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than RTEMS_MAXIMUM_DISABLE_PERIOD 
microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the MC68020 to vector an interrupt and for the RTEMS entry
overhead before invoking the user's interrupt handler are a
total of RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK 
microseconds.  These combine to yield a worst case
interrupt latency of less than 
RTEMS_MAXIMUM_DISABLE_PERIOD + RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK 
microseconds at RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ
Mhz.  [NOTE:  The maximum period with interrupts
disabled was last determined for Release 
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed and based upon
worst case (i.e. CPU cache disabled and no instruction overlap)
times for a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ
Mhz MC68020.  The interrupt vector and entry
overhead time was generated on an MVME135 benchmark platform
using the Multiprocessing Communications registers to generate
as the interrupt source.

@ifinfo
@node MVME136 Timing Data Context Switch, MVME136 Timing Data Directive Times, MVME136 Timing Data Interrupt Latency, MVME136 Timing Data
@end ifinfo
@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the MVME135 benchmark platform when no floating
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
floating point context is dependent on whether an MC68881 or
MC68882 is being used as well as the state of the numeric
coprocessor.  These numeric coprocessors define three operating
states: initialized, idle, and busy.  RTEMS places the
coprocessor in the initialized state when a task is started or
restarted.  Once the task has utilized the coprocessor, it is in
the idle state when floating point instructions are not
executing and the busy state when floating point instructions
are executing.  The state of the coprocessor is task specific.

The following table summarizes the context switch
times for the MVME135 benchmark platform:

@include timetbl.texi

@tex
\global\advance \smallskipamount by 4pt
@end tex
