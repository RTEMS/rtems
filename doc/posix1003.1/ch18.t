@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Thread Cancellation

@section Thread Cancellation Overview

@subsection Cancelability States

@subsection Cancellation Points

@subsection Thread Cancellation Cleanup Handlers

@subsection Async-Cancel Safety

@section Thread Cancellation Functions

@subsection Canceling Execution of a Thread

@example
pthread_cancel()
@end example

@subsection Setting Cancelability State

@example
pthread_setcancelstate()
pthread_setcanceltype()
pthread_testcancel()
@end example

@subsection Establishing Cancellation Handlers

@example
pthread_cleanup_push()
pthread_cleanup_pop()
@end example

@section Language-Independent Cancellation Functionality

@subsection Requesting Cancellation

@subsection Associating Cleanup Code With Scopes

@subsection Controlling Cancellation Within Scopes

@subsection Defined Cancellation Sequence

@subsection List of Cancellation Points

