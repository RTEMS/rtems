@c
@c  Timing information for the DMV177
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

@chapter RTEMS_BSP Timing Data

@section Introduction

The timing data for RTEMS on the DY-4 RTEMS_BSP board
is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
PowerPC version of RTEMS.

@section Hardware Platform

All times reported in this chapter were measured using a RTEMS_BSP board.
All data and code caching was disabled.  This results in very deterministic
times which represent the worst possible performance.  Many embedded 
applications disable caching to insure that execution times are
repeatable.  Moreover, the JTAG port on certain revisions of the PowerPC
603e does not operate properly if caching is enabled.  Thus during 
development and debug, caching must be off.

The PowerPC decrementer register was was used to gather
all timing information.  In the PowerPC architecture,
this register typically counts
something like CPU cycles or is a function of the clock
speed.  On the PPC603e decrements once for every four (4) bus cycles.
On the RTEMS_BSP, the bus operates at a clock speed of 
33 Mhz.  This result in a very accurate number since it is a function of the 
microprocessor itself.  Thus all measurements in this 
chapter are reported as the actual number of decrementer
clicks reported.  

To convert the numbers reported to microseconds, one should
divide the number reported by 8.650752.  This number was derived as
shown below:

@example
((33 * 1048576) / 1000000) / 4 = 8.650752
@end example

All sources of hardware interrupts were disabled,
although traps were enabled and the interrupt level of the
PowerPC allows all interrupts.

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
RTEMS is hand-timed with some assistance from the PowerPC simulator.
The maximum period with interrupts disabled with RTEMS has not
been calculated on this target.

The interrupt vector and entry overhead time was
generated on the PSIM benchmark platform using the PowerPC's
decrementer register.  This register was programmed to generate
an interrupt after one countdown.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
bus cycle on the RTEMS_BSP benchmark platform when no floating
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
times for the RTEMS_BSP benchmark platform:

