@c
@c  COPYRIGHT (c) 1988-1997.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@include ../common/timemac.texi
@tex
\global\advance \smallskipamount by -4pt
@end tex

@ifinfo
@node PPC603e Timing Data, PPC603e Timing Data Introduction, Memory Requirements RTEMS RAM Workspace Worksheet, Top
@end ifinfo
@chapter PPC603e Timing Data
@ifinfo
@menu
* PPC603e Timing Data Introduction::
* PPC603e Timing Data Hardware Platform::
* PPC603e Timing Data Interrupt Latency::
* PPC603e Timing Data Context Switch::
* PPC603e Timing Data Directive Times::
* PPC603e Timing Data Task Manager::
* PPC603e Timing Data Interrupt Manager::
* PPC603e Timing Data Clock Manager::
* PPC603e Timing Data Timer Manager::
* PPC603e Timing Data Semaphore Manager::
* PPC603e Timing Data Message Manager::
* PPC603e Timing Data Event Manager::
* PPC603e Timing Data Signal Manager::
* PPC603e Timing Data Partition Manager::
* PPC603e Timing Data Region Manager::
* PPC603e Timing Data Dual-Ported Memory Manager::
* PPC603e Timing Data I/O Manager::
* PPC603e Timing Data Rate Monotonic Manager::
@end menu
@end ifinfo

@ifinfo
@node PPC603e Timing Data Introduction, PPC603e Timing Data Hardware Platform, PPC603e Timing Data, PPC603e Timing Data
@end ifinfo
@section Introduction

The timing data for RTEMS on the PPC603e implementation
of the PowerPC architecture is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
PowerPC version of RTEMS.

@ifinfo
@node PPC603e Timing Data Hardware Platform, PPC603e Timing Data Interrupt Latency, PPC603e Timing Data Introduction, PPC603e Timing Data
@end ifinfo
@section Hardware Platform

All times reported in this chapter were measured using the PowerPC
Instruction Simulator (PSIM). PSIM simulates a variety of PowerPC
6xx models with the PPC603e being used as the basis for the measurements
reported in this chapter.

The PowerPC decrementer register was was used to gather
all timing information.  In real hardware implementations
of the PowerPC architecture, this register would typically
count something like CPU cycles or be a function of the clock
speed.  However, wth PSIM each count of the decrementer register 
represents an instruction.  Thus all measurements in this 
chapter are reported as the actual number of instructions
executed.  All sources of hardware interrupts were disabled,
although traps were enabled and the interrupt level of the
PowerPC allows all interrupts.

@ifinfo
@node PPC603e Timing Data Interrupt Latency, PPC603e Timing Data Context Switch, PPC603e Timing Data Hardware Platform, PPC603e Timing Data
@end ifinfo
@section Interrupt Latency

The maximum period with traps disabled or the
processor interrupt level set to it's highest value inside RTEMS
is less than RTEMS_MAXIMUM_DISABLE_PERIOD
microseconds including the instructions which
disable and re-enable interrupts.  The time required for the
PowerPC to vector an interrupt and for the RTEMS entry overhead
before invoking the user's trap handler are a total of 
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
microseconds.  These combine to yield a worst case interrupt
latency of less than RTEMS_MAXIMUM_DISABLE_PERIOD +
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK microseconds at 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

The maximum period with interrupts disabled within
RTEMS is hand-timed with some assistance from PSIM.  The maximum
period with interrupts disabled with RTEMS occurs .... XXX

The interrupt vector and entry overhead time was
generated on the PSIM benchmark platform using the PowerPC's
decrementer register.  This register was programmed to generate
an interrupt after one countdown.

@ifinfo
@node PPC603e Timing Data Context Switch, PPC603e Timing Data Directive Times, PPC603e Timing Data Interrupt Latency, PPC603e Timing Data
@end ifinfo
@section Context Switch

The RTEMS processor context switch time is XXX
microseconds on the PSIM benchmark platform when no floating
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

The following table summarizes the context switch
times for the PSIM benchmark platform:

@include timetbl.texi

@tex
\global\advance \smallskipamount by 4pt
@end tex


