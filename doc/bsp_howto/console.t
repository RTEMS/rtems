@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Console Driver

@subsection = Introduction

This chapter describes how to do a console driver using RTEMS Termios
support. 

The serial driver is called as soon as printf/scanf or read/write kind of
input/output are needed. There are two main functioning modes: 

@itemize @bullet

@item console: formatted input/output, with special characters (end of
line, tabulations, etc...) recognition and processing,

@item raw: permits raw data processing. 

@end itemize

We may think that one need two serial drivers to deal with those two types
of data, but Termios permits having only one driver. 

@subsection = Termios

Termios is a standard for terminal management, included in several Unix
versions. Termios is good because: 

@itemize @bullet

@item from the user's side: primitives to access the terminal and change
configuration settings are the same under Unix and Rtems. 

@item from the BSP developer's side: it frees you from dealing with buffer
states and mutual exclusions on them. 

@end itemize

Termios support includes: 

@itemize @bullet

@item raw and console handling,

@item blocking or non-blocking characters receive, with or without
Timeout. 

@end itemize

For more information on Termios, type man termios in your Unix box or go
to http://www.freebsd.org/cgi/man.cgi. 

@subsection = Driver Functioning Modes

There are generally two main functioning modes for an UART (Universal
Asynchronous Receiver-Transmitter, i.e. the serial chip): 

@itemize @bullet

@item polling mode: the processor blocks on sending/receiving characters.
This mode is not powerful, but is necessary when one wants to print an
error message when the board hung. This is also the most simple mode to
program,

@item interrupt mode: the processor doesn't block on sending/receiving
characters. Two buffers (one for the in-going characters, the others for
the characters to be sent) are used. An interrupt is raised as soon as a
character is in the UART. Then the int errupt subroutine insert the
character at the input buffer queue. When a character is asked for input,
this at the head of the buffer is returned. When characters have to be
sent, one have to put the first characters (the number depends on the
UART) in th e UART. Then an interrupt is raised when all the characters
have been emitted. The interrupt subroutine has to send the characters
remaining in the output buffer the same way. 

@end itemize

@subsection = Serial Driver Functioning Overview

Figure 5 is an attempt of showing how a Termios driven serial driver works : 

@itemize @bullet

@item the application programmer uses standard C library call (printf,
scanf, read, write, etc.),

@item C library (in fact that's Cygnus Newlib) calls RTEMS procedure: glue
is made in newlib*.c files which can be found under
$RTEMS_ROOT/c/src/lib/libc directory,

@item Glue code calls your serial driver entry routines. 

@end itemize

@subsection = Polled I/O

You have to point Termios out which functions are used for simple
character input/output: 


Function

Description

@example
int pollWrite (int minor, const char *buf, int len) 

for (i=0; i<len; i++) {
     put buf[i] into the UART channel minor
     wait for the character to be transmitted
     on the serial line
}
int pollread(int minor)
@end example

wait for a character to be available in the UART channel minor, then return it.

@subsection = Interrupted I/O

The UART generally generates interrupts when it is ready to accept or to
emit a number of characters. In this mode, the interrupt subroutine is the
core of the driver: 


Function

Description

@example
rtems_isr InterruptHandler (rtems_vector_number v)
@end example

check whether there was an error

if some characters were received: 
   ask Termios to put them on his input buffer

if some characters have been transmitted (i.e. the UART output buffer is empty)
   tell TERMIOS that the characters have been
   transmitted. The TERMIOS routine will call
   the InterruptWrite function with the number
   of characters not transmitted yet if it is
   not zero.

@example
static int InterruptWrite(int minor, const char *buf, int len)
@end example

you have to put the n first buf characters in the UART channel minor
buffer (n is the UART channel size, n=1 on the MC68640). Generally, an
interrupt is raised after these n characters being transmitted. So you may
have to enable the UART interrupts after having put the characters in the
UART. 


Figure 5: general TERMIOS driven serial driver functioning

@subsection = Initialization

The driver initialization is called once during RTEMS initialization
process. 

The console_initialize function has to : 

@itemize @bullet

@item initialize Termios support: call rtems_termios_initialize(),

@item initialize your integrated processor's UART: the operation is
normally described in your user's manual and you must follow these
instructions but it usually consists in: 

@item reinitialize the UART channels,

@item set the channels configuration to Termios default one, i.e.: 9600
bauds, no parity, 1 stop bit, 8 bits per character,

@item register your console interrupt routine to RTEMS: 

@example
	rtems_interrupt_catch (InterruptHandler,CONSOLE_VECTOR,&old_handler);
@end example

@item enable the UART channels,

@item register your device name: in order to use the console (i.e. being
able to do printf/scanf on stdin, stdout, and stderr), you have to
register the "/dev/console" device: 

@example
rtems_io_register_name ("dev/console", major, i);
@end example

@end itemize

@subsection = Opening a serial device

The console device is opened during RTEMS initialization but the
console_open function is called when a new device is opened. For instance,
if you register the "/dev/tty1" device for the UART channel 2, the
console_open will be called with a fopen("/dev/t ty", mode) in your
application. 

The console_open function has to inform Termios of your low-level function
for serial line support; the "callbacks". 

The gen68340 BSP defines two kinds of callbacks: 

@itemize @bullet

@item functions to use with polled input/output,

@item functions to use with interrupted input/output. 

@end itemize

@subsubsection = Polled I/O

You have to point Termios out which functions are used for simple
character input/output, i.e. pointers to pollWrite and pollRead functions
defined in 8.4.1. 

@subsubsection = Interrupted I/O

Driver functioning is quite different in this mode. You can see there's no
read function passed to Termios. Indeed a console_read call returns the
content of Termios input buffer. This buffer is filled in the driver
interrupt subroutine (cf. 8.4.2). 

But you actually have to provide a pointer to the InterruptWrite function. 

@subsection = Closing a serial device

The driver entry point is: console_close. 

You just have to notify Termios that the serial device was closed, with a
call to rtems_termios_close. 

@subsection = Reading characters from the serial device

The driver entry point is: console_read. 

You just have to return the content of the Termios input buffer. 

Call rtems_termios_read. 

@subsection = Writing characters to the serial device

The driver entry point is: console_write. 

You just have to add the characters at the end of the Termios output
buffer. 

Call rtems_termios_write. 

@subsection = Changing serial line parameters

The driver entry point is: console_control. 

The application write is able to control the serial line configuration
with Termios calls (such as the ioctl command, see Termios man page for
more details). If you want to support dynamic configuration, you have to
write the console_control piece of code . Look at the gen68340 BSP for an
example of how it is done. Basically ioctl commands call console_control
with the serial line configuration in a Termios structure. You just have
to reinitialize the UART with the correct settings. 

