@c
@c  COPYRIGHT (c) 1988-1998.
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
@node i960CA Timing Data, i960CA Timing Data Introduction, Memory Requirements RTEMS RAM Workspace Worksheet, Top
@end ifinfo
@chapter Timing Data
@ifinfo
@menu
* i960CA Timing Data Introduction::
* i960CA Timing Data Hardware Platform::
* i960CA Timing Data Interrupt Latency::
* i960CA Timing Data Context Switch::
* i960CA Timing Data Directive Times::
* i960CA Timing Data Task Manager::
* i960CA Timing Data Interrupt Manager::
* i960CA Timing Data Clock Manager::
* i960CA Timing Data Timer Manager::
* i960CA Timing Data Semaphore Manager::
* i960CA Timing Data Message Manager::
* i960CA Timing Data Event Manager::
* i960CA Timing Data Signal Manager::
* i960CA Timing Data Partition Manager::
* i960CA Timing Data Region Manager::
* i960CA Timing Data Dual-Ported Memory Manager::
* i960CA Timing Data I/O Manager::
* i960CA Timing Data Rate Monotonic Manager::
@end menu
@end ifinfo

NOTE: The i960CA board used by the RTEMS Project to
obtain i960CA times is currently broken.  The information in
this chapter was obtained using Release 3.2.1.

@ifinfo
@node i960CA Timing Data Introduction, i960CA Timing Data Hardware Platform, i960CA Timing Data, i960CA Timing Data
@end ifinfo
@section Introduction

The timing data for the i960CA version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the i960CA version of RTEMS.

@ifinfo
@node i960CA Timing Data Hardware Platform, i960CA Timing Data Interrupt Latency, i960CA Timing Data Introduction, i960CA Timing Data
@end ifinfo
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

@ifinfo
@node i960CA Timing Data Interrupt Latency, i960CA Timing Data Context Switch, i960CA Timing Data Hardware Platform, i960CA Timing Data
@end ifinfo
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

@ifinfo
@node i960CA Timing Data Context Switch, i960CA Timing Data Directive Times, i960CA Timing Data Interrupt Latency, i960CA Timing Data
@end ifinfo
@section Context Switch

The RTEMS processor context switch time is RTEMS_NO_FP_CONTEXTS
microseconds on the Cyclone CVME961 benchmark platform.  This
time represents the raw context switch time with no user
extensions configured.  Additional execution time is required
when a TSWITCH user extension is configured.  The use of the
TSWITCH extension is application dependent.  Thus, its execution
time is not considered part of the base context switch time.

The i960CA has no hardware floating point capability
and floating point tasks are not supported.

The following table summarizes the context switch
times for the CVME961 benchmark platform:

@include timetbl.texi
 
@tex
\global\advance \smallskipamount by 4pt
@end tex

