@c
@c  Written by Eric Norum
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Network Task Structure and Data Flow

A schematic diagram of the tasks and message @b{mbuf} queues in a
simple RTEMS networking application is shown in the following
figure:

@ifset use-ascii
@example
@group
NO ASCII VERSION OF THE TASKING FIGURE IS AVAILABLE
@end group
@end example
@end ifset

@ifset use-tex
@image{networkflow,5in,6in}
@end ifset

@ifset use-html
@html
<IMG SRC="networkflow.jpg" ALIGN=CENTER 
     ALT="Network Task Structure and Data Flow">
@end html
@end ifset


The transmit task  for each network interface is normally blocked waiting
for a packet to arrive in the transmit queue.  Once a packet arrives, the
transmit task may block waiting for an event from the transmit interrupt
handler.  The transmit interrupt handler sends an RTEMS event to the transmit
task to indicate that transmit hardware resources have become available.

The receive task for each network interface is normally blocked waiting
for an event from the receive interrupt handler.  When this event is received
the receive task reads the packet and forwards it to the network stack
for subsequent processing by the network task.

The network task processes incoming packets and takes care of
timed operations such as handling TCP timeouts and
aging and removing routing table entries.

The `Network code' contains routines which may run in the context of
the user application tasks, the interface receive task or the network task.
A network semaphore ensures that
the data structures manipulated by the network code remain consistent.
