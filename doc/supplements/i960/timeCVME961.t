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

@chapter CVME961 Timing Data

NOTE: The CVME961 board used by the RTEMS Project to
obtain i960CA times is currently broken.  The information in
this chapter was obtained using Release 3.2.1.

@section Introduction

The timing data for the i960CA version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the i960CA version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Cyclone
Microsystems CVME961 board.  The CVME961 is a 33 Mhz board with
dynamic RAM which has two wait state dynamic memory (four CPU
cycles) for read accesses and one wait state (two CPU cycles)
for write accesses.  The Z8536 on a SQUALL SQSIO4 mezzanine
board was used to measure elapsed time with one-half microsecond
resolution.  All sources of hardware interrupts are disabled,
although the interrupt level of the i960CA allows all interrupts.

The maximum  interrupt disable period was measured by
summing the number of CPU cycles required by each assembly
language instruction executed while interrupts were disabled.
Zero wait state memory was assumed.  The total CPU cycles
executed with interrupts disabled, including the instructions to
disable and enable interrupts, was divided by 33 to simulate a
i960CA executing at 33 Mhz with zero wait states.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than
RTEMS_MAXIMUM_DISABLE_PERIOD microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the i960CA to generate an interrupt using the sysctl
instruction, vectoring to an interrupt handler, and for the
RTEMS entry overhead before invoking the user's interrupt
handler are a total of RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
microseconds.  These combine to yield
a worst case interrupt latency of less than
RTEMS_MAXIMUM_DISABLE_PERIOD + RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
microseconds.  [NOTE: The maximum period with interrupts
disabled within RTEMS was last calculated for Release
RTEMS_RELEASE_FOR_MAXIMUM_DISABLE_PERIOD.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed.  The interrupt
vector and entry overhead time was generated on the Cyclone
CVME961 benchmark platform using the sysctl instruction as the
interrupt source.

@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the Cyclone CVME961 benchmark platform.  This
time represents the raw context switch time with no user
extensions configured.  Additional execution time is required
when a TSWITCH user extension is configured.  The use of the
TSWITCH extension is application dependent.  Thus, its execution
time is not considered part of the base context switch time.

The CVME961 has no hardware floating point capability
and floating point tasks are not supported.

The following table summarizes the context switch
times for the CVME961 benchmark platform:

