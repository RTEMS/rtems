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
fatal error manager will invoke a user-supplied fatal error
handler.  If no user-supplied handler is configured,  the RTEMS
provided default fatal error handler is invoked.  If the
user-supplied fatal error handler returns to the executive the
default fatal error handler is then invoked.  This chapter
describes the precise operations of the default fatal error
handler.

@section Default Fatal Error Handler Operations

The default fatal error handler which is invoked by
the fatal_error_occurred directive when there is no user handler
configured or the user handler returns control to RTEMS.  The
default fatal error handler disables processor interrupts (i.e.
sets the I bit in the PSW register to 0), places the error code
in r1, and executes a break instruction to simulate a halt
processor instruction.

