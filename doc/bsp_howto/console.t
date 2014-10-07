@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

Termios is a standard for terminal management, included in the POSIX
1003.1b standard.  As part of the POSIX and Open Group Single UNIX
Specification, is commonly provided on UNIX implementations.  The
Open Group has the termios portion of the POSIX standard online
at @uref{http://opengroup.org/onlinepubs/007908775/xbd/termios.html
,http://opengroup.org/onlinepubs/007908775/xbd/termios.html}.
The requirements for the @code{<termios.h>} file are also provided
and are at @uref{http://opengroup.org/onlinepubs/007908775/xsh/termios.h.html,
http://opengroup.org/onlinepubs/007908775/xsh/termios.h.html}.

Having RTEMS support for Termios is beneficial because:

@itemize @bullet

@item from the user's side because it provides standard primitive operations
to access the terminal and change configuration settings.  These operations
are the same under UNIX and RTEMS.

@item from the BSP developer's side because it frees the
developer from dealing with buffer states and mutual exclusions on them.
Early RTEMS console device drivers also did their own special
character processing.

@item it is part of an internationally recognized standard.

@item it makes porting code from other environments easier.

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

There are generally three main functioning modes for an UART (Universal
Asynchronous Receiver-Transmitter, i.e. the serial chip):

@itemize @bullet

@item polled mode
@item interrupt driven mode
@item task driven mode

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

The task driven mode is similar to interrupt driven mode, but the actual data
processing is done in dedicated tasks instead of interrupt routines.

@section Serial Driver Functioning Overview

The following Figure shows how a Termios driven serial driver works:

@ifset use-ascii
@center Figure not included in ASCII version
@end ifset

@ifset use-tex
@sp1
@center{@image{TERMIOSFlow,,6in}}
@end ifset

@ifset use-html
@html
<P ALIGN="center"><IMG SRC="TERMIOSFlow.png" ALT="Termios Flow"></P>
@end html
@end ifset

The following list describes the basic flow.

@itemize @bullet

@item the application programmer uses standard C library call (printf,
scanf, read, write, etc.),

@item C library (ctx.g. RedHat (formerly Cygnus) Newlib) calls
the RTEMS system call interface.  This code can be found in the
@file{cpukit/libcsupport/src} directory.

@item Glue code calls the serial driver entry routines.

@end itemize

@subsection Basics

The low-level driver API changed between RTEMS 4.10 and RTEMS 4.11.  The legacy
callback API is still supported, but its use is discouraged.  The following
functions are deprecated:

@itemize @bullet

@item @code{rtems_termios_open()} - use @code{rtems_termios_device_open()} in
combination with @code{rtems_termios_device_install()} instead.

@item @code{rtems_termios_close()} - use @code{rtems_termios_device_close()}
instead.

@end itemize

This manual describes the new API.  A new console driver should consist of
three parts.

@enumerate

@item The basic console driver functions using the Termios support.  Add this
the BSPs Makefile.am:

@example
@group
[...]
libbsp_a_SOURCES += ../../shared/console-termios.c
[...]
@end group
@end example

@item A general serial module specific low-level driver providing the handler
table for the Termios @code{rtems_termios_device_install()} function.  This
low-level driver could be used for more than one BSP.

@item A BSP specific initialization routine @code{console_initialize()}, that
calls @code{rtems_termios_device_install()} providing a low-level driver
context for each installed device.

@end enumerate

You need to provide a device handler structure for the Termios device
interface.  The functions are described later in this chapter.  The first open
and set attributes handler return a boolean status to indicate success (true)
or failure (false).  The polled read function returns an unsigned character in
case one is available or minus one otherwise.

If you want to use polled IO it should look like the following.  Termios must
be told the addresses of the handler that are to be used for simple character
IO, i.e. pointers to the @code{my_driver_poll_read()} and
@code{my_driver_poll_write()} functions described later in @ref{Console Driver
Termios and Polled IO}.

@example
@group
const rtems_termios_handler my_driver_handler_polled = @{
  .first_open = my_driver_first_open,
  .last_close = my_driver_last_close,
  .poll_read = my_driver_poll_read,
  .write = my_driver_poll_write,
  .set_attributes = my_driver_set_attributes,
  .stop_remote_tx = NULL,
  .start_remote_tx = NULL,
  .mode = TERMIOS_POLLED
@}
@end group
@end example

For an interrupt driven implementation you need the following.  The driver
functioning is quite different in this mode.  There is no device driver read
handler to be passed to Termios.  Indeed a @code{console_read()} call returns the
contents of Termios input buffer.  This buffer is filled in the driver
interrupt subroutine, see also @ref{Console Driver Termios and Interrupt Driven
IO}.  The driver is responsible for providing a pointer to the
@code{my_driver_interrupt_write()} function.

@example
@group
const rtems_termios_handler my_driver_handler_interrupt = @{
  .first_open = my_driver_first_open,
  .last_close = my_driver_last_close,
  .poll_read = NULL,
  .write = my_driver_interrupt_write,
  .set_attributes = my_driver_set_attributes,
  .stopRemoteTx = NULL,
  .stop_remote_tx = NULL,
  .start_remote_tx = NULL,
  .mode = TERMIOS_IRQ_DRIVEN
@};
@end group
@end example

You can also provide hander for remote transmission control.  This
is not covered in this manual, so they are set to @code{NULL} in the above
examples.

The low-level driver should provide a data structure for its device context.
The initialization routine must provide a context for each installed device via
@code{rtems_termios_device_install()}.  For simplicity of the console
initialization example the device name is also present.  Her is an example header file.

@example
@group
#ifndef MY_DRIVER_H
#define MY_DRIVER_H

#include <rtems/termiostypes.h>

#include <some-chip-header.h>

/* Low-level driver specific data structure */
typedef struct @{
  rtems_termios_device_context base;
  const char *device_name;
  volatile module_register_block *regs;
  /* More stuff */
@} my_driver_context;

extern const rtems_termios_handler my_driver_handler_polled;

extern const rtems_termios_handler my_driver_handler_interrupt;

#endif /* MY_DRIVER_H */
@end group
@end example

@subsection Termios and Polled IO

The following handler are provided by the low-level driver and invoked by
Termios for simple character IO.

The @code{my_driver_poll_write()} routine is responsible for writing @code{n}
characters from @code{buf} to the serial device specified by @code{tty}.

@example
@group
static void my_driver_poll_write(
  rtems_termios_device_context *base,
  const char                   *buf,
  size_t                        n
)
@{
  my_driver_context *ctx = (my_driver_context *) base;
  size_t i;

  /* Write */
  for (i = 0; i < n; ++i) @{
    my_driver_write_char(ctx, buf[i]);
  @}
@}
@end group
@end example

The @code{my_driver_poll_read} routine is responsible for reading a single
character from the serial device specified by @code{tty}.  If no character is
available, then the routine should return minus one.

@example
@group
static int my_driver_poll_read(rtems_termios_device_context *base)
@{
  my_driver_context *ctx = (my_driver_context *) base;

  /* Check if a character is available */
  if (my_driver_can_read_char(ctx)) @{
    /* Return the character */
    return my_driver_read_char(ctx);
  @} else @{
    /* Return an error status */
    return -1;
  @}
@}
@end group
@end example

@subsection Termios and Interrupt Driven IO

The UART generally generates interrupts when it is ready to accept or to emit a
number of characters.  In this mode, the interrupt subroutine is the core of
the driver.

The @code{my_driver_interrupt_handler()} is responsible for processing
asynchronous interrupts from the UART.  There may be multiple interrupt
handlers for a single UART.  Some UARTs can generate a unique interrupt vector
for each interrupt source such as a character has been received or the
transmitter is ready for another character.

In the simplest case, the @code{my_driver_interrupt_handler()} will have to check
the status of the UART and determine what caused the interrupt.  The following
describes the operation of an @code{my_driver_interrupt_handler} which has to
do this:

@example
@group
static void my_driver_interrupt_handler(
  rtems_vector_number  vector,
  void                *arg
)
@{
  rtems_termios_tty *tty = arg;
  my_driver_context *ctx = rtems_termios_get_device_context(tty);
  char buf[N];
  size_t n;

  /*
   * Check if we have received something.  The function reads the
   * received characters from the device and stores them in the
   * buffer.  It returns the number of read characters.
   */
  n = my_driver_read_received_chars(ctx, buf, N);
  if (n > 0) @{
    /* Hand the data over to the Termios infrastructure */
    rtems_termios_enqueue_raw_characters(tty, buf, n);
  @}

  /*
   * Check if we have something transmitted.  The functions returns
   * the number of transmitted characters since the last write to the
   * device.
   */
  n = my_driver_transmitted_chars(ctx);
  if (n > 0) @{
    /*
     * Notify Termios that we have transmitted some characters.  It
     * will call now the interrupt write function if more characters
     * are ready for transmission.
     */
    rtems_termios_dequeue_characters(tty, n);
  @}
@}
@end group
@end example

The @code{my_driver_interrupt_write()} function is responsible for telling the
device that the @code{n} characters at @code{buf} are to be transmitted.  It
the value @code{n} is zero to indicate that no more characters are to send.
The driver can disable the transmit interrupts now.  This routine is invoked
either from task context with disabled interrupts to start a new transmission
process with exactly one character in case of an idle output state or from the
interrupt handler to refill the transmitter.  If the routine is invoked to
start the transmit process the output state will become busy and Termios starts
to fill the output buffer.  If the transmit interrupt arises before Termios was
able to fill the transmit buffer you will end up with one interrupt per
character.

@example
@group
static void my_driver_interrupt_write(
  rtems_termios_device_context  *base,
  const char                    *buf,
  size_t                         n
)
@{
  my_driver_context *ctx = (my_driver_context *) base;

  /*
   * Tell the device to transmit some characters from buf (less than
   * or equal to n).  When the device is finished it should raise an
   * interrupt.  The interrupt handler will notify Termios that these
   * characters have been transmitted and this may trigger this write
   * function again.  You may have to store the number of outstanding
   * characters in the device data structure.
   */

  /*
   * Termios will set n to zero to indicate that the transmitter is
   * now inactive.  The output buffer is empty in this case.  The
   * driver may disable the transmit interrupts now.
   */
@}
@end group
@end example

@subsection Initialization

The BSP specific driver initialization is called once during the RTEMS
initialization process.

The @code{console_initialize()} function may look like this:

@example
@group
#include <my-driver.h>
#include <rtems/console.h>
#include <bsp.h>
#include <bsp/fatal.h>

static my_driver_context driver_context_table[M] = @{ /* Some values */ @};

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
@{
  rtems_status_code sc;
#ifdef SOME_BSP_USE_INTERRUPTS
  const rtems_termios_handler *handler = &my_driver_handler_interrupt;
#else
  const rtems_termios_handler *handler = &my_driver_handler_polled;
#endif

  /*
   * Initialize the Termios infrastructure.  If Termios has already
   * been initialized by another device driver, then this call will
   * have no effect.
   */
  rtems_termios_initialize();

  /* Initialize each device */
  for (
    minor = 0;
    minor < RTEMS_ARRAY_SIZE(driver_context_table);
    ++minor
  ) @{
    my_driver_context *ctx = &driver_context_table[minor];

    /*
     * Install this device in the file system and Termios.  In order
     * to use the console (i.e. being able to do printf, scanf etc.
     * on stdin, stdout and stderr), one device must be registered as
     * "/dev/console" (CONSOLE_DEVICE_NAME).
     */
    sc = rtems_termios_device_install(
      ctx->device_name,
      major,
      minor,
      handler,
      NULL,
      ctx
    );
    if (sc != RTEMS_SUCCESSFUL) @{
      bsp_fatal(SOME_BSP_FATAL_CONSOLE_DEVICE_INSTALL);
    @}
  @}

  return RTEMS_SUCCESSFUL;
@}
@end group
@end example

@subsection Opening a serial device

The @code{console_open()} function provided by @file{console-termios.c} is
called whenever a serial device is opened.  The device registered as
@code{"/dev/console"} (@code{CONSOLE_DEVICE_NAME}) is opened automatically
during RTEMS initialization.  For instance, if UART channel 2 is registered as
@code{"/dev/tty1"}, the @code{console_open()} entry point will be called as the
result of an @code{fopen("/dev/tty1", mode)} in the application.

During the first open of the device Termios will call the
@code{my_driver_first_open()} handler.

@example
@group
static bool my_driver_first_open(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
@{
  my_driver_context *ctx = (my_driver_context *) base;
  rtems_status_code sc;
  bool ok;

  /*
   * You may add some initialization code here.
   */

  /*
   * Sets the initial baud rate.  This should be set to the value of
   * the boot loader.  This function accepts only exact Termios baud
   * values.
   */
  sc = rtems_termios_set_initial_baud(tty, MY_DRIVER_BAUD_RATE);
  if (sc != RTEMS_SUCCESSFUL) @{
    /* Not a valid Termios baud */
  @}

  /*
   * Alternatively you can set the best baud.
   */
  rtems_termios_set_best_baud(term, MY_DRIVER_BAUD_RATE);

  /*
   * To propagate the initial Termios attributes to the device use
   * this.
   */
  ok = my_driver_set_attributes(base, term);
  if (!ok) @{
    /* This is bad */
  @}

  /*
   * Return true to indicate a successful set attributes, and false
   * otherwise.
   */
  return true;
@}
@end group
@end example

@subsection Closing a Serial Device

The @code{console_close()} provided by @file{console-termios.c} is invoked when
the serial device is to be closed.  This entry point corresponds to the device
driver close entry point.

Termios will call the @code{my_driver_last_close()} handler if the last close
happens on the device.
@example
@group
static void my_driver_last_close(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  rtems_libio_open_close_args_t *args
)
@{
  my_driver_context *ctx = (my_driver_context *) base;

  /*
   * The driver may do some cleanup here.
   */
@}
@end group
@end example

@subsection Reading Characters from a Serial Device

The @code{console_read()} provided by @file{console-termios.c} is invoked when
the serial device is to be read from.  This entry point corresponds to the
device driver read entry point.

@subsection Writing Characters to a Serial Device

The @code{console_write()} provided by @file{console-termios.c} is invoked when
the serial device is to be written to.  This entry point corresponds to the
device driver write entry point.

@subsection Changing Serial Line Parameters

The @code{console_control()} provided by @file{console-termios.c} is invoked
when the line parameters for a particular serial device are to be changed.
This entry point corresponds to the device driver IO control entry point.

The application writer is able to control the serial line configuration with
Termios calls (such as the @code{ioctl()} command, see the Termios
documentation for more details).  If the driver is to support dynamic
configuration, then it must have the @code{console_control()} piece of code.
Basically @code{ioctl()} commands call @code{console_control()} with the serial
line configuration in a Termios defined data structure.

The driver is responsible for reinitializing the device with the correct
settings.  For this purpose Termios calls the @code{my_driver_set_attributes()}
handler.

@example
@group
static bool my_driver_set_attributes(
  rtems_termios_device_context *base,
  const struct termios         *term
)
@{
  my_driver_context *ctx = (my_driver_context *) base;

  /*
   * Inspect the termios data structure and configure the device
   * appropriately.  The driver should only be concerned with the
   * parts of the structure that specify hardware setting for the
   * communications channel such as baud, character size, etc.
   */

  /*
   * Return true to indicate a successful set attributes, and false
   * otherwise.
   */
  return true;
@}
@end group
@end example
