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

@chapter ERC32 Timing Data

@section Introduction

The timing data for RTEMS on the ERC32 implementation
of the SPARC architecture is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
SPARC version of RTEMS.

@section Hardware Platform

All times reported in this chapter were measured
using the SPARC Instruction Simulator (SIS) developed by the
European Space Agency.  SIS simulates the ERC32 -- a custom low
power implementation combining the Cypress 90C601 integer unit,
the Cypress 90C602 floating point unit, and a number of
peripherals such as counter timers, interrupt controller and a
memory controller.

For the RTEMS tests, SIS is configured with the
following characteristics:

@itemize @bullet
@item 15 Mhz clock speed

@item 0 wait states for PROM accesses

@item 0 wait states for RAM accesses
@end itemize

The ERC32's General Purpose Timer was used to gather
all timing information.  This timer was programmed to operate
with one microsecond accuracy.  All sources of hardware
interrupts were disabled, although traps were enabled and the
interrupt level of the SPARC allows all interrupts.

@section Interrupt Latency

The maximum period with traps disabled or the
processor interrupt level set to it's highest value inside RTEMS
is less than RTEMS_MAXIMUM_DISABLE_PERIOD
microseconds including the instructions which
disable and re-enable interrupts.  The time required for the
ERC32 to vector an interrupt and for the RTEMS entry overhead
before invoking the user's trap handler are a total of 
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
microseconds.  These combine to yield a worst case interrupt
latency of less than RTEMS_MAXIMUM_DISABLE_PERIOD +
RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK microseconds at 
RTEMS_MAXIMUM_DISABLE_PERIOD_MHZ Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

The maximum period with interrupts disabled within
RTEMS is hand-timed with some assistance from SIS.  The maximum
period with interrupts disabled with RTEMS occurs during a
context switch when traps are disabled to flush all the register
windows to memory.  The length of time spent flushing the
register windows varies based on the number of windows which
must be flushed.  Based on the information reported by SIS, it
takes from 4.0 to 18.0 microseconds (37 to 122 instructions) to
flush the register windows.  It takes approximately 41 CPU
cycles (2.73 microseconds) to flush each register window set to
memory.  The register window flush operation is heavily memory
bound.

[NOTE: All traps are disabled during the register
window flush thus disabling both software generate traps and
external interrupts.  During a normal RTEMS critical section,
the processor interrupt level (pil) is raised to level 15 and
traps are left enabled.  The longest path for a normal critical
section within RTEMS is less than 50 instructions.]

The interrupt vector and entry overhead time was
generated on the SIS benchmark platform using the ERC32's
ability to forcibly generate an arbitrary interrupt as the
source of the "benchmark" interrupt.

@section Context Switch

The RTEMS processor context switch time is 10
microseconds on the SIS benchmark platform when no floating
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
times for the ERC32 benchmark platform:

