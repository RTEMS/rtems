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

@chapter MYBSP Timing Data

@section Introduction

The timing data for the ARM version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the interrupt latency and the context switch
times as they pertain to the ARM version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Motorola
MYBSP CPU board.  The MYBSP is a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ
Mhz board with SDRAM and no numeric coprocessor.  A
countdown timer on this board was used to measure
elapsed time with a 20 nanosecond resolution.  All
sources of hardware interrupts were disabled, although the
interrupt level of the ARM microprocessor allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  The worst case times of the ARM9DTMI microprocessor
were used for each instruction.  Zero wait state memory was
assumed.  The total CPU cycles executed with interrupts
disabled, including the instructions to disable and enable
interrupts, was divided by TBD to simulate a TBD Mhz processor.  It
should be noted that the worst case instruction times 
assume that the internal cache is disabled and that no
instructions overlap.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than RTEMS_MAXIMUM_DISABLE_PERIOD 
microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the processor to vector an interrupt and for the RTEMS entry
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
times for a RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz processor.  
The interrupt vector and entry
overhead time was generated on an MYBSP benchmark platform
using the Multiprocessing Communications registers to generate
as the interrupt source.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the MYBSP benchmark platform when no floating
point context is saved or restored.  Additional execution time
is required when a TASK_SWITCH user extension is configured.
The use of the TASK_SWITCH extension is application dependent.
Thus, its execution time is not considered part of the raw
context switch time.

The ARM processor benchmarked does not have a floating point
unit and consequently no FPU results are reported.

@c Since RTEMS was designed specifically for embedded
@c missile applications which are floating point intensive, the
@c executive is optimized to avoid unnecessarily saving and
@c restoring the state of the numeric coprocessor.  The state of
@c the numeric coprocessor is only saved when an FLOATING_POINT
@c task is dispatched and that task was not the last task to
@c utilize the coprocessor.  In a system with only one
@c FLOATING_POINT task, the state of the numeric coprocessor will
@c never be saved or restored.  When the first FLOATING_POINT task
@c is dispatched, RTEMS does not need to save the current state of
@c the numeric coprocessor.

@c The exact amount of time required to save and restore
@c floating point context is dependent on whether an XXX or
@c XXX is being used as well as the state of the numeric
@c coprocessor.  These numeric coprocessors define three operating
@c states: initialized, idle, and busy.  RTEMS places the
@c coprocessor in the initialized state when a task is started or
@c restarted.  Once the task has utilized the coprocessor, it is in
@c the idle state when floating point instructions are not
@c executing and the busy state when floating point instructions
@c are executing.  The state of the coprocessor is task specific.

The following table summarizes the context switch
times for the MYBSP benchmark platform:

