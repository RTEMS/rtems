@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Timer Driver

The timer driver is primarily used by the RTEMS Timing Tests.  
This driver provides as accurate a benchmark timer as possible.
It typically reports its time in microseconds, CPU cycles, or
bus cycles.  This information can be very useful for determining
precisely what pieces of code require optimization and to measure the
impact of specific minor changes.

The gen68340 BSP also uses the Timer Driver to support a high performance
mode of the on-CPU UART.

@section Benchmark Timer

The RTEMS Timing Test Suite requires a benchmark timer.  The
RTEMS Timing Test Suite is very helpful for determining
the performance of target hardware and comparing its performance
to that of other RTEMS targets.

This section describes the routines which are assumed to exist by
the RTEMS Timing Test Suite.  The names used are @b{EXACTLY} what
is used in the RTEMS Timing Test Suite so follow the naming convention.

@subsection benchmark_timer_initialize

Initialize the timer source.

@example
void benchmark_timer_initialize(void)
@{
  initialize the benchmark timer
@}
@end example

@subsection Read_timer

The @code{benchmark_timer_read} routine returns the number of benchmark
time units (typically microseconds) that have elapsed since the last
call to @code{benchmark_timer_initialize}.

@example
int benchmark_timer_read(void)
@{
  stop time = read the hardware timer
  if the subtract overhead feature is enabled
    subtract overhead from stop time 
  return the stop time
@}
@end example

Many implementations of this routine subtract the overhead required
to initialize and read the benchmark timer.  This makes the times reported
more accurate.

Some implementations report 0 if the harware timer value change is 
sufficiently small.  This is intended to indicate that the execution time 
is below the resolution of the timer.

@subsection benchmark_timer_disable_subtracting_average_overhead

This routine is invoked by the "Check Timer" (@code{tmck}) test in the 
RTEMS Timing Test Suite.  It makes the @code{benchmark_timer_read}
routine NOT subtract the overhead required
to initialize and read the benchmark timer.  This is used
by the @code{tmoverhd} test to determine the overhead
required to initialize and read the timer.

@example
void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
@{
    disable the subtract overhead feature
@}
@end example

The @code{benchmark_timer_find_average_overhead} variable is used to
indicate the state of the "subtract overhead feature".

@section gen68340 UART FIFO Full Mode

The gen68340 BSP is an example of the use of the timer to support the UART
input FIFO full mode (FIFO means First In First Out and roughly means
buffer). This mode consists in the UART raising an interrupt when n
characters have been received (@i{n} is the UART's FIFO length). It results
in a lower interrupt processing time, but the problem is that a scanf
primitive will block on a receipt of less than @i{n} characters. The solution
is to set a timer that will check whether there are some characters
waiting in the UART's input FIFO. The delay time has to be set carefully
otherwise high rates will be broken: 

@itemize @bullet

@item if no character was received last time the interrupt subroutine was
entered, set a long delay,

@item otherwise set the delay to the delay needed for @i{n} characters
receipt. 

@end itemize
