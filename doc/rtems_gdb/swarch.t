@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Software Architecture

RTEMS/GDB host-target debugger is architectured around:

@itemize @bullet
@item One debugger, running on the host.
@item One debug server, running on the target.
@end itemize

The host debugger communicates with the debug server using Sun RPC
over UDP/IP (Ethernet).

When opening a debugging session, the user takes the control over
the entire target system. The target system is viewed as a single multi-threaded
process.

The debugger can observe the state of the system objects, without
stopping the entire system. However, breakpoints and stepping affect (stop)
the entire system.

For a given target, only one single system debugging session is possible
at a time.

