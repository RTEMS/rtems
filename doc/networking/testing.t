@c
@c  Written by Eric Norum
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Testing the Driver

@section Preliminary Setup

The network used to test the driver should include at least:

@itemize @bullet

@item The hardware on which the driver is to run.
It makes testing much easier if you can run a debugger to control
the operation of the target machine.

@item An Ethernet network analyzer or a workstation with an
`Ethernet snoop' program such as @code{ethersnoop} or
@code{tcpdump}.

@item A workstation.

@end itemize

During early debug, you should consider putting the target, workstation, 
and snooper on a small network by themselves.  This offers a few 
advantages:

@itemize @bullet

@item There is less traffic to look at on the snooper and for the target
to process while bringing the driver up.

@item Any serious errors will impact only your small network not a building
or campus network.  You want to avoid causing any unnecessary problems.

@item Test traffic is easier to repeatably generate.

@item Performance measurements are not impacted by other systems on
the network.

@end itemize

@section Debug Output 

There are a number of sources of debug output that can be enabled 
to aid in tracing the behavior of the network stack.  The following
is a list of them:

@itemize @bullet

@item mbuf activity

There are commented out calls to @code{printf} in the file 
@code{sys/mbuf.h} in the network stack code.  Uncommenting
these lines results in output when mbuf's are allocated
and freed.  This is very useful for finding memory leaks.

@item TX and RX queuing

There are commented out calls to @code{printf} in the file 
@code{net/if.h} in the network stack code.  Uncommenting
these lines results in output when packets are placed
on or removed from one of the transmit or receive packet
queues.  These queues can be viewed as the boundary line
between a device driver and the network stack.  If the 
network stack is enqueuing packets to be transmitted that
the device driver is not dequeuing, then that is indicative
of a problem in the transmit side of the device driver.
Conversely, if the device driver is enqueueing packets
as it receives them (via a call to @code{ether_input}) and
they are not being dequeued by the network stack,
then there is a problem.  This situation would likely indicate
that the network server task is not running.

@item TCP state transitions

In the unlikely event that one would actually want to see 
TCP state transitions, the @code{TCPDEBUG} macro can be defined
in the file @code{opt_tcpdebug.h}.  This results in the routine
@code{tcp_trace()} being called by the network stack and
the state transitions logged into the @code{tcp_debug} data
structure.  If the variable @code{tcpconsdebug} in the file
@code{netinet/tcp_debug.c} is set to 1, then the state transitions
will also be printed to the console.

@end itemize

@section Driver basic operation

The network demonstration program @code{netdemo} may be used for these tests.

@itemize @bullet

@item Edit @code{networkconfig.h} to reflect the values for your network.

@item Start with @code{RTEMS_USE_BOOTP} not defined.

@item Edit @code{networkconfig.h} to configure the driver
with an
explicit Ethernet and Internet address and with reception of
broadcast packets disabled:

Verify that the program continues to run once the driver has been attached.

@item Issue a @samp{u} command to send UDP
packets to the `discard' port.
Verify that the packets appear on the network.

@item Issue a @samp{s} command to print the network and driver statistics.

@item On a workstation, add a static route to the target system.

@item On that same workstation try to `ping' the target system.
Verify that the ICMP echo request and reply packets appear on the net.

@item Remove the static route to the target system.
Modify @code{networkconfig.h} to attach the driver
with reception of broadcast packets enabled.
Try to `ping' the target system again.
Verify that ARP request/reply and ICMP echo request/reply packets appear
on the net.

@item Issue a @samp{t} command to send TCP
packets to the `discard' port.
Verify that the packets appear on the network.

@item Issue a @samp{s} command to print the network and driver statistics.

@item Verify that you can telnet to ports 24742
and 24743 on the target system from one or more
workstations on your network.

@end itemize

@section BOOTP/DHCP operation

Set up a BOOTP/DHCP server on the network.
Set define @code{RTEMS USE_BOOT} in @code{networkconfig.h}.
Run the @code{netdemo} test program.
Verify that the target system configures itself from the BOOTP/DHCP server and
that all the above tests succeed.

@section Stress Tests

Once the driver passes the tests described in the previous section it should
be subjected to conditions which exercise it more
thoroughly and which test its error handling routines.

@subsection Giant packets

@itemize @bullet
@item Recompile the driver with @code{MAXIMUM_FRAME_SIZE} set to
a smaller value, say 514.

@item `Ping' the driver from another workstation and verify
that frames larger than 514 bytes are correctly rejected.

@item Recompile the driver with @code{MAXIMUM_FRAME_SIZE} restored  to 1518.
@end itemize

@subsection Resource Exhaustion

@itemize @bullet
@item Edit  @code{networkconfig.h}
so that the driver is configured with just two receive and transmit descriptors.

@item Compile and run the @code{netdemo} program.

@item Verify that the program operates properly and that you can
still telnet to both the ports.

@item Display the driver statistics (Console `@code{s}' command or telnet
`control-G' character) and verify that:

@enumerate

@item The number of transmit interrupts is non-zero.
This indicates that all transmit descriptors have been in use at some time.

@item The number of missed packets is non-zero.
This indicates that all receive descriptors have been in use at some time.

@end enumerate

@end itemize

@subsection Cable Faults

@itemize @bullet
@item Run the @code{netdemo} program.

@item Issue a `@code{u}' console command to make the target machine transmit
a bunch of UDP packets.

@item While the packets are being transmitted, disconnect and reconnect the
network cable.

@item Display the network statistics and verify that the driver has
detected the loss of carrier.

@item Verify that you can still telnet to both ports on the target machine.

@end itemize

@subsection Throughput

Run the @code{ttcp} network benchmark program.
Transfer large amounts of data (100's of megabytes) to and from the target
system.

The procedure for testing throughput from a host to an RTEMS target
is as follows:

@enumerate
@item Download and start the ttcp program on the Target.

@item In response to the @code{ttcp} prompt, enter @code{-s -r}.  The
meaning of these flags is described in the @code{ttcp.1} manual page
found in the @code{ttcp_orig} subdirectory.

@item On the host run @code{ttcp -s -t <<insert the hostname or IP address of
the Target here>>}

@end enumerate


The procedure for testing throughput from an RTEMS target
to a Host is as follows:

@enumerate
@item On the host run @code{ttcp -s -r}.

@item Download and start the ttcp program on the Target.

@item In response to the @code{ttcp} prompt, enter @code{-s -t <<insert
the hostname or IP address of the Target here>>}.  You need to type the
IP address of the host unless your Target is talking to your Domain Name
Server.

@end enumerate

To change the number of buffers, the buffer size, etc. you just add the
extra flags to the @code{-t} machine as specified in the @code{ttcp.1}
manual page found in the @code{ttcp_orig} subdirectory.





