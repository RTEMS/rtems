@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Default Fatal Error Processing

@section Introduction

Upon detection of a fatal error by either the
application or RTEMS the fatal error manager is invoked.  The
fatal error manager will invoke the user-supplied fatal error
handlers.  If no user-supplied handlers are configured,  the
RTEMS provided default fatal error handler is invoked.  If the
user-supplied fatal error handlers return to the executive the
default fatal error handler is then invoked.  This chapter
describes the precise operations of the default fatal error
handler.

@section Default Fatal Error Handler Operations

The default fatal error handler which is invoked by
the @code{rtems_fatal_error_occurred} directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler performs the following actions:

@itemize @bullet

@item places the error code in r3, and

@item executes a trap instruction which results in a Program Exception.

@end itemize

If the Program Exception returns, then the following actions are performed:

@itemize @bullet

@item disables all processor exceptions by loading a 0 into the MSR, and

@item goes into an infinite loop to simulate a halt processor instruction.

@end itemize

