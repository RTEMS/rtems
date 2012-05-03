@c
@c  Written by Eric Norum
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Networking Driver

@section Introduction

This chapter is intended to provide an introduction to the
procedure for writing RTEMS network device drivers.
The example code is taken from the `Generic 68360' network device
driver.  The source code for this driver is located in the
@code{c/src/lib/libbsp/m68k/gen68360/network} directory in the RTEMS
source code distribution.  Having a copy of this driver at
hand when reading the following notes will help significantly.

@section Learn about the network device 

Before starting to write the network driver become completely
familiar with the programmer's view of the device.
The following points list some of the details of the
device that must be understood before a driver can be written.

@itemize @bullet

@item Does the device use DMA to transfer packets to and from
memory or does the processor have to
copy packets to and from memory on the device?

@item If the device uses DMA, is it capable of forming a single
outgoing packet from multiple fragments scattered in separate
memory buffers?

@item If the device uses DMA, is it capable of chaining multiple
outgoing packets, or does each outgoing packet require
intervention by the driver?

@item Does the device automatically pad short frames to the minimum
64 bytes or does the driver have to supply the padding?

@item Does the device automatically retry a transmission on detection
of a collision?

@item If the device uses DMA, is it capable of buffering multiple
packets to memory, or does the receiver have to be restarted
after the arrival of each packet?

@item How are packets that are too short, too long, or received with
CRC errors handled?  Does the device automatically continue
reception or does the driver have to intervene?

@item How is the device Ethernet address set?  How is the device
programmed to accept or reject broadcast and multicast packets?

@item What interrupts does the device generate?  Does it generate an
interrupt for each incoming packet, or only for packets received
without error?  Does it generate an interrupt for each packet
transmitted, or only when the transmit queue is empty?  What
happens when a transmit error is detected?

@end itemize

In addition, some controllers have specific questions regarding 
board specific configuration.  For example, the SONIC Ethernet
controller has a very configurable data bus interface.  It can
even be configured for sixteen and thirty-two bit data buses.  This
type of information should be obtained from the board vendor.

@section Understand the network scheduling conventions

When writing code for the driver transmit and receive tasks,
take care to follow the network scheduling conventions.  All tasks
which are associated with networking share various
data structures and resources.  To ensure the consistency
of these structures the tasks
execute only when they hold the network semaphore (@code{rtems_bsdnet_semaphore}).
The transmit and receive tasks must abide by this protocol.  Be very
careful to avoid `deadly embraces' with the other network tasks.
A number of routines are provided to make it easier for the network
driver code to conform to the network task scheduling conventions.

@itemize @bullet

@item @code{void rtems_bsdnet_semaphore_release(void)}

This function releases the network semaphore.
The network driver tasks must call this function immediately before
making any blocking RTEMS request.

@item @code{void rtems_bsdnet_semaphore_obtain(void)}

This function obtains the network semaphore.
If a network driver task has released the network semaphore to allow other
network-related tasks to run while the task blocks, then this function must 
be called to reobtain the semaphore immediately after the return from the
blocking RTEMS request.

@item @code{rtems_bsdnet_event_receive(rtems_event_set, rtems_option, rtems_interval, rtems_event_set *)}
The network driver task should call this function when it wishes to wait
for an event.  This function releases the network semaphore,
calls @code{rtems_event_receive} to wait for the specified event
or events and reobtains the semaphore.
The value returned is the value returned by the @code{rtems_event_receive}.

@end itemize

@section Network Driver Makefile

Network drivers are considered part of the BSD network package and as such
are to be compiled with the appropriate flags.  This can be accomplished by 
adding @code{-D__INSIDE_RTEMS_BSD_TCPIP_STACK__} to the @code{command line}.
If the driver is inside the RTEMS source tree or is built using the
RTEMS application Makefiles, then adding the following line accomplishes
this:

@example
DEFINES += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
@end example

This is equivalent to the following list of definitions.  Early versions
of the RTEMS BSD network stack required that all of these be defined.

@example
-D_COMPILING_BSD_KERNEL_ -DKERNEL -DINET -DNFS \
    -DDIAGNOSTIC -DBOOTP_COMPAT
@end example

Defining these macros tells the network header files that the driver
is to be compiled with extended visibility into the network stack.  This
is in sharp contrast to applications that simply use the network stack.
Applications do not require this level of visibility and should stick
to the portable application level API.

As a direct result of being logically internal to the network stack,
network drivers use the BSD memory allocation routines   This means,
for example, that malloc takes three arguments.  See the SONIC
device driver (@code{c/src/lib/libchip/network/sonic.c}) for an example
of this.  Because of this, network drivers should not include 
@code{<stdlib.h>}.  Doing so will result in conflicting definitions
of @code{malloc()}.

@b{Application level} code including network servers such as the FTP
daemon are @b{not} part of the BSD kernel network code and should not be
compiled with the BSD network flags.  They should include
@code{<stdlib.h>} and not define the network stack visibility
macros.

@section Write the Driver Attach Function

The driver attach function is responsible for configuring the driver
and making the connection between the network stack
and the driver.

Driver attach functions take a pointer to an
@code{rtems_bsdnet_ifconfig} structure as their only argument.
and set the driver parameters based on the
values in this structure.  If an entry in the configuration
structure is zero the attach function chooses an
appropriate default value for that parameter.


The driver should then set up several fields in the ifnet structure
in the device-dependent data structure supplied and maintained by the driver:

@table @code
@item ifp->if_softc 
Pointer to the device-dependent data.  The first entry
in the device-dependent data structure must be an @code{arpcom}
structure.

@item ifp->if_name
The name of the device.  The network stack uses this string
and the device number for device name lookups.  The device name should
be obtained from the @code{name} entry in the configuration structure.

@item ifp->if_unit
The device number.  The network stack uses this number and the
device name for device name lookups.  For example, if
@code{ifp->if_name} is @samp{scc} and @code{ifp->if_unit} is @samp{1},
the full device name would be @samp{scc1}.  The unit number should be
obtained from the `name' entry in the configuration structure.

@item ifp->if_mtu
The maximum transmission unit for the device.  For Ethernet
devices this value should almost always be 1500.

@item ifp->if_flags
The device flags.  Ethernet devices should set the flags
to @code{IFF_BROADCAST|IFF_SIMPLEX}, indicating that the
device can broadcast packets to multiple destinations
and does not receive and transmit at the same time.

@item ifp->if_snd.ifq_maxlen
The maximum length of the queue of packets waiting to be
sent to the driver.  This is normally set to @code{ifqmaxlen}.

@item ifp->if_init
The address of the driver initialization function.

@item ifp->if_start
The address of the driver start function.

@item ifp->if_ioctl
The address of the driver ioctl function.

@item ifp->if_output
The address of the output function.  Ethernet devices
should set this to @code{ether_output}.
@end table

RTEMS provides a function to parse the driver name in the
configuration structure into a device name and unit number.

@example
int rtems_bsdnet_parse_driver_name (
  const struct rtems_bsdnet_ifconfig *config,
  char **namep
);
@end example

The function takes two arguments; a pointer to the configuration
structure and a pointer to a pointer to a character.  The function
parses the configuration name entry, allocates memory for the driver
name, places the driver name in this memory, sets the second argument
to point to the name and returns the unit number. 
On error, a message is printed and -1 is returned.

Once the attach function  has set up the above entries it must link the
driver data structure onto the list of devices by
calling @code{if_attach}.  Ethernet devices should then
call @code{ether_ifattach}.  Both functions take a pointer to the
device's @code{ifnet} structure as their only argument.

The attach function should return a non-zero value to indicate that
the driver has been successfully configured and attached.

@section Write the Driver Start Function.
This function is called each time the network stack wants to start the
transmitter.  This occures whenever the network stack adds a packet
to a device's send queue and the @code{IFF_OACTIVE} bit in the
device's @code{if_flags} is not set.

For many devices this function need only set the @code{IFF_OACTIVE} bit in the
@code{if_flags} and send an event to the transmit task
indicating that a packet is in the driver transmit queue.


@section Write the Driver Initialization Function.

This function should initialize the device, attach to interrupt handler, 
and start the driver transmit and receive tasks.  The function

@example
rtems_id
rtems_bsdnet_newproc (char *name,
		      int stacksize,
		      void(*entry)(void *),
		      void *arg);
@end example

should be used to start the driver tasks.

Note that the network stack may call the driver initialization function more
than once.
Make sure multiple versions of the receive and transmit tasks are not accidentally
started.



@section Write the Driver Transmit Task

This task is reponsible for removing packets from the driver send queue and sending them to the device.  The task should block waiting for an event from the
driver start function indicating that packets are waiting to be transmitted.
When the transmit task has drained the driver send queue the task should clear
the @code{IFF_OACTIVE} bit in @code{if_flags} and block until another outgoing
packet is queued.


@section Write the Driver Receive Task
This task should block until a packet arrives from the device.  If the
device is an Ethernet interface the function @code{ether_input} should be called
to forward the packet to the network stack.   The arguments to @code{ether_input}
are a pointer to the interface data structure, a pointer to the ethernet
header and a pointer to an mbuf containing the packet itself.




@section Write the Driver Interrupt Handler
A typical interrupt handler will do nothing more than the hardware
manipulation required to acknowledge the interrupt and send an RTEMS event
to wake up the driver receive or transmit task waiting for the event.
Network interface interrupt handlers must not make any calls to other
network routines.



@section Write the Driver IOCTL Function
This function handles ioctl requests directed at the device.  The ioctl
commands which must be handled are:

@table @code
@item SIOCGIFADDR
@item SIOCSIFADDR
If the device is an Ethernet interface these
commands should be passed on to @code{ether_ioctl}.

@item SIOCSIFFLAGS
This command should be used to start or stop the device,
depending on the state of the interface @code{IFF_UP} and
@code{IFF_RUNNING} bits in @code{if_flags}:
@table @code
@item IFF_RUNNING
Stop the device.

@item IFF_UP
Start the device.

@item IFF_UP|IFF_RUNNING
Stop then start the device.

@item 0
Do nothing.

@end table
@end table



@section Write the Driver Statistic-Printing Function
This function should print the values of any statistic/diagnostic
counters the network driver may use.  The driver ioctl function should call
the statistic-printing function when the ioctl command is
@code{SIO_RTEMS_SHOW_STATS}.


