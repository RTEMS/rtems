@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Timer Driver

You can program the timer driver for your own needs, but here are two uses
of it: 

@subsection = UART'S FIFO Full Mode

The gen68340 BSP is an example of the use of the timer to support the UART
input FIFO full mode (FIFO means First In First Out and roughly means
buffer). This mode consists in the UART raising an interrupt when n
characters have been received (n is the UA RT's FIFO length). It results
in a lower interrupt processing time, but the problem is that a scanf
primitive will block on a receipt of less than n characters. The solution
is to set a timer that will check whether there are some characters
waiting in th e UART's input FIFO. The delay time has to be set carefully
otherwise high rates will be broken: 

@itemize @bullet

@item if no character was received last time the interrupt subroutine was
entered, set a long delay,

@item otherwise set the delay to the delay needed for n characters
receipt. 

@end itemize

@subsection = Measuring RTEMS Primitives Time

RTEMS Timing Test Suite needs a timer support. You have to provide two
primitives: 



Function

Description

@example
void Timer_initialize (void)
@end example

Initialize the timer to be a counter to the microsecond.

@example
int Read_timer (void)
@end example

Returns the number of microseconds elapsed since the last call to
Timer_initialize. 

@example
rtems_status_code Empty_function (void)

return RTEMS_SUCCESSFUL;
@end example

@example
void Set_find_average_overhead (rtems_boolean find_flag)
@end example

DOES NOTHING ????????????????????????????????????????????????????????????????????????????????????????????????????

		


