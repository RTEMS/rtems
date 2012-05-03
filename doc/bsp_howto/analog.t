@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Analog Driver

The Analog driver is responsible for providing an
interface to Digital to Analog Converters (DACs) and 
Analog to Digital Converters (ADCs).  The capabilities provided
by this class of device driver are:

@itemize @bullet
@item Initialize an Analog Board
@item Open a Particular Analog
@item Close a Particular Analog
@item Read from a Particular Analog
@item Write to a Particular Analog
@item Reset DACs
@item Reinitialize DACS
@end itemize

Most analog devices are found on I/O cards that support multiple 
DACs or ADCs on a single card.

There are currently no analog device drivers included in the 
RTEMS source tree.  The information provided in this chapter
is based on drivers developed for applications using RTEMS.
It is hoped that this driver model information can form the
basis for a standard analog driver model that can be supported
in future RTEMS distribution.

@section Major and Minor Numbers

The @b{major} number of a device driver is its index in the 
RTEMS Device Address Table.

A @b{minor} number is associated with each device instance
managed by a particular device driver.  An RTEMS minor number 
is an @code{unsigned32} entity.  Convention calls 
dividing the bits in the minor number down into categories
like the following:

@itemize @bullet

@item @b{board} - indicates the board a particular device is located on
@item @b{port} - indicates the particular device on a board.

@end itemize

From the above, it should be clear that a single device driver
can support multiple copies of the same board in a single system.
The minor number is used to distinguish the devices.

@section Analog Driver Configuration

There is not a standard analog driver configuration table but some
fields are common across different drivers.  The analog driver
configuration table is typically an array of structures with each
structure containing the information for a particular board.
The following is a list of the type of information normally required
to configure an analog board:

@table @b
@item board_offset
is the base address of a board.

@item DAC_initial_values
is an array of the voltages that should be written to each DAC
during initialization.  This allows the driver to start the board
in a known state.

@end table

@section Initialize an Analog Board

At system initialization, the analog driver's initialization entry point
will be invoked.  As part of initialization, the driver will perform
whatever board initialization is required and then set all
outputs to their configured initial state.

The analog driver may register a device name for each DAC and ADC in
the system.

@section Open a Particular Analog

This is the driver open call.  Usually this call does nothing other than
validate the minor number.  

With some drivers, it may be necessary to allocate memory when a particular
device is opened.  If that is the case, then this is often the place
to do this operation.

@section Close a Particular Analog

This is the driver close call.  Usually this call does nothing.

With some drivers, it may be necessary to allocate memory when a particular
device is opened.  If that is the case, then this is the place
where that memory should be deallocated.

@section Read from a Particular Analog

This corresponds to the driver read call.  After validating the minor
number and arguments, this call reads the indicated device.  Most analog
devices store the last value written to a DAC.  Since DACs are output 
only devices, saving the last written value gives the appearance that
DACs can be read from also.  If the device is an ADC, then it is sampled.

@b{NOTE:} Many boards have multiple analog inputs but only one ADC.  On
these boards, it will be necessary to provide some type of mutual exclusion
during reads.  On these boards, there is a MUX which must be switched 
before sampling the ADC.  After the MUX is switched, the driver must
delay some short period of time (usually microseconds) before the 
signal is stable and can be sampled.  To make matters worse, some ADCs 
cannot respond to wide voltage swings in a single sample.  On these
ADCs, one must do two samples when the voltage swing is too large.
On a practical basis, this means that the driver usually ends up
double sampling the ADC on these systems.

The value returned is a single precision floating point number 
representing the voltage read.  This value is stored in the
@code{argument_block} passed in to the call.  By returning the 
voltage, the caller is freed from having to know the number of
bits in the analog and board dependent conversion algorithm.

@section Write to a Particular Analog

This corresponds to the driver write call.  After validating the minor
number and arguments, this call writes the indicated device.  If the
specified device is an ADC, then an error is usually returned.  

The value written is a single precision floating point number
representing the voltage to be written to the specified DAC.  
This value is stored in the @code{argument_block} passed in to the
call.  By passing the voltage to the device driver, the caller is
freed from having to know the number of bits in the analog
and board dependent conversion algorithm.

@section Reset DACs

This is one of the IOCTL functions supported by the I/O control
device driver entry point.  When this IOCTL function is invoked,
all of the DACs are written to 0.0 volts.

@section Reinitialize DACS

This is one of the IOCTL functions supported by the I/O control
device driver entry point.  When this IOCTL function is invoked,
all of the DACs are written with the initial value configured
for this device.

@section Get Last Written Values

This is one of the IOCTL functions supported by the I/O control
device driver entry point.  When this IOCTL function is invoked,
the following information is returned to the caller:

@itemize @bullet 
@item last value written to the specified DAC 
@item timestamp of when the last write was performed
@end itemize

