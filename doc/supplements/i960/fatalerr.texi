@c
@c  COPYRIGHT (c) 1988-1997.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c

@ifinfo
@node Default Fatal Error Processing, Default Fatal Error Processing Introduction, Interrupt Processing Interrupt Stack, Top
@end ifinfo
@chapter Default Fatal Error Processing
@ifinfo
@menu
* Default Fatal Error Processing Introduction::
* Default Fatal Error Processing Default Fatal Error Handler Operations::
@end menu
@end ifinfo

@ifinfo
@node Default Fatal Error Processing Introduction, Default Fatal Error Processing Default Fatal Error Handler Operations, Default Fatal Error Processing, Default Fatal Error Processing
@end ifinfo
@section Introduction

Upon detection of a fatal error by either the
application or RTEMS the fatal error manager is invoked.  The
fatal error manager will invoke the user-supplied fatal error
handlers.  If no user-supplied handlers is configured,  the
RTEMS provided default fatal error handler is invoked.  If the
user-supplied fatal error handlers return to the executive the
default fatal error handler is then invoked.  This chapter
describes the precise operations of the default fatal error
handler.

@ifinfo
@node Default Fatal Error Processing Default Fatal Error Handler Operations, Board Support Packages, Default Fatal Error Processing Introduction, Default Fatal Error Processing
@end ifinfo
@section Default Fatal Error Handler Operations

The default fatal error handler which is invoked by
the fatal_error_occurred directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler disables processor interrupts to
level 31, places the error code in G0, and executes a branch to
self instruction to simulate a halt processor instruction.
