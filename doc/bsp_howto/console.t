@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Console Driver

@section Introduction

This chapter describes the operation of a console driver using 
the RTEMS POSIX Termios support.  Traditionally RTEMS has referred
to all serial device drivers as console device drivers.  A
console driver can be used to do raw data processing in addition
to the "normal" standard input and output device functions required
of a console.

The serial driver may be called as the consequence of a C Library
call such as @code{printf} or @code{scanf} or directly via the
@code{read} or @code{write} system calls. 
There are two main functioning modes: 

@itemize @bullet

@item console: formatted input/output, with special characters (end of
line, tabulations, etc.) recognition and processing,

@item raw: permits raw data processing. 

@end itemize

One may think that two serial drivers are needed to handle these two types
of data, but Termios permits having only one driver. 

@section Termios

Termios is a standard for terminal management, included in the POSIX 1003.1b
standard.  It is commonly provided on UNIX implementations.  
Having RTEMS support for Termios is beneficial:

@itemize @bullet

@item from the user's side because it provides standard primitive operations
to access the terminal and change configuration settings.  These operations
are the same under Unix and Rtems. 

@item from the BSP developer's side because it frees the
developer from dealing with buffer states and mutual exclusions on them. 
Early RTEMS console device drivers also did their own special
character processing.

@end itemize

Termios support includes: 

@itemize @bullet

@item raw and console handling,

@item blocking or non-blocking characters receive, with or without
Timeout. 

@end itemize

At this time, RTEMS documentation does not include a thorough discussion
of the Termios functionality.  For more information on Termios,
type @code{man termios} on a Unix box or point a web browser
at
@uref{http://www.freebsd.org/cgi/man.cgi}.

@section Driver Functioning Modes

There are generally two main functioning modes for an UART (Universal
Asynchronous Receiver-Transmitter, i.e. the serial chip): 

@itemize @bullet

@item polled mode
@item interrupt driven mode

@end itemize

In polled mode, the processor blocks on sending/receiving characters.
This mode is not the most efficient way to utilize the UART. But 
polled mode is usually necessary when one wants to print an
error message in the event of a fatal error such as a fatal error
in the BSP.  This is also the simplest mode to
program.  Polled mode is generally preferred if the serial port is
to be used primarily as a debug console.  In a simple polled driver,
the software will continuously check the status of the UART when
it is reading or writing to the UART.  Termios improves on this
by delaying the caller for 1 clock tick between successive checks
of the UART on a read operation.

In interrupt driven mode, the processor does not block on sending/receiving
characters.  Data is buffered between the interrupt service routine
and application code.  Two buffers are used to insulate the application
from the relative slowness of the serial device.  One of the buffers is
used for incoming characters, while the other is used for outgoing characters.

An interrupt is raised when a character is received by the UART.
The interrupt subroutine places the incoming character at the end
of the input buffer.  When an application asks for input,
the characters at the front of the buffer are returned.

When the application prints to the serial device, the outgoing characters
are placed at the end of the output buffer.  The driver will place
one or more characters in the UART (the exact number depends on the UART)
An interrupt will be raised when all the characters have been transmitted.
The interrupt service routine has to send the characters
remaining in the output buffer the same way.   When the transmitting side
of the UART is idle, it is typically necessary to prime the transmitter 
before the first interrupt will occur.

@section Serial Driver Functioning Overview

The following Figure shows how a Termios driven serial driver works: 

@example
Figure not included in this draft
@end example

The following list describes the basic flow.

@itemize @bullet

@item the application programmer uses standard C library call (printf,
scanf, read, write, etc.),

@item C library (in fact that's Cygnus Newlib) calls RTEMS 
system call interface. This code can be found in the 
@code{c/src/lib/libc} directory.

@item Glue code calls the serial driver entry routines. 

@end itemize

@subsection Termios and Polled I/O

The following functions are provided by the driver and invoked by
Termios for simple character input/output.  The specific names of
these routines are not important as Termios invokes them indirectly
via function pointers.

@subsubsection pollWrite

The @code{pollWrite} routine is responsible for writing @code{len} characters
from @code{buf} to the serial device specified by @code{minor}.

@example
@group
int pollWrite (int minor, const char *buf, int len) 
@{
  for (i=0; i<len; i++) @{
     put buf[i] into the UART channel minor
     wait for the character to be transmitted
     on the serial line
  @}
  return 0
@}
@end group
@end example

@subsubsection pollRead

The @code{pollRead} routine is responsible for reading a single character
from the serial device specified by @code{minor}.  If no character is
available, then the routine should return -1.

@example
@group
int pollRead(int minor)
@{
   read status of UART
   if status indicates a character is available
     return character
   return -1
@}
@end group
@end example

@subsection Termios and Interrupt Driven I/O

The UART generally generates interrupts when it is ready to accept or to
emit a number of characters. In this mode, the interrupt subroutine is the
core of the driver.

@subsubsection InterruptHandler

The @code{InterruptHandler} is responsible for processing asynchronous
interrupts from the UART.  There may be multiple interrupt handlers for
a single UART.  Some UARTs can generate a unique interrupt vector for
each interrupt source such as a character has been received or the
transmitter is ready for another character.

In the simplest case, the @code{InterruptHandler} will have to check 
the status of the UART and determine what caused the interrupt.
The following describes the operation of an @code{InterruptHandler} 
which has to do this:

@example
@group
rtems_isr InterruptHandler (rtems_vector_number v)
@{
  check whether there was an error

  if some characters were received: 
     Ask Termios to put them on his input buffer

  if some characters have been transmitted
        (i.e. the UART output buffer is empty)
     Tell TERMIOS that the characters have been
     transmitted. The TERMIOS routine will call
     the InterruptWrite function with the number
     of characters not transmitted yet if it is
     not zero.
@}
@end group
@end example

@subsubsection InterruptWrite

The @code{InterruptWrite} is responsible for telling the UART 
that the @code{len} characters at @code{buf} are to be transmitted.

@example
static int InterruptWrite(int minor, const char *buf, int len)
@{
  tell the UART to transmit len characters from buf
  return 0
@}
@end example

The driver has to put the @i{n} first buf characters in the UART channel minor
buffer (@i{n} is the UART channel size, @i{n}=1 on the MC68640). Generally, an
interrupt is raised after these @i{n} characters being transmitted. So
UART interrupts may have to be enabled after putting the characters in the
UART. 


@subsection Initialization

The driver initialization is called once during the RTEMS initialization
process. 

The @code{console_initialize} function has to: 

@itemize @bullet

@item initialize Termios support: call @code{rtems_termios_initialize()}.  If
Termios has already been initialized by another device driver, then
this call will have no effect.

@item Initialize the UART: This procedure should
be described in the UART manual.  This procedure @b{MUST} be
followed precisely.  This procedure varies but 
usually consists of: 

@itemize @bullet
@item reinitialize the UART channels

@item set the channels configuration to the Termios default:
9600 bauds, no parity, 1 stop bit, and 8 bits per character
@end itemize

@item If interrupt driven, register the console interrupt routine to RTEMS:

@example
rtems_interrupt_catch(
    InterruptHandler, CONSOLE_VECTOR, &old_handler);
@end example

@item enable the UART channels.

@item register the device name: in order to use the console (i.e. being
able to do printf/scanf on stdin, stdout, and stderr), some device
must be registered as "/dev/console":

@example
rtems_io_register_name ("dev/console", major, i);
@end example

@end itemize

@subsection Opening a serial device

The @code{console_open} function is called whenever a serial
device is opened.  The device registered as @code{"/dev/console"}
is opened automatically during RTEMS initialization.
For instance, if UART channel 2 is registered as "/dev/tty1",
the @code{console_open} entry point will be called as
the result of an @code{fopen("/dev/tty1", mode)} in the
application. 

The @code{console_open} function has to inform Termios of the low-level
functions for serial line support; the "callbacks". 

The gen68340 BSP defines two sets of callback tables: 

@itemize @bullet

@item one with functions for polled input/output

@item another with functions for interrupt driven input/output 

@end itemize

This code can be found in the file @code{$BSPROOT/console/console.c}.

@subsubsection Polled I/O

Termios must be told the addresses of the functions that are to be 
used for simple character input/output, i.e. pointers to the
@code{pollWrite} and @code{pollRead} functions
defined earlier in @ref{Console Driver Termios and Polled I/O}.

@subsubsection Interrupt Driven I/O

Driver functioning is quite different in this mode.  There is no
device driver read function to be passed to Termios. Indeed a
@code{console_read} call returns the contents of Termios input buffer.
This buffer is filled in the driver interrupt subroutine
(see @ref{Console Driver Termios and Interrupt Driven I/O}).

The driver is responsible for providing a pointer to the 
@code{InterruptWrite} function. 

@subsection Closing a Serial Device

The @code{console_close} is invoked when the serial device is to
be closed.  This entry point corresponds to the device driver
close entry point.

This routine is responsible for notifying Termios that the serial
device was closed.  This is done with a call to @code{rtems_termios_close}. 

@subsection Reading Characters From a Serial Device

The @code{console_read} is invoked when the serial device is to
be read from.  This entry point corresponds to the device driver
read entry point.

This routine is responsible for returning the content of the
Termios input buffer.   This is done by invoking the
@code{rtems_termios_read} routine. 

@subsection Writing Characters to a Serial Device

The @code{console_write} is invoked when the serial device is to
be written to.  This entry point corresponds to the device driver
write entry point.

This routine is responsible for adding the requested characters to
the Termios output queue for this device.  This is done by
calling the routine @code{rtems_termios_write} 
to add the characters at the end of the Termios output
buffer. 

@subsection Changing Serial Line Parameters

The @code{console_control} is invoked when the line parameters
for a particular serial device are to be changed.
This entry point corresponds to the device driver
io_control entry point.

The application write is able to control the serial line configuration
with Termios calls (such as the @code{ioctl} command, see 
the Termios documentation for
more details). If the driver is to support dynamic configuration, then
is must have the @code{console_control} piece of code. Refer to the gen68340
BSP for an example of how it is done.  Basically @code{ioctl} 
commands call @code{console_control} with the serial line
configuration in a Termios defined data structure. The driver
is responsible for reinitializing the UART with the correct settings. 

