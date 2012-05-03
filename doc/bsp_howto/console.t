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

@item C library (e.g. RedHat (formerly Cygnus) Newlib) calls
the RTEMS system call interface.  This code can be found in the
@code{cpukit/libcsupport/src} directory.

@item Glue code calls the serial driver entry routines. 

@end itemize

@subsection Basics

You need to include the following header files in your Termios device driver
source file:
@example
@group
#include <unistd.h>
#include <termios.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/console.h>
@end group
@end example

You need to provide a data structure for the Termios driver interface.  The
functions are described later in this chapter.  The functions should return
zero on succes and minus one in case of an error.  Currently the return value
will be not checked from the Termios infrastructure in most cases.  One notable
exception is the polled read function, here is the return value important.

If you want to use polled IO it should look like the following.  You may also
have a look at @code{c/src/lib/libbsp/shared/console-polled.c} for a shared
implementation of the basic framework.  Termios must be told the addresses of
the functions that are to be used for simple character IO, i.e. pointers to the
@code{my_driver_poll_read} and @code{my_driver_poll_write} functions described
later in @ref{Console Driver Termios and Polled IO}.

@example
@group
static const rtems_termios_callbacks my_driver_callbacks_polled = @{
    .firstOpen = my_driver_first_open,
    .lastClose = my_driver_last_close,
    .pollRead = my_driver_poll_read,
    .write = my_driver_poll_write,
    .setAttributes = my_driver_set_attributes,
    .stopRemoteTx = NULL,
    .startRemoteTx = NULL,
    .outputUsesInterrupts = TERMIOS_POLLED
@};
@end group
@end example

For an interrupt driven implementation you need the following.  The driver
functioning is quite different in this mode.  There is no device driver read
function to be passed to Termios.  Indeed a @code{console_read} call returns
the contents of Termios input buffer.  This buffer is filled in the driver
interrupt subroutine, see also
@ref{Console Driver Termios and Interrupt Driven IO}.
The driver is responsible for providing a pointer to the
@code{my_driver_interrupt_write} function. 

@example
@group
static const rtems_termios_callbacks my_driver_callbacks_interrupt = @{
    .firstOpen = my_driver_first_open,
    .lastClose = my_driver_last_close,
    .pollRead = NULL,
    .write = my_driver_interrupt_write,
    .setAttributes = my_driver_set_attributes,
    .stopRemoteTx = NULL,
    .startRemoteTx = NULL,
    .outputUsesInterrupts = TERMIOS_IRQ_DRIVEN
@};
@end group
@end example

You can also provide callback functions for remote transmission control.  This
is not covered in this manual, so thay are set to @code{NULL} in the above
examples.

Normally the device specific data structures are stored in a table which is
indexed by the minor number.  You may need an entry for the Termios handler
pointer in your data structure.  For simplicity of the console initialization
example the device name is also present.

@example
@group
/* Driver specific data structure */
typedef struct @{
    const char *device_name;
    struct rtems_termios_tty *tty;
@} my_driver_entry;

/*
 * This table contains the driver specific data.  It is later
 * indexed by the minor number.
 */
static my_driver_entry my_driver_table [MY_DRIVER_DEVICE_NUMBER];
@end group
@end example

@subsection Termios and Polled IO

The following functions are provided by the driver and invoked by
Termios for simple character IO.

The @code{my_driver_poll_write} routine is responsible for writing @code{n}
characters from @code{buf} to the serial device specified by @code{minor}.

On success, the number of bytes written is returned (zero indicates nothing
was written).  On error, @code{-1} is returned.

NOTE: Due to the current implementation of termios, any data passed into
  the write function will be lost. 

@example
@group
static ssize_t my_driver_poll_write(int minor, const char *buf, size_t n)
@{
    my_driver_entry *e = &my_driver_table [minor];
    int i = 0;
    
    /*
     * There is no need to check the minor number since it is derived
     * from a file descriptor.  The upper layer takes care that it is
     * in a valid range.
     */
    
    /* Write */
    for (i = 0; i < n; ++i) @{
        my_driver_write_char(e, buf [i]);
    @}
    
    return n;
@}
@end group
@end example

The @code{my_driver_poll_read} routine is responsible for reading a single
character from the serial device specified by @code{minor}.  If no character is
available, then the routine should return minus one.

@example
@group
static int my_driver_poll_read(int minor)
@{
    my_driver_entry *e = &my_driver_table [minor];
    
    /*
     * There is no need to check the minor number since it is derived
     * from a file descriptor.  The upper layer takes care that it is
     * in a valid range.
     */

    /* Check if a character is available */
    if (my_driver_can_read_char(e)) @{
        /* Return the character */
        return my_driver_read_char(e);
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

The @code{my_driver_interrupt_handler} is responsible for processing
asynchronous interrupts from the UART.  There may be multiple interrupt
handlers for a single UART.  Some UARTs can generate a unique interrupt vector
for each interrupt source such as a character has been received or the
transmitter is ready for another character.

In the simplest case, the @code{my_driver_interrupt_handler} will have to check
the status of the UART and determine what caused the interrupt.  The following
describes the operation of an @code{my_driver_interrupt_handler} which has to
do this:

@example
@group
static void my_driver_interrupt_handler(
    rtems_vector_number vector,
    void *arg
)
@{
    my_driver_entry *e = (my_driver_entry *) arg;
    char buf [N];
    int n = 0;

    /*
     * Check if we have received something.  The function reads the
     * received characters from the device and stores them in the
     * buffer.  It returns the number of read characters.
     */
    n = my_driver_read_received_chars(e, buf, N);
    if (n > 0) @{
        /* Hand the data over to the Termios infrastructure */
        rtems_termios_enqueue_raw_characters(e->tty, buf, n);
    @}

    /*
     * Check if we have something transmitted.  The functions returns
     * the number of transmitted characters since the last write to the
     * device.
     */
    n = my_driver_transmitted_chars(e);
    if (n > 0) @{
        /*
         * Notify Termios that we have transmitted some characters.  It
         * will call now the interrupt write function if more characters
         * are ready for transmission.
         */
        rtems_termios_dequeue_characters(e->tty, n);
    @}
@}
@end group
@end example

The @code{my_driver_interrupt_write} function is responsible for telling the
device that the @code{n} characters at @code{buf} are to be transmitted.  The
return value may be arbitrary since it is not checked from Termios.  It is
guaranteed that @code{n} is greater than zero.  This routine is invoked either
from task context with disabled interrupts to start a new transmission process
with exactly one character in case of an idle output state or from the
interrupt handler to refill the transmitter.  If the routine is invoked to
start the transmit process the output state will become busy and Termios starts
to fill the output buffer.  If the transmit interrupt arises before Termios was
able to fill the transmit buffer you will end up with one interrupt per
character.

On error, the function should return @code{-1}. On success, it should return
@code{0}, since it the interrupt handler will report the actual number of
characters transmitted.
 
@example
@group
static ssize_t my_driver_interrupt_write(int minor, const char *buf, size_t n)
@{
    my_driver_entry *e = &my_driver_table [minor];
    
    /*
     * There is no need to check the minor number since it is derived
     * from a file descriptor.  The upper layer takes care that it is
     * in a valid range.
     */

    /*
     * Tell the device to transmit some characters from buf (less than
     * or equal to n).  When the device is finished it should raise an
     * interrupt.  The interrupt handler will notify Termios that these
     * characters have been transmitted and this may trigger this write
     * function again.  You may have to store the number of outstanding
     * characters in the device data structure.
     */

    return 0;
@}
@end group
@end example

@subsection Initialization

The driver initialization is called once during the RTEMS initialization
process. 

The @code{console_initialize} function may look like this:

@example
@group
rtems_device_driver console_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    rtems_device_minor_number i = 0;

    /*
     * Initialize the Termios infrastructure.  If Termios has already
     * been initialized by another device driver, then this call will
     * have no effect.
     */
    rtems_termios_initialize();
    
    /* Initialize each device */
    for (i = 0; i < MY_DRIVER_DEVICE_NUMBER; ++i) @{
        my_driver_entry *e = &my_driver_table [i];

        /*
         * Register this device in the file system.  In order to use the
         * console (i.e. being able to do printf, scanf etc. on stdin,
         * stdout and stderr), some device must be registered
         * as "/dev/console" (CONSOLE_DEVICE_NAME).
         */
        sc = rtems_io_register_name (e->device_name, major, i);
        RTEMS_CHECK_SC(sc, "Register IO device");

        /*
         * Initialize this device and install the interrupt handler if
         * necessary.  You may also initialize the device in the first
         * open call.
         */
    @}

    return RTEMS_SUCCESSFUL;
@}
@end group
@end example

@subsection Opening a serial device

The @code{console_open} function is called whenever a serial device is opened.
The device registered as @code{"/dev/console"} (@code{CONSOLE_DEVICE_NAME}) is
opened automatically during RTEMS initialization.  For instance, if UART
channel 2 is registered as "/dev/tty1", the @code{console_open} entry point
will be called as the result of an @code{fopen("/dev/tty1", mode)} in the
application. 

The @code{console_open} function has to inform Termios of the low-level
functions for serial line support. 

@example
@group
rtems_device_driver console_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    struct rtems_termios_callbacks *callbacks =
        &my_driver_callbacks_polled;

    /*
     * Check the minor number.  Termios does currently not check
     * the return value of the first open call so the minor
     * number must be checked here.
     */
    if (MY_DRIVER_IS_MINOR_INVALID(minor)) @{
        return RTEMS_INVALID_NUMBER;
    @}

    /*
     * Depending on the IO mode you need to pass a different set of
     * callback functions to Termios.
     */
    if (MY_DRIVER_USES_INTERRUPTS(minor)) @{
        callbacks = &my_driver_callbacks_interrupt;
    @}

    return rtems_termios_open(major, minor, arg, callbacks);
@}
@end group
@end example

During the first open of the device Termios will call @code{my_driver_first_open}.

@example
@group
static int my_driver_first_open(int major, int minor, void *arg)
@{
    my_driver_entry *e = &my_driver_table [minor];
    struct rtems_termios_tty *tty =
        ((rtems_libio_open_close_args_t *) arg)->iop->data1;
    
    /* Check minor number */
    if (MY_DRIVER_IS_MINOR_INVALID(minor)) @{
        return -1;
    @}

    /* Connect the TTY data structure */
    e->tty = tty;

    /*
     * You may add some initialization code here.
     */

    /*
     * Sets the inital baud rate.  This should be set to the value of
     * the boot loader.
     */
    return rtems_termios_set_initial_baud(e->tty, MY_DRIVER_BAUD_RATE);
@}
@end group
@end example

@subsection Closing a Serial Device

The @code{console_close} is invoked when the serial device is to be closed.
This entry point corresponds to the device driver close entry point.

This routine is responsible for notifying Termios that the serial device was
closed.  This is done with a call to @code{rtems_termios_close}. 

@example
@group
rtems_device_driver console_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    return rtems_termios_close(arg);
@}
@end group
@end example

Termios will call the @code{my_driver_last_close} function if the last close
happens on the device.
@example
@group
static int my_driver_last_close(int major, int minor, void *arg)
@{
    my_driver_entry *e = &my_driver_table [minor];
    
    /*
     * There is no need to check the minor number since it is derived
     * from a file descriptor.  The upper layer takes care that it is
     * in a valid range.
     */

    /* Disconnect the TTY data structure */
    e->tty = NULL;

    /*
     * The driver may do some cleanup here.
     */

    return 0;
@}
@end group
@end example

@subsection Reading Characters from a Serial Device

The @code{console_read} is invoked when the serial device is to be read from.
This entry point corresponds to the device driver read entry point.

This routine is responsible for returning the content of the Termios input
buffer.  This is done by invoking the @code{rtems_termios_read} routine. 

@example
@group
rtems_device_driver console_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    return rtems_termios_read(arg);
@}
@end group
@end example

@subsection Writing Characters to a Serial Device

The @code{console_write} is invoked when the serial device is to be written to.
This entry point corresponds to the device driver write entry point.

This routine is responsible for adding the requested characters to the Termios
output queue for this device.  This is done by calling the routine
@code{rtems_termios_write} to add the characters at the end of the Termios
output buffer. 

@example
@group
rtems_device_driver console_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    return rtems_termios_write(arg);
@}
@end group
@end example

@subsection Changing Serial Line Parameters

The @code{console_control} is invoked when the line parameters for a particular
serial device are to be changed.  This entry point corresponds to the device
driver io_control entry point.

The application writer is able to control the serial line configuration with
Termios calls (such as the @code{ioctl} command, see the Termios documentation
for more details).  If the driver is to support dynamic configuration, then it
must have the @code{console_control} piece of code.  Basically @code{ioctl}
commands call @code{console_control} with the serial line configuration in a
Termios defined data structure.

@example
@group
rtems_device_driver console_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
)
@{
    return rtems_termios_ioctl(arg);
@}
@end group
@end example

The driver is responsible for reinitializing the device with the correct
settings.  For this purpuse Termios calls the @code{my_driver_set_attributes}
function.

@example
@group
static int my_driver_set_attributes(
    int minor,
    const struct termios *t
)
@{
    my_driver_entry *e = &my_driver_table [minor];
    
    /*
     * There is no need to check the minor number since it is derived
     * from a file descriptor.  The upper layer takes care that it is
     * in a valid range.
     */

    /*
     * Inspect the termios data structure and configure the device
     * appropriately.  The driver should only be concerned with the
     * parts of the structure that specify hardware setting for the
     * communications channel such as baud, character size, etc.
     */

    return 0;
@}
@end group
@end example
