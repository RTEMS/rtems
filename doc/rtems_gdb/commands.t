@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@section Commands

The usual GDB commands are available to display or modify data structures and
machine registers, disassemble code, display source listings, set and reset
breakpoints, control the debuggee execution, etc. Refer to Debugging with GDB
for a complete documentation of the standard GDB commands.

GDB 4.17 has been extended with a number of commands specifically
for multi-threaded debugging within a RTEMS host-target environment. These commands
are described in this section.


@subsection Host-target connection control

@itemize
@item (gdb) setrpcmode sun : specifies the SUN RPC type.
@item (gdb) setdaemontype rdbg : specifies the RDBG debug server type.
@item (gdb) target rtems target-name : specifies the target type.
@end itemize

@subsection System status information

@itemize
@item (gdb) info threads : gives a list of all threads in the debuggee,
their names, identifiers and states. It also indicates which thread is
the target thread.
@end itemize


@subsection Thread control

@itemize
@item (gdb) thread target [id] : allows the user to get/set the target thread.
The argument id is either the thread's symbolic name or its local identifier.
@item (gdb) thread detach [id...] : all stops (breakpoints) of some specified
thread(s) can be ignored using the thread detach command. Each time a detached
thread hits a breakpoint, RTEMS/GDB will automatically restart the debuggee
without passing control to the user.
@item (gdb) thread attach [id...] : (re)attach specified thread(s).
@end itemize



